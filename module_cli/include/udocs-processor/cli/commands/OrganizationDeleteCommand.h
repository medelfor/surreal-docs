/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <string>
#include "udocs-processor/services/OrganizationService.h"

namespace udocs_processor {
class OrganizationDeleteCommand {
 public:
  struct DeleteRequest {
    std::string Name;
    std::string Token;
  };

  OrganizationDeleteCommand(
      std::shared_ptr<OrganizationService> OrganizationService,
      std::shared_ptr<spdlog::sinks::sink> Sink);

  void Delete(const DeleteRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "organization-delete";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<OrganizationService> Service;
};
}  // namespace udocs_processor
