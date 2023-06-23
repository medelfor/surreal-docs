/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "OrganizationListView.h"

namespace udocs_processor {
class FtxOrganizationListView : public virtual OrganizationListView,
    public virtual FtxSimpleView {
 public:
  FtxOrganizationListView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
