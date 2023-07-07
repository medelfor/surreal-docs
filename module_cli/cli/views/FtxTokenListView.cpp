/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxTokenListView.h"
#include "udocs-processor/StringHelper.h"

void udocs_processor::FtxTokenListView::ShowTokens(
    const std::vector<TokenService::StrippedTokenData> &Tokens) {
  FtxSimpleView::ReportSuccess(TOKEN_SUCCESS_MESSAGE);

  uint64_t NowTimestamp = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()).count();

  std::vector<std::string> TokensList;
  TokensList.reserve(Tokens.size());
  for (size_t i = 0; i < Tokens.size(); ++i) {
    std::string Date = StringHelper::MakeDateTime(Tokens[i].ExpiresAt,
        true, true);
    std::string Line = fmt::format(TOKEN_PATTERN,
        NowTimestamp > Tokens[i].ExpiresAt ? EXPIRED : ACTIVE, i, Date,
        fmt::join(Tokens[i].Scopes, SCOPES_GLUE));
    TokensList.emplace_back(std::move(Line));
  }

  FtxSimpleView::ShowList(std::move(TokensList));
}

void udocs_processor::FtxTokenListView::Init() {
  FtxSimpleView::Init();

  FtxSimpleView::ReportProgress(PROGRESS_MESSAGE);
}

udocs_processor::FtxTokenListView::FtxTokenListView() {}
