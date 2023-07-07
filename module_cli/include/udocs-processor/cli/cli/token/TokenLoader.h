/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <string>
#include <memory>
#include "udocs-processor/cli/commands/security/Vault.h"
#include "udocs-processor/cli/StandardInputReader.h"

namespace udocs_processor {
class TokenLoader {
 public:
  enum class TokenSource {
    NO_PREFERENCE,
    ENVIRONMENT,
    STDINPUT,
    VAULT
  };

  TokenLoader(std::shared_ptr<Vault> SecureVault,
      std::shared_ptr<StandarInputReader> StdIn);

  virtual std::string LoadToken(TokenSource Source) const;

  static constexpr const char ENVIRONMENT_VAR[] = "SURDOCS_TOKEN";
  static constexpr const char VAULT_SURREAL_DOCS_ID[] =
      "Surreal Docs CLI/Surreal Cloud Token";

 private:
  bool CheckFormat(const std::string& Token) const;

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<Vault> SecureVault;
  std::shared_ptr<StandarInputReader> StdIn;

  static constexpr const char TOKEN_PREFIX[] = "cld_";
  static constexpr size_t TOKEN_PAYLOAD_SIZE = 36;
};
}  // namespace udocs_processor
