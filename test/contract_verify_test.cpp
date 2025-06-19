#include <filesystem>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

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

    class ContractVerifyFailureTest : public testing::TestWithParam<FailureTestInfo>
    { 
    protected:
        // capture std::cout in the custom buffer during the test
        std::stringstream customBuffer;
        std::streambuf* originalBuffer = nullptr;

    public:
        void SetUp() override
        {
            originalBuffer = std::cout.rdbuf();
            std::cout.rdbuf(customBuffer.rdbuf());
        }

        void TearDown() override
        {
            std::cout.rdbuf(originalBuffer);
        }        
    };

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

        EXPECT_FALSE(contractverify::checkCompliance(*ast, stateStructName));
        EXPECT_THAT(customBuffer.str(), testing::StrEq(info.expectedErrorMessage));
    }

    FailureTestInfo failureTestInfos[] = {
        {
            "test_fail_variadic_argument.h",
            "[ ERROR ] Variadic arguments are not allowed.\n",
            "VaridicArguments"
        },
        {
            "test_fail_parameter_pack.h",
            "[ ERROR ] Parameter packs are not allowed.\n",
            "ParameterPack"
        },
        {
            "test_fail_array_declaration.h",
            "[ ERROR ] Plain arrays are not allowed, use the Array class provided by the QPI instead.\n",
            "ArrayDeclaration"
        },
        {
            "test_fail_array_indexing.h",
            "[ ERROR ] Plain arrays are not allowed, use the Array class provided by the QPI instead.\n",
            "ArrayIndexing"
        },
    };

    INSTANTIATE_TEST_SUITE_P(CVFT,
        ContractVerifyFailureTest,
        testing::ValuesIn(failureTestInfos),
        [](const testing::TestParamInfo<ContractVerifyFailureTest::ParamType>& info)
        { return info.param.testName; }
    );

}  // namespace contractverify