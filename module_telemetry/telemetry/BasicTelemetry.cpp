/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/telemetry/BasicTelemetry.h"
#include <udocs-processor/version.h>
#include <fmt/format.h>

udocs_processor::BasicTelemetry::BasicTelemetry(
    std::shared_ptr<TelemetryReporter> Reporter) : Reporter(Reporter) {
  Version = fmt::format("{}.{}.{}", VERSION_MAJOR, VERSION_MINOR,
      VERSION_INDEX);
  if (strlen(VERSION_TAG) > 0) {
    Version += fmt::format("-{}", VERSION_TAG);
  }
  Version += fmt::format("+{}.{}", VERSION_BUILD, VERSION_BUILD_ID);
}

void udocs_processor::BasicTelemetry::ReportInit(std::string Command) {
  nlohmann::json Payload;
  Payload[PAYLOAD_COMMAND] = std::move(Command);
  Payload[PAYLOAD_VERSION] = Version;

  Reporter->ReportTelemetryEvent(EVENT_INIT, Payload);
}

void udocs_processor::BasicTelemetry::ReportFail(
    std::string Command, std::string ErrorMessage) {
  nlohmann::json Payload;
  Payload[PAYLOAD_COMMAND] = std::move(Command);
  Payload[PAYLOAD_VERSION] = Version;
  Payload[PAYLOAD_ERROR_MESSAGE] = std::move(ErrorMessage);

  Reporter->ReportTelemetryEvent(EVENT_FAIL, Payload);
}

void udocs_processor::BasicTelemetry::ReportFinish(
    std::string Command) {
  nlohmann::json Payload;
  Payload[PAYLOAD_COMMAND] = std::move(Command);
  Payload[PAYLOAD_VERSION] = Version;

  Reporter->ReportTelemetryEvent(EVENT_FINISH, Payload);
}

void
udocs_processor::BasicTelemetry::ReportLog(std::string Command, std::string Log,
    std::optional<std::string> ContactMeBackAt) {
  nlohmann::json Payload;
  Payload[PAYLOAD_COMMAND] = std::move(Command);
  Payload[PAYLOAD_VERSION] = Version;
  Payload[PAYLOAD_LOG] = std::move(Log);
  Payload[PAYLOAD_CONTACT_ME_BACK_AT] = nullptr;
  if (ContactMeBackAt) {
    Payload[PAYLOAD_CONTACT_ME_BACK_AT] = *ContactMeBackAt;
  }

  Reporter->ReportTelemetryEvent(EVENT_LOG, Payload, true);
}
