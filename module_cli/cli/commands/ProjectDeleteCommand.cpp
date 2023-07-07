/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/ProjectDeleteCommand.h"

udocs_processor::ProjectDeleteCommand::ProjectDeleteCommand(
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

void udocs_processor::ProjectDeleteCommand::Delete(
    const DeleteRequest &Request) const {
  ProjectService::DeleteProjectRequest DeleteRequest;
  DeleteRequest.Location = {Request.Project, Request.Organization};
  DeleteRequest.Token = Request.Token;
  DeleteRequest.Version = Request.Version;
  DeleteRequest.DeleteProject = Request.DeleteProject;

  ApiStatus Status = Service->Delete(DeleteRequest);
  if (Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("Delete project: {}/{}", Status.GetCode(),
        Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't delete the project: {}",
        Status.GetMessageDescription())};
  }

  l->info("Successfuly deleted project");
}
