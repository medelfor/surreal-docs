/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "OrganizationListView.h"

namespace udocs_processor {
class FtxOrganizationListView : public virtual OrganizationListView,
    public virtual FtxSimpleView {
 public:
  FtxOrganizationListView();

  ~FtxOrganizationListView() override = default;

  void Init() override;

  void ShowOrganizations(
      const std::vector<std::string> &Organizations) override;

 private:
  static constexpr const char* SUCCESS_MESSAGE =
      "The organization(s) were successfuly retrieved";
  static constexpr const char* PROGRESS_MESSAGE =
      "Retrieving the organizations...";
  static constexpr const char* ORGANIZATION_FORMAT = " {}";
};
}  // namespace udocs_processor
