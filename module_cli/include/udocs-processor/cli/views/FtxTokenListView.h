/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "TokenListView.h"

namespace udocs_processor {
class FtxTokenListView : public virtual TokenListView,
    public virtual FtxSimpleView {
 public:
  FtxTokenListView();

  void ShowTokens(
      const std::vector<TokenService::StrippedTokenData> &Tokens) override;

  void Init() override;

  ~FtxTokenListView() override = default;

 private:
  static constexpr const char PROGRESS_MESSAGE[] = "Retrieving tokens...";
  static constexpr const char SCOPES_GLUE[] = ", ";
  static constexpr const char TOKEN_PATTERN[] = "{} {:>4} Expires {:>20} [{}]";
  static constexpr const char TOKEN_SUCCESS_MESSAGE[] =
      "Token(s) were successfully retrieved";
  static constexpr const char EXPIRED[] = "[ EXPIR. ]";
  static constexpr const char ACTIVE[] =  "[ ACTIVE ]";
};
}  // namespace udocs_processor
