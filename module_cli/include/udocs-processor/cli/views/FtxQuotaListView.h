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

  void Init() override;

  void ShowQuota(
      const std::vector<QuotaService::QuotaResource> &Resources) override;

  ~FtxQuotaListView() override = default;

 private:
  static constexpr const char PROGRESS_MESSAGE[] = "Retrieving quota...";
  static constexpr const char LIMITED_RESOURCE_PATTERN[] =
      "{} {:>20} {:>10}/{}";
  static constexpr const char UNLIMITED_RESOURCE_PATTERN[] = "{} {:>20} {:>10}";
  static constexpr const char SUCCESS_MESSAGE[] =
      "Quota was successfully retrieved";
  static constexpr const char REACHED[] = "[ EXC ]";
  static constexpr const char ACTIVE[] =  "       ";
};
}  // namespace udocs_processor
