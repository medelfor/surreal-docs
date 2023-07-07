/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/ProjectPublishCommand.h"
#include "udocs-processor/cli/views/ProjectPublishView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"
#include "udocs-processor/cli/cli/token/TokenLoader.h"

namespace udocs_processor {
class ProjectPublishCLI {
 public:
  struct Arguments {
    std::string Location;
    enum class ProjectService::PublishProjectRequest::Scope Scope =
        ProjectService::PublishProjectRequest::Scope::PUBLIC;
    TokenLoader::TokenSource Source = TokenLoader::TokenSource::NO_PREFERENCE;
  };

  ProjectPublishCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<ProjectPublishCommand> Command,
      std::shared_ptr<TokenLoader> Token,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool PublishProject(const Arguments& Args) const;

  void SetView(std::unique_ptr<ProjectPublishView> CliView);

 private:
  ProjectPublishCommand::PublishRequest MakeRequest(
      const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "project-publish-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "project-publish";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<ProjectPublishView> CliView;
  std::unique_ptr<ProjectPublishCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
  std::shared_ptr<TokenLoader> Token;
};
}  // namespace udocs_processor
