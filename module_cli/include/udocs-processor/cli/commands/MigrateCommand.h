/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>
#include <string>
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>
#include "InitCommand.h"

namespace udocs_processor {
class MigrateCommand {
 public:
  struct MigrateRequest {
    std::string ProjectDir;
  };

  struct TakenAction {
    std::string Description;
  };

  MigrateCommand(const InitCommand& Init,
      std::shared_ptr<spdlog::sinks::sink> Sink);

  std::vector<TakenAction> Migrate(const MigrateRequest& Request) const;

  static const uint64_t MIGRATION_VERSION = 1;

 private:
  nlohmann::json InitializeDescriptor() const;

  nlohmann::json LoadMigrationDescriptor(const std::string& ProjectDir) const;

  bool IsInitializedProject(const std::string& ProjectDir) const;

  void SaveMigrationDescriptor(const std::string& ProjectDir,
      const nlohmann::json& Desciptor) const;

  static constexpr const char* ACTION_INSTANTIATE_DESCRIPTOR =
      "Migration descriptor was installed";
  static constexpr const char* ACTION_UPDATE_DESCRIPTOR =
      "Migration descriptor was updated";
  static constexpr const char* ACTION_UPDATE_PLUGIN =
      "Surreal Docs UE plugin was updated";

  static constexpr const char* JSON_VERSION = "version";

  static constexpr const char* LOGGER_NAME = "migrate";
  static constexpr const char* MIGRATION_DESCRIPTOR_PATH =
      "surdocs/migration.json";
  static constexpr const char* PROJECT_DESCRIPTOR_PATH =
      "surdocs/surreal.json";

  const InitCommand& Init;

  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor
