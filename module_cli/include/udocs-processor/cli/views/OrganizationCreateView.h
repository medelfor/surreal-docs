/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "SimpleView.h"

namespace udocs_processor {
class OrganizationCreateView : public virtual SimpleView {
 public:
  ~OrganizationCreateView() override = default;

  virtual void ReportSuccess() = 0;
};
}  // namespace udocs_processor
