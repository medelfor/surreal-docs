/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <boost/asio.hpp> NOLINT()
#include <boost/process.hpp> NOLINT()
#include "udocs-processor/cli/ue/UnrealInteraction.h"
#include <spdlog/spdlog.h> NOLINT()
#include <fstream> NOLINT()
#include <regex> NOLINT()
#include <set> NOLINT()
#include "udocs-processor/StringHelper.h"
#include "udocs-processor/ExecStreamHelper.h"

std::vector<udocs_processor::UnrealInteraction::UnrealVersion>
    udocs_processor::UnrealInteraction::EnumerateVersions() const {
  std::vector<UnrealVersion> Versions;

  winreg::RegKey Key{};

  winreg::RegResult Opened = Key.TryOpen(HKEY_LOCAL_MACHINE,
      UNREAL_VERSIONS_REG_KEY, KEY_WOW64_64KEY | KEY_READ);
  if (Opened) {
    for (const auto& SubKeyName : Key.EnumSubKeys()) {
      winreg::RegKey SubKey;
      Opened = SubKey.TryOpen(HKEY_LOCAL_MACHINE, UNREAL_VERSIONS_REG_KEY +
          std::wstring(REGISTRY_KEY_SEPARATOR) + SubKeyName,
          KEY_WOW64_64KEY | KEY_READ);
      if (Opened) {
        winreg::RegExpected<std::wstring> Val =
            SubKey.TryGetStringValue(UNREAL_VERSION_VALUE_KEY);
        if (Val) {
          Versions.emplace_back(UnrealVersion{
              StringHelper::ws2s(SubKeyName),
              StringHelper::ws2s(Val.GetValue()), false
          });
        }
      }
    }
  } else {
    l->error("Couldn't open versions reg key {}",
        StringHelper::ws2s(Opened.ErrorMessage()));
  }

  Opened = Key.TryOpen(HKEY_CURRENT_USER,
      UNREAL_BUILDS_REG_KEY, KEY_WOW64_64KEY | KEY_ALL_ACCESS);
  if (Opened) {
    for (const auto& Value : Key.EnumValues()) {
      winreg::RegExpected<std::wstring> Val =
          Key.TryGetStringValue(Value.first);
      if (Val) {
        Versions.emplace_back(UnrealVersion{
            StringHelper::ws2s(Value.first),
            StringHelper::ws2s(Val.GetValue()), true
        });
      }
    }
  } else {
    l->error("Couldn't open builds reg key {}",
        StringHelper::ws2s(Opened.ErrorMessage()));
  }

  return Versions;
}

nlohmann::json udocs_processor::UnrealInteraction::LoadProjectFile(
    const std::string& ProjectDir) const {
  std::string ProjectFile = FindProjectFile(ProjectDir);
  return nlohmann::json::parse(std::ifstream(ProjectDir + DIRECTORY_SEPARATOR +
      ProjectFile));
}

std::string udocs_processor::UnrealInteraction::FindProjectFile(
    const std::string& ProjectDir) const {
  std::filesystem::directory_iterator Files{ProjectDir};
  std::string FileName;
  bool HasFound = false;
  for (const auto& File : Files) {
    if (File.is_regular_file()) {
      std::string Name = File.path().filename().string();
      size_t Pos = Name.rfind(UNREAL_PROJECT_EXTENSION);
      if (Pos != std::string::npos &&
          Pos + strlen(UNREAL_PROJECT_EXTENSION) == Name.size()) {
        l->info("Found `{}` as a project file", Name);
        HasFound = true;
        FileName = std::move(Name);
        break;
      }
    }
  }

  if (!HasFound) {
    l->error("Unable to locate project file in `{}`", ProjectDir);
    throw std::runtime_error{"No project file found"};
  }

  return FileName;
}

size_t udocs_processor::UnrealInteraction::DetermineVersion(
    const std::string &ProjectDir,
    const std::vector<UnrealVersion> &Versions) const {
  nlohmann::json ProjectFile = LoadProjectFile(ProjectDir);
  std::string EngineAssociation;
  if (ProjectFile.contains(PROJECT_ENGINE_ASSOCIATION)) {
    EngineAssociation = ProjectFile[PROJECT_ENGINE_ASSOCIATION];
  }

  if (EngineAssociation.find(DIRECTORY_SEPARATOR) != std::string::npos) {
    // association is a path to the engine root
    for (size_t i = 0; i < Versions.size(); ++i) {
      const auto &Version = Versions[i];
      std::filesystem::path PathOne{Version.EngineDir};
      std::filesystem::path PathTwo{ProjectDir};
      PathTwo /= EngineAssociation;
      if (PathOne == PathTwo) {
        return i;
      }
    }
  } else if (!EngineAssociation.empty()) {
    // association is a version/identifier
    for (size_t i = 0; i < Versions.size(); ++i) {
      const auto &Version = Versions[i];
      if (Version.Version == EngineAssociation) {
        return i;
      }
    }
  }

  // identifier not found amongst versions, resort to parent directory
  // iteration
  std::filesystem::path ProjectPath{ProjectDir};
  while (ProjectPath.parent_path() != ProjectPath) {
    ProjectPath = std::filesystem::absolute(ProjectPath);
    for (size_t i = 0; i < Versions.size(); ++i) {
      const auto &Version = Versions[i];
      std::filesystem::path PathOne{Version.EngineDir};
      if (std::filesystem::exists(PathOne)) {
        PathOne = std::filesystem::absolute(PathOne);
      }
      if (PathOne == ProjectPath) {
        l->info("Found engine at `{}`", ProjectPath.string());
        return i;
      }
    }

    ProjectPath = ProjectPath.parent_path();
  }

  return ~0;
}

void udocs_processor::UnrealInteraction::Build(const std::string &Target,
    const std::string& Configuration, const std::string &ProjectDir,
    const UnrealVersion &Version) const {
  if (Target.empty()) return;

  std::string ProjectFile = FindProjectFile(ProjectDir);

  std::vector<std::string> Arguments = {
      StringHelper::Normalize(Version.EngineDir) + UNREAL_UAT_PATH,
      UNREAL_UAT_BUILD_COMMAND, UNREAL_UAT_PROJECT +
      StringHelper::Normalize(ProjectDir) + ProjectFile,
      UNREAL_UAT_TARGET + (Target.empty() ? DEFAULT_TARGET : Target),
      UNREAL_UAT_PLATFORM_WIN64, UNREAL_UAT_CLIENT_CONFIG + Configuration,
      UNREAL_UAT_BUILD, UNREAL_UAT_NOTOOLS};

  using time_point = std::chrono::steady_clock::time_point;
  time_point Begin = std::chrono::steady_clock::now();

  ExecStreamHelper::ProcessResult Result = ExecStreamHelper::Run(Arguments);

  l->warn("UAT build output ({}):\n{}", Result.ExitCode, Result.Output);
  uint64_t ExitCode = ParseUATExitCode(Result.Output);
  time_point End = std::chrono::steady_clock::now();
  uint64_t TimeMs = std::chrono::duration_cast<
      std::chrono::milliseconds>(End - Begin).count();
  l->info("UAT build took {}ms and exited with code {}",
      TimeMs, ExitCode);

  if (ExitCode) {
    l->error("UAT failed");
    throw std::runtime_error{fmt::format("UAT exited with code {}",
        ExitCode)};
  }
}

uint64_t udocs_processor::UnrealInteraction::ParseUATExitCode(
    const std::string& UATOutput) const {
  static std::regex Pattern{UAT_EXIT_CODE_PATTERN, std::regex::icase};
  std::smatch Match;
  if (std::regex_search(UATOutput, Match, Pattern)) {
    uint64_t ExitCode = StringHelper::SafeStoull(
        Match[UAT_EXIT_CODE_PATTERN_CODE_GROUP].str());
    return ExitCode;
  }

  return ~0;
}

std::string udocs_processor::UnrealInteraction::FindUeEditor(
    const std::string& EngineDir, bool DoUseCmd) const {
  std::filesystem::path Engine5Editor{EngineDir + DIRECTORY_SEPARATOR +
      (DoUseCmd ? UNREAL_EDITOR_5_CMD_PATH : UNREAL_EDITOR_5_PATH)};
  std::filesystem::path Engine4Editor{EngineDir + DIRECTORY_SEPARATOR +
      (DoUseCmd ? UNREAL_EDITOR_4_CMD_PATH : UNREAL_EDITOR_4_PATH)};

  if (std::filesystem::exists(Engine5Editor)) {
    return Engine5Editor.string();
  }
  if (std::filesystem::exists(Engine4Editor)) {
    return Engine4Editor.string();
  }

  std::string Message = fmt::format("Couldn't find editor binary at `{}`",
      EngineDir);
  l->error(Message);
  throw std::runtime_error{Message};
}

void udocs_processor::UnrealInteraction::Run(const std::string &ProjectDir,
    const UnrealVersion& Version, const std::vector<std::string> &Arguments,
    bool DoUseCmd) const {
  std::string ProjectFile = FindProjectFile(ProjectDir);

  std::string UeEditor = FindUeEditor(Version.EngineDir, DoUseCmd);
  std::vector<std::string> FinalArgs;
  FinalArgs.emplace_back(UeEditor);
  FinalArgs.emplace_back(StringHelper::Normalize(ProjectDir) + ProjectFile);
  FinalArgs.insert(FinalArgs.end(), Arguments.begin(), Arguments.end());

  using time_point = std::chrono::steady_clock::time_point;
  time_point Begin = std::chrono::steady_clock::now();

  ExecStreamHelper::ProcessResult Result = ExecStreamHelper::Run(FinalArgs);

  l->warn("Editor output:\n{}", Result.Output);
  uint64_t ExitCode = Result.ExitCode;  // ParseUATExitCode(Output);
  time_point End = std::chrono::steady_clock::now();
  uint64_t TimeMs = std::chrono::duration_cast<
      std::chrono::milliseconds>(End - Begin).count();
  l->info("Editor took {}ms and exited with code {}",
      TimeMs, Result.ExitCode);

  if (Result.ExitCode) {
    l->warn("Editor exited with {}", Result.ExitCode);
  }
}

bool udocs_processor::UnrealInteraction::CheckIfUnrealProjectRoot(
    const std::string &ProjectDir) const {
  try {
    nlohmann::json ProjectFile = LoadProjectFile(ProjectDir);
  } catch (const std::exception& Exc) {
    return false;
  }

  return true;
}

void
udocs_processor::UnrealInteraction::EnablePlugin(const std::string &ProjectDir,
    const std::string &PluginName) const {
  std::string ProjectFile = FindProjectFile(ProjectDir);
  nlohmann::json Project = nlohmann::json::parse(std::ifstream(
      ProjectDir + DIRECTORY_SEPARATOR + ProjectFile));
  if (!Project.contains(PROJECT_PLUGINS)) {
    Project[PROJECT_PLUGINS] = nlohmann::json::array();
  }

  if (Project[PROJECT_PLUGINS].is_array()) {
    bool IsEnabledAlready = false;

    for (auto &Plugin : Project[PROJECT_PLUGINS]) {
      if (Plugin.contains(PROJECT_PLUGIN_NAME) &&
          Plugin[PROJECT_PLUGIN_NAME].is_string() &&
          Plugin[PROJECT_PLUGIN_NAME] == PluginName) {
        Plugin[PROJECT_PLUGIN_ENABLED] = true;
        IsEnabledAlready = true;
      }
    }

    if (!IsEnabledAlready) {
      nlohmann::json NewPlugin;
      NewPlugin[PROJECT_PLUGIN_NAME] = PluginName;
      NewPlugin[PROJECT_PLUGIN_ENABLED] = true;
      Project[PROJECT_PLUGINS].emplace_back(std::move(NewPlugin));
    }
  } else {
    l->error("Plugins are not an array");
    throw std::runtime_error{"Invalid format of the project file"};
  }

  // let's make backup
  std::ifstream ProjectFileInput(ProjectFile);
  std::ofstream ProjectFileBackup(ProjectFile + BACKUP_POSTFIX, std::ios::out);
  ProjectFileBackup << ProjectFileInput.rdbuf();
  if (!ProjectFileBackup.good()) {
    l->error("Couldn't create backup {}", ProjectFile + BACKUP_POSTFIX);
  }
  ProjectFileBackup.close();

  std::filesystem::path ProjectFilePath{ProjectFile};
  if (!std::filesystem::exists(ProjectFilePath)) {
    l->error("{} doesn't exist", ProjectFile);
    throw std::runtime_error{"Project file doesn't exist"};
  }

  // remove read only
  std::filesystem::permissions(ProjectFilePath,
      std::filesystem::perms::group_all | std::filesystem::perms::owner_all,
      std::filesystem::perm_options::add);

  std::ofstream NewProjectFile(ProjectFile, std::ios::out);
  if (!NewProjectFile.good()) {
    l->error("Cannot open {} for write", ProjectFile);
    throw std::runtime_error{"Unable to write to the project file"};
  }
  NewProjectFile << Project.dump(JSON_INDENT);
}

std::vector<std::string> udocs_processor::UnrealInteraction::EnumerateModules(
    const std::string &ProjectDir) const {
  nlohmann::json ProjectFile = LoadProjectFile(ProjectDir);
  std::vector<std::string> Modules;

  if (ProjectFile.contains(PROJECT_MODULES) &&
      ProjectFile[PROJECT_MODULES].is_array()) {
    for (const auto& Module : ProjectFile[PROJECT_MODULES]) {
      if (Module.is_object() && Module.contains(PROJECT_MODULE_NAME) &&
          Module[PROJECT_MODULE_NAME].is_string()) {
        Modules.emplace_back(Module[PROJECT_MODULE_NAME]);
      }
    }
  }

  return Modules;
}

std::string udocs_processor::UnrealInteraction::DetermineProjectName(
    const std::string &ProjectDir) const {
  std::string ProjectFile = FindProjectFile(ProjectDir);

  return ProjectFile.substr(0, ProjectFile.rfind(EXTENSION_SEPARATOR));
}

udocs_processor::UnrealInteraction::UnrealInteraction(
    std::shared_ptr<spdlog::sinks::sink> Sink) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }

  Configurations.emplace_back(CONFIGURATION_DEBUG);
  Configurations.emplace_back(CONFIGURATION_DEBUG_GAME);
  Configurations.emplace_back(CONFIGURATION_DEVELOPMENT);
  Configurations.emplace_back(CONFIGURATION_SHIPPING);
  Configurations.emplace_back(CONFIGURATION_TEST);
}

std::map<std::string, std::optional<std::string>>
udocs_processor::UnrealInteraction::EnumerateTargets(
    const std::string &ProjectDir) const {
  std::string ProjectName = DetermineProjectName(ProjectDir);

  std::set<std::string> Targets = FindAllAvailableTargets(ProjectDir);
  std::filesystem::path TargetsDirectory{ProjectDir + DIRECTORY_SEPARATOR +
      WIN64_TARGETS_DIRECTORY};

  std::map<std::string, std::optional<std::string>> Result;
  if (std::filesystem::exists(TargetsDirectory)) {
    std::filesystem::directory_iterator It{TargetsDirectory};

    for (const auto& Path : It) {
      if (!Path.is_directory()) continue;
      std::string Name = Path.path().filename().string();

      if (!Targets.count(Name)) continue;

      std::optional<std::string> LastConfiguration;
      std::filesystem::file_time_type LatestTime;

      for (const auto& Conf : Configurations) {
        std::filesystem::path ConfDirectory{Path.path().string() +
            DIRECTORY_SEPARATOR + Conf};

        if (!std::filesystem::exists(ConfDirectory)) continue;

        auto LastWriteTime = std::filesystem::last_write_time(ConfDirectory);
        if (LastWriteTime > LatestTime) {
          LastConfiguration = Conf;
          LatestTime = LastWriteTime;
        }
      }

      Result.insert(std::make_pair(Name, LastConfiguration));
    }
  }

  // insert remaining targets that weren't built at all
  for (const auto& Target : Targets) {
    if (Result.find(Target) == Result.end()) {
      Result.insert(std::make_pair(Target, std::optional<std::string>{}));
    }
  }

  return Result;

  return {};
}

std::vector<udocs_processor::UnrealInteraction::Source>
    udocs_processor::UnrealInteraction::EnumerateSources(
        const std::string &ProjectDir) const {
  std::vector<Source> PluginModules = FindModules(ProjectDir +
      DIRECTORY_SEPARATOR + PLUGINS_DIRECTORY);
  std::vector<Source> Modules = FindModules(ProjectDir +
      DIRECTORY_SEPARATOR + SOURCE_DIRECTORY);

  for (auto& Module : PluginModules) {
    Module.IsPlugin = true;
  }

  PluginModules.insert(PluginModules.end(),
      std::move_iterator(Modules.begin()), std::move_iterator(Modules.end()));

  return PluginModules;
}

std::vector<udocs_processor::UnrealInteraction::Source>
    udocs_processor::UnrealInteraction::FindModules(
        const std::string& Dir) const {
  static std::regex BuildCsRegex{BUILD_CS_PATTERN, std::regex::icase};

  std::filesystem::path Path{Dir};
  if (std::filesystem::exists(Path)) {
    std::filesystem::recursive_directory_iterator It{Path};

    std::vector<Source> Result;
    for (const auto& File : It) {
      std::smatch Match;
      std::string FileName = File.path().filename().string();

      if (File.is_regular_file() &&
          std::regex_match(FileName, Match, BuildCsRegex)) {
        std::string ModuleName = Match[BUILD_CS_PATTERN_NAME_INDEX].str();

        if (ModuleName == UNREALISTIC_DOCS_PLUGIN) continue;

        Result.emplace_back(Source{
          File.path().parent_path().string(), ModuleName
        });
      }
    }

    return Result;
  }

  return {};
}

std::optional<std::string>
    udocs_processor::UnrealInteraction::DetermineLastCompiledTarget(
        const std::string &ProjectDir) const {
  std::set<std::string> AllTargets = FindAllAvailableTargets(ProjectDir);

  std::filesystem::path TargetsDirectory{ProjectDir + DIRECTORY_SEPARATOR +
      WIN64_TARGETS_DIRECTORY};
  if (!std::filesystem::exists(TargetsDirectory)) return {};

  std::filesystem::directory_iterator It{TargetsDirectory};

  if (std::filesystem::exists(TargetsDirectory)) {
    std::optional<std::string> LastTarget;
    std::filesystem::file_time_type LatestTime;

    for (const auto &Path : It) {
      if (!Path.is_directory()) continue;
      std::string Name = Path.path().filename().string();

      if (!AllTargets.count(Name)) continue;

      auto LastWriteTime = std::filesystem::last_write_time(Path);
      if (LastWriteTime > LatestTime) {
        LastTarget = Path.path().filename().string();
        LatestTime = LastWriteTime;
      }
    }

    return LastTarget;
  }

  return {};
}

bool udocs_processor::UnrealInteraction::IsBlueprintProject(
    const std::string &ProjectDir) const {
  std::filesystem::path SourcePath{ProjectDir + DIRECTORY_SEPARATOR +
      SOURCE_DIRECTORY};

  return !std::filesystem::exists(SourcePath);
}

std::set<std::string>
    udocs_processor::UnrealInteraction::FindAllAvailableTargets(
        const std::string& ProjectDir) const {
  static std::regex TargetRegex{TARGET_PATTERN, std::regex::icase};

  std::set<std::string> Result;

  std::filesystem::recursive_directory_iterator It{ProjectDir};

  for (const auto& File : It) {
    std::smatch Match;
    std::string FileName = File.path().filename().string();

    if (File.is_regular_file() &&
        std::regex_match(FileName, Match, TargetRegex)) {
      std::string TargetName = Match[TARGET_PATTERN_NAME_INDEX].str();

      Result.emplace(std::move(TargetName));
    }
  }

  return Result;
}

void udocs_processor::UnrealInteraction::EnableAutoCompile(
    const std::string &ProjectDir) const {
  std::filesystem::path PerProjectEditorSettingsPath{
    PER_PROJECT_EDITOR_SETTINGS_PATH};

  bool Exists = true;
  if (!std::filesystem::exists(PerProjectEditorSettingsPath)) {
    std::filesystem::create_directories(
        PerProjectEditorSettingsPath.parent_path());
    Exists = false;
  }

  std::string Content;
  if (Exists) {
    std::ifstream Input(PerProjectEditorSettingsPath.string());
    if (!Input.good()) {
      l->error("Couldn't create per project settings path {}",
          PerProjectEditorSettingsPath.string());
      return;  // do not fail or throw, just ignore
    }
    std::stringstream ContentStream;
    ContentStream << Input.rdbuf();
    Content = ContentStream.str();
    Input.close();
  }

  std::ofstream Output(PerProjectEditorSettingsPath.string());
  if (!Output.good()) {
    l->error("Couldn't write per project settings path {}",
        PerProjectEditorSettingsPath.string());
    return;  // do not fail or throw, just ignore
  }

  Output << Content;

  std::regex CompileEnabledRegex{COMPILE_ENABLED_PATTERN, std::regex::icase};
  if (!std::regex_search(Content, CompileEnabledRegex)) {
    Output << AUTO_COMPILE_ON_STARTUP;
  }
}
