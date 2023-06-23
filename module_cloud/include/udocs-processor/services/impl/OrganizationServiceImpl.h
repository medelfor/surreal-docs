/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <memory>
#include <spdlog/logger.h>
#include <grpcpp/channel.h>
#include "surapi/Organization.grpc.pb.h"
#include "udocs-processor/services/OrganizationService.h"

namespace udocs_processor {
class OrganizationServiceImpl : public OrganizationService {
 public:
  explicit OrganizationServiceImpl(std::shared_ptr<grpc::Channel> Channel);

  ApiStatus Create(
      const CreateOrganizationRequest &Request) const override;

  ListOrganizationsResponse List(
      const ListOrganizationsRequest &Request) const override;

  ApiStatus Delete(
      const DeleteOrganizationRequest &Request) const override;

 private:
  std::shared_ptr<spdlog::logger> l;
  std::unique_ptr<surapi::Organization::Stub> Organization;

  constexpr static char LOGGER_NAME[] = "medelfor-surapi";
};
}  // namespace udocs_processor
