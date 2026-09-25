# onnxcc

A command-line tool for working with ONNX models, built in C++20 on top of the official ONNX and Protobuf libraries.

This repository currently implements the tool's command-line interface, a deterministic test-fixture generator, and an automated test suite covering the CLI's behavior. Model loading, graph inspection, execution, and compilation are planned for later phases and are not yet implemented (see "Current Scope" below).

## Contents

- [Current Scope](#current-scope)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Building](#building)
- [Usage](#usage)
- [CLI Reference](#cli-reference)
- [Running Tests](#running-tests)
- [Generating Test Fixtures](#generating-test-fixtures)
- [Dependencies](#dependencies)
- [Commit Conventions](#commit-conventions)
- [Roadmap](#roadmap)

## Current Scope

The `onnxcc` executable currently supports one subcommand, `dump`, and top-level `--help`. At this stage, `dump` validates its arguments and reports errors clearly, but does not yet open or inspect the referenced ONNX file. The `--show-graph` and `--verbose` flags are accepted and parsed correctly but do not yet produce additional output. This is an intentional, documented scope boundary for the current phase of the project, not an oversight.

Three additional subcommands — `run`, `compile`, and `benchmark` — are planned for a later phase and are not present in this build.

## Project Structure

```
.
├── CMakeLists.txt
├── requirements.txt
├── scripts/
│   └── generate_test_models.py
├── src/
│   ├── CMakeLists.txt
│   └── onnxcc/
│       ├── cli/
│       │   ├── cli.h
│       │   └── cli.cpp
│       ├── third_party/
│       │   └── cxxopts.hpp
│       ├── main.cpp
│       ├── version.cpp
│       └── version.h.in
└── tests/
    ├── CMakeLists.txt
    ├── fixtures/           
    └── unit/
        ├── test_cli.cpp
        └── test_sanity.cpp
```

Argument parsing lives entirely under `src/onnxcc/cli/`. `main.cpp` is intentionally thin: it delegates parsing to `onnxcc::cli::parse()` and dispatches to a handler based on the result.

## Requirements

- CMake 3.26 or later
- A C++20 compiler (tested with AppleClang; GCC and MSVC with C++20 support should also work)
- Python 3.11+ (required only for regenerating test fixtures; not required to build or run the C++ tool)

No manual installation of Protobuf, ONNX, cxxopts, or GoogleTest is required. Protobuf, ONNX, and GoogleTest are fetched automatically by CMake at configure time. cxxopts is vendored directly in the repository and requires no network access.

## Building

```bash
git clone <repository-url>
cd onnxcc-task-yuvrajbhatt22
cmake -S . -B build
cmake --build build
```

The first configure step downloads and builds Protobuf, ONNX, and their transitive dependencies, which can take several minutes. Subsequent builds are incremental and much faster.

The resulting executable is placed at `build/onnxcc`.

## Usage

Display top-level usage:

```bash
./build/onnxcc --help
```

Validate arguments for a model file:

```bash
./build/onnxcc dump --model path/to/model.onnx
```

Boolean flags:

```bash
./build/onnxcc dump --model path/to/model.onnx --show-graph
./build/onnxcc dump --model path/to/model.onnx --verbose
```

Display usage for the `dump` subcommand specifically:

```bash
./build/onnxcc dump --help
```

## CLI Reference

| Invocation | Behavior |
|---|---|
| `onnxcc dump --model <path>` | Exit code 0 |
| `onnxcc dump --model <path> --show-graph` | Exit code 0; `--show-graph` is a boolean flag |
| `onnxcc dump --model <path> --verbose` | Exit code 0; `--verbose` is a boolean flag |
| `onnxcc dump --help` | Prints usage for `dump`, listing its options; exit code 0 |
| `onnxcc --help` | Prints top-level usage listing available subcommands; exit code 0 |
| `onnxcc dump` (no `--model`) | Exit code non-zero; error message on stderr |
| `onnxcc <unrecognized subcommand>` | Exit code non-zero; stderr names the unrecognized subcommand |
| `onnxcc` (no arguments) | Exit code non-zero; usage printed on stderr |

Normal output, including help text, is written to stdout. Errors are written to stderr. `dump` does not require the referenced file to exist or be a valid ONNX model at this stage; only its arguments are validated.

## Running Tests

Tests are written with GoogleTest and discovered automatically by CTest.

```bash
ctest --test-dir build --output-on-failure
```

The suite currently includes:

- Two sanity tests confirming the test framework and version-reporting module work correctly.
- Eight tests exercising the CLI contract described above, each verifying the returned exit code together with which output stream (stdout or stderr) received the corresponding message.

## Generating Test Fixtures

`scripts/generate_test_models.py` produces a small, deterministic ONNX model and a matching input tensor, used as fixtures for future testing. It is not part of the C++ build and is run independently.

The generated model is a 4-8-2 multilayer perceptron with a ReLU activation after each layer, built directly with `onnx.helper` rather than exported from a framework such as PyTorch. This avoids PyTorch's export path fusing linear-layer operations into a single `Gemm` node; the generated graph instead uses only `MatMul`, `Add`, and `Relu` operations, matching what this phase of the project supports.

Setup:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

Generate the fixtures:

```bash
python scripts/generate_test_models.py
```

This writes two files to `tests/fixtures/`, neither of which is committed to version control:

- `mlp.onnx` — the model, validated against `onnx.checker` before being written.
- `input.bin` — a single `(1, 4)` float32 input tensor, serialized as 16 raw bytes with no header.

The script uses fixed random seeds and is idempotent: running it repeatedly produces byte-identical output.

## Dependencies

Fetched automatically via CMake `FetchContent`:

| Library | Version | Purpose |
|---|---|---|
| Protobuf | 31.1 | Serialization format underlying ONNX's file format |
| ONNX | 1.23.0 | ONNX model schema, protobuf definitions, and model checker |
| GoogleTest | 1.15.2 | Unit testing framework |

Vendored directly in the repository (no network access required to build):

| Library | Version | Location |
|---|---|---|
| cxxopts | 3.3.1 | `src/onnxcc/third_party/cxxopts.hpp` |

Python dependencies for the fixture generator are pinned in `requirements.txt`.

## Commit Conventions

This repository follows [Conventional Commits](https://www.conventionalcommits.org/). Commit messages take the form `type(scope): description`, for example `feat(cli): implement dump subcommand and top-level argument parsing`. Common types used in this project:

| Type | Meaning |
|---|---|
| `feat` | A new feature |
| `fix` | A bug fix |
| `build` | Changes to the build system or external dependencies |
| `test` | Adding or changing tests |
| `docs` | Documentation or comment changes only |
| `chore` | Routine maintenance not covered by the above |

## Roadmap

The following are planned but not yet implemented:

- Actual model loading and validation in `dump`, including honoring `--show-graph` and `--verbose`.
- Additional subcommands: `run`, `compile`, `benchmark`.
- Model execution matching the output of ONNX Runtime within a numerical tolerance of 1e-4.