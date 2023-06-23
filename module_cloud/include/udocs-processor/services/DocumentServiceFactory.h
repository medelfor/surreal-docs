/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <memory>
#include "DocumentService.h"

namespace udocs_processor {
class DocumentServiceFactory {
 public:
  virtual std::shared_ptr<DocumentService> Make() const = 0;
};
}  // namespace udocs_processor
