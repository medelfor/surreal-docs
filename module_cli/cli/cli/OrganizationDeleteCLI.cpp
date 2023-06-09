/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/cli/OrganizationDeleteCLI.h"
#include <spdlog/spdlog.h>
#include <future> NOLINT()
#include <thread> NOLINT()
#include <chrono> NOLINT()

bool udocs_processor::OrganizationDeleteCLI::DeleteOrganization(
    const Arguments &Args) const {
  Telemetry->ReportInit(TELEMETRY_COMMAND_NAME);

  CliView->Init();

  bool Success = true;
  auto Future = std::async(std::launch::async,
    [this, &Success, &Args]() {
      std::string OutDirectory;
      try {
        if (Args.PreConfirmation || CliView->Confirm()) {
          CliView->ReportProgress();
          OrganizationDeleteCommand::DeleteRequest Request = MakeRequest(Args);

          Command->Delete(Request);
          CliView->ReportSuccess();
        } else {
          l->info("User cancelled the deletion");
        }
      } catch (const std::exception& Exc) {
        Success = false;
        Telemetry->ReportFail(TELEMETRY_COMMAND_NAME, Exc.what());
        l->error("Exception in organization delete thread: {}", Exc.what());
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

udocs_processor::OrganizationDeleteCommand::DeleteRequest
    udocs_processor::OrganizationDeleteCLI::MakeRequest(
        const Arguments& Args) const {
  return {Args.Name, Token->LoadToken(Args.Source)};
}

void udocs_processor::OrganizationDeleteCLI::SetView(
    std::unique_ptr<OrganizationDeleteView> View) {
  this->CliView = std::move(View);
}

udocs_processor::OrganizationDeleteCLI::OrganizationDeleteCLI(
  std::shared_ptr<spdlog::sinks::sink> Sink,
  std::unique_ptr<OrganizationDeleteCommand> Command,
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
