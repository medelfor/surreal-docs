/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/cli/GenerateCLI.h"
#include <spdlog/spdlog.h>
#include <future> NOLINT()
#include <thread> NOLINT()
#include <chrono> NOLINT()
#include "udocs-processor/cli/cli/util/CliHelper.h"

bool udocs_processor::GenerateCLI::Generate(const Arguments &Args) const {
  Telemetry->ReportInit(TELEMETRY_COMMAND_NAME);

  GenerateView* SelectedView = Args.IsInteractive
      ? InteractiveView.get() : NonInteractiveView.get();

  SelectedView->Init();
  SelectedView->SetStatus(GenerateView::Status::PREPARING, "");

  bool Success = true;
  auto Future = std::async(std::launch::async,
    [this, SelectedView, &Success, &Args]() {
      std::string OutDirectory;
      bool Started = false;
      try {
        GenerateCommand::GenerateRequest Request = MakeRequest(Args);
        OutDirectory = Request.OutDirectory;
        Started = true;

        Command->Generate(Request,
          [this, SelectedView]
          (GenerateCommand::Status Status_, const std::string& Message) {
            if (Status_ == GenerateCommand::Status::ERROR) {
              l->error("Generate command reported error: {}, throwing...",
                  Message);
              Telemetry->ReportFail(TELEMETRY_COMMAND_NAME, Message);
              throw std::runtime_error{Message};
            }

            l->info("Status: {}", static_cast<uint32_t>(Status_));
            auto It = GenerateStatusToStatus.find(Status_);
            if (It != GenerateStatusToStatus.end()) {
              SelectedView->SetStatus(It->second, "");
            } else {
              l->warn("Unexpected status: {}", static_cast<uint32_t>(Status_));
            }
          });
      } catch (const std::exception& Exc) {
        Success = false;
        Telemetry->ReportFail(TELEMETRY_COMMAND_NAME, Exc.what());
        l->error("Exception in generate thread: {}", Exc.what());
        SelectedView->SetFinished(true);
        SelectedView->SetStatus(GenerateView::Status::ERROR,
            fmt::format("Unexpected error: {}", Exc.what()));
      }

      if (Started) {
        SelectedView->SetStatus(GenerateView::Status::CLEANING_UP, "");
        Command->CleanUp(OutDirectory);
      }

      if (Success) {
        SelectedView->SetFinished(true);
        SelectedView->SetStatus(GenerateView::Status::FINISHED, "");
      }
    });

  auto View = std::thread(
    [&, this]() {
      while (!SelectedView->DoExit()) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds{POLL_DURATION_MS});
        SelectedView->Tick();
      }
      SelectedView->Destroy();
    });
  SelectedView->Start();

  Future.wait();
  View.join();

  Telemetry->ReportFinish(TELEMETRY_COMMAND_NAME);

  return Success;
}

udocs_processor::GenerateCommand::GenerateRequest
    udocs_processor::GenerateCLI::MakeRequest(const Arguments& Args) const {
  std::string PathToSurrealJson = SURDOCS_DIRECTORY + (DIRECTORY_SEPARATOR +
      std::string(SURREAL_JSON));
  std::ifstream SurrealJson{PathToSurrealJson};
  if (!SurrealJson.good()) {
    l->error("Unable to open `{}`", PathToSurrealJson);
    throw std::runtime_error{fmt::format("Can't open `{}`", PathToSurrealJson)};
  }
  nlohmann::json Surreal = nlohmann::json::parse(SurrealJson);
  std::unique_ptr<SurrealProject> Project = Loader->Deserialize(Surreal);

  GenerateCommand::GenerateRequest Request;
  Request.OutDirectory = Args.OutDirectory;
  Request.Format = Args.DeployLocation
      ? GenerateCommand::GenerateRequest::ExportFormat::DEPLOYMENT
      : GenerateCommand::GenerateRequest::ExportFormat::HTML;
  Request.DoExportPrivate = Project->DoExportPrivate;
  Request.Target = std::move(Project->Target);
  Request.EngineRoot = std::move(Project->EngineRoot);
  Request.Configuration = std::move(Project->Configuration);
  Request.CppInputs = std::move(Project->CppInputs);
  Request.CppExclude = std::move(Project->CppExclude);
  Request.ContentInclude = std::move(Project->ContentInclude);
  Request.ContentExclude = std::move(Project->ContentExclude);
  Request.NativeInclude = std::move(Project->NativeInclude);
  Request.NativeExclude = std::move(Project->NativeExclude);
  Request.SurDocsDirectory = SURDOCS_DIRECTORY;
  Request.ProjectDirectory = CURRENT_DIRECTORY;
  Request.DoUseCmd = Args.IsInteractive;
  Request.Version = Project->Version;
  Request.DoCleanOut = Args.DoCleanOut;

  if (Args.DeployLocation) {
    CliHelper::Location Location = CliHelper::ParseLocation(
        *Args.DeployLocation);
    Request.Project = Location.Project;
    Request.Organization = Location.Organization;
    Request.DoDeploy = true;
    Request.Token = Token->LoadToken(Args.Source);
  }

  return Request;
}

udocs_processor::GenerateCLI::GenerateCLI(
  std::shared_ptr<spdlog::sinks::sink> Sink,
  std::unique_ptr<GenerateCommand> Command,
  std::unique_ptr<GenerateView> InteractiveView,
  std::unique_ptr<GenerateView> NonInteractiveView,
  std::shared_ptr<SurrealLoader> Loader,
  std::shared_ptr<TokenLoader> Token,
  std::shared_ptr<BasicTelemetry> Telemetry)
    : Command(std::move(Command)),
    InteractiveView(std::move(InteractiveView)),
    NonInteractiveView(std::move(NonInteractiveView)),
    Loader(Loader), Telemetry(Telemetry), Token(Token) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }

  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::COMPILING, GenerateView::Status::COMPILING));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::PURGING_OUT_DIR,
      GenerateView::Status::PURGING_OUT));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::PREDEPLOYING, GenerateView::Status::PREDEPLOY));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::DEPLOYING, GenerateView::Status::DEPLOYING));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::EXTRACTING_FROM_BP,
      GenerateView::Status::EXTRACTING_FROM_BP));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::EXTRACTING_FROM_CPP,
      GenerateView::Status::EXTRACTING_FROM_CPP));
  GenerateStatusToStatus.insert(std::make_pair(GenerateCommand::Status::LOADING,
      GenerateView::Status::LOADING));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::PROCESSING, GenerateView::Status::PROCESSING));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::SERIALIZING_INTERNAL,
      GenerateView::Status::SERIALIZING_INTERNAL));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::SERIALIZING_IMAGES,
      GenerateView::Status::SERIALIZING_IMAGES));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::SERIALIZING_HTML,
      GenerateView::Status::SERIALIZING_HTML));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::DEPLOYING, GenerateView::Status::DEPLOYING));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::FINALIZING, GenerateView::Status::FINALIZING));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::CLEANING_UP, GenerateView::Status::CLEANING_UP));
  GenerateStatusToStatus.insert(std::make_pair(
      GenerateCommand::Status::FINISHED, GenerateView::Status::FINISHED));
  GenerateStatusToStatus.insert(std::make_pair(GenerateCommand::Status::ERROR,
      GenerateView::Status::ERROR));
}
