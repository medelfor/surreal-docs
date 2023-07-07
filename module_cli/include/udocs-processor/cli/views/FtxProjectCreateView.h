/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "ProjectCreateView.h"

namespace udocs_processor {
class FtxProjectCreateView : public virtual ProjectCreateView,
    public virtual FtxSimpleView {
 public:
  FtxProjectCreateView() = default;

  void Init() override;

  void ReportSuccess() override;

  ~FtxProjectCreateView() override = default;

 private:
  static constexpr const char* SUCCESS_MESSAGE =
      "The project was successfuly created";
  static constexpr const char* PROGRESS_MESSAGE =
      "Creating the project...";
};
}  // namespace udocs_processor
