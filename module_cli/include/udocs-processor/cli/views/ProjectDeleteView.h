/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "SimpleView.h"

namespace udocs_processor {
class ProjectDeleteView : public virtual SimpleView {
 public:
  virtual void ReportProgress() = 0;

  virtual void ReportSuccess() = 0;

  virtual bool Confirm() = 0;

  ~ProjectDeleteView() override = default;
};
}  // namespace udocs_processor
