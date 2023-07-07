/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/cli/token/TokenLoader.h"
#include "udocs-processor/StringHelper.h"

udocs_processor::TokenLoader::TokenLoader(std::shared_ptr<Vault> SecureVault,
      std::shared_ptr<StandarInputReader> StdIn) : SecureVault(SecureVault),
      StdIn(StdIn) {}

std::string udocs_processor::TokenLoader::LoadToken(TokenSource Source) const {
  if (Source == TokenSource::NO_PREFERENCE || Source == TokenSource::VAULT) {
    bool Loaded = false;
    std::string Token;
    try {
      Token = SecureVault->LoadSecret(VAULT_SURREAL_DOCS_ID);
      Loaded = true;
    } catch (const std::exception& Exc) {}
    if (Loaded && CheckFormat(Token)) {
      return Token;
    } else if (Source != TokenSource::NO_PREFERENCE) {
      throw std::invalid_argument{"No valid token in the vault"};
    }
  }

  if (Source == TokenSource::STDINPUT) {
    std::string Input = StdIn->ReadInput();
    StringHelper::TrimAllWhitespaces(Input);
    if (CheckFormat(Input)) {
      return Input;
    } else {
      throw std::invalid_argument{"No valid token in the stdin"};
    }
  }

  if (Source == TokenSource::NO_PREFERENCE ||
      Source == TokenSource::ENVIRONMENT) {
    const char* Token = std::getenv(ENVIRONMENT_VAR);
    if (Token && CheckFormat(Token)) {
      return Token;
    }
  }

  throw std::invalid_argument{"No valid token found"};
}

bool udocs_processor::TokenLoader::CheckFormat(
    const std::string &Token) const {
  if (Token.size() != strlen(TOKEN_PREFIX) + TOKEN_PAYLOAD_SIZE ||
      Token.rfind(TOKEN_PREFIX, 0) != 0) {
    return false;
  }

  return true;
}
