/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <nlohmann/json.hpp>
#include "udocs-processor/cli/surreal/SurrealLoader.h"

namespace udocs_processor {
nlohmann::json_pointer<std::string>
    SurrealLoader::EngineRootPath{SURREAL_ENGINE_ROOT_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::ContactMeAtPath{SURREAL_CONTACT_ME_AT_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::TargetPath{SURREAL_TARGET_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::ConfigurationPath{SURREAL_CONFIGURATION_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::DoExportPrivatePath{SURREAL_DO_EXPORT_PRIVATE_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::ContentIncludePath{SURREAL_CONTENT_INCLUDE_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::ContentExcludePath{SURREAL_CONTENT_EXCLUDE_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::NativeIncludePath{SURREAL_NATIVE_INCLUDE_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::NativeExcludePath{SURREAL_NATIVE_EXCLUDE_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::CppIncludePath{SURREAL_CPP_INCLUDE_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::CppExcludePath{SURREAL_CPP_EXCLUDE_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::LogoPath{SURREAL_LOGO_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::TagPath{SURREAL_TAG_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::VersionPath{SURREAL_VERSION_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::NamePath{SURREAL_NAME_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::OrganizationPath{SURREAL_ORGANIZATION_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::CopyrightPath{SURREAL_COPYRIGHT_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::TitlePath{SURREAL_TITLE_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::LanguagePath{SURREAL_LANGUAGE_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::MasterPath{SURREAL_MASTER_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::ImagesRootPath{SURREAL_IMAGES_ROOT_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::ContentsPath{SURREAL_CONTENTS_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::ContentEntryTitlePath{SURREAL_CONTENT_ENTRY_TITLE_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::ContentEntryPagesPath{SURREAL_CONTENT_ENTRY_PAGES_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::PageTitlePath{SURREAL_PAGE_TITLE_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::PageIsPinnedPath{SURREAL_PAGE_IS_PINNED_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::PageIsExternalPath{SURREAL_PAGE_IS_EXTERNAL_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::PagePagePath{SURREAL_PAGE_PAGE_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::PageApiPath{SURREAL_PAGE_API_PATH};
nlohmann::json_pointer<std::string>
    SurrealLoader::UrlPath{SURREAL_URL_PATH};
}  // namespace udocs_processor

std::unique_ptr<udocs_processor::SurrealProject>
    udocs_processor::SurrealLoader::Deserialize(
        const nlohmann::json& Data) const {
  std::unique_ptr<SurrealProject> Result = std::make_unique<SurrealProject>();

  Result->DoExportPrivate = Data.at(DoExportPrivatePath);
  Result->Target = Data.at(TargetPath);
  Result->EngineRoot = Data.at(EngineRootPath);
  Result->Configuration = Data.at(ConfigurationPath);
  Result->CppInputs = Data.at(CppIncludePath);
  Result->CppExclude = Data.at(CppExcludePath);
  Result->ContentInclude = Data.at(ContentIncludePath);
  Result->ContentExclude = Data.at(ContentExcludePath);
  Result->NativeInclude = Data.at(NativeIncludePath);
  Result->NativeExclude = Data.at(NativeExcludePath);
  Result->Logo = Data.at(LogoPath);
  Result->Tag = Data.at(TagPath);
  Result->Version = Data.at(VersionPath);
  Result->Name = Data.at(NamePath);
  Result->Url = Data.at(UrlPath);
  Result->Organization = Data.at(OrganizationPath);
  Result->Copyright = Data.at(CopyrightPath);
  Result->DocsTitle = Data.at(TitlePath);
  Result->Language = Data.at(LanguagePath);
  Result->Master = Data.at(MasterPath);
  Result->ImagesRoot = Data.at(ImagesRootPath);
  Result->Contents.reserve(Data.at(ContentsPath).size());
  if (Data.contains(ContactMeAtPath)) {
    Result->ContactMeAt = Data.at(ContactMeAtPath);
  }

  for (const auto& Entry : Data.at(ContentsPath)) {
    SurrealProject::ContentEntry NewEntry;
    NewEntry.Title = Entry.at(ContentEntryTitlePath);
    NewEntry.Pages.reserve(Entry.at(ContentEntryPagesPath).size());

    for (const auto& Page : Entry.at(ContentEntryPagesPath)) {
      SurrealProject::Page NewPage;

      if (Page.contains(PageApiPath)) {
        auto It = StringToType.find(Page.at(PageApiPath));
        NewPage.Type_ = It != StringToType.end()
            ? It->second : SurrealProject::Page::Type::PAGE;
      }
      if (Page.contains(PageTitlePath)) {
        NewPage.Title = Page.at(PageTitlePath);
      }
      if (Page.contains(PagePagePath)) {
        NewPage.Address = Page.at(PagePagePath);
      }
      if (Page.contains(PageIsExternalPath)) {
        NewPage.IsExternal = Page.at(PageIsExternalPath);
      }
      if (Page.contains(PageIsPinnedPath)) {
        NewPage.IsPinned = Page.at(PageIsPinnedPath);
      }

      NewEntry.Pages.emplace_back(std::move(NewPage));
    }

    Result->Contents.emplace_back(std::move(NewEntry));
  }

  return Result;
}

std::unique_ptr<udocs_processor::SurrealProject>
    udocs_processor::SurrealLoader::Deserialize(
        const std::string &Project) const {
  return Deserialize(nlohmann::json::parse(Project));
}

std::string udocs_processor::SurrealLoader::Serialize(
    const SurrealProject &Project) const {
  nlohmann::json Data;

  Data[DoExportPrivatePath] = Project.DoExportPrivate;
  Data[TargetPath] = Project.Target;
  Data[EngineRootPath] = Project.EngineRoot;
  Data[ConfigurationPath] = Project.Configuration;
  Data[CppIncludePath] = Project.CppInputs;
  Data[CppExcludePath] = Project.CppExclude;
  Data[ContentIncludePath] = Project.ContentInclude;
  Data[ContentExcludePath] = Project.ContentExclude;
  Data[NativeIncludePath] = Project.NativeInclude;
  Data[NativeExcludePath] = Project.NativeExclude;
  Data[LogoPath] = Project.Logo;
  Data[TagPath] = Project.Tag;
  Data[VersionPath] = Project.Version;
  Data[NamePath] = Project.Name;
  Data[UrlPath] = Project.Url;
  Data[OrganizationPath] = Project.Organization;
  Data[CopyrightPath] = Project.Copyright;
  Data[TitlePath] = Project.DocsTitle;
  Data[LanguagePath] = Project.Language;
  Data[MasterPath] = Project.Master;
  Data[ImagesRootPath] = Project.ImagesRoot;
  Data[ContactMeAtPath] = Project.ContactMeAt;
  Data[ContentsPath] = nlohmann::json::array();

  for (const auto& Entry : Project.Contents) {
    nlohmann::json EntryData;
    EntryData[ContentEntryTitlePath] = Entry.Title;
    EntryData[ContentEntryPagesPath] = nlohmann::json::array();

    for (const auto& Page : Entry.Pages) {
      nlohmann::json PageData;
      if (Page.Title) {
        PageData[PageTitlePath] = *Page.Title;
      }
      if (Page.IsExternal) {
        PageData[PageIsExternalPath] = *Page.IsExternal;
      }
      if (Page.Address) {
        PageData[PagePagePath] = *Page.Address;
      }
      if (Page.Type_) {
        auto It = TypeToString.find(*Page.Type_);
        if (It != TypeToString.end()) {
          PageData[PageApiPath] = It->second;
        }
      }
      if (Page.IsPinned) {
        PageData[PageIsPinnedPath] = *Page.IsPinned;
      }

      EntryData[ContentEntryPagesPath].emplace_back(std::move(PageData));
    }

    Data[ContentsPath].emplace_back(std::move(EntryData));
  }

  return Data.dump(JSON_INDENT);
}

udocs_processor::SurrealLoader::SurrealLoader() {
  StringToType.insert(std::make_pair(SURREAL_PAGE_API_CPP,
      SurrealProject::Page::Type::API_CPP));
  StringToType.insert(std::make_pair(SURREAL_PAGE_API_BLUEPRINT,
      SurrealProject::Page::Type::API_BLUEPRINT));

  TypeToString.insert(std::make_pair(SurrealProject::Page::Type::API_CPP,
      SURREAL_PAGE_API_CPP));
  TypeToString.insert(std::make_pair(SurrealProject::Page::Type::API_BLUEPRINT,
      SURREAL_PAGE_API_BLUEPRINT));
}
