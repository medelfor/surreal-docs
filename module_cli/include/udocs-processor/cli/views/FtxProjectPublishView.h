/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "ProjectPublishView.h"

namespace udocs_processor {
class FtxProjectPublishView : public virtual ProjectPublishView,
    public virtual FtxSimpleView {
 public:
  FtxProjectPublishView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
