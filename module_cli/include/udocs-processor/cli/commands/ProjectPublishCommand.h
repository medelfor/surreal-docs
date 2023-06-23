/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>

namespace udocs_processor {
class ProjectPublishCommand {
 public:
  struct PublishRequest {};

  explicit ProjectPublishCommand(std::shared_ptr<spdlog::sinks::sink> Sink);

  void Publish(const PublishRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "project-publish";

  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor
