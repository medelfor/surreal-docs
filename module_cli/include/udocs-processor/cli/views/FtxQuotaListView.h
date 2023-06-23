/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "QuotaListView.h"

namespace udocs_processor {
class FtxQuotaListView : public virtual QuotaListView,
    public virtual FtxSimpleView {
 public:
  FtxQuotaListView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
