/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/InitCommand.h"
#include "udocs-processor/cli/views/InitView.h"
#include "udocs-processor/cli/ue/UnrealInteraction.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"

namespace udocs_processor {
class InitCLI {
 public:
  struct Arguments {};

  InitCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<InitCommand> Command,
      std::unique_ptr<UnrealInteraction> Ue,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool Init(const Arguments& Args);

  void SetView(std::unique_ptr<InitView> InteractiveView);

  void OnDone();

 private:
  std::filesystem::path Absolutify(const std::string& Path) const;

  void InitializeView(const SurrealProject& Project);

  static constexpr const char* LOGGER_NAME = "init-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "init";

  static constexpr const char DEFAULT_CONFIGURATION[] = "Debug";

  static constexpr const char CURRENT_DIRECTORY[] = "./";
  static constexpr char DIRECTORY_SEPARATOR = '/';

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<InitView> InteractiveView;
  std::unique_ptr<InitCommand> Command;
  std::unique_ptr<UnrealInteraction> Ue;
  std::vector<UnrealInteraction::Source> Sources;
  std::set<int> InitiallySelectedSources;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;

  std::unique_ptr<SurrealProject> Project;

  bool IsNewProject = false;
  bool IsDone = true;
};
}  // namespace udocs_processor
