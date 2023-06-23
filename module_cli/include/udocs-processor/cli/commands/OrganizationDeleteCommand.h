/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>

namespace udocs_processor {
class OrganizationDeleteCommand {
 public:
  struct DeleteRequest {};

  explicit OrganizationDeleteCommand(std::shared_ptr<spdlog::sinks::sink> Sink);

  void Delete(const DeleteRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "organization-delete";

  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor
