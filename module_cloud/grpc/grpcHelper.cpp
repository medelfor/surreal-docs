/*
 * Copyright © 2022, Medelför OÜ. All rights reserved.
 * For any additional information refer to https://medelfor.com
 */

#include "udocs-processor/grpc/grpcHelper.h"

api::Status* udocs_processor::grpcHelper::ToStatus(const ApiStatus &Status) {
  auto* Result = new api::Status;
  Result->set_message(Status.GetMessageDescription());
  Result->set_code(Status.GetCode());
  return Result;
}

udocs_processor::ApiStatus udocs_processor::grpcHelper::ToStatus(
    const grpc::Status& Status) {
  return {ApiStatus::GRPC_LAYER_FAILED, Status.error_message()};
}
