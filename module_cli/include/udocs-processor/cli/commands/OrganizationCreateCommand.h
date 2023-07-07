/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <string>
#include "udocs-processor/services/OrganizationService.h"

namespace udocs_processor {
class OrganizationCreateCommand {
 public:
  struct CreateRequest {
    std::string Name;
    std::string Token;
  };

  OrganizationCreateCommand(
      std::shared_ptr<OrganizationService> OrganizationService,
      std::shared_ptr<spdlog::sinks::sink> Sink);

  void Create(const CreateRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "organization-create";
  constexpr static char ORGANIZATION_URL_PATTERN[] = "[-a-zA-Z0-9_]{1,32}";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<OrganizationService> Service;
};
}  // namespace udocs_processor
