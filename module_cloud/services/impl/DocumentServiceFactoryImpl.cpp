/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/services/impl/DocumentServiceFactoryImpl.h"
#include "udocs-processor/services/impl/DocumentServiceImpl.h"

std::shared_ptr<udocs_processor::DocumentService>
    udocs_processor::DocumentServiceFactoryImpl::Make() const {
  return std::make_shared<DocumentServiceImpl>(Sink, Channel);
}

udocs_processor::DocumentServiceFactoryImpl::DocumentServiceFactoryImpl(
    std::shared_ptr<spdlog::sinks::sink> Sink,
    std::shared_ptr<grpc::Channel> Channel) : Channel(Channel), Sink(Sink) {}
