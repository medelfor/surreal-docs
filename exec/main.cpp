/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <udocs-processor/subprocessors/ReferenceCacheBuilder.h>
#include <fmt/chrono.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <Poco/Net/NetSSL.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/RejectCertificateHandler.h>
#include <ShlObj.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <fstream>
#include <iostream>
#include "udocs-processor/version.h"
#include "udocs-processor/UDocsProcessor.h"
#include "udocs-processor/IdGenerator.h"
#include "udocs-processor/FileDataLoader.h"
#include "udocs-processor/serializers/type/UTypeMarkdownSerializer.h"
#include "udocs-processor/StringHelper.h"
#include "udocs-processor/cli/commands/generate/Generator.h"
#include "udocs-processor/cli/surreal/SurrealLoader.h"
#include "udocs-processor/cli/thirdparty/WinReg.hpp"
#include "udocs-processor/cli/commands/generate/CppPreparer.h"
#include "udocs-processor/cli/ue/UnrealInteraction.h"
#include "udocs-processor/cli/commands/generate/BpPreparer.h"
#include "udocs-processor/cli/cli/GenerateCLI.h"
#include "udocs-processor/cli/views/FtxGenerateView.h"
#include "udocs-processor/cli/SurDocsCLI.h"
#include "udocs-processor/cli/views/NonInteractiveGenerateView.h"
#include "udocs-processor/cli/views/FtxInitView.h"
#include "udocs-processor/telemetry/grpcTelemetryService.h"
#include "udocs-processor/telemetry/grpc.h"
#include "udocs-processor/cli/version/grpc.h"
#include "udocs-processor/grpc/grpc.h"
#include "udocs-processor/cli/version/ProductService.h"
#include "udocs-processor/cli/version/grpcProductService.h"
#include "udocs-processor/cli/version/VersionChecker.h"
#include "udocs-processor/cli/views/FtxTokenCreateView.h"
#include "udocs-processor/cli/commands/oauth2/HydraOAuth2.h"
#include "udocs-processor/services/impl/TokenServiceImpl.h"
#include "udocs-processor/cli/commands/TokenListCommand.h"
#include "udocs-processor/cli/cli/TokenListCLI.h"
#include "udocs-processor/cli/views/FtxTokenListView.h"
#include "udocs-processor/cli/views/FtxAuthView.h"
#include "udocs-processor/cli/commands/security/WindowsVault.h"
#include "udocs-processor/services/impl/QuotaServiceImpl.h"
#include "udocs-processor/cli/views/FtxQuotaListView.h"
#include "udocs-processor/services/impl/OrganizationServiceImpl.h"
#include "udocs-processor/cli/views/FtxOrganizationDeleteView.h"
#include "udocs-processor/cli/views/FtxOrganizationCreateView.h"
#include "udocs-processor/cli/views/FtxOrganizationListView.h"
#include "udocs-processor/cli/views/FtxProjectListView.h"
#include "udocs-processor/services/impl/ProjectServiceImpl.h"
#include "udocs-processor/cli/views/FtxProjectAnnounceView.h"
#include "udocs-processor/cli/views/FtxProjectCollaboratorAddView.h"
#include "udocs-processor/cli/views/FtxProjectCollaboratorDeleteView.h"
#include "udocs-processor/cli/views/FtxProjectCollaboratorListView.h"
#include "udocs-processor/cli/views/FtxProjectCreateView.h"
#include "udocs-processor/cli/views/FtxProjectDeleteView.h"
#include "udocs-processor/cli/views/FtxProjectPublishView.h"
#include "udocs-processor/services/impl/DocumentServiceImpl.h"
#include "udocs-processor/cli/version/AdsManager.h"
#include "udocs-processor/cli/version/DefaultAds.h"
#include "SSLInitializer.h"
#include "udocs-processor/cli/views/FtxMigrateView.h"

static constexpr const int SUCCESS_EXIT_CODE = 0;
static constexpr const int FAILURE_EXIT_CODE = 1;
static constexpr const char DIR_SEPARATOR[] = "\\";
static constexpr const char SURDOCS_RES_ENV[] = "SURDOCS_RES";
static constexpr const char SURDOCS_INSTALL_ENV[] = "SURDOCS_INSTALL";
static constexpr const char LOGGER_NAME[] = "launcher";
static constexpr const char SURDOCS_APPDATA[] = "/Medelfor/SurrealDocs/";
static constexpr const char SURDOCS_LOGS_DEFAULT[] = "logs";
static constexpr const char SURDOCS_LOG_NAME_PATTERN[] =
    "SD-log-{:%Y.%m.%d_%H-%M-%S}.log";
static constexpr const char RES_DIRECTORY[] = "res";
static constexpr const char BIN_DIRECTORY[] = "bin";
static constexpr const wchar_t CACHE_DIR_KEY[] = L"Cache";
static constexpr const wchar_t CLIENT_ID_KEY[] = L"ClientId";
static constexpr const wchar_t DEFAULT_KEY[] = L"";
static constexpr const wchar_t SURDOCS_REGKEY[] =
    L"SOFTWARE\\Medelfor\\SurrealDocs";

static constexpr const char TELEMETRY_SOURCE[] = "surdocs";
static constexpr const char DEFAULT_CLIENT_ID[] = "johndoe";
static constexpr const char SURREAL_DOCS_PRODUCT_ID[] = "SURDOCS";
static constexpr const char SURREAL_JSON_PATH[] = "surdocs/surreal.json";

static constexpr const char SETTINGS_FILE[] = "settings.json";
static constexpr const char SETTINGS_SURREAL_CLOUD_API_PATH[] =
    "/surreal_cloud_api_address";
static constexpr const char SETTINGS_SURREAL_CLOUD_API_DEFAULT[] =
    "api.surrealdocs.com:443";
static constexpr const char SETTINGS_CALLBACK_SERVER_PORT_PATH[] =
    "/callback/port";
static constexpr uint64_t SETTINGS_CALLBACK_SERVER_PORT_DEFAULT = 19741;
static constexpr const char SETTINGS_OAUTH2_HOST_PATH[] = "/oauth2/host";
static constexpr const char SETTINGS_OAUTH2_HOST_DEFAULT[] =
    "hydra.medelfor.com";
static constexpr const char SETTINGS_OAUTH2_PORT_PATH[] = "/oauth2/port";
static constexpr uint64_t SETTINGS_OAUTH2_PORT_DEFAULT = 443;
static constexpr const char SETTINGS_OAUTH2_CLIENT_ID_PATH[] =
    "/oauth2/client_id";
static constexpr const char SETTINGS_OAUTH2_CLIENT_ID_DEFAULT[] = "";
static constexpr const char SETTINGS_API_PATH[] = "/api_address";
static constexpr const char SETTINGS_API_DEFAULT[] = "api.medelfor.com:443";
static constexpr const char SETTINGS_CA_CERT_PATH[] = "/ca_cert";
static constexpr const char SETTINGS_CA_CERT_DEFAULT[] =
    "certificates/ca-cert.crt";
static constexpr const char SETTINGS_DO_USE_HTTPS_PATH[] = "/do_use_https";
static constexpr const bool SETTINGS_DO_USE_HTTPS_DEFAULT = true;
static constexpr const char SURREAL_CONTACT_ME_AT_PATH[] = "/contact_me_at";


std::shared_ptr<spdlog::sinks::sink> SetupLogSink(const std::string& Path,
    std::string& LogAt) {
  auto Now = std::chrono::system_clock::now();
  std::string FileName = fmt::format(SURDOCS_LOG_NAME_PATTERN, Now);
  LogAt = fmt::format("{}\\{}", Path, FileName);
  std::filesystem::path FsPath{LogAt};
  std::filesystem::create_directories(FsPath.parent_path());
  std::ofstream CreateFileStream(LogAt);
  if (!CreateFileStream.is_open()) {
    throw std::runtime_error{"Unable to instatiate log sink"};
  }
  CreateFileStream.close();
  return std::make_shared<spdlog::sinks::basic_file_sink_mt>(LogAt);
}

nlohmann::json LoadSettings(const std::string& CacheDir,
    std::shared_ptr<spdlog::logger> l) {
  std::string Path = CacheDir + DIR_SEPARATOR + SETTINGS_FILE;
  std::ifstream Input{Path};
  if (!Input.good()) {
    l->error("Can't open settings at `{}`", Path);
    return {};
  }

  try {
    return nlohmann::json::parse(Input);
  } catch (const std::exception& Exc) {
    l->error("Can't parse settings at `{}`", Path);
    return {};
  }
}

std::string LoadRootCert(const std::string& Path,
    std::shared_ptr<spdlog::logger> l) {
  std::ifstream Input{Path};
  if (!Input.good()) {
    l->error("Can't open root cert at `{}`", Path);
    l->flush();
    throw std::runtime_error{"Unable to load root certificate"};
  }

  std::stringstream Stream;
  Stream << Input.rdbuf();
  return Stream.str();
}

std::optional<std::string> LoadContactAddress(
    std::shared_ptr<spdlog::logger> l) {
  try {
    std::ifstream Input{SURREAL_JSON_PATH};
    if (!Input.good()) {
      l->info("Can't open surreal.json, contact address is empty");
    }

    std::stringstream Stream;
    Stream << Input.rdbuf();

    nlohmann::json Data = nlohmann::json::parse(Stream.str());
    nlohmann::json_pointer<std::string> ContactMeAtPath{
        SURREAL_CONTACT_ME_AT_PATH};
    if (Data.contains(ContactMeAtPath)) {
      std::string ContactAddress = Data.at(ContactMeAtPath);
      l->info("Using `{}` as contact address", ContactAddress);
      return ContactAddress;
    }
  } catch (const std::exception& Exc) {
    l->warn("Exception when loading contact address: {}", Exc.what());
  }

  return {};
}

#define ENSURE_SETTING(Type, Variable, Path, Default)                   \
  Type Variable;                                                        \
  try {                                                                 \
    nlohmann::json_pointer<std::string> p{Path};                        \
    Variable = Settings.value(p, Default);                              \
    l->info("Using `{}` as {}", Variable, Path);                        \
  } catch (const std::exception& Exc) {                                 \
    l->info("Exception when loading setting {}: {}", Path, Exc.what()); \
    std::cout << Exc.what();                                            \
    std::cout.flush(); \
    return FAILURE_EXIT_CODE;                                           \
  }

#define ENSURE_STRING_SETTING(Variable, Path, Default)\
  ENSURE_SETTING(std::string, Variable, Path, Default)

#define ENSURE_BOOL_SETTING(Variable, Path, Default)\
  ENSURE_SETTING(bool, Variable, Path, Default)

#define ENSURE_UINT64_SETTING(Variable, Path, Default)\
  ENSURE_SETTING(uint64_t, Variable, Path, Default)

int main(int Argc, char** Argv) {
  winreg::RegKey SurDocsKey;
  using udocs_processor::StringHelper;

  std::string AppData;
  {
    std::wstring AppDataW;
    PWSTR AppDataStr = nullptr;
    HRESULT Result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL,
        &AppDataStr);
    if (Result != S_OK) {
      std::cout << "AppData cannot be found!";
      return 1;
    }
    AppDataW = AppDataStr;
    AppData = StringHelper::ws2s(AppDataW) + SURDOCS_APPDATA;
    CoTaskMemFree(AppDataStr);
  }

  // load cache dir from win registry
  std::string CachePath;
  winreg::RegResult Opened = SurDocsKey.TryOpen(
      HKEY_LOCAL_MACHINE, SURDOCS_REGKEY, KEY_READ | KEY_WOW64_64KEY);
  if (Opened) {
    winreg::RegExpected<std::wstring> Val =
        SurDocsKey.TryGetStringValue(CACHE_DIR_KEY);
    if (Val.IsValid()) {
      CachePath = StringHelper::Normalize((StringHelper::ws2s(Val.GetValue())));
    }
  }
  std::string LogsPath = AppData + SURDOCS_LOGS_DEFAULT;

  std::string LogAt;
  auto LoggerSink = SetupLogSink(LogsPath, LogAt);
  auto l = std::make_shared<spdlog::logger>(LOGGER_NAME, LoggerSink);
  spdlog::initialize_logger(l);

  nlohmann::json Settings = LoadSettings(CachePath, l);

  // load res dir from the win registry
  std::string InstallDir;
  const char* SurdocsRes = std::getenv(SURDOCS_RES_ENV);
  std::string ResourcesPath;
  if (SurdocsRes) {
    ResourcesPath = udocs_processor::StringHelper::Normalize(SurdocsRes);
    l->info("Using `{}` as res path (env)", ResourcesPath);
  } else if (Opened) {
    winreg::RegExpected<std::wstring> Val =
        SurDocsKey.TryGetStringValue(DEFAULT_KEY);
    if (Val.IsValid()) {
      InstallDir = StringHelper::ws2s(Val.GetValue());
      ResourcesPath = udocs_processor::StringHelper::Normalize(InstallDir) +
          RES_DIRECTORY + DIR_SEPARATOR;
      l->info("Using `{}` as res path (reg)", ResourcesPath);
    }
  }

  const char* SurdocsInstall = std::getenv(SURDOCS_INSTALL_ENV);
  if (SurdocsInstall) {
    InstallDir = SurdocsInstall;
    l->info("Using `{}` as install path (env)", SurdocsInstall);
  }

  std::string ClientId = DEFAULT_CLIENT_ID;
  if (Opened) {
    winreg::RegExpected<std::wstring> Val =
        SurDocsKey.TryGetStringValue(CLIENT_ID_KEY);
    if (Val.IsValid()) {
      ClientId = StringHelper::ws2s(Val.GetValue());
      l->info("Using `{}` as client id (reg)", ClientId);
    }
  }

  std::unique_ptr<udocs_processor::UnrealInteraction> UnrealInteraction =
      std::make_unique<udocs_processor::UnrealInteraction>(LoggerSink);

  std::string BinariesPath = InstallDir +
      std::string(InstallDir.empty() ? "" : DIR_SEPARATOR) + BIN_DIRECTORY;
  l->info("Using `{}` as bin path", BinariesPath);

  ENSURE_STRING_SETTING(SurrealCloudApiAddress, SETTINGS_SURREAL_CLOUD_API_PATH,
      SETTINGS_SURREAL_CLOUD_API_DEFAULT)
  ENSURE_STRING_SETTING(ApiAddress, SETTINGS_API_PATH, SETTINGS_API_DEFAULT)
  ENSURE_STRING_SETTING(CaCert, SETTINGS_CA_CERT_PATH, SETTINGS_CA_CERT_DEFAULT)
  ENSURE_STRING_SETTING(Oauth2ClientId, SETTINGS_OAUTH2_CLIENT_ID_PATH,
      SETTINGS_OAUTH2_CLIENT_ID_DEFAULT)
  ENSURE_STRING_SETTING(Oauth2Host, SETTINGS_OAUTH2_HOST_PATH,
      SETTINGS_OAUTH2_HOST_DEFAULT)
  ENSURE_UINT64_SETTING(CallbackServerPort, SETTINGS_CALLBACK_SERVER_PORT_PATH,
      SETTINGS_CALLBACK_SERVER_PORT_DEFAULT)
  ENSURE_UINT64_SETTING(Oauth2HostPort, SETTINGS_OAUTH2_PORT_PATH,
      SETTINGS_OAUTH2_PORT_DEFAULT)
  ENSURE_BOOL_SETTING(DoUseHttps, SETTINGS_DO_USE_HTTPS_PATH,
      SETTINGS_DO_USE_HTTPS_DEFAULT)

  CaCert = InstallDir + std::string(InstallDir.empty() ? "" : DIR_SEPARATOR) +
      CaCert;

  // instantiate telemetry services
  std::optional<std::string> RootCert;
  if (DoUseHttps) {
    RootCert = LoadRootCert(CaCert, l);
  }
  auto Channel = udocs_processor::CreateChannel(ApiAddress, RootCert);
  auto CloudChannel = udocs_processor::CreateSurrealCloudChannel(
      SurrealCloudApiAddress, RootCert);

  udocs_processor::SSLInitializer SSLInitializer{l};
  if (!SSLInitializer.Initialize(CaCert)) return FAILURE_EXIT_CODE;

  std::unique_ptr<udocs_processor::TelemetryService> TelemetryService =
      std::make_unique<udocs_processor::grpcTelemetryService>(Channel);
  std::shared_ptr<udocs_processor::TelemetryReporter> TelemetryReporter =
      std::make_shared<udocs_processor::TelemetryReporter>(LoggerSink,
          std::move(TelemetryService), ClientId, TELEMETRY_SOURCE);
  std::shared_ptr<udocs_processor::BasicTelemetry> BasicTelemetry =
      std::make_shared<udocs_processor::BasicTelemetry>(TelemetryReporter);

  // instantiate version checker
  auto VersionChannel = udocs_processor::CreateVersionChannel(ApiAddress,
      std::move(RootCert));
  std::shared_ptr<udocs_processor::ProductService> ProductService =
      std::make_shared<udocs_processor::grpcProductService>(VersionChannel);
  std::shared_ptr<udocs_processor::VersionChecker> VersionChecker =
      std::make_shared<udocs_processor::VersionChecker>(LoggerSink,
          SURREAL_DOCS_PRODUCT_ID, VERSION_MAJOR, VERSION_MINOR,
          VERSION_INDEX, VERSION_BUILD);
  VersionChecker->SetProductService(ProductService);
  VersionChecker->SetCachePath(AppData);

  VersionChecker->UpdateVersion();
  std::optional<std::string> NewVersion = VersionChecker->IsOutdated();

  // ads loader
  std::shared_ptr<udocs_processor::DefaultAds> DefaultAds =
      std::make_shared<udocs_processor::DefaultAds>();
  std::shared_ptr<udocs_processor::AdsManager> AdsManager =
      std::make_shared<udocs_processor::AdsManager>(LoggerSink,
          SURREAL_DOCS_PRODUCT_ID);
  AdsManager->SetProductService(ProductService);
  AdsManager->SetCachePath(AppData);
  AdsManager->SetDefaultAds(DefaultAds);

  AdsManager->UpdateAds();

  // instantiate `generate`
  std::unique_ptr<udocs_processor::UDocsDocumentRenderer> Renderer =
      std::make_unique<udocs_processor::UDocsDocumentRenderer>(LoggerSink);
  Renderer->SetResourcesPath(ResourcesPath);
  std::shared_ptr<udocs_processor::ProjectService> ProjectService =
      std::make_shared<udocs_processor::ProjectServiceImpl>(LoggerSink,
          CloudChannel);
  std::unique_ptr<udocs_processor::DocumentService> DocumentService =
      std::make_unique<udocs_processor::DocumentServiceImpl>(LoggerSink,
          CloudChannel);
  std::unique_ptr<udocs_processor::Generator> Generator =
      std::make_unique<udocs_processor::Generator>(std::move(DocumentService),
          ProjectService, LoggerSink);
  Generator->SetResourcesPath(ResourcesPath);
  Generator->SetBinaryPath(BinariesPath);
  Generator->SetInstallPath(InstallDir);
  Generator->SetRenderer(std::move(Renderer));
  std::unique_ptr<udocs_processor::BpPreparer> BpPreparer =
      std::make_unique<udocs_processor::BpPreparer>(LoggerSink,
          std::move(UnrealInteraction));
  BpPreparer->SetResDirectory(ResourcesPath);
  std::unique_ptr<udocs_processor::CppPreparer> CppPreparer =
      std::make_unique<udocs_processor::CppPreparer>(LoggerSink);
  CppPreparer->SetResDirectory(ResourcesPath);
  CppPreparer->SetBinDirectory(BinariesPath);

  std::unique_ptr<udocs_processor::GenerateCommand> GenerateCommand =
      std::make_unique<udocs_processor::GenerateCommand>(LoggerSink,
          std::move(BpPreparer), std::move(CppPreparer), ProjectService,
          std::move(Generator));

  std::shared_ptr<udocs_processor::LogReporter> LogReporter =
      std::make_shared<udocs_processor::LogReporter>(LogAt, LoggerSink,
          BasicTelemetry);
  LogReporter->SetContactMeAt(LoadContactAddress(l));

  std::unique_ptr<udocs_processor::GenerateView> InteractiveGenerateView =
      std::make_unique<udocs_processor::FtxGenerateView>();
  InteractiveGenerateView->SetLogFilePath(LogAt);
  InteractiveGenerateView->SetLogReporter({}, LogReporter);
  InteractiveGenerateView->SetAds(AdsManager->GetAds());
  std::unique_ptr<udocs_processor::GenerateView> NonInteractiveGenerateView =
      std::make_unique<udocs_processor::NonInteractiveGenerateView>(std::cout);
  NonInteractiveGenerateView->SetLogFilePath(LogAt);
  if (NewVersion) {
    InteractiveGenerateView->SetNewVersion(*NewVersion);
    NonInteractiveGenerateView->SetNewVersion(*NewVersion);
  }
  std::shared_ptr<udocs_processor::Vault> Vault =
      std::make_shared<udocs_processor::WindowsVault>();
  std::shared_ptr<udocs_processor::StandarInputReader> InputReader =
      std::make_shared<udocs_processor::StandarInputReader>();
  std::shared_ptr<udocs_processor::TokenLoader> TokenLoader =
      std::make_shared<udocs_processor::TokenLoader>(Vault, InputReader);
  std::shared_ptr<udocs_processor::SurrealLoader> Loader =
      std::make_shared<udocs_processor::SurrealLoader>();
  std::unique_ptr<udocs_processor::GenerateCLI> GenerateCLI =
      std::make_unique<udocs_processor::GenerateCLI>(
          LoggerSink, std::move(GenerateCommand),
          std::move(InteractiveGenerateView),
          std::move(NonInteractiveGenerateView), Loader, TokenLoader,
          BasicTelemetry);

  // instantiate `init`
  std::unique_ptr<udocs_processor::UnrealInteraction> UnrealInteraction2 =
      std::make_unique<udocs_processor::UnrealInteraction>(LoggerSink);
  std::unique_ptr<udocs_processor::UnrealInteraction> UnrealInteraction3 =
      std::make_unique<udocs_processor::UnrealInteraction>(LoggerSink);
  std::unique_ptr<udocs_processor::InitCommand> InitCommand =
      std::make_unique<udocs_processor::InitCommand>(
          std::move(UnrealInteraction3), Loader);
  InitCommand->SetInstallPath(InstallDir);
  InitCommand->SetResourcesPath(ResourcesPath);
  const udocs_processor::InitCommand& InitReference = *InitCommand;
  std::unique_ptr<udocs_processor::InitCLI> InitCLI =
      std::make_unique<udocs_processor::InitCLI>(
          LoggerSink, std::move(InitCommand), std::move(UnrealInteraction2),
          BasicTelemetry);

  std::unique_ptr<udocs_processor::InitView> InteractiveInitView =
      std::make_unique<udocs_processor::FtxInitView>(*InitCLI);
  InteractiveInitView->SetLogReporter({}, LogReporter);
  InteractiveInitView->SetLogFilePath(LogAt);
  if (NewVersion) {
    InteractiveInitView->SetNewVersion(*NewVersion);
  }
  InitCLI->SetView(std::move(InteractiveInitView));

  // instantiate `token create`
  std::shared_ptr<udocs_processor::OAuth2> Oauth2 =
      std::make_shared<udocs_processor::HydraOAuth2>(LoggerSink,
          CallbackServerPort, Oauth2ClientId, Oauth2Host, Oauth2HostPort,
          DoUseHttps, ResourcesPath);
  std::shared_ptr<udocs_processor::TokenService> TokenService =
      std::make_shared<udocs_processor::TokenServiceImpl>(LoggerSink,
          CloudChannel);
  std::unique_ptr<udocs_processor::TokenCreateCommand> TokenCreateCommand =
      std::make_unique<udocs_processor::TokenCreateCommand>(
          LoggerSink, Oauth2, TokenService);
  std::unique_ptr<udocs_processor::TokenCreateCLI> TokenCreateCLI =
      std::make_unique<udocs_processor::TokenCreateCLI>(
          LoggerSink, std::move(TokenCreateCommand), BasicTelemetry);

  std::unique_ptr<udocs_processor::TokenCreateView> TokenCreateView =
      std::make_unique<udocs_processor::FtxTokenCreateView>();
  TokenCreateView->SetLogReporter({}, LogReporter);
  TokenCreateView->SetLogFilePath(LogAt);
  TokenCreateCLI->SetView(std::move(TokenCreateView));

  // instantiate `token list`
  std::unique_ptr<udocs_processor::TokenListCommand> TokenListCommand =
      std::make_unique<udocs_processor::TokenListCommand>(
          LoggerSink, Oauth2, TokenService);
  std::unique_ptr<udocs_processor::TokenListCLI> TokenListCLI =
      std::make_unique<udocs_processor::TokenListCLI>(
          LoggerSink, std::move(TokenListCommand), BasicTelemetry);

  std::unique_ptr<udocs_processor::TokenListView> TokenListView =
      std::make_unique<udocs_processor::FtxTokenListView>();
  TokenListView->SetLogReporter({}, LogReporter);
  TokenListView->SetLogFilePath(LogAt);
  TokenListCLI->SetView(std::move(TokenListView));

  // instantiate `auth`
  std::unique_ptr<udocs_processor::AuthCommand> AuthCommand =
      std::make_unique<udocs_processor::AuthCommand>(
          LoggerSink, Vault);
  std::unique_ptr<udocs_processor::AuthCLI> AuthCLI =
      std::make_unique<udocs_processor::AuthCLI>(
          LoggerSink, std::move(AuthCommand), TokenLoader, BasicTelemetry);

  std::unique_ptr<udocs_processor::AuthView> AuthView =
      std::make_unique<udocs_processor::FtxAuthView>();
  AuthView->SetLogReporter({}, LogReporter);
  AuthView->SetLogFilePath(LogAt);
  AuthCLI->SetView(std::move(AuthView));

  // instantiate `quota list`
  std::shared_ptr<udocs_processor::QuotaService> QuotaService =
      std::make_shared<udocs_processor::QuotaServiceImpl>(LoggerSink,
          CloudChannel);
  std::unique_ptr<udocs_processor::QuotaListCommand> QuotaListCommand =
      std::make_unique<udocs_processor::QuotaListCommand>(
          QuotaService, LoggerSink);
  std::unique_ptr<udocs_processor::QuotaListCLI> QuotaListCLI =
      std::make_unique<udocs_processor::QuotaListCLI>(
          LoggerSink, std::move(QuotaListCommand), TokenLoader, BasicTelemetry);

  std::unique_ptr<udocs_processor::QuotaListView> QuotaListView =
      std::make_unique<udocs_processor::FtxQuotaListView>();
  QuotaListView->SetLogReporter({}, LogReporter);
  QuotaListView->SetLogFilePath(LogAt);
  QuotaListCLI->SetView(std::move(QuotaListView));

  // instantiate `organization delete`
  std::shared_ptr<udocs_processor::OrganizationService> OrganizationService =
      std::make_shared<udocs_processor::OrganizationServiceImpl>(LoggerSink,
          CloudChannel);
  std::unique_ptr<udocs_processor::OrganizationDeleteCommand>
      OrganizationDeleteCommand =
      std::make_unique<udocs_processor::OrganizationDeleteCommand>(
          OrganizationService, LoggerSink);
  std::unique_ptr<udocs_processor::OrganizationDeleteCLI> OrganizationDeleteCLI
      = std::make_unique<udocs_processor::OrganizationDeleteCLI>(LoggerSink,
          std::move(OrganizationDeleteCommand), TokenLoader, BasicTelemetry);

  std::unique_ptr<udocs_processor::OrganizationDeleteView>
      OrganizationDeleteView =
      std::make_unique<udocs_processor::FtxOrganizationDeleteView>();
  OrganizationDeleteView->SetLogReporter({}, LogReporter);
  OrganizationDeleteView->SetLogFilePath(LogAt);
  OrganizationDeleteCLI->SetView(std::move(OrganizationDeleteView));

  // instantiate `organization create`
  std::unique_ptr<udocs_processor::OrganizationCreateCommand>
      OrganizationCreateCommand =
      std::make_unique<udocs_processor::OrganizationCreateCommand>(
          OrganizationService, LoggerSink);
  std::unique_ptr<udocs_processor::OrganizationCreateCLI> OrganizationCreateCLI
      = std::make_unique<udocs_processor::OrganizationCreateCLI>(LoggerSink,
          std::move(OrganizationCreateCommand), TokenLoader, BasicTelemetry);

  std::unique_ptr<udocs_processor::OrganizationCreateView>
      OrganizationCreateView =
      std::make_unique<udocs_processor::FtxOrganizationCreateView>();
  OrganizationCreateView->SetLogReporter({}, LogReporter);
  OrganizationCreateView->SetLogFilePath(LogAt);
  OrganizationCreateCLI->SetView(std::move(OrganizationCreateView));

  // instantiate `organization list`
  std::unique_ptr<udocs_processor::OrganizationListCommand>
      OrganizationListCommand =
      std::make_unique<udocs_processor::OrganizationListCommand>(
          OrganizationService, LoggerSink);
  std::unique_ptr<udocs_processor::OrganizationListCLI> OrganizationListCLI
      = std::make_unique<udocs_processor::OrganizationListCLI>(LoggerSink,
          std::move(OrganizationListCommand), TokenLoader, BasicTelemetry);

  std::unique_ptr<udocs_processor::OrganizationListView>
      OrganizationListView =
      std::make_unique<udocs_processor::FtxOrganizationListView>();
  OrganizationListView->SetLogReporter({}, LogReporter);
  OrganizationListView->SetLogFilePath(LogAt);
  OrganizationListCLI->SetView(std::move(OrganizationListView));

  // instantiate `project list`
  std::unique_ptr<udocs_processor::ProjectListCommand> ProjectListCommand =
      std::make_unique<udocs_processor::ProjectListCommand>(
          ProjectService, LoggerSink);
  std::unique_ptr<udocs_processor::ProjectListCLI> ProjectListCLI
      = std::make_unique<udocs_processor::ProjectListCLI>(LoggerSink,
          std::move(ProjectListCommand), TokenLoader, BasicTelemetry);

  std::unique_ptr<udocs_processor::ProjectListView> ProjectListView =
      std::make_unique<udocs_processor::FtxProjectListView>();
  ProjectListView->SetLogReporter({}, LogReporter);
  ProjectListView->SetLogFilePath(LogAt);
  ProjectListCLI->SetView(std::move(ProjectListView));

  // instantiate `project announce`
  std::unique_ptr<udocs_processor::ProjectAnnounceCommand>
      ProjectAnnounceCommand =
      std::make_unique<udocs_processor::ProjectAnnounceCommand>(
          ProjectService, LoggerSink);
  std::unique_ptr<udocs_processor::ProjectAnnounceCLI> ProjectAnnounceCLI
      = std::make_unique<udocs_processor::ProjectAnnounceCLI>(LoggerSink,
          std::move(ProjectAnnounceCommand), TokenLoader, BasicTelemetry);

  std::unique_ptr<udocs_processor::ProjectAnnounceView> ProjectAnnounceView =
      std::make_unique<udocs_processor::FtxProjectAnnounceView>();
  ProjectAnnounceView->SetLogReporter({}, LogReporter);
  ProjectAnnounceView->SetLogFilePath(LogAt);
  ProjectAnnounceCLI->SetView(std::move(ProjectAnnounceView));

  // instantiate `project collaborator add`
  std::unique_ptr<udocs_processor::ProjectCollaboratorAddCommand>
      ProjectCollaboratorAddCommand =
      std::make_unique<udocs_processor::ProjectCollaboratorAddCommand>(
          ProjectService, LoggerSink);
  std::unique_ptr<udocs_processor::ProjectCollaboratorAddCLI>
      ProjectCollaboratorAddCLI
      = std::make_unique<udocs_processor::ProjectCollaboratorAddCLI>(LoggerSink,
          std::move(ProjectCollaboratorAddCommand), TokenLoader,
          BasicTelemetry);

  std::unique_ptr<udocs_processor::ProjectCollaboratorAddView>
      ProjectCollaboratorAddView =
      std::make_unique<udocs_processor::FtxProjectCollaboratorAddView>();
  ProjectCollaboratorAddView->SetLogReporter({}, LogReporter);
  ProjectCollaboratorAddView->SetLogFilePath(LogAt);
  ProjectCollaboratorAddCLI->SetView(std::move(ProjectCollaboratorAddView));

  // instantiate `project collaborator delete`
  std::unique_ptr<udocs_processor::ProjectCollaboratorDeleteCommand>
      ProjectCollaboratorDeleteCommand =
      std::make_unique<udocs_processor::ProjectCollaboratorDeleteCommand>(
          ProjectService, LoggerSink);
  std::unique_ptr<udocs_processor::ProjectCollaboratorDeleteCLI>
      ProjectCollaboratorDeleteCLI
      = std::make_unique<udocs_processor::ProjectCollaboratorDeleteCLI>(
          LoggerSink, std::move(ProjectCollaboratorDeleteCommand), TokenLoader,
          BasicTelemetry);

  std::unique_ptr<udocs_processor::ProjectCollaboratorDeleteView>
      ProjectCollaboratorDeleteView =
      std::make_unique<udocs_processor::FtxProjectCollaboratorDeleteView>();
  ProjectCollaboratorDeleteView->SetLogReporter({}, LogReporter);
  ProjectCollaboratorDeleteView->SetLogFilePath(LogAt);
  ProjectCollaboratorDeleteCLI->SetView(std::move(
      ProjectCollaboratorDeleteView));

  // instantiate `project collaborator list`
  std::unique_ptr<udocs_processor::ProjectCollaboratorListCommand>
      ProjectCollaboratorListCommand =
      std::make_unique<udocs_processor::ProjectCollaboratorListCommand>(
          ProjectService, LoggerSink);
  std::unique_ptr<udocs_processor::ProjectCollaboratorListCLI>
      ProjectCollaboratorListCLI
      = std::make_unique<udocs_processor::ProjectCollaboratorListCLI>(
          LoggerSink, std::move(ProjectCollaboratorListCommand), TokenLoader,
          BasicTelemetry);

  std::unique_ptr<udocs_processor::ProjectCollaboratorListView>
      ProjectCollaboratorListView =
      std::make_unique<udocs_processor::FtxProjectCollaboratorListView>();
  ProjectCollaboratorListView->SetLogReporter({}, LogReporter);
  ProjectCollaboratorListView->SetLogFilePath(LogAt);
  ProjectCollaboratorListCLI->SetView(std::move(
      ProjectCollaboratorListView));

  // instantiate `project create`
  std::unique_ptr<udocs_processor::ProjectCreateCommand>
      ProjectCreateCommand =
      std::make_unique<udocs_processor::ProjectCreateCommand>(
          ProjectService, LoggerSink);
  std::unique_ptr<udocs_processor::ProjectCreateCLI> ProjectCreateCLI
      = std::make_unique<udocs_processor::ProjectCreateCLI>(LoggerSink,
          std::move(ProjectCreateCommand), TokenLoader, BasicTelemetry);

  std::unique_ptr<udocs_processor::ProjectCreateView> ProjectCreateView =
      std::make_unique<udocs_processor::FtxProjectCreateView>();
  ProjectCreateView->SetLogReporter({}, LogReporter);
  ProjectCreateView->SetLogFilePath(LogAt);
  ProjectCreateCLI->SetView(std::move(ProjectCreateView));

  // instantiate `project delete`
  std::unique_ptr<udocs_processor::ProjectDeleteCommand>
      ProjectDeleteCommand =
      std::make_unique<udocs_processor::ProjectDeleteCommand>(
          ProjectService, LoggerSink);
  std::unique_ptr<udocs_processor::ProjectDeleteCLI> ProjectDeleteCLI
      = std::make_unique<udocs_processor::ProjectDeleteCLI>(LoggerSink,
          std::move(ProjectDeleteCommand), TokenLoader, BasicTelemetry);

  std::unique_ptr<udocs_processor::ProjectDeleteView> ProjectDeleteView =
      std::make_unique<udocs_processor::FtxProjectDeleteView>();
  ProjectDeleteView->SetLogReporter({}, LogReporter);
  ProjectDeleteView->SetLogFilePath(LogAt);
  ProjectDeleteCLI->SetView(std::move(ProjectDeleteView));

  // instantiate `project publish`
  std::unique_ptr<udocs_processor::ProjectPublishCommand>
      ProjectPublishCommand =
      std::make_unique<udocs_processor::ProjectPublishCommand>(
          ProjectService, LoggerSink);
  std::unique_ptr<udocs_processor::ProjectPublishCLI> ProjectPublishCLI
      = std::make_unique<udocs_processor::ProjectPublishCLI>(LoggerSink,
          std::move(ProjectPublishCommand), TokenLoader, BasicTelemetry);

  std::unique_ptr<udocs_processor::ProjectPublishView> ProjectPublishView =
      std::make_unique<udocs_processor::FtxProjectPublishView>();
  ProjectPublishView->SetLogReporter({}, LogReporter);
  ProjectPublishView->SetLogFilePath(LogAt);
  ProjectPublishCLI->SetView(std::move(ProjectPublishView));

  // instantiate `migrate`
  std::unique_ptr<udocs_processor::MigrateCommand> MigrateCommand =
      std::make_unique<udocs_processor::MigrateCommand>(InitReference,
          LoggerSink);
  std::unique_ptr<udocs_processor::MigrateCLI> MigrateCLI
      = std::make_unique<udocs_processor::MigrateCLI>(LoggerSink,
          std::move(MigrateCommand), BasicTelemetry);

  std::unique_ptr<udocs_processor::MigrateView> MigrateView =
      std::make_unique<udocs_processor::FtxMigrateView>();
  MigrateView->SetLogReporter({}, LogReporter);
  MigrateView->SetLogFilePath(LogAt);
  MigrateCLI->SetView(std::move(MigrateView));

  TelemetryReporter->StartReporting();
  udocs_processor::SurDocsCLI Cli{LoggerSink,
      InputReader,
      std::move(GenerateCLI),
      std::move(InitCLI),
      std::move(TokenCreateCLI),
      std::move(TokenListCLI),
      std::move(AuthCLI),
      std::move(QuotaListCLI),
      std::move(OrganizationListCLI),
      std::move(OrganizationCreateCLI),
      std::move(OrganizationDeleteCLI),
      std::move(ProjectAnnounceCLI),
      std::move(ProjectCollaboratorAddCLI),
      std::move(ProjectCollaboratorDeleteCLI),
      std::move(ProjectCollaboratorListCLI),
      std::move(ProjectCreateCLI),
      std::move(ProjectDeleteCLI),
      std::move(ProjectListCLI),
      std::move(ProjectPublishCLI),
      std::move(MigrateCLI),
      TelemetryReporter};
  bool Result = Cli.Start(Argc, Argv);

  TelemetryReporter->StopReporting();
  TelemetryReporter->Join();
  LoggerSink->flush();

  SSLInitializer.Deinitialize();

  return Result ? SUCCESS_EXIT_CODE : FAILURE_EXIT_CODE;
}
