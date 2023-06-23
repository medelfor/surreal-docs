/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <string>
#include <vector>
#include <functional>

namespace udocs_processor {
class ExecStreamHelper {
 public:
  struct ProcessResult {
    int ExitCode = 0;
    std::string Output;
  };

  static ProcessResult Run(const std::vector<std::string>& Arguments);

 private:
  static constexpr const char* NEWLINE = "\n";
  static constexpr const char* ARGUMENTS_JOINER = "\" \"";
};
}  // namespace udocs_processor
