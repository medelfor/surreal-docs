/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/SurDocsCLI.h"
#include "udocs-processor/cli/cli/InitCLI.h"
#include "udocs-processor/cli/cli/AuthCLI.h"
#include "udocs-processor/StringHelper.h"
#include "udocs-processor/version.h"

#undef max
#undef min
#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>

bool udocs_processor::SurDocsCLI::Start(int Argc, char** Argv) {
  CLI::App App;
  App.description(CLI_DESCRIPTION);
  App.name(APP_NAME);

  // --no-tele
  CLI::Option* NoTele = App.add_flag_callback(GLOBAL_FLAG_NO_TELE, [this](){
    Telemetry->SetTelemetryEnabled(false);
  }, GLOBAL_FLAG_NO_TELE_DESCRIPTION);

  // --version
  CLI::Option* Version = nullptr;
  {
    std::string Tag = strlen(VERSION_TAG) > 0
        ? fmt::format("-{}", VERSION_TAG) : "";
    std::string Build = fmt::format("+{}.{}", VERSION_BUILD,
        VERSION_BUILD_ID);
    std::string VersionString = fmt::format("{}.{}.{}{}{}", VERSION_MAJOR,
        VERSION_MINOR, VERSION_INDEX, Tag, Build);
    Version = App.set_version_flag(FLAG_VERSION, VersionString,
        FLAG_VERSION_DESCRIPTION);
  }

  // --token-vault, --token-stdin, --token-env
  bool TokenVault = false;
  bool TokenStdin = false;
  bool TokenEnv = false;
  CLI::App* TokenSourceGroup = App.add_option_group(TOKEN_SOURCE_GROUP);
  TokenSourceGroup->add_flag(GLOBAL_FLAG_TOKEN_VAULT, TokenVault,
      GLOBAL_FLAG_TOKEN_VAULT_DESCRIPTION);
  TokenSourceGroup->add_flag(GLOBAL_FLAG_TOKEN_ENV, TokenEnv,
      GLOBAL_FLAG_TOKEN_ENV_DESCRIPTION);
  TokenSourceGroup->add_flag(GLOBAL_FLAG_TOKEN_STDIN, TokenStdin,
      GLOBAL_FLAG_TOKEN_STDIN_DESCRIPTION);
  TokenSourceGroup->excludes(Version);

  // generate
  CLI::App* GenerateCommand = App.add_subcommand(
      GENERATE_COMMAND, GENERATE_DESCRIPTION);
  bool IsNonInteractive = false;
  bool DoCleanOut = false;
  std::string DirectoryOut;
  std::string GenerateLocation;
  GenerateCommand->add_flag<bool>(GENERATE_FLAG_NON_INTERACTIVE,
      IsNonInteractive, GENERATE_FLAG_NON_INTERACTIVE_DESCRIPTION);
  GenerateCommand->add_flag<bool>(GENERATE_FLAG_CLEAN,
      DoCleanOut, GENERATE_FLAG_CLEAN_DESCRIPTION);
  GenerateCommand->add_option<std::string>(GENERATE_FLAG_OUT_DIRECTORY,
      DirectoryOut, GENERATE_FLAG_OUT_DIRECTORY_DESCRIPTION)->required();
  GenerateCommand->add_option<std::string>(GENERATE_OPTION_DO_DEPLOY,
      GenerateLocation, GENERATE_OPTION_DO_DEPLOY_DESCRIPTION);
  GenerateCommand->callback(
    [&IsNonInteractive, &DoCleanOut, &DirectoryOut, &GenerateLocation, this,
     &TokenStdin, &TokenVault, &TokenEnv](){
      GenerateCLI::Arguments Args;
      Args.DoCleanOut = DoCleanOut;
      Args.IsInteractive = !IsNonInteractive;
      if (!GenerateLocation.empty()) {
        Args.DeployLocation = GenerateLocation;
      }
      Args.OutDirectory = std::move(DirectoryOut);
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      bool Result = Generate->Generate(Args);
      if (!Result) {
        throw std::runtime_error{"Generate returned `false`, propagating"};
      }
    });
  GenerateCommand->excludes(Version);

  // init
  CLI::App* InitCommand = App.add_subcommand(INIT_COMMAND, INIT_DESCRIPTION);
  InitCommand->callback(
    [this]() {
      InitCLI::Arguments Args;
      bool Result = Init->Init(Args);
      if (!Result) {
        throw std::runtime_error{"Init returned `false`, propagating"};
      }
    });
  InitCommand->excludes(Version);
  InitCommand->excludes(TokenSourceGroup);

  // token
  CLI::App* TokenCommand = App.add_subcommand(TOKEN_COMMAND, TOKEN_DESCRIPTION);
  TokenCommand->require_subcommand();
  TokenCommand->excludes(Version);
  TokenCommand->excludes(TokenSourceGroup);

  // token create
  CLI::App* TokenCreateCommand = TokenCommand->add_subcommand(
      TOKEN_CREATE_COMMAND, TOKEN_CREATE_DESCRIPTION);
  TokenCreateCommand->callback(
    [this]() {
      TokenCreateCLI::Arguments Args;
      bool Result = TokenCreate->CreateToken(Args);
      if (!Result) {
        throw std::runtime_error{"Token create returned `false`, propagating"};
      }
    });
  TokenCreateCommand->excludes(Version);
  TokenCreateCommand->excludes(TokenSourceGroup);

  // token list
  CLI::App* TokenListCommand = TokenCommand->add_subcommand(TOKEN_LIST_COMMAND,
      TOKEN_LIST_DESCRIPTION);
  TokenListCommand->callback(
    [this]() {
      TokenListCLI::Arguments Args;
      bool Result = TokenList->ListTokens(Args);
      if (!Result) {
        throw std::runtime_error{"Token list returned `false`, propagating"};
      }
    });
  TokenListCommand->excludes(Version);
  TokenListCommand->excludes(TokenSourceGroup);

  // auth
  CLI::App* AuthCommand = App.add_subcommand(AUTH_COMMAND, AUTH_DESCRIPTION);
  AuthCommand->callback(
    [this]() {
      AuthCLI::Arguments Args;
      bool Result = Auth->Auth(Args);
      if (!Result) {
        throw std::runtime_error{"Auth returned `false`, propagating"};
      }
    });
  AuthCommand->excludes(Version);
  AuthCommand->excludes(TokenSourceGroup);

  // quota
  CLI::App* QuotaCommand = App.add_subcommand(QUOTA_COMMAND, QUOTA_DESCRIPTION);
  QuotaCommand->require_subcommand();

  // quota list
  CLI::App* QuotaListCommand = QuotaCommand->add_subcommand(QUOTA_LIST_COMMAND,
      QUOTA_LIST_DESCRIPTION);
  QuotaListCommand->callback(
    [this, &TokenStdin, &TokenVault, &TokenEnv]() {
      QuotaListCLI::Arguments Args;
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      bool Result = QuotaList->ListQuota(Args);
      if (!Result) {
        throw std::runtime_error{"Quota list returned `false`, propagating"};
      }
    });

  // organization
  CLI::App* OrganizationCommand = App.add_subcommand(ORGANIZATION_COMMAND,
      ORGANIZATION_DESCRIPTION);
  OrganizationCommand->require_subcommand();

  // organization list
  CLI::App* OrganizationListCommand = OrganizationCommand->add_subcommand(
      ORGANIZATION_LIST_COMMAND, ORGANIZATION_LIST_DESCRIPTION);
  OrganizationListCommand->callback(
    [this, &TokenStdin, &TokenVault, &TokenEnv]() {
      OrganizationListCLI::Arguments Args;
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      bool Result = OrganizationList->ListOrganizations(Args);
      if (!Result) {
        throw std::runtime_error{"Org list returned `false`, propagating"};
      }
    });

  // organization create
  CLI::App* OrganizationCreateCommand = OrganizationCommand->add_subcommand(
      ORGANIZATION_CREATE_COMMAND, ORGANIZATION_CREATE_DESCRIPTION);
  std::string OrganizationCreateName;
  OrganizationCreateCommand->add_option(ORGANIZATION_CREATE_OPTION_NAME,
    OrganizationCreateName,
    ORGANIZATION_CREATE_OPTION_NAME_DESCRIPTION)->required();
  OrganizationCreateCommand->callback(
    [this, &OrganizationCreateName, &TokenStdin, &TokenVault, &TokenEnv]() {
      OrganizationCreateCLI::Arguments Args;
      Args.Name = OrganizationCreateName;
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      bool Result = OrganizationCreate->CreateOrganization(Args);
      if (!Result) {
        throw std::runtime_error{"Org create returned `false`, propagating"};
      }
    });

  // organization delete
  CLI::App* OrganizationDeleteCommand = OrganizationCommand->add_subcommand(
      ORGANIZATION_DELETE_COMMAND, ORGANIZATION_DELETE_DESCRIPTION);
  std::string OrganizationDeleteName;
  bool OrganizationDeletePreConfirmation = false;
  OrganizationDeleteCommand->add_option(ORGANIZATION_DELETE_OPTION_NAME,
    OrganizationDeleteName,
    ORGANIZATION_DELETE_OPTION_NAME_DESCRIPTION)->required();
  OrganizationDeleteCommand->add_flag(ORGANIZATION_DELETE_FLAG_CONFIRM,
      OrganizationDeletePreConfirmation,
      ORGANIZATION_DELETE_FLAG_CONFIRM_DESCRIPTION);
  OrganizationDeleteCommand->callback(
    [this, &OrganizationDeleteName, &OrganizationDeletePreConfirmation,
     &TokenStdin, &TokenVault, &TokenEnv]() {
      OrganizationDeleteCLI::Arguments Args;
      Args.Name = OrganizationDeleteName;
      Args.PreConfirmation = OrganizationDeletePreConfirmation;
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      bool Result = OrganizationDelete->DeleteOrganization(Args);
      if (!Result) {
        throw std::runtime_error{"Org delete returned `false`, propagating"};
      }
    });

  // project
  CLI::App* ProjectCommand = App.add_subcommand(PROJECT_COMMAND,
      PROJECT_DESCRIPTION);
  ProjectCommand->require_subcommand();

  // project list
  CLI::App* ProjectListCommand = ProjectCommand->add_subcommand(
      PROJECT_LIST_COMMAND, PROJECT_LIST_DESCRIPTION);
  std::string ProjectListOrganization;
  ProjectListCommand->add_option(PROJECT_LIST_OPTION_ORGANIZATION,
      ProjectListOrganization,
      PROJECT_LIST_OPTION_ORGANIZATION_DESCRIPTION)->required();
  ProjectListCommand->callback(
    [this, &TokenStdin, &TokenVault, &TokenEnv, &ProjectListOrganization]() {
      ProjectListCLI::Arguments Args;
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      Args.Organization = ProjectListOrganization;
      bool Result = ProjectList->ListProjects(Args);
      if (!Result) {
        throw std::runtime_error{"Project list returned `false`, propagating"};
      }
    });

  // project create
  CLI::App* ProjectCreateCommand = ProjectCommand->add_subcommand(
      PROJECT_CREATE_COMMAND, PROJECT_CREATE_DESCRIPTION);
  std::string ProjectCreateLocation;
  ProjectCreateCommand->add_option(PROJECT_CREATE_OPTION_LOCATION,
    ProjectCreateLocation,
    PROJECT_CREATE_OPTION_LOCATION_DESCRIPTION)->required();
  ProjectCreateCommand->callback(
    [this, &ProjectCreateLocation, &TokenStdin, &TokenVault, &TokenEnv]() {
      ProjectCreateCLI::Arguments Args;
      Args.Location = ProjectCreateLocation;
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      bool Result = ProjectCreate->CreateProject(Args);
      if (!Result) {
        throw std::runtime_error{"Project create returned `false`, "
            "propagating"};
      }
    });

  // project delete
  CLI::App* ProjectDeleteCommand = ProjectCommand->add_subcommand(
      PROJECT_DELETE_COMMAND, PROJECT_DELETE_DESCRIPTION);
  std::string ProjectDeleteLocation;
  bool ProjectDeletePreConfirmation = false;
  std::string ProjectDeleteVersion;
  bool ProjectDeleteVersionsOnly = false;
  ProjectDeleteCommand->add_option(PROJECT_DELETE_OPTION_LOCATION,
    ProjectDeleteLocation,
    PROJECT_DELETE_OPTION_LOCATION_DESCRIPTION)->required();
  ProjectDeleteCommand->add_flag(PROJECT_DELETE_FLAG_CONFIRM,
      ProjectDeletePreConfirmation,
      PROJECT_DELETE_FLAG_CONFIRM_DESCRIPTION);
  CLI::Option* ProjectDeleteOptionVersion = ProjectDeleteCommand->add_option(
      PROJECT_DELETE_OPTION_VERSION, ProjectDeleteVersion,
      PROJECT_DELETE_OPTION_VERSION_DESCRIPTION);
  ProjectDeleteCommand->add_flag(
      PROJECT_DELETE_FLAG_ONLY_VERSIONS, ProjectDeleteVersionsOnly,
      PROJECT_DELETE_FLAG_ONLY_VERSIONS_DESCRIPTION)->
      excludes(ProjectDeleteOptionVersion);
  ProjectDeleteCommand->callback(
    [this, &ProjectDeleteLocation, &ProjectDeletePreConfirmation,
     &ProjectDeleteVersion, &ProjectDeleteVersionsOnly,
     &TokenStdin, &TokenVault, &TokenEnv]() {
      ProjectDeleteCLI::Arguments Args;
      Args.Location = ProjectDeleteLocation;
      Args.DeleteAllVersionsOnly = ProjectDeleteVersionsOnly;
      if (!ProjectDeleteVersion.empty()) {
        Args.Version = ProjectDeleteVersion;
      }
      Args.PreConfirmation = ProjectDeletePreConfirmation;
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      bool Result = ProjectDelete->DeleteProject(Args);
      if (!Result) {
        throw std::runtime_error{"Project delete returned `false`, "
            "propagating"};
      }
    });

  // project announce
  CLI::App* ProjectAnnounceCommand = ProjectCommand->add_subcommand(
      PROJECT_ANNOUNCE_COMMAND, PROJECT_ANNOUNCE_DESCRIPTION);
  std::string ProjectAnnounceLocation;
  std::string ProjectAnnounceText;
  bool ProjectAnnounceClear = false;
  ProjectAnnounceCommand->add_option(PROJECT_ANNOUNCE_OPTION_LOCATION,
      ProjectAnnounceLocation,
      PROJECT_ANNOUNCE_OPTION_LOCATION_DESCRIPTION)->required();
  CLI::Option_group* ProjectAnnounceGroup =
      ProjectAnnounceCommand->add_option_group(PROJECT_ANNOUNCE_GROUP);
  ProjectAnnounceGroup->required();
  ProjectAnnounceGroup->add_option(PROJECT_ANNOUNCE_OPTION_TEXT,
      ProjectAnnounceText,
      PROJECT_ANNOUNCE_OPTION_TEXT_DESCRIPTION);
  ProjectAnnounceGroup->add_flag(PROJECT_ANNOUNCE_FLAG_CONFIRM,
      ProjectAnnounceClear, PROJECT_ANNOUNCE_FLAG_CONFIRM_DESCRIPTION);
  ProjectAnnounceCommand->callback(
    [this, &ProjectAnnounceLocation, &TokenStdin, &TokenVault, &TokenEnv,
     &ProjectAnnounceText]() {
      ProjectAnnounceCLI::Arguments Args;
      Args.Location = ProjectAnnounceLocation;
      Args.Announcement = ProjectAnnounceText;
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      bool Result = ProjectAnnounce->Announce(Args);
      if (!Result) {
        throw std::runtime_error{"Project announce returned `false`, "
            "propagating"};
      }
    });

  // project publish
  CLI::App* ProjectPublishCommand = ProjectCommand->add_subcommand(
      PROJECT_PUBLISH_COMMAND, PROJECT_PUBLISH_DESCRIPTION);
  std::string ProjectPublishLocation;
  bool IsPrivate = false;
  bool IsPublic = false;
  ProjectPublishCommand->add_option(PROJECT_PUBLISH_OPTION_LOCATION,
      ProjectPublishLocation,
      PROJECT_PUBLISH_OPTION_LOCATION_DESCRIPTION)->required();
  CLI::Option_group* ProjectPublishScope =
      ProjectPublishCommand->add_option_group(PROJECT_PUBLISH_SCOPE_GROUP);
  ProjectPublishScope->required();

  ProjectPublishScope->add_flag(PROJECT_PUBLISH_FLAG_PRIVATE,
      IsPrivate, PROJECT_PUBLISH_FLAG_PRIVATE_DESCRIPTION);
  ProjectPublishScope->add_flag(PROJECT_PUBLISH_FLAG_PUBLIC,
      IsPublic, PROJECT_PUBLISH_FLAG_PUBLIC_DESCRIPTION);
  ProjectPublishCommand->callback(
    [this, &ProjectPublishLocation, &TokenStdin, &TokenVault, &TokenEnv,
     &IsPrivate, &IsPublic]() {
      ProjectPublishCLI::Arguments Args;
      Args.Location = ProjectPublishLocation;
      Args.Scope = IsPublic
          ? ProjectService::PublishProjectRequest::Scope::PUBLIC
          : ProjectService::PublishProjectRequest::Scope::PRIVATE;
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      bool Result = ProjectPublish->PublishProject(Args);
      if (!Result) {
        throw std::runtime_error{"Project publish returned `false`, "
            "propagating"};
      }
    });

  // project collaborator
  CLI::App* CollaboratorCommand = ProjectCommand->add_subcommand(
      COLLABORATOR_COMMAND, COLLABORATOR_DESCRIPTION);
  CollaboratorCommand->require_subcommand();

  // project collaborator list
  CLI::App* CollaboratorListCommand = CollaboratorCommand->add_subcommand(
      COLLABORATOR_LIST_COMMAND, COLLABORATOR_LIST_DESCRIPTION);
  std::string CollaboratorListLocation;
  CollaboratorListCommand->add_option(COLLABORATOR_LIST_OPTION_LOCATION,
      CollaboratorListLocation,
      COLLABORATOR_LIST_OPTION_LOCATION_DESCRIPTION)->required();
  CollaboratorListCommand->callback(
    [this, &TokenStdin, &TokenVault, &TokenEnv, &CollaboratorListLocation]() {
      ProjectCollaboratorListCLI::Arguments Args;
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      Args.Location = CollaboratorListLocation;
      bool Result = ProjectCollaboratorList->ListProjectCollaborators(Args);
      if (!Result) {
        throw std::runtime_error{"Project collaborator list returned "
            "`false`, propagating"};
      }
    });

  // project collaborator add
  CLI::App* CollaboratorAddCommand = CollaboratorCommand->add_subcommand(
      COLLABORATOR_ADD_COMMAND, COLLABORATOR_ADD_DESCRIPTION);
  std::string CollaboratorAddLocation;
  std::string CollaboratorAddEmail;
  CollaboratorAddCommand->add_option(COLLABORATOR_ADD_OPTION_LOCATION,
    CollaboratorAddLocation,
    COLLABORATOR_ADD_OPTION_LOCATION_DESCRIPTION)->required();
  CollaboratorAddCommand->add_option(COLLABORATOR_ADD_OPTION_EMAIL,
    CollaboratorAddEmail,
    COLLABORATOR_ADD_OPTION_EMAIL_DESCRIPTION)->required();
  CollaboratorAddCommand->callback(
    [this, &CollaboratorAddLocation, &CollaboratorAddEmail, &TokenStdin,
     &TokenVault, &TokenEnv]() {
      ProjectCollaboratorAddCLI::Arguments Args;
      Args.Location = CollaboratorAddLocation;
      Args.Email = CollaboratorAddEmail;
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      bool Result = ProjectCollaboratorAdd->AddProjectCollaborator(Args);
      if (!Result) {
        throw std::runtime_error{"Project collaborator add returned `false`, "
            "propagating"};
      }
    });

  // project collaborator delete
  CLI::App* CollaboratorDeleteCommand = CollaboratorCommand->add_subcommand(
      COLLABORATOR_DELETE_COMMAND, COLLABORATOR_DELETE_DESCRIPTION);
  std::string CollaboratorDeleteLocation;
  bool CollaboratorDeletePreConfirmation = false;
  std::string CollaboratorDeleteEmail;
  CollaboratorDeleteCommand->add_option(COLLABORATOR_DELETE_OPTION_LOCATION,
    CollaboratorDeleteLocation,
    COLLABORATOR_DELETE_OPTION_LOCATION_DESCRIPTION)->required();
  CollaboratorDeleteCommand->add_flag(COLLABORATOR_DELETE_FLAG_CONFIRM,
      CollaboratorDeletePreConfirmation,
      COLLABORATOR_DELETE_FLAG_CONFIRM_DESCRIPTION);
  CollaboratorDeleteCommand->add_option(
      COLLABORATOR_DELETE_OPTION_EMAIL, CollaboratorDeleteEmail,
      COLLABORATOR_DELETE_OPTION_EMAIL_DESCRIPTION)->required();
  CollaboratorDeleteCommand->callback(
    [this, &CollaboratorDeleteLocation, &CollaboratorDeletePreConfirmation,
     &CollaboratorDeleteEmail, &TokenStdin, &TokenVault, &TokenEnv]() {
      ProjectCollaboratorDeleteCLI::Arguments Args;
      Args.Location = CollaboratorDeleteLocation;
      Args.Email = CollaboratorDeleteEmail;
      Args.PreConfirmation = CollaboratorDeletePreConfirmation;
      Args.Source = DetermineSource(TokenVault, TokenEnv, TokenStdin);
      bool Result = ProjectCollaboratorDelete->DeleteProjectCollaborator(Args);
      if (!Result) {
        throw std::runtime_error{"Project collaborator delete returned "
            "`false`, propagating"};
      }
    });

  // migrate
  CLI::App* MigrateCommand = App.add_subcommand(
      MIGRATE_COMMAND, MIGRATE_DESCRIPTION);
  MigrateCommand->callback(
    [this]() {
      MigrateCLI::Arguments Args;
      bool Result = Migrate->Migrate(Args);
      if (!Result) {
        throw std::runtime_error{"Migrate returned `false`, propagating"};
      }
    });

  try {
    App.parse(Argc, Argv);
  } catch (const CLI::ParseError &Exc) {
    l->error("Command Parsing Exception: {}", Exc.what());
    return !App.exit(Exc);
  } catch (const std::exception& Exc) {
    l->error("Exception: {}", Exc.what());
    return false;
  }

  return true;
}

udocs_processor::TokenLoader::TokenSource
    udocs_processor::SurDocsCLI::DetermineSource(
        bool TokenVault, bool TokenEnv, bool TokenStdin) const {
  if (TokenVault) {
    return TokenLoader::TokenSource::VAULT;
  }
  if (TokenStdin) {
    return TokenLoader::TokenSource::STDINPUT;
  }
  if (TokenEnv) {
    return TokenLoader::TokenSource::ENVIRONMENT;
  }
  return TokenLoader::TokenSource::NO_PREFERENCE;
}

udocs_processor::SurDocsCLI::SurDocsCLI(
    std::shared_ptr<spdlog::sinks::sink> Sink,
    std::shared_ptr<StandarInputReader> InputReader,
    std::unique_ptr<GenerateCLI> Generate,
    std::unique_ptr<InitCLI> Init,
    std::unique_ptr<TokenCreateCLI> TokenCreate,
    std::unique_ptr<TokenListCLI> TokenList,
    std::unique_ptr<AuthCLI> Auth,
    std::unique_ptr<QuotaListCLI> QuotaList,
    std::unique_ptr<OrganizationListCLI> OrganizationList,
    std::unique_ptr<OrganizationCreateCLI> OrganizationCreate,
    std::unique_ptr<OrganizationDeleteCLI> OrganizationDelete,
    std::unique_ptr<ProjectAnnounceCLI> ProjectAnnounce,
    std::unique_ptr<ProjectCollaboratorAddCLI> ProjectCollaboratorAdd,
    std::unique_ptr<ProjectCollaboratorDeleteCLI> ProjectCollaboratorDelete,
    std::unique_ptr<ProjectCollaboratorListCLI> ProjectCollaboratorList,
    std::unique_ptr<ProjectCreateCLI> ProjectCreate,
    std::unique_ptr<ProjectDeleteCLI> ProjectDelete,
    std::unique_ptr<ProjectListCLI> ProjectList,
    std::unique_ptr<ProjectPublishCLI> ProjectPublish,
    std::unique_ptr<MigrateCLI> Migrate,
    std::shared_ptr<TelemetryReporter> Telemetry)
    : Generate(std::move(Generate)), Init(std::move(Init)),
    TokenCreate(std::move(TokenCreate)), TokenList(std::move(TokenList)),
    Auth(std::move(Auth)), Telemetry(Telemetry), InputReader(InputReader),
    QuotaList(std::move(QuotaList)),
    OrganizationList(std::move(OrganizationList)),
    OrganizationCreate(std::move(OrganizationCreate)),
    OrganizationDelete(std::move(OrganizationDelete)),
    ProjectAnnounce(std::move(ProjectAnnounce)),
    ProjectCollaboratorAdd(std::move(ProjectCollaboratorAdd)),
    ProjectCollaboratorDelete(std::move(ProjectCollaboratorDelete)),
    ProjectCollaboratorList(std::move(ProjectCollaboratorList)),
    ProjectCreate(std::move(ProjectCreate)),
    ProjectDelete(std::move(ProjectDelete)),
    ProjectList(std::move(ProjectList)),
    ProjectPublish(std::move(ProjectPublish)),
    Migrate(std::move(Migrate)) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}
