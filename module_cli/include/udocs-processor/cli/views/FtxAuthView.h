/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "AuthView.h"
#include "FtxSimpleView.h"

namespace udocs_processor {
class FtxAuthView : public virtual AuthView, public virtual FtxSimpleView {
 public:
  FtxAuthView();

  void ReportSuccess() override;

  void Init() override;

  ~FtxAuthView() override = default;

 private:
  static constexpr const char PROGRESS_MESSAGE[] =
      "Storing the token in the vault";
  static constexpr const char TOKEN_SUCCESS_MESSAGE[] =
      "Token was successfully stored";
};
}  // namespace udocs_processor
