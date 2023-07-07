/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "ProjectListView.h"

namespace udocs_processor {
class FtxProjectListView : public virtual ProjectListView,
    public virtual FtxSimpleView {
 public:
  FtxProjectListView() = default;

  void Init() override;

  void ShowProjects(
      const std::vector<ProjectService::ProjectData>& Projects) override;

  ~FtxProjectListView() override = default;

 private:
  static constexpr const char* SUCCESS_MESSAGE =
      "The project(s) were successfuly retrieved";
  static constexpr const char* PROGRESS_MESSAGE =
      "Retrieving the projects...";
  static constexpr const char* PROJECT_FORMAT = " {:<30} [{}]";
  static constexpr const char* PUBLIC = "public";
  static constexpr const char* PRIVATE = "private";
};
}  // namespace udocs_processor
