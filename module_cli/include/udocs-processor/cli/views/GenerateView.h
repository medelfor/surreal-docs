/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <string>
#include <map>
#include "View.h"

namespace udocs_processor {
class GenerateView : public virtual View {
 public:
  enum class Status {
    PREPARING,
    COMPILING,
    EXTRACTING_FROM_BP,
    EXTRACTING_FROM_CPP,
    LOADING,
    PROCESSING,
    SERIALIZING_INTERNAL,
    SERIALIZING_IMAGES,
    SERIALIZING_HTML,
    FINALIZING,
    CLEANING_UP,
    FINISHED,
    ERROR
  };

  GenerateView();

  virtual void SetStatus(Status Status_, const std::string& Message) = 0;

  std::string GetStatusString(Status Status_) const;

 private:
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
  static constexpr const char STATUS_EXTRACTING_FINALIZING[] = "Finalizing";
  static constexpr const char STATUS_EXTRACTING_CLEANING_UP[] = "Cleaning up";
  static constexpr const char STATUS_EXTRACTING_FINISHED[] = "Finished";
  static constexpr const char STATUS_EXTRACTING_ERROR[] = "Error";

  std::map<Status, std::string> StatusToString;
};
}  // namespace udocs_processor
