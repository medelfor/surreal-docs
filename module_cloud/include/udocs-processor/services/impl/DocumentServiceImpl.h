/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <optional>
#include <memory>
#include <map>
#include "surapi/Document.grpc.pb.h"
#include "udocs-processor/services/DocumentService.h"

namespace udocs_processor {
class DocumentServiceImpl : public DocumentService {
 public:
  DocumentServiceImpl(std::shared_ptr<spdlog::sinks::sink> Sink,
      std::shared_ptr<grpc::Channel> Channel);

  void Add(AddDocumentRequest &Request) override;

  ApiStatus Finish() override;

 private:
  std::shared_ptr<spdlog::logger> l;
  std::unique_ptr<surapi::Document::Stub> Document;
  std::unique_ptr<grpc::ClientWriter<surapi::AddDocumentRequest>> Writer;
  grpc::ClientContext Context;
  surapi::AddDocumentRequest AddRequest;
  api::Status Response;

  std::map<udocs_processor::ManifestEntry::Format, surapi::DocumentFormat>
      FormatsToFormats;
  std::map<udocs_processor::ManifestEntry::Type, surapi::DocumentType>
      TypesToTypes;

  constexpr static char LOGGER_NAME[] = "cloud-layer";
};
}  // namespace udocs_processor
