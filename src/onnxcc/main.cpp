#include <iostream>
#include "onnxcc/cli/cli.h"

namespace {

// TODO: stub for now — Part A only needs argument validation, not the actual
// dump logic (no file opened yet). Just return 0 for success. If you want
// visible proof --verbose/--show-graph were parsed correctly while testing,
// you can print something conditionally here, but it's not required yet.
int run_dump(const onnxcc::cli::DumpOptions& options) {
    (void)options;
    return 0;
}

} // namespace


// TODO:
// 1. call onnxcc::cli::parse(argc, argv), store the ParseResult
// 2. if result.command == onnxcc::cli::Command::Dump,
//      return run_dump(result.dump_options)
// 3. otherwise, cli::parse() has already handled everything (printed
//    help/usage/errors on the right stream) — just return result.exit_code

int main(int argc, char** argv) {
    onnxcc::cli::ParseResult result = onnxcc::cli::parse(argc, argv);

    if (result.command == onnxcc::cli::Command::Dump) {
        return run_dump(result.dump_options);
    }

    return result.exit_code;
}