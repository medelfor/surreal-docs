/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <fmt/format.h>
#include "udocs-processor/cli/views/FtxInitView.h"
#include "udocs-processor/cli/CLISignalHandler.h"

#undef ERROR
#define NOLINT()

void udocs_processor::FtxInitView::SetEngineVersions(
    std::vector<UnrealInteraction::UnrealVersion> Versions) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  CompiledEngineVersions.clear();
  CompiledEngineVersions.reserve(Versions.size());
  for (const auto& Version : Versions) {
    CompiledEngineVersions.emplace_back(Version.IsCustomBuild
        ? Version.EngineDir : Version.Version);
  }
  EngineVersions = std::move(Versions);
}

void udocs_processor::FtxInitView::SetTargets(
    std::map<std::string, std::string> Targets) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  this->Targets.clear();
  for (auto& Target : Targets) {
    this->Targets.emplace_back(std::move(Target.first));
  }
  TargetsAgainstConfigurations = std::move(Targets);
}

void udocs_processor::FtxInitView::SetSources(
    std::vector<UnrealInteraction::Source> Sources) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  SelectedSources = std::vector<char>(Sources.size(), false);
  this->Sources = std::move(Sources);

  for (size_t i = 0; i < this->Sources.size(); ++i) {
    const auto &Source = this->Sources[i];

    SourcesList->Add(ftxui::Checkbox(fmt::format("{}{}",
        Source.IsPlugin ? PLUGIN_PREFIX : "", Source.Module),
        reinterpret_cast<bool*>(SelectedSources.data() + i)));
  }
}

void udocs_processor::FtxInitView::SetName(std::string Name) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  this->Name = std::move(Name);
}

void udocs_processor::FtxInitView::SetVersion(std::string Version) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  this->Version = std::move(Version);
}

void udocs_processor::FtxInitView::SelectEngineVersion(int VersionIndex) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  if (VersionIndex < EngineVersions.size()) {
    SelectedEngineVersion = VersionIndex;
  } else {
    throw std::invalid_argument{"Invalid version index"};
  }
}

void udocs_processor::FtxInitView::SelectTarget(
    const std::string &Target) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  for (size_t i = 0; i < Targets.size(); ++i) {
    if (Targets[i] == Target) {
      SelectedTarget = i;
      break;
    }
  }
}

void udocs_processor::FtxInitView::SelectSources(
    const std::vector<int> &Sources) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  for (int SourceIndex : Sources) {
    if (SourceIndex < SelectedSources.size()) {
      SelectedSources[SourceIndex] = true;
    } else {
      throw std::invalid_argument{"Invalid source index"};
    }
  }
}

void udocs_processor::FtxInitView::Init() {
  FtxView::Init();

  using namespace ftxui; NOLINT()

  DoExportPrivateCheckbox = Checkbox("Export private members",
      &DoExportPrivate_);
  NameInput = Input(&Name, NAME_PLACEHOLDER);
  VersionInput = Input(&Version, VERSION_PLACEHOLDER);
  ContactMeAtInput = Input(&ContactMeAt, CONTACT_ME_AT_PLACEHOLDER);
  OrganizationInput = Input(&Organization, ORGANIZATION_PLACEHOLDER);
  StartButton = Button(START_LABEL, [this]() {
    Cli.OnDone();
  });

  EngineVersionRadio = Radiobox(&CompiledEngineVersions,
      &SelectedEngineVersion);
  TargetRadio = Radiobox(&Targets, &SelectedTarget);
  ConfigurationRadio = Radiobox(&Configurations, &SelectedConfiguration);

  SourcesList = Container::Vertical({});

  Renderer = ftxui::Renderer(
      Container::Vertical({
       EngineVersionRadio, ConfigurationRadio, TargetRadio, NameInput,
       VersionInput, OrganizationInput, ContactMeAtInput,
       DoExportPrivateCheckbox, SourcesList, StartButton, GetComponents()
      }),
      [&]() {
        return WrapInWindow({
          vbox({
            hbox({
              window(
                hbox({
                  Gap(), text("Engine") | ForegroundColor1()
                }),
                vbox({
                  EngineVersionRadio->Render() | vscroll_indicator | frame |
                      ForegroundColor1(),
                  filler(),
                  text(SelectedEngineVersion < EngineVersions.size() ?
                      EngineVersions[SelectedEngineVersion].EngineDir : "")
                      | color(BackgroundColor_) | bgcolor(ForegroundColor1_)
                })) | yflex_grow | size(ftxui::WIDTH, ftxui::EQUAL, 60),
              window(
                hbox({
                  Gap(), text("Target") | ForegroundColor1()
                }),
                vbox({
                  TargetRadio->Render() | vscroll_indicator | frame |
                      ForegroundColor1(),
                  filler()
                })) | flex_grow,
              window(
                hbox({
                  Gap(2), text("Configuration") | ForegroundColor1()
                }),
                vbox({
                  ConfigurationRadio->Render() | ForegroundColor1(),
                  filler()
                })),
            }) | flex_grow,
            hbox({
              window(
                hbox({
                  Gap(), text("Export") | ForegroundColor1()
                }),
                vbox({
                  separatorEmpty(),
                  hbox({
                    separatorEmpty(),
                    DoExportPrivateCheckbox->Render() | ForegroundColor1(),
                  }),
                  separatorEmpty(),
                  window(
                    hbox({
                      Gap(), text("Modules to export") | ForegroundColor1()
                    }),
                    vbox({
                      SourcesList->Render() | flex_grow | vscroll_indicator |
                      frame | ForegroundColor1()
                    }) | flex_grow)})) | flex_grow,
              vbox({
                window(
                  hbox({
                    Gap(), text("Project") | ForegroundColor1()
                  }),
                  vbox({
                    hbox({
                      text("Name:") | ForegroundColor2(),
                      separatorEmpty(),
                      NameInput->Render() | ForegroundColor1()
                    }),
                    hbox({
                      text("Version:") | ForegroundColor2(),
                      separatorEmpty(),
                      VersionInput->Render() | ForegroundColor1()
                    }),
                    hbox({
                      text("Organization:") | ForegroundColor2(),
                      separatorEmpty(),
                      OrganizationInput->Render() | ForegroundColor1()
                    }),
                    vbox({
                      text("If I report a bug, contact me back at:")
                          | ForegroundColor2(),
                      hbox({
                        separatorEmpty(),
                        ContactMeAtInput->Render() | ForegroundColor1()
                      })
                    })
                  })) | xflex_grow,
                window(
                  hbox({
                    Gap(), text("Status") | ForegroundColor1()
                  }),
                  vbox({
                    hbox({
                      text("[ STND ]") | ForegroundColor1()
                          | HHide(CurrentStatus != Status::STAND_BY),
                      text("[ FAIL ]") | ErroneuousColor()
                          | HHide(CurrentStatus != Status::FAILED),
                      text("[  OK  ]") | SuccessColor()
                          | HHide(CurrentStatus != Status::FINISHED),
                      spinner(18, FrameCount) | SuccessColor()
                          | HHide(CurrentStatus != Status::PROCESSING),
                      separatorEmpty(),
                      text(StatusString + (HasFinished()
                          ? PRESS_TO_EXIT_PREFIX + std::string(PRESS_TO_EXIT)
                          : ""))
                          | ForegroundColor1()
                          | size(ftxui::WIDTH, ftxui::EQUAL, 40),
                    }) | flex_grow,
                    filler(),
                    hbox({
                      filler(),
                      StartButton->Render()
                          | size(ftxui::WIDTH, ftxui::EQUAL, 20)
                          | HHide(CurrentStatus != InitView::Status::STAND_BY)
                          | ForegroundColor1()
                    })
                  }) | flex_grow) | flex_grow,
              }) | flex_grow
            }) | flex_grow | size(ftxui::HEIGHT, ftxui::LESS_THAN, 15),
            hflow(paragraph(""))
          }) | VHide(CurrentStatus == Status::FAILED ||
              CurrentStatus == Status::GATHERING) | flex_grow,
          vbox({
            filler(),
            hbox({
              filler(),
              text("Gathering info") | ForegroundColor1(),
              spinner(1, FrameCount),
              filler()
            }),
            filler()
          }) | VHide(CurrentStatus != Status::GATHERING),
          vbox({
            text(PRESS_TO_EXIT) | ForegroundColor1(),
            filler()
          }) | VHide(CurrentStatus != Status::FAILED),
          WrapError(ErrorMessage, CurrentStatus == Status::FAILED)
        }, "Initialization");
      });

  Renderer |= CatchEvent([&, this](Event Event_) {
    if ((IsFinished || CurrentStatus == Status::FAILED) &&
         Event_ == Event::Return) {
      DoExit_ = true;
      this->Screen.Exit();
      return true;
    }
    return false;
  });

  CLISignalHandler::OverrideHandler();
}

void udocs_processor::FtxInitView::Tick() {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  FtxView::Tick();

  ++FrameCount;
  Screen.PostEvent(ftxui::Event::Custom);

  // bad
  if (SelectedTarget < Targets.size() && SelectedTarget != LastSelectedTarget) {
    auto It = TargetsAgainstConfigurations.find(Targets[SelectedTarget]);
    if (It != TargetsAgainstConfigurations.end()) {
      for (size_t i = 0; i < Configurations.size(); ++i) {
        if (Configurations[i] == It->second) {
          SelectedConfiguration = i;
          LastSelectedTarget = SelectedTarget;
          break;
        }
      }
    }
  }
}

void udocs_processor::FtxInitView::Destroy() {}

udocs_processor::FtxInitView::FtxInitView(InitCLI &Cli)
    : Screen(ftxui::ScreenInteractive::Fullscreen()), Cli(Cli) {
  OnOff = {TOGGLE_ON, TOGGLE_OFF};
  Configurations = {CONFIGURATION_DEBUG, CONFIGURATION_DEBUG_GAME,
      CONFIGURATION_DEVELOPMENT, CONFIGURATION_SHIPPING, CONFIGURATION_TEST};

  StatusToString.insert(std::make_pair(Status::STAND_BY, STATUS_STAND_BY));
  StatusToString.insert(std::make_pair(Status::PROCESSING, STATUS_PROCESSING));
  StatusToString.insert(std::make_pair(Status::FINISHED, STATUS_FINISHED));
  StatusToString.insert(std::make_pair(Status::FAILED, STATUS_FAILED));
}

bool udocs_processor::FtxInitView::HasFinished() const {
  return CurrentStatus == Status::FINISHED ||
      CurrentStatus == Status::FAILED;
}

void udocs_processor::FtxInitView::SetStatus(InitView::Status CurrentStatus,
    const std::string &Message) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  if (CurrentStatus == InitView::Status::FAILED) {
    StatusString = GetStatusString(CurrentStatus);
    this->CurrentStatus = CurrentStatus;
    ErrorMessage = Message;
  } else if (this->CurrentStatus != InitView::Status::FAILED) {
    StatusString = GetStatusString(CurrentStatus);
    this->CurrentStatus = CurrentStatus;
  }
}

bool udocs_processor::FtxInitView::DoExit() const {
  return DoExit_;
}

void udocs_processor::FtxInitView::SetFinished(bool IsFinished) {
  this->IsFinished = IsFinished;
}

std::string udocs_processor::FtxInitView::GetStatusString(
    InitView::Status Status_) const {
  auto It = StatusToString.find(Status_);
  if (It != StatusToString.end()) {
    return It->second;
  }

  return "";
}

void udocs_processor::FtxInitView::SetOrganization(std::string Organization) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  this->Organization = std::move(Organization);
}

const std::string &udocs_processor::FtxInitView::GetSelectedName() const {
  return Name;
}

const std::string &
udocs_processor::FtxInitView::GetSelectedOrganization() const {
  return Organization;
}

const std::string &udocs_processor::FtxInitView::GetSelectedVersion() const {
  return Version;
}

std::vector<udocs_processor::UnrealInteraction::Source>
udocs_processor::FtxInitView::GetSelectedSources() const {
  std::vector<UnrealInteraction::Source> Result;

  for (size_t i = 0; i < SelectedSources.size(); ++i) {
    if (i < Sources.size() && SelectedSources[i]) {
      Result.emplace_back(Sources[i]);
    }
  }

  return Result;
}

std::optional<udocs_processor::UnrealInteraction::UnrealVersion>
udocs_processor::FtxInitView::GetSelectedEngineVersion() const {
  if (SelectedEngineVersion < EngineVersions.size()) {
    return EngineVersions[SelectedEngineVersion];
  } else {
    return {};
  }
}

std::optional<std::string>
udocs_processor::FtxInitView::GetSelectedTarget() const {
  if (SelectedTarget < Targets.size()) {
    return Targets[SelectedTarget];
  } else {
    return {};
  }
}

const std::string &
udocs_processor::FtxInitView::GetSelectedConfiguration() const {
  return Configurations[SelectedConfiguration];
}

bool udocs_processor::FtxInitView::DoExportPrivate() const {
  return DoExportPrivate_;
}

void udocs_processor::FtxInitView::SetDoExportPrivate(bool DoExport) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  this->DoExportPrivate_ = DoExport;
}

void udocs_processor::FtxInitView::Start() {
  Screen.Loop(Renderer);
}

const std::string &udocs_processor::FtxInitView::GetContactMeAt() const {
  return ContactMeAt;
}

void udocs_processor::FtxInitView::SetContactMeAt(std::string ContactMeAt) {
  this->ContactMeAt = std::move(ContactMeAt);
}
