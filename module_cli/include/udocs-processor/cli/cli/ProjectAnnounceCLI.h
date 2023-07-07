/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <optional>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/ProjectAnnounceCommand.h"
#include "udocs-processor/cli/views/ProjectAnnounceView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"
#include "udocs-processor/cli/cli/token/TokenLoader.h"

namespace udocs_processor {
class ProjectAnnounceCLI {
 public:
  struct Arguments {
    std::string Location;
    std::optional<std::string> Announcement;
    TokenLoader::TokenSource Source = TokenLoader::TokenSource::NO_PREFERENCE;
  };

  ProjectAnnounceCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<ProjectAnnounceCommand> Command,
      std::shared_ptr<TokenLoader> Token,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool Announce(const Arguments& Args) const;

  void SetView(std::unique_ptr<ProjectAnnounceView> CliView);

 private:
  ProjectAnnounceCommand::AnnounceRequest MakeRequest(
      const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "project-announce-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "project-announce";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<ProjectAnnounceView> CliView;
  std::unique_ptr<ProjectAnnounceCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
  std::shared_ptr<TokenLoader> Token;
};
}  // namespace udocs_processor
