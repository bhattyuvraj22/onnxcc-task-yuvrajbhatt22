# onnx 1.23 requires a minimum of 3.26 version of cmake
cmake_minimum_required(VERSION 3.26)

project(onnxcc VERSION 0.1.0 LANGUAGES CXX)
set(ONNXCC_VERSION_CODENAME "Tumjang")

# Export compile commands for static analysis tools like clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# requires c++20 and no compiler extensions
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# avoid -Weverything is too verbose
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(ONNXCC_WARNINGS -Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(ONNXCC_WARNINGS /W4 /permissive-)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_IGNORE_PREFIX_PATH "/opt/anaconda3")

include(FetchContent)

# Disable tests and shared libraries for protobuf subproject
set(protobuf_BUILD_TESTS OFF CACHE BOOL "Disable tests for subproject" FORCE)
set(protobuf_BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries for subproject" FORCE)

# To match onnx 1.23 requirements
# Fetch it first to avoid conflicts with the system-installed protobuf
FetchContent_Declare(
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG v31.1
    GIT_SHALLOW TRUE
    SYSTEM
)
FetchContent_MakeAvailable(protobuf)

# Disable tests and shared libraries for onnx subproject and to enable machine independent code generation
set(ONNX_BUILD_TESTS OFF CACHE BOOL "Disable tests for subproject" FORCE)
set(ONNX_BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries for subproject" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FetchContent_Declare(
    onnx
    GIT_REPOSITORY https://github.com/onnx/onnx.git
    GIT_TAG v1.23.0
    GIT_SHALLOW TRUE
    SYSTEM
)
FetchContent_MakeAvailable(onnx)

add_subdirectory(src)