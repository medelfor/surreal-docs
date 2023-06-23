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
  FtxProjectCollaboratorListView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
