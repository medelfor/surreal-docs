/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <spdlog/logger.h>
#include <string>
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>
#include "ProductService.h"

namespace udocs_processor {
class DefaultAds;
class AdsManager {
 public:
  struct Ad {
    std::string Text;
    std::string Icon;
    uint64_t Color = 0;
  };

  AdsManager(std::shared_ptr<spdlog::sinks::sink> Sink, std::string ProductId);

  void SetDefaultAds(std::shared_ptr<DefaultAds> Default);

  void SetProductService(std::shared_ptr<ProductService> Product);

  void SetCachePath(const std::string& CachePath);

  void UpdateAds();

  std::vector<Ad> GetAds() const;

 private:
  void SaveSelf() const;

  bool LoadSelf();

  Ad ParseAd(const nlohmann::json& Data) const;

  std::vector<Ad> Ads;
  std::string CachePath;
  std::string ProductId;
  uint64_t LastCheckedOn = 0;

  static constexpr uint64_t ADS_POLL_INTERVAL_S = 3600 * 6;  // every 6hr
  static constexpr const char* ADS_CACHE_FILENAME = "ads_cache.json";
  static constexpr const char* ADS_CACHE_ADS = "ads";
  static constexpr const char* ADS_CACHE_CHECKED_ON = "checked_on";

  static constexpr const char* JSON_AD_TEXT = "text";
  static constexpr const char* JSON_AD_ICON = "icon";
  static constexpr const char* JSON_AD_ICON_COLOR = "icon_color";

  static constexpr const char* AD_FORMAT = "json_v1";

  static constexpr const char* LOGGER_NAME = "version-checker";
  std::shared_ptr<spdlog::logger> l;
  std::shared_ptr<DefaultAds> Default;
  std::shared_ptr<ProductService> Product;
};
}  // namespace udocs_processor
