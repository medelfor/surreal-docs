/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/ProjectListCommand.h"

udocs_processor::ProjectListCommand::ProjectListCommand(
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

std::vector<udocs_processor::ProjectService::ProjectData>
    udocs_processor::ProjectListCommand::List(
      const ListRequest &Request) const {
  ProjectService::ListProjectsRequest ListRequest;
  ListRequest.Token = Request.Token;
  ListRequest.Organization = Request.Organization;

  ProjectService::ListProjectsResponse Response = Service->List(ListRequest);
  if (Response.Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("List projects: {}/{}", Response.Status.GetCode(),
        Response.Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't list the projects: {}",
        Response.Status.GetMessageDescription())};
  }

  l->info("Successfuly listed projects");

  return Response.Projects;
}
