/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/cli/ProjectCollaboratorListCLI.h"
#include <spdlog/spdlog.h>
#include <future> NOLINT()
#include <thread> NOLINT()
#include <chrono> NOLINT()
#include "udocs-processor/cli/cli/util/CliHelper.h"

bool udocs_processor::ProjectCollaboratorListCLI::ListProjectCollaborators(
    const Arguments &Args) const {
  Telemetry->ReportInit(TELEMETRY_COMMAND_NAME);

  CliView->Init();

  bool Success = true;
  auto Future = std::async(std::launch::async,
    [this, &Success, &Args]() {
      std::string OutDirectory;
      try {
        ProjectCollaboratorListCommand::ListRequest Request = MakeRequest(Args);

        CliView->ShowCollaboratos(Command->List(Request));
      } catch (const std::exception& Exc) {
        Success = false;
        Telemetry->ReportFail(TELEMETRY_COMMAND_NAME, Exc.what());
        l->error("Exception in project collaborator list thread: {}",
            Exc.what());
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

udocs_processor::ProjectCollaboratorListCommand::ListRequest
    udocs_processor::ProjectCollaboratorListCLI::MakeRequest(
        const Arguments& Args) const {
  ProjectCollaboratorListCommand::ListRequest Request;
  Request.Token = Token->LoadToken(Args.Source);

  CliHelper::Location Location = CliHelper::ParseLocation(Args.Location);
  Request.Project = Location.Project;
  Request.Organization = Location.Organization;

  return Request;
}

void udocs_processor::ProjectCollaboratorListCLI::SetView(
    std::unique_ptr<ProjectCollaboratorListView> View) {
  this->CliView = std::move(View);
}

udocs_processor::ProjectCollaboratorListCLI::ProjectCollaboratorListCLI(
  std::shared_ptr<spdlog::sinks::sink> Sink,
  std::unique_ptr<ProjectCollaboratorListCommand> Command,
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
