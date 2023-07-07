/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxProjectCollaboratorDeleteView.h"

void udocs_processor::FtxProjectCollaboratorDeleteView::Init() {
  FtxSimpleView::Init();
}

void udocs_processor::FtxProjectCollaboratorDeleteView::ReportProgress() {
  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}

void udocs_processor::FtxProjectCollaboratorDeleteView::ReportSuccess() {
  FtxSimpleView::ReportSuccess(SUCCESS_MESSAGE);
}

bool udocs_processor::FtxProjectCollaboratorDeleteView::Confirm() {
  FtxSimpleView::ReportProgress(GATHERING_INFO_MESSAGE);
  return FtxSimpleView::Confirm(CONFIRMATION_MESSAGE);
}
