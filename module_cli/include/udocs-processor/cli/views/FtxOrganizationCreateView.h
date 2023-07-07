/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "OrganizationCreateView.h"

namespace udocs_processor {
class FtxOrganizationCreateView : public virtual OrganizationCreateView,
    public virtual FtxSimpleView {
 public:
  FtxOrganizationCreateView();

  ~FtxOrganizationCreateView() override = default;

  void Init() override;

  void ReportSuccess() override;

 private:
  static constexpr const char* SUCCESS_MESSAGE =
      "The organization was successfuly created";
  static constexpr const char* PROGRESS_MESSAGE =
      "Creating the organization...";
};
}  // namespace udocs_processor
