/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "SimpleView.h"
#include "FtxView.h"

namespace udocs_processor {
class FtxSimpleView : public virtual SimpleView, public virtual FtxView {
 public:
  void Init() override;

  void Tick() override;

  void Start() override;

  void Destroy() override;

  bool DoExit() const override;

  void SetFinished(bool IsFinished) override;

 protected:
  void ReportSuccess(const std::string &Message) override;

  void ReportError(const std::string &Message) override;

  void ReportWarning(const std::string &Message) override;

  void ReportProgress(const std::string &Message) override;

  void ShowList(const std::vector<std::string> &List) override;
};
}  // namespace udocs_processor
