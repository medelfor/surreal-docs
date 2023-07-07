/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxOrganizationListView.h"

udocs_processor::FtxOrganizationListView::FtxOrganizationListView() {}

void udocs_processor::FtxOrganizationListView::Init() {
  FtxSimpleView::Init();
  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}

void udocs_processor::FtxOrganizationListView::ShowOrganizations(
    const std::vector<std::string> &Organizations) {
  FtxSimpleView::ReportSuccess(SUCCESS_MESSAGE);
  std::vector<std::string> List;
  for (const auto& Org : Organizations) {
    List.emplace_back(fmt::format(ORGANIZATION_FORMAT, Org));
  }
  FtxSimpleView::ShowList(std::move(List));
}
