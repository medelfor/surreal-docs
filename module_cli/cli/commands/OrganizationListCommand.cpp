/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/OrganizationListCommand.h"

udocs_processor::OrganizationListCommand::OrganizationListCommand(
    std::shared_ptr<OrganizationService> OrganizationService,
    std::shared_ptr<spdlog::sinks::sink> Sink) : Service(OrganizationService) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}

std::vector<std::string> udocs_processor::OrganizationListCommand::List(
    const ListRequest &Request) const {
  OrganizationService::ListOrganizationsRequest ListRequest;
  ListRequest.Token = Request.Token;

  OrganizationService::ListOrganizationsResponse Response =
      Service->List(ListRequest);
  if (Response.Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("List orgs: {}/{}", Response.Status.GetCode(),
        Response.Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't list the orgs: {}",
        Response.Status.GetMessageDescription())};
  }

  l->info("Successfuly listed orgs");

  return Response.Organizations;
}
