/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <map>
#include <memory>
#include <chrono> NOLINT()
#include "udocs-processor/cli/commands/OrganizationDeleteCommand.h"
#include "udocs-processor/cli/views/OrganizationDeleteView.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"
#include "udocs-processor/cli/cli/token/TokenLoader.h"

namespace udocs_processor {
class OrganizationDeleteCLI {
 public:
  struct Arguments {
    bool PreConfirmation = false;
    std::string Name;
    TokenLoader::TokenSource Source = TokenLoader::TokenSource::NO_PREFERENCE;
  };

  OrganizationDeleteCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<OrganizationDeleteCommand> Command,
      std::shared_ptr<TokenLoader> Token,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool DeleteOrganization(const Arguments& Args) const;

  void SetView(std::unique_ptr<OrganizationDeleteView> CliView);

 private:
  OrganizationDeleteCommand::DeleteRequest MakeRequest(
      const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "organization-delete-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "organization-delete";

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<OrganizationDeleteView> CliView;
  std::unique_ptr<OrganizationDeleteCommand> Command;

  std::shared_ptr<spdlog::logger> l;

  std::shared_ptr<BasicTelemetry> Telemetry;
  std::shared_ptr<TokenLoader> Token;
};
}  // namespace udocs_processor
