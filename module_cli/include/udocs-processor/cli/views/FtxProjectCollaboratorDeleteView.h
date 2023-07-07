/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "ProjectCollaboratorDeleteView.h"

namespace udocs_processor {
class FtxProjectCollaboratorDeleteView
    : public virtual ProjectCollaboratorDeleteView,
    public virtual FtxSimpleView {
 public:
  FtxProjectCollaboratorDeleteView() = default;

  void Init() override;

  void ReportProgress() override;

  void ReportSuccess() override;

  bool Confirm() override;

  ~FtxProjectCollaboratorDeleteView() override = default;

 private:
  static constexpr const char* CONFIRMATION_MESSAGE = "Are you sure you want "
      "to remove the collaborator? They will lose read access to the project";
  static constexpr const char* SUCCESS_MESSAGE =
      "The collaborator was successfuly removed";
  static constexpr const char* PROGRESS_MESSAGE =
      "Removing the collaborator...";
  static constexpr const char* GATHERING_INFO_MESSAGE =
      "Confirming choice...";
};
}  // namespace udocs_processor
