/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include <udocs-processor/Scopes.h>
#include <udocs-processor/services/impl/TokenServiceImpl.h>

udocs_processor::TokenService::CreateTokenResponse
    udocs_processor::TokenServiceImpl::Create(
    const udocs_processor::TokenService::CreateTokenRequest &Request) const {
  grpc::ClientContext Context;
  surapi::CreateTokenRequest CreateRequest;
  CreateRequest.set_hydraaccesstoken(Request.HydraAccessToken);
  CreateRequest.mutable_scopes()->Add(Request.Scopes.begin(),
      Request.Scopes.end());

  surapi::CreateTokenResponse Response;
  grpc::Status Status = Token->Create(&Context, CreateRequest, &Response);
  if (!Status.ok()) {
    return {{ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (create token) {}",
            Status.error_code())}};
  }

  TokenService::CreateTokenResponse Token;
  Token.Status = {Response.status().code(), Response.status().message()};
  Token.Token.Token = Response.token().token();
  Token.Token.Basic.ExpiresAt = Response.token().basic().expiresatseconds();
  Token.Token.Basic.Scopes.insert(Token.Token.Basic.Scopes.end(),
      std::move_iterator(Response.mutable_token()->mutable_basic()->
          mutable_scopes()->begin()),
      std::move_iterator(Response.mutable_token()->mutable_basic()->
          mutable_scopes()->end()));

  return Token;
}

udocs_processor::TokenService::ListTokensResponse
    udocs_processor::TokenServiceImpl::List(
    const std::string &HydraAccessToken) const {
  grpc::ClientContext Context;
  surapi::ListTokensRequest ListRequest;
  ListRequest.set_hydraaccesstoken(HydraAccessToken);

  surapi::ListTokensResponse Response;
  grpc::Status Status = Token->List(&Context, ListRequest, &Response);
  if (!Status.ok()) {
    return {{ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (list tokens) {}",
            Status.error_code())}};
  }

  TokenService::ListTokensResponse Tokens;
  Tokens.Status = {Response.status().code(), Response.status().message()};
  Tokens.Tokens.reserve(Response.tokens().size());
  for (auto& Token : *Response.mutable_tokens()) {
    TokenService::StrippedTokenData TokenData{};
    TokenData.ExpiresAt = Token.expiresatseconds();
    TokenData.Scopes.insert(TokenData.Scopes.end(),
        std::move_iterator(Token.mutable_scopes()->begin()),
        std::move_iterator(Token.mutable_scopes()->end()));

    Tokens.Tokens.emplace_back(std::move(TokenData));
  }

  return Tokens;
}

udocs_processor::TokenServiceImpl::TokenServiceImpl(
    std::shared_ptr<grpc::Channel> Channel) {
  Token = std::make_unique<surapi::Token::Stub>(Channel);
  l = spdlog::get(LOGGER_NAME);
}
