/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <string>
#include <optional>

namespace udocs_processor {
class Vault {
 public:
  virtual void SaveSecret(const std::string& Id,
    const std::string& UserName, std::string Secret) = 0;

  virtual std::string LoadSecret(const std::string& Id) = 0;
};
}  // namespace udocs_processor
