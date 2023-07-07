/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "ProjectPublishView.h"

namespace udocs_processor {
class FtxProjectPublishView : public virtual ProjectPublishView,
    public virtual FtxSimpleView {
 public:
  FtxProjectPublishView() = default;

  void Init() override;

  void ReportSuccess() override;

  ~FtxProjectPublishView() override = default;

 private:
  static constexpr const char* SUCCESS_MESSAGE =
      "The project was successfuly published";
  static constexpr const char* PROGRESS_MESSAGE =
      "Publishing the project...";
};
}  // namespace udocs_processor
