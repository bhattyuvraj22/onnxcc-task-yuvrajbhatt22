#include <iostream>
#include "onnxcc/cli/cli.h"

namespace {
    
// Stub: Part A only requires validating arguments, not opening the model
// file. Real dump logic (loading the model, honoring --show-graph and
// --verbose) is out of scope here -- see the task doc: "dump does not have
// to actually open the ONNX file."
int run_dump(const onnxcc::cli::DumpOptions& options) {
    (void)options;
    return 0;
}

} // namespace


// Delegates argument parsing to cli::parse(), then either runs the dump
// handler (if a valid subcommand was parsed) or returns the exit code
// parse() already determined -- help/usage/errors were already printed
// on the correct stream by that point.

int main(int argc, char** argv) {
    onnxcc::cli::ParseResult result = onnxcc::cli::parse(argc, argv);

    if (result.command == onnxcc::cli::Command::Dump) {
        return run_dump(result.dump_options);
    }

    return result.exit_code;
}