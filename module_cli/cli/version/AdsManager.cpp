/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/version/AdsManager.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include "udocs-processor/cli/version/DefaultAds.h"

bool udocs_processor::AdsManager::LoadSelf() {
  std::string Path = CachePath + ADS_CACHE_FILENAME;
  std::ifstream Input{Path};
  if (!Input.good()) {
    l->error("Couldn't load ads check info from `{}`", Path);
    return false;
  }

  nlohmann::json Data = nlohmann::json::parse(Input);
  LastCheckedOn = Data[ADS_CACHE_CHECKED_ON];

  for (const auto& Message : Data[ADS_CACHE_ADS]) {
    Ads.emplace_back(ParseAd(Message));
  }

  return true;
}

void udocs_processor::AdsManager::SaveSelf() const {
  std::string Path = CachePath + ADS_CACHE_FILENAME;
  std::ofstream Output{Path};
  if (!Output.good()) {
    l->error("Couldn't save ads check info to `{}`", Path);
    return;
  }

  nlohmann::json Data;
  Data[ADS_CACHE_CHECKED_ON] = LastCheckedOn;
  nlohmann::json Messages = nlohmann::json::array();
  for (const auto& Ad : Ads) {
    nlohmann::json Message;
    Message[JSON_AD_TEXT] = Ad.Text;
    Message[JSON_AD_ICON] = Ad.Icon;
    Message[JSON_AD_ICON_COLOR] = Ad.Color;

    Messages.emplace_back(std::move(Message));
  }
  Data[ADS_CACHE_ADS] = std::move(Messages);

  Output << Data.dump();
}

void udocs_processor::AdsManager::UpdateAds() {
  uint64_t CurrentTime = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()).count();

  bool DoUpdateAds = false;
  if (LoadSelf()) {
    if (LastCheckedOn + ADS_POLL_INTERVAL_S < CurrentTime) {
      DoUpdateAds = true;
      l->info("Ads cache is outdated, rechecking");
    }
  } else {
    DoUpdateAds = true;
    l->info("No ads cache found, rechecking");
  }

  if (DoUpdateAds) {
    LastCheckedOn = CurrentTime;
    try {
      ProductService::GetBroadcastResponse Response =
          Product->GetBroadcast(ProductId);
      if (Response.Status.GetCode() != ApiStatus::SUCCESS) {
        l->error("Error when updating ads: {}/{}",
            Response.Status.GetCode(), Response.Status.GetMessageDescription());
      } else {
        for (const auto &Message : Response.Messages) {
          if (Message.Format != AD_FORMAT) {
            l->warn("Undefined format {}", Message.Format);
            continue;
          }

          Ads.emplace_back(ParseAd(nlohmann::json::parse(Message.Payload)));
        }
        SaveSelf();
        return;
      }
    } catch (const std::exception& Exc) {
      l->error("Exception when updating ads: ", Exc.what());
    }

    Ads = Default->GetAds();
    SaveSelf();
  }
}

std::vector<udocs_processor::AdsManager::Ad>
    udocs_processor::AdsManager::GetAds() const {
  return Ads;
}

void udocs_processor::AdsManager::SetDefaultAds(
    std::shared_ptr<DefaultAds> Default) {
  this->Default = Default;
}

void udocs_processor::AdsManager::SetCachePath(const std::string &CachePath) {
  this->CachePath = CachePath;
}

udocs_processor::AdsManager::AdsManager(
    std::shared_ptr<spdlog::sinks::sink> Sink, std::string ProductId)
    : ProductId(std::move(ProductId)) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}

void udocs_processor::AdsManager::SetProductService(
    std::shared_ptr<ProductService> Product) {
  this->Product = Product;
}

udocs_processor::AdsManager::Ad
    udocs_processor::AdsManager::ParseAd(const nlohmann::json& Data) const {
  Ad AdData;
  AdData.Color = Data[JSON_AD_ICON_COLOR];
  AdData.Text = Data[JSON_AD_TEXT];
  AdData.Icon = Data[JSON_AD_ICON];

  return AdData;
}
