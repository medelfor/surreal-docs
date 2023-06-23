/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "OrganizationDeleteView.h"

namespace udocs_processor {
class FtxOrganizationDeleteView : public virtual OrganizationDeleteView,
    public virtual FtxSimpleView {
 public:
  FtxOrganizationDeleteView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
