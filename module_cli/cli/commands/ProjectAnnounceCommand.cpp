/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/ProjectAnnounceCommand.h"

udocs_processor::ProjectAnnounceCommand::ProjectAnnounceCommand(
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

void udocs_processor::ProjectAnnounceCommand::Announce(
    const AnnounceRequest &Request) const {
  ProjectService::SetAnnouncementRequest SetRequest;
  SetRequest.Location = {Request.Project, Request.Organization};
  SetRequest.Token = Request.Token;
  SetRequest.Announcement = Request.Announcement;

  ApiStatus Status = Service->SetAnnouncement(SetRequest);
  if (Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("Set announcement project: {}/{}", Status.GetCode(),
        Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't set the announcement: {}",
        Status.GetMessageDescription())};
  }

  l->info("Successfuly set the announcement");
}
