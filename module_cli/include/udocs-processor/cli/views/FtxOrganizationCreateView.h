/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "OrganizationCreateView.h"

namespace udocs_processor {
class FtxOrganizationCreateView : public virtual OrganizationCreateView,
    public virtual FtxSimpleView {
 public:
  FtxOrganizationCreateView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
