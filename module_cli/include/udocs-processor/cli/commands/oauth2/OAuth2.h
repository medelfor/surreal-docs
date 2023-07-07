/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <set>
#include <string>

namespace udocs_processor {
class OAuth2 {
 public:
  struct OAuth2Access {
    bool IsAuthorized = false;
    std::set<std::string> Scopes;
    std::string AccessToken;
  };

  virtual OAuth2Access Authorize(
      const std::set<std::string>& Scopes) = 0;
};
}  // namespace udocs_processor
