# onnx 1.19 requires a minimum of 3.24 version of cmake
cmake_minimum_required(VERSION 3.24)

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
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4 /permissive-)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_subdirectory(src)