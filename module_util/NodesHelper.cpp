/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#define NOLINT()

#include "udocs-processor/NodesHelper.h"
#include <fmt/format.h>
#include <regex> NOLINT()
#include "udocs-processor/doctree/native/NativePropertyNode.h"
#include "udocs-processor/doctree/native/NativeModuleNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintModuleNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintDelegateTypeNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintPropertyNode.h"
#include "udocs-processor/doctree/native/DelegateTypeNode.h"
#include "udocs-processor/doctree/NodeTypeChecker.h"
#include "udocs-processor/StringHelper.h"

bool udocs_processor::NodesHelper::IsGlobalVariable(
    const NativePropertyNode& Node) {
  if (auto Parent = Node.GetParent().lock()) {
    if (NodeTypeChecker<NativeModuleNode>(*Parent).Matched()) {
      return true;
    }
  }

  return false;
}

std::shared_ptr<udocs_processor::DocNode>
    udocs_processor::NodesHelper::FindModule(DocNode& Node) {
  std::shared_ptr<DocNode> CurrentNode = Node.shared_from_this();

  while (CurrentNode && !(
      NodeTypeChecker<BlueprintModuleNode>(*CurrentNode).Matched() ||
          NodeTypeChecker<NativeModuleNode>(*CurrentNode).Matched())) {
    CurrentNode = CurrentNode->GetParent().lock();
  }

  return CurrentNode;
}

void udocs_processor::NodesHelper::ExtractLegacyUeEnum(
    const std::string& Namespace, std::string& TrueNamespace,
    std::string& NameNamespace) {
  size_t NAMESPACE_SEPARATOR_LEN = strlen(NAMESPACE_SEPARATOR);
  size_t Pos = Namespace.rfind(NAMESPACE_SEPARATOR);
  if (Pos != std::string::npos &&
      Pos + NAMESPACE_SEPARATOR_LEN < Namespace.size()) {
    NameNamespace = Namespace.substr(
        Pos + NAMESPACE_SEPARATOR_LEN);
    TrueNamespace = Namespace.substr(0, Pos);
  } else {
    NameNamespace = Namespace;
    TrueNamespace = "";
  }
}

std::string udocs_processor::NodesHelper::FormatNumber(double Value) {
  std::string Formatted = fmt::format("{}", Value);

  if (Formatted.find(FLOATING_POINT) == std::string::npos) {
    Formatted += INTEGER_POSTFIX;
  }

  return Formatted;
}

bool udocs_processor::NodesHelper::ParseRotatorInitializer(
    const std::string& Rotator, double& X, double& Y, double& Z) {
  static std::regex Pattern(ROTATOR_FORMAT, std::regex::icase);
  std::smatch Match;
  if (std::regex_match(Rotator, Match, Pattern)) {
    Y = StringHelper::SafeStof(Match[1].str());  // pitch is Y
    Z = StringHelper::SafeStof(Match[2].str());  // yaw is Z
    X = StringHelper::SafeStof(Match[3].str());  // roll is X

    return true;
  }

  return false;
}

bool udocs_processor::NodesHelper::ParseVectorInitializer(
    const std::string& Vector, double& X, double& Y, double& Z) {
  std::regex Pattern(VECTOR_FORMAT, std::regex::icase);
  std::smatch Match;
  if (std::regex_match(Vector, Match, Pattern)) {
    X = StringHelper::SafeStof(Match[1].str());
    Y = StringHelper::SafeStof(Match[2].str());
    Z = StringHelper::SafeStof(Match[3].str());

    return true;
  }

  return false;
}

bool udocs_processor::NodesHelper::ParseParameterXYZInitializer(
    const std::string& Initializer, double& X, double& Y, double& Z) {
  std::regex Pattern(XYZ_PARAMETER_FORMAT, std::regex::icase);
  std::smatch Match;
  if (std::regex_match(Initializer, Match, Pattern)) {
    X = StringHelper::SafeStof(Match[1].str());
    Y = StringHelper::SafeStof(Match[2].str());
    Z = StringHelper::SafeStof(Match[3].str());

    return true;
  }

  return false;
}

std::string udocs_processor::NodesHelper::MakeDisplayName(
    const BlueprintDelegateTypeNode& Type) {
  if (auto Counterpart = Type.GetNativeCounterpart().lock()) {
    std::string_view Name = Counterpart->GetName();
    return StringHelper::FormatByCapital(Name.empty() ? Name : Name.substr(1),
        {StringHelper::SPACE_CH});
  } else {
    // the only possible case here is that it's type of a property (event disp.)
    if (auto Property = Type.GetParent().lock()) {
      NodeTypeChecker<BlueprintPropertyNode> IsProp(*Property);
      if (IsProp.Matched()) {
        return IsProp.GetNode()->GetDisplayName();
      }
    }
  }

  return "";
}
