/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#define NOLINT()

#include "udocs-processor/cli/commands/OrganizationCreateCommand.h"
#include <spdlog/spdlog.h>
#include <regex> NOLINT()

udocs_processor::OrganizationCreateCommand::OrganizationCreateCommand(
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

void udocs_processor::OrganizationCreateCommand::Create(
    const CreateRequest &Request) const {
  static std::regex OrgNameRegex{ORGANIZATION_URL_PATTERN, std::regex::icase};
  std::smatch Match;
  if (!std::regex_match(Request.Name, Match, OrgNameRegex)) {
    throw std::invalid_argument{"Invalid name of organization"};
  }

  OrganizationService::CreateOrganizationRequest CreateRequest;
  CreateRequest.Name = Request.Name;
  CreateRequest.Token = Request.Token;

  ApiStatus Status = Service->Create(CreateRequest);
  if (Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("Create org: {}/{}", Status.GetCode(),
        Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't create the org: {}",
        Status.GetMessageDescription())};
  }

  l->info("Successfuly created org");
}
