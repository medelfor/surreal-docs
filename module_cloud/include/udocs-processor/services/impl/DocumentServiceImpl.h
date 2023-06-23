/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once

#include <optional>
#include <memory>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include "surapi/Document.grpc.pb.h"
#include "udocs-processor/services/DocumentService.h"

namespace udocs_processor {
class DocumentServiceImpl : public DocumentService {
 public:
  explicit DocumentServiceImpl(std::shared_ptr<grpc::Channel> Channel);

  ApiStatus Add(AddDocumentRequest &Request) override;

  ApiStatus Finish() override;

 private:
  std::shared_ptr<spdlog::logger> l;
  std::unique_ptr<surapi::Document::Stub> Document;
  std::unique_ptr<grpc::ClientWriter<surapi::AddDocumentRequest>> Writer;

  std::map<udocs_processor::ManifestEntry::Format, surapi::DocumentFormat>
      FormatsToFormats;
  std::map<udocs_processor::ManifestEntry::Type, surapi::DocumentType>
      TypesToTypes;

  constexpr static char LOGGER_NAME[] = "medelfor-surapi";
};
}  // namespace udocs_processor
