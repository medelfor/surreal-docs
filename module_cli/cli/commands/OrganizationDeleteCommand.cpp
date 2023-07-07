/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/OrganizationDeleteCommand.h"

udocs_processor::OrganizationDeleteCommand::OrganizationDeleteCommand(
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

void udocs_processor::OrganizationDeleteCommand::Delete(
    const DeleteRequest &Request) const {
  OrganizationService::DeleteOrganizationRequest DeleteRequest;
  DeleteRequest.Name = Request.Name;
  DeleteRequest.Token = Request.Token;

  ApiStatus Status = Service->Delete(DeleteRequest);
  if (Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("Delete org: {}/{}", Status.GetCode(),
        Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't delete the org: {}",
        Status.GetMessageDescription())};
  }

  l->info("Successfuly deleted org");
}
