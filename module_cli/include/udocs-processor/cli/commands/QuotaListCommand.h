/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <memory>

namespace udocs_processor {
class QuotaListCommand {
 public:
  struct ListRequest {};

  explicit QuotaListCommand(std::shared_ptr<spdlog::sinks::sink> Sink);

  void List(const ListRequest& Request) const;

 private:
  static constexpr const char* LOGGER_NAME = "quota-list";

  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor
