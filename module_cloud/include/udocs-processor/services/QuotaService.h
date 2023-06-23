/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <udocs-processor/ApiStatus.h>
#include <map>
#include <string>
#include <optional>
#include <vector>

namespace udocs_processor {
class QuotaService {
 public:
  struct QuotaResource {
    std::string Name;
    uint64_t Used = 0;
    std::optional<uint64_t> Max;
  };

  struct ListQuotaResponse {
    ApiStatus Status;
    std::vector<QuotaResource> Resources;
  };

  virtual ListQuotaResponse List(const std::string& Token) const = 0;

  constexpr static char QUOTA_MAX_ORGS[] = "MAX_ORGANIZATIONS";
  constexpr static char QUOTA_MAX_DOC_SIZE[] = "MAX_DOCUMENT_SIZE";
  constexpr static char QUOTA_MAX_DATA_SIZE[] = "MAX_TOTAL_SIZE";
};
}  // namespace udocs_processor
