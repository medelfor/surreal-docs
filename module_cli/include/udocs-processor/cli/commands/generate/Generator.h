/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "udocs-processor/report/ProcessingReport.h"
#include "udocs-processor/serializers/DynamicPageData.h"
#include "udocs-processor/serializers/JSONDocTreeSerializer.h"
#include "udocs-processor/cli/commands/generate/rendering/UDocsDocumentRenderer.h"

#undef ERROR

namespace udocs_processor {
class Generator {
 public:
  enum class Status {
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

  using CallbackType = std::function<void(Status, const std::string&)>;

  struct GenerationRequest {
    enum class ExportFormat {
      HTML,
      DEPLOYMENT
    };

    std::string OutDirectory;
    std::string InputDoxy;
    std::string InputUData;
    std::string InputDocs;
    bool DoExportPrivate = true;
    ExportFormat Format = ExportFormat::HTML;
  };

  explicit Generator(std::shared_ptr<spdlog::sinks::sink> Sink);

  std::vector<udocs_processor::ManifestEntry>
      Generate(const GenerationRequest& Request,
          const CallbackType& StageCallback) const;

  void SetResourcesPath(std::string ResourcesPath);

  void SetRenderer(std::unique_ptr<UDocsDocumentRenderer> Renderer);

  void CleanUp(const std::string& OutDirectory) const;

 private:
  void CopyResources(const GenerationRequest& Request) const;

  void SerializeToHtml(const GenerationRequest& Request,
      const CallbackType& StageCallback,
      const std::vector<udocs_processor::ManifestEntry>& Manifest) const;

  static constexpr const char* TYPE_TEMPLATE =
      "html/udocs_processor/types/Type.html";
  static constexpr const char* FUNCTION_TEMPLATE =
      "html/udocs_processor/functions/Function.html";
  static constexpr const char* HTML2PNG_PATH =
      "misc/util/udocs_processor/html2png/html2png.js";
  static constexpr const char* UDATA_MODULES = "modules";
  static constexpr const char* HTML_EXTENSION = ".html";
  constexpr static char DIRECTORY_SEPARATOR = '/';

  static constexpr const char* LOGGER_NAME = "generator";

  static constexpr const char* GENERATED_IMAGES_DIR = "converted";
  static constexpr const char* INTERNAL_JSON_DIR = "pages";
  static constexpr const char* LOCAL_RES_DIR = "localres";

  static constexpr const char* THEME_CSS = "styles/udocs_processor/theme1.css";
  static constexpr const char* THEME_JS =
      "scripts/udocs_processor/theme/theme1/theme1.bundle.js";

  constexpr static char URL_SEPARATOR = '/';
  constexpr static char URL_SEPARATOR_LOCAL = '+';

  static const bool IS_TRIAL = true;
  static const bool DO_PRETTY_PRINT = false;
  static const size_t IMAGE_MAX_SIDE_SIZE = 2048;
  static const size_t MAX_IMAGE_FILESIZE = 1024 * 1024 * 4;  // 4MB
  static const size_t MAX_PAGE_FILESIZE = 1024 * 1024 * 2;  // 2MB

  std::vector<std::string> ResourcesToCopy;
  std::shared_ptr<spdlog::sinks::sink> Sink;
  std::shared_ptr<spdlog::logger> l;
  std::string ResourcesPath;
  std::unique_ptr<UDocsDocumentRenderer> Renderer;
};
}  // namespace udocs_processor
