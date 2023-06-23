/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/commands/InitCommand.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include "udocs-processor/StringHelper.h"

std::unique_ptr<udocs_processor::SurrealProject>
    udocs_processor::InitCommand::CreateNewProject(
        const std::string& ProjectDir) const {
  // load default docs descriptor
  std::ifstream Input(ResourcesPath + DIRECTORY_SEPARATOR +
      DEFAULT_DOCS_DIRECTORY + DIRECTORY_SEPARATOR + SURREAL_JSON);
  if (!Input.good()) {
    throw std::runtime_error{"Unable to load the default project"};
  }
  std::stringstream Buffer;
  Buffer << Input.rdbuf();

  std::unique_ptr<SurrealProject> Project = Loader->Deserialize(Buffer.str());

  std::vector<UnrealInteraction::UnrealVersion> Versions =
      Ue->EnumerateVersions();
  size_t UsedVersion = Ue->DetermineVersion(ProjectDir, Versions);
  Project->EngineRoot = UsedVersion < Versions.size()
      ? Versions[UsedVersion].EngineDir
      : (Versions.empty() ? "" : Versions[0].EngineDir);
  std::optional<std::string> LastCompiledTarget =
      Ue->DetermineLastCompiledTarget(ProjectDir);
  Project->Target = LastCompiledTarget ? *LastCompiledTarget : "";
  Project->Name = Ue->DetermineProjectName(ProjectDir);

  bool HasSetConf = false;
  if (LastCompiledTarget) {
    std::map<std::string, std::optional<std::string>> Targets =
        Ue->EnumerateTargets(ProjectDir);
    auto It = Targets.find(*LastCompiledTarget);
    if (It != Targets.end() && It->second) {
      Project->Configuration = *It->second;
      HasSetConf = true;
    }
  }

  if (!HasSetConf) {
    Project->Configuration = DEFAULT_CONFIGURATION;
  }

  return Project;
}

std::unique_ptr<udocs_processor::SurrealProject>
    udocs_processor::InitCommand::LoadProject(
        const std::string& ProjectDir) const {
  std::ifstream Input(ProjectDir + DIRECTORY_SEPARATOR + SURDOCS_DIRECTORY +
      DIRECTORY_SEPARATOR + SURREAL_JSON);
  if (!Input.good()) {
    throw std::runtime_error{"Unable to load the project"};
  }
  std::stringstream Buffer;
  Buffer << Input.rdbuf();

  return Loader->Deserialize(Buffer.str());
}

void udocs_processor::InitCommand::InstallPlugin(
    const std::string &ProjectDir) const {
  std::filesystem::path PluginPath{InstallPath + PLUGIN_DIRECTORY};
  std::filesystem::path PluginsDir{UnrealInteraction::PLUGINS_DIRECTORY +
      (DIRECTORY_SEPARATOR + std::string(TARGET_PLUGIN_DIRECTORY))};

  std::filesystem::create_directories(PluginsDir);

  std::filesystem::copy(PluginPath, PluginsDir,
      std::filesystem::copy_options::overwrite_existing |
      std::filesystem::copy_options::recursive);

  Ue->EnablePlugin(ProjectDir, UNREALISTIC_DOCS_PLUGIN);
}

void udocs_processor::InitCommand::SaveProject(const SurrealProject &Project,
    const std::string &ProjectDir) const {
  std::string DirectoryPath = ProjectDir + DIRECTORY_SEPARATOR +
      SURDOCS_DIRECTORY;
  std::filesystem::create_directories({DirectoryPath});
  std::ofstream OutputTo(DirectoryPath + DIRECTORY_SEPARATOR + SURREAL_JSON,
      std::ios::trunc);
  if (!OutputTo.good()) {
    throw std::runtime_error{"Unable to save the project"};
  }
  OutputTo << Loader->Serialize(Project);
}

void udocs_processor::InitCommand::InstallDefaultDocumentation(
    const std::string &ProjectDir) const {
  std::filesystem::path DefaultDocPath{ResourcesPath + DEFAULT_DOCS_DIRECTORY};
  std::filesystem::path SurdocsDir{SURDOCS_DIRECTORY};

  std::filesystem::copy(DefaultDocPath, SurdocsDir,
      std::filesystem::copy_options::overwrite_existing |
      std::filesystem::copy_options::recursive);
}

bool udocs_processor::InitCommand::IsNewProject(
    const std::string &ProjectDir) const {
  std::filesystem::path SurrealPath{ProjectDir + DIRECTORY_SEPARATOR +
      SURDOCS_DIRECTORY + DIRECTORY_SEPARATOR + SURREAL_JSON};

  return !std::filesystem::exists(SurrealPath);
}

void udocs_processor::InitCommand::InitializeProject(
    const std::string& ProjectDir, bool IsNewProject) const {
  if (IsNewProject) {
    InstallDefaultDocumentation(ProjectDir);
  }

  InstallPlugin(ProjectDir);

  if (IsNewProject) {
    Ue->EnableAutoCompile(ProjectDir);
  }
}

void udocs_processor::InitCommand::SetInstallPath(std::string InstallPath) {
  this->InstallPath = StringHelper::Normalize(InstallPath);
}

void udocs_processor::InitCommand::SetResourcesPath(std::string ResourcesPath) {
  this->ResourcesPath = StringHelper::Normalize(ResourcesPath);
}

udocs_processor::InitCommand::InitCommand(
    std::unique_ptr<UnrealInteraction> Ue,
    std::shared_ptr<SurrealLoader> Loader) : Ue(std::move(Ue)),
    Loader(std::move(Loader)) {}
