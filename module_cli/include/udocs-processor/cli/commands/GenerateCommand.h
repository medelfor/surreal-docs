/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "udocs-processor/cli/commands/generate/CppPreparer.h"
#include "udocs-processor/cli/commands/generate/BpPreparer.h"
#include "udocs-processor/cli/commands/generate/Generator.h"
#include "udocs-processor/services/ProjectService.h"

namespace udocs_processor {
class GenerateCommand {
 public:
  struct GenerateRequest {
    enum class ExportFormat {
      HTML,
      DEPLOYMENT
    };
    std::string OutDirectory;
    std::string ProjectDirectory;
    std::string SurDocsDirectory;
    bool DoExportPrivate = true;
    ExportFormat Format = ExportFormat::HTML;
    std::vector<std::string> ContentInclude;
    std::vector<std::string> ContentExclude;
    std::vector<std::string> NativeInclude;
    std::vector<std::string> NativeExclude;
    std::vector<std::string> CppInputs;
    std::vector<std::string> CppExclude;
    std::string EngineRoot;
    std::string Target;
    std::string Configuration;
    bool DoUseCmd = false;
    bool DoCleanOut = false;
    bool DoDeploy = false;
    std::string Organization;
    std::string Project;
    std::string Token;
    std::string Version;
  };

  enum class Status {
    PREDEPLOYING,
    PURGING_OUT_DIR,
    COMPILING,
    EXTRACTING_FROM_BP,
    EXTRACTING_FROM_CPP,
    LOADING,
    PROCESSING,
    SERIALIZING_INTERNAL,
    SERIALIZING_IMAGES,
    DEPLOYING,
    SERIALIZING_HTML,
    FINALIZING,
    CLEANING_UP,
    FINISHED,
    ERROR
  };

  using CallbackType = std::function<void(Status, const std::string&)>;

  GenerateCommand(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<BpPreparer> Bp, std::unique_ptr<CppPreparer> Cpp,
      std::shared_ptr<ProjectService> Project, std::unique_ptr<Generator> Gen);

  void Generate(const GenerateRequest& Request,
      const CallbackType& StatusCallback) const;

  void CleanUp(const std::string& OutDirectory) const;

 private:
  std::string ConvertToAbsolutePath(const std::string& Path) const;

  static constexpr const char* LOGGER_NAME = "generate-preparer";
  static constexpr const char* UDATA_AT = "bp_wd/udata.json";
  static constexpr const char* DOXY_JSON_AT = "cpp_wd/json";

  std::shared_ptr<ProjectService> Project;
  std::unique_ptr<BpPreparer> Bp;
  std::unique_ptr<CppPreparer> Cpp;
  std::unique_ptr<Generator> Gen;
  std::shared_ptr<spdlog::logger> l;
  std::map<GenerateRequest::ExportFormat,
      Generator::GenerationRequest::ExportFormat> Formats;
  std::map<BpPreparer::Status, Status> BpStatusToStatus;
  std::map<Generator::Status, Status> GenStatusToStatus;

  void PerformDeployPreCheck(const std::string &Organization,
      const std::string &Project, const std::string &Token,
      const std::string &Version,
      Generator::GenerationRequest& GenRequest) const;
};
}  // namespace udocs_processor
