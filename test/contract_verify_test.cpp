#include <filesystem>

#include <gtest/gtest.h>

#include "check_compliance.h"
#include "test_files_config.h"


TEST(ContractVerifyTest, ParsingWorks) {
    std::filesystem::path filepath = std::filesystem::path(testfiles::baseDir).append("test_ok.h");
    std::unique_ptr<cppast::CppCompound> ast = contractverify::parseAST(filepath.string());
    EXPECT_NE(ast, nullptr);
}

TEST(ContractVerifyTest, FailVariadicArgs) {
    std::filesystem::path filepath = std::filesystem::path(testfiles::baseDir).append("test_fail_variadic_argument.h");
    std::unique_ptr<cppast::CppCompound> ast = contractverify::parseAST(filepath.string());

    std::string stateStructName = "TESTCON";
    // std::string stateStructName = findStateStructName(*ast);

    EXPECT_EQ(contractverify::checkCompliance(*ast, stateStructName), false);
}

TEST(ContractVerifyTest, FailParameterPack) {
    std::filesystem::path filepath = std::filesystem::path(testfiles::baseDir).append("test_fail_parameter_pack.h");
    std::unique_ptr<cppast::CppCompound> ast = contractverify::parseAST(filepath.string());

    std::string stateStructName = "TESTCON";
    // std::string stateStructName = findStateStructName(*ast);

    EXPECT_EQ(contractverify::checkCompliance(*ast, stateStructName), false);
}