/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/GenerateView.h"

udocs_processor::GenerateView::GenerateView() {
  StatusToString.insert(std::make_pair(Status::PREDEPLOY, STATUS_PREDEPLOY));
  StatusToString.insert(std::make_pair(Status::PREPARING, STATUS_PREPARING));
  StatusToString.insert(std::make_pair(Status::COMPILING, STATUS_COMPILING));
  StatusToString.insert(std::make_pair(Status::EXTRACTING_FROM_BP,
      STATUS_EXTRACTING_FROM_BP));
  StatusToString.insert(std::make_pair(Status::EXTRACTING_FROM_CPP,
      STATUS_EXTRACTING_FROM_CPP));
  StatusToString.insert(std::make_pair(Status::LOADING,
      STATUS_EXTRACTING_LOADING));
  StatusToString.insert(std::make_pair(Status::PROCESSING,
      STATUS_EXTRACTING_PROCESSING));
  StatusToString.insert(std::make_pair(Status::SERIALIZING_INTERNAL,
      STATUS_EXTRACTING_SERIALIZING_INTERNAL));
  StatusToString.insert(std::make_pair(Status::SERIALIZING_IMAGES,
      STATUS_EXTRACTING_SERIALIZING_IMAGES));
  StatusToString.insert(std::make_pair(Status::SERIALIZING_HTML,
      STATUS_EXTRACTING_SERIALIZING_HTML));
  StatusToString.insert(std::make_pair(Status::DEPLOYING,
      STATUS_EXTRACTING_DEPLOYING));
  StatusToString.insert(std::make_pair(Status::PURGING_OUT,
      STATUS_EXTRACTING_PURGING_OUT));
  StatusToString.insert(std::make_pair(Status::FINALIZING,
      STATUS_EXTRACTING_FINALIZING));
  StatusToString.insert(std::make_pair(Status::CLEANING_UP,
      STATUS_EXTRACTING_CLEANING_UP));
  StatusToString.insert(std::make_pair(Status::FINISHED,
      STATUS_EXTRACTING_FINISHED));
  StatusToString.insert(std::make_pair(Status::ERROR, STATUS_EXTRACTING_ERROR));
}

std::string udocs_processor::GenerateView::GetStatusString(
    Status Status_) const {
  auto It = StatusToString.find(Status_);
  if (It != StatusToString.end()) {
    return It->second;
  }

  return "";
}

void udocs_processor::GenerateView::SetAds(std::vector<AdsManager::Ad> Ads) {
  this->Ads = std::move(Ads);
}
