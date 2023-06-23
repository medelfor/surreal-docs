/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <memory>
#include <string>
#include <optional>
#include "BasicTelemetry.h"

// win64 only
namespace udocs_processor {
class LogReporter {
 public:
  LogReporter(std::string LogSavedAt,
      std::shared_ptr<spdlog::sinks::sink> LoggerSink,
      std::shared_ptr<BasicTelemetry> Telemetry);

  void SetContactMeAt(std::optional<std::string> ContactMeAt);

  void ReportLogs(std::string Command,
      std::optional<std::string> ContactMeAt);

  std::optional<std::string> GetContactMeAt() const;

 private:
  static constexpr const char* LOGGER_NAME = "log-reporter";
  static constexpr const char* COULDNT_READ_LOGS = "<couldn't read>";

  static constexpr const char* LOGFILE_MODE = "r";

  std::shared_ptr<spdlog::logger> l;
  std::optional<std::string> ContactMeAt;
  std::string LogSavedAt;
  std::shared_ptr<spdlog::sinks::sink> LoggerSink;
  std::shared_ptr<BasicTelemetry> Telemetry;
};
}  // namespace udocs_processor
