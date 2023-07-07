/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <grpcpp/channel.h>
#include <memory>
#include <string>
#include "surapi/Quota.grpc.pb.h"
#include "udocs-processor/services/QuotaService.h"

namespace udocs_processor {
class QuotaServiceImpl : public QuotaService {
 public:
  QuotaServiceImpl(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::shared_ptr<grpc::Channel> Channel);

  ListQuotaResponse List(const std::string &Token) const override;

 private:
  std::shared_ptr<spdlog::logger> l;
  std::unique_ptr<surapi::Quota::Stub> Quota;

  constexpr static char LOGGER_NAME[] = "cloud-layer";
};
}  // namespace udocs_processor
