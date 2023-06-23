/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "View.h"

namespace udocs_processor {
class SimpleView : public virtual View {
 protected:
  virtual void ReportSuccess(const std::string& Message) = 0;

  virtual void ReportError(const std::string& Message) = 0;

  virtual void ReportWarning(const std::string& Message) = 0;

  virtual void ReportProgress(const std::string& Message) = 0;

  virtual void ShowList(const std::vector<std::string>& List) = 0;
};
}  // namespace udocs_processor
