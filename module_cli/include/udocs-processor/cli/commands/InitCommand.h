/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "udocs-processor/cli/surreal/SurrealLoader.h"
#include "udocs-processor/cli/ue/UnrealInteraction.h"

namespace udocs_processor {
class InitCommand {
 public:
  InitCommand(std::unique_ptr<UnrealInteraction> Ue,
    std::shared_ptr<SurrealLoader> Loader);

  bool IsNewProject(const std::string& ProjectDir) const;

  std::unique_ptr<SurrealProject> CreateNewProject(
      const std::string& ProjectDir) const;

  std::unique_ptr<SurrealProject> LoadProject(
      const std::string& ProjectDir) const;

  void SaveProject(const SurrealProject& Project,
      const std::string& ProjectDir) const;

  void InitializeProject(const std::string& ProjectDir,
      bool IsNewProject) const;

  void SetInstallPath(std::string InstallPath);

  void SetResourcesPath(std::string ResourcesPath);

 private:
  void InstallPlugin(const std::string& ProjectDir) const;

  void InstallDefaultDocumentation(const std::string& ProjectDir) const;

  static constexpr const char UNREALISTIC_DOCS_PLUGIN[] = "UnrealisticDocs";
  static constexpr const char DEFAULT_CONFIGURATION[] = "Debug";
  static constexpr const char SURDOCS_DIRECTORY[] = "surdocs";
  static constexpr const char PLUGIN_DIRECTORY[] = "plugins/UnrealisticDocs";
  static constexpr const char TARGET_PLUGIN_DIRECTORY[] = "UnrealisticDocs";
  static constexpr const char DEFAULT_DOCS_DIRECTORY[] =
      "misc/other/udocs_processor/default_docs";
  static constexpr const char SURREAL_JSON[] = "surreal.json";
  static constexpr char DIRECTORY_SEPARATOR = '/';

  std::string InstallPath;
  std::string ResourcesPath;

  std::unique_ptr<UnrealInteraction> Ue;
  std::shared_ptr<SurrealLoader> Loader;
};
}  // namespace udocs_processor
