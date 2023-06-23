/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>

namespace udocs_processor {
class OrganizationCreateCommand {
 public:
  struct CreateRequest {};

  explicit OrganizationCreateCommand(std::shared_ptr<spdlog::sinks::sink> Sink);

  void Create(const CreateRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "organization-create";

  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor
