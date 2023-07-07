/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "SimpleView.h"

namespace udocs_processor {
class OrganizationListView : public virtual SimpleView {
 public:
  ~OrganizationListView() override = default;

  virtual void ShowOrganizations(
      const std::vector<std::string>& Organizations) = 0;
};
}  // namespace udocs_processor
