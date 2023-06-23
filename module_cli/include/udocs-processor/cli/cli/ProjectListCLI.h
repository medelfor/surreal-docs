/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/ProjectListCommand.h"
#include "udocs-processor/cli/views/ProjectListView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"

namespace udocs_processor {
class ProjectListCLI {
 public:
  struct Arguments {};

  ProjectListCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<ProjectListCommand> Command,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool ListProjects(const Arguments& Args) const;

  void SetView(std::unique_ptr<ProjectListView> CliView);

 private:
  ProjectListCommand::ListRequest MakeRequest(const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "project-list-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "project-list";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<ProjectListView> CliView;
  std::unique_ptr<ProjectListCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
};
}  // namespace udocs_processor
