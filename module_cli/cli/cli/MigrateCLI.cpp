/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/cli/MigrateCLI.h"
#include <spdlog/spdlog.h>
#include <filesystem> NOLINT()
#include <future> NOLINT()
#include <thread> NOLINT()
#include <chrono> NOLINT()
#include "udocs-processor/StringHelper.h"

bool udocs_processor::MigrateCLI::Migrate(const Arguments &Args) const {
  Telemetry->ReportInit(TELEMETRY_COMMAND_NAME);

  CliView->Init();

  bool Success = true;
  auto Future = std::async(std::launch::async,
    [this, &Success, &Args]() {
      std::string OutDirectory;
      try {
        MigrateCommand::MigrateRequest Request = MakeRequest(Args);

        std::vector<MigrateCommand::TakenAction> Protocol =
            Command->Migrate(Request);
        std::vector<MigrateView::MigrationAction> Actions;
        Actions.reserve(Protocol.size());
        for (auto& ProtocolAction : Protocol) {
          Actions.emplace_back(MigrateView::MigrationAction{
            std::move(ProtocolAction.Description)});
        }
        CliView->ReportProtocol(Actions);
      } catch (const std::exception& Exc) {
        Success = false;
        Telemetry->ReportFail(TELEMETRY_COMMAND_NAME, Exc.what());
        l->error("Exception in migrate thread: {}", Exc.what());
        CliView->ReportError(Exc.what());
      }

      CliView->SetFinished(true);
    });

  auto ViewThread = std::thread(
    [&, this]() {
      while (!CliView->DoExit()) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds{POLL_DURATION_MS});
        CliView->Tick();
      }
      CliView->Destroy();
    });
  CliView->Start();

  Future.wait();
  ViewThread.join();

  Telemetry->ReportFinish(TELEMETRY_COMMAND_NAME);

  return Success;
}

udocs_processor::MigrateCommand::MigrateRequest
    udocs_processor::MigrateCLI::MakeRequest(const Arguments& Args) const {
  std::string ProjectDir = std::filesystem::canonical(
      CURRENT_DIRECTORY).string();
  return {StringHelper::Normalize(ProjectDir)};
}

void udocs_processor::MigrateCLI::SetView(
    std::unique_ptr<MigrateView> View) {
  this->CliView = std::move(View);
}

udocs_processor::MigrateCLI::MigrateCLI(
  std::shared_ptr<spdlog::sinks::sink> Sink,
  std::unique_ptr<MigrateCommand> Command,
  std::shared_ptr<BasicTelemetry> Telemetry)
    : Command(std::move(Command)), Telemetry(Telemetry) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}
