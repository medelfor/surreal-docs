/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "ProjectAnnounceView.h"

namespace udocs_processor {
class FtxProjectAnnounceView : public virtual ProjectAnnounceView,
    public virtual FtxSimpleView {
 public:
  FtxProjectAnnounceView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
