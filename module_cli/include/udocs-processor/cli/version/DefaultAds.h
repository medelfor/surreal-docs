/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include "AdsManager.h"

namespace udocs_processor {
class DefaultAds {
 public:
  DefaultAds();

  const std::vector<AdsManager::Ad>& GetAds() const;

 private:
  std::vector<AdsManager::Ad> Ads;

  const uint64_t AD_0_COLOR = 197;
  const uint64_t AD_1_COLOR = 45;
  const uint64_t AD_2_COLOR = 56;
  const uint64_t AD_3_COLOR = 226;

  static constexpr const char PRESS_TO_EXIT[] = ". Press ESC to exit";

  static constexpr const char AD_0[] =
      "Need a cloud to make your docs available "
      "on the internet? Try Surreal Cloud starting "
      "at $7.99/month. More info at medelfor.com";
  static constexpr const char AD_1[] =
      "Need a cloud to share your docs privately with the team? "
      "Try Surreal Cloud+ starting "
      "at $14.99/month. More info at medelfor.com";
  static constexpr const char AD_2[] =
      "Need an on-premises solution to share your docs within organization? "
      "Try Surreal Cloud Pro starting "
      "at $39.99/month. More info at medelfor.com";

  static constexpr const char AD_3_IMAGE[] =
R"( █    ██
█ █  ██
 █  ██
   ██
  ██  █
 ██  █ █
██    █ )";

  static constexpr const char AD_IMAGE[] =
R"(  ▄▄▄▄▄▄
▄████████▄
██████████
 ▀██████▀
█▄      ▄█
 ▀██████▀
█▄      ▄█
 ▀██████▀ )";
};
}  // namespace udocs_processor
