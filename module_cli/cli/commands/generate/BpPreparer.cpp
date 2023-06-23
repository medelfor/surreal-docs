/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/commands/generate/BpPreparer.h"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>
#include "udocs-processor/StringHelper.h"
#include "udocs-processor/cli/ue/UnrealInteraction.h"

udocs_processor::BpPreparer::BpPreparer(
    std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<UnrealInteraction> Ue) : Ue(std::move(Ue)) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}

void udocs_processor::BpPreparer::SetResDirectory(
    const std::string& ResDirectory) {
  this->ResDirectory = StringHelper::Normalize(ResDirectory);
}

void udocs_processor::BpPreparer::CleanUp(
    const std::string &OutDirectory) const {
  std::filesystem::remove_all({MakeBpWd(OutDirectory)});
}

std::string udocs_processor::BpPreparer::MakeBpWd(
    const std::string& OutDirectory) const {
  return StringHelper::Normalize(OutDirectory) + BP_WD_NAME +
      DIRECTORY_SEPARATOR;
}

void udocs_processor::BpPreparer::Prepare(
    const udocs_processor::BpPreparer::PrepareRequest &Request,
    const std::function<void(Status)>& StageCallback) {
  // 0. create bp wd
  // 1. run build
  // 2. prepare UDataRequest
  // 3. run the commandlet

  std::string BpWdPath = MakeBpWd(Request.OutDirectory);
  std::filesystem::path BpWd{BpWdPath};
  std::filesystem::create_directories(BpWd);

  StageCallback(Status::COMPILING);
  Ue->Build(Request.Target, Request.Configuration,
      Request.ProjectDirectory, Request.Version);

  StageCallback(Status::EXTRACTING_INFO);
  nlohmann::json Proto = nlohmann::json::parse(std::ifstream(
      ResDirectory + PROTO_UDATA_REQUEST));
  static nlohmann::json_pointer<std::string> ContentInclude{
      PROTO_UDATA_CONTENT_INCLUDE_PATH};
  static nlohmann::json_pointer<std::string> ContentExclude{
      PROTO_UDATA_CONTENT_EXCLUDE_PATH};
  static nlohmann::json_pointer<std::string> NativeInclude{
      PROTO_UDATA_NATIVE_INCLUDE_PATH};
  static nlohmann::json_pointer<std::string> NativeExclude{
      PROTO_UDATA_NATIVE_EXCLUDE_PATH};
  static nlohmann::json_pointer<std::string> Output{
      PROTO_UDATA_OUTPUT_PATH};

  Proto[ContentInclude] = Request.ContentInclude;
  Proto[ContentExclude] = Request.ContentExclude;
  Proto[NativeInclude] = Request.NativeInclude;
  Proto[NativeExclude] = Request.NativeExclude;
  Proto[Output] = std::filesystem::absolute(BpWd).string();

  std::string OutputRequestPath = std::filesystem::absolute(BpWd).string() +
      DIRECTORY_SEPARATOR + REQUEST;
  {
    std::ofstream OutputProto{OutputRequestPath};
    if (!OutputProto.good()) {
      l->error("Failed to open request file");
      throw std::runtime_error{"Unable to open request file"};
    }
    OutputProto << Proto.dump();
    if (!OutputProto.good()) {
      l->error("Failed to write request file");
      throw std::runtime_error{"Unable to write request file"};
    }
  }

  Ue->Run(Request.ProjectDirectory, Request.Version, {
    EDITOR_COMMANDLET, EDITOR_REQUEST_FILE + OutputRequestPath
  }, Request.DoUseCmd);
}
