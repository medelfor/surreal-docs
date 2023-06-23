/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/ProjectCreateCommand.h"
#include "udocs-processor/cli/views/ProjectCreateView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"

namespace udocs_processor {
class ProjectCreateCLI {
 public:
  struct Arguments {};

  ProjectCreateCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<ProjectCreateCommand> Command,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool CreateProject(const Arguments& Args) const;

  void SetView(std::unique_ptr<ProjectCreateView> CliView);

 private:
  ProjectCreateCommand::CreateRequest MakeRequest(const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "project-create-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "project-create";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<ProjectCreateView> CliView;
  std::unique_ptr<ProjectCreateCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
};
}  // namespace udocs_processor
