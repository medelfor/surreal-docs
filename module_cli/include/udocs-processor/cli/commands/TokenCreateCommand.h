/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>

namespace udocs_processor {
class TokenCreateCommand {
 public:
  struct CreateRequest {};

  explicit TokenCreateCommand(std::shared_ptr<spdlog::sinks::sink> Sink);

  void Create(const CreateRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "token-create";

  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor
