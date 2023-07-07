/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <string>
#include <map>
#include <vector>
#include "View.h"
#include "udocs-processor/cli/version/AdsManager.h"

namespace udocs_processor {
class GenerateView : public virtual View {
 public:
  enum class Status {
    PREDEPLOY,
    PREPARING,
    COMPILING,
    EXTRACTING_FROM_BP,
    EXTRACTING_FROM_CPP,
    LOADING,
    PROCESSING,
    SERIALIZING_INTERNAL,
    SERIALIZING_IMAGES,
    SERIALIZING_HTML,
    PURGING_OUT,
    DEPLOYING,
    FINALIZING,
    CLEANING_UP,
    FINISHED,
    ERROR
  };

  GenerateView();

  virtual void SetStatus(Status Status_, const std::string& Message) = 0;

  void SetAds(std::vector<AdsManager::Ad> Ads);

  std::string GetStatusString(Status Status_) const;

  ~GenerateView() override = default;

 private:
  static constexpr const char STATUS_PREDEPLOY[] = "Pre-deployment check";
  static constexpr const char STATUS_PREPARING[] = "Preparing";
  static constexpr const char STATUS_COMPILING[] = "Compiling project";
  static constexpr const char STATUS_EXTRACTING_FROM_BP[] =
      "Retrieveing Blueprints info";
  static constexpr const char STATUS_EXTRACTING_FROM_CPP[] =
      "Retrieveing C++ info";
  static constexpr const char STATUS_EXTRACTING_LOADING[] =
      "Loading documentation tree";
  static constexpr const char STATUS_EXTRACTING_PROCESSING[] =
      "Processing documentation tree";
  static constexpr const char STATUS_EXTRACTING_SERIALIZING_INTERNAL[] =
      "Serializing";
  static constexpr const char STATUS_EXTRACTING_SERIALIZING_IMAGES[] =
      "Rendering images";
  static constexpr const char STATUS_EXTRACTING_SERIALIZING_HTML[] =
      "Rendering HTML";
  static constexpr const char STATUS_EXTRACTING_PURGING_OUT[] =
      "Purging the out directory";
  static constexpr const char STATUS_EXTRACTING_DEPLOYING[] = "Deploying";
  static constexpr const char STATUS_EXTRACTING_FINALIZING[] = "Finalizing";
  static constexpr const char STATUS_EXTRACTING_CLEANING_UP[] = "Cleaning up";
  static constexpr const char STATUS_EXTRACTING_FINISHED[] = "Finished";
  static constexpr const char STATUS_EXTRACTING_ERROR[] = "Error";

  std::map<Status, std::string> StatusToString;

 protected:
  std::vector<AdsManager::Ad> Ads;
};
}  // namespace udocs_processor
