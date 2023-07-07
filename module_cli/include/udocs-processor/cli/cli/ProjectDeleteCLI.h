/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/ProjectDeleteCommand.h"
#include "udocs-processor/cli/views/ProjectDeleteView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"
#include "udocs-processor/cli/cli/token/TokenLoader.h"

namespace udocs_processor {
class ProjectDeleteCLI {
 public:
  struct Arguments {
    std::string Location;
    std::optional<std::string> Version;
    bool DeleteAllVersionsOnly = false;
    bool PreConfirmation = false;
    TokenLoader::TokenSource Source = TokenLoader::TokenSource::NO_PREFERENCE;
  };

  ProjectDeleteCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<ProjectDeleteCommand> Command,
      std::shared_ptr<TokenLoader> Token,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool DeleteProject(const Arguments& Args) const;

  void SetView(std::unique_ptr<ProjectDeleteView> CliView);

 private:
  ProjectDeleteCommand::DeleteRequest MakeRequest(const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "project-delete-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "project-delete";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<ProjectDeleteView> CliView;
  std::unique_ptr<ProjectDeleteCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
  std::shared_ptr<TokenLoader> Token;
};
}  // namespace udocs_processor
