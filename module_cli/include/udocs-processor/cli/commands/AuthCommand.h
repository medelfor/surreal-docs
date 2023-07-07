/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <string>
#include "udocs-processor/cli/commands/security/Vault.h"

namespace udocs_processor {
class AuthCommand {
 public:
  struct AuthRequest {
    std::string Token;
  };

  AuthCommand(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::shared_ptr<Vault> SecureVault);

  void Auth(const AuthRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "auth";
  static constexpr const char SURREAL_DOCS_DEFAULT_USER[] = "Default Token";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<Vault> SecureVault;
};
}  // namespace udocs_processor
