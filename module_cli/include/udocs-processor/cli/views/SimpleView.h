/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "View.h"

namespace udocs_processor {
class SimpleView : public virtual View {
 public:
  virtual void ReportError(const std::string& Message) = 0;

  virtual void ReportWarning(const std::string& Message) = 0;

  virtual void ShowDescription(const std::string& Description) = 0;

  virtual void ShowList(std::vector<std::string> List) = 0;

  ~SimpleView() override = default;

 protected:
  virtual bool Confirm(const std::string& Message) = 0;

  virtual void ReportProgress(const std::string& Message) = 0;

  virtual void ReportSuccess(const std::string& Message) = 0;
};
}  // namespace udocs_processor
