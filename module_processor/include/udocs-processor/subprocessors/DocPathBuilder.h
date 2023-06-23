/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <random> NOLINT()
#include <memory>
#include <vector>
#include "Subprocessor.h"
#include "udocs-processor/DocPathsCache.h"
#include "udocs-processor/NodeHasher.h"

namespace udocs_processor {
// this class fills cache with links ID -> doc path of page the entity with
// such id should lead to
class DocPathBuilder : public Subprocessor {
 public:
  explicit DocPathBuilder(DocPathsCache &DocPathCache,
      std::unique_ptr<NodeHasher> Hasher);

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

  int Visit(DocsImageNode &Node) override;

  int Visit(DocsIndexNode &Node) override;

  void After() override;

  std::string_view GetName() const override;

 private:
  std::string MakeHash(std::string Seed) const;

  template<typename T>
  void AddModuleDocPath(T& Node, bool IsNative);

  template<typename M, typename N>
  void AddMemberDocPath(N& Node);

  template<typename M, typename N>
  void AddMemberDocPathImpl(N& Node, const std::string& FirstName,
      const std::string& FirstNamespace);

  std::string BuildPathFromNamespace(const std::string& Namespace) const;

  std::string GatherDocPath(DocPathsCache::IdType Id) const;

  void AddDocPath(std::string PathPart, std::shared_ptr<DocNode> Node,
      std::shared_ptr<DocNode> Parent);

  std::string MakeDocPathFromString(const std::string& Raw,
      const char* Separator) const;

  std::string MakeDocPathPart(const std::string& Identifier) const;

  std::string MakeDocPath(const std::string& FullPath) const;

  std::string MakeModuleDocPath(const DocNode& Node, bool IsNative) const;

  std::string MakeDocPath(const NativeMemberNode& Node) const;

  std::string MakeDocPath(const BlueprintUNode& Node) const;

  std::string MakeDocPath(const DocsPageNode& Node) const;

  std::string MakeDocPath(const NativeEnumNode& Node,
      std::string& TrueNamespace) const;

  std::string MakeDocPath(BlueprintDelegateTypeNode& Delegate,
      DelegateTypeNode& Counterpart) const;

  std::string MakeDelegateDocPath(BlueprintDelegateTypeNode& Delegate,
      const std::string& DelegateName) const;

  void HashNode(DocNode& Node);

  std::string HashNode(const std::string& Name, DocNode& Node) const;

  constexpr static uint32_t HASH_LENGTH = 32;

  constexpr static char SUBPROCESSOR_NAME[] = "Doc Paths Builder";

  constexpr static char DOCPATH_SEPARATOR[] = "/";
  constexpr static char DOCPATH_API_BLUEPRINT[] = "api/blueprint/";
  constexpr static char DOCPATH_API_CPP[] = "api/cpp/";
  constexpr static char DOCPATH_DOCS[] = "docs/";
  constexpr static char DOCPATH_IMAGES[] = "graphics/";
  // graphics/__generated for generated and graphics/<images_root> for
  // user-provided
  constexpr static char RESERVED_NAME_GENERATED[] = "__generated";

  constexpr static char NAMESPACE_SEPARATOR[] = "::";
  constexpr static char SPACE = ' ';
  constexpr static char SPACE_REPLACEMENT = '_';
  constexpr static char HASH_SEPARATOR[] = "_";
  constexpr static char UE_LEGACY_ENUM_SEPARATOR[] = "_";

  constexpr static char UDOCS_PROCESSOR_LOGGER_NAME[] = "udocs-processor";

  // bp delegate type nodes to respective native delegate nodes
  std::unordered_map<BlueprintDelegateTypeNode*, DelegateTypeNode*> Delegates;
  // blueprint function to native delegate type
  std::unordered_map<DocPathsCache::IdType, DocPathsCache::IdType>
    NativeDelegates;
  // blueprint function which to blueprint delegate type of
  // non-native nature
  std::unordered_map<DocPathsCache::IdType, std::shared_ptr<DocNode>>
    BlueprintDelegates;

  std::unordered_map<DocPathsCache::IdType, DocPathsCache::IdType>
      ElementsToEnums;
  std::unordered_set<DocPathsCache::IdType> ImageNodes;
  std::unordered_set<DocPathsCache::IdType> PageNodes;
  std::vector<DocsPageNode*> NonCanonicalPages;
  std::unordered_map<DocPathsCache::IdType, std::shared_ptr<DocNode>>
      IdsToNodes;
  std::unordered_map<DocPathsCache::IdType, std::string> IdsToPathParts;
  std::unordered_map<DocPathsCache::IdType, std::shared_ptr<DocNode>>
      IdsToNodeParents;
  mutable std::shared_ptr<spdlog::logger> l;
  DocPathsCache& DocPathCache;

  std::unique_ptr<NodeHasher> Hasher;
  std::shared_ptr<DocsIndexNode> Index;

  std::random_device RandomDevice;
  mutable std::mt19937 Generator;
};
}  // namespace udocs_processor
