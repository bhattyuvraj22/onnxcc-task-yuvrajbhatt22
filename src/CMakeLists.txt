configure_file(onnxcc/version.h.in onnxcc/version.h @ONLY)

add_library(onnxcc_lib STATIC
    onnxcc/version.cpp
)

add_executable(onnxcc onnxcc/main.cpp
    onnxcc/main.cpp
)

# Link the onnxcc_lib library to the onnxcc executable
target_link_libraries(onnxcc PRIVATE onnxcc_lib)

# Include the current source directory for the onnxcc_lib target
target_include_directories(onnxcc_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)