/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <udocs-processor/ApiStatus.h>
#include <map>
#include <string>
#include <vector>

namespace udocs_processor {
class OrganizationService {
 public:
  struct CreateOrganizationRequest {
    std::string Token;
    std::string Name;
  };

  struct DeleteOrganizationRequest {
    std::string Token;
    std::string Name;
  };

  struct ListOrganizationsRequest {
    std::string Token;
  };

  struct ListOrganizationsResponse {
    ApiStatus Status;
    std::vector<std::string> Organizations;
  };

  virtual ApiStatus Create(
      const CreateOrganizationRequest& Request) const = 0;

  virtual ListOrganizationsResponse List(
      const ListOrganizationsRequest& Request) const = 0;

  virtual ApiStatus Delete(
      const DeleteOrganizationRequest& Request) const = 0;
};
}  // namespace udocs_processor
