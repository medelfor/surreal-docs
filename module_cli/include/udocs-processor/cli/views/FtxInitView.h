/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "InitView.h"
#include "FtxView.h"
#include "udocs-processor/cli/cli/InitCLI.h"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>

namespace udocs_processor {
class FtxInitView : public InitView, public FtxView {
 public:
  explicit FtxInitView(InitCLI& Cli);

  void SetEngineVersions(
      std::vector<UnrealInteraction::UnrealVersion> Versions) override;

  // targets to confs
  void SetTargets(
      std::map<std::string, std::string> Targets) override;

  void SetSources(
      std::vector<UnrealInteraction::Source> Sources) override;

  void SetName(std::string Name) override;

  void SetVersion(std::string Version) override;

  void SetOrganization(std::string Organization) override;

  void SetContactMeAt(std::string ContactMeAt) override;

  void SelectEngineVersion(int VersionIndex) override;

  void SelectTarget(const std::string &Target) override;

  void SelectSources(const std::vector<int> &Sources) override;

  void Init() override;

  void Tick() override;

  void Destroy() override;

  void SetStatus(Status CurrentStatus, const std::string &Message) override;

  bool DoExit() const override;

  void SetFinished(bool IsFinished) override;

  void Start() override;

  const std::string &GetContactMeAt() const override;

  const std::string &GetSelectedName() const override;

  const std::string &GetSelectedOrganization() const override;

  const std::string &GetSelectedVersion() const override;

  std::vector<UnrealInteraction::Source> GetSelectedSources() const override;

  std::optional<UnrealInteraction::UnrealVersion>
      GetSelectedEngineVersion() const override;

  std::optional<std::string> GetSelectedTarget() const override;

  const std::string &GetSelectedConfiguration() const override;

  bool DoExportPrivate() const override;

  void SetDoExportPrivate(bool DoExport) override;

 private:
  bool HasFinished() const;

  std::string GetStatusString(Status Status_) const;

  static constexpr const char PRESS_TO_EXIT[] = "Press ENTER to exit";
  static constexpr const char PRESS_TO_EXIT_PREFIX[] = ". ";
  static constexpr const char TOGGLE_ON[] = "On";
  static constexpr const char TOGGLE_OFF[] = "Off";

  static constexpr const char NAME_PLACEHOLDER[] = "My Project";
  static constexpr const char VERSION_PLACEHOLDER[] = "1.0.0";
  static constexpr const char ORGANIZATION_PLACEHOLDER[] = "My Company";
  static constexpr const char CONTACT_ME_AT_PLACEHOLDER[] = "<don't contact>";
  static constexpr const char START_LABEL[] = "       Done";  // yep

  static constexpr const char CONFIGURATION_DEBUG[] = "Debug";
  static constexpr const char CONFIGURATION_DEBUG_GAME[] = "DebugGame";
  static constexpr const char CONFIGURATION_DEVELOPMENT[] = "Development";
  static constexpr const char CONFIGURATION_SHIPPING[] = "Shipping";
  static constexpr const char CONFIGURATION_TEST[] = "Test";

  static constexpr const char PLUGIN_PREFIX[] = "[Plugin] ";

  static constexpr const char STATUS_STAND_BY[] = "Standing by";
  static constexpr const char STATUS_PROCESSING[] = "Applying configuration";
  static constexpr const char STATUS_FINISHED[] = "Finished";
  static constexpr const char STATUS_FAILED[] = "Failed";

  std::map<Status, std::string> StatusToString;

  std::vector<std::string> OnOff;

  std::mutex SelectionsProtection;

  std::map<std::string, std::string> TargetsAgainstConfigurations;
  std::vector<std::string> Configurations;
  std::vector<std::string> Targets;
  std::vector<UnrealInteraction::Source> Sources;
  std::vector<UnrealInteraction::UnrealVersion> EngineVersions;
  std::vector<std::string> CompiledEngineVersions;
  std::string Name;
  std::string Version;
  std::string Organization;
  std::string ContactMeAt;
  bool DoExportPrivate_ = false;

  int SelectedEngineVersion = 0;
  int SelectedTarget = 0;
  int LastSelectedTarget = -1;
  int SelectedConfiguration = 0;
  std::vector<char> SelectedSources;

  ftxui::Component EngineVersionRadio;
  ftxui::Component ConfigurationRadio;
  ftxui::Component TargetRadio;
  ftxui::Component SourcesList;
  ftxui::Component NameInput;
  ftxui::Component VersionInput;
  ftxui::Component OrganizationInput;
  ftxui::Component ContactMeAtInput;
  ftxui::Component DoExportPrivateCheckbox;
  ftxui::Component StartButton;
  ftxui::Component Renderer;
  ftxui::ScreenInteractive Screen;
  uint32_t FrameCount = 0;

  std::string StatusString;
  std::string ErrorMessage;
  Status CurrentStatus = Status::STAND_BY;
  bool DoExit_ = false;
  bool IsFinished = false;

  InitCLI& Cli;
};
}  // namespace udocs_processor
