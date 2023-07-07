/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/version/DefaultAds.h"

udocs_processor::DefaultAds::DefaultAds() {
  Ads.emplace_back(AdsManager::Ad{AD_0, AD_IMAGE, AD_0_COLOR});
  Ads.emplace_back(AdsManager::Ad{AD_1, AD_IMAGE, AD_1_COLOR});
  Ads.emplace_back(AdsManager::Ad{AD_2, AD_IMAGE, AD_2_COLOR});
}

const std::vector<udocs_processor::AdsManager::Ad> &
    udocs_processor::DefaultAds::GetAds() const {
  return Ads;
}

