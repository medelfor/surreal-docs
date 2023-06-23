/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/TokenCreateCommand.h"
#include "udocs-processor/cli/views/TokenCreateView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"

namespace udocs_processor {
class TokenCreateCLI {
 public:
  struct Arguments {};

  TokenCreateCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<TokenCreateCommand> Command,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool CreateToken(const Arguments& Args) const;

  void SetView(std::unique_ptr<TokenCreateView> CliView);

 private:
  TokenCreateCommand::CreateRequest MakeRequest(const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "token-create-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "token-create";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<TokenCreateView> CliView;
  std::unique_ptr<TokenCreateCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
};
}  // namespace udocs_processor
