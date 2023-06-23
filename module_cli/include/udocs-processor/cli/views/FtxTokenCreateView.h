/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "TokenCreateView.h"

namespace udocs_processor {
class FtxTokenCreateView : public virtual TokenCreateView,
    public virtual FtxSimpleView {
 public:
  FtxTokenCreateView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
