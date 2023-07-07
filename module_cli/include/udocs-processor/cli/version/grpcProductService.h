/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <grpcpp/channel.h>
#include <memory>
#include <string>
#include "ProductService.h"
#include "api/products/Product.grpc.pb.h"

namespace udocs_processor {
class grpcProductService : public ProductService {
 public:
  explicit grpcProductService(std::shared_ptr<grpc::Channel> Channel);

  LatestVersionResponse GetLatestVersion(const std::string &ProductId) override;

  GetBroadcastResponse GetBroadcast(const std::string &ProductId) override;

 private:
  std::unique_ptr<api::products::Product::Stub> Product;
};
}  // namespace udocs_processor
