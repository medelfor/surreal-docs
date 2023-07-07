/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <grpcpp/channel.h>
#include <memory>
#include "surapi/Organization.grpc.pb.h"
#include "udocs-processor/services/OrganizationService.h"

namespace udocs_processor {
class OrganizationServiceImpl : public OrganizationService {
 public:
  OrganizationServiceImpl(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::shared_ptr<grpc::Channel> Channel);

  ApiStatus Create(
      const CreateOrganizationRequest &Request) const override;

  ListOrganizationsResponse List(
      const ListOrganizationsRequest &Request) const override;

  ApiStatus Delete(
      const DeleteOrganizationRequest &Request) const override;

 private:
  std::shared_ptr<spdlog::logger> l;
  std::unique_ptr<surapi::Organization::Stub> Organization;

  constexpr static char LOGGER_NAME[] = "cloud-layer";
};
}  // namespace udocs_processor
