/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <udocs-processor/ApiStatus.h>
#include <map>
#include <vector>
#include <string>

namespace udocs_processor {
class TokenService {
 public:
  struct StrippedTokenData {
    uint64_t ExpiresAt = 0;
    std::vector<std::string> Scopes;
  };

  struct TokenData {
    StrippedTokenData Basic;
    std::string Token;
  };

  struct CreateTokenRequest {
    std::string HydraAccessToken;
    std::vector<std::string> Scopes;
  };

  struct CreateTokenResponse {
    ApiStatus Status;
    TokenData Token;
  };

  struct ListTokensResponse {
    ApiStatus Status;
    std::vector<StrippedTokenData> Tokens;
  };

  virtual CreateTokenResponse Create(
      const CreateTokenRequest& Request) const = 0;

  virtual ListTokensResponse List(
      const std::string& HydraAccessToken) const = 0;
};
}  // namespace udocs_processor
