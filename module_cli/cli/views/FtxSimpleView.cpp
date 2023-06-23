/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxSimpleView.h"

void udocs_processor::FtxSimpleView::Init() {
  FtxView::Init();
}

void udocs_processor::FtxSimpleView::Tick() {
  FtxView::Tick();
}

void udocs_processor::FtxSimpleView::Start() {
  View::Start();
}

void udocs_processor::FtxSimpleView::Destroy() {

}

bool udocs_processor::FtxSimpleView::DoExit() const {
  return false;
}

void udocs_processor::FtxSimpleView::SetFinished(bool IsFinished) {

}

void udocs_processor::FtxSimpleView::ReportSuccess(const std::string &Message) {

}

void udocs_processor::FtxSimpleView::ReportError(const std::string &Message) {

}

void udocs_processor::FtxSimpleView::ReportWarning(const std::string &Message) {

}

void udocs_processor::FtxSimpleView::ReportProgress(
    const std::string &Message) {

}

void udocs_processor::FtxSimpleView::ShowList(
    const std::vector<std::string> &List) {

}
