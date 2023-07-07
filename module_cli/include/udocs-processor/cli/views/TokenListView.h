/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "SimpleView.h"
#include "udocs-processor/services/TokenService.h"

namespace udocs_processor {
class TokenListView : public virtual SimpleView {
 public:
  virtual void ShowTokens(
      const std::vector<TokenService::StrippedTokenData>& Tokens) = 0;

  ~TokenListView() override = default;
};
}  // namespace udocs_processor
