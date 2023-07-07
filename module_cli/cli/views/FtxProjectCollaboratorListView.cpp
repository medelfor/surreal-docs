/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxProjectCollaboratorListView.h"

void udocs_processor::FtxProjectCollaboratorListView::Init() {
  FtxSimpleView::Init();
  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}

void udocs_processor::FtxProjectCollaboratorListView::ShowCollaboratos(
    std::vector<std::string> Collaborators) {
  FtxSimpleView::ReportSuccess(SUCCESS_MESSAGE);
  std::vector<std::string> List;
  for (const auto& Collaborator : Collaborators) {
    List.emplace_back(fmt::format(COLLABORATOR_FORMAT, Collaborator));
  }
  FtxSimpleView::ShowList(std::move(List));
}
