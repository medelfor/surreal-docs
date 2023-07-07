/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

namespace udocs_processor {
// OAuth2
class SurrealApiScopes {
 public:
  static constexpr char TOKENS_WRITE[] = "tokens_write";
  static constexpr char TOKENS_READ[] = "tokens_read";
};

// Surreal Cloud Tokens
class SurrealCloudScopes {
 public:
  static constexpr char PROJECTS_WRITE[] = "project:write";
  static constexpr char PROJECTS_READ[] = "project:read";
  static constexpr char ORGANIZATIONS_WRITE[] = "org:write";
  static constexpr char ORGANIZATIONS_READ[] = "org:read";
  static constexpr char QUOTAS_READ[] = "quota:read";
  static constexpr char DOCUMENTS_WRITE[] = "document:write";
};
}  // namespace udocs_processor
