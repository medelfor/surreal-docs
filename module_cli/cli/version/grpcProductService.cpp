/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <fmt/format.h>
#include "udocs-processor/cli/version/grpcProductService.h"
#include "api/products/Product.pb.h"

udocs_processor::ProductService::LatestVersionResponse
    udocs_processor::grpcProductService::GetLatestVersion(
        const std::string &ProductId) {
  grpc::ClientContext Context;

  api::products::LatestVersionRequest Request;
  Request.set_productid(ProductId);

  api::products::LatestVersionResponse Response;
  grpc::Status Status = Product->GetLatestVersion(
      &Context, Request, &Response);

  LatestVersionResponse Result;
  if (!Status.ok()) {
    Result.Status = {ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call {}", Status.error_code())};
    return Result;
  }

  Result.Status = {Response.status().code(), Response.status().message()};
  Result.Version = Response.latestversioncombined();
  Result.BuildNumber = Response.buildnumber();
  Result.Major = Response.major();
  Result.Minor = Response.minor();
  Result.Index = Response.index();

  return Result;
}

udocs_processor::grpcProductService::grpcProductService(
    std::shared_ptr<grpc::Channel> Channel) {
  Product = std::make_unique<api::products::Product::Stub>(Channel);
}

udocs_processor::ProductService::GetBroadcastResponse
    udocs_processor::grpcProductService::GetBroadcast(
        const std::string &ProductId) {
  grpc::ClientContext Context;

  api::products::GetBroadcastRequest Request;
  Request.set_productid(ProductId);

  api::products::GetBroadcastResponse Response;
  grpc::Status Status = Product->GetBroadcast(
      &Context, Request, &Response);

  GetBroadcastResponse Result;
  if (!Status.ok()) {
    Result.Status = {ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call {}", Status.error_code())};
    return Result;
  }

  Result.Status = {Response.status().code(), Response.status().message()};
  Result.Messages.reserve(Response.messages().size());
  for (auto& Message : *Response.mutable_messages()) {
    Result.Messages.emplace_back(BroadcastMessage{
      std::move(*Message.mutable_payload()),
      Message.id(),
      Message.format()
    });
  }

  return Result;
}
