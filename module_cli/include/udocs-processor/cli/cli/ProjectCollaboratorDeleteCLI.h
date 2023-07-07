/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/ProjectCollaboratorDeleteCommand.h"
#include "udocs-processor/cli/views/ProjectCollaboratorDeleteView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"
#include "udocs-processor/cli/cli/token/TokenLoader.h"

namespace udocs_processor {
class ProjectCollaboratorDeleteCLI {
 public:
  struct Arguments {
    std::string Location;
    std::string Email;
    bool PreConfirmation = false;
    TokenLoader::TokenSource Source = TokenLoader::TokenSource::NO_PREFERENCE;
  };

  ProjectCollaboratorDeleteCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<ProjectCollaboratorDeleteCommand> Command,
      std::shared_ptr<TokenLoader> Token,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool DeleteProjectCollaborator(const Arguments& Args) const;

  void SetView(std::unique_ptr<ProjectCollaboratorDeleteView> CliView);

 private:
  ProjectCollaboratorDeleteCommand::DeleteRequest MakeRequest(
      const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "project-collaborator-delete-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME =
      "project-collaborator-delete";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<ProjectCollaboratorDeleteView> CliView;
  std::unique_ptr<ProjectCollaboratorDeleteCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
  std::shared_ptr<TokenLoader> Token;
};
}  // namespace udocs_processor
