/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/NodeHasher.h"

#define NOLINT()

#include <crc32c/crc32c.h>
#include <regex> NOLINT()
#include "udocs-processor/doctree/NodeTypeChecker.h"
#include "udocs-processor/doctree/native/NativeMemberNode.h"
#include "udocs-processor/doctree/native/DelegateTypeNode.h"
#include "udocs-processor/doctree/native/LogCategoryNode.h"
#include "udocs-processor/doctree/native/NativeClassNode.h"
#include "udocs-processor/doctree/native/NativeEnumNode.h"
#include "udocs-processor/doctree/native/NativeFunctionNode.h"
#include "udocs-processor/doctree/native/NativeModuleNode.h"
#include "udocs-processor/doctree/native/NativePropertyNode.h"
#include "udocs-processor/doctree/native/NativeParameterNode.h"
#include "udocs-processor/doctree/native/NativeTypeNode.h"
#include "udocs-processor/doctree/native/TemplateNode.h"
#include "udocs-processor/doctree/ReferencableWordNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintUNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintFunctionNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintModuleNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintClassNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintEnumNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintStructNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintPropertyNode.h"
#include "udocs-processor/doctree/blueprints/BlueprintParameterNode.h"
#include "udocs-processor/doctree/blueprints/types/BlueprintDelegateTypeNode.h"
#include "udocs-processor/StringHelper.h"

/*
 * Basically, when hashing for purposes of including the hash into doc path
 * there's always choice between two things:
 * 1) if hash is generated by bigger amount of things then it's more breakable,
 * hence more changes by programmer (e.g. renaming a folder in a path) can break
 * permalinks
 * 2) if hash is generated by lesser amount of things then it's less breakable
 * but has bigger chances of clash
 *
 * The policy here is as follows: always prefer lesser breakability over lesser
 * chances of clash
 * */

int udocs_processor::NodeHasher::Visit(DelegateTypeNode &Node) {
  HashIt(HashByPath(Node));

  return 1;
}

int udocs_processor::NodeHasher::Visit(LogCategoryNode &Node) {
  HashIt(HashByPath(Node));

  return 1;
}

// path + template
int udocs_processor::NodeHasher::Visit(NativeClassNode &Node) {
  HashIt(HashByPath(Node) + HashByTemplate(Node));

  return 1;
}

int udocs_processor::NodeHasher::Visit(NativeEnumNode &Node) {
  HashIt(HashByPath(Node));

  return 1;
}

// template + params + function flags
int udocs_processor::NodeHasher::Visit(NativeFunctionNode &Node) {
  HashIt(HashByFullSignature(Node));

  return 1;
}

int udocs_processor::NodeHasher::Visit(NativeModuleNode &Node) {
  HashIt(HashByPath(Node));

  return 1;
}

int udocs_processor::NodeHasher::Visit(NativePropertyNode &Node) {
  HashIt(HashByPath(Node));

  return 1;
}

int udocs_processor::NodeHasher::Visit(BlueprintClassNode &Node) {
  HashIt(HashByPath(Node));

  return 1;
}

int udocs_processor::NodeHasher::Visit(BlueprintEnumNode &Node) {
  HashIt(HashByPath(Node));

  return 1;
}

int udocs_processor::NodeHasher::Visit(BlueprintFunctionNode &Node) {
  HashIt(HashByPath(Node));

  return 1;
}

int udocs_processor::NodeHasher::Visit(BlueprintModuleNode &Node) {
  HashIt(HashByPath(Node));

  return 1;
}

int udocs_processor::NodeHasher::Visit(BlueprintPropertyNode &Node) {
  HashIt(HashByPath(Node));

  return 1;
}

int udocs_processor::NodeHasher::Visit(BlueprintDelegateTypeNode &Node) {
  if (auto Counterpart = Node.GetNativeCounterpart().lock()) {
    // only those delegates are supported who have counterparts
    HashIt(HashByPath(*Counterpart));
  }

  return 1;
}

int udocs_processor::NodeHasher::Visit(BlueprintStructNode &Node) {
  HashIt(HashByPath(Node));

  return 1;
}

uint32_t udocs_processor::NodeHasher::HashNode(DocNode &Node) {
  Hash = 0;
  Node.Accept(*this);

  return Hash;
}

void udocs_processor::NodeHasher::HashIt(const std::string& ToHash) {
  Hash = crc32c::Crc32c(ToHash);
}

std::string udocs_processor::NodeHasher::HashByPath(DocNode &Node) {
  // header path + name or /script/path + name
  return Node.GetPath() + PATH_AND_NAME_SEPARATOR +
      Node.GetName();
}

std::string udocs_processor::NodeHasher::HashByTemplate(DocNode& Node) const {
  // find template (the node always exists, can have no children)
  // and generate template part of signature
  class TemplateFinder : public DocNodeVisitor {
   public:
    explicit TemplateFinder(const NodeHasher &This)
        : This(This) {}

    int Visit(TemplateNode &Node) override {
      Signature = This.HashBySignature(Node);

      return 0;
    }

    const NodeHasher& This;
    std::string Signature;
  } TemplateFinderInstance(*this);
  Node.IterateChildren(TemplateFinderInstance, false);

  return TemplateFinderInstance.Signature;
}

std::string udocs_processor::NodeHasher::HashByFullSignature(
    NativeFunctionNode& Node) const {
  std::string ReturnType = FullStringifyType(Node);
  std::string Signature = ReturnType + SIGNATURE_RET_TYPE_DELIMITER +
      HashByTemplate(Node) + SIGNATURE_TEMPLATE_DELIMITER +
      HashBySignature(Node) + std::to_string(Node.IsConst());

  return Signature;
}

template<typename T>
std::string udocs_processor::NodeHasher::HashBySignature(
    T& Node) const {
  class ParamFinder : public DocNodeVisitor {
   public:
    explicit ParamFinder(const NodeHasher &This) : This(This) {}

    int Visit(NativeParameterNode &Node) override {
      Signature += (This.FullStringifyType(Node) + SIGNATURE_TYPE_DELIMITER);

      return 0;
    }

    const NodeHasher& This;
    std::string Signature;
  } ParamFinderInstance(*this);
  Node.IterateChildren(ParamFinderInstance, false);

  return std::move(ParamFinderInstance.Signature);
}

std::string udocs_processor::NodeHasher::FullStringifyType(
    DocNode& Node) const {
  // this will not glue two keywords/identifiers into a single one
  // but will remove any spaces that can be safely removed
  std::string StringifiedType = StringifyType(Node);
  // replace spaces on # in situations where spaces connect two keywords
  // or identifiers like: typename ClassX -> typename#ClassX
  StringifiedType = std::regex_replace(StringifiedType, Separation,
      SIGNATURE_SEPARATION_REPLACE_PATTERN);

  // remove any other spaces
  StringHelper::ReplaceAll(StringifiedType, SPACE, "");

  return StringifiedType;
}

std::string udocs_processor::NodeHasher::StringifyType(
    DocNode& Node) const {
  // find type
  class TypeFinder : public DocNodeVisitor {
   public:
    int Visit(NativeTypeNode &Node) override {
      Type = &Node;

      return 0;
    }

    NativeTypeNode* Type = nullptr;
  } TypeFinderInstance;
  Node.IterateChildren(TypeFinderInstance, false);

  if (TypeFinderInstance.Type) {
    // concat all referencable words
    class WordsFinder : public DocNodeVisitor {
     public:
      int Visit(ReferencableWordNode &Node) override {
        Result += Node.GetWord();

        return 0;
      }

      std::string Result;
    } WordsFinderInstance;
    TypeFinderInstance.Type->IterateChildren(WordsFinderInstance, false);

    return WordsFinderInstance.Result;
  }

  // actually, this can lead to unexpected results if at the end two signatures
  // match because none of their parameters could get stringified
  return "";
}

udocs_processor::NodeHasher::NodeHasher() {
  Separation = std::regex(SIGNATURE_SEPARATION_PATTERN, std::regex::icase);
}
