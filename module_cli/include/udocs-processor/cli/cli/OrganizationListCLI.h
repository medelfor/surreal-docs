/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/OrganizationListCommand.h"
#include "udocs-processor/cli/views/OrganizationListView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"

namespace udocs_processor {
class OrganizationListCLI {
 public:
  struct Arguments {};

  OrganizationListCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<OrganizationListCommand> Command,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool ListOrganizations(const Arguments& Args) const;

  void SetView(std::unique_ptr<OrganizationListView> CliView);

 private:
  OrganizationListCommand::ListRequest MakeRequest(const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "organization-list-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "organization-list";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<OrganizationListView> CliView;
  std::unique_ptr<OrganizationListCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
};
}  // namespace udocs_processor
