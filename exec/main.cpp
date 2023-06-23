/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <udocs-processor/subprocessors/ReferenceCacheBuilder.h>
#include <udocs-processor/subprocessors/ExportPolicy.h>
#include <udocs-processor/subprocessors/CrossReferencer.h>
#include <udocs-processor/subprocessors/ComplementFinder.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <fmt/chrono.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <fstream>
#include <sstream>
#include <csignal>
#include "udocs-processor/version.h"
#include "udocs-processor/UDocsProcessor.h"
#include "udocs-processor/IdGenerator.h"
#include "udocs-processor/treeloaders/UDataTreeLoader.h"
#include "udocs-processor/treeloaders/DoxybookTreeLoader.h"
#include "udocs-processor/FileDataLoader.h"
#include "udocs-processor/treeloaders/PagesTreeLoader.h"
#include "udocs-processor/serializers/UFunctionHTMLSerializer.h"
#include "udocs-processor/DocPathsCache.h"
#include "udocs-processor/subprocessors/DocPathBuilder.h"
#include "udocs-processor/serializers/type/UTypeHTMLSerializer.h"
#include "udocs-processor/serializers/type/UTypeMarkdownSerializer.h"
#include "udocs-processor/serializers/HTMLDocTreeSerializer.h"
#include "udocs-processor/serializers/JSONDocTreeSerializer.h"
#include "udocs-processor/serializers/DynamicPageData.h"
#include "udocs-processor/serializers/markdown/SurrealMarkdownToHtmlConverter.h"
#include "udocs-processor/NodeHasher.h"
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
#include "udocs-processor/cli/version/ProductService.h"
#include "udocs-processor/cli/version/grpcProductService.h"
#include "udocs-processor/cli/version/VersionChecker.h"

static constexpr const int SUCCESS_EXIT_CODE = 0;
static constexpr const int FAILURE_EXIT_CODE = 1;
static constexpr const char DIR_SEPARATOR[] = "\\";
static constexpr const char SURDOCS_RES_ENV[] = "SURDOCS_RES";
static constexpr const char SURDOCS_INSTALL_ENV[] = "SURDOCS_INSTALL";
static constexpr const char LOGGER_NAME[] = "launcher";
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

int main(int Argc, char** Argv) {
  winreg::RegKey SurDocsKey;
  using udocs_processor::StringHelper;

  // load cache dir from win registry
  std::string LogsPath;
  std::string CachePath;
  winreg::RegResult Opened = SurDocsKey.TryOpen(
      HKEY_CURRENT_USER, SURDOCS_REGKEY);
  if (Opened) {
    winreg::RegExpected<std::wstring> Val =
        SurDocsKey.TryGetStringValue(CACHE_DIR_KEY);
    if (Val.IsValid()) {
      CachePath = StringHelper::Normalize((StringHelper::ws2s(Val.GetValue())));
      LogsPath = CachePath;
    }
  }
  LogsPath += SURDOCS_LOGS_DEFAULT;

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

  nlohmann::json_pointer<std::string> ApiAddressPath{SETTINGS_API_PATH};
  nlohmann::json_pointer<std::string> CaCertPath{SETTINGS_CA_CERT_PATH};
  nlohmann::json_pointer<std::string> DoUseHttpsPath{
    SETTINGS_DO_USE_HTTPS_PATH};

  std::string ApiAddress = Settings.value(ApiAddressPath, SETTINGS_API_DEFAULT);
  l->info("Using `{}` as the api address", ApiAddress);
  std::string CaCert = InstallDir +
      std::string(InstallDir.empty() ? "" : DIR_SEPARATOR) +
      Settings.value(CaCertPath, SETTINGS_CA_CERT_DEFAULT);
  l->info("Using `{}` as the path to root certificates", CaCert);
  bool DoUseHttps = Settings.value(DoUseHttpsPath,
      SETTINGS_DO_USE_HTTPS_DEFAULT);
  l->info("Using https: `{}`", DoUseHttps);

  // instantiate telemetry services
  std::optional<std::string> RootCert;
  if (DoUseHttps) {
    RootCert = LoadRootCert(CaCert, l);
  }
  auto Channel = udocs_processor::CreateChannel(ApiAddress,
      RootCert);

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
  VersionChecker->SetCachePath(CachePath);

  VersionChecker->UpdateVersion();
  std::optional<std::string> NewVersion = VersionChecker->IsOutdated();

  // instantiate generator
  std::unique_ptr<udocs_processor::UDocsDocumentRenderer> Renderer =
      std::make_unique<udocs_processor::UDocsDocumentRenderer>(LoggerSink);
  Renderer->SetResourcesPath(ResourcesPath);
  std::unique_ptr<udocs_processor::Generator> Generator =
      std::make_unique<udocs_processor::Generator>(LoggerSink);
  Generator->SetResourcesPath(ResourcesPath);
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
          std::move(BpPreparer), std::move(CppPreparer), std::move(Generator));

  std::shared_ptr<udocs_processor::LogReporter> LogReporter =
      std::make_shared<udocs_processor::LogReporter>(LogAt, LoggerSink,
          BasicTelemetry);
  LogReporter->SetContactMeAt(LoadContactAddress(l));

  std::unique_ptr<udocs_processor::GenerateView> InteractiveGenerateView =
      std::make_unique<udocs_processor::FtxGenerateView>();
  InteractiveGenerateView->SetLogFilePath(LogAt);
  InteractiveGenerateView->SetLogReporter({}, LogReporter);
  std::unique_ptr<udocs_processor::GenerateView> NonInteractiveGenerateView =
      std::make_unique<udocs_processor::NonInteractiveGenerateView>(std::cout);
  NonInteractiveGenerateView->SetLogFilePath(LogAt);
  if (NewVersion) {
    InteractiveGenerateView->SetNewVersion(*NewVersion);
    NonInteractiveGenerateView->SetNewVersion(*NewVersion);
  }

  std::shared_ptr<udocs_processor::SurrealLoader> Loader =
      std::make_shared<udocs_processor::SurrealLoader>();
  std::unique_ptr<udocs_processor::GenerateCLI> GenerateCLI =
      std::make_unique<udocs_processor::GenerateCLI>(
          LoggerSink, std::move(GenerateCommand),
          std::move(InteractiveGenerateView),
          std::move(NonInteractiveGenerateView), Loader, BasicTelemetry);

  std::unique_ptr<udocs_processor::UnrealInteraction> UnrealInteraction2 =
      std::make_unique<udocs_processor::UnrealInteraction>(LoggerSink);
  std::unique_ptr<udocs_processor::UnrealInteraction> UnrealInteraction3 =
      std::make_unique<udocs_processor::UnrealInteraction>(LoggerSink);
  std::unique_ptr<udocs_processor::InitCommand> InitCommand =
      std::make_unique<udocs_processor::InitCommand>(
          std::move(UnrealInteraction3), Loader);
  InitCommand->SetInstallPath(InstallDir);
  InitCommand->SetResourcesPath(ResourcesPath);
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

  TelemetryReporter->StartReporting();
  udocs_processor::SurDocsCLI Cli{LoggerSink,
      std::move(GenerateCLI), std::move(InitCLI), TelemetryReporter};
  bool Result = Cli.Start(Argc, Argv);

  TelemetryReporter->StopReporting();
  TelemetryReporter->Join();
  LoggerSink->flush();

  return Result ? SUCCESS_EXIT_CODE : FAILURE_EXIT_CODE;
}
