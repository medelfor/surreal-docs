/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/OrganizationCreateCommand.h"
#include "udocs-processor/cli/views/OrganizationCreateView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"

namespace udocs_processor {
class OrganizationCreateCLI {
 public:
  struct Arguments {};

  OrganizationCreateCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<OrganizationCreateCommand> Command,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool CreateOrganization(const Arguments& Args) const;

  void SetView(std::unique_ptr<OrganizationCreateView> CliView);

 private:
  OrganizationCreateCommand::CreateRequest MakeRequest(
      const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "organization-create-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "organization-create";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<OrganizationCreateView> CliView;
  std::unique_ptr<OrganizationCreateCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
};
}  // namespace udocs_processor
