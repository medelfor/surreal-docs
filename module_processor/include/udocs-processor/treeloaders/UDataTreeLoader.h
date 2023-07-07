/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/spdlog.h>
#include <functional>
#include <unordered_map>
#include <string>
#include <memory>
#include <map>
#include <nlohmann/json.hpp>
#include "TreeLoader.h"
#include "DocCommentProcessor.h"
#include "udocs-processor/doctree/blueprints/BlueprintClassNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintPropertyNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintFunctionNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintObjectTypeNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintPrimitiveTypeNode.h"
#include "udocs-processor/doctree/AccessModifier.h"

namespace udocs_processor {
class BlueprintClassParentNode;
class BlueprintEnumElementNode;
class BlueprintEnumNode;
class BlueprintModuleNode;
class BlueprintParameterNode;
class BlueprintStructNode;
class BlueprintArrayTypeNode;
class BlueprintDelegateTypeNode;
class BlueprintEnumTypeNode;
class BlueprintInterfaceTypeNode;
class BlueprintMapTypeNode;
class BlueprintObjectTypeNode;
class BlueprintPrimitiveTypeNode;
class BlueprintSetTypeNode;
class BlueprintStructTypeNode;
class BlueprintTypeNode;
class BlueprintInitializerNode;
class SeeNode;

class BaseCommentProcessor : public RefCommentProcessor {
 public:
  explicit BaseCommentProcessor(
      IdGenerator& IdGen, BlueprintUNode& Node,
      const std::string& Content, ProcessingReport* Report);

  void OnBrief(size_t NextSymbolPos);

  void OnSee(size_t NextSymbolPos);

  void OnSince(size_t NextSymbolPos);

  void OnDeprecated(size_t NextSymbolPos);

  void Process() override;

 private:
  std::shared_ptr<SeeNode> See;
  ProcessingReport* Report = nullptr;
  BlueprintUNode& Node;
  IdGenerator& IdGen;

  constexpr static char COMMAND_BRIEF[] = "brief";
  constexpr static char COMMAND_SEE[] = "see";
  constexpr static char COMMAND_SINCE[] = "since";
  constexpr static char COMMAND_DEPRECATED[] = "deprecated";
};

class NodeCommentProcessor : public BaseCommentProcessor {
 public:
  explicit NodeCommentProcessor(
      IdGenerator& IdGen, BlueprintFunctionNode& Function,
      const std::string& Content, ProcessingReport* Report);

  void OnParam(size_t NextSymbolPos);

  void OnReturn(size_t NextSymbolPos);

  void Process() override;

 private:
  void ProcessParam(size_t NextSymbolPos, bool IsReturn);

  ProcessingReport* Report = nullptr;
  BlueprintFunctionNode& Function;
  IdGenerator& IdGen;

  constexpr static char COMMAND_PARAM[] = "param";
  constexpr static char COMMAND_RETURN[] = "return";

  constexpr static char RETURN_PARAM_NAME[] = "ReturnValue";

  constexpr static char PARAM_PATTERN[] =
      R"-((?:"([^"]+)"|([^\s]+))\s+(.+)(\n|\r)*)-";
};

class UDataTreeLoader : public TreeLoader {
 public:
  UDataTreeLoader();

  // it's expected that json conforms to modules part of udata json scheme
  // in other words it's expected to be array of modules
  void SetUDataJson(std::shared_ptr<const nlohmann::json> Json);

  void LoadTree(IdGenerator &IdGen, std::weak_ptr<DocNode> Root) override;

  std::string_view GetName() const override;

 private:
#define TYPE_DESERIALIZER_ARGS std::weak_ptr<DocNode> Parent, \
  IdGenerator & IdGen, const nlohmann::json& Json, \
  std::shared_ptr<BlueprintTypeNode> Base, bool& IsReferencable

  using TypeDeserializer =
      std::function<std::shared_ptr<BlueprintTypeNode>(
        TYPE_DESERIALIZER_ARGS)>;

  // todo(artsiom.drapun): make functions const where possible

  std::shared_ptr<BlueprintModuleNode> DeserializeModule(
      std::weak_ptr<DocNode> Parent,
      IdGenerator &IdGen, const nlohmann::json& Json);

  std::shared_ptr<BlueprintClassNode> DeserializeClass(
      std::weak_ptr<DocNode> Parent,
      IdGenerator &IdGen, const nlohmann::json& Json);

  std::shared_ptr<BlueprintEnumNode> DeserializeEnum(
      std::weak_ptr<DocNode> Parent,
      IdGenerator &IdGen, const nlohmann::json& Json);

  std::shared_ptr<BlueprintFunctionNode> DeserializeFunction(
      std::weak_ptr<DocNode> Parent,
      IdGenerator &IdGen, const nlohmann::json& Json);

  std::shared_ptr<BlueprintParameterNode> DeserializeParameter(
      std::weak_ptr<DocNode> Parent,
      IdGenerator &IdGen, const nlohmann::json& Json);

  std::shared_ptr<BlueprintPropertyNode> DeserializeProperty(
      std::weak_ptr<DocNode> Parent,
      IdGenerator &IdGen, const nlohmann::json& Json);

  std::shared_ptr<BlueprintStructNode> DeserializeStruct(
      std::weak_ptr<DocNode> Parent,
      IdGenerator &IdGen, const nlohmann::json& Json);

  // isReferencable - if an initializer of this type can reference something
  // true for enum and object and inteface
  std::shared_ptr<BlueprintTypeNode> DeserializeType(
      std::weak_ptr<DocNode> Parent,
      IdGenerator &IdGen, const nlohmann::json& Json, bool& IsReferencable);

  void DeserializeName(const nlohmann::json& Json,
      std::shared_ptr<BlueprintUNode> Node);

  void DeserializeBasicIdentifierInfo(const nlohmann::json& Json,
      std::shared_ptr<BlueprintUNode> Node);

  void DeserializeIdentifier(const nlohmann::json& Json,
      std::shared_ptr<BlueprintUNode> Node);

  std::shared_ptr<BlueprintTypeNode> DeserializePrimitiveType(
      std::weak_ptr<DocNode> Parent, IdGenerator & IdGen,
      const nlohmann::json& Json, std::shared_ptr<BlueprintTypeNode> Base,
      bool& IsReferencable);

  std::shared_ptr<BlueprintTypeNode> DeserializeStructType(
      std::weak_ptr<DocNode> Parent, IdGenerator & IdGen,
      const nlohmann::json& Json, std::shared_ptr<BlueprintTypeNode> Base,
      bool& IsReferencable);

  std::shared_ptr<BlueprintTypeNode> DeserializeEnumType(
      std::weak_ptr<DocNode> Parent, IdGenerator & IdGen,
      const nlohmann::json& Json, std::shared_ptr<BlueprintTypeNode> Base,
      bool& IsReferencable);

  std::shared_ptr<BlueprintTypeNode> DeserializeArrayType(
      std::weak_ptr<DocNode> Parent, IdGenerator & IdGen,
      const nlohmann::json& Json, std::shared_ptr<BlueprintTypeNode> Base,
      bool& IsReferencable);

  std::shared_ptr<BlueprintTypeNode> DeserializeSetType(
      std::weak_ptr<DocNode> Parent, IdGenerator & IdGen,
      const nlohmann::json& Json, std::shared_ptr<BlueprintTypeNode> Base,
      bool& IsReferencable);

  std::shared_ptr<BlueprintTypeNode> DeserializeMapType(
      std::weak_ptr<DocNode> Parent, IdGenerator & IdGen,
      const nlohmann::json& Json, std::shared_ptr<BlueprintTypeNode> Base,
      bool& IsReferencable);

  std::shared_ptr<BlueprintTypeNode> DeserializeInterfaceType(
      std::weak_ptr<DocNode> Parent, IdGenerator & IdGen,
      const nlohmann::json& Json, std::shared_ptr<BlueprintTypeNode> Base,
      bool& IsReferencable);

  std::shared_ptr<BlueprintTypeNode> DeserializeObjectType(
      std::weak_ptr<DocNode> Parent, IdGenerator & IdGen,
      const nlohmann::json& Json, std::shared_ptr<BlueprintTypeNode> Base,
      bool& IsReferencable);

  std::shared_ptr<BlueprintTypeNode> DeserializeDelegateType(
      std::weak_ptr<DocNode> Parent, IdGenerator & IdGen,
      const nlohmann::json& Json, std::shared_ptr<BlueprintTypeNode> Base,
      bool& IsReferencable);

  void DeserializeBaseType(const nlohmann::json& Json,
      std::shared_ptr<BlueprintTypeNode> Node);

  // supposed to be called after all the function's info is already loaded
  void ProcessFunctionComments(
      IdGenerator& IdGen, BlueprintFunctionNode& Function);

  void ProcessStandardComments(IdGenerator& IdGen, BlueprintUNode& Node);

  void ProcessRefs(IdGenerator& IdGen, BlueprintUNode& Node);

  std::shared_ptr<BlueprintInitializerNode> MakeInitializer(
      std::weak_ptr<DocNode> Parent, IdGenerator & IdGen,
      std::string Value, bool IsReferencable) const;

  std::map<std::string, ClassType> StringToClassType;
  std::map<std::string, Replication> StringToReplication;
  std::map<std::string, LifetimeCondition> StringToLifetimeCondition;
  std::map<std::string, MulticastDelegateFlags>
      StringToMulticastDelegateFlags;
  std::map<std::string, RpcMode> StringToRpc;
  std::map<std::string, AccessModifier> StringToAccessModifier;
  std::map<std::string, TypeDeserializer> StringToTypeDeserializer;
  std::map<std::string, ObjectReference> StringToObjectReference;
  std::map<std::string, PrimitiveType::PrimitiveType>
      StringToPrimitiveType;

  constexpr static char IDENTIFIER[] = "identifier";
  constexpr static char MODULE_CLASSES[] = "classes";
  constexpr static char MODULE_ENUMS[] = "enums";
  constexpr static char MODULE_STRUCTS[] = "structs";
  constexpr static char BASIC_INFO_NAME[] = "name";
  constexpr static char BASIC_INFO_PATH[] = "path";
  constexpr static char BASIC_INFO_IS_NATIVE[] = "is_native";
  constexpr static char IDENTIFIER_TOOLTIP[] = "tooltip";
  constexpr static char IDENTIFIER_SHORT_TOOLTIP[] = "short_tooltip";
  constexpr static char IDENTIFIER_CATEGORY[] = "category";
  constexpr static char IDENTIFIER_IS_DEPRECATED[] = "is_deprecated";
  constexpr static char IDENTIFIER_DEPRECATION_MESSAGE[] =
      "deprecation_message";
  constexpr static char NAME_NAME[] = "name";
  constexpr static char NAME_DISPLAY_NAME[] = "display_name";
  constexpr static char CLASS_IS_ABSTRACT[] = "is_abstract";
  constexpr static char CLASS_IS_INTERFACE[] = "is_interface";
  constexpr static char CLASS_IS_BLUEPRINT_TYPE[] = "is_blueprint_type";
  constexpr static char CLASS_IS_BLUEPRINTABLE[] = "is_blueprintable";
  constexpr static char CLASS_TYPE[] = "class_type";
  constexpr static char CLASS_SUPER_CLASS[] = "super_class";
  constexpr static char CLASS_NAMESPACE[] = "namespace";
  constexpr static char CLASS_CONFIG_NAME[] = "config_name";
  constexpr static char CLASS_INTERFACES[] = "interfaces";
  constexpr static char CLASS_FUNCTIONS[] = "functions";
  constexpr static char CLASS_PROPERTIES[] = "properties";
  constexpr static char UDOCS_PROCESSOR_LOGGER_NAME[] = "udocs-processor";

  constexpr static char CLASS_TYPE_CLASS[] = "class";
  constexpr static char CLASS_TYPE_COMPONENT[] = "component";
  constexpr static char CLASS_TYPE_WIDGET[] = "widget";
  constexpr static char CLASS_TYPE_LIBRARY[] = "function_library";
  constexpr static char CLASS_TYPE_ACTOR[] = "actor";
  constexpr static char CLASS_TYPE_DEVSETTINGS[] = "devsettings";
  constexpr static char CLASS_TYPE_SUBSYSTEM[] = "subsystem";

  constexpr static char PROPERTY_TYPE[] = "type";
  constexpr static char PROPERTY_IS_INSTANCE_EDITABLE[] =
      "is_instance_editable";
  constexpr static char PROPERTY_IS_ARCHETYPE_EDITABLE[] =
      "is_archetype_editable";
  constexpr static char PROPERTY_IS_READ_ONLY[] = "is_read_only";
  constexpr static char PROPERTY_IS_EXPOSE_ON_SPAWN[] = "is_expose_on_spawn";
  constexpr static char PROPERTY_IS_PRIVATE[] = "is_private";
  constexpr static char PROPERTY_IS_EXPOSE_TO_CINEMATIC[] =
      "is_expose_to_cinematic";
  constexpr static char PROPERTY_IS_CONFIG_VARIABLE[] = "is_config_variable";
  constexpr static char PROPERTY_IS_BLUEPRINT_VISIBLE[] =
      "is_blueprint_visible";
  constexpr static char PROPERTY_MIN[] = "min";
  constexpr static char PROPERTY_MAX[] = "max";
  constexpr static char PROPERTY_DEFAULT_VALUE[] = "default_value";
  constexpr static char PROPERTY_REPLICATION[] = "replication";
  constexpr static char PROPERTY_REPLICATION_CONDITION[] =
      "replication_condition";
  constexpr static char PROPERTY_MULTICAST_DELEGATE_FLAGS[] =
      "multicast_delegate_flags";

  constexpr static char PROPERTY_REPLICATION_NONE[] = "none";
  constexpr static char PROPERTY_REPLICATION_REPLICATE[] = "replicate";
  constexpr static char PROPERTY_REPLICATION_REPNOTIFY[] = "repnotify";

  constexpr static char PROPERTY_REPLICATION_CONDITION_NONE[] = "none";
  constexpr static char PROPERTY_REPLICATION_CONDITION_INITIAL_ONLY[] =
      "initial_only";
  constexpr static char PROPERTY_REPLICATION_CONDITION_OWNER_ONLY[] =
      "owner_only";
  constexpr static char PROPERTY_REPLICATION_CONDITION_SKIP_OWNER[] =
      "skip_owner";
  constexpr static char PROPERTY_REPLICATION_CONDITION_SIMULATED_ONLY[] =
      "simulated_only";
  constexpr static char PROPERTY_REPLICATION_CONDITION_AUTONOMOUS_ONLY[] =
      "autonomous_only";
  constexpr static char PROPERTY_REPLICATION_CONDITION_SIMULATED_OR_PHYSICS[] =
      "simulated_or_physics";
  constexpr static char PROPERTY_REPLICATION_CONDITION_INITIAL_OR_OWNER[] =
      "initial_or_owner";
  constexpr static char PROPERTY_REPLICATION_CONDITION_CUSTOM[] = "custom";
  constexpr static char PROPERTY_REPLICATION_CONDITION_REPLAY_OR_OWNER[] =
      "replay_or_owner";
  constexpr static char PROPERTY_REPLICATION_CONDITION_REPLAY_ONLY[] =
      "replay_only";
  constexpr static char
    PROPERTY_REPLICATION_CONDITION_SIMULATED_ONLY_NO_REPLAY[] =
        "simulated_only_no_replay";
  constexpr static char
    PROPERTY_REPLICATION_CONDITION_SIMULATED_OR_PHYSICS_NO_REPLAY[] =
        "simulated_or_physics_no_replay";
  constexpr static char PROPERTY_REPLICATION_CONDITION_SKIP_REPLAY[] =
      "skip_replay";
  constexpr static char PROPERTY_REPLICATION_CONDITION_NEVER[] = "never";

  constexpr static char PROPERTY_MULTICAST_ASSIGNABLE[] = "assignable";
  constexpr static char PROPERTY_MULTICAST_CALLABLE[] = "callable";
  constexpr static char PROPERTY_MULTICAST_AUTHORITY_ONLY[] = "authority_only";

  constexpr static char FUNCTION_KEYWORDS[] = "keywords";
  constexpr static char FUNCTION_COMPACT_NODE_TITLE[] = "compact_node_title";
  constexpr static char FUNCTION_NODE_COMMENT[] = "node_comment";
  constexpr static char FUNCTION_ACCESS[] = "access";
  constexpr static char FUNCTION_RPC[] = "rpc";
  constexpr static char FUNCTION_IS_PURE[] = "is_pure";
  constexpr static char FUNCTION_IS_CALL_IN_EDITOR[] = "is_call_in_editor";
  constexpr static char FUNCTION_IS_OVERRIDE[] = "is_override";
  constexpr static char FUNCTION_IS_EVENT[] = "is_event";
  constexpr static char FUNCTION_IS_RELIABLE[] = "is_reliable";
  constexpr static char FUNCTION_IS_STATIC[] = "is_static";
  constexpr static char FUNCTION_IS_CONST[] = "is_const";
  constexpr static char FUNCTION_IS_BLUEPRINT_CALLABLE[] =
      "is_blueprint_callable";
  constexpr static char FUNCTION_IS_OVERRIDABLE[] = "is_overridable";
  constexpr static char FUNCTION_IS_BLUEPRINT_PROTECTED[] =
      "is_blueprint_protected";
  constexpr static char FUNCTION_IS_LATENT[] = "is_latent";
  constexpr static char FUNCTION_IS_COSMETIC[] = "is_cosmetic";
  constexpr static char FUNCTION_IS_AUTHORITY_ONLY[] = "is_authority_only";
  constexpr static char FUNCTION_PARAMETERS[] = "parameters";
  constexpr static char FUNCTION_RETURN[] = "return";

  constexpr static char FUNCTION_RPC_CLIENT[] = "client";
  constexpr static char FUNCTION_RPC_SERVER[] = "server";
  constexpr static char FUNCTION_RPC_NET_MULTICAST[] = "net_multicast";
  constexpr static char FUNCTION_RPC_NOT_REPLICATED[] = "not_replicated";

  constexpr static char ACCESS_PUBLIC[] = "public";
  constexpr static char ACCESS_PRIVATE[] = "private";
  constexpr static char ACCESS_PROTECTED[] = "protected";

  constexpr static char STRUCT_PROPERTIES[] = "properties";
  constexpr static char STRUCT_IS_BLUEPRINT_TYPE[] = "is_blueprint_type";

  constexpr static char ENUM_IS_BLUEPRINT_TYPE[] = "is_blueprint_type";
  constexpr static char ENUM_ELEMENTS[] = "elements";

  constexpr static char ENUM_ELEMENT_DESCRIPTION[] = "description";
  constexpr static char ENUM_ELEMENT_IS_HIDDEN[] = "is_hidden";

  constexpr static char MEMBER_SEPARATOR[] = ":";

  constexpr static char PARAMETER_NAME[] = "name";
  constexpr static char PARAMETER_TYPE[] = "type";
  constexpr static char PARAMETER_DEFAULT_VALUE[] = "default_value";

  constexpr static char TYPE_PRIMITIVE[] = "primitive";
  constexpr static char TYPE_STRUCT[] = "struct";
  constexpr static char TYPE_ENUM[] = "enum";
  constexpr static char TYPE_ARRAY[] = "array";
  constexpr static char TYPE_SET[] = "set";
  constexpr static char TYPE_MAP[] = "map";
  constexpr static char TYPE_INTERFACE[] = "interface";
  constexpr static char TYPE_OBJECT[] = "object";
  constexpr static char TYPE_DELEGATE[] = "delegate";

  constexpr static char TYPE_TYPE[] = "type";
  constexpr static char TYPE_BASIC_INFO[] = "basic_info";
  constexpr static char TYPE_IS_REFERENCE[] = "is_reference";
  constexpr static char TYPE_IS_CONST[] = "is_const";

  constexpr static char TYPE_DELEGATE_IS_MULTICAST[] = "is_multicast";
  constexpr static char TYPE_DELEGATE_SIGNATURE[] = "signature";

  constexpr static char TYPE_PRIMITIVE_TYPE[] = "primitive_type";

  constexpr static char TYPE_ARRAY_ELEMENT_TYPE[] = "element_type";

  constexpr static char TYPE_SET_ELEMENT_TYPE[] = "element_type";

  constexpr static char TYPE_OBJECT_CLASS_TYPE[] = "class_type";
  constexpr static char TYPE_OBJECT_REFERENCE[] = "reference";

  constexpr static char TYPE_MAP_KEY_TYPE[] = "key_type";
  constexpr static char TYPE_MAP_VALUE_TYPE[] = "value_type";

  constexpr static char OBJECT_REFERENCE_OBJECT[] = "object";
  constexpr static char OBJECT_REFERENCE_WEAK_OBJECT[] = "weak_object";
  constexpr static char OBJECT_REFERENCE_LAZY_OBJECT[] = "lazy_object";
  constexpr static char OBJECT_REFERENCE_SOFT_OBJECT[] = "soft_object";
  constexpr static char OBJECT_REFERENCE_CLASS[] = "class";
  constexpr static char OBJECT_REFERENCE_SOFT_CLASS[] = "soft_class";
  constexpr static char OBJECT_REFERENCE_UNDEFINED[] = "undefined";

  constexpr static char PRIMITIVE_TYPE_FLOAT[] = "float";
  constexpr static char PRIMITIVE_TYPE_DOUBLE[] = "double";
  constexpr static char PRIMITIVE_TYPE_BYTE[] = "byte";
  constexpr static char PRIMITIVE_TYPE_INT[] = "int";
  constexpr static char PRIMITIVE_TYPE_INT64[] = "int64";
  constexpr static char PRIMITIVE_TYPE_BOOL[] = "bool";
  constexpr static char PRIMITIVE_TYPE_STRING[] = "string";
  constexpr static char PRIMITIVE_TYPE_TEXT[] = "text";
  constexpr static char PRIMITIVE_TYPE_NAME[] = "name";
  constexpr static char PRIMITIVE_TYPE_UNDEFINED[] = "undefined";

  constexpr static char TREELOADER_NAME[] = "UData Tree Loader";

  std::shared_ptr<const nlohmann::json> Json;
  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor

