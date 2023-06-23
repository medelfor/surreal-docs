/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "TokenListView.h"

namespace udocs_processor {
class FtxTokenListView : public virtual TokenListView,
    public virtual FtxSimpleView {
 public:
  FtxTokenListView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
