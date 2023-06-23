/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <boost/asio.hpp> NOLINT()
#include <boost/process.hpp> NOLINT()
#include <boost/process/exe.hpp> NOLINT()
#include "udocs-processor/cli/commands/generate/CppPreparer.h"
#include <spdlog/spdlog.h> NOLINT()
#include <thread> NOLINT()
#include <chrono> NOLINT()
#include <future> NOLINT()
#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
#include <inja/template.hpp>
#include <boost/process/child.hpp>
#include "udocs-processor/StringHelper.h"
#include "udocs-processor/ExecStreamHelper.h"

void udocs_processor::CppPreparer::SetBinDirectory(
    const std::string& BinDirectory) {
  this->BinDirectory = StringHelper::Normalize(BinDirectory);
}

void udocs_processor::CppPreparer::Prepare(
    const udocs_processor::CppPreparer::PrepareRequest &Request) {
  std::filesystem::path CppWd{MakeCppWd(Request.OutDirectory)};
  bool Created = std::filesystem::create_directories(CppWd);
  // because trailing slash and MSVC
//  if (!Created) {
//    std::string Message = fmt::format("Unable to create cpp wd: {}",
//        CppWd.string());
//    l->error(Message);
//    throw std::runtime_error{Message};
//  }

  RunDoxygen(Request);
  RunDoxybook(Request);
}

void udocs_processor::CppPreparer::RunDoxygen(
    const udocs_processor::CppPreparer::PrepareRequest &Request) {
  PrepareDoxyfile(Request);

  // run doxygen-ue
  std::vector<std::string> Arguments = {
      BinDirectory + DOXYGEN_EXECUTABLE,
      MakeCppWd(Request.OutDirectory) + DOXYFILE_NAME};

  using time_point = std::chrono::steady_clock::time_point;
  time_point Begin = std::chrono::steady_clock::now();

  ExecStreamHelper::ProcessResult Result = ExecStreamHelper::Run(Arguments);

  l->warn("DoxygenUE output:\n{}", Result.Output);
  time_point End = std::chrono::steady_clock::now();
  uint64_t TimeMs = std::chrono::duration_cast<
      std::chrono::milliseconds>(End - Begin).count();
  l->info("DoxygenUE took {}ms and exited with code {}",
      TimeMs, Result.ExitCode);

  if (Result.ExitCode) {
    l->error("DoxygenUE failed");
    throw std::runtime_error{fmt::format("DoxygenUE exited with code {}",
        Result.ExitCode)};
  }
}

void udocs_processor::CppPreparer::PrepareDoxyfile(
    const udocs_processor::CppPreparer::PrepareRequest &Request) {
  std::string ProtoDoxyfile = ResDirectory + DOXYFILE;
  nlohmann::json Data;
  std::string CppWd = MakeCppWd(Request.OutDirectory);
  std::string OutputDir = CppWd + DOXYGEN_XML_DIR;
  std::filesystem::create_directories(OutputDir);
  Data[DOXYFILE_DATA_INCLUDE] = Request.Inputs;
  Data[DOXYFILE_DATA_EXCLUDE] = Request.Exclude;
  Data[DOXYFILE_DATA_OUTPUT] = OutputDir;
  nlohmann::json DataForTemplate;
  DataForTemplate[DOXYFILE_DATA] = std::move(Data);

  std::string Doxyfile = Env.render_file(ProtoDoxyfile, DataForTemplate);
  std::string DoxyfilePath = CppWd + DOXYFILE_NAME;
  std::ofstream Output(DoxyfilePath, std::ios::trunc);
  if (!Output.is_open()) {
    throw std::runtime_error{fmt::format(
        "Unable to write Doxyfile to `{}`", DoxyfilePath)};
  }
  Output.write(Doxyfile.c_str(), Doxyfile.size());
}

void udocs_processor::CppPreparer::RunDoxybook(
    const udocs_processor::CppPreparer::PrepareRequest &Request) const {
  // run doxybook
  // create output dir (cause apparently doxybook can't figure out how to do it)
  std::string CppWd = MakeCppWd(Request.OutDirectory);
  std::string OutputDir = CppWd + DOXYBOOK_OUTPUT_DIR;

  std::filesystem::create_directories(OutputDir);

  std::vector<std::string> Arguments = {
      BinDirectory + DOXYBOOK2_EXECUTABLE, DOXYBOOK2_INPUT,
      CppWd + DOXYGEN_XML_DIR, DOXYBOOK2_OUTPUT, OutputDir, DOXYBOOK2_J};

  using time_point = std::chrono::steady_clock::time_point;
  time_point Begin = std::chrono::steady_clock::now();

  ExecStreamHelper::ProcessResult Result = ExecStreamHelper::Run(Arguments);

  l->warn("Doxybook output:\n{}", Result.Output);
  time_point End = std::chrono::steady_clock::now();
  uint64_t TimeMs = std::chrono::duration_cast<
      std::chrono::milliseconds>(End - Begin).count();
  l->info("Doxybook took {}ms and exited with code {}",
      TimeMs, Result.ExitCode);

  if (Result.ExitCode) {
    l->error("Doxybook failed");
    throw std::runtime_error{fmt::format("Doxybook exited with code {}",
        Result.ExitCode)};
  }
}

udocs_processor::CppPreparer::CppPreparer(
    std::shared_ptr<spdlog::sinks::sink> Sink) {
  l = spdlog::get(LOGGER_NAME);
  if (!l) {
    l = std::make_shared<spdlog::logger>(LOGGER_NAME);
    if (Sink) {
      l->sinks().emplace_back(Sink);
    }
  }
}

void udocs_processor::CppPreparer::SetResDirectory(
    const std::string& ResDirectory) {
  this->ResDirectory = StringHelper::Normalize(ResDirectory);
}

void udocs_processor::CppPreparer::CleanUp(
    const std::string &OutDirectory) const {
  std::filesystem::remove_all({MakeCppWd(OutDirectory)});
}

std::string udocs_processor::CppPreparer::MakeCppWd(
    const std::string& OutDirectory) const {
  return StringHelper::Normalize(OutDirectory) + CPP_WD_NAME +
      DIRECTORY_SEPARATOR;
}
