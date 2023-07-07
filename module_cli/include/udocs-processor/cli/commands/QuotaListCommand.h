/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <vector>
#include <string>
#include "udocs-processor/services/QuotaService.h"

namespace udocs_processor {
class QuotaListCommand {
 public:
  struct ListRequest {
    std::string Token;
  };

  QuotaListCommand(std::shared_ptr<QuotaService> Quota,
      std::shared_ptr<spdlog::sinks::sink> Sink);

  std::vector<QuotaService::QuotaResource> List(
      const ListRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "quota-list";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<QuotaService> Quota;
};
}  // namespace udocs_processor
