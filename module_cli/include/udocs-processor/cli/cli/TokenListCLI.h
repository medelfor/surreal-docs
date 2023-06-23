/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/TokenListCommand.h"
#include "udocs-processor/cli/views/TokenListView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"

namespace udocs_processor {
class TokenListCLI {
 public:
  struct Arguments {};

  TokenListCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<TokenListCommand> Command,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool ListTokens(const Arguments& Args) const;

  void SetView(std::unique_ptr<TokenListView> CliView);

 private:
  TokenListCommand::ListRequest MakeRequest(const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "token-list-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "token-list";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<TokenListView> CliView;
  std::unique_ptr<TokenListCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
};
}  // namespace udocs_processor
