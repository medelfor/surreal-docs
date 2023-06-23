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
  FtxProjectCollaboratorAddView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
