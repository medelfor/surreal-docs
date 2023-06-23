/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/commands/generate/Generator.h"
#include <udocs-processor/subprocessors/ReferenceCacheBuilder.h>
#include <udocs-processor/subprocessors/ExportPolicy.h>
#include <udocs-processor/subprocessors/CrossReferencer.h>
#include <udocs-processor/subprocessors/ComplementFinder.h>
#include <fstream>
#include <sstream>
#include <stringtoolbox.hpp>
#include "udocs-processor/UDocsProcessor.h"
#include "udocs-processor/IdGenerator.h"
#include "udocs-processor/treeloaders/UDataTreeLoader.h"
#include "udocs-processor/treeloaders/DoxybookTreeLoader.h"
#include "udocs-processor/FileDataLoader.h"
#include "udocs-processor/treeloaders/PagesTreeLoader.h"
#include "udocs-processor/serializers/UFunctionHTMLSerializer.h"
#include "udocs-processor/DocPathsCache.h"
#include "udocs-processor/subprocessors/DocPathBuilder.h"
#include "udocs-processor/serializers/type/UTypeHTMLSerializer.h"
#include "udocs-processor/serializers/type/UTypeMarkdownSerializer.h"
#include "udocs-processor/serializers/HTMLDocTreeSerializer.h"
#include "udocs-processor/serializers/JSONDocTreeSerializer.h"
#include "udocs-processor/serializers/DynamicPageData.h"
#include "udocs-processor/serializers/markdown/SurrealMarkdownToHtmlConverter.h"
#include "udocs-processor/NodeHasher.h"
#include "udocs-processor/StringHelper.h"

std::vector<udocs_processor::ManifestEntry>
    udocs_processor::Generator::Generate(
        const GenerationRequest& Request,
        const CallbackType& StageCallback) const {
  if (Request.Format != GenerationRequest::ExportFormat::DEPLOYMENT &&
      Request.Format != GenerationRequest::ExportFormat::HTML) {
    StageCallback(Status::ERROR, "Unsupported export format");
    return {};
  }

  udocs_processor::IdGenerator IdGen(Sink);
  udocs_processor::UDocsProcessor Processor(IdGen, Sink);
  Processor.SetStatusReporter(  // todo replace with a map
    [&StageCallback](UDocsProcessor::Status Status) {
      switch (Status) {
        case UDocsProcessor::Status::LOADING:
          StageCallback(Generator::Status::LOADING, "");
          break;
        case UDocsProcessor::Status::PROCESSING:
          StageCallback(Generator::Status::PROCESSING, "");
          break;
      }
    });
  std::unique_ptr<UDataTreeLoader> DataLoader1 =
      std::make_unique<UDataTreeLoader>();
  std::ifstream UDataStream(Request.InputUData);
  std::shared_ptr<nlohmann::json> UData =
    std::make_shared<nlohmann::json>(nlohmann::json::parse(UDataStream));
  if (!UData->contains(UDATA_MODULES)) {
    StageCallback(Status::ERROR, "Invalid UData: no modules");
    return {};
  }

  *UData = (*UData)[UDATA_MODULES];
  DataLoader1->SetUDataJson(UData);
  DoxybookDataLoader DoxyData;
  DoxyData.SetWorkingDirectory(Request.InputDoxy);
  std::unique_ptr<DoxybookTreeLoader> DataLoader2 =
          std::make_unique<DoxybookTreeLoader>();
  DataLoader2->SetJsonLoader(&DoxyData);

  std::unique_ptr<FileDataLoader> FileDataLoader1 =
          std::make_unique<FileDataLoader>(l);
  FileDataLoader1->SetWorkingDirectory(Request.InputDocs);
  std::unique_ptr<PagesTreeLoader> PagesTreeLoader_ =
          std::make_unique<PagesTreeLoader>();

  PagesTreeLoader_->SetImageMaxSideSize(IMAGE_MAX_SIDE_SIZE);
  PagesTreeLoader_->SetMaxImageFileSize(MAX_IMAGE_FILESIZE);
  PagesTreeLoader_->SetMaxPageFileSize(MAX_PAGE_FILESIZE);
  PagesTreeLoader_->AllowImageType(ImageType::PNG);

  PagesTreeLoader_->SetDataLoader(std::move(FileDataLoader1));

  // order must remain
  Processor.AddTreeLoader(std::move(PagesTreeLoader_));
  Processor.AddTreeLoader(std::move(DataLoader1));
  Processor.AddTreeLoader(std::move(DataLoader2));

  std::unique_ptr<CrossReferencer> CrossReferencer_ =
    std::make_unique<CrossReferencer>();
  std::unique_ptr<NodeHasher> Hasher = std::make_unique<NodeHasher>();
  std::unique_ptr<ComplementFinder> ComplementFinder_ =
    std::make_unique<ComplementFinder>(std::move(Hasher), IdGen);
  CrossReferencer* CrossReferencerPtr = CrossReferencer_.get();
  ComplementFinder* ComplementFinderPtr = ComplementFinder_.get();
  std::function<void(ReferenceCacheBuilder&)> Callback =
    [CrossReferencerPtr, ComplementFinderPtr]
    (ReferenceCacheBuilder& Builder) {
      CrossReferencerPtr->SetCache(Builder.GetCache());
      ComplementFinderPtr->SetCache(Builder.GetCache());
    };
  ComplementFinderPtr->SetCrossReferencer(CrossReferencerPtr);
  std::unique_ptr<ReferenceCacheBuilder> CacheBuilder =
    std::make_unique<ReferenceCacheBuilder>(Callback);

  std::unique_ptr<DocPathsCache> Cache = std::make_unique<DocPathsCache>();
  std::unique_ptr<NodeHasher> Hasher2 = std::make_unique<NodeHasher>();
  std::unique_ptr<DocPathBuilder> Builder =
    std::make_unique<DocPathBuilder>(*Cache, std::move(Hasher2));
  std::unique_ptr<ExportPolicy> ExportPolicy_ =
    std::make_unique<ExportPolicy>(*Cache);
  ExportPolicy_->SetDoExportPrivate(Request.DoExportPrivate);

  Processor.AddSubprocessor(std::move(CacheBuilder));
  Processor.AddSubprocessor(std::move(ComplementFinder_));
  Processor.AddSubprocessor(std::move(CrossReferencer_));
  Processor.AddSubprocessor(std::move(ExportPolicy_));
  Processor.AddSubprocessor(std::move(Builder));

  std::ifstream FunctionTemplate(ResourcesPath + FUNCTION_TEMPLATE);
  std::stringstream FunctionTemplateBuffer;
  FunctionTemplateBuffer << FunctionTemplate.rdbuf();

  std::unique_ptr<UFunctionHTMLSerializer> FunctionSerializer =
          std::make_unique<UFunctionHTMLSerializer>();
  FunctionSerializer->SetIsTrial(IS_TRIAL);
  FunctionSerializer->SetFunctionTemplate(FunctionTemplateBuffer.str());
  FunctionSerializer->SetResourcesPath(ResourcesPath);

  std::ifstream TypeTemplate(ResourcesPath + TYPE_TEMPLATE);
  std::stringstream TypeTemplateBuffer;
  TypeTemplateBuffer << TypeTemplate.rdbuf();
  std::unique_ptr<UTypeHTMLSerializer> TypeToImagesSerializer =
    std::make_unique<UTypeHTMLSerializer>();
  TypeToImagesSerializer->SetResourcesPath(ResourcesPath);
  TypeToImagesSerializer->SetTypeTemplate(TypeTemplateBuffer.str());

  std::unique_ptr<udocs_processor::IntegrityCheck> Checker =
      std::make_unique<udocs_processor::IntegrityCheck>();
  Checker->SetResourcesPath(ResourcesPath);

  std::unique_ptr<UTypeToStringSerializer> TypeToStringSerializer =
    std::make_unique<UTypeMarkdownSerializer>(*Cache);
  std::unique_ptr<FileDataLoader> FileDataLoader2 =
    std::make_unique<FileDataLoader>(l);
  std::unique_ptr<JSONDocTreeSerializer> Serializer =
    std::make_unique<JSONDocTreeSerializer>(std::move(FunctionSerializer),
      std::move(TypeToImagesSerializer), std::move(TypeToStringSerializer),
      std::move(FileDataLoader2), std::move(Cache),
      std::move(Checker));

  // todo replace with a map
  Serializer->SetStatusReporter(
    [&StageCallback](JSONDocTreeSerializer::Status Status) {
      switch (Status) {
        case JSONDocTreeSerializer::Status::SERIALIZING_IMAGES:
          StageCallback(Generator::Status::SERIALIZING_IMAGES, "");
          break;
        case JSONDocTreeSerializer::Status::SERIALIZING_JSON:
          StageCallback(Generator::Status::SERIALIZING_INTERNAL, "");
          break;
      }
    });

  Serializer->SetPrettyPrint(DO_PRETTY_PRINT);
  Serializer->SetOutputDirectory(Request.OutDirectory);
  Serializer->SetDoBase64EncodeImages(Request.Format ==
    GenerationRequest::ExportFormat::DEPLOYMENT);
  Serializer->SetHtml2PngPath(ResourcesPath + HTML2PNG_PATH);
  JSONDocTreeSerializer* Serializer_ = Serializer.get();
  Processor.SetDocTreeSerializer(std::move(Serializer));

  Processor.Process();

  if (Request.Format == GenerationRequest::ExportFormat::HTML) {
    StageCallback(Status::SERIALIZING_HTML, "");
    SerializeToHtml(Request, StageCallback, Serializer_->GetManifest());
  }

  StageCallback(Status::FINALIZING, "");
  CopyResources(Request);
  StageCallback(Status::FINISHED, "");

  return Serializer_->GetManifest();
}

void udocs_processor::Generator::CleanUp(
    const std::string& OutDirectory) const {
  // remove all temp folder
  // 1. converted
  // 2. pages
  std::filesystem::remove_all({OutDirectory + DIRECTORY_SEPARATOR +
      GENERATED_IMAGES_DIR});
  std::filesystem::remove_all({OutDirectory + DIRECTORY_SEPARATOR +
      INTERNAL_JSON_DIR});
}

void udocs_processor::Generator::CopyResources(
    const GenerationRequest& Request) const {
  // copy neccessary static resources
  // 1. theme1.bundle.js
  // 2. theme1.css
  for (const auto& File : ResourcesToCopy) {
    std::string OutputPath = Request.OutDirectory +
        DIRECTORY_SEPARATOR + LOCAL_RES_DIR + DIRECTORY_SEPARATOR + File;
    std::filesystem::path FilePath{OutputPath};
    std::filesystem::create_directories(FilePath.parent_path());
    std::ifstream Input(ResourcesPath + DIRECTORY_SEPARATOR + File,
      std::ios::binary);
    std::ofstream Output(OutputPath);
    Output << Input.rdbuf();
    l->info("Copied `{}`", File);
  }
}

void udocs_processor::Generator::SerializeToHtml(
    const GenerationRequest& Request, const CallbackType& StageCallback,
    const std::vector<udocs_processor::ManifestEntry>& Manifest) const {
  Renderer->SetResourcesPath(ResourcesPath);

  nlohmann::json Index;
  for (auto& Entry : Manifest) {
    if (Entry.GetType() == udocs_processor::ManifestEntry::Type::INDEX) {
      Index = nlohmann::json::parse(std::ifstream(Entry.GetFileName()));
      break;
    }
  }

  if (Index.empty()) {
    throw std::runtime_error{"No index found"};
  }

  for (auto& Entry : Manifest) {
    switch (Entry.GetType()) {
      case ManifestEntry::Type::PAGE:
      {
        std::ifstream ContentStream(Entry.GetFileName());
        std::stringstream ContentStreamBuffer;
        ContentStreamBuffer << ContentStream.rdbuf();
        std::string Result = Renderer->Render(ContentStreamBuffer.str(), Index,
            Entry.GetType(), Entry.GetFormat());

        std::string Name = Entry.GetDocPath();
        for (char& Ch : Name) {
          if (Ch == URL_SEPARATOR) Ch = URL_SEPARATOR_LOCAL;
        }
        std::ofstream Ofstream(Request.OutDirectory + DIRECTORY_SEPARATOR +
            Name + HTML_EXTENSION, std::ofstream::trunc);
        Ofstream.write(Result.c_str(), Result.size());
        l->info("Serialized: {}", Name);
        break;
      }
      case ManifestEntry::Type::RESOURCE:
      {
        std::unique_ptr<FileDataLoader> Loader =
            std::make_unique<FileDataLoader>(l);
        Loader->SetWorkingDirectory(Request.OutDirectory);
        size_t Loaded = 0;
        const char* Result = Loader->LoadWholeBinaryFile(
            Entry.GetFileName(), Loaded);
        std::filesystem::path DocPath{Request.OutDirectory +
            DIRECTORY_SEPARATOR + Entry.GetDocPath()};
        std::filesystem::create_directories(DocPath.parent_path());

        Loader->WriteBytes(Request.OutDirectory + DIRECTORY_SEPARATOR +
            Entry.GetDocPath(), Result, Loaded);
        delete[] Result;
        l->info("Serialized: {}", Entry.GetDocPath());
        break;
      }
    }
  }
}

void udocs_processor::Generator::SetResourcesPath(std::string ResourcesPath) {
  this->ResourcesPath = StringHelper::Normalize(ResourcesPath);
}

void udocs_processor::Generator::SetRenderer(
    std::unique_ptr<UDocsDocumentRenderer> Renderer) {
  this->Renderer = std::move(Renderer);
}

udocs_processor::Generator::Generator(std::shared_ptr<spdlog::sinks::sink> Sink)
  : Sink(Sink) {
  ResourcesToCopy.emplace_back(THEME_CSS);
  ResourcesToCopy.emplace_back(THEME_JS);

  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    spdlog::register_logger(l);

    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}
