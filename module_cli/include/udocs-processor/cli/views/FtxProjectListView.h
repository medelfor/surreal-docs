/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "ProjectListView.h"

namespace udocs_processor {
class FtxProjectListView : public virtual ProjectListView,
    public virtual FtxSimpleView {
 public:
  FtxProjectListView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
