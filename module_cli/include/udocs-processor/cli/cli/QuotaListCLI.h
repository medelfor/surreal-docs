/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/QuotaListCommand.h"
#include "udocs-processor/cli/views/QuotaListView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"

namespace udocs_processor {
class QuotaListCLI {
 public:
  struct Arguments {};

  QuotaListCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<QuotaListCommand> Command,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool ListQuota(const Arguments& Args) const;

  void SetView(std::unique_ptr<QuotaListView> CliView);

 private:
  QuotaListCommand::ListRequest MakeRequest(const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "quota-list-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "quota-list";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<QuotaListView> CliView;
  std::unique_ptr<QuotaListCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
};
}  // namespace udocs_processor
