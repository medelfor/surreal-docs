/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/AuthCommand.h"
#include "udocs-processor/cli/cli/token/TokenLoader.h"

udocs_processor::AuthCommand::AuthCommand(
    std::shared_ptr<spdlog::sinks::sink> Sink,
    std::shared_ptr<Vault> SecureVault) : SecureVault(SecureVault) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}

void udocs_processor::AuthCommand::Auth(const AuthRequest &Request) const {
  SecureVault->SaveSecret(TokenLoader::VAULT_SURREAL_DOCS_ID,
      SURREAL_DOCS_DEFAULT_USER, Request.Token);
}
