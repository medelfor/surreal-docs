/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxAuthView.h"

udocs_processor::FtxAuthView::FtxAuthView() {}

void udocs_processor::FtxAuthView::ReportSuccess() {
  FtxSimpleView::ReportSuccess(TOKEN_SUCCESS_MESSAGE);
}

void udocs_processor::FtxAuthView::Init() {
  FtxSimpleView::Init();
  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}
