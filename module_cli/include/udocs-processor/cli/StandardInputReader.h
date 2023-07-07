/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#define NOLINT()

#include <iostream>
#include <string>
#include <sstream>
#include <future> NOLINT()

namespace udocs_processor {
class StandarInputReader {
 public:
  std::string ReadInput() const {
    std::string Line;
    std::getline(std::cin, Line);

    bool HasBom = false;
    if (Line.size() >= BOM_SIZE) {
      HasBom = true;
      for (size_t i = 0; i < BOM_SIZE; ++i) {
        if (Line[i] != BOM[i]) {
          HasBom = false;
          break;
        }
      }
    }

    return HasBom ? Line.substr(BOM_SIZE) : Line;
  }

 private:
  static constexpr const char BOM[] = "\xEF\xBB\xBF";
  static constexpr size_t BOM_SIZE = sizeof(BOM) - 1;
  static constexpr size_t WAIT_CIN_TIMEOUT_MS = 200;
};
}  // namespace udocs_processor
