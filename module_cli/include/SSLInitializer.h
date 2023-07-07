/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <string>
#include <memory>

namespace udocs_processor {
class SSLInitializer {
 public:
  explicit SSLInitializer(std::shared_ptr<spdlog::logger> l);

  bool Initialize(const std::string& CaCert) const;

  void Deinitialize() const;

 private:
  constexpr static char SSL_CIPHERS[] = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
  constexpr static bool DO_USE_DEFAULT_SSL_CERTS = false;
  constexpr static int SSL_VERIFICATION_DEPTH = 9;

  std::shared_ptr<spdlog::logger> l;
};
}  // namespace udocs_processor
