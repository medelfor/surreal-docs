/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/spdlog.h>
#include <md4c.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <vector>
#include <utility>
#include <nlohmann/json.hpp>
#include "DocTreeSerializer.h"
#include "udocs-processor/doctree/DocNodeVisitor.h"
#include "UFunctionSerializer.h"
#include "udocs-processor/serializers/type/UTypeToImagesSerializer.h"
#include "udocs-processor/serializers/type/UTypeToStringSerializer.h"
#include "udocs-processor/serializers/markdown/SurrealMarkdown.h"
#include "udocs-processor/FileDataLoader.h"
#include "udocs-processor/doctree/native/LogCategoryNode.h"
#include "udocs-processor/doctree/docs/DocsPageNode.h"
#include "udocs-processor/DocPathsCache.h"
#include "udocs-processor/doctree/native/NativeClassNode.h"
#include "udocs-processor/doctree/native/NativeFunctionNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintClassNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintFunctionNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintPropertyNode.h"
#include "udocs-processor/serializers/IntegrityCheck.h"

namespace udocs_processor {
class ProcessingReport;
class HasCounterpart;
class BlueprintableNode;

class ManifestEntry {
 public:
  enum class Type {
    PAGE,
    RESOURCE,
    INDEX,
    DEBUG
  };

  enum class Format {
    JSON_V1,  // pages
    INDEX_JSON_V1,  // index
    DEBUG_JSON_V1,  // debug page
    PNG_BASE64,  // png resources for db
    PNG  // png resources for direct download
  };

  ManifestEntry(
      std::string FileName, std::string DocPath,
      Type NewType, Format NewFormat);

  const std::string &GetFileName() const;

  const std::string &GetDocPath() const;

  Type GetType() const;

  Format GetFormat() const;

 private:
  std::string FileName;
  std::string DocPath;
  Type mType = Type::PAGE;
  Format mFormat = Format::JSON_V1;
};

namespace extract_description {
int EnteredBlock(MD_BLOCKTYPE Type, void* Detail, void* Userdata);

int ExitedBlock(MD_BLOCKTYPE Type, void* Detail, void* Userdata);

int EnteredSpan(MD_SPANTYPE Type, void* Detail, void* Userdata);

int ExitedSpan(MD_SPANTYPE Type, void* Detail, void* Userdata);

int Text(MD_TEXTTYPE Type, const MD_CHAR* Text, MD_SIZE Size, void* Userdata);
}  // namespace extract_description

// exported title and name/display name should match
class JSONDocTreeSerializer : public DocTreeSerializer, public DocNodeVisitor {
  friend class Serializer;
  friend class DescriptionExtractor;
  friend class DocPathsGenerator;

 public:
  enum class Status {
    SERIALIZING_JSON,
    SERIALIZING_IMAGES
  };

  enum class Verbosity {
    FULL,
    MODULE_PREVIEW,
    CLASS_PREVIEW,
    INDEX_PREVIEW  // only for modules for index for ExtractBaseInfo()
  };

  class DescriptionExtractor {
   public:
    friend int extract_description::EnteredBlock(
        MD_BLOCKTYPE Type, void *Detail, void *Userdata);
    friend int extract_description::ExitedBlock(
        MD_BLOCKTYPE Type, void *Detail, void *Userdata);
    friend int extract_description::EnteredSpan(
        MD_SPANTYPE Type, void *Detail, void *Userdata);
    friend int extract_description::ExitedSpan(
        MD_SPANTYPE Type, void *Detail, void *Userdata);
    friend int extract_description::Text(
        MD_TEXTTYPE Type, const MD_CHAR *Text, MD_SIZE Size, void *Userdata);

    void Parse(const std::string &Markdown);

    std::string & GetDescription();

   private:
    constexpr static char SPACE = ' ';

    bool IsInP = false;
    bool GatheredP = false;
    std::string Description;
  };

  class BpInitializerExporter : public DocNodeVisitor {
   public:
    explicit BpInitializerExporter(JSONDocTreeSerializer &Serializer);

    std::string MakeInitializer(BlueprintUNode& Node, bool IsParameter);

   private:
    int Visit(BlueprintArrayTypeNode &Node) override;

    int Visit(BlueprintMapTypeNode &Node) override;

    int Visit(BlueprintSetTypeNode &Node) override;

    int Visit(BlueprintEnumTypeNode &Node) override;

    int Visit(BlueprintObjectTypeNode &Node) override;

    int Visit(BlueprintPrimitiveTypeNode &Node) override;

    int Visit(BlueprintStructTypeNode &Node) override;

    void ExportBaseInitializer();

    constexpr static char VECTOR_FORMAT[] = "{}, {}, {}";
    constexpr static char ROTATOR_FORMAT[] = "{}° roll, {}° pitch, {}° yaw";

    JSONDocTreeSerializer& Serializer;
    std::string FormattedInitializer;
    BlueprintTypeNode* Type = nullptr;
    BlueprintUNode* UNode = nullptr;
    bool IsParameter = false;
  };

  class TitleAndBreadcrumbsGenerator : public DocNodeVisitor {
   public:
    explicit TitleAndBreadcrumbsGenerator(JSONDocTreeSerializer &Serializer);

    int Visit(DelegateTypeNode &Node) override;

    int Visit(LogCategoryNode &Node) override;

    int Visit(NativeClassNode &Node) override;

    int Visit(NativeEnumNode &Node) override;

    int Visit(NativeFunctionNode &Node) override;

    int Visit(NativeModuleNode &Node) override;

    int Visit(NativePropertyNode &Node) override;

    int Visit(BlueprintClassNode &Node) override;

    int Visit(BlueprintEnumNode &Node) override;

    int Visit(BlueprintFunctionNode &Node) override;

    int Visit(BlueprintModuleNode &Node) override;

    int Visit(BlueprintStructNode &Node) override;

    int Visit(BlueprintDelegateTypeNode &Node) override;

    int Visit(DocsPageNode &Node) override;

   private:
    void ExportTitle(const DocsPageNode& Node);

    void ExportTitle(const BlueprintUNode& Node);

    void ExportTitle(const NativeMemberNode& Node);

    void ExportTitle(std::string Title, const DocNode& Node);

    JSONDocTreeSerializer& Serializer;
  };

  explicit JSONDocTreeSerializer(
      std::unique_ptr<UFunctionSerializer> FunctionSerializer,
      std::unique_ptr<UTypeToImagesSerializer> TypeToImagesSerializer,
      std::unique_ptr<UTypeToStringSerializer> TypeToStringSerializer,
      std::unique_ptr<FileDataLoader> DataLoader,
      std::unique_ptr<DocPathsCache> PathsCache,
      std::unique_ptr<IntegrityCheck> Checker);

  void Serialize(DocNode &Root) override;

  void SetOutputDirectory(const std::string& Directory);

  void SetIsTrial(bool IsTrial);

  const std::vector<ManifestEntry> &GetManifest() const;

  void SetDoBase64EncodeImages(bool DoBase64EncodeImages);

  void SetPrettyPrint(bool DoPrettyPrint);

  void SetLogger(std::shared_ptr<spdlog::logger> Logger) const override;

  void SetHtml2PngPath(std::string Path);

  void SetStatusReporter(std::function<void(Status)> ReportCallback);

 private:
  struct ImageToProcess {
    ImageToProcess(
        std::string DocPath, std::string FileReference, std::string FileName)
        : DocPath(std::move(DocPath)),
        FileReference(std::move(FileReference)),
        FileName(std::move(FileName)) {}

    std::string DocPath;
    std::string FileReference;
    std::string FileName;  // may be omitted for user provided images
  };

  int Visit(DelegateTypeNode &Node) override;

  int Visit(LogCategoryNode &Node) override;

  int Visit(NativeClassNode &Node) override;

  int Visit(NativeEnumNode &Node) override;

  int Visit(NativeFunctionNode &Node) override;

  int Visit(NativeModuleNode &Node) override;

  int Visit(NativePropertyNode &Node) override;

  int Visit(BlueprintFunctionNode &Node) override;

  int Visit(BlueprintClassNode &Node) override;

  int Visit(BlueprintEnumNode &Node) override;

  int Visit(BlueprintModuleNode &Node) override;

  int Visit(BlueprintStructNode &Node) override;

  int Visit(BlueprintDelegateTypeNode &Node) override;

  int Visit(DocsImageNode &Node) override;

  int Visit(DocsIndexNode &Node) override;

  int Visit(DocsPageNode &Node) override;

  // returns array
  nlohmann::json SerializeTrees(DocsIndexNode& Index);

  const std::string & ObtainDocPath(DocNode &Node) const;

  std::vector<std::string> SerializeTypeImages(BlueprintUNode& Node);

  BlueprintTypeNode* FindType(BlueprintUNode& Node) const;

  void FinalizeImages();

  void ReleaseResources();

  void MakeIndexes();

  void MakeDebug(DocNode& Root);

  void ExportEssentialInfo(const DocNode& Node, nlohmann::json& Page) const;

  void ExportDocPath(const DocNode& Node, nlohmann::json& Json) const;

  void ExportBasePageInfo(const DocNode& Node, nlohmann::json& Page) const;

  void ExportDescriptions(const DocNode& Node, nlohmann::json& Json,
      Verbosity Verb = Verbosity::FULL) const;

  void ExportDeprecationInfo(const DocNode& Node, nlohmann::json& Json) const;

  void ExportBaseInfo(const DocNode& Node, nlohmann::json& Json,
      Verbosity Verb = Verbosity::FULL, bool DoExtractDesc = true,
      bool DoExportDocPath = true) const;

  void ExportBaseInfo(const NativeMemberNode& Node,
      nlohmann::json& Page, Verbosity Verb = Verbosity::FULL,
      bool DoExtractDesc = true, bool DoExportDocPath = true) const;

  void ExportBaseInfo(const BlueprintUNode& Node, nlohmann::json& Page,
      Verbosity Verb = Verbosity::FULL, bool DoExtractDesc = true,
      bool DoExportDocPath = true) const;

  void ExportTemplate(DocNode& Node, nlohmann::json& Page,
      Verbosity Verb = Verbosity::FULL) const;

  // returns array
  nlohmann::json ExportNativeParameters(
      DocNode& Node, Verbosity Verb = Verbosity::FULL) const;

  void ExportCounterpart(const HasCounterpart& Node,
      nlohmann::json& Page) const;

  void ExportSee(DocNode& Node, nlohmann::json& Page,
      bool AsString = false) const;

  void ExportUeInfo(DocNode& Node, nlohmann::json& Page,
      Verbosity Verb = Verbosity::FULL) const;

  nlohmann::json ExportBlueprintType(BlueprintUNode& Node,
      bool& IsDelegateType);

  void ExportIsUEntity(BlueprintableNode& Node, nlohmann::json& Json) const;

  template<typename T>
  std::string ExportReferences(DocNode& Node, bool DoFormat = true) const;

  void ExportLogCategory(LogCategoryNode& Node, nlohmann::json& Page,
      Verbosity Verb = Verbosity::FULL) const;

  bool ExportProperty(BlueprintPropertyNode& Node,
      nlohmann::json& Json, Verbosity Verb = Verbosity::FULL);

  void ExportProperties(BlueprintUNode& Node,
      nlohmann::json& Json, Verbosity Verb = Verbosity::FULL);

  void ExportProperty(NativePropertyNode& Node, nlohmann::json& Json,
      Verbosity Verb = Verbosity::FULL) const;

  void ExportDelegate(DelegateTypeNode& Node, nlohmann::json& Json,
      Verbosity Verb = Verbosity::FULL) const;

  void ExportDelegate(BlueprintDelegateTypeNode& Node, nlohmann::json& Json,
      Verbosity Verb = Verbosity::FULL);

  void ExportStruct(BlueprintStructNode& Node, nlohmann::json& Json,
      Verbosity Verb = Verbosity::FULL);

  void ExportClass(NativeClassNode& Node, nlohmann::json& Json,
      Verbosity Verb = Verbosity::FULL) const;

  void SortByNameGroupedByAccess(
      std::map<AccessModifier, nlohmann::json>& Map) const;

  void AddInStandardLayout(nlohmann::json& Target,
      std::map<AccessModifier, nlohmann::json>& Map) const;

  struct Function {
    nlohmann::json Json;
    NativeFunctionNode* Func;
  };

  struct Complementary {
    nlohmann::json Func;
    AccessModifier Access;
  };

  void SortByNameGroupedByAccess(
      std::map<AccessModifier, std::vector<Function>>& Map) const;

  void AddInStandardLayout(nlohmann::json& Target,
      std::map<AccessModifier, std::vector<Function>>& Map,
      std::unordered_map<NativeFunctionNode*,
        Complementary>& Complements) const;

  nlohmann::json ExportClassParents(NativeClassNode& Node,
      Verbosity Verb = Verbosity::FULL) const;

  void ExportClass(BlueprintClassNode& Node, nlohmann::json& Json,
      Verbosity Verb = Verbosity::FULL);

  void ExportEnum(NativeEnumNode& Node, nlohmann::json& Json,
      Verbosity Verb = Verbosity::FULL) const;

  void ReExportUeLegacyEnum(NativeEnumNode& Node, nlohmann::json& Json) const;

  void ExportEnum(BlueprintEnumNode& Node, nlohmann::json& Json,
      Verbosity Verb = Verbosity::FULL) const;

  void ExportFunction(NativeFunctionNode& Node, nlohmann::json& Json,
      Verbosity Verb = Verbosity::FULL) const;

  void ExportFunction(BlueprintFunctionNode& Node, nlohmann::json& Json,
      Verbosity Verb = Verbosity::FULL);

  void ExportTitle(DocNode& Node, nlohmann::json& Json);

  void ExportBreadcrumbs(DocNode& Node, nlohmann::json& Json,
      bool SkipTillModule = false);

  void AddContent(std::string FilePath, std::string Content);

  void FlushContent();

  // adds page to manifest as well
  void WritePage(const std::string &DocPath, nlohmann::json &Json,
    ManifestEntry::Format Format,
    ManifestEntry::Type Type = ManifestEntry::Type::PAGE);

  // adds image to manifest as well
  void WriteImage(const std::string &DocPath, const std::string &FilePath,
      ManifestEntry::Format TargetFormat, ManifestEntry::Format SourceFormat);

  void WriteToFile(std::string_view Content, const std::string& Path) const;

  std::string Wd(const std::string& Directory);

  std::string ExtractShortDescription(const std::string& ShortDescription,
      const std::string& FullDescription) const;

  std::string ExtractShortDescription(const DocNode &Node) const;

  // replaces refs with nodes being referenced
  std::string ProcessText(const std::string& Text) const;

  // replaces refs with their text
  std::string ProcessTextAsLiteral(const std::string& Text) const;

  std::string ExportName(const DocNode& Node) const;

  inline std::string MakeSafeName(const std::string& Name) const;

  // escapes html
  static std::string MakeTextSafe(const std::string& Text);

  static std::string MakeUrlSafe(const std::string& Url);

  std::string FormatKeyValue(
      const std::string &Key, const std::string &Value) const;

  std::string GlueRefWords(DocNode& Node, const std::string& Glue,
      bool HumanReadable = true, bool UseSmartGlue = false) const;

  std::string SmartGlue(
      const std::string& Current, const std::string& ToAdd) const;

  std::string GetHumanReadableName(DocNode& Node) const;

  std::string MakeLinkWord(DocNode& Node) const;

  std::string MakeLinkWord(DocNode& Node, const std::string& Text) const;

  template<typename T>
  std::string MakeLinkWordFromNode(T& Node) const;

  // warning any string returned by the method is invalidated when the method
  // is called again
  std::string EncodeBase64(const char* Data, size_t Size);

  void SortByNamespaceAndName(nlohmann::json& List) const;

  void SortByName(nlohmann::json& List) const;

  void SortByDisplayNameAndAccess(nlohmann::json& List) const;

  void SortByDisplayName(nlohmann::json& List) const;

  // returns array
  nlohmann::json SerializeTree(DocsGroupNode &Page);

  // returns object
  nlohmann::json SerializeTree(
      DocsPageNode &Page, bool &DoExpand);

  static void AddTreeChildren(nlohmann::json& Tree, bool DoExpand,
      nlohmann::json& Target);

  std::string DocPathOfGeneratedImage(BlueprintFunctionNode &Node) const;

  constexpr static char DOCPATH_API_BLUEPRINT[] = "api/blueprint";
  constexpr static char DOCPATH_API_CPP[] = "api/cpp";

  constexpr static char IMAGES_SUBDIR[] = "images/";
  constexpr static char PAGES_SUBDIR[] = "pages/";
  constexpr static char CONVERSION_SUBDIR[] = "converted/";
  constexpr static char GENERATED_IMAGES_SUBDIR[] = "__generated/";
  constexpr static char GENERATED_TYPE_IMAGES_SUBDIR[] = "types/";
  constexpr static char GENERATED_FUNCTIONS_IMAGES_SUBDIR[] = "functions/";
  constexpr static char DIRECTORY_SEPARATOR = '/';
  constexpr static char PNG_RESOURCE_EXTENSION[] = ".png";

  constexpr static char PAGE_VERSION[] = "file_version";
  constexpr static char PAGE_TYPE[] = "type";
  constexpr static char PAGE_TYPE_PAGE[] = "page";
  constexpr static char PAGE_TYPE_INDEX[] = "index";
  constexpr static char PAGE_TYPE_NATIVE_CLASS[] = "native_class";
  constexpr static char PAGE_TYPE_BLUEPRINT_CLASS[] = "blueprint_class";
  constexpr static char PAGE_TYPE_BLUEPRINT_STRUCT[] = "blueprint_struct";
  constexpr static char PAGE_TYPE_NATIVE_FUNCTION[] = "native_function";
  constexpr static char PAGE_TYPE_BLUEPRINT_FUNCTION[] = "blueprint_function";
  constexpr static char PAGE_TYPE_NATIVE_ENUM[] = "native_enum";
  constexpr static char PAGE_TYPE_BLUEPRINT_ENUM[] = "blueprint_enum";
  constexpr static char PAGE_TYPE_NATIVE_MODULE[] = "native_module";
  constexpr static char PAGE_TYPE_BLUEPRINT_MODULE[] = "blueprint_module";
  constexpr static char PAGE_TYPE_NATIVE_INDEX[] = "native_index";
  constexpr static char PAGE_TYPE_BLUEPRINT_INDEX[] = "blueprint_index";
  constexpr static char PAGE_TYPE_NATIVE_DELEGATE[] = "native_delegate";
  constexpr static char PAGE_TYPE_BLUEPRINT_DELEGATE[] = "blueprint_delegate";
  constexpr static char PAGE_TYPE_NATIVE_LOG_CATEGORY[] = "native_log_category";
  constexpr static char PAGE_TYPE_NATIVE_VARIABLE[] = "native_variable";
  constexpr static char PAGE_TITLE[] = "title";
  constexpr static char PAGE_BREADCRUMBS[] = "breadcrumbs";

  constexpr static char DEBUG_IDS[] = "ids";
  constexpr static char DEBUG_ID_ID[] = "id";
  constexpr static char DEBUG_ID_DESCRIPTION[] = "description";
  constexpr static char DEBUG_DOCPATH[] = "debug";

  constexpr static char PAGE_BODY[] = "body";

  constexpr static char INDEX_LANGUAGE[] = "language";
  constexpr static char INDEX_DOCS_TITLE[] = "docstitle";
  constexpr static char INDEX_MASTER_DOC[] = "master_doc";
  constexpr static char INDEX_PROJECT_NAME[] = "project_name";
  constexpr static char INDEX_COPYRIGHT[] = "copyright";
  constexpr static char INDEX_COPYRIGHT_URL[] = "copyright_url";
  constexpr static char INDEX_CURRENT_VERSION[] = "current_version";
  constexpr static char INDEX_WEBSITE[] = "website";
  constexpr static char INDEX_LOGO_URL[] = "logo_url";
  constexpr static char INDEX_VERSION_TAG[] = "version_tag";
  constexpr static char INDEX_ORGANIZATION[] = "organization";
  constexpr static char INDEX_IMAGES_ROOT[] = "images_root";
  constexpr static char INDEX_TREES[] = "trees";
  constexpr static char INDEX_DOCPATH[] = "index";

  constexpr static char TREES_GROUP_TITLE[] = "title";
  constexpr static char TREES_GROUP_CHILDREN[] = "children";

  constexpr static char TREE_PAGE_TITLE[] = "title";
  constexpr static char TREE_PAGE_URL[] = "url";
  constexpr static char TREE_PAGE_IS_CANONICAL[] = "is_canonical";
  constexpr static char TREE_PAGE_IS_PINNED[] = "is_pinned";
  constexpr static char TREE_PAGE_IS_EMPTY[] = "is_empty";
  constexpr static char TREE_PAGE_CHILDREN[] = "children";

  constexpr static char NODE_NAME[] = "name";
  constexpr static char NODE_SINCE[] = "since";
  constexpr static char NODE_SHORT_DESCRIPTION[] = "short_desc";
  constexpr static char NODE_FULL_DESCRIPTION[] = "full_desc";
  constexpr static char NODE_DESCRIPTION[] = "description";
  constexpr static char NODE_IS_DEPRECATED[] = "is_deprecated";
  constexpr static char NODE_DEPRECATION_VERSION[] = "deprecation_version";
  constexpr static char NODE_DEPRECATION_MESSAGE[] = "deprecation_message";
  constexpr static char NODE_PATH[] = "path";  // header or content-path
  constexpr static char NODE_NATIVE_API[] = "api";
  constexpr static char NODE_NATIVE_INCLUDE[] = "include";
  constexpr static char NODE_NATIVE_NAMESPACE[] = "namespace";
  constexpr static char NODE_BLUEPRINT_DISPLAY_NAME[] = "display_name";
  constexpr static char NODE_BLUEPRINT_CATEGORY[] = "category";
  constexpr static char NODE_BLUEPRINT_IS_NATIVE_DEFINED[] = "is_native";
  constexpr static char NODE_SEE[] = "see";
  constexpr static char DOCPATH[] = "docpath";
  constexpr static char NODE_COUNTERPART[] = "counterpart";
  constexpr static char NODE_ACCESS[] = "access";
  constexpr static char NODE_UE_INFO[] = "ue_info";
  constexpr static char NODE_UE_INFO_META[] = "meta";
  constexpr static char NODE_UE_INFO_NORMAL[] = "normal";
  constexpr static char NODE_UE_INFO_MACRO[] = "macro";
  constexpr static char NODE_TEMPLATE[] = "template";
  constexpr static char NODE_IS_INTERNAL[] = "is_internal";
  constexpr static char UE_INFO_MACRO_NAME[] = "macro";
  constexpr static char PARAM_TYPE[] = "type";
  constexpr static char PARAM_DEFAULT[] = "default";

  constexpr static char HMTL2PNG_LIST_GLUE[] = ", ";
  constexpr static char HMTL2PNG_EXECUTOR[] = "node";
  constexpr static char HMTL2PNG_INPUT[] = "--input=";
  constexpr static char HMTL2PNG_OUTPUT[] = "--output=";
  constexpr static char HMTL2PNG_LIST[] = "--ids=";

  constexpr static char PAGE_EXTENSION[] = ".json";

  constexpr static char KEY_VALUE_FORMAT[] = R"({} = "{}")";

  constexpr static char SPACE[] = " ";
  constexpr static char JOIN_SEE_BY[] = ", ";
  constexpr static char SOFTBREAK[] = "&#8302;";
  constexpr static char JOIN_UE_INFO_BY[] = ", ";
  constexpr static char JOIN_PARAMS_BY[] = ", ";
  constexpr static char TYPE_AND_PARAM_GLUE[] = " ";
  constexpr static char PARAM_DEF_VALUE_FORMAT[] = "{} = {}";
  constexpr static char SMART_GLUE[] = " ";
  constexpr static char TEMPLATE_OPEN = '<';
  constexpr static char TEMPLATE_CLOSE = '>';
  constexpr static char REFERENCE = '&';
  constexpr static char POINTER = '*';
  constexpr static char COMMA = ',';
  constexpr static char DOUBLEQUOTE = '"';
  constexpr static char NEWLINE = '\n';
  constexpr static char PAGE_NAME_SEPARATOR = '_';

  constexpr static int APPROXIMATE_LENGTH_OF_GENERATED_IMAGE_NAME = 12;

  constexpr static int INDEX_JSON_VERSION = 1;
  constexpr static int PAGE_JSON_VERSION = 1;
  constexpr static int DEBUG_JSON_VERSION = 1;
  // 2mb
  constexpr static uint64_t FLUSH_CONTENT_IF_MORE_THAN = 2 * 1024 * 1024;

  constexpr static char INLINE_REFERENCE_PART1[] = R"(<ref refid=")";
  constexpr static char INLINE_REFERENCE_PART2[] = R"(">)";
  constexpr static char INLINE_REFERENCE_PART3[] = R"(</ref>)";

  constexpr static char NATIVE_MODULE_DELEGATES[] = "delegate_previews";
  constexpr static char NATIVE_MODULE_LOG_CATEGORIES[] =
      "log_category_previews";
  constexpr static char NATIVE_MODULE_CLASSES[] = "class_previews";
  constexpr static char NATIVE_MODULE_STRUCTS[] = "struct_previews";
  constexpr static char NATIVE_MODULE_INTERFACES[] = "interface_previews";
  constexpr static char NATIVE_MODULE_FUNCTIONS[] = "function_previews";
  constexpr static char NATIVE_MODULE_VARIABLES[] = "variable_previews";
  constexpr static char NATIVE_MODULE_ENUMS[] = "enum_previews";

  constexpr static char NAMESPACE_SEPARATOR[] = "::";

  constexpr static char NATIVE_NAMESPACE[] = "namespace";

  constexpr static char NATIVE_CLASS_CLASS_TYPE[] = "class_type";
  constexpr static char NATIVE_CLASS_IS_FINAL[] = "is_final";
  constexpr static char NATIVE_CLASS_UIS_FINAL[] = "uis_final";
  constexpr static char NATIVE_CLASS_PARENTS[] = "parents";
  constexpr static char NATIVE_CLASS_UPARENTS[] = "uparents";
  constexpr static char NATIVE_CLASS_UNAME[] = "uname";
  constexpr static char NATIVE_CLASS_PARENT_ACCESS[] = "access";
  constexpr static char NATIVE_CLASS_PARENT_IS_VIRTUAL[] = "is_virtual";
  constexpr static char NATIVE_CLASS_PARENT_PARENT[] = "parent";
  constexpr static char NATIVE_CLASS_DELEGATES[] = "delegate_previews";
  constexpr static char NATIVE_CLASS_CLASSES[] = "class_previews";
  constexpr static char NATIVE_CLASS_STRUCTS[] = "struct_previews";
  constexpr static char NATIVE_CLASS_FUNCTIONS[] = "function_previews";
  constexpr static char NATIVE_CLASS_VARIABLES[] = "variable_previews";
  constexpr static char NATIVE_CLASS_ENUMS[] = "enum_previews";

  constexpr static char BLUEPRINT_PROPERTIES[] = "property_previews";
  constexpr static char BLUEPRINT_EVENT_DISPATCHERS[] = "event_disp_previews";
  constexpr static char BLUEPRINT_CLASS_TYPE[] = "class_type";
  constexpr static char BLUEPRINT_CLASS_TYPE_CLASS[] = "class";
  constexpr static char BLUEPRINT_CLASS_TYPE_WIDGET[] = "widget";
  constexpr static char BLUEPRINT_CLASS_TYPE_FUNCTION_LIBRARY[] =
      "function library";
  constexpr static char BLUEPRINT_CLASS_TYPE_COMPONENT[] = "component";
  constexpr static char BLUEPRINT_CLASS_TYPE_ACTOR[] = "actor";
  constexpr static char BLUEPRINT_CLASS_IS_BLUEPRINT_TYPE[] =
      "is_blueprint_type";
  constexpr static char BLUEPRINT_CLASS_IS_BLUEPRINTABLE[] =
      "is_blueprintable";
  constexpr static char BLUEPRINT_CLASS_IS_ABSTRACT[] = "is_abstract";
  constexpr static char BLUEPRINT_CLASS_IS_INTERFACE[] = "is_interface";
  constexpr static char BLUEPRINT_CLASS_CONFIG[] = "config";
  constexpr static char BLUEPRINT_CLASS_NAMESPACE[] = "namespace";
  constexpr static char BLUEPRINT_CLASS_FUNCTIONS[] = "function_previews";
  constexpr static char BLUEPRINT_CLASS_PARENTS[] = "parents";
  constexpr static char BLUEPRINT_CLASS_INTERFACES[] = "interfaces";
  constexpr static char BLUEPRINT_CLASS_PARENT_PARENT[] = "parent";

  constexpr static char BLUEPRINT_ENUM_ELEMENTS[] = "elements";
  constexpr static char BLUEPRINT_ENUM_IS_BLUEPRINT_TYPE[] =
      "is_blueprint_type";
  constexpr static char BLUEPRINT_ENUM_ELEMENT_DESCRIPTION[] =
      "description";

  constexpr static char BLUEPRINT_STRUCT_IS_BLUEPRINT_TYPE[] =
      "is_blueprint_type";

  constexpr static char NATIVE_CLASS_TYPE_STRUCT[] = "struct";
  constexpr static char NATIVE_CLASS_TYPE_INTERFACE[] = "interface";
  constexpr static char NATIVE_CLASS_TYPE_CLASS[] = "class";

  constexpr static char ACCESS_PUBLIC[] = "public";
  constexpr static char ACCESS_PROTECTED[] = "protected";
  constexpr static char ACCESS_PRIVATE[] = "private";

  constexpr static char NATIVE_IS_UENTITY[] = "is_uentity";

  constexpr static char NATIVE_ENUM_IS_STRONG[] = "is_strong";
  constexpr static char NATIVE_ENUM_IS_UE_LEGACY[] = "is_ue_legacy";
  constexpr static char NATIVE_ENUM_TYPE[] = "enum_type";
  constexpr static char NATIVE_ENUM_ELEMENT_DESCRIPTION[] = "description";
  constexpr static char NATIVE_ENUM_ELEMENT_DEFAULT[] = "default";
  constexpr static char NATIVE_ENUM_ELEMENTS[] = "elements";

  constexpr static char NATIVE_PROPERTY_IS_STATIC[] = "is_static";
  constexpr static char NATIVE_PROPERTY_IS_CONST[] = "is_const";
  constexpr static char NATIVE_PROPERTY_IS_MUTABLE[] = "is_mutable";
  constexpr static char NATIVE_PROPERTY_IS_VOLATILE[] = "is_volatile";
  constexpr static char NATIVE_PROPERTY_INITIALIZER[] = "initializer";
  constexpr static char NATIVE_PROPERTY_TYPE[] = "var_type";
  constexpr static char NATIVE_REPLICATION_CONDITION[] =
      "replication_condition";
  constexpr static char NATIVE_REPLICATION_MODE[] = "replication_mode";

  constexpr static char NATIVE_FUNCTION_RET_TYPE[] = "return";
  constexpr static char NATIVE_FUNCTION_IS_INLINE[] = "is_inline";
  constexpr static char NATIVE_FUNCTION_IS_STATIC[] = "is_static";
  constexpr static char NATIVE_FUNCTION_IS_EXPLICIT[] = "is_explicit";
  constexpr static char NATIVE_FUNCTION_IS_VIRTUAL[] = "is_virtual";
  constexpr static char NATIVE_FUNCTION_IS_FINAL[] = "is_final";
  constexpr static char NATIVE_FUNCTION_IS_CONST[] = "is_const";
  constexpr static char NATIVE_FUNCTION_IS_OVERRIDE[] = "is_override";
  constexpr static char NATIVE_FUNCTION_IS_PURE_VIRTUAL[] = "is_pure_virtual";
  constexpr static char NATIVE_FUNCTION_DEFINITION[] = "definition";
  constexpr static char NATIVE_FUNCTION_DEFINITION_DEFAULT[] = "default";
  constexpr static char NATIVE_FUNCTION_DEFINITION_DELETE[] = "delete";
  constexpr static char NATIVE_FUNCTION_DEFINITION_NORMAL[] = "normal";
  constexpr static char NATIVE_FUNCTION_HAS_SUPER[] = "has_super";
  constexpr static char NATIVE_FUNCTION_CLASS_OF_SUPER[] = "super_class";
  constexpr static char NATIVE_FUNCTION_SUPER[] = "super";
  constexpr static char NATIVE_FUNCTION_MAIN[] = "main";
  constexpr static char NATIVE_FUNCTION_RET_DESC[] = "ret_value_desc";
  constexpr static char NATIVE_FUNCTION_VALIDATION[] = "validation";
  constexpr static char NATIVE_FUNCTION_IS_IMPLEMENTATION[] =
      "is_implementation";
  constexpr static char NATIVE_FUNCTION_PARAMS[] = "params";
  constexpr static char NATIVE_FUNCTION_IMPL[] = "impl";
  constexpr static char NATIVE_FUNCTION_HAS_IMPL[] = "has_impl";

  constexpr static char BLUEPRINT_ACCESS[] = "access";
  constexpr static char BLUEPRINT_FUNCTION_RPC[] = "rpc";
  constexpr static char BLUEPRINT_FUNCTION_RPC_NOT_REPLICATED[] =
      "not_replicated";
  constexpr static char BLUEPRINT_FUNCTION_RPC_NET_MULTICAST[] =
      "net_multicast";
  constexpr static char BLUEPRINT_FUNCTION_RPC_SERVER[] = "server";
  constexpr static char BLUEPRINT_FUNCTION_RPC_CLIENT[] = "client";
  constexpr static char BLUEPRINT_FUNCTION_IS_PURE[] = "is_pure";
  constexpr static char BLUEPRINT_FUNCTION_IS_CALL_IN_EDITOR[] =
      "is_call_in_editor";
  constexpr static char BLUEPRINT_FUNCTION_IS_RELIABLE[] = "is_reliable";
  constexpr static char BLUEPRINT_FUNCTION_IS_STATIC[] = "is_static";
  constexpr static char BLUEPRINT_FUNCTION_IS_CONST[] = "is_const";
  constexpr static char BLUEPRINT_FUNCTION_IS_INT_IMPLEMENTATION[] =
      "is_int_implementation";
  constexpr static char BLUEPRINT_FUNCTION_IS_OVERRIDABLE[] =
      "is_overridable";
  constexpr static char BLUEPRINT_FUNCTION_HAS_SUPER[] = "has_super";
  constexpr static char BLUEPRINT_FUNCTION_CLASS_OF_SUPER[] = "super_class";
  constexpr static char BLUEPRINT_FUNCTION_SUPER[] = "super";
  constexpr static char BLUEPRINT_FUNCTION_PARAMETERS[] = "params";
  constexpr static char BLUEPRINT_FUNCTION_HAS_RET[] = "has_ret";
  constexpr static char BLUEPRINT_FUNCTION_HAS_PARAMS[] = "has_params";
  constexpr static char BLUEPRINT_FUNCTION_IMAGE[] = "image";
  constexpr static char BLUEPRINT_FUNCTION_IS_SINGLE_RET_VALUE[] =
      "is_ret_single";

  constexpr static char RETURN_PARAMETER_NAME[] = "returnvalue";

  constexpr static char BLUEPRINT_PARAMETER_NAME[] = "name";
  constexpr static char BLUEPRINT_PARAMETER_DEFAULT_VALUE[] = "default";
  constexpr static char BLUEPRINT_PARAMETER_IS_OUT[] = "is_out";

  constexpr static char BLUEPRINT_PROPERTY_IS_INSTANCE_EDITABLE[] =
      "is_instance_editable";
  constexpr static char BLUEPRINT_PROPERTY_IS_ARCHETYPE_EDITABLE[] =
      "is_archetype_editable";
  constexpr static char BLUEPRINT_PROPERTY_IS_READ_ONLY[] =
      "is_read_only";
  constexpr static char BLUEPRINT_PROPERTY_IS_EXPOSED_ON_SPAWN[] =
      "is_exposed_on_spawn";
  constexpr static char BLUEPRINT_PROPERTY_ACCESS[] = "access";
  constexpr static char BLUEPRINT_PROPERTY_DO_EXPOSE_TO_CINEMATIC[] =
      "is_exposed_to_cinematic";
  constexpr static char BLUEPRINT_PROPERTY_IS_CONFIG[] = "is_config";
  constexpr static char BLUEPRINT_PROPERTY_MIN[] = "min";
  constexpr static char BLUEPRINT_PROPERTY_MAX[] = "max";
  constexpr static char BLUEPRINT_PROPERTY_REPLICATION_MODE[] =
      "replication_mode";
  constexpr static char BLUEPRINT_PROPERTY_REPLICATION_CONDIITION[] =
      "replication_condition";
  constexpr static char BLUEPRINT_PROPERTY_DEFAULT_VALUE[] = "default";
  constexpr static char BLUEPRINT_PROPERTY_IS_BP_ASSIGNABLE[] = "is_assignable";
  constexpr static char BLUEPRINT_PROPERTY_IS_BP_CALLABLE[] = "is_callable";
  constexpr static char BLUEPRINT_PROPERTY_IS_BP_AUTH_ONLY[] = "is_auth_only";

  constexpr static char BLUEPRINT_TYPE[] = "type";
  constexpr static char BLUEPRINT_TYPE_IMAGE[] = "image";
  constexpr static char BLUEPRINT_TYPE_DELEGATE_IMAGE[] = "delegate_image";
  constexpr static char BLUEPRINT_TYPE_IS_ANONYMOUS_DELEGATE[] =
      "is_anon_delegate";
  constexpr static char BLUEPRINT_TYPE_IS_REF[] = "is_ref";
  constexpr static char BLUEPRINT_TYPE_IS_CONST[] = "is_const";
  constexpr static char BLUEPRINT_TYPE_TEXT[] = "text";

  constexpr static char BLUEPRINT_DELEGATE_IS_MULTICAST[] = "is_multicast";

  constexpr static char NATIVE_DELEGATE_IS_EVENT[] = "is_event";
  constexpr static char NATIVE_DELEGATE_IS_DERIVED_EVENT[] =
      "is_derived_event";
  constexpr static char NATIVE_DELEGATE_IS_MULTICAST[] = "is_multicast";
  constexpr static char NATIVE_DELEGATE_IS_DYNAMIC[] = "is_dynamic";
  constexpr static char NATIVE_DELEGATE_HAS_RET_VALUE[] = "has_ret_value";
  constexpr static char NATIVE_DELEGATE_RET_TYPE[] = "return";
  constexpr static char NATIVE_DELEGATE_PARAMS[] = "params";
  constexpr static char NATIVE_DELEGATE_RET_DESCR[] = "ret_value_desc";
  constexpr static char NATIVE_DELEGATE_MACRO[] = "delegate_macro";
  constexpr static char NATIVE_DELEGATE_OWNING_CLASS[] = "owning";
  constexpr static char NATIVE_DELEGATE_PARENT_CLASS[] = "parent";

  constexpr static char LOG_CATEGORY_COMPILE_LEVEL[] = "compile";
  constexpr static char LOG_CATEGORY_RUNTIME_LEVEL[] = "runtime";
  constexpr static char LOG_CATEGORY_TYPE[] = "log_category_type";

  constexpr static char LOG_CATEGORY_CLASS[] = "class";
  constexpr static char LOG_CATEGORY_EXTERN[] = "extern";

  constexpr static char LOG_LEVEL_FATAL[] = "fatal";
  constexpr static char LOG_LEVEL_ERROR[] = "error";
  constexpr static char LOG_LEVEL_WARNING[] = "warning";
  constexpr static char LOG_LEVEL_DISPLAY[] = "display";
  constexpr static char LOG_LEVEL_LOG[] = "log";
  constexpr static char LOG_LEVEL_VERBOSE[] = "verbose";
  constexpr static char LOG_LEVEL_VERY_VERBOSE[] = "very_verbose";
  constexpr static char LOG_LEVEL_ALL[] = "all";

  constexpr static char INDEX_MODULES[] = "modules";
  constexpr static char CPP_INDEX_TITLE[] = "C++ API Reference";
  constexpr static char BLUEPRINT_INDEX_TITLE[] = "Blueprint API Reference";

  constexpr static char REPLICATION_MODE_NONE[] = "none";
  constexpr static char REPLICATION_MODE_REPLICATED[] = "replicated";
  constexpr static char REPLICATION_MODE_REP_NOTIFY[] = "rep_notify";

  constexpr static char REPLICATION_COND_NONE[] = "none";
  constexpr static char REPLICATION_COND_INITIAL_ONLY[] = "initial_only";
  constexpr static char REPLICATION_COND_OWNER_ONLY[] = "owner_only";
  constexpr static char REPLICATION_COND_SKIP_OWNER[] = "skip_owner";
  constexpr static char REPLICATION_COND_SIMULATED_ONLY[] = "simulated_only";
  constexpr static char REPLICATION_COND_AUTONOMOUS_ONLY[] = "autonomous_only";
  constexpr static char REPLICATION_COND_SIMULATED_OR_PHYSICS[] =
      "simulated_or_physics";
  constexpr static char REPLICATION_COND_INITIAL_OR_OWNER[] =
      "initial_or_owner";
  constexpr static char REPLICATION_COND_CUSTOM[] = "custom";
  constexpr static char REPLICATION_COND_REPLAY_OR_OWNER[] = "replay_or_owner";
  constexpr static char REPLICATION_COND_REPLAY_ONLY[] = "replay_only";
  constexpr static char REPLICATION_COND_SIMULATED_ONLY_NO_REPLAY[] =
      "simulated_only_no_replay";
  constexpr static char REPLICATION_COND_SIMULATED_OR_PHYSICS_NO_REPLAY[] =
      "simulated_or_physics_no_replay";
  constexpr static char REPLICATION_COND_SKIP_REPLAY[] = "skip_replay";
  constexpr static char REPLICATION_COND_NEVER[] = "never";

  constexpr static char META_DISPLAY_NAME[] = "displayname";
  constexpr static char META_CATEGORY[] = "category";
  constexpr static char REPLICATED_USING[] = "replicatedusing";

  constexpr static char BLUEPRINT_PATH_OBJECT_SEPARATOR = '.';

  constexpr static int PRETTY_INDENDATION_SIZE = 4;

  constexpr static uint64_t HTML2PNG_TIMEOUT_MS = 10000;

  bool mIsTrial = false;
  bool mDoBase64EncodeImages = false;
  bool mPrettyPrint = false;

  std::shared_ptr<DocsIndexNode> Index;

  std::unique_ptr<DocPathsCache> PathsCache;

  std::string ImagesDir;
  std::string ConversionOutputDir;
  std::string TypesImagesDir;
  std::string FunctionsImagesDir;
  std::string GeneratedImagesDir;
  std::string PagesDir;
  std::string OutputDirectory;

  std::string Html2PngPath;

  std::unique_ptr<BpInitializerExporter> InitializerExporter;

  std::unique_ptr<UFunctionSerializer> FunctionSerializer;
  std::unique_ptr<UTypeToImagesSerializer> TypeToImagesSerializer;
  std::unique_ptr<UTypeToStringSerializer> TypeToStringSerializer;

  std::vector<ImageToProcess> UserProvidedImagesToProcess;
  std::vector<ImageToProcess> GeneratedImagesToProcess;
  std::vector<ManifestEntry> Manifest;

  std::set<std::string> SerializedTypes;
  // native delegate ids whose blueprint counterparts have already been written
  std::unordered_set<DocPathsCache::IdType> NativeDelegatesWritten;
  // abs. path -> content
  std::unordered_map<std::string, std::string> ContentToWrite;
  uint64_t CurrentContentSize = 0;

  nlohmann::json BlueprintIndex;
  nlohmann::json CppIndex;

  std::string EmptyString;

  size_t LastBase64Size = 0;
  size_t PageNumber = 0;
  char* Base64Buffer = nullptr;

  ManifestEntry::Format PageFormat = ManifestEntry::Format::JSON_V1;

  std::unique_ptr<IntegrityCheck> Checker;

  std::unordered_map<DocPathsCache::IdType, std::string> PageTitles;
  std::unordered_map<DocPathsCache::IdType, std::string> PageBreadCrumbs;
  std::map<RpcMode, std::string> RpcModeToString;
  std::map<Replication, std::string> ReplicationModeToString;
  std::map<LifetimeCondition, std::string> ReplicationConditionToString;
  std::map<ClassType, std::string> ClassTypeToString;
  std::map<LogVerbosity, std::string> LogLevelToString;
  std::map<LogCategoryType, std::string> LogCategoryToString;
  std::map<NativeClassType, std::string> NativeClassTypeToString;
  std::map<AccessModifier, std::string> AccessToString;
  std::map<NativeFunctionDefinition, std::string> DefinitionToString;
  std::set<std::string> CosmeticSpecifiers;
  std::array<AccessModifier, 3> AccessModifiers = {AccessModifier::PUBLIC,
      AccessModifier::PROTECTED, AccessModifier::PRIVATE};
  std::map<PageType, std::string> ApiToDocPath;
  std::map<PageType, std::string> ApiToTitle;

  std::unique_ptr<FileDataLoader> DataLoader;
  std::function<void(Status)> ReportCallback;

  mutable std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor

