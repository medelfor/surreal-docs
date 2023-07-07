/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "SimpleView.h"

namespace udocs_processor {
class OrganizationDeleteView : public virtual SimpleView {
 public:
  ~OrganizationDeleteView() override = default;

  virtual void ReportProgress() = 0;

  virtual void ReportSuccess() = 0;

  virtual bool Confirm() = 0;
};
}  // namespace udocs_processor
