/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/cli/InitCLI.h"
#include <spdlog/spdlog.h>
#include <future> NOLINT()
#include <thread> NOLINT()
#include <chrono> NOLINT()
#include <set>

#undef ERROR

udocs_processor::InitCLI::InitCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
    std::unique_ptr<InitCommand> Command, std::unique_ptr<UnrealInteraction> Ue,
    std::shared_ptr<BasicTelemetry> Telemetry)
    : Command(std::move(Command)), Ue(std::move(Ue)), Telemetry(Telemetry) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}

bool udocs_processor::InitCLI::Init(
    const udocs_processor::InitCLI::Arguments &Args) {
  Telemetry->ReportInit(TELEMETRY_COMMAND_NAME);
  InteractiveView->Init();
  InteractiveView->SetStatus(InitView::Status::GATHERING, "");

  bool Success = true;

  auto Future = std::async(std::launch::async,
    [this, &Success, &Args]() {
      std::string OutDirectory;
      try {
        IsNewProject = Command->IsNewProject(CURRENT_DIRECTORY);

        if (IsNewProject) {
          Project = Command->CreateNewProject(CURRENT_DIRECTORY);
        } else {
          Project = Command->LoadProject(CURRENT_DIRECTORY);
        }

        InitializeView(*Project);
      } catch (const std::exception& Exc) {
        Success = false;
        Telemetry->ReportFail(TELEMETRY_COMMAND_NAME, Exc.what());
        l->error("Exception in init-loading thread: {}", Exc.what());
        InteractiveView->SetStatus(InitView::Status::FAILED,
            fmt::format("Unexpected error: {}", Exc.what()));
        return;
      }

      InteractiveView->SetStatus(InitView::Status::STAND_BY, "");
    });

  auto View = std::thread(
    [this]() {
      while (!InteractiveView->DoExit()) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds{POLL_DURATION_MS});
        InteractiveView->Tick();
      }
    });
  InteractiveView->Start();

  Future.wait();
  View.join();
  InteractiveView->Destroy();
  Telemetry->ReportFinish(TELEMETRY_COMMAND_NAME);

  return Success;
}

void udocs_processor::InitCLI::InitializeView(
    const SurrealProject& Project) const {
  std::vector<UnrealInteraction::UnrealVersion> Versions =
      Ue->EnumerateVersions();
  std::vector<UnrealInteraction::Source> Sources =
      Ue->EnumerateSources(CURRENT_DIRECTORY);
  std::map<std::string, std::optional<std::string>> Targets =
      Ue->EnumerateTargets(CURRENT_DIRECTORY);

  InteractiveView->SetEngineVersions(Versions);
  InteractiveView->SetSources(Sources);

  std::map<std::string, std::string> ActualTargets;
  for (const auto& Target : Targets) {
    ActualTargets.insert(std::make_pair(Target.first,
        Target.second ? *Target.second : DEFAULT_CONFIGURATION));
  }
  InteractiveView->SetTargets(std::move(ActualTargets));

  InteractiveView->SetName(Project.Name);
  InteractiveView->SetOrganization(Project.Organization);
  InteractiveView->SetContactMeAt(Project.ContactMeAt);
  InteractiveView->SetVersion(Project.Version);
  InteractiveView->SelectTarget(Project.Target);
  InteractiveView->SetDoExportPrivate(Project.DoExportPrivate);

  std::filesystem::path EnginePath = Absolutify(Project.EngineRoot);
  for (size_t i = 0; i < Versions.size(); ++i) {
    const auto& Version = Versions[i];
    std::filesystem::path VersionRoot = Absolutify(Version.EngineDir);

    if (VersionRoot == EnginePath) {
      InteractiveView->SelectEngineVersion(i);
      break;
    }
  }

  std::vector<int> SelectedSources;
  for (const auto& CppInclude : Project.CppInputs) {
    std::filesystem::path IncludePath = Absolutify(CppInclude);

    // n^2? yes baby
    for (size_t i = 0; i < Sources.size(); ++i) {
      std::filesystem::path SourceRoot = Absolutify(Sources[i].Path);

      if (IncludePath == SourceRoot) {
        SelectedSources.emplace_back(i);
      }
    }
  }

  InteractiveView->SelectSources(SelectedSources);
}

std::filesystem::path udocs_processor::InitCLI::Absolutify(
    const std::string& Path) const {
  std::filesystem::path Absolute{Path};
  if (std::filesystem::exists(Absolute)) {
    Absolute = std::filesystem::absolute(Absolute);
  }

  return Absolute;
}

void udocs_processor::InitCLI::OnDone() {
  IsDone = false;
  std::async(std::launch::async,
    [this]() {
      try {
        InteractiveView->SetStatus(InitView::Status::PROCESSING, "");

        Command->InitializeProject(CURRENT_DIRECTORY, IsNewProject);

        Project->DoExportPrivate = InteractiveView->DoExportPrivate();
        Project->Name = InteractiveView->GetSelectedName();
        Project->Organization = InteractiveView->GetSelectedOrganization();
        Project->Version = InteractiveView->GetSelectedVersion();
        Project->ContactMeAt = InteractiveView->GetContactMeAt();
        if (auto EngineVersion = InteractiveView->GetSelectedEngineVersion()) {
          Project->EngineRoot = EngineVersion->EngineDir;
        }
        if (auto Target = InteractiveView->GetSelectedTarget()) {
          Project->Target = *Target;
        }
        Project->Configuration = InteractiveView->GetSelectedConfiguration();

        std::set<std::filesystem::path> ExistingCppIncludes;
        std::set<std::string> ExistingNativeIncludes;

        for (const auto& Include : Project->CppInputs)
          ExistingCppIncludes.emplace(Absolutify(Include));
        for (const auto& Include : Project->NativeInclude)
          ExistingNativeIncludes.emplace(Include);

        for (auto& Source : InteractiveView->GetSelectedSources()) {
          if (!ExistingCppIncludes.count(Absolutify(Source.Path))) {
            std::filesystem::path Path{Source.Path};
            Path = std::filesystem::weakly_canonical(Path).
                make_preferred().string();
            Project->CppInputs.emplace_back(Path.string());
          }

          if (!ExistingNativeIncludes.count(Source.Module)) {
            Project->NativeInclude.emplace_back(Source.Module);
          }
        }

        Command->SaveProject(*Project, CURRENT_DIRECTORY);
      } catch (const std::exception& Exc) {
        l->error("Exception in init-saving thread: {}", Exc.what());
        InteractiveView->SetStatus(InitView::Status::FAILED,
            fmt::format("Unexpected error: {}", Exc.what()));
        InteractiveView->SetFinished(true);
        IsDone = true;
        return;
      }

      InteractiveView->SetStatus(InitView::Status::FINISHED, "");
      InteractiveView->SetFinished(true);
      IsDone = true;
    });
}

void udocs_processor::InitCLI::SetView(
    std::unique_ptr<InitView> InteractiveView) {
  this->InteractiveView = std::move(InteractiveView);
}
