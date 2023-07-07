/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxOrganizationDeleteView.h"

udocs_processor::FtxOrganizationDeleteView::FtxOrganizationDeleteView() {}

void udocs_processor::FtxOrganizationDeleteView::ReportSuccess() {
  FtxSimpleView::ReportSuccess(SUCCESS_MESSAGE);
}

bool udocs_processor::FtxOrganizationDeleteView::Confirm() {
  FtxSimpleView::ReportProgress(GATHERING_INFO_MESSAGE);
  return FtxSimpleView::Confirm(CONFIRMATION_MESSAGE);
}

void udocs_processor::FtxOrganizationDeleteView::Init() {
  FtxSimpleView::Init();
}

void udocs_processor::FtxOrganizationDeleteView::ReportProgress() {
  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}
