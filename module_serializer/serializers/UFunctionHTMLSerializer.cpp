/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#define NOLINT()

#include "udocs-processor/serializers/UFunctionHTMLSerializer.h"

#include <fmt/format.h>
#include <utility>
#include <regex> NOLINT()
#include <inja/inja.hpp>

#include <nlohmann/json.hpp>
#include "udocs-processor/doctree/blueprints/BlueprintFunctionNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintParameterNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintInitializerNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintClassNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintTypeNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintObjectTypeNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintArrayTypeNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintEnumTypeNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintInterfaceTypeNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintMapTypeNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintPrimitiveTypeNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintDelegateTypeNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintSetTypeNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintStructTypeNode.h"
#include "udocs-processor/doctree/ReferencableWordNode.h"
#include "udocs-processor/doctree/DocNodeVisitor.h"
#include "udocs-processor/doctree/NodeTypeChecker.h"
#include "udocs-processor/NodesHelper.h"
#include "udocs-processor/StringHelper.h"

udocs_processor::UFunctionHTMLSerializer::UFunctionHTMLSerializer() {
  RpcModeToString.insert(
      std::make_pair(RpcMode::NotReplicated, RPC_NONE));
  RpcModeToString.insert(
      std::make_pair(RpcMode::Server, RPC_SERVER));
  RpcModeToString.insert(
      std::make_pair(RpcMode::Client, RPC_CLIENT));
  RpcModeToString.insert(
      std::make_pair(RpcMode::NetMulticast, RPC_MULTICAST));

  Environment.add_callback(RESOURCE_CALLABLE,
      RESOURCE_CALLABLE_ARGS_COUNT,
      [this](InjaArgsStripped Args) {
        return MakeResource(Args);
  });
}

nlohmann::json udocs_processor::UFunctionHTMLSerializer::MakeResource(
    InjaArgs Args) const {
  if (!Args.empty() && Args[0]->is_string()) {
    return GetResourcesPath() + DIRECTORY_SEPARATOR +
        Args[0]->get<std::string>();
  }

  return "";
}

std::string udocs_processor::UFunctionHTMLSerializer::Serialize(
    BlueprintFunctionNode &Function) {
  nlohmann::json FunctionData;

  FunctionData[NAME] = Function.GetDisplayName();
  FunctionData[COMPACT_TITLE] = Function.GetCompactNodeTitle();
  FunctionData[IS_COMPACT_FUNCTION] = !Function.GetCompactNodeTitle().empty();

  FunctionData[RPC] = RPC_NONE;
  auto It = RpcModeToString.find(Function.GetRpcMode());
  if (It != RpcModeToString.end()) {
    FunctionData[RPC] = It->second;
  }

  bool IsEvent = false;
  bool IsDelegate = false;
  bool IsBpDelegate = false;
  bool IsFunction = false;
  bool IsConstructionScript = false;
  bool IsPure = false;

  if (auto Parent = Function.GetParent().lock();
      Parent &&
      NodeTypeChecker<BlueprintDelegateTypeNode>(*Parent).Matched()) {
    // if child of bp delegate type then it's a signature of
    // a delegate - native or not
    IsDelegate = true;
    IsBpDelegate = !Function.IsNative();  // is event dispatcher
  } else if (Function.IsEvent()) {
    IsEvent = true;
  } else if (Function.GetName() == CONSTRUCTION_SCRIPT_NAME) {
    IsConstructionScript = true;
  } else if (Function.IsPure()) {
    IsPure = true;
  } else {
    IsFunction = true;
  }

  FunctionData[IS_EVENT] = IsEvent;
  FunctionData[IS_DELEGATE] = IsDelegate;
  FunctionData[IS_BP_DELEGATE] = IsBpDelegate;
  FunctionData[IS_FUNCTION] = IsFunction;
  FunctionData[IS_CONSTRUCTION_SCRIPT] = IsConstructionScript;
  FunctionData[IS_PURE] = IsPure;

  bool IsFunctionLibrary = false;
  std::string Class;
  if (auto ParentClass = std::dynamic_pointer_cast<BlueprintClassNode>(
      Function.GetParent().lock())) {
      Class = ParentClass->GetDisplayName();
      IsFunctionLibrary =
          ParentClass->GetClassType() == ClassType::FunctionLibrary;
  }
  FunctionData[IS_FUNCTION_LIBRARY] = IsFunctionLibrary;
  FunctionData[CLASS] = Class;

  FunctionData[IS_TRIAL] = mIsTrial;

  FunctionData[IS_COSMETIC] = Function.IsCosmetic();
  FunctionData[IS_AUTHORITY_ONLY] = Function.IsAuthorityOnly();
  FunctionData[IS_RELIABLE] = Function.IsReliable();

  FunctionData[PARAMS] = ExportParameters(Function, false);
  FunctionData[OUT_PARAMS] = ExportParameters(Function, true);

  return Environment.render(FunctionTemplate, FunctionData);
}

nlohmann::json udocs_processor::UFunctionHTMLSerializer::ExportParameters(
    BlueprintFunctionNode &Function, bool ShouldBeReturn) const {
  nlohmann::json Result = nlohmann::json::array();

  class ParamFinder : public DocNodeVisitor {
   public:
    ParamFinder(nlohmann::json& Result, bool ShouldBeReturn)
      : ShouldBeReturn(ShouldBeReturn), Result(Result) {}

    int Visit(BlueprintParameterNode &Node) override {
      if (Node.IsReturnParameter() != ShouldBeReturn) {
        return 0;
      }

      nlohmann::json Parameter;
      Parameter[ANY_PARAM_NAME] = Node.GetDisplayName();
      Parameter[ANY_PARAM_TYPE] = {};

      class InitializerFinder : public DocNodeVisitor {
       public:
         int Visit(BlueprintInitializerNode &Node) override {
           Initializer = &Node;

           return 1;
         }

        BlueprintInitializerNode* Initializer = nullptr;
      } InitializerFinderInstance;
      Node.IterateChildren(InitializerFinderInstance, false);

      std::string DefValue;
      if (InitializerFinderInstance.Initializer) {
        class ConcatRefs : public DocNodeVisitor {
         public:
          int Visit(ReferencableWordNode &Node) override {
            Result += Node.GetWord();

            return 1;
          }

          std::string Result;
        } Concater;
        InitializerFinderInstance.Initializer->IterateChildren(Concater, false);

        DefValue = std::move(Concater.Result);
      }

      TypeExporter Exporter(DefValue);
      Node.IterateChildren(Exporter, false);

      Parameter[ANY_PARAM_TYPE][ANY_PARAM_TYPE_STRUCTURE] =
          Exporter.GetStructure();
      Parameter[ANY_PARAM_TYPE][ANY_PARAM_TYPE_IS_REFERENCE] =
          Exporter.IsReference();
      Parameter[ANY_PARAM_TYPE][ANY_PARAM_TYPE_MAIN] = Exporter.GetMainType();
      Parameter[ANY_PARAM_TYPE][ANY_PARAM_TYPE_SECONDARY] =
          Exporter.GetSecondaryType();

      if (!Node.IsReturnParameter()) {
        Parameter[PARAM_VALUE] = {};
        Parameter[PARAM_VALUE][PARAM_VALUE_IS_CHECKED] = Exporter.IsChecked();
        Parameter[PARAM_VALUE][PARAM_VALUE_TEXT] = Exporter.GetText();
        Parameter[PARAM_VALUE][PARAM_VALUE_VECTOR] = Exporter.GetVector();
      }

      Result.push_back(Parameter);

      return 0;
    }

    nlohmann::json& Result;
    bool ShouldBeReturn = false;
  } ParamFinderInstance(Result, ShouldBeReturn);
  Function.IterateChildren(ParamFinderInstance, false);

  return Result;
}

const std::string &
  udocs_processor::UFunctionHTMLSerializer::GetFunctionTemplate() const {
  return FunctionTemplate;
}

void udocs_processor::UFunctionHTMLSerializer::SetFunctionTemplate(
    std::string FunctionTemplate) {
  this->FunctionTemplate = std::move(FunctionTemplate);
}

bool udocs_processor::UFunctionHTMLSerializer::IsTrial() const {
  return mIsTrial;
}

void udocs_processor::UFunctionHTMLSerializer::SetIsTrial(bool IsTrial) {
  mIsTrial = IsTrial;
}

std::string udocs_processor::UFunctionHTMLSerializer::GetFileExtension() const {
  return FILE_EXTENSION;
}

const std::string &udocs_processor::TypeExporter::GetMainType() const {
  return MainType;
}

const std::string &udocs_processor::TypeExporter::GetSecondaryType() const {
  return SecondaryType;
}

const std::string &udocs_processor::TypeExporter::GetStructure() const {
  return Structure;
}

bool udocs_processor::TypeExporter::IsChecked() const {
  return mIsChecked;
}

const nlohmann::json &udocs_processor::TypeExporter::GetVector() const {
  return Vector;
}

const std::string &udocs_processor::TypeExporter::GetText() const {
  return Text;
}

bool udocs_processor::TypeExporter::IsReference() const {
  return mIsReference;
}

std::string udocs_processor::TypeExporter::GetObjectNameFromPath(
    const std::string& Path) const {
  size_t Pos = Path.rfind(PATH_OBJECT_SEPARATOR);

  if (Pos != std::string::npos && Pos + 1 < Path.size()) {
    return Path.substr(Pos + 1);
  } else {
    return Path;
  }
}

int udocs_processor::TypeExporter::Visit(BlueprintEnumTypeNode &Node) {
  ExtractBaseInfo(Node);

  MainType = TYPE_ENUM;
  Structure = TYPE_STRUCTURE_ENUM;
  Text = DefaultValue;

  if (Callback) {
    Callback(*this);
  }

  return 0;
}

int udocs_processor::TypeExporter::Visit(BlueprintInterfaceTypeNode &Node) {
  ExtractBaseInfo(Node);

  MainType = TYPE_INTERFACE;
  Structure = TYPE_STRUCTURE_CLASS;
  Text = GetObjectNameFromPath(DefaultValue);

  if (Callback) {
    Callback(*this);
  }

  return 0;
}

int udocs_processor::TypeExporter::Visit(BlueprintObjectTypeNode &Node) {
  ExtractBaseInfo(Node);

  MainType = TYPE_OBJECT;
  auto It = ObjectReferenceToString.find(Node.GetObjectReference());
  if (It != ObjectReferenceToString.begin()) {
    MainType = It->second;
  }

  Structure = TYPE_STRUCTURE_CLASS;
  Text = GetObjectNameFromPath(DefaultValue);

  if (Callback) {
    Callback(*this);
  }

  return 0;
}

int udocs_processor::TypeExporter::Visit(BlueprintPrimitiveTypeNode &Node) {
  ExtractBaseInfo(Node);

  PrimitiveType::PrimitiveType Type = Node.GetPrimitiveType();

  MainType = TYPE_BOOL;
  auto It = PrimitiveTypeToString.find(Type);
  if (It != PrimitiveTypeToString.end()) {
    MainType = It->second;
  }

  if (Type == PrimitiveType::PrimitiveType::Bool) {
    Structure = TYPE_STRUCTURE_BOOL;
    mIsChecked = DefaultValue == TRUE_LITERAL;
  } else {
    Structure = TYPE_STRUCTURE_TEXT;
  }

  if (Type == PrimitiveType::PrimitiveType::Float ||
      Type == PrimitiveType::PrimitiveType::Double) {
    Text = NodesHelper::FormatNumber(StringHelper::SafeStof(DefaultValue));
  } else {
    Text = DefaultValue;
  }

  if (Callback) {
    Callback(*this);
  }

  return 0;
}

int udocs_processor::TypeExporter::Visit(BlueprintArrayTypeNode &Node) {
  ExtractBaseInfo(Node);

  // expecting one type child
  TypeExporter Exporter(DefaultValue);
  Node.IterateChildren(Exporter, false);

  Structure = TYPE_STRUCTURE_ARRAY;
  MainType = Exporter.GetMainType();

  if (Callback) {
    Callback(*this);
  }

  return 0;
}

int udocs_processor::TypeExporter::Visit(BlueprintMapTypeNode &Node) {
  ExtractBaseInfo(Node);

  // expecting two types child
  bool FirstOne = true;
  TypeExporter Exporter(DefaultValue,
    [this, &FirstOne]
    (TypeExporter& Exporter) {
      (FirstOne ? MainType : SecondaryType) = Exporter.GetMainType();
      FirstOne = false;
    });
  Node.IterateChildren(Exporter, false);

  Structure = TYPE_STRUCTURE_MAP;
  MainType = Exporter.GetMainType();

  if (Callback) {
    Callback(*this);
  }

  return 0;
}

int udocs_processor::TypeExporter::Visit(BlueprintSetTypeNode &Node) {
  ExtractBaseInfo(Node);

  // expecting one type child
  TypeExporter Exporter(DefaultValue);
  Node.IterateChildren(Exporter, false);

  Structure = TYPE_STRUCTURE_SET;
  MainType = Exporter.GetMainType();

  if (Callback) {
    Callback(*this);
  }

  return 0;
}

int udocs_processor::TypeExporter::Visit(BlueprintStructTypeNode &Node) {
  ExtractBaseInfo(Node);

  if (Node.GetPath() == NodesHelper::VECTOR_PATH) {
    MainType = TYPE_VECTOR;
    Structure = TYPE_STRUCTURE_VECTOR;
    double X, Y, Z;
    if (NodesHelper::ParseParameterXYZInitializer(DefaultValue, X, Y, Z)) {
      Vector = MakeVector(X, Y, Z);
    }
  } else if (Node.GetPath() == NodesHelper::ROTATOR_PATH) {
    MainType = TYPE_ROTATOR;
    Structure = TYPE_STRUCTURE_VECTOR;
    double X, Y, Z;
    if (NodesHelper::ParseParameterXYZInitializer(DefaultValue, X, Y, Z)) {
      Vector = MakeVector(X, Y, Z);
    }
  } else {
    Structure = TYPE_STRUCTURE_DEFAULT;

    if (Node.GetPath() == NodesHelper::TRANSFORM_PATH) {
      MainType = TYPE_TRANSFORM;
    } else {
      MainType = TYPE_STRUCT;
    }
  }

  if (Callback) {
    Callback(*this);
  }

  return 0;
}

int udocs_processor::TypeExporter::Visit(BlueprintDelegateTypeNode &Node) {
  ExtractBaseInfo(Node);

  MainType = TYPE_DEFAULT;
  Structure = TYPE_STRUCTURE_DELEGATE;

  if (Callback) {
    Callback(*this);
  }

  return 0;
}

nlohmann::json udocs_processor::TypeExporter::MakeVector(
    double X, double Y, double Z) const {
  nlohmann::json Vector;

  Vector[PARAM_VALUE_VECTOR_X] = NodesHelper::FormatNumber(X);
  Vector[PARAM_VALUE_VECTOR_Y] = NodesHelper::FormatNumber(Y);
  Vector[PARAM_VALUE_VECTOR_Z] = NodesHelper::FormatNumber(Z);

  return Vector;
}



void udocs_processor::TypeExporter::ExtractBaseInfo(BlueprintTypeNode& Node) {
  mIsReference = Node.IsReference();
}

udocs_processor::TypeExporter::TypeExporter(const std::string& DefaultValue)
    : DefaultValue(DefaultValue) {
  Vector = MakeVector(0, 0, 0);

  ObjectReferenceToString.insert(
      std::make_pair(ObjectReference::Class, TYPE_CLASS));
  ObjectReferenceToString.insert(
      std::make_pair(ObjectReference::SoftClass, TYPE_SOFT_CLASS));
  ObjectReferenceToString.insert(
      std::make_pair(ObjectReference::Object, TYPE_OBJECT));
  ObjectReferenceToString.insert(
      std::make_pair(ObjectReference::WeakObject, TYPE_OBJECT));
  ObjectReferenceToString.insert(
      std::make_pair(ObjectReference::SoftObject, TYPE_SOFT_OBJECT));

  PrimitiveTypeToString.insert(
      std::make_pair(PrimitiveType::PrimitiveType::Bool, TYPE_BOOL));
  PrimitiveTypeToString.insert(
      std::make_pair(PrimitiveType::PrimitiveType::Byte, TYPE_BYTE));
  PrimitiveTypeToString.insert(
      std::make_pair(PrimitiveType::PrimitiveType::Int, TYPE_INT));
  PrimitiveTypeToString.insert(
      std::make_pair(PrimitiveType::PrimitiveType::Int64, TYPE_INT64));
  PrimitiveTypeToString.insert(
      std::make_pair(PrimitiveType::PrimitiveType::Double, TYPE_DOUBLE));
  PrimitiveTypeToString.insert(
      std::make_pair(PrimitiveType::PrimitiveType::Float, TYPE_FLOAT));
  PrimitiveTypeToString.insert(
      std::make_pair(PrimitiveType::PrimitiveType::Text, TYPE_TEXT));
  PrimitiveTypeToString.insert(
      std::make_pair(PrimitiveType::PrimitiveType::Name, TYPE_NAME));
  PrimitiveTypeToString.insert(
      std::make_pair(PrimitiveType::PrimitiveType::String, TYPE_STRING));
}

udocs_processor::TypeExporter::TypeExporter(const std::string& DefaultValue,
    std::function<void(TypeExporter&)> Callback)
    : TypeExporter(DefaultValue) {
  this->Callback = std::move(Callback);
}
