/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <spdlog/spdlog.h>
#include "udocs-processor/cli/commands/QuotaListCommand.h"

udocs_processor::QuotaListCommand::QuotaListCommand(
    std::shared_ptr<QuotaService> Quota,
    std::shared_ptr<spdlog::sinks::sink> Sink) : Quota(Quota) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}

std::vector<udocs_processor::QuotaService::QuotaResource>
    udocs_processor::QuotaListCommand::List(
        const ListRequest &Request) const {
  QuotaService::ListQuotaResponse Response = Quota->List(Request.Token);
  if (Response.Status.GetCode() != ApiStatus::SUCCESS) {
    l->error("List quota: {}/{}", Response.Status.GetCode(),
        Response.Status.GetMessageDescription());
    throw std::runtime_error{fmt::format("Couldn't list quota: {}",
        Response.Status.GetMessageDescription())};
  }

  l->info("Successfuly listed quota");

  return Response.Resources;
}
