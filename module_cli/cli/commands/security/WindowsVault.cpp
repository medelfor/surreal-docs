/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/commands/security/WindowsVault.h"
#include <windows.h>
#include <wincred.h>
#include <fmt/format.h>
#include <stdexcept>

void udocs_processor::WindowsVault::SaveSecret(const std::string &Id,
    const std::string &UserName, std::string Secret) {
  DWORD Length = Secret.size();
  std::wstring TargetName{Id.begin(), Id.end()};
  std::wstring UserNameW{UserName.begin(), UserName.end()};
  CREDENTIALW Credential{};
  Credential.Type = CRED_TYPE_GENERIC;
  Credential.Persist = CRED_PERSIST_LOCAL_MACHINE;
  Credential.TargetName = TargetName.data();
  Credential.CredentialBlobSize = Length;
  Credential.UserName = UserNameW.data();
  Credential.CredentialBlob = reinterpret_cast<LPBYTE>(Secret.data());

  BOOL Success = CredWriteW(&Credential, 0);
  if (!Success) {
    throw std::runtime_error{fmt::format("Unable to write the credentials: {}",
        GetLastError())};
  }
}

std::string udocs_processor::WindowsVault::LoadSecret(
    const std::string &Id) {
  std::wstring TargetName{Id.begin(), Id.end()};
  PCREDENTIALW Credential{};
  BOOL Success = CredReadW(TargetName.c_str(), CRED_TYPE_GENERIC, 0,
      &Credential);
  if (!Success) {
    throw std::runtime_error{fmt::format("Unable to read the credentials: {}",
        GetLastError())};
  }

  std::string Secret(reinterpret_cast<char*>(Credential->CredentialBlob),
      Credential->CredentialBlobSize);

  return Secret;
}
