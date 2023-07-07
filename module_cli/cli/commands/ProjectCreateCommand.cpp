/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#define NOLINT()

#include "udocs-processor/cli/commands/ProjectCreateCommand.h"
#include <spdlog/spdlog.h>
#include <regex> NOLINT()

udocs_processor::ProjectCreateCommand::ProjectCreateCommand(
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

void udocs_processor::ProjectCreateCommand::Create(
    const CreateRequest &Request) const {
  static std::regex ProjectNameRegex{PROJECT_URL_PATTERN, std::regex::icase};
  std::smatch Match;
  if (!std::regex_match(Request.Project, Match, ProjectNameRegex)) {
    throw std::invalid_argument{"Invalid name of project"};
  }

  ProjectService::CreateProjectRequest CreateRequest;
  CreateRequest.Location = {Request.Project, Request.Organization};
  CreateRequest.Token = Request.Token;

  ApiStatus Status = Service->Create(CreateRequest);
  if (Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("Create project: {}/{}", Status.GetCode(),
        Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't create the project: {}",
        Status.GetMessageDescription())};
  }

  l->info("Successfuly created project");
}
