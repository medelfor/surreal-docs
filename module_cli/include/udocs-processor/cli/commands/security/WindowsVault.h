/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <string>
#include "Vault.h"

namespace udocs_processor {
class WindowsVault : public Vault {
 public:
  void SaveSecret(const std::string &Id, const std::string &UserName,
      std::string Secret) override;

  std::string LoadSecret(const std::string &Id) override;
};
}  // namespace udocs_processor
