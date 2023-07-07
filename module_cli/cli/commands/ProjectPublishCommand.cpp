/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/ProjectPublishCommand.h"

udocs_processor::ProjectPublishCommand::ProjectPublishCommand(
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

void udocs_processor::ProjectPublishCommand::Publish(
    const PublishRequest &Request) const {
  ProjectService::PublishProjectRequest PublishRequest;
  PublishRequest.Location = {Request.Project, Request.Organization};
  PublishRequest.Token = Request.Token;
  PublishRequest.Scope = Request.Scope;

  ApiStatus Status = Service->Publish(PublishRequest);
  if (Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("Publish project: {}/{}", Status.GetCode(),
        Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't publish the project: {}",
        Status.GetMessageDescription())};
  }

  l->info("Successfuly published the project");
}
