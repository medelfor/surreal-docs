/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/cli/ProjectPublishCLI.h"
#include <spdlog/spdlog.h>
#include <future> NOLINT()
#include <thread> NOLINT()
#include <chrono> NOLINT()

bool udocs_processor::ProjectPublishCLI::PublishProject(
    const Arguments &Args) const {
  Telemetry->ReportInit(TELEMETRY_COMMAND_NAME);

  CliView->Init();

  bool Success = true;
  auto Future = std::async(std::launch::async,
    [this, &Success, &Args]() {
      std::string OutDirectory;
      try {
        ProjectPublishCommand::PublishRequest Request = MakeRequest(Args);

        Command->Publish(Request);
      } catch (const std::exception& Exc) {
        Success = false;
        Telemetry->ReportFail(TELEMETRY_COMMAND_NAME, Exc.what());
        l->error("Exception in publish request thread: {}", Exc.what());
        CliView->SetFinished(true);
      }

      if (Success) {
        CliView->SetFinished(true);
      }
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

udocs_processor::ProjectPublishCommand::PublishRequest
    udocs_processor::ProjectPublishCLI::MakeRequest(
        const Arguments& Args) const {
  return {};
}

void udocs_processor::ProjectPublishCLI::SetView(
    std::unique_ptr<ProjectPublishView> View) {
  this->CliView = std::move(View);
}

udocs_processor::ProjectPublishCLI::ProjectPublishCLI(
  std::shared_ptr<spdlog::sinks::sink> Sink,
  std::unique_ptr<ProjectPublishCommand> Command,
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
