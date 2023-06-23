/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/OrganizationCreateCommand.h"

udocs_processor::OrganizationCreateCommand::OrganizationCreateCommand(
    std::shared_ptr<spdlog::sinks::sink> Sink) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}

void udocs_processor::OrganizationCreateCommand::Create(
    const CreateRequest &Request) const {}