/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "SimpleView.h"

namespace udocs_processor {
class ProjectCollaboratorListView : public virtual SimpleView {
 public:
  virtual void ShowCollaboratos(std::vector<std::string> Collaborators) = 0;

  ~ProjectCollaboratorListView() override = default;
};
}  // namespace udocs_processor
