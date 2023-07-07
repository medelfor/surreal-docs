/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPClientSession.h>
#include <memory>
#include <set>
#include <string>
#include "OAuth2.h"

namespace udocs_processor {
class HydraOAuth2 : public OAuth2 {
 public:
  HydraOAuth2(std::shared_ptr<spdlog::sinks::sink> Sink,
      uint64_t Port, std::string OAuth2ClientId,
      std::string OAuth2Host, uint64_t OAuth2Port, bool DoUseHttps,
      std::string ResourcesPath);

  OAuth2Access Authorize(const std::set<std::string>& Scopes) override;

 private:
  class CallbackRequestFactory : public Poco::Net::HTTPRequestHandlerFactory {
   public:
    class CallbackHandler : public Poco::Net::HTTPRequestHandler {
     public:
      CallbackHandler(std::function<void(OAuth2Access)> StopServer,
          std::string Verifier, std::string State,
          std::shared_ptr<spdlog::logger> l, std::string OAuth2ClientId,
          std::string OAuth2Host, uint64_t OAuth2Port, bool DoUseHttps,
          std::string CallbackPage);

      void handleRequest(Poco::Net::HTTPServerRequest &Request,
          Poco::Net::HTTPServerResponse &Response) override;

      OAuth2Access ExchangeCode(std::string Code,
          std::string CodeVerifier) const;

     private:
      std::shared_ptr<Poco::Net::HTTPClientSession> MakeClientSession(
          const std::string& Host, int Port) const;

      std::function<void(OAuth2Access)> StopServer;
      std::string Verifier;
      std::string State;
      std::string OAuth2ClientId;
      std::string OAuth2Host;
      std::string CallbackPage;
      uint64_t OAuth2Port = 0;
      bool DoUseHttps = false;
      std::shared_ptr<spdlog::logger> l;

      constexpr static char MIME_HTML[] = "text/html";
    };

    CallbackRequestFactory(std::function<void(OAuth2Access)> StopServer,
        std::string Verifier, std::string State,
        std::shared_ptr<spdlog::logger> l, std::string OAuth2ClientId,
        std::string OAuth2Host, uint64_t OAuth2Port, bool DoUseHttps,
        std::string CallbackPage);

    Poco::Net::HTTPRequestHandler* createRequestHandler(
        const Poco::Net::HTTPServerRequest &Request) override;

   private:
    std::function<void(OAuth2Access)> StopServer;
    std::string Verifier;
    std::string State;
    std::string OAuth2ClientId;
    std::string OAuth2Host;
    std::string CallbackPage;
    uint64_t OAuth2Port = 0;
    bool DoUseHttps = false;
    std::shared_ptr<spdlog::logger> l;
  };

  OAuth2Access StartServer(const std::set<std::string>& Scopes);

  bool OpenBrowser(const std::set<std::string>& Scopes,
      const std::string& Verifier, const std::string& State) const;

  std::string MakeChallenge(const std::string& Verifier) const;

  std::string GenerateCode(uint64_t Size) const;

  uint64_t Port = 0;
  std::string OAuth2ClientId;
  std::string OAuth2Host;
  std::string ResourcesPath;
  std::string CallbackPage;
  uint64_t OAuth2Port = 0;
  bool DoUseHttps = false;

  std::shared_ptr<spdlog::logger> l;

  std::condition_variable FinishedCv;

  static constexpr int MAX_QUEUED = 10;
  static constexpr int MAX_THREADS = 2;
  static constexpr int CHALLENGE_VERIFIER_SIZE = 32;
  static constexpr int STATE_SIZE = 16;

  constexpr static char CALLBACK_PAGE_DEFAULT[] =
      "Thank you! We've received the login information. You can now close "
      "the tab.";
  constexpr static char CALLBACK_PAGE_PATH[] =
      "html/udocs_processor/oauth2_callback.html";

  constexpr static char QUERY_STATE[] = "state";
  constexpr static char QUERY_CODE[] = "code";
  constexpr static char QUERY_ERROR[] = "error";

  static constexpr char HYDRA_TOKEN_ENDPOINT[] = "/oauth2/token";
  static constexpr char HYDRA_TOKEN_ENDPOINT_RESPONSE_ACCESS_TOKEN[] =
      "access_token";
  static constexpr char HYDRA_TOKEN_ENDPOINT_RESPONSE_SCOPE[] = "scope";
  static constexpr char HYDRA_TOKEN_ENDPOINT_CODE[] = "code";
  static constexpr char HYDRA_TOKEN_ENDPOINT_CLIENT_ID[] = "client_id";
  static constexpr char HYDRA_TOKEN_ENDPOINT_CODE_VERIFIER[] = "code_verifier";
  static constexpr char HYDRA_TOKEN_ENDPOINT_GRANT_TYPE[] = "grant_type";
  static constexpr char HYDRA_TOKEN_ENDPOINT_GRANT_TYPE_AUTH_CODE[] =
      "authorization_code";

  static constexpr char AUTHORIZATION_URL[] =
      "{}://{}:{}/oauth2/auth?response_type=code&client_id={}&scope={}"
      "&state={}&code_challenge_method=S256&code_challenge={}";

  static constexpr char ORY_ERROR_ERROR[] = "error";

  static constexpr char PROTOCOL_HTTP[] = "http";
  static constexpr char PROTOCOL_HTTPS[] = "https";

  static constexpr const char LOGGER_NAME[] = "hydra-oauth2";
  static constexpr const char OPEN_COMMAND[] = R"(start "" "{}")";
  static constexpr const char SCOPES_GLUE = ' ';
};
}  // namespace udocs_processor
