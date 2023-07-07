/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxProjectPublishView.h"

void udocs_processor::FtxProjectPublishView::Init() {
  FtxSimpleView::Init();
  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}

void udocs_processor::FtxProjectPublishView::ReportSuccess() {
  FtxSimpleView::ReportSuccess(SUCCESS_MESSAGE);
}
