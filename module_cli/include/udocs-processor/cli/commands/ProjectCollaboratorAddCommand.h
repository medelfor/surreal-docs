/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>

namespace udocs_processor {
class ProjectCollaboratorAddCommand {
 public:
  struct AddRequest {};

  explicit ProjectCollaboratorAddCommand(
      std::shared_ptr<spdlog::sinks::sink> Sink);

  void Add(const AddRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "project-collaborator-add";

  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor
