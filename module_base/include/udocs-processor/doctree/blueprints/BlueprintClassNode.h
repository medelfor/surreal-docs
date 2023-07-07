/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <memory>
#include <string>
#include "NativeClassCounterpart.h"
#include "udocs-processor/doctree/HasCounterpart.h"

namespace udocs_processor {
class NativeClassNode;

enum class ClassType {
  Class,
  Widget,
  FunctionLibrary,
  Component,
  Actor,
  DeveloperSettings,
  Subsystem
};

class BlueprintClassNode
    : public NativeClassCounterpart, public HasCounterpart {
 public:
  BlueprintClassNode(std::weak_ptr<DocNode> Parent, uint64_t Id);

  std::weak_ptr<DocNode> GetCounterpart() const override;

  std::weak_ptr<NativeClassNode> GetNativeCounterpart() const;

  void SetNativeCounterpart(std::weak_ptr<NativeClassNode> NativeCounterpart);

  bool IsBlueprintType() const;

  void SetIsBlueprintType(bool IsBlueprintType);

  bool IsBlueprintable() const;

  void SetIsBlueprintable(bool IsBlueprintable);

  bool IsAbstract() const;

  void SetIsAbstract(bool IsAbstract);

  bool IsInterface() const;

  void SetIsInterface(bool IsInterface);

  ClassType GetClassType() const;

  void SetClassType(ClassType Type);

  const std::string& GetNamespace() const;

  void SetNamespace(std::string Namespace);

  const std::string& GetConfigName() const;

  void SetConfigName(std::string ConfigName);

  int Accept(DocNodeVisitor &Visitor) override;

  ~BlueprintClassNode() override = default;

 private:
  std::weak_ptr<NativeClassNode> NativeCounterpart;
  bool mIsAbstract = false;
  bool mIsInterface = false;
  bool mIsBlueprintType = false;
  bool mIsBlueprintable = false;
  ClassType Type = ClassType::Class;
  std::string Namespace;
  std::string ConfigName;
};
}  // namespace udocs_processor

