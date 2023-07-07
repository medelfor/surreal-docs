/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/GenerateCommand.h"
#include "udocs-processor/cli/commands/generate/BpPreparer.h"
#include "udocs-processor/StringHelper.h"

udocs_processor::GenerateCommand::GenerateCommand(
    std::shared_ptr<spdlog::sinks::sink> Sink, std::unique_ptr<BpPreparer> Bp,
    std::unique_ptr<CppPreparer> Cpp, std::shared_ptr<ProjectService> Project,
    std::unique_ptr<Generator> Gen)
    : Bp(std::move(Bp)), Cpp(std::move(Cpp)), Gen(std::move(Gen)),
    Project(Project) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }

  Formats.insert(std::make_pair(GenerateRequest::ExportFormat::HTML,
      Generator::GenerationRequest::ExportFormat::HTML));
  Formats.insert(std::make_pair(GenerateRequest::ExportFormat::DEPLOYMENT,
      Generator::GenerationRequest::ExportFormat::DEPLOYMENT));

  BpStatusToStatus.insert(std::make_pair(BpPreparer::Status::EXTRACTING_INFO,
      Status::EXTRACTING_FROM_BP));
  BpStatusToStatus.insert(std::make_pair(BpPreparer::Status::COMPILING,
      Status::COMPILING));

  GenStatusToStatus.insert(std::make_pair(Generator::Status::LOADING,
      Status::LOADING));
  GenStatusToStatus.insert(std::make_pair(Generator::Status::PROCESSING,
      Status::PROCESSING));
  GenStatusToStatus.insert(std::make_pair(
      Generator::Status::SERIALIZING_INTERNAL, Status::SERIALIZING_INTERNAL));
  GenStatusToStatus.insert(std::make_pair(Generator::Status::SERIALIZING_IMAGES,
      Status::SERIALIZING_IMAGES));
  GenStatusToStatus.insert(std::make_pair(Generator::Status::SERIALIZING_HTML,
      Status::SERIALIZING_HTML));
  GenStatusToStatus.insert(std::make_pair(Generator::Status::FINALIZING,
      Status::FINALIZING));
  GenStatusToStatus.insert(std::make_pair(Generator::Status::CLEANING_UP,
      Status::CLEANING_UP));
  GenStatusToStatus.insert(std::make_pair(Generator::Status::FINISHED,
      Status::CLEANING_UP));  // not a mistake
  GenStatusToStatus.insert(std::make_pair(Generator::Status::ERROR,
      Status::ERROR));
  GenStatusToStatus.insert(std::make_pair(Generator::Status::DEPLOYING,
      Status::DEPLOYING));
}

/*
 * 1. run bp prepare
 * 2. run cpp prepare
 * 3. run generator
 */
void udocs_processor::GenerateCommand::Generate(
    const GenerateRequest &Request, const CallbackType& StatusCallback) const {
  Generator::GenerationRequest GenRequest;
  if (Request.DoDeploy) {
    StatusCallback(Status::PREDEPLOYING, "");
    PerformDeployPreCheck(Request.Organization, Request.Project, Request.Token,
        Request.Version, GenRequest);
  }

  if (Request.DoCleanOut) {
    StatusCallback(Status::PURGING_OUT_DIR, "");

    if (std::filesystem::exists(Request.OutDirectory) &&
        std::filesystem::exists(Request.ProjectDirectory) &&
        std::filesystem::canonical(Request.OutDirectory) ==
        std::filesystem::canonical(Request.ProjectDirectory)) {
      throw std::domain_error{"Refusing to purge the project directory"};
    }

    std::string ProjectDir = ConvertToAbsolutePath(Request.OutDirectory);
    l->warn("Purging the out directory ({})...", ProjectDir);
    std::filesystem::remove_all(ProjectDir);
  }

  BpPreparer::PrepareRequest BpRequest;
  BpRequest.Version = {
      "", ConvertToAbsolutePath(Request.EngineRoot)
  };
  BpRequest.ProjectDirectory =
      ConvertToAbsolutePath(Request.ProjectDirectory);
  BpRequest.OutDirectory = ConvertToAbsolutePath(Request.OutDirectory);
  BpRequest.ContentInclude = Request.ContentInclude;
  BpRequest.ContentExclude = Request.ContentExclude;
  BpRequest.NativeInclude = Request.NativeInclude;
  BpRequest.NativeExclude = Request.NativeExclude;
  BpRequest.Configuration = Request.Configuration;
  BpRequest.Target = Request.Target;
  BpRequest.DoUseCmd = Request.DoUseCmd;
  Bp->Prepare(BpRequest, [this, &StatusCallback](BpPreparer::Status Status) {
    auto It = BpStatusToStatus.find(Status);
    if (It != BpStatusToStatus.end()) {
      StatusCallback(It->second, "");
    }
  });

  if (!Request.CppInputs.empty()) {
    StatusCallback(Status::EXTRACTING_FROM_CPP, "");
    CppPreparer::PrepareRequest CppRequest;
    CppRequest.OutDirectory = BpRequest.OutDirectory;
    CppRequest.Inputs = Request.CppInputs;
    CppRequest.Exclude = Request.CppExclude;
    Cpp->Prepare(CppRequest);
  }

  GenRequest.DoExportPrivate = Request.DoExportPrivate;
  GenRequest.OutDirectory = BpRequest.OutDirectory;
  GenRequest.InputDocs = ConvertToAbsolutePath(Request.SurDocsDirectory);
  GenRequest.InputUData = GenRequest.OutDirectory + UDATA_AT;
  GenRequest.InputDoxy = GenRequest.OutDirectory + DOXY_JSON_AT;
  auto It = Formats.find(Request.Format);
  if (It != Formats.end()) {
    GenRequest.Format = It->second;
  } else {
    l->error("Invalid export format {}", static_cast<uint32_t>(Request.Format));
    throw std::invalid_argument{"Invalid format"};
  }
  GenRequest.DoDeploy = Request.DoDeploy;
  GenRequest.Organization = Request.Organization;
  GenRequest.Project = Request.Project;
  GenRequest.Token = Request.Token;
  GenRequest.Version = Request.Version;
  Gen->Generate(GenRequest,
    [this, &StatusCallback]
    (Generator::Status Status, const std::string& Message) {
      auto It = GenStatusToStatus.find(Status);
      if (It != GenStatusToStatus.end()) {
        StatusCallback(It->second, Message);
      }
    });
}

void udocs_processor::GenerateCommand::CleanUp(
    const std::string& OutDirectory) const {
  std::string AbsPath = ConvertToAbsolutePath(OutDirectory);
  Bp->CleanUp(OutDirectory);
  Cpp->CleanUp(OutDirectory);
  Gen->CleanUp(OutDirectory);
}

std::string udocs_processor::GenerateCommand::ConvertToAbsolutePath(
    const std::string& Path) const {
  return StringHelper::Normalize(std::filesystem::absolute(
      std::filesystem::path{Path}).string());
}

void udocs_processor::GenerateCommand::PerformDeployPreCheck(
    const std::string &Organization, const std::string &Project,
    const std::string &Token, const std::string &Version,
    Generator::GenerationRequest& GenRequest) const {
  if (Version.empty()) {
    throw std::invalid_argument{"The version cannot be empty"};
  }

  ProjectService::DeployPreCheckRequest Request;
  Request.Version = Version;
  Request.Location = {Project, Organization};
  Request.Token = Token;

  ProjectService::DeployPreCheckResponse Response =
      this->Project->PerformDeployPreCheck(Request);
  if (Response.Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("Perform deploy pre check: {}/{}", Response.Status.GetCode(),
        Response.Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Deployment Pre Check: {}",
        Response.Status.GetMessageDescription())};
  }

  GenRequest.DoesVersionExist = Response.DoesVersionExist;
}
