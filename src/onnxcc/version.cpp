#include "onnxcc/version.h"
#include <string_view>

namespace onnxcc {
    std::string_view get_version() {
        return ONNXCC_VERSION;
    }

    std::string_view get_version_codename() {
        return ONNXCC_VERSION_CODENAME;
    }
}