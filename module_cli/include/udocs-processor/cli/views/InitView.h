/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <set>
#include <string>
#include "View.h"
#include "udocs-processor/cli/ue/UnrealInteraction.h"

namespace udocs_processor {
class InitView : public virtual View {
 public:
  enum class Status {
    GATHERING,
    STAND_BY,
    PROCESSING,
    FINISHED,
    FAILED
  };

  virtual void SetStatus(Status CurrentStatus, const std::string& Message) = 0;

  virtual void SetEngineVersions(
      std::vector<UnrealInteraction::UnrealVersion> Versions) = 0;

  virtual void SetTargets(
      std::map<std::string, std::string> Targets) = 0;

  virtual void SetSources(
      std::vector<UnrealInteraction::Source> Sources) = 0;

  virtual void SetName(std::string Name) = 0;

  virtual void SetVersion(std::string Version) = 0;

  virtual void SetOrganization(std::string Organization) = 0;

  virtual void SetContactMeAt(std::string ContactMeAt) = 0;

  virtual void SelectEngineVersion(int VersionIndex) = 0;

  virtual void SelectTarget(const std::string& Target) = 0;

  virtual void SelectSources(const std::set<int> &Sources) = 0;

  virtual const std::string& GetSelectedName() const = 0;

  virtual const std::string& GetSelectedOrganization() const = 0;

  virtual const std::string& GetContactMeAt() const = 0;

  virtual const std::string& GetSelectedVersion() const = 0;

  virtual std::vector<UnrealInteraction::Source> GetSelectedSources() const = 0;

  virtual std::set<int> GetSelectedSourceIndices() const = 0;

  virtual std::optional<UnrealInteraction::UnrealVersion>
      GetSelectedEngineVersion() const = 0;

  virtual std::optional<std::string> GetSelectedTarget() const = 0;

  virtual const std::string& GetSelectedConfiguration() const = 0;

  virtual void SetDoExportPrivate(bool DoExport) = 0;

  virtual bool DoExportPrivate() const = 0;

  ~InitView() override = default;
};
}  // namespace udocs_processor
