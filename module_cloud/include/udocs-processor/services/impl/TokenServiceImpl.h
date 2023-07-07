/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <grpcpp/channel.h>
#include <spdlog/logger.h>
#include <memory>
#include <string>
#include "surapi/Token.grpc.pb.h"
#include "udocs-processor/services/TokenService.h"

namespace udocs_processor {
class TokenServiceImpl : public TokenService {
 public:
  TokenServiceImpl(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::shared_ptr<grpc::Channel> Channel);

  CreateTokenResponse Create(const CreateTokenRequest &Request) const override;

  ListTokensResponse List(const std::string &HydraAccessToken) const override;

 private:
  std::shared_ptr<spdlog::logger> l;
  std::unique_ptr<surapi::Token::Stub> Token;

  constexpr static char LOGGER_NAME[] = "cloud-layer";
};
}  // namespace udocs_processor
