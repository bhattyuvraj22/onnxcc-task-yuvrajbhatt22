#include <gtest/gtest.h>
#include "onnxcc/version.h"
#include <string>

// Trivial test to ensure gtest is working
TEST(SanityCheck, GTESTWorks) {
    EXPECT_TRUE(true);
}

// Trivial test to ensure version is not empty
TEST(SanityCheck, VersionCheck) {
    std::string version = std::string(onnxcc::get_version());
    EXPECT_FALSE(version.empty());
}