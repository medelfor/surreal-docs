/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>

namespace udocs_processor {
class ProjectCollaboratorDeleteCommand {
 public:
  struct DeleteRequest {};

  explicit ProjectCollaboratorDeleteCommand(
      std::shared_ptr<spdlog::sinks::sink> Sink);

  void Delete(const DeleteRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "project-collaborator-delete";

  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor
