/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include <udocs-processor/Scopes.h>
#include <udocs-processor/services/impl/ProjectServiceImpl.h>

udocs_processor::ApiStatus udocs_processor::ProjectServiceImpl::Create(
    const CreateProjectRequest &Request) const {
  grpc::ClientContext Context;
  surapi::CreateProjectRequest CreateRequest;
  CreateRequest.set_token(Request.Token);
  auto* Location = new surapi::Address;
  Location->set_organization(Request.Location.Organization);
  Location->set_project(Request.Location.Project);
  CreateRequest.set_allocated_location(Location);

  api::Status Response;
  grpc::Status Status = Project->Create(
      &Context, CreateRequest, &Response);
  if (!Status.ok()) {
    return {ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (create project) {}",
            Status.error_code())};
  }

  return {Response.code(), Response.message()};
}

udocs_processor::ApiStatus udocs_processor::ProjectServiceImpl::Publish(
    const PublishProjectRequest &Request) const {
  grpc::ClientContext Context;
  surapi::PublishProjectRequest PublishRequest;
  PublishRequest.set_token(Request.Token);
  auto* Location = new surapi::Address;
  Location->set_organization(Request.Location.Organization);
  Location->set_project(Request.Location.Project);
  PublishRequest.set_allocated_location(Location);

  if (auto It = ScopesToScopes.find(Request.Scope);
      It != ScopesToScopes.end()) {
    PublishRequest.set_scope(It->second);
  } else {
    l->error("Undefined scope {}", static_cast<uint32_t>(Request.Scope));
    throw std::invalid_argument{"Invalid scope to publish the project in"};
  }

  api::Status Response;
  grpc::Status Status = Project->Publish(
      &Context, PublishRequest, &Response);
  if (!Status.ok()) {
    return {ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (publish project) {}",
            Status.error_code())};
  }

  return {Response.code(), Response.message()};
}

udocs_processor::ProjectService::ListProjectsResponse
    udocs_processor::ProjectServiceImpl::List(
    const ListProjectsRequest &Request) const {
  grpc::ClientContext Context;
  surapi::ListProjectsRequest ListRequest;
  ListRequest.set_organization(Request.Organization);
  ListRequest.set_token(Request.Token);

  surapi::ListProjectsResponse Response;
  grpc::Status Status = Project->List(&Context, ListRequest,
      &Response);
  if (!Status.ok()) {
    return {{ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (list projects) {}",
            Status.error_code())}};
  }

  std::vector<ProjectService::ProjectData> Data;
  Data.reserve(Response.projects().size());
  for (auto& Project : *Response.mutable_projects()) {
    Data.emplace_back(ProjectService::ProjectData{
      std::move(*Project.mutable_name()), Project.ispublic()});
  }
  return {{Response.status().code(), Response.status().message()},
      std::move(Data)};
}

udocs_processor::ApiStatus udocs_processor::ProjectServiceImpl::Delete(
    const DeleteProjectRequest &Request) const {
  grpc::ClientContext Context;
  surapi::DeleteProjectRequest DeleteRequest;
  DeleteRequest.set_token(Request.Token);
  auto* Location = new surapi::Address;
  Location->set_organization(Request.Location.Organization);
  Location->set_project(Request.Location.Project);
  DeleteRequest.set_allocated_location(Location);
  if (Request.Version) {
    DeleteRequest.set_version(*Request.Version);
  } else {
    DeleteRequest.clear_version();
  }
  DeleteRequest.set_deleteproject(Request.DeleteProject);

  api::Status Response;
  grpc::Status Status = Project->Delete(
      &Context, DeleteRequest, &Response);
  if (!Status.ok()) {
    return {ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (delete project) {}",
            Status.error_code())};
  }

  return {Response.code(), Response.message()};
}

udocs_processor::ApiStatus udocs_processor::ProjectServiceImpl::AddCollaborator(
    const AddCollaboratorRequest &Request) const {
  grpc::ClientContext Context;
  surapi::AddCollaboratorRequest AddRequest;
  AddRequest.set_token(Request.Token);
  auto* Location = new surapi::Address;
  Location->set_organization(Request.Location.Organization);
  Location->set_project(Request.Location.Project);
  AddRequest.set_allocated_location(Location);
  AddRequest.set_collaboratoremail(Request.Email);

  api::Status Response;
  grpc::Status Status = Project->AddCollaborator(
      &Context, AddRequest, &Response);
  if (!Status.ok()) {
    return {ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (add collaborator) {}",
            Status.error_code())};
  }

  return {Response.code(), Response.message()};
}

udocs_processor::ProjectService::ListCollaboratorsResponse
    udocs_processor::ProjectServiceImpl::ListCollaborators(
    const ListCollaboratorsRequest &Request) const {
  grpc::ClientContext Context;
  surapi::ListCollaboratorsRequest ListRequest;
  auto* Location = new surapi::Address;
  Location->set_organization(Request.Location.Organization);
  Location->set_project(Request.Location.Project);
  ListRequest.set_allocated_location(Location);
  ListRequest.set_token(Request.Token);

  surapi::ListCollaboratorsResponse Response;
  grpc::Status Status = Project->ListCollaborators(&Context, ListRequest,
      &Response);
  if (!Status.ok()) {
    return {{ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (list collaborators) {}",
            Status.error_code())}};
  }

  std::vector<std::string> Collaborators;
  Collaborators.insert(Collaborators.end(),
      std::move_iterator(Response.mutable_collaboratoremails()->begin()),
      std::move_iterator(Response.mutable_collaboratoremails()->end()));
  return {{Response.status().code(), Response.status().message()},
      std::move(Collaborators)};
}

udocs_processor::ApiStatus
    udocs_processor::ProjectServiceImpl::RemoveCollaborator(
    const RemoveCollaboratorRequest &Request) const {
  grpc::ClientContext Context;
  surapi::RemoveCollaboratorRequest RemoveRequest;
  RemoveRequest.set_token(Request.Token);
  auto* Location = new surapi::Address;
  Location->set_organization(Request.Location.Organization);
  Location->set_project(Request.Location.Project);
  RemoveRequest.set_allocated_location(Location);
  RemoveRequest.set_collaboratoremail(Request.Email);

  api::Status Response;
  grpc::Status Status = Project->RemoveCollaborator(
      &Context, RemoveRequest, &Response);
  if (!Status.ok()) {
    return {ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (remove collaborator) {}",
            Status.error_code())};
  }

  return {Response.code(), Response.message()};
}

udocs_processor::ApiStatus udocs_processor::ProjectServiceImpl::SetAnnouncement(
    const SetAnnouncementRequest &Request) const {
  grpc::ClientContext Context;
  surapi::SetAnnouncementRequest SetRequest;
  SetRequest.set_token(Request.Token);
  auto* Location = new surapi::Address;
  Location->set_organization(Request.Location.Organization);
  Location->set_project(Request.Location.Project);
  SetRequest.set_allocated_location(Location);
  SetRequest.set_announcement(Request.Announcement);

  api::Status Response;
  grpc::Status Status = Project->SetAnnouncement(
      &Context, SetRequest, &Response);
  if (!Status.ok()) {
    return {ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (set announcement) {}",
            Status.error_code())};
  }

  return {Response.code(), Response.message()};
}

udocs_processor::ProjectService::ListVersionsResponse
    udocs_processor::ProjectServiceImpl::ListVersions(
        const ListVersionsRequest &Request) const {
  grpc::ClientContext Context;
  surapi::ListVersionsRequest ListRequest;
  auto* Location = new surapi::Address;
  Location->set_organization(Request.Location.Organization);
  Location->set_project(Request.Location.Project);
  ListRequest.set_allocated_location(Location);
  ListRequest.set_token(Request.Token);

  surapi::ListVersionsResponse Response;
  grpc::Status Status = Project->ListVersions(&Context, ListRequest, &Response);
  if (!Status.ok()) {
    return {{ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (list versions) {}",
            Status.error_code())}};
  }

  std::vector<std::string> Versions;
  Versions.insert(Versions.end(),
      std::move_iterator(Response.mutable_versions()->begin()),
      std::move_iterator(Response.mutable_versions()->end()));
  return {{Response.status().code(), Response.status().message()},
      std::move(Versions)};
}

udocs_processor::ApiStatus udocs_processor::ProjectServiceImpl::CreateVersion(
    const CreateVersionRequest &Request) const {
  grpc::ClientContext Context;
  surapi::CreateVersionRequest CreateRequest;
  CreateRequest.set_token(Request.Token);
  auto* Location = new surapi::Address;
  Location->set_organization(Request.Location.Organization);
  Location->set_project(Request.Location.Project);
  CreateRequest.set_allocated_location(Location);
  CreateRequest.set_version(Request.Version);

  api::Status Response;
  grpc::Status Status = Project->CreateVersion(
      &Context, CreateRequest, &Response);
  if (!Status.ok()) {
    return {ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (create version) {}",
            Status.error_code())};
  }

  return {Response.code(), Response.message()};
}

udocs_processor::ProjectServiceImpl::ProjectServiceImpl(
    std::shared_ptr<grpc::Channel> Channel) {
  Project = std::make_unique<surapi::Project::Stub>(Channel);
  l = spdlog::get(LOGGER_NAME);

  ScopesToScopes.insert(std::make_pair(PublishProjectRequest::Scope::PRIVATE,
      surapi::ProjectScope::PRIVATE));
  ScopesToScopes.insert(std::make_pair(PublishProjectRequest::Scope::PUBLIC,
      surapi::ProjectScope::PUBLIC));
}
