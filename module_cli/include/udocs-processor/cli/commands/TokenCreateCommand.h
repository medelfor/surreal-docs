/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include "udocs-processor/cli/commands/oauth2/OAuth2.h"
#include "udocs-processor/services/TokenService.h"

namespace udocs_processor {
class TokenCreateCommand {
 public:
  struct CreateRequest {};

  TokenCreateCommand(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::shared_ptr<OAuth2> Oauth2,
      std::shared_ptr<TokenService> Token);

  TokenService::TokenData Create(const CreateRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "token-create";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<OAuth2> Oauth2;
  std::shared_ptr<TokenService> Token;
};
}  // namespace udocs_processor
