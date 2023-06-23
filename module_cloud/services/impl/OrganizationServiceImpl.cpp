/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include <udocs-processor/Scopes.h>
#include <udocs-processor/services/impl/OrganizationServiceImpl.h>
#include <regex> NOLINT()

udocs_processor::OrganizationServiceImpl::OrganizationServiceImpl(
    std::shared_ptr<grpc::Channel> Channel) {
  Organization = std::make_unique<surapi::Organization::Stub>(Channel);
  l = spdlog::get(LOGGER_NAME);
}

udocs_processor::ApiStatus udocs_processor::OrganizationServiceImpl::Create(
    const CreateOrganizationRequest &Request) const {
  grpc::ClientContext Context;
  surapi::CreateOrganizationRequest CreateRequest;
  CreateRequest.set_token(Request.Token);
  CreateRequest.set_name(Request.Name);

  api::Status Response;
  grpc::Status Status = Organization->Create(
      &Context, CreateRequest, &Response);
  if (!Status.ok()) {
    return {ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (create organization) {}",
            Status.error_code())};
  }

  return {Response.code(), Response.message()};
}

udocs_processor::OrganizationService::ListOrganizationsResponse
    udocs_processor::OrganizationServiceImpl::List(
        const ListOrganizationsRequest &Request) const {
  grpc::ClientContext Context;
  surapi::ListOrganizationsRequest ListRequest;
  ListRequest.set_token(Request.Token);

  surapi::ListOrganizationsResponse Response;
  grpc::Status Status = Organization->List(&Context, ListRequest, &Response);
  if (!Status.ok()) {
    return {{ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (list organizations) {}",
            Status.error_code())}};
  }

  std::vector<std::string> Organizations;
  Organizations.insert(Organizations.end(),
      std::move_iterator(Response.mutable_organizationname()->begin()),
      std::move_iterator(Response.mutable_organizationname()->end()));
  return {{Response.status().code(), Response.status().message()},
      std::move(Organizations)};
}

udocs_processor::ApiStatus udocs_processor::OrganizationServiceImpl::Delete(
    const DeleteOrganizationRequest &Request) const {
  grpc::ClientContext Context;
  surapi::DeleteOrganizationRequest DeleteRequest;
  DeleteRequest.set_token(Request.Token);
  DeleteRequest.set_name(Request.Name);

  api::Status Response;
  grpc::Status Status = Organization->Delete(
      &Context, DeleteRequest, &Response);
  if (!Status.ok()) {
    return {ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (delete organization) {}",
            Status.error_code())};
  }

  return {Response.code(), Response.message()};
}
