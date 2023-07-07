/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <string>

namespace udocs_processor {
class CliHelper {
 public:
  struct Location {
    std::string Organization;
    std::string Project;
  };

  static Location ParseLocation(const std::string& LocationString);

 private:
  static constexpr char ORGANIZATION_PROJECT_SEPARATOR = '/';
};
}  // namespace udocs_processor
