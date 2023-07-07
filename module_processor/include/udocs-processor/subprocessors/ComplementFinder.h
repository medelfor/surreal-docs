/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <set>
#include <unordered_set>
#include <map>
#include "Subprocessor.h"
#include "udocs-processor/NodeHasher.h"

namespace udocs_processor {
class ReferenceCache;
class NodeTypeCheckerBase;
class IdGenerator;
class CrossReferencer;

/*
 * this class pretty freely operates raw and weak pointers because it's
 * guaranteed that no nodes will be freed while this class processes the tree
 */
class ComplementFinder : public Subprocessor {
 public:
  explicit ComplementFinder(std::unique_ptr<NodeHasher> Hasher,
      IdGenerator &IdGen);

  void SetCache(ReferenceCache* Cache);

  void SetCrossReferencer(CrossReferencer* Referencer);

  std::string_view GetName() const override;

  bool Before() override;

  int Visit(EventOwningClassNode &Node) override;

  int Visit(EventParentClassNode &Node) override;

  int Visit(ClassParentNode &Node) override;

  int Visit(NativeClassNode &Node) override;

  int Visit(NativeFunctionNode &Node) override;

  int Visit(BlueprintClassNode &Node) override;

  int Visit(BlueprintEnumNode &Node) override;

  int Visit(BlueprintFunctionNode &Node) override;

  int Visit(BlueprintModuleNode &Node) override;

  int Visit(BlueprintPropertyNode &Node) override;

  int Visit(BlueprintStructNode &Node) override;

  int Visit(BlueprintDelegateTypeNode &Node) override;

  int Visit(BlueprintClassParentNode &Node) override;

  int Visit(DocsPageNode &Node) override;

  void After() override;

 private:
  void PostProcessNativeInterfaces();

  template<typename N, typename B>
  bool SetCounterparts(B& BlueprintEntity,
      const std::string& Postfix, const std::string& Prefix,
      const std::function<bool(N&)>& Decider,
      bool DoTakeIntoAccountDeprecation = false);

  template<typename N, typename B>
  bool SetCounterparts(B& BlueprintEntity,
      const std::string& Postfix, const std::string& Prefix,
      const std::function<bool(N&)>& Decider,
      std::set<DocNode*>& AllowedParents,
      bool DoTakeIntoAccountDeprecation = false);

  DocNode* FindModule(DocNode& Node) const;

  std::string MakeFullName(
      const std::string& Namespace, const std::string& Name) const;

  std::unordered_set<NativeFunctionNode*> ComplementsProcessed;

  void FindCanonicalPages();

  void ProcessFunctions();

  void ProcessBlueprintFunction(BlueprintFunctionNode& Node);

  void FindComplementaryFunctions(NativeFunctionNode& Node);

  void FindSuperFunction(NativeFunctionNode& Node);

  void FindSuperFunction(BlueprintFunctionNode& Node);

  void CopyUDataFromSuperFunction(NativeFunctionNode& Node);

  template<typename F, typename C>
  void SetupInterfaceImplementations(F& Node, std::unordered_set<F*>& Cache);

  std::set<DocNode*> FindInterfacesAndContainerCounterparts(
      BlueprintFunctionNode& Node);

  static const size_t InvalidIndex = static_cast<size_t>(~0);

  template<typename T>
  struct HierarchyEntry {
    T* Class = nullptr;
    size_t Index = InvalidIndex;
    size_t Previous = InvalidIndex;

    HierarchyEntry(T *Class, size_t Index, size_t Previous);
  };

  template<typename T>
  using HierarchyType = std::vector<std::shared_ptr<HierarchyEntry<T>>>;

  template<typename T>
  using AscendingHandler =
      std::function<bool(T&, HierarchyType<T>&)>;

  void SetSuperFunction(NativeFunctionNode& Where, NativeFunctionNode& What);

  bool FindVirtualBaseFunction(NativeClassNode& Class,
      NativeFunctionNode& Terminal,
      std::map<NativeClassNode*, NativeFunctionNode*>& Overrides,
      const std::string& Name, uint32_t Signature,
      bool& IsNativeImpl, size_t& LastVirtBaseIndex,
      size_t LastIndexInHierarchy);

  template<typename T, typename P>
  void AscendHierarchy(T& Start,
      const AscendingHandler<T>& Handler,
      bool HandleStart = false);

  // is IINTERFACE
  bool IsClassInterface(NativeClassNode& Node) const;

  bool IsClassInterface(BlueprintClassNode& Node) const;

  constexpr static char UDOCS_PROCESSOR_LOGGER_NAME[] = "udocs-processor";
  constexpr static char SUBPROCESSOR_NAME[] = "Complement Finder";
  constexpr static char NAMESPACE_SEPARATOR[] = "::";
  constexpr static char FUNCTION_POSTFIX[] = "()";

  constexpr static char IINTERFACE_PREFIX[] = "I";
  constexpr static char STRUCT_PREFIX[] = "F";
  constexpr static char CLASS_PREFIX[] = "U";
  constexpr static char ACTOR_PREFIX[] = "A";
  constexpr static char UCLASS_DEPRECATED_PREFIX[] = "DEPRECATED_";
  // function(?), property
  constexpr static char UMEMBER_DEPRECATED_POSTFIX[] = "_DEPRECATED";

  std::unique_ptr<NodeHasher> Hasher;
  ReferenceCache* Cache = nullptr;
  CrossReferencer* mCrossReferencer = nullptr;
  bool IsModulesCacheFilled = false;
  std::map<std::string, NativeModuleNode*> ModulesCache;
  std::unordered_set<NativeFunctionNode*> NativeFunctionsToProcess;
  std::unordered_set<BlueprintFunctionNode*> BlueprintFunctionsToProcess;
  std::unordered_set<NativeFunctionNode*> FunctionWithCopiedUData;
  std::unordered_set<NativeFunctionNode*>
      NativeFunctionWithSetupInterfaceImplementations;
  std::unordered_set<BlueprintFunctionNode*>
      BpFunctionWithSetupInterfaceImplementations;
  std::vector<DocsPageNode*> NonCanonicalPages;
  std::unordered_map<std::string, DocsPageNode*> CanonicalPages;
  std::set<NativeClassNode*> UInterfaces;
  std::shared_ptr<spdlog::logger> l;
  IdGenerator &IdGen;
};
}  // namespace udocs_processor
