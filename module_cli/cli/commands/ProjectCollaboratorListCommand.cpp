/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/ProjectCollaboratorListCommand.h"

udocs_processor::ProjectCollaboratorListCommand::ProjectCollaboratorListCommand(
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

std::vector<std::string> udocs_processor::ProjectCollaboratorListCommand::List(
    const ListRequest &Request) const {
  ProjectService::ListCollaboratorsRequest ListRequest;
  ListRequest.Token = Request.Token;
  ListRequest.Location = {Request.Project, Request.Organization};

  ProjectService::ListCollaboratorsResponse Response =
      Service->ListCollaborators(ListRequest);
  if (Response.Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("List collabs: {}/{}", Response.Status.GetCode(),
        Response.Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't list the collabs: {}",
        Response.Status.GetMessageDescription())};
  }

  l->info("Successfuly listed collabs");

  return Response.Emails;
}
