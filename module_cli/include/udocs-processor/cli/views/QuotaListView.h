/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "SimpleView.h"
#include "udocs-processor/services/QuotaService.h"

namespace udocs_processor {
class QuotaListView : public virtual SimpleView {
 public:
  virtual void ShowQuota(
      const std::vector<QuotaService::QuotaResource>& Resources) = 0;

  ~QuotaListView() override = default;
};
}  // namespace udocs_processor
