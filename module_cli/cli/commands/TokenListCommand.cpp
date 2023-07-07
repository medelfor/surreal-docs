/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/TokenListCommand.h"
#include "udocs-processor/Scopes.h"

udocs_processor::TokenListCommand::TokenListCommand(
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

std::vector<udocs_processor::TokenService::StrippedTokenData>
    udocs_processor::TokenListCommand::List(
        const ListRequest &Request) const {
  static std::set<std::string> RequiredScopes = {SurrealApiScopes::TOKENS_READ};
  OAuth2::OAuth2Access Access = Oauth2->Authorize(RequiredScopes);

  if (!Access.IsAuthorized) {
    throw std::domain_error{"Authorization failed"};
  }

  if (Access.Scopes != RequiredScopes) {
    throw std::domain_error{"One or more of required scopes weren't provided"};
  }

  TokenService::ListTokensResponse Response = Token->List(Access.AccessToken);
  if (Response.Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("List token: {}/{}", Response.Status.GetCode(),
        Response.Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't list the tokens: {}",
        Response.Status.GetMessageDescription())};
  }

  l->info("Successfuly listed tokens");

  return Response.Tokens;
}
