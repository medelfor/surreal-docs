/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "SSLInitializer.h"
#include <Poco/Net/NetSSL.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/RejectCertificateHandler.h>

bool udocs_processor::SSLInitializer::Initialize(
    const std::string& CaCert) const {
  try {
    Poco::Net::initializeSSL();

    Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrCert =
        new Poco::Net::RejectCertificateHandler(false);
    Poco::Net::Context::Ptr ptrContext = new Poco::Net::Context(
        Poco::Net::Context::CLIENT_USE, "", "", CaCert,
        Poco::Net::Context::VERIFY_STRICT, SSL_VERIFICATION_DEPTH,
        DO_USE_DEFAULT_SSL_CERTS, SSL_CIPHERS);

    Poco::Net::SSLManager::instance().initializeClient(
        nullptr, ptrCert, ptrContext);
  } catch (Poco::Exception &Exc) {
    l->error("Error when configuring SSL: {}", Exc.displayText());

    return false;
  } catch (std::exception &exc) {
    l->error("Error when configuring SSL: {}", exc.what());

    return false;
  }

  return true;
}

udocs_processor::SSLInitializer::SSLInitializer(
    std::shared_ptr<spdlog::logger> l) : l(l) {}

void udocs_processor::SSLInitializer::Deinitialize() const {
  Poco::Net::uninitializeSSL();
}
