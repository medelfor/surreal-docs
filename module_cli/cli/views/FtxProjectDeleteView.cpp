/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxProjectDeleteView.h"

void udocs_processor::FtxProjectDeleteView::Init() {
  FtxSimpleView::Init();
}

void udocs_processor::FtxProjectDeleteView::ReportProgress() {
  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}

void udocs_processor::FtxProjectDeleteView::ReportSuccess() {
  FtxSimpleView::ReportSuccess(SUCCESS_MESSAGE);
}

bool udocs_processor::FtxProjectDeleteView::Confirm() {
  FtxSimpleView::ReportProgress(GATHERING_INFO_MESSAGE);
  return FtxSimpleView::Confirm(CONFIRMATION_MESSAGE);
}
