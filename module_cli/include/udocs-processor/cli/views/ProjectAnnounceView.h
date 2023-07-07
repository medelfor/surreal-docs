/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "SimpleView.h"

namespace udocs_processor {
class ProjectAnnounceView : public virtual SimpleView {
 public:
  virtual void ReportSuccess() = 0;

  ~ProjectAnnounceView() override = default;
};
}  // namespace udocs_processor
