/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <memory>
#include <string>
#include <optional>
#include "TelemetryReporter.h"

namespace udocs_processor {
class BasicTelemetry {
 public:
  explicit BasicTelemetry(std::shared_ptr<TelemetryReporter> Reporter);

  void ReportInit(std::string Command);

  void ReportFail(std::string Command, std::string ErrorMessage);

  void ReportLog(std::string Command, std::string Log,
      std::optional<std::string> ContactMeBackAt);

  void ReportFinish(std::string Command);

 private:
  static constexpr const char* EVENT_INIT = "init";
  static constexpr const char* EVENT_FAIL = "fail";
  static constexpr const char* EVENT_FINISH = "finish";
  static constexpr const char* EVENT_LOG = "log";

  static constexpr const char* PAYLOAD_VERSION = "version";
  static constexpr const char* PAYLOAD_COMMAND = "command";
  static constexpr const char* PAYLOAD_ERROR_MESSAGE = "error";
  static constexpr const char* PAYLOAD_LOG = "log";
  static constexpr const char* PAYLOAD_CONTACT_ME_BACK_AT = "contact_me_at";

  std::string Version;

  std::shared_ptr<TelemetryReporter> Reporter;
};
}  // namespace udocs_processor
