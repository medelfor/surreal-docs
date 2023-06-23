/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/telemetry/LogReporter.h"

udocs_processor::LogReporter::LogReporter(std::string LogSavedAt,
    std::shared_ptr<spdlog::sinks::sink> LoggerSink,
    std::shared_ptr<BasicTelemetry> Telemetry)
    : LogSavedAt(std::move(LogSavedAt)), LoggerSink(LoggerSink),
    Telemetry(Telemetry) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (LoggerSink) {
      l->sinks().emplace_back(LoggerSink);
    }
  }
}

void udocs_processor::LogReporter::SetContactMeAt(
    std::optional<std::string> ContactMeAt) {
  this->ContactMeAt = std::move(ContactMeAt);
}

void udocs_processor::LogReporter::ReportLogs(
    std::string Command, std::optional<std::string> ContactMeAt) {
  LoggerSink->flush();

  std::optional<std::string> EffectiveContactMeAt = this->ContactMeAt;
  if (ContactMeAt) {
    EffectiveContactMeAt = *ContactMeAt;
  }

  bool Success = false;
  auto Handler = ::_fsopen(LogSavedAt.c_str(), LOGFILE_MODE, _SH_DENYNO);
  if (Handler) {
    ::fseek(Handler, 0, SEEK_END);
    uint64_t End = ::ftell(Handler);
    ::fseek(Handler, 0, SEEK_SET);

    char* Buffer = new char[End];

    size_t Read = ::fread_s(Buffer, End, sizeof(char), End, Handler);
    if (!::ferror(Handler)) {
      Success = true;
      std::string Logs{Buffer, Read};
      l->info("Read {} bytes of logfile `{}`", Read, LogSavedAt);
      Telemetry->ReportLog(std::move(Command), std::move(Logs),
          std::move(EffectiveContactMeAt));
    }

    delete[] Buffer;
  }

  if (!Success) {
    l->error("Couldn't open log file for reporting logs");
    Telemetry->ReportLog(std::move(Command), COULDNT_READ_LOGS,
        std::move(EffectiveContactMeAt));
  }
}

std::optional<std::string>
    udocs_processor::LogReporter::GetContactMeAt() const {
  return ContactMeAt;
}
