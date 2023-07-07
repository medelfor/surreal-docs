/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/cli/util/CliHelper.h"
#include <stdexcept>

udocs_processor::CliHelper::Location udocs_processor::CliHelper::ParseLocation(
    const std::string &LocationString) {
  size_t Pos = LocationString.find(ORGANIZATION_PROJECT_SEPARATOR);
  if (Pos == std::string::npos || Pos == LocationString.size() - 1 ||
      Pos == 0) {
    throw std::invalid_argument{"Invalid location format"};
  }

  return {LocationString.substr(0, Pos), LocationString.substr(Pos + 1)};
}
