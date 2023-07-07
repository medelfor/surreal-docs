/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/ProjectCollaboratorDeleteCommand.h"

udocs_processor::ProjectCollaboratorDeleteCommand::
    ProjectCollaboratorDeleteCommand(
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

void udocs_processor::ProjectCollaboratorDeleteCommand::Delete(
    const DeleteRequest &Request) const {
  ProjectService::RemoveCollaboratorRequest RemoveRequest;
  RemoveRequest.Location = {Request.Project, Request.Organization};
  RemoveRequest.Token = Request.Token;
  RemoveRequest.Email = Request.Email;

  ApiStatus Status = Service->RemoveCollaborator(RemoveRequest);
  if (Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("Remove collaborator: {}/{}", Status.GetCode(),
        Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't remove the collaborator: {}",
        Status.GetMessageDescription())};
  }

  l->info("Successfuly removed the collaborator");
}
