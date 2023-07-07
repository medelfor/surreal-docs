/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <memory>
#include "udocs-processor/cli/cli/GenerateCLI.h"
#include "udocs-processor/cli/cli/InitCLI.h"
#include "udocs-processor/cli/cli/TokenCreateCLI.h"
#include "udocs-processor/cli/cli/TokenListCLI.h"
#include "udocs-processor/cli/cli/AuthCLI.h"
#include "StandardInputReader.h"
#include "udocs-processor/cli/cli/QuotaListCLI.h"
#include "udocs-processor/cli/cli/OrganizationListCLI.h"
#include "udocs-processor/cli/cli/OrganizationCreateCLI.h"
#include "udocs-processor/cli/cli/OrganizationDeleteCLI.h"
#include "udocs-processor/cli/cli/ProjectAnnounceCLI.h"
#include "udocs-processor/cli/cli/ProjectCollaboratorAddCLI.h"
#include "udocs-processor/cli/cli/ProjectCollaboratorDeleteCLI.h"
#include "udocs-processor/cli/cli/ProjectCreateCLI.h"
#include "udocs-processor/cli/cli/ProjectDeleteCLI.h"
#include "udocs-processor/cli/cli/ProjectListCLI.h"
#include "udocs-processor/cli/cli/ProjectPublishCLI.h"
#include "udocs-processor/cli/cli/ProjectCollaboratorListCLI.h"
#include "udocs-processor/cli/cli/MigrateCLI.h"

namespace udocs_processor {
class SurDocsCLI {
 public:
  SurDocsCLI(std::shared_ptr<spdlog::sinks::sink> Sink,
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
      std::shared_ptr<TelemetryReporter> Telemetry);

  bool Start(int Argc, char** Argv);

 private:
  TokenLoader::TokenSource DetermineSource(bool TokenVault, bool TokenEnv,
      bool TokenStdin) const;

  static constexpr const char* LOGGER_NAME = "cli";
  static constexpr const char* APP_NAME = "surdocs";

  static constexpr const char* CLI_DESCRIPTION = "Command line interface to "
      "interact with Surreal Docs. Most of the commands expect working "
      "directory to be a root of an Unreal Engine project";

  static constexpr const char* GLOBAL_FLAG_NO_TELE = "--no-tele";
  static constexpr const char* GLOBAL_FLAG_NO_TELE_DESCRIPTION =
      "Disables sending anonymized usage data to Medelfor during execution "
      "of the current command";

  static constexpr const char* TOKEN_SOURCE_GROUP = "Token Source";

  static constexpr const char* GLOBAL_FLAG_TOKEN_VAULT = "--token-vault,-a";
  static constexpr const char* GLOBAL_FLAG_TOKEN_VAULT_DESCRIPTION =
      "Load the access token from the vault";

  static constexpr const char* GLOBAL_FLAG_TOKEN_ENV = "--token-env,-e";
  static constexpr const char* GLOBAL_FLAG_TOKEN_ENV_DESCRIPTION =
      "Load the access token from the SURDOCS_TOKEN environment variable";

  static constexpr const char* GLOBAL_FLAG_TOKEN_STDIN = "--token-stdin,-s";
  static constexpr const char* GLOBAL_FLAG_TOKEN_STDIN_DESCRIPTION =
      "Load the access token from the standard input";

  static constexpr const char* FLAG_VERSION = "-v,--version";
  static constexpr const char* FLAG_VERSION_DESCRIPTION =
      "Show version of Surreal Docs CLI";

  static constexpr const char* GENERATE_FLAG_OUT_DIRECTORY = "out";
  static constexpr const char* GENERATE_FLAG_OUT_DIRECTORY_DESCRIPTION =
      "Directory where to dump results of docs generation";
  static constexpr const char* GENERATE_OPTION_DO_DEPLOY = "-d,--deploy";
  static constexpr const char* GENERATE_OPTION_DO_DEPLOY_DESCRIPTION =
      "Address where to deploy the docs in format <organization>/<project>. "
      "Expects an access token.";
  static constexpr const char* GENERATE_FLAG_CLEAN = "-c,--clean";
  static constexpr const char* GENERATE_FLAG_CLEAN_DESCRIPTION =
      "Clear the out directory. Use with caution";
  static constexpr const char* GENERATE_FLAG_NON_INTERACTIVE = "-n,--non-rich";
  static constexpr const char* GENERATE_FLAG_NON_INTERACTIVE_DESCRIPTION =
      "Fallback to non-rich UI. Recommended to add when calling from a script";
  static constexpr const char* GENERATE_COMMAND = "generate";
  static constexpr const char* GENERATE_DESCRIPTION =
      "Start docs generation";

  static constexpr const char* INIT_COMMAND = "init";
  static constexpr const char* INIT_DESCRIPTION =
      "Initialize/reconfigure Surreal Docs for this Unreal Engine project";

  static constexpr const char* TOKEN_COMMAND = "token";
  static constexpr const char* TOKEN_DESCRIPTION =
      "Manage Surreal Cloud access tokens";

  static constexpr const char* TOKEN_CREATE_COMMAND = "create";
  static constexpr const char* TOKEN_CREATE_DESCRIPTION =
      "Issue a Surreal Cloud access token. Login required via Medelfor account";

  static constexpr const char* TOKEN_LIST_COMMAND = "list";
  static constexpr const char* TOKEN_LIST_DESCRIPTION =
      "List issued Surreal Cloud access tokens. Login required "
      "via Medelfor account";

  static constexpr const char* AUTH_COMMAND = "auth";
  static constexpr const char* AUTH_DESCRIPTION =
      "Save a Surreal Cloud access token in the vault. Expects the token to "
      "be in stdandard input";

  static constexpr const char* QUOTA_COMMAND = "quota";
  static constexpr const char* QUOTA_DESCRIPTION =
      "Manage Surreal Cloud resources quota";

  static constexpr const char* QUOTA_LIST_COMMAND = "list";
  static constexpr const char* QUOTA_LIST_DESCRIPTION =
      "List available and used Surreal Cloud resources. Expects an access "
      "token";

  static constexpr const char* ORGANIZATION_COMMAND = "organization";
  static constexpr const char* ORGANIZATION_DESCRIPTION =
      "Manage Surreal Cloud organizations";

  static constexpr const char* ORGANIZATION_LIST_COMMAND = "list";
  static constexpr const char* ORGANIZATION_LIST_DESCRIPTION =
      "List owned Surreal Cloud organizations. Expects an access token";

  static constexpr const char* ORGANIZATION_CREATE_COMMAND = "create";
  static constexpr const char* ORGANIZATION_CREATE_DESCRIPTION =
      "Create a new Surreal Cloud organization. Expects an access token";
  static constexpr const char* ORGANIZATION_CREATE_OPTION_NAME = "name";
  static constexpr const char* ORGANIZATION_CREATE_OPTION_NAME_DESCRIPTION =
      "Name of the organization to create";

  static constexpr const char* ORGANIZATION_DELETE_COMMAND = "delete";
  static constexpr const char* ORGANIZATION_DELETE_DESCRIPTION =
      "Delete a Surreal Cloud organization, all its projects and documents. "
      "Expects an access token";
  static constexpr const char* ORGANIZATION_DELETE_OPTION_NAME = "name";
  static constexpr const char* ORGANIZATION_DELETE_OPTION_NAME_DESCRIPTION =
      "Name of the organization to delete";
  static constexpr const char* ORGANIZATION_DELETE_FLAG_CONFIRM = "-y,--yes";
  static constexpr const char* ORGANIZATION_DELETE_FLAG_CONFIRM_DESCRIPTION =
      "Confirm deletion";

  static constexpr const char* PROJECT_COMMAND = "project";
  static constexpr const char* PROJECT_DESCRIPTION =
      "Manage Surreal Cloud projects";

  static constexpr const char* PROJECT_LIST_COMMAND = "list";
  static constexpr const char* PROJECT_LIST_DESCRIPTION =
      "List projects within the specified organization. Expects an access "
      "token";
  static constexpr const char* PROJECT_LIST_OPTION_ORGANIZATION =
      "organization";
  static constexpr const char* PROJECT_LIST_OPTION_ORGANIZATION_DESCRIPTION =
      "The organization to load projects from";

  static constexpr const char* PROJECT_CREATE_COMMAND = "create";
  static constexpr const char* PROJECT_CREATE_DESCRIPTION =
      "Create a new project within the specified organization. "
      "Expects an access token";
  static constexpr const char* PROJECT_CREATE_OPTION_LOCATION = "location";
  static constexpr const char* PROJECT_CREATE_OPTION_LOCATION_DESCRIPTION =
      "Address of the project to create, in format <organization>/<project>";

  static constexpr const char* PROJECT_DELETE_COMMAND = "delete";
  static constexpr const char* PROJECT_DELETE_DESCRIPTION =
      "Delete the whole project, all its versions only or a single version. "
      "Expects an access token";
  static constexpr const char* PROJECT_DELETE_OPTION_LOCATION = "location";
  static constexpr const char* PROJECT_DELETE_OPTION_LOCATION_DESCRIPTION =
      "Address of the project to delete, in format <organization>/<project>";
  static constexpr const char* PROJECT_DELETE_FLAG_CONFIRM = "-y,--yes";
  static constexpr const char* PROJECT_DELETE_FLAG_CONFIRM_DESCRIPTION =
      "Confirm deletion";
  static constexpr const char* PROJECT_DELETE_OPTION_VERSION = "-v,--version";
  static constexpr const char* PROJECT_DELETE_OPTION_VERSION_DESCRIPTION =
      "Specific version to delete";
  static constexpr const char* PROJECT_DELETE_FLAG_ONLY_VERSIONS =
      "-o,--versions-only";
  static constexpr const char* PROJECT_DELETE_FLAG_ONLY_VERSIONS_DESCRIPTION =
      "Remove all versions but keep the project";

  static constexpr const char* PROJECT_ANNOUNCE_COMMAND = "announce";
  static constexpr const char* PROJECT_ANNOUNCE_DESCRIPTION =
      "Set the project-wide banner with specified text";
  static constexpr const char* PROJECT_ANNOUNCE_GROUP = "Announcement";
  static constexpr const char* PROJECT_ANNOUNCE_OPTION_LOCATION =
      "location";
  static constexpr const char* PROJECT_ANNOUNCE_OPTION_LOCATION_DESCRIPTION =
      "Address of the project to make announcement in, in format "
      "<organization>/<project>";
  static constexpr const char* PROJECT_ANNOUNCE_OPTION_TEXT = "announcement";
  static constexpr const char* PROJECT_ANNOUNCE_OPTION_TEXT_DESCRIPTION =
      "Text of the announcement";
  static constexpr const char* PROJECT_ANNOUNCE_FLAG_CONFIRM = "-c,--clear";
  static constexpr const char* PROJECT_ANNOUNCE_FLAG_CONFIRM_DESCRIPTION =
      "Clear the announcement";

  static constexpr const char* PROJECT_PUBLISH_COMMAND = "publish";
  static constexpr const char* PROJECT_PUBLISH_DESCRIPTION =
      "Make the project public (available to the Internet) or private "
      "(available to collaborators only)";
  static constexpr const char* PROJECT_PUBLISH_OPTION_LOCATION = "location";
  static constexpr const char* PROJECT_PUBLISH_OPTION_LOCATION_DESCRIPTION =
      "Address of the project to publish, in format "
      "<organization>/<project>";
  static constexpr const char* PROJECT_PUBLISH_SCOPE_GROUP = "Project Scope";
  static constexpr const char* PROJECT_PUBLISH_FLAG_PRIVATE = "-r,--private";
  static constexpr const char* PROJECT_PUBLISH_FLAG_PRIVATE_DESCRIPTION =
      "Make available to collaborators";
  static constexpr const char* PROJECT_PUBLISH_FLAG_PUBLIC = "-p,--public";
  static constexpr const char* PROJECT_PUBLISH_FLAG_PUBLIC_DESCRIPTION =
      "Make available to the Internet";

  static constexpr const char* COLLABORATOR_COMMAND = "collaborator";
  static constexpr const char* COLLABORATOR_DESCRIPTION =
      "Manage collaborators";

  static constexpr const char* COLLABORATOR_LIST_COMMAND = "list";
  static constexpr const char* COLLABORATOR_LIST_DESCRIPTION =
      "List colaborators added to the specified project. Expects an access "
      "token";
  static constexpr const char* COLLABORATOR_LIST_OPTION_LOCATION = "location";
  static constexpr const char* COLLABORATOR_LIST_OPTION_LOCATION_DESCRIPTION =
      "Address of the project to load collaborators from, in format "
      "<organization>/<project>";

  static constexpr const char* COLLABORATOR_ADD_COMMAND = "add";
  static constexpr const char* COLLABORATOR_ADD_DESCRIPTION =
      "Add a new collaborator to the specified roject. "
      "Expects an access token";
  static constexpr const char* COLLABORATOR_ADD_OPTION_LOCATION = "location";
  static constexpr const char* COLLABORATOR_ADD_OPTION_LOCATION_DESCRIPTION =
      "Address of the project to add a collaborator to, in format "
      "<organization>/<project>";
  static constexpr const char* COLLABORATOR_ADD_OPTION_EMAIL = "email";
  static constexpr const char* COLLABORATOR_ADD_OPTION_EMAIL_DESCRIPTION =
      "Email address of the collaborator's Medelfor account";

  static constexpr const char* COLLABORATOR_DELETE_COMMAND = "delete";
  static constexpr const char* COLLABORATOR_DELETE_DESCRIPTION =
      "Remove a collaborator out of the project. "
      "Expects an access token";
  static constexpr const char* COLLABORATOR_DELETE_OPTION_LOCATION = "location";
  static constexpr const char* COLLABORATOR_DELETE_OPTION_LOCATION_DESCRIPTION =
      "Address of the project to delete, in format <organization>/<project>";
  static constexpr const char* COLLABORATOR_DELETE_FLAG_CONFIRM = "-y,--yes";
  static constexpr const char* COLLABORATOR_DELETE_FLAG_CONFIRM_DESCRIPTION =
      "Confirm deletion";
  static constexpr const char* COLLABORATOR_DELETE_OPTION_EMAIL = "email";
  static constexpr const char* COLLABORATOR_DELETE_OPTION_EMAIL_DESCRIPTION =
      "Email address of the collaborator's Medelfor account";

  static constexpr const char* MIGRATE_COMMAND = "migrate";
  static constexpr const char* MIGRATE_DESCRIPTION =
      "Migrates Surreal project to the latest version. It's recommended to "
      "call it after every Surreal Docs update";

  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<StandarInputReader> InputReader;
  std::unique_ptr<GenerateCLI> Generate;
  std::unique_ptr<InitCLI> Init;
  std::unique_ptr<TokenCreateCLI> TokenCreate;
  std::unique_ptr<TokenListCLI> TokenList;
  std::unique_ptr<AuthCLI> Auth;
  std::unique_ptr<QuotaListCLI> QuotaList;
  std::unique_ptr<OrganizationListCLI> OrganizationList;
  std::unique_ptr<OrganizationCreateCLI> OrganizationCreate;
  std::unique_ptr<OrganizationDeleteCLI> OrganizationDelete;
  std::unique_ptr<ProjectAnnounceCLI> ProjectAnnounce;
  std::unique_ptr<ProjectCollaboratorAddCLI> ProjectCollaboratorAdd;
  std::unique_ptr<ProjectCollaboratorDeleteCLI> ProjectCollaboratorDelete;
  std::unique_ptr<ProjectCollaboratorListCLI> ProjectCollaboratorList;
  std::unique_ptr<ProjectCreateCLI> ProjectCreate;
  std::unique_ptr<ProjectDeleteCLI> ProjectDelete;
  std::unique_ptr<ProjectListCLI> ProjectList;
  std::unique_ptr<ProjectPublishCLI> ProjectPublish;
  std::unique_ptr<MigrateCLI> Migrate;
  std::shared_ptr<TelemetryReporter> Telemetry;
};
}  // namespace udocs_processor
