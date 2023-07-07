/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxProjectListView.h"

void udocs_processor::FtxProjectListView::Init() {
  FtxSimpleView::Init();
  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}

void udocs_processor::FtxProjectListView::ShowProjects(
    const std::vector<ProjectService::ProjectData>& Projects) {
  FtxSimpleView::ReportSuccess(SUCCESS_MESSAGE);
  std::vector<std::string> List;
  for (const auto& Project : Projects) {
    List.emplace_back(fmt::format(PROJECT_FORMAT, Project.Name,
        Project.IsPublic ? PUBLIC : PRIVATE));
  }
  FtxSimpleView::ShowList(std::move(List));
}
