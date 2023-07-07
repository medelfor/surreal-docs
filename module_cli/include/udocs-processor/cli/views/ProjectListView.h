/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "SimpleView.h"
#include "udocs-processor/services/ProjectService.h"

namespace udocs_processor {
class ProjectListView : public virtual SimpleView {
 public:
  virtual void ShowProjects(
      const std::vector<ProjectService::ProjectData>& Projects) = 0;

  ~ProjectListView() override = default;
};
}  // namespace udocs_processor
