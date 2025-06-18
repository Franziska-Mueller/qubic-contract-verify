#include <filesystem>

#include <gtest/gtest.h>

#include "check_compliance.h"
#include "test_files_config.h"

namespace contractverify
{
    struct FailureTestInfo
    {
        std::string filename;
        std::string expectedErrorMessage;
        std::string testName;
    };

    class ContractVerifyFailureTest : public testing::TestWithParam<FailureTestInfo> { };

    TEST(ContractVerifyTest, ParsingWorks) {
        std::filesystem::path filepath = std::filesystem::path(testfiles::baseDir).append("test_ok.h");
        std::unique_ptr<cppast::CppCompound> ast = contractverify::parseAST(filepath.string());
        EXPECT_NE(ast, nullptr);

        // TODO: find a "real" contract example that actually works, QUTIL breaks some of the rules, e.g. global constant names
        //std::string stateStructName = contractverify::findStateStructName(*ast);
        //EXPECT_EQ(stateStructName, "QUTIL");
        //EXPECT_EQ(contractverify::checkCompliance(*ast, stateStructName), true);
    }

    TEST_P(ContractVerifyFailureTest, FailsWithExpectedError) {
        const FailureTestInfo& info = GetParam();
        std::filesystem::path filepath = std::filesystem::path(testfiles::baseDir).append(info.filename);
        std::unique_ptr<cppast::CppCompound> ast = contractverify::parseAST(filepath.string());

        std::string stateStructName = contractverify::findStateStructName(*ast);
        EXPECT_EQ(stateStructName, "TESTCON");

        EXPECT_EQ(contractverify::checkCompliance(*ast, stateStructName), false);

        // TODO: capture std::cout and check that output is equal to expected error message
    }

    FailureTestInfo failureTestInfos[] = {
        {"test_fail_variadic_argument.h","variadic arguments are not allowed!", "VaridicArguments"},
        {"test_fail_parameter_pack.h", "", "ParameterPack"},
        {"test_fail_array_declaration.h", "", "ArrayDeclaration"},
        {"test_fail_array_indexing.h", "", "ArrayIndexing"},
    };

    INSTANTIATE_TEST_SUITE_P(CVFT,
        ContractVerifyFailureTest,
        testing::ValuesIn(failureTestInfos),
        [](const testing::TestParamInfo<ContractVerifyFailureTest::ParamType>& info)
        { return info.param.testName; }
    );

}  // namespace contractverify