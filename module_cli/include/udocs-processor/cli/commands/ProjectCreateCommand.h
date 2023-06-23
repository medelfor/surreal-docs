/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>

namespace udocs_processor {
class ProjectCreateCommand {
 public:
  struct CreateRequest {};

  explicit ProjectCreateCommand(std::shared_ptr<spdlog::sinks::sink> Sink);

  void Create(const CreateRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "project-create";

  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor
