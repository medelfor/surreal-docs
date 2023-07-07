/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <grpcpp/security/credentials.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <memory>
#include <string>
#include <optional>

namespace udocs_processor {
std::shared_ptr<grpc::Channel> CreateSurrealCloudChannel(
    const std::string& Address, std::optional<std::string> RootCert);
}  // namespace udocs_processor
