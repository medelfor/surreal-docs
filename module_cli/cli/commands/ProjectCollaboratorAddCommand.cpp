/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/ProjectCollaboratorAddCommand.h"

udocs_processor::ProjectCollaboratorAddCommand::ProjectCollaboratorAddCommand(
    std::shared_ptr<ProjectService> Service,
    std::shared_ptr<spdlog::sinks::sink> Sink) : Service(Service) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}

void udocs_processor::ProjectCollaboratorAddCommand::Add(
    const AddRequest &Request) const {
  ProjectService::AddCollaboratorRequest AddRequest;
  AddRequest.Location = {Request.Project, Request.Organization};
  AddRequest.Token = Request.Token;
  AddRequest.Email = Request.Email;

  ApiStatus Status = Service->AddCollaborator(AddRequest);
  if (Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("Add collaborator: {}/{}", Status.GetCode(),
        Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't add the collaborator: {}",
        Status.GetMessageDescription())};
  }

  l->info("Successfuly added the collaborator");
}
