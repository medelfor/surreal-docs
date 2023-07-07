/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <string>
#include "udocs-processor/services/ProjectService.h"

namespace udocs_processor {
class ProjectCreateCommand {
 public:
  struct CreateRequest {
    std::string Organization;
    std::string Project;
    std::string Token;
  };

  ProjectCreateCommand(std::shared_ptr<ProjectService> Service,
      std::shared_ptr<spdlog::sinks::sink> Sink);

  void Create(const CreateRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "project-create";
  constexpr static char PROJECT_URL_PATTERN[] = "[-a-zA-Z0-9_]{1,32}";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<ProjectService> Service;
};
}  // namespace udocs_processor
