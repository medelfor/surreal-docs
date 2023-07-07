/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxOrganizationCreateView.h"

udocs_processor::FtxOrganizationCreateView::FtxOrganizationCreateView() {}

void udocs_processor::FtxOrganizationCreateView::Init() {
  FtxSimpleView::Init();
  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}

void udocs_processor::FtxOrganizationCreateView::ReportSuccess() {
  FtxSimpleView::ReportSuccess(SUCCESS_MESSAGE);
}
