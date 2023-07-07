/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <vector>
#include <string>
#include "udocs-processor/services/ProjectService.h"

namespace udocs_processor {
class ProjectCollaboratorListCommand {
 public:
  struct ListRequest {
    std::string Organization;
    std::string Project;
    std::string Token;
  };

  ProjectCollaboratorListCommand(std::shared_ptr<ProjectService> Service,
      std::shared_ptr<spdlog::sinks::sink> Sink);

  std::vector<std::string> List(const ListRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "project-collaborator-list";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<ProjectService> Service;
};
}  // namespace udocs_processor
