/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <string>
#include <map>
#include <memory>
#include "udocs-processor/cli/commands/GenerateCommand.h"
#include "udocs-processor/cli/views/GenerateView.h"
#include "udocs-processor/cli/surreal/SurrealLoader.h"
#include "udocs-processor/telemetry/BasicTelemetry.h"
#include "udocs-processor/cli/cli/token/TokenLoader.h"

namespace udocs_processor {
class GenerateCLI {
 public:
  struct Arguments {
    bool IsInteractive = true;
    bool DoCleanOut = false;
    std::string OutDirectory;
    std::optional<std::string> DeployLocation;
    TokenLoader::TokenSource Source = TokenLoader::TokenSource::NO_PREFERENCE;
  };

  GenerateCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::unique_ptr<GenerateCommand> Command,
      std::unique_ptr<GenerateView> InteractiveView,
      std::unique_ptr<GenerateView> NonInteractiveView,
      std::shared_ptr<SurrealLoader> Loader,
      std::shared_ptr<TokenLoader> Token,
      std::shared_ptr<BasicTelemetry> Telemetry);

  bool Generate(const Arguments& Args) const;

 private:
  GenerateCommand::GenerateRequest MakeRequest(const Arguments& Args) const;

  static constexpr const char* LOGGER_NAME = "generate-cli";
  static constexpr const char* TELEMETRY_COMMAND_NAME = "generate";

  static constexpr const char CURRENT_DIRECTORY[] = "./";
  static constexpr const char SURDOCS_DIRECTORY[] = "surdocs";
  static constexpr const char SURREAL_JSON[] = "surreal.json";
  static constexpr char DIRECTORY_SEPARATOR = '/';

  static constexpr int POLL_DURATION_MS = 100;

  std::unique_ptr<GenerateView> InteractiveView;
  std::unique_ptr<GenerateView> NonInteractiveView;
  std::unique_ptr<GenerateCommand> Command;
  std::shared_ptr<SurrealLoader> Loader;

  std::shared_ptr<BasicTelemetry> Telemetry;

  std::shared_ptr<spdlog::logger> l;
  std::map<GenerateCommand::Status, GenerateView::Status>
      GenerateStatusToStatus;
  std::shared_ptr<TokenLoader> Token;
};
}  // namespace udocs_processor
