/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/cli/ProjectDeleteCLI.h"
#include <spdlog/spdlog.h>
#include <future> NOLINT()
#include <thread> NOLINT()
#include <chrono> NOLINT()
#include "udocs-processor/cli/cli/util/CliHelper.h"

bool udocs_processor::ProjectDeleteCLI::DeleteProject(
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
          ProjectDeleteCommand::DeleteRequest Request = MakeRequest(Args);

          Command->Delete(Request);
          CliView->ReportSuccess();
        } else {
          l->info("User cancelled the deletion");
        }
      } catch (const std::exception& Exc) {
        Success = false;
        Telemetry->ReportFail(TELEMETRY_COMMAND_NAME, Exc.what());
        l->error("Exception in project delete thread: {}", Exc.what());
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

udocs_processor::ProjectDeleteCommand::DeleteRequest
    udocs_processor::ProjectDeleteCLI::MakeRequest(
        const Arguments& Args) const {
  ProjectDeleteCommand::DeleteRequest Request;
  Request.DeleteProject = !Args.Version && !Args.DeleteAllVersionsOnly;
  Request.Version = Args.Version;
  Request.Token = Token->LoadToken(Args.Source);

  CliHelper::Location Location = CliHelper::ParseLocation(Args.Location);
  Request.Project = Location.Project;
  Request.Organization = Location.Organization;

  return Request;
}

void udocs_processor::ProjectDeleteCLI::SetView(
    std::unique_ptr<ProjectDeleteView> View) {
  this->CliView = std::move(View);
}

udocs_processor::ProjectDeleteCLI::ProjectDeleteCLI(
  std::shared_ptr<spdlog::sinks::sink> Sink,
  std::unique_ptr<ProjectDeleteCommand> Command,
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
