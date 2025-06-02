#include <filesystem>

#include <gtest/gtest.h>

#include "check_compliance.h"
#include "test_files_config.h"


TEST(ContractVerifyTest, ParsingWorks) {
    std::filesystem::path filepath = std::filesystem::path(testfiles::baseDir).append("test_ok.h");
    std::unique_ptr<cppast::CppCompound> ast = contractverify::parseAST(filepath.string());
    EXPECT_NE(ast, nullptr);
}