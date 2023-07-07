/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxTokenCreateView.h"

void udocs_processor::FtxTokenCreateView::ShowToken(const std::string& Token,
    uint64_t ExpiresOn) {
  FtxSimpleView::ReportSuccess(TOKEN_SUCCESS_MESSAGE);
  FtxSimpleView::ShowDescription(fmt::format(TOKEN_IS, Token));
}

udocs_processor::FtxTokenCreateView::FtxTokenCreateView() {}

void udocs_processor::FtxTokenCreateView::Init() {
  FtxSimpleView::Init();

  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}
