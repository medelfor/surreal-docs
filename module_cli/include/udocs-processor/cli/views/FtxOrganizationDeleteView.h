/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "OrganizationDeleteView.h"

namespace udocs_processor {
class FtxOrganizationDeleteView : public virtual OrganizationDeleteView,
    public virtual FtxSimpleView {
 public:
  FtxOrganizationDeleteView();

  ~FtxOrganizationDeleteView() override = default;

  void Init() override;

  void ReportProgress() override;

  void ReportSuccess() override;

  bool Confirm() override;

 private:
  static constexpr const char* CONFIRMATION_MESSAGE = "Are you sure you want "
      "to delete the organization? It'll permanently delete all the documents "
      "within it";
  static constexpr const char* SUCCESS_MESSAGE =
      "The organization was successfuly deleted";
  static constexpr const char* PROGRESS_MESSAGE =
      "Deleting the organization...";
  static constexpr const char* GATHERING_INFO_MESSAGE =
      "Confirming choice...";
};
}  // namespace udocs_processor
