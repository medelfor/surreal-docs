/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <string>
#include "udocs-processor/services/ProjectService.h"

namespace udocs_processor {
class ProjectCollaboratorDeleteCommand {
 public:
  struct DeleteRequest {
    std::string Organization;
    std::string Project;
    std::string Email;
    std::string Token;
  };

  ProjectCollaboratorDeleteCommand(std::shared_ptr<ProjectService> Service,
      std::shared_ptr<spdlog::sinks::sink> Sink);

  void Delete(const DeleteRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "project-collaborator-delete";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<ProjectService> Service;
};
}  // namespace udocs_processor
