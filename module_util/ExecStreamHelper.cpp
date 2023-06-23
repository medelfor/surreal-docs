/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include <boost/asio.hpp> NOLINT()
#include <boost/process.hpp> NOLINT()
#include "udocs-processor/ExecStreamHelper.h"
#include <fmt/format.h> NOLINT()

udocs_processor::ExecStreamHelper::ProcessResult
udocs_processor::ExecStreamHelper::Run(
    const std::vector<std::string>& Arguments) {
  boost::asio::io_service IoService;
  std::future<std::string> Data;

  std::string Command = fmt::format("\"{}\"",
      fmt::join(Arguments, ARGUMENTS_JOINER));
  boost::process::child Child(Command,
    boost::process::std_in.close(),
    (boost::process::std_out & boost::process::std_err) > Data, IoService);

  IoService.run();
  Child.wait();

  ProcessResult Result;
  Result.Output = Data.get();
  Result.ExitCode = Child.exit_code();

  return std::move(Result);
}
