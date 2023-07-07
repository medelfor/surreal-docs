/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "FtxSimpleView.h"
#include "MigrateView.h"

namespace udocs_processor {
class FtxMigrateView : public virtual MigrateView,
    public virtual FtxSimpleView {
 public:
  FtxMigrateView() = default;

  void Init() override;

  void ReportProtocol(const std::vector<MigrationAction> &Protocol) override;

  ~FtxMigrateView() override = default;

 private:
  static constexpr const char* SUCCESS_MESSAGE =
      "The project was successfully updated";
  static constexpr const char* NOTHING_TO_UPDATE_MESSAGE =
      "The project is up-to-date. Nothing to migrate";
  static constexpr const char* PROTOCOL_HEADING =
      "The following actions were taken:";
  static constexpr const char* ACTION_PATTERN = "- {}";
  static constexpr const char* PROGRESS_MESSAGE =
      "Migrating the project...";
};
}  // namespace udocs_processor
