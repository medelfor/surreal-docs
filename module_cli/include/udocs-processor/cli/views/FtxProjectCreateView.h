/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "ProjectCreateView.h"

namespace udocs_processor {
class FtxProjectCreateView : public virtual ProjectCreateView,
    public virtual FtxSimpleView {
 public:
  FtxProjectCreateView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
