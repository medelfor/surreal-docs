/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "ProjectDeleteView.h"

namespace udocs_processor {
class FtxProjectDeleteView : public virtual ProjectDeleteView,
    public virtual FtxSimpleView {
 public:
  FtxProjectDeleteView() = default;

  void Init() override;

  void ReportProgress() override;

  void ReportSuccess() override;

  bool Confirm() override;

  ~FtxProjectDeleteView() override = default;

 private:
  static constexpr const char* CONFIRMATION_MESSAGE = "Are you sure you want "
      "to delete the project/version(s)? It'll permanently delete all the "
      "documents within it";
  static constexpr const char* SUCCESS_MESSAGE =
      "The project was successfuly deleted";
  static constexpr const char* PROGRESS_MESSAGE =
      "Deleting the project...";
  static constexpr const char* GATHERING_INFO_MESSAGE =
      "Confirming choice...";
};
}  // namespace udocs_processor
