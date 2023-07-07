/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxQuotaListView.h"

void udocs_processor::FtxQuotaListView::ShowQuota(
    const std::vector<QuotaService::QuotaResource> &Resources) {
  std::vector<std::string> Items;
  Items.reserve(Resources.size());
  for (const auto& Resource : Resources) {
    std::string Item;
    if (Resource.Max) {
      bool HasReached = Resource.Used >= *Resource.Max;
      Item = fmt::format(LIMITED_RESOURCE_PATTERN,
          HasReached ? REACHED : ACTIVE, Resource.Name, Resource.Used,
          *Resource.Max);
    } else {
      Item = fmt::format(UNLIMITED_RESOURCE_PATTERN, ACTIVE, Resource.Name,
          Resource.Used);
    }

    Items.emplace_back(std::move(Item));
  }
  FtxSimpleView::ReportSuccess(SUCCESS_MESSAGE);
  FtxSimpleView::ShowList(std::move(Items));
}

udocs_processor::FtxQuotaListView::FtxQuotaListView() {}

void udocs_processor::FtxQuotaListView::Init() {
  FtxSimpleView::Init();
  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}
