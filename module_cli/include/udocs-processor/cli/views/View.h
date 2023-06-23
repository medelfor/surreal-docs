/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <string>
#include <memory>
#include "udocs-processor/version.h"
#include "udocs-processor/telemetry/LogReporter.h"

namespace udocs_processor {
class View {
 public:
  View();

  virtual void SetLogReporter(std::optional<std::string> ContactMeAt,
      std::shared_ptr<LogReporter> Reporter);

  virtual void Start() {}

  virtual void Init() = 0;

  virtual void Tick() = 0;

  virtual void Destroy() = 0;

  virtual bool DoExit() const = 0;

  virtual void SetFinished(bool IsFinished) = 0;

  void SetLogFilePath(std::string LogFilePath);

  void SetNewVersion(std::string NewVersion);

  std::string GetVersion() const;

  std::string GetLogFilePath() const;

  std::string GetNewVersionAnnouncement() const;

  static constexpr const char COPYRIGHT[] =
      "Copyright (c) 2022-2023, Medelfor, Limited. All rights reserved.";
  static constexpr const char ERROR_ISSUE[] =
      "If you believe something's wrong with the product, please create an "
      "issue in Surreal Docs GitHub available at "
      "https://github.com/medelfor/surreal-docs You can also contact us at "
      "support@medelfor.com or @medelfor at Twitter. We're glad to help!";
  static constexpr const char ERROR_SORRY[] = "";

 protected:
  std::shared_ptr<LogReporter> GetLogReporter() const;

 private:
  std::string NewVersion;
  std::string LogFilePath;
  std::string Version;
  static constexpr const char STORED_LOG_PATTERN[] =
      "Current session log is stored at {}. You can find more details about "
      "the issue there";
  static constexpr const char VERSION_PATTERN[] = "Surreal Docs v{}.{}.{}";
  static constexpr const char VERSION_TAG_PATTERN[] = "-{}";

  static constexpr const char NEW_VERSION_PATTERN[] =
      "Surreal Docs v{} is out!";

  std::optional<std::string> ContactMeAt;
  std::shared_ptr<LogReporter> Reporter;
};
}  // namespace udocs_processor
