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
  FtxProjectDeleteView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
