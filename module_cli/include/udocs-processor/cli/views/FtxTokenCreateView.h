/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "TokenCreateView.h"

namespace udocs_processor {
class FtxTokenCreateView : public virtual TokenCreateView,
    public virtual FtxSimpleView {
 public:
  FtxTokenCreateView();

  void ShowToken(const std::string& Token, uint64_t ExpiresOn) override;

  void Init() override;

  ~FtxTokenCreateView() override = default;

 private:
  static constexpr const char PROGRESS_MESSAGE[] = "Issuing token...";
  static constexpr const char TOKEN_IS[] = "{}";
  static constexpr const char TOKEN_SUCCESS_MESSAGE[] =
      "Token was successfully acquired";
};
}  // namespace udocs_processor
