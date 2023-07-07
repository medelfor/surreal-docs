/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include <udocs-processor/Scopes.h>
#include <udocs-processor/services/impl/QuotaServiceImpl.h>

udocs_processor::QuotaServiceImpl::QuotaServiceImpl(
    std::shared_ptr<spdlog::sinks::sink> Sink,
    std::shared_ptr<grpc::Channel> Channel) {
  Quota = std::make_unique<surapi::Quota::Stub>(Channel);
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}

udocs_processor::QuotaService::ListQuotaResponse
    udocs_processor::QuotaServiceImpl::List(const std::string &Token) const {
  grpc::ClientContext Context;
  surapi::ListQuotaRequest ListRequest;
  ListRequest.set_token(Token);

  surapi::ListQuotaResponse Response;
  grpc::Status Status = Quota->List(&Context, ListRequest,
      &Response);
  if (!Status.ok()) {
    return {{ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (list quota) {}",
            Status.error_code())}};
  }

  std::vector<QuotaService::QuotaResource> Data;
  Data.reserve(Response.resources().size());
  for (auto& Resource : *Response.mutable_resources()) {
    QuotaService::QuotaResource QuotaResource{};
    QuotaResource.Name = std::move(*Resource.mutable_name());
    if (Resource.has_max()) {
      QuotaResource.Max = Resource.max();
    }
    QuotaResource.Used = Resource.used();
    Data.emplace_back(std::move(QuotaResource));
  }
  return {{Response.status().code(), Response.status().message()},
      std::move(Data)};
}
