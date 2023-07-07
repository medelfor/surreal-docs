/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <udocs-processor/ApiStatus.h>
#include <map>
#include <string>
#include <vector>
#include <optional>

namespace udocs_processor {
class ProjectService {
 public:
  struct Address {
    std::string Project;
    std::string Organization;
  };

  struct CreateProjectRequest {
    std::string Token;
    Address Location;
  };

  struct PublishProjectRequest {
    enum class Scope {
      PRIVATE,
      PUBLIC
    } Scope = Scope::PUBLIC;

    Address Location;
    std::string Token;
  };

  struct DeleteProjectRequest {
    Address Location;
    std::string Token;
    std::optional<std::string> Version;
    bool DeleteProject = false;
  };

  struct ListProjectsRequest {
    std::string Organization;
    std::string Token;
  };

  struct ProjectData {
    std::string Name;
    bool IsPublic = false;
  };

  struct ListProjectsResponse {
    ApiStatus Status;
    std::vector<ProjectData> Projects;
  };

  struct AddCollaboratorRequest {
    Address Location;
    std::string Token;
    std::string Email;
  };

  struct ListCollaboratorsRequest {
    Address Location;
    std::string Token;
  };

  struct ListCollaboratorsResponse {
    ApiStatus Status;
    std::vector<std::string> Emails;
  };

  struct RemoveCollaboratorRequest {
    Address Location;
    std::string Token;
    std::string Email;
  };

  struct SetAnnouncementRequest {
    Address Location;
    std::string Token;
    std::string Announcement;
  };

  struct ListVersionsRequest {
    Address Location;
    std::string Token;
  };

  struct ListVersionsResponse {
    ApiStatus Status;
    std::vector<std::string> Versions;
  };

  struct CreateVersionRequest {
    Address Location;
    std::string Token;
    std::string Version;
  };

  struct DeployPreCheckRequest {
    Address Location;
    std::string Token;
    std::string Version;
  };

  struct DeployPreCheckResponse {
    ApiStatus Status;
    bool DoesVersionExist = false;
    std::optional<int64_t> RemainingSpace = 0;
  };

  virtual ApiStatus Create(const CreateProjectRequest& Request) const = 0;

  virtual ApiStatus Publish(
      const PublishProjectRequest& Request) const = 0;

  virtual ListProjectsResponse List(
      const ListProjectsRequest& Request) const = 0;

  virtual ApiStatus Delete(const DeleteProjectRequest& Request) const = 0;

  virtual ApiStatus AddCollaborator(
      const AddCollaboratorRequest& Request) const = 0;

  virtual ListCollaboratorsResponse ListCollaborators(
      const ListCollaboratorsRequest& Request) const = 0;

  virtual ApiStatus RemoveCollaborator(
      const RemoveCollaboratorRequest& Request) const = 0;

  virtual ApiStatus SetAnnouncement(
      const SetAnnouncementRequest& Request) const = 0;

  virtual ListVersionsResponse ListVersions(
      const ListVersionsRequest& Request) const = 0;

  virtual ApiStatus CreateVersion(
      const CreateVersionRequest& Request) const = 0;

  virtual DeployPreCheckResponse PerformDeployPreCheck(
      const DeployPreCheckRequest& Request) const = 0;
};
}  // namespace udocs_processor
