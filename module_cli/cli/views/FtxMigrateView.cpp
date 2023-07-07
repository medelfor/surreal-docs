/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxMigrateView.h"

void udocs_processor::FtxMigrateView::Init() {
  FtxSimpleView::Init();
  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}

void udocs_processor::FtxMigrateView::ReportProtocol(
    const std::vector<MigrationAction> &Protocol) {
  FtxSimpleView::ReportSuccess(Protocol.empty()
      ? NOTHING_TO_UPDATE_MESSAGE
      : SUCCESS_MESSAGE);

  if (!Protocol.empty()) {
    std::vector<std::string> List;
    List.emplace_back(PROTOCOL_HEADING);
    List.emplace_back("");

    for (const auto &Item : Protocol) {
      List.emplace_back(fmt::format(ACTION_PATTERN, Item.Description));
    }

    FtxSimpleView::ShowList(std::move(List));
  }
}
