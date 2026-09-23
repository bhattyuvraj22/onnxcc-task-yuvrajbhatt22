#pragma once
#include <string>

namespace onnxcc::cli {

struct DumpOptions {
    std::string model_path;
    bool show_graph = false;
    bool verbose = false;
};

enum class Command {
    None,  
    Dump,
};

struct ParseResult {
    Command command = Command::None;
    DumpOptions dump_options;
    int exit_code = 0;
};

ParseResult parse(int argc, char** argv);

} // namespace onnxcc::cli