/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/MigrateCommand.h"
#include "udocs-processor/cli/views/MigrateView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"

namespace udocs_processor {
class MigrateCLI {
 public:
  struct Arguments {};

  MigrateCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<MigrateCommand> Command,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool Migrate(const Arguments& Args) const;

  void SetView(std::unique_ptr<MigrateView> CliView);

 private:
  MigrateCommand::MigrateRequest MakeRequest(const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "migrate-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "migrate";
  static constexpr const char* CURRENT_DIRECTORY = ".";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<MigrateView> CliView;
  std::unique_ptr<MigrateCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
};
}  // namespace udocs_processor
