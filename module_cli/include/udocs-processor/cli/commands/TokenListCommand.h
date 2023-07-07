/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <vector>
#include "udocs-processor/cli/commands/oauth2/OAuth2.h"
#include "udocs-processor/services/TokenService.h"

namespace udocs_processor {
class TokenListCommand {
 public:
  struct ListRequest {};

  TokenListCommand(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::shared_ptr<OAuth2> Oauth2,
      std::shared_ptr<TokenService> Token);

  std::vector<udocs_processor::TokenService::StrippedTokenData> List(
      const ListRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "token-list";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<OAuth2> Oauth2;
  std::shared_ptr<TokenService> Token;
};
}  // namespace udocs_processor
