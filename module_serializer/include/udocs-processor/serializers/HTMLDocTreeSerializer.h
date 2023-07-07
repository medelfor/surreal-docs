/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/spdlog.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
#include <inja/environment.hpp>
#include "DynamicPageData.h"
#include "udocs-processor/serializers/markdown/SurrealMarkdownToHtmlConverter.h"

namespace udocs_processor {
class ProcessingReport;

// isn't supposed to be used twice
class HTMLDocTreeSerializer {
 public:
  explicit HTMLDocTreeSerializer(std::shared_ptr<spdlog::sinks::sink> Sink);

  std::string Serialize(
      nlohmann::json Page, nlohmann::json Index,
      const DynamicPageData &DynamicData);

  void SetIsTrial(bool IsTrial);

  void SetIsLocalBuild(bool IsLocalBuild);

  const std::string &GetResourcesPath() const;

  void SetResourcesPath(std::string ResourcesPath);

  void SetPageTemplatePath(std::string PageTemplatePath);

  void SetTreeTemplatePath(std::string TreeTemplatePath);

  void SetTimeAgoTemplatePath(std::string TimeAgoTemplatePath);

  void SetUrlPrefix(std::string UrlPrefix);

  void SetSurrealMarkdownToHtmlConverter(
      std::unique_ptr<SurrealMarkdownToHtmlConverter>
          SurrealMarkdownToHtmlConverter);

  void SetIsInternalUrlDecider(
      std::unique_ptr<IsLinkInternalDecider> IsUrlInternalDecider);

  void ExtendWithLink(std::string Link);

  void ExtendWithPreFooter(std::string PreFooter);

  void ExtendWithAfterBody(std::string AfterBody);

  void ExtendWithPreBody(std::string PreBody);

  void ExtendWithAfterHead(std::string AfterHead);

 private:
  using InjaArgsStripped = std::vector<const nlohmann::json*>&;
  using InjaArgs = const InjaArgsStripped;

  void ResetExtension();

  nlohmann::json FunctionResource(InjaArgs Args) const;

  nlohmann::json FunctionUniversalUrl(InjaArgs Args) const;

  nlohmann::json FunctionDocumentUrl(InjaArgs Args) const;

  nlohmann::json FunctionDocumentUrlVersion(InjaArgs Args) const;

  nlohmann::json FunctionIsInternal(InjaArgs Args) const;

  nlohmann::json FunctionMarkdown(InjaArgs Args) const;

  nlohmann::json FunctionLinks(InjaArgs Args) const;

  std::string MakeResourceUrl(const std::string& Resource) const;

  std::string UniversalMakeUrl(const std::string& Url) const;

  std::string MakeDocumentUrl(const std::string& Page) const;

  std::string MakeDocumentUrl(const std::string& Page,
      const std::string& Version) const;

  std::string FormatAgo(size_t Timestamp);

  bool FunctionIsInternal(const std::string& Url) const;

  nlohmann::json BuildTrees(const nlohmann::json& Trees,
      const std::string& CurrentPage);

  std::string SerializeSubTree(const nlohmann::json& Tree, size_t Level,
      size_t& Id, bool& DoesSubtreeContainCurrent,
      const std::string& CurrentPage);

  std::string ProcessContent(const std::string& Content) const;

  std::string ProcessLinks(const std::string& Content) const;

  std::string BuildDocsRootUrl(const std::string& Language,
      const std::string& Version, const std::string& Project,
      const std::string& Instance) const;

  std::string BuildLocalUrl(const std::string& Page) const;

  constexpr static char HTML_SERIALIZER_LOGGER_NAME[] = "html-serializer";
  constexpr static char DIRECTORY_SEPARATOR = '/';

  constexpr static char PAGE_DATA_PREFIX[] = "p";
  constexpr static char INDEX_DATA_PREFIX[] = "i";
  constexpr static char EXTENSION_DATA_PREFIX[] = "e";
  constexpr static char DYNAMIC_DATA_PREFIX[] = "d";
  constexpr static char THEME_ANNOUNCEMENT[] = "theme_announcement";
  constexpr static char SURREAL_DOCS_VERSION[] = "surreal_docs_version";
  constexpr static char LATEST_VERSION[] = "latest_version";
  constexpr static char CANONICAL[] = "canonical";
  constexpr static char PREVIOUS_PAGE[] = "prev";
  constexpr static char INSTANCE[] = "instance";
  constexpr static char PROJECT[] = "project";
  constexpr static char IS_LOCAL[] = "is_local";
  constexpr static char NEXT_PAGE[] = "next";
  constexpr static char PAGE_LINK[] = "link";
  constexpr static char PAGE_TITLE[] = "title";
  constexpr static char AVAILABLE_VERSIONS[] = "available_versions";
  constexpr static char SIDEBAR_TREES[] = "sidebar_trees";
  constexpr static char LAST_UPDATED[] = "last_updated";
  constexpr static char INDEX_TREES[] = "trees";
  constexpr static char INDEX_VERSION[] = "current_version";
  constexpr static char INDEX_LANGUAGE[] = "language";
  constexpr static char INDEX_IMAGES_ROOT[] = "images_root";

  constexpr static char EXTENSION_LINKS[] = "links";
  constexpr static char EXTENSION_AFTERBODY[] = "afterbody";
  constexpr static char EXTENSION_AFTERHEAD[] = "afterhead";
  constexpr static char EXTENSION_PREBODY[] = "prebody";
  constexpr static char EXTENSION_PREFOOTER[] = "prefooter";

  constexpr static char FUNCTION_RESOURCE_NAME[] = "resource";
  constexpr static int FUNCTION_RESOURCE_ARGS_COUNT = 1;

  constexpr static char FUNCTION_UNIVERSAL_URL_NAME[] = "uniurl";
  constexpr static int FUNCTION_UNIVERSAL_URL_ARGS_COUNT = 1;

  constexpr static char FUNCTION_DOCUMENT_URL_NAME[] = "docurl";
  constexpr static int FUNCTION_DOCUMENT_URL_ARGS_COUNT = 1;

  constexpr static char FUNCTION_DOCUMENT_URL_VERSION_NAME[] = "docurl_ver";
  constexpr static int FUNCTION_DOCUMENT_URL_VERSION_ARGS_COUNT = 2;

  constexpr static char FUNCTION_IS_INTERNAL_NAME[] = "is_internal";
  constexpr static int FUNCTION_IS_INTERNAL_ARGS_COUNT = 1;

  constexpr static char FUNCTION_MARKDOWN_NAME[] = "markdown";
  constexpr static int FUNCTION_MARKDOWN_ARGS_COUNT = 1;

  constexpr static char FUNCTION_LINKS_NAME[] = "links";
  constexpr static int FUNCTION_LINKS_ARGS_COUNT = 1;

  constexpr static char PROTOCOL_MARKER[] = "://";
  constexpr static char URL_SEPARATOR[] = "/";

  constexpr static char TIME_LOCAL[] = "local";
  constexpr static char TIME_YEARS[] = "years";
  constexpr static char TIME_MONTHS[] = "months";
  constexpr static char TIME_WEEKS[] = "weeks";
  constexpr static char TIME_DAYS[] = "days";
  constexpr static char TIME_DAY[] = "day";
  constexpr static char TIME_MONTH[] = "month";
  constexpr static char TIME_YEAR[] = "year";

  constexpr static int MONTHS_IN_YEAR = 12;
  constexpr static int WEEKS_PER_MONTH = 4;
  constexpr static int DAYS_IN_WEEK = 7;
  constexpr static int HOURS_IN_DAY = 24;

  // from json
  constexpr static char INDEX_TREE_TITLE[] = "title";
  constexpr static char INDEX_TREE_CHILDREN[] = "children";

  constexpr static char INDEX_SUBTREE_TITLE[] = "title";
  constexpr static char INDEX_SUBTREE_CHILDREN[] = "children";
  constexpr static char INDEX_SUBTREE_URL[] = "url";
  constexpr static char INDEX_TREE_IS_PINNED[] = "is_pinned";
  constexpr static char INDEX_TREE_IS_CANONICAL[] = "is_canonical";
  constexpr static char INDEX_TREE_IS_EMPTY[] = "is_empty";

  // for templates
  constexpr static char PAGE_TREE_TITLE[] = "title";
  constexpr static char PAGE_TREE_BODY[] = "body";
  constexpr static char PAGE_TREE_IS_CURRENT[] = "is_current";

  constexpr static char TREE_TITLE[] = "title";
  constexpr static char TREE_BODY[] = "body";
  constexpr static char TREE_LEVEL[] = "level";
  constexpr static char TREE_URL[] = "url";
  constexpr static char TREE_ID[] = "id";
  constexpr static char TREE_HAS_CHILDREN[] = "has_children";
  constexpr static char TREE_IS_CURRENT[] = "is_current";  // is current subtree
  constexpr static char TREE_IS_CURRENT_PAGE[] = "is_current_page";
  constexpr static char TREE_IS_PINNED[] = "is_pinned";
  constexpr static char TREE_IS_EMPTY[] = "is_empty";

  constexpr static char PAGE_PAGENAME[] = "docpath";

  constexpr static char PREFIX_API_BP[] = "api/blueprint";
  constexpr static char PREFIX_API_CPP[] = "api/cpp";
  constexpr static char PREFIX_DOCS[] = "docs";

  bool mIsTrial = false;
  bool IsLocalBuild = false;

  inja::Environment Environment{};
  std::string ResourcesPath;
  std::string PageTemplatePath;
  std::string TreeTemplatePath;
  std::string TimeAgoTemplatePath;

  inja::Template TreeTemplate;
  inja::Template TimeAgoTemplate;

  // like https://surrealdocs.com/
  // not like https://surrealdocs.com/instance/project/1.0.0/en-us
  std::string UrlPrefix;
  std::string DocsRootUrl;
  std::string ImagesRoot;
  std::string Language;
  std::string Version;
  std::string Instance;
  std::string Project;

  nlohmann::json Extension;

  std::shared_ptr<spdlog::logger> l;
  std::unique_ptr<SurrealMarkdownToHtmlConverter> SurrealMarkdownProcessor;
  std::unique_ptr<IsLinkInternalDecider> IsUrlInternalDecider;
};
}  // namespace udocs_processor

