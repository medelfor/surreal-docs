/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <string>
#include "udocs-processor/services/ProjectService.h"

namespace udocs_processor {
class ProjectCollaboratorAddCommand {
 public:
  struct AddRequest {
    std::string Organization;
    std::string Project;
    std::string Email;
    std::string Token;
  };

  ProjectCollaboratorAddCommand(std::shared_ptr<ProjectService> Service,
      std::shared_ptr<spdlog::sinks::sink> Sink);

  void Add(const AddRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "project-collaborator-add";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<ProjectService> Service;
};
}  // namespace udocs_processor
