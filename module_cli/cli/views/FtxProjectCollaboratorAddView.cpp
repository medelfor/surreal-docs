/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxProjectCollaboratorAddView.h"

void udocs_processor::FtxProjectCollaboratorAddView::Init() {
  FtxSimpleView::Init();
  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}

void udocs_processor::FtxProjectCollaboratorAddView::ReportSuccess() {
  FtxSimpleView::ReportSuccess(SUCCESS_MESSAGE);
}
