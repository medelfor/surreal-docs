/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include "udocs-processor/cli/commands/MigrateCommand.h"

udocs_processor::MigrateCommand::MigrateCommand(const InitCommand& Init,
    std::shared_ptr<spdlog::sinks::sink> Sink) : Init(Init) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}

std::vector<udocs_processor::MigrateCommand::TakenAction>
    udocs_processor::MigrateCommand::Migrate(
        const MigrateRequest &Request) const {
  if (!IsInitializedProject(Request.ProjectDir)) {
    throw std::domain_error{"The project isn't initialized (call \"surdocs "
      "init\"). Migration isn't needed"};
  }

  std::vector<TakenAction> Actions;

  nlohmann::json Descriptor = LoadMigrationDescriptor(Request.ProjectDir);

  bool IsOutdated = false;
  if (Descriptor.empty() || !Descriptor.contains(JSON_VERSION) ||
      !Descriptor[JSON_VERSION].is_number()) {
    l->info("Migration descriptor is either invalid or non-existant");
    IsOutdated = true;
    Actions.emplace_back(TakenAction{ACTION_INSTANTIATE_DESCRIPTOR});
    Descriptor = InitializeDescriptor();
  } else if (Descriptor[JSON_VERSION] < MIGRATION_VERSION) {
    l->info("Migration version is outdated");
    IsOutdated = true;
    Actions.emplace_back(TakenAction{ACTION_UPDATE_DESCRIPTOR});
    Descriptor[JSON_VERSION] = MIGRATION_VERSION;
  }

  SaveMigrationDescriptor(Request.ProjectDir, Descriptor);

  if (IsOutdated) {
    l->info("Installing plugin");
    Init.InstallPlugin(Request.ProjectDir);
    Actions.emplace_back(TakenAction{ACTION_UPDATE_PLUGIN});
  }

  return Actions;
}

nlohmann::json udocs_processor::MigrateCommand::LoadMigrationDescriptor(
    const std::string& ProjectDir) const {
  std::ifstream Input(ProjectDir + MIGRATION_DESCRIPTOR_PATH);
  if (!Input.good()) {
    l->info("Couldn't open migration descriptor");
    return {};
  }
  std::stringstream Buffer;
  Buffer << Input.rdbuf();

  return nlohmann::json::parse(Buffer.str());
}

nlohmann::json udocs_processor::MigrateCommand::InitializeDescriptor() const {
  nlohmann::json Descriptor;
  Descriptor[JSON_VERSION] = MIGRATION_VERSION;

  return Descriptor;
}

void udocs_processor::MigrateCommand::SaveMigrationDescriptor(
    const std::string& ProjectDir,
    const nlohmann::json &Desciptor) const {
  std::ofstream Output(ProjectDir + MIGRATION_DESCRIPTOR_PATH);
  if (!Output.good()) {
    l->error("Couldn't open (w) migration descriptor");
    throw std::runtime_error{"Unable to write migration descriptor"};
  }

  Output << Desciptor.dump();
}

bool udocs_processor::MigrateCommand::IsInitializedProject(
    const std::string& ProjectDir) const {
  return std::ifstream(ProjectDir + PROJECT_DESCRIPTOR_PATH).good();
}
