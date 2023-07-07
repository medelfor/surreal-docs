/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/services/impl/DocumentServiceImpl.h"

void udocs_processor::DocumentServiceImpl::Add(
    AddDocumentRequest &Request) {
  AddRequest.set_token(Request.Token);
  AddRequest.set_project(Request.Project);
  AddRequest.set_organization(Request.Organization);
  AddRequest.set_version(Request.Version);
  AddRequest.set_docpath(Request.DocPath);
  AddRequest.set_content(std::move(Request.Content));

  if (auto It = FormatsToFormats.find(Request.Format);
      It != FormatsToFormats.end()) {
    AddRequest.set_format(It->second);
  } else {
    l->error("Undefined format {}", static_cast<uint32_t>(Request.Format));
    throw std::invalid_argument{"Invalid format of document"};
  }

  if (auto It = TypesToTypes.find(Request.Type);
      It != TypesToTypes.end()) {
    AddRequest.set_type(It->second);
  } else {
    l->error("Undefined type {}", static_cast<uint32_t>(Request.Type));
    throw std::invalid_argument{"Invalid type of document"};
  }

  if (!Writer) {
    Writer = Document->Add(&Context, &Response);
  }

  bool Success = Writer->Write(AddRequest);
  if (!Success) {
    throw std::runtime_error{"Couldn't write an entry to the cloud"};
  }
}

udocs_processor::DocumentServiceImpl::DocumentServiceImpl(
    std::shared_ptr<spdlog::sinks::sink> Sink,
    std::shared_ptr<grpc::Channel> Channel) {
  Document = std::make_unique<surapi::Document::Stub>(Channel);
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }

  FormatsToFormats.insert(std::make_pair(ManifestEntry::Format::JSON_V1,
      surapi::DocumentFormat::JSON_V1));
  FormatsToFormats.insert(std::make_pair(ManifestEntry::Format::DEBUG_JSON_V1,
      surapi::DocumentFormat::DEBUG_JSON_V1));
  FormatsToFormats.insert(std::make_pair(ManifestEntry::Format::INDEX_JSON_V1,
      surapi::DocumentFormat::INDEX_JSON_V1));
  FormatsToFormats.insert(std::make_pair(ManifestEntry::Format::PNG,
      surapi::DocumentFormat::PNG));
  FormatsToFormats.insert(std::make_pair(ManifestEntry::Format::PNG_BASE64,
      surapi::DocumentFormat::PNG_BASE64));

  TypesToTypes.insert(std::make_pair(ManifestEntry::Type::PAGE,
      surapi::DocumentType::PAGE));
  TypesToTypes.insert(std::make_pair(ManifestEntry::Type::INDEX,
      surapi::DocumentType::INDEX));
  TypesToTypes.insert(std::make_pair(ManifestEntry::Type::RESOURCE,
      surapi::DocumentType::RESOURCE));
  TypesToTypes.insert(std::make_pair(ManifestEntry::Type::DEBUG,
      surapi::DocumentType::DEBUG));
}

udocs_processor::ApiStatus udocs_processor::DocumentServiceImpl::Finish() {
  if (!Writer) throw std::runtime_error{"Writer isn't initialized"};

  bool Ok = Writer->WritesDone();
  l->info("Writes done {}", Ok);
  grpc::Status Status = Writer->Finish();
  if (!Status.ok()) {
    return {ApiStatus::GRPC_LAYER_FAILED,
        fmt::format("Couldn't process RPC call (add document) {}",
            Status.error_code())};
  }

  return {Response.code(), Response.message()};
}
