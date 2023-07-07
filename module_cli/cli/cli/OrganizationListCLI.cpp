/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/cli/OrganizationListCLI.h"
#include <spdlog/spdlog.h>
#include <future> NOLINT()
#include <thread> NOLINT()
#include <chrono> NOLINT()

bool udocs_processor::OrganizationListCLI::ListOrganizations(
    const Arguments &Args) const {
  Telemetry->ReportInit(TELEMETRY_COMMAND_NAME);

  CliView->Init();

  bool Success = true;
  auto Future = std::async(std::launch::async,
    [this, &Success, &Args]() {
      std::string OutDirectory;
      try {
        OrganizationListCommand::ListRequest Request = MakeRequest(Args);

        CliView->ShowOrganizations(Command->List(Request));
      } catch (const std::exception& Exc) {
        Success = false;
        Telemetry->ReportFail(TELEMETRY_COMMAND_NAME, Exc.what());
        l->error("Exception in organization list thread: {}", Exc.what());
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

udocs_processor::OrganizationListCommand::ListRequest
    udocs_processor::OrganizationListCLI::MakeRequest(
        const Arguments& Args) const {
  return {Token->LoadToken(Args.Source)};
}

void udocs_processor::OrganizationListCLI::SetView(
    std::unique_ptr<OrganizationListView> View) {
  this->CliView = std::move(View);
}

udocs_processor::OrganizationListCLI::OrganizationListCLI(
  std::shared_ptr<spdlog::sinks::sink> Sink,
  std::unique_ptr<OrganizationListCommand> Command,
  std::shared_ptr<TokenLoader> Token,
  std::shared_ptr<BasicTelemetry> Telemetry)
    : Command(std::move(Command)), Telemetry(Telemetry), Token(Token) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}
