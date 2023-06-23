/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/View.h"
#include <fmt/format.h>
#include "udocs-processor/version.h"

std::string udocs_processor::View::GetVersion() const {
  return Version;
}

udocs_processor::View::View() {
  Version = fmt::format(VERSION_PATTERN, VERSION_MAJOR, VERSION_MINOR,
      VERSION_INDEX);
  if (strlen(VERSION_TAG) > 0) {
    Version += fmt::format(VERSION_TAG_PATTERN, VERSION_TAG);
  }
}

void udocs_processor::View::SetLogFilePath(std::string LogFilePath) {
  this->LogFilePath = std::move(LogFilePath);
}

std::string udocs_processor::View::GetLogFilePath() const {
  return fmt::format(STORED_LOG_PATTERN, LogFilePath);
}

void udocs_processor::View::SetNewVersion(std::string NewVersion) {
  this->NewVersion = std::move(NewVersion);
}

std::string udocs_processor::View::GetNewVersionAnnouncement() const {
  return NewVersion.empty()
      ? "" : fmt::format(NEW_VERSION_PATTERN, NewVersion);
}

void udocs_processor::View::SetLogReporter(
    std::optional<std::string> ContactMeAt,
    std::shared_ptr<LogReporter> Reporter) {
  this->ContactMeAt = std::move(ContactMeAt);
  this->Reporter = Reporter;
}

std::shared_ptr<udocs_processor::LogReporter>
    udocs_processor::View::GetLogReporter() const {
  return Reporter;
}
