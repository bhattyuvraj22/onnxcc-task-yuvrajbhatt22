#include "onnxcc/cli/cli.h"
#include "onnxcc/third_party/cxxopts.hpp"
#include <iostream>

namespace onnxcc::cli {

namespace {

// TODO: parses the "dump" subcommand's own args (everything after argv[1]).
// Build a cxxopts::Options here with --model, --show-graph, --verbose, and
// dump's own --help. Look up: options.add_options(), cxxopts::value<T>(),
// options.parse(argc, argv), and options.help().

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

    // TODO:
    // 1. argc == 1 (no args at all) -> usage to stderr, exit_code != 0
    // 2. argv[1] == "--help" or "-h" -> top-level usage to stdout, exit_code == 0
    // 3. argv[1] == "dump" -> return parse_dump(argc, argv)
    // 4. anything else -> "unknown subcommand: <argv[1]>" to stderr, exit_code != 0

   
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