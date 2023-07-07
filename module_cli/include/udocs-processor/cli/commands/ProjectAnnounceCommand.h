/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <string>
#include "udocs-processor/services/ProjectService.h"

namespace udocs_processor {
class ProjectAnnounceCommand {
 public:
  struct AnnounceRequest {
    std::string Organization;
    std::string Project;
    std::string Announcement;
    std::string Token;
  };

  ProjectAnnounceCommand(std::shared_ptr<ProjectService> Service,
      std::shared_ptr<spdlog::sinks::sink> Sink);

  void Announce(const AnnounceRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "project-announce";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<ProjectService> Service;
};
}  // namespace udocs_processor
