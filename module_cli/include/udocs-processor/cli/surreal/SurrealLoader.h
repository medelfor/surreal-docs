/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <optional>

namespace udocs_processor {
struct SurrealProject {
  struct Page {
    enum class Type {
      PAGE,
      API_CPP,
      API_BLUEPRINT
    };
    std::optional<Type> Type_ = Type::PAGE;
    std::optional<std::string> Address;
    std::optional<std::string> Title;
    std::optional<bool> IsExternal = false;
    std::optional<bool> IsPinned = false;
  };

  struct ContentEntry {
    std::string Title;
    std::vector<Page> Pages;
  };

  std::vector<std::string> ContentInclude;
  std::vector<std::string> ContentExclude;
  std::vector<std::string> NativeInclude;
  std::vector<std::string> NativeExclude;
  std::vector<std::string> CppInputs;
  std::vector<std::string> CppExclude;
  std::string EngineRoot;
  std::string Target;
  std::string Configuration;
  bool DoExportPrivate = false;
  std::string Logo;
  std::string Tag;
  std::string Version;
  std::string Name;
  std::string Url;
  std::string Organization;
  std::string Copyright;
  std::string DocsTitle;
  std::string Language;
  std::string Master;
  std::string ImagesRoot;
  std::string ContactMeAt;
  std::vector<ContentEntry> Contents;
};

class SurrealLoader {
 public:
  SurrealLoader();

  std::unique_ptr<SurrealProject> Deserialize(const std::string& Project) const;

  std::unique_ptr<SurrealProject> Deserialize(
      const nlohmann::json& Project) const;

  std::string Serialize(const SurrealProject& Project) const;

 private:
  static nlohmann::json_pointer<std::string> EngineRootPath;
  static nlohmann::json_pointer<std::string> TargetPath;
  static nlohmann::json_pointer<std::string> ConfigurationPath;
  static nlohmann::json_pointer<std::string> DoExportPrivatePath;
  static nlohmann::json_pointer<std::string> ContentIncludePath;
  static nlohmann::json_pointer<std::string> ContentExcludePath;
  static nlohmann::json_pointer<std::string> NativeIncludePath;
  static nlohmann::json_pointer<std::string> NativeExcludePath;
  static nlohmann::json_pointer<std::string> CppIncludePath;
  static nlohmann::json_pointer<std::string> CppExcludePath;
  static nlohmann::json_pointer<std::string> LogoPath;
  static nlohmann::json_pointer<std::string> TagPath;
  static nlohmann::json_pointer<std::string> VersionPath;
  static nlohmann::json_pointer<std::string> UrlPath;
  static nlohmann::json_pointer<std::string> NamePath;
  static nlohmann::json_pointer<std::string> OrganizationPath;
  static nlohmann::json_pointer<std::string> CopyrightPath;
  static nlohmann::json_pointer<std::string> TitlePath;
  static nlohmann::json_pointer<std::string> LanguagePath;
  static nlohmann::json_pointer<std::string> MasterPath;
  static nlohmann::json_pointer<std::string> ContactMeAtPath;
  static nlohmann::json_pointer<std::string> ImagesRootPath;
  static nlohmann::json_pointer<std::string> ContentsPath;
  static nlohmann::json_pointer<std::string> ContentEntryTitlePath;
  static nlohmann::json_pointer<std::string> ContentEntryPagesPath;
  static nlohmann::json_pointer<std::string> PageTitlePath;
  static nlohmann::json_pointer<std::string> PageIsPinnedPath;
  static nlohmann::json_pointer<std::string> PageIsExternalPath;
  static nlohmann::json_pointer<std::string> PagePagePath;
  static nlohmann::json_pointer<std::string> PageApiPath;

  static constexpr const char SURREAL_CONTACT_ME_AT_PATH[] = "/contact_me_at";
  static constexpr const char SURREAL_LOGO_PATH[] = "/logo";
  static constexpr const char SURREAL_TAG_PATH[] = "/tag";
  static constexpr const char SURREAL_VERSION_PATH[] = "/version";
  static constexpr const char SURREAL_NAME_PATH[] = "/name";
  static constexpr const char SURREAL_ORGANIZATION_PATH[] = "/organization";
  static constexpr const char SURREAL_COPYRIGHT_PATH[] = "/copyright";
  static constexpr const char SURREAL_TITLE_PATH[] = "/docs_title";
  static constexpr const char SURREAL_LANGUAGE_PATH[] = "/language";
  static constexpr const char SURREAL_MASTER_PATH[] = "/master";
  static constexpr const char SURREAL_URL_PATH[] = "/url";
  static constexpr const char SURREAL_IMAGES_ROOT_PATH[] = "/images_root";
  static constexpr const char SURREAL_CONTENTS_PATH[] = "/contents";
  static constexpr const char SURREAL_CONTENT_ENTRY_TITLE_PATH[] = "/title";
  static constexpr const char SURREAL_CONTENT_ENTRY_PAGES_PATH[] = "/pages";
  static constexpr const char SURREAL_PAGE_TITLE_PATH[] = "/title";
  static constexpr const char SURREAL_PAGE_IS_PINNED_PATH[] = "/is_pinned";
  static constexpr const char SURREAL_PAGE_IS_EXTERNAL_PATH[] = "/is_external";
  static constexpr const char SURREAL_PAGE_PAGE_PATH[] = "/page";
  static constexpr const char SURREAL_PAGE_API_PATH[] = "/api";
  static constexpr const char SURREAL_PAGE_API_BLUEPRINT[] = "blueprint";
  static constexpr const char SURREAL_PAGE_API_CPP[] = "cpp";
  static constexpr const char SURREAL_ENGINE_ROOT_PATH[] = "/ue/root";
  static constexpr const char SURREAL_TARGET_PATH[] = "/ue/target";
  static constexpr const char SURREAL_CONFIGURATION_PATH[] =
      "/ue/configuration";
  static constexpr const char SURREAL_DO_EXPORT_PRIVATE_PATH[] =
      "/filters/do_export_private";
  static constexpr const char SURREAL_CONTENT_INCLUDE_PATH[] =
      "/filters/content/include";
  static constexpr const char SURREAL_CONTENT_EXCLUDE_PATH[] =
      "/filters/content/exclude";
  static constexpr const char SURREAL_NATIVE_INCLUDE_PATH[] =
      "/filters/native_modules/include";
  static constexpr const char SURREAL_NATIVE_EXCLUDE_PATH[] =
      "/filters/native_modules/exclude";
  static constexpr const char SURREAL_CPP_INCLUDE_PATH[] =
      "/filters/cpp/include";
  static constexpr const char SURREAL_CPP_EXCLUDE_PATH[] =
      "/filters/cpp/exclude";

  static constexpr const int JSON_INDENT = 4;

  std::map<std::string, SurrealProject::Page::Type> StringToType;
  std::map<SurrealProject::Page::Type, std::string> TypeToString;
};
}  // namespace udocs_processor
