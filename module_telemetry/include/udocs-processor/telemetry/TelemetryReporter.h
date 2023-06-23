/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <memory>
#include <string>
#include <thread> NOLINT()
#include <mutex> NOLINT()
#include <vector>
#include <nlohmann/json.hpp>
#include "TelemetryService.h"

namespace udocs_processor {
class TelemetryReporter {
 public:
  TelemetryReporter(std::shared_ptr<spdlog::sinks::sink> LoggerSink,
      std::unique_ptr<TelemetryService> Service, std::string ClientId,
      std::string Source);

  void SetTelemetryEnabled(bool IsEnabled);

  // user HaveUserConsent = true only with user's explicit consent
  void ReportTelemetryEvent(const std::string& EventName,
      const nlohmann::json& Payload, bool HaveUserConsent = false);

  void StartReporting();

  void StopReporting();

  void Join();

 private:
  static constexpr const char* LOGGER_NAME = "telemetry-reporter";

  bool IsTelemetryEnabled = true;
  bool DoReport = false;
  std::thread Worker;

  const std::string Source;
  const std::string ClientId;

  static constexpr int QUEUE_POLL_DELAY_MS = 100;

  std::mutex EventsProtector;
  std::shared_ptr<spdlog::logger> l;
  std::vector<TelemetryService::TelemetryEvent> EventsToSend;
  std::unique_ptr<TelemetryService> Service;
};
}  // namespace udocs_processor
