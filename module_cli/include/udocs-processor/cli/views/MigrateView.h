/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "SimpleView.h"

namespace udocs_processor {
class MigrateView : public virtual SimpleView {
 public:
  struct MigrationAction {
    std::string Description;
  };

  virtual void ReportProtocol(const std::vector<MigrationAction>& Protocol) = 0;

  ~MigrateView() override = default;
};
}  // namespace udocs_processor
