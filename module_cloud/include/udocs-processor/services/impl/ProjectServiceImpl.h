/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include "udocs-processor/services/ProjectService.h"
#include "surapi/Project.grpc.pb.h"
#include <memory>
#include <grpcpp/channel.h>
#include <grpcpp/channel.h>
#include <spdlog/logger.h>

namespace udocs_processor {
class ProjectServiceImpl : public ProjectService {
 public:
  explicit ProjectServiceImpl(std::shared_ptr<grpc::Channel> Channel);

  ApiStatus Create(const CreateProjectRequest &Request) const override;

  ApiStatus Publish(const PublishProjectRequest &Request) const override;

  ListProjectsResponse List(
      const ListProjectsRequest &Request) const override;

  ApiStatus Delete(const DeleteProjectRequest &Request) const override;

  ApiStatus AddCollaborator(
      const AddCollaboratorRequest &Request) const override;

  ListCollaboratorsResponse ListCollaborators(
      const ListCollaboratorsRequest &Request) const override;

  ApiStatus RemoveCollaborator(
      const RemoveCollaboratorRequest &Request) const override;

  ApiStatus SetAnnouncement(
      const SetAnnouncementRequest &Request) const override;

  ListVersionsResponse ListVersions(
      const ListVersionsRequest &Request) const override;

  ApiStatus CreateVersion(
      const CreateVersionRequest &Request) const override;

 private:
  std::shared_ptr<spdlog::logger> l;
  std::unique_ptr<surapi::Project::Stub> Project;

  std::map<enum class PublishProjectRequest::Scope, surapi::ProjectScope>
      ScopesToScopes;

  constexpr static char LOGGER_NAME[] = "medelfor-surapi";
};
}  // namespace udocs_processor
