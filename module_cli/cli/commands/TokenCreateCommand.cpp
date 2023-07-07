/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/TokenCreateCommand.h"
#include "udocs-processor/Scopes.h"

udocs_processor::TokenCreateCommand::TokenCreateCommand(
    std::shared_ptr<spdlog::sinks::sink> Sink,
    std::shared_ptr<OAuth2> Oauth2, std::shared_ptr<TokenService> Token)
    : Oauth2(Oauth2), Token(Token) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}

udocs_processor::TokenService::TokenData
    udocs_processor::TokenCreateCommand::Create(
        const CreateRequest &Request) const {
  static std::set<std::string> RequiredScopes = {SurrealApiScopes::TOKENS_READ,
      SurrealApiScopes::TOKENS_WRITE};
  OAuth2::OAuth2Access Access = Oauth2->Authorize(RequiredScopes);

  if (!Access.IsAuthorized) {
    throw std::domain_error{"Authorization failed"};
  }

  if (Access.Scopes != RequiredScopes) {
    throw std::domain_error{"One or more of required scopes weren't provided"};
  }

  TokenService::CreateTokenRequest CreateRequest{};
  CreateRequest.Scopes = {
      SurrealCloudScopes::PROJECTS_WRITE,
      SurrealCloudScopes::PROJECTS_READ,
      SurrealCloudScopes::DOCUMENTS_WRITE,
      SurrealCloudScopes::ORGANIZATIONS_READ,
      SurrealCloudScopes::ORGANIZATIONS_WRITE,
      SurrealCloudScopes::QUOTAS_READ};
  CreateRequest.HydraAccessToken = std::move(Access.AccessToken);

  TokenService::CreateTokenResponse Response = Token->Create(CreateRequest);
  if (Response.Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("Create token: {}/{}", Response.Status.GetCode(),
        Response.Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't issue a token: {}",
        Response.Status.GetMessageDescription())};
  }

  l->info("Successfuly issued token");

  return Response.Token;
}
