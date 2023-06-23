/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <memory>
#include <spdlog/logger.h>
#include <grpcpp/channel.h>
#include <grpcpp/channel.h>
#include "surapi/Token.grpc.pb.h"
#include "udocs-processor/services/TokenService.h"

namespace udocs_processor {
class TokenServiceImpl : public TokenService {
 public:
  explicit TokenServiceImpl(std::shared_ptr<grpc::Channel> Channel);

  CreateTokenResponse Create(const CreateTokenRequest &Request) const override;

  ListTokensResponse List(const std::string &HydraAccessToken) const override;

 private:
  std::shared_ptr<spdlog::logger> l;
  std::unique_ptr<surapi::Token::Stub> Token;

  constexpr static char LOGGER_NAME[] = "medelfor-surapi";
};
}  // namespace udocs_processor
