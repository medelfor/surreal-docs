/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "SimpleView.h"

namespace udocs_processor {
class TokenCreateView : public virtual SimpleView {
 public:
  virtual void ShowToken(const std::string& Token, uint64_t ExpiresOn) = 0;

  ~TokenCreateView() override = default;
};
}  // namespace udocs_processor
