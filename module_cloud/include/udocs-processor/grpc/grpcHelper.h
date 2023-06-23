/*
 Copyright © 2023, Kommpute Entertainment™ Sp. z o.o. All rights reserved.
 For any additional information refer to https://kommpute.com
 */

#pragma once

#include <udocs-processor//ApiStatus.h>
#include <grpcpp/support/status.h>
#include "api/Status.pb.h"

namespace udocs_processor {
class grpcHelper {
 public:
  static api::Status* ToStatus(const ApiStatus& Status);

  static ApiStatus ToStatus(const grpc::Status& Status);

 private:
};
}  // namespace surapi
