#include <gtest/gtest.h>

#include "check_compliance.h"


TEST(ContractVerifyTest, ParsingWorks) {
    std::string inputPath = "D:\\Programming\\Repos\\qubic-contract-verify\\test\\testfiles\\test_ok.h";
    std::unique_ptr<cppast::CppCompound> ast = contractverify::parseAST(inputPath);
    EXPECT_NE(ast, nullptr);
}