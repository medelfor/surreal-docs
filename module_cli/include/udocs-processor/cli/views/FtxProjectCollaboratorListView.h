/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "ProjectCollaboratorListView.h"

namespace udocs_processor {
class FtxProjectCollaboratorListView
    : public virtual ProjectCollaboratorListView, public virtual FtxSimpleView {
 public:
  FtxProjectCollaboratorListView() = default;

  void Init() override;

  void ShowCollaboratos(std::vector<std::string> Collaborators) override;

  ~FtxProjectCollaboratorListView() override = default;

 private:
  static constexpr const char* SUCCESS_MESSAGE =
      "The project collaborator(s) were successfuly retrieved";
  static constexpr const char* PROGRESS_MESSAGE =
      "Retrieving the collaborators...";
  static constexpr const char* COLLABORATOR_FORMAT = " {}";
};
}  // namespace udocs_processor
