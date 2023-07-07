/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "ProjectCollaboratorAddView.h"

namespace udocs_processor {
class FtxProjectCollaboratorAddView : public virtual ProjectCollaboratorAddView,
    public virtual FtxSimpleView {
 public:
  FtxProjectCollaboratorAddView() = default;

  void Init() override;

  void ReportSuccess() override;

  ~FtxProjectCollaboratorAddView() override = default;

 private:
  static constexpr const char* SUCCESS_MESSAGE =
      "The collaborator was successfuly added to the project";
  static constexpr const char* PROGRESS_MESSAGE =
      "Adding the collaborator...";
};
}  // namespace udocs_processor
