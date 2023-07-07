/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <vector>
#include <string>
#include "udocs-processor/services/OrganizationService.h"

namespace udocs_processor {
class OrganizationListCommand {
 public:
  struct ListRequest {
    std::string Token;
  };

  OrganizationListCommand(
      std::shared_ptr<OrganizationService> OrganizationService,
      std::shared_ptr<spdlog::sinks::sink> Sink);

  std::vector<std::string> List(const ListRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "organization-list";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<OrganizationService> Service;
};
}  // namespace udocs_processor
