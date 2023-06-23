/* Copyright © 2023, Medelfor, Limited. All rights reserved. */

#pragma once


#include <grpcpp/channel.h>
#include "udocs-processor/services/DocumentServiceFactory.h"

namespace udocs_processor {
class DocumentServiceFactoryImpl : public DocumentServiceFactory {
 public:
  DocumentServiceFactoryImpl(std::shared_ptr<grpc::Channel> Channel);

  std::shared_ptr<DocumentService> Make() const override;

 private:
  std::shared_ptr<grpc::Channel> Channel;
};
}  // namespace udocs_processor
