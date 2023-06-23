/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>

namespace udocs_processor {
class AuthCommand {
 public:
  struct AuthRequest {};

  explicit AuthCommand(std::shared_ptr<spdlog::sinks::sink> Sink);

  void Auth(const AuthRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "auth";

  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor
