/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/AuthCommand.h"
#include "udocs-processor/cli/views/AuthView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"

namespace udocs_processor {
class AuthCLI {
 public:
  struct Arguments {};

  AuthCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<AuthCommand> Command,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool Auth(const Arguments& Args) const;

  void SetView(std::unique_ptr<AuthView> CliView);

 private:
  AuthCommand::AuthRequest MakeRequest(const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "auth-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "auth";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<AuthView> CliView;
  std::unique_ptr<AuthCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
};
}  // namespace udocs_processor
