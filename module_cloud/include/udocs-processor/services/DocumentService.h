/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <udocs-processor/ApiStatus.h>
#include <udocs-processor/serializers/JSONDocTreeSerializer.h>
#include <map>
#include <string>

namespace udocs_processor {
class DocumentService {
 public:
  struct AddDocumentRequest {
    std::string Token;
    std::string Project;
    std::string Organization;
    std::string Version;
    std::string DocPath;
    std::string Content;
    udocs_processor::ManifestEntry::Format Format =
        udocs_processor::ManifestEntry::Format::JSON_V1;
    udocs_processor::ManifestEntry::Type Type =
        udocs_processor::ManifestEntry::Type::PAGE;
  };

  virtual ApiStatus Add(AddDocumentRequest& Request) = 0;

  virtual ApiStatus Finish() = 0;

  // todo add some kind of check before deploy: access, existence, etc
};
}  // namespace udocs_processor
