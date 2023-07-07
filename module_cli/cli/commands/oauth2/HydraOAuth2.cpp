/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/commands/oauth2/HydraOAuth2.h"
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/URI.h>
#include <cryptopp/sha.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>
#include <cryptopp/hex.h>
#include <spdlog/spdlog.h>
#include <optional>
#include <fstream>
#include <nlohmann/json.hpp>
#include "udocs-processor/ExecStreamHelper.h"
#include "udocs-processor/StringHelper.h"

udocs_processor::HydraOAuth2::HydraOAuth2(
    std::shared_ptr<spdlog::sinks::sink> Sink,
    uint64_t Port, std::string OAuth2ClientId, std::string OAuth2Host,
    uint64_t OAuth2Port, bool DoUseHttps,
    std::string ResourcesPath) : Port(Port),
    OAuth2Host(std::move(OAuth2Host)), OAuth2Port(OAuth2Port),
    DoUseHttps(DoUseHttps), OAuth2ClientId(std::move(OAuth2ClientId)),
    ResourcesPath(std::move(ResourcesPath)) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }

  std::string Path = StringHelper::Normalize(this->ResourcesPath) +
      CALLBACK_PAGE_PATH;
  std::ifstream CallbackPageStream{Path};
  if (CallbackPageStream.good()) {
    std::stringstream CallbackPageBuffer;
    CallbackPageBuffer << CallbackPageStream.rdbuf();

    CallbackPage = CallbackPageBuffer.str();
  } else {
    l->error("Couldn't load {}", Path);
    CallbackPage = CALLBACK_PAGE_DEFAULT;
  }
}

udocs_processor::OAuth2::OAuth2Access
udocs_processor::HydraOAuth2::Authorize(
    const std::set<std::string>& Scopes) {
  /*
   * 1. run the server
   * 2. open browser
   * 3. wait for callback
   * 4. exchange code
   * 5. return results
   */
  return StartServer(Scopes);
}

udocs_processor::OAuth2::OAuth2Access
    udocs_processor::HydraOAuth2::StartServer(
        const std::set<std::string>& Scopes) {
  auto* ServerParams = new Poco::Net::HTTPServerParams();

  ServerParams->setMaxQueued(MAX_QUEUED);
  ServerParams->setMaxThreads(MAX_THREADS);

  std::string ChallengeVerifier = GenerateCode(CHALLENGE_VERIFIER_SIZE);
  std::string State = GenerateCode(STATE_SIZE);

  std::mutex ResultMutex;
  OAuth2Access Result;
  bool Finished = false;

  Poco::SharedPtr<CallbackRequestFactory> RequestHandlerFactoryInstance =
      Poco::makeShared<CallbackRequestFactory>(
        [this, &Result, &Finished, &ResultMutex]
        (OAuth2Access Access) {
          l->info("Trying to write callback result");

          std::unique_lock<std::mutex> Lock{ResultMutex};
          if (Finished) return;

          FinishedCv.notify_all();
          Result = std::move(Access);
          Finished = true;

          l->info("Trying to write callback result");
        }, ChallengeVerifier, State, l, OAuth2ClientId, OAuth2Host, OAuth2Port,
        DoUseHttps, std::move(CallbackPage));

  Poco::Net::HTTPServer Server(RequestHandlerFactoryInstance,
      Poco::Net::ServerSocket(Poco::UInt16(Port)), ServerParams);

  l->info("Starting hydra-oauth2 local callback server");

  Server.start();

  if (!OpenBrowser(Scopes, ChallengeVerifier, State)) {
    return {false};
  }

  std::unique_lock<std::mutex> Lock{ResultMutex};
  FinishedCv.wait(Lock);

  l->info("Stopping hydra-oauth2 local callback server");
  Server.stopAll();

  l->info("Stopped hydra-oauth2 local callback server");

  return Result;
}

bool udocs_processor::HydraOAuth2::OpenBrowser(
    const std::set<std::string>& Scopes, const std::string& Verifier,
    const std::string& State) const {
  l->info("Opening browser");

  std::string AuthorizationUrl = fmt::format(AUTHORIZATION_URL,
    DoUseHttps ? PROTOCOL_HTTPS : PROTOCOL_HTTP, OAuth2Host, OAuth2Port,
    OAuth2ClientId, fmt::format("{}", fmt::join(Scopes,
        std::string(&SCOPES_GLUE, sizeof(SCOPES_GLUE)))),
        State, MakeChallenge(Verifier));

  std::string Command = fmt::format(OPEN_COMMAND, AuthorizationUrl);
  uint64_t ExitCode = std::system(Command.c_str());

  l->info("Browser opened with {} exit code", ExitCode);

  return !ExitCode;
}

Poco::Net::HTTPRequestHandler*
    udocs_processor::HydraOAuth2::CallbackRequestFactory::createRequestHandler(
        const Poco::Net::HTTPServerRequest &Request) {
  return new CallbackHandler(StopServer, Verifier, State, l, OAuth2ClientId,
      OAuth2Host, OAuth2Port, DoUseHttps, CallbackPage);
}

udocs_processor::HydraOAuth2::CallbackRequestFactory::
    CallbackRequestFactory(std::function<void(OAuth2Access)> StopServer,
        std::string Verifier, std::string State,
        std::shared_ptr<spdlog::logger> l, std::string OAuth2ClientId,
        std::string OAuth2Host, uint64_t OAuth2Port, bool DoUseHttps,
        std::string CallbackPage)
    : StopServer(std::move(StopServer)), Verifier(std::move(Verifier)),
    State(std::move(State)), l(l), OAuth2ClientId(std::move(OAuth2ClientId)),
    OAuth2Host(std::move(OAuth2Host)), OAuth2Port(OAuth2Port),
    DoUseHttps(DoUseHttps), CallbackPage(std::move(CallbackPage)) {}

void udocs_processor::HydraOAuth2::CallbackRequestFactory::CallbackHandler::
    handleRequest(Poco::Net::HTTPServerRequest &Request,
      Poco::Net::HTTPServerResponse &Response) {
  Poco::URI Uri(Request.getURI());

  std::optional<std::string> Code;
  std::optional<std::string> State;
  std::optional<std::string> Error;

  for (const auto& Param : Uri.getQueryParameters()) {
    if (Param.first == QUERY_STATE) {
      State = Param.second;
    } else if (Param.first == QUERY_CODE) {
      Code = Param.second;
    } else if (Param.first == QUERY_ERROR) {
      Error = Param.second;
    }
  }

  Response.setContentType(MIME_HTML);
  Response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
  Response.send() << CallbackPage;

  if (Error || !Code || !State || *State != this->State) {
    l->error("Couldn't authorize: one of the parameters is missing");
    if (Error) {
      l->error("Error: {}", *Error);
    }
  } else {
    StopServer(ExchangeCode(*Code, Verifier));
  }
}

std::shared_ptr<Poco::Net::HTTPClientSession>
    udocs_processor::HydraOAuth2::CallbackRequestFactory::CallbackHandler::
        MakeClientSession(const std::string& Host, int Port) const {
  std::shared_ptr<Poco::Net::HTTPClientSession> Session;

  if (DoUseHttps) {
    Session = std::make_shared<Poco::Net::HTTPSClientSession>(Host, Port);
  } else {
    Session = std::make_shared<Poco::Net::HTTPClientSession>(Host, Port);
  }

  return Session;
}

udocs_processor::HydraOAuth2::CallbackRequestFactory::CallbackHandler::
    CallbackHandler(std::function<void(OAuth2Access)> StopServer,
        std::string Verifier, std::string State,
        std::shared_ptr<spdlog::logger> l, std::string OAuth2ClientId,
        std::string OAuth2Host, uint64_t OAuth2Port, bool DoUseHttps,
        std::string CallbackPage)
    : StopServer(std::move(StopServer)), Verifier(std::move(Verifier)),
    State(std::move(State)), l(l), OAuth2ClientId(std::move(OAuth2ClientId)),
    OAuth2Host(std::move(OAuth2Host)), OAuth2Port(OAuth2Port),
    DoUseHttps(DoUseHttps), CallbackPage(std::move(CallbackPage)) {}

udocs_processor::OAuth2::OAuth2Access
    udocs_processor::HydraOAuth2::CallbackRequestFactory::CallbackHandler::
    ExchangeCode(std::string Code, std::string CodeVerifier) const {
  try {
    std::shared_ptr<Poco::Net::HTTPClientSession> HttpSession =
        MakeClientSession(OAuth2Host, OAuth2Port);
    Poco::Net::HTTPRequest Request(Poco::Net::HTTPRequest::HTTP_POST,
        HYDRA_TOKEN_ENDPOINT, Poco::Net::HTTPMessage::HTTP_1_1);

    Poco::Net::HTMLForm Form;
    Form.set(HYDRA_TOKEN_ENDPOINT_GRANT_TYPE,
        HYDRA_TOKEN_ENDPOINT_GRANT_TYPE_AUTH_CODE);
    Form.set(HYDRA_TOKEN_ENDPOINT_CODE, Code);
    Form.set(HYDRA_TOKEN_ENDPOINT_CLIENT_ID, OAuth2ClientId);
    Form.set(HYDRA_TOKEN_ENDPOINT_CODE_VERIFIER, CodeVerifier);
    Form.prepareSubmit(Request);

    Form.write(HttpSession->sendRequest(Request));

    Poco::Net::HTTPResponse Response;
    std::istream& ResponseStream = HttpSession->receiveResponse(Response);
    std::istreambuf_iterator<char> BufferIterator(ResponseStream);

    std::string ResponseText(BufferIterator, {});
    nlohmann::json ResponseData = nlohmann::json::parse(ResponseText);
    if (Response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
      std::string AccessToken =
          ResponseData[HYDRA_TOKEN_ENDPOINT_RESPONSE_ACCESS_TOKEN];
      std::string ScopesString =
          ResponseData[HYDRA_TOKEN_ENDPOINT_RESPONSE_SCOPE];
      std::set<std::string> Scopes;
      StringHelper::Split(ScopesString, SCOPES_GLUE,
          std::inserter(Scopes, Scopes.begin()));

      return {true, std::move(Scopes), AccessToken};
    } else {
      std::string Message = ResponseData[ORY_ERROR_ERROR];
      l->warn("Code exchange returned non-200: {}", Message);
    }
  } catch (Poco::Exception& Exc) {
    l->error("Exception exchanging code: {}", Exc.displayText());
  } catch (std::exception& exc) {
    l->error("Exception exchanging code: {}", exc.what());
  }

  return {false};
}

std::string udocs_processor::HydraOAuth2::GenerateCode(uint64_t Size) const {
  CryptoPP::SecByteBlock Code{Size};
  CryptoPP::AutoSeededRandomPool Pool{};

  Pool.GenerateBlock(Code, Size);

  std::string Hexed;
  CryptoPP::HexEncoder Hex(
    new CryptoPP::StringSink(Hexed));
  Hex.Put(Code.data(), Code.size());
  Hex.MessageEnd();

  return Hexed;
}

std::string udocs_processor::HydraOAuth2::MakeChallenge(
    const std::string& Verifier) const {
  std::string Digest;

  CryptoPP::SHA256 Hash;
  Hash.Update(reinterpret_cast<const CryptoPP::byte*>(Verifier.data()),
      Verifier.size());
  Digest.resize(Hash.DigestSize());
  Hash.Final(reinterpret_cast<CryptoPP::byte*>(Digest.data()));

  std::string Hexed;
  CryptoPP::StringSource Pipe(Digest, true,
      new CryptoPP::Base64URLEncoder(
          new CryptoPP::StringSink(Hexed)));

  return Hexed;
}
