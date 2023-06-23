/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include <thread> NOLINT()
#include <future> NOLINT()
#include "udocs-processor/telemetry/TelemetryReporter.h"

udocs_processor::TelemetryReporter::TelemetryReporter(
    std::shared_ptr<spdlog::sinks::sink> LoggerSink,
    std::unique_ptr<TelemetryService> Service, std::string ClientId,
    std::string Source) : Service(std::move(Service)),
    ClientId(std::move(ClientId)), Source(std::move(Source)) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (LoggerSink) {
      l->sinks().emplace_back(LoggerSink);
    }
  }
}

void udocs_processor::TelemetryReporter::SetTelemetryEnabled(bool IsEnabled) {
  this->IsTelemetryEnabled = IsEnabled;
}

void udocs_processor::TelemetryReporter::ReportTelemetryEvent(
    const std::string &EventName, const nlohmann::json &Payload,
    bool HaveUserConsent) {
  if (!IsTelemetryEnabled && !HaveUserConsent) return;

  std::lock_guard<std::mutex> Lock{EventsProtector};

  EventsToSend.emplace_back(TelemetryService::TelemetryEvent{
    ClientId,
    Source,
    EventName,
    Payload.dump()
  });
}

void udocs_processor::TelemetryReporter::StartReporting() {
  DoReport = true;

  Worker = std::thread([this](){
    // no weak ptr for this is okay here
    l->info("Starting telemetry reporting");

    while (DoReport) {
      std::this_thread::sleep_for(std::chrono::milliseconds{
        QUEUE_POLL_DELAY_MS});
      std::vector<TelemetryService::TelemetryEvent> Copy;

      {
        std::lock_guard<std::mutex> Lock{EventsProtector};
        Copy = std::move(EventsToSend);
        EventsToSend.clear();
      }

      for (const auto& Event : Copy) {
        ApiStatus Status = Service->ReportTelemetryEvent(Event);
        if (Status.GetCode() != ApiStatus::SUCCESS) {
          l->warn("Couldn't report telemetry event `{}`: {}/{}",
              Event.EventName, Status.GetCode(),
              Status.GetMessageDescription());
        } else {
          l->info("Reported telemetry event `{}`", Event.EventName);
        }
      }
    }

    l->info("Ending telemetry reporting");
  });
}

void udocs_processor::TelemetryReporter::StopReporting() {
  DoReport = false;
}

void udocs_processor::TelemetryReporter::Join() {
  if (Worker.joinable()) {
    Worker.join();
  }
}
