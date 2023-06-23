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
  FtxProjectCollaboratorDeleteView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
