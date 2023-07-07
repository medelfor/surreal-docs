/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "ProjectAnnounceView.h"

namespace udocs_processor {
class FtxProjectAnnounceView : public virtual ProjectAnnounceView,
    public virtual FtxSimpleView {
 public:
  FtxProjectAnnounceView() = default;

  void Init() override;

  void ReportSuccess() override;

  ~FtxProjectAnnounceView() override = default;

 private:
  static constexpr const char* SUCCESS_MESSAGE =
      "The announcement was successfuly made";
  static constexpr const char* PROGRESS_MESSAGE =
      "Announcing...";
};
}  // namespace udocs_processor
