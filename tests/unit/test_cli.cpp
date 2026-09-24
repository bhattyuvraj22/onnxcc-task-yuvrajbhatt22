#include <gtest/gtest.h>
#include "onnxcc/cli/cli.h"

#include <string>
#include <vector>

namespace {

// Builds a fake argv[] from plain strings, so a test can call cli::parse()
// the same way main() does, without a real command line.
//
// Why this needs to be a struct, not a free function returning char**:
// argv[i] must point at real, mutable char buffers that stay alive for as
// long as parse() is running. If we just returned a vector<char*> and let
// the vector<string> it points into go out of scope, every pointer in
// argv would be dangling. Keeping both vectors together as one object's
// members means they share the same lifetime automatically.
struct FakeArgs {
    std::vector<std::string> storage;
    std::vector<char*> argv;

    explicit FakeArgs(std::vector<std::string> args) : storage(std::move(args)) {
        argv.reserve(storage.size());
        for (auto& s : storage) {
            argv.push_back(s.data());
        }
    }

    int argc() const { return static_cast<int>(argv.size()); }
    char** data() { return argv.data(); }
};

// What one parse() call produced: the struct your own code returns, plus
// everything that was printed, sorted by which stream it went to.
struct ParseCapture {
    onnxcc::cli::ParseResult result;
    std::string stdout_text;
    std::string stderr_text;
};

// Runs cli::parse() with fake command-line args, capturing stdout/stderr
// so a test can check both WHAT was printed and WHERE it went.
ParseCapture capture_parse(std::vector<std::string> args) {
    FakeArgs fake(std::move(args));

    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();

    ParseCapture capture;
    capture.result = onnxcc::cli::parse(fake.argc(), fake.data());

    capture.stdout_text = testing::internal::GetCapturedStdout();
    capture.stderr_text = testing::internal::GetCapturedStderr();

    return capture;
}

} // namespace

// Row 1: onnxcc dump --model path/to/file.onnx -> exit 0
TEST(CliContract, DumpWithModelExitsZero) {
    auto capture = capture_parse({"onnxcc", "dump", "--model", "f.onnx"});

    EXPECT_EQ(capture.result.exit_code, 0);
    EXPECT_EQ(capture.result.command, onnxcc::cli::Command::Dump);
    EXPECT_EQ(capture.result.dump_options.model_path, "f.onnx");
}

// Row 2: --show-graph is a boolean flag
TEST(CliContract, DumpShowGraphIsBooleanFlag) {
    auto capture = capture_parse({"onnxcc", "dump", "--model", "f.onnx", "--show-graph"});

    EXPECT_EQ(capture.result.exit_code, 0);
    EXPECT_TRUE(capture.result.dump_options.show_graph);
    EXPECT_FALSE(capture.result.dump_options.verbose);
}

// Row 3: --verbose is a boolean flag
TEST(CliContract, DumpVerboseIsBooleanFlag) {
    auto capture = capture_parse({"onnxcc", "dump", "--model", "f.onnx", "--verbose"});

    EXPECT_EQ(capture.result.exit_code, 0);
    EXPECT_TRUE(capture.result.dump_options.verbose);
    EXPECT_FALSE(capture.result.dump_options.show_graph);
}

// Row 4: onnxcc dump --help -> usage for dump's 3 options, stdout, exit 0
TEST(CliContract, DumpHelpPrintsUsageOnStdout) {
    auto capture = capture_parse({"onnxcc", "dump", "--help"});

    EXPECT_EQ(capture.result.exit_code, 0);
    EXPECT_NE(capture.stdout_text.find("--model"), std::string::npos);
    EXPECT_NE(capture.stdout_text.find("--show-graph"), std::string::npos);
    EXPECT_NE(capture.stdout_text.find("--verbose"), std::string::npos);
    EXPECT_TRUE(capture.stderr_text.empty());
}

// Row 5: onnxcc --help -> top-level usage, stdout, exit 0
TEST(CliContract, TopLevelHelpPrintsUsageOnStdout) {
    auto capture = capture_parse({"onnxcc", "--help"});

    EXPECT_EQ(capture.result.exit_code, 0);
    EXPECT_NE(capture.stdout_text.find("dump"), std::string::npos);
    EXPECT_TRUE(capture.stderr_text.empty());
}

// Row 6: onnxcc dump (no --model) -> exit non-zero, readable error on stderr
TEST(CliContract, DumpWithoutModelFailsOnStderr) {
    auto capture = capture_parse({"onnxcc", "dump"});

    EXPECT_NE(capture.result.exit_code, 0);
    EXPECT_FALSE(capture.stderr_text.empty());
    EXPECT_TRUE(capture.stdout_text.empty());
    EXPECT_EQ(capture.result.command, onnxcc::cli::Command::None);
}

// Row 7: onnxcc bogus -> exit non-zero, stderr names the bad subcommand
TEST(CliContract, UnknownSubcommandNamesItOnStderr) {
    auto capture = capture_parse({"onnxcc", "bogus"});

    EXPECT_NE(capture.result.exit_code, 0);
    EXPECT_NE(capture.stderr_text.find("bogus"), std::string::npos);
    EXPECT_TRUE(capture.stdout_text.empty());
}

// Row 8: onnxcc (no arguments) -> exit non-zero, usage on stderr
TEST(CliContract, NoArgumentsPrintsUsageOnStderr) {
    auto capture = capture_parse({"onnxcc"});

    EXPECT_NE(capture.result.exit_code, 0);
    EXPECT_FALSE(capture.stderr_text.empty());
    EXPECT_TRUE(capture.stdout_text.empty());
}