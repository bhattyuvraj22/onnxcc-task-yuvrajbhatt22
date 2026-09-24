#include "onnxcc/cli/cli.h"
#include "onnxcc/third_party/cxxopts.hpp"
#include <iostream>

namespace onnxcc::cli {

namespace {

// Parses the "dump" subcommand's own arguments (everything after argv[1]):
// --model (required), --show-graph and --verbose (boolean flags), and
// dump's own --help.

    ParseResult parse_dump(int argc, char** argv) {
        ParseResult result;

        cxxopts::Options options("onnxcc dump", "Inspect an ONNX model");
        options.add_options()
            ("model", "Path to the .onnx model file", cxxopts::value<std::string>())
            ("show-graph", "Print the model's graph structure",
                cxxopts::value<bool>()->default_value("false"))
            ("verbose", "Enable verbose output",
                cxxopts::value<bool>()->default_value("false"))
            ("h,help", "Print usage for dump");

        cxxopts::ParseResult parsed;
        try {
            parsed = options.parse(argc, argv);
        } catch (const std::exception& e) {
            std::cerr << "onnxcc dump: " << e.what() << "\n";
            result.exit_code = 1;
            return result;
        }

        if (parsed.count("help")) {
            std::cout << options.help() << "\n";
            result.exit_code = 0;
            return result;
        }

        if (parsed.count("model") == 0) {
            std::cerr << "onnxcc dump: --model is required\n";
            result.exit_code = 1;
            return result;
        }

        result.command = Command::Dump;
        result.dump_options.model_path = parsed["model"].as<std::string>();
        result.dump_options.show_graph = parsed["show-graph"].as<bool>();
        result.dump_options.verbose = parsed["verbose"].as<bool>();
        result.exit_code = 0;
        return result;
    } //ParseResult parse_dump
} //namespace

// Handles the top-level command line: no arguments, --help/-h, dispatching
// to "dump", or an unrecognized subcommand.

    ParseResult parse(int argc, char** argv) {
        ParseResult result;

        if (argc == 1) {
            std::cerr << "Usage: onnxcc <command> [options]\n";
            std::cerr << "Commands:\n";
            std::cerr << "  dump    Inspect an ONNX model\n";
            result.exit_code = 1;
            return result;
        }

        std::string first_arg = argv[1];

        if (first_arg == "--help" || first_arg == "-h") {
            std::cout << "Usage: onnxcc <command> [options]\n";
            std::cout << "Commands:\n";
            std::cout << "  dump    Inspect an ONNX model\n";
            result.exit_code = 0;
            return result;
        }

        if (first_arg == "dump") {
            return parse_dump(argc - 1, argv + 1);
        }

        std::cerr << "onnxcc: unknown subcommand '" << first_arg << "'\n";
        result.exit_code = 1;
        return result;
    } //ParseResult parse

}  // namespace onnxcc::cli