/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>

namespace udocs_processor {
class ProjectAnnounceCommand {
 public:
  struct AnnounceRequest {};

  explicit ProjectAnnounceCommand(std::shared_ptr<spdlog::sinks::sink> Sink);

  void Announce(const AnnounceRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "project-announce";

  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor
