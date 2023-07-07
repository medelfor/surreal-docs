/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <string>
#include "udocs-processor/services/ProjectService.h"

namespace udocs_processor {
class ProjectPublishCommand {
 public:
  struct PublishRequest {
    std::string Organization;
    std::string Project;
    enum class ProjectService::PublishProjectRequest::Scope Scope =
        ProjectService::PublishProjectRequest::Scope::PUBLIC;
    std::string Token;
  };

  ProjectPublishCommand(std::shared_ptr<ProjectService> Service,
      std::shared_ptr<spdlog::sinks::sink> Sink);

  void Publish(const PublishRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "project-publish";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<ProjectService> Service;
};
}  // namespace udocs_processor
