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

    struct SuccessTestInfo
    {
        std::string filename;
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

    class ContractVerifySuccessTest : public testing::TestWithParam<SuccessTestInfo> {};

    TEST(ContractVerifyTest, ParsingWorks) {
        std::filesystem::path filepath = std::filesystem::path(testfiles::baseDir).append("test_ok.h");
        std::unique_ptr<cppast::CppCompound> ast = contractverify::parseAST(filepath.string());
        EXPECT_NE(ast, nullptr);

        // TODO: find a "real" contract example that actually works, QUTIL breaks some of the rules, e.g. global constant names
        //std::string stateStructName = contractverify::findStateStructName(*ast);
        //EXPECT_EQ(stateStructName, "QUTIL");
        //EXPECT_EQ(contractverify::checkCompliance(*ast, stateStructName), true);
    }

    TEST_P(ContractVerifySuccessTest, PassesComplianceCheck) {
        const SuccessTestInfo& info = GetParam();
        std::filesystem::path filepath = std::filesystem::path(testfiles::baseDir).append(info.filename);
        std::unique_ptr<cppast::CppCompound> ast = contractverify::parseAST(filepath.string());

        std::string stateStructName = contractverify::findStateStructName(*ast);
        EXPECT_EQ(stateStructName, "TESTCON");

        EXPECT_TRUE(contractverify::checkCompliance(*ast, stateStructName));
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

    SuccessTestInfo successTestInfos[] = {
        {
            "test_ok_function_call.h",
            "FunctionCall"
        },
        {
            "test_ok_initializer_list.h",
            "InitializerList"
        },
        {
            "test_ok_cstyle_cast.h",
            "CStyleCast"
        },
    };

    FailureTestInfo failureTestInfos[] = {
        {
            "test_fail_variadic_argument.h",
            "[ ERROR ] Variadic arguments are not allowed.\n",
            "VaridicArgument"
        },
        {
            "test_fail_parameter_pack.h",
            "[ ERROR ] Parameter packs are not allowed.\n",
            "ParameterPack"
        },
        {
            "test_fail_variadic_sizeof.h",
            "[ ERROR ] Variadic expressions are not allowed.\n",
            "VariadicSizeof"
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
        {
            "test_fail_string_literal.h",
            "[ ERROR ] String literals are not allowed.\n",
            "StringLiteral"
        },
        {
            "test_fail_char_literal.h",
            "[ ERROR ] Char literals are not allowed.\n",
            "CharLiteral"
        },
        {
            "test_fail_pointer_dereferencing.h",
            "[ ERROR ] Pointer dereferencing (unary operator `*`) is not allowed.\n",
            "PointerDereferencing"
        },
        {
            "test_fail_variable_referencing.h",
            "[ ERROR ] Variable referencing (unary operator `&`) is not allowed.\n",
            "VariableReferencing"
        },
        {
            "test_fail_allocation_new.h",
            "[ ERROR ] Allocation via `new` is not allowed.\n",
            "AllocationNew"
        },
        {
            "test_fail_deallocation_delete.h",
            "[ ERROR ] Deallocation via `delete` is not allowed.\n",
            "DeallocationDelete"
        },
        {
            "test_fail_deallocation_delete_array.h",
            "[ ERROR ] Deallocation via `delete` is not allowed.\n",
            "DeallocationDeleteArray"
        },
        {
            "test_fail_div.h",
            "[ ERROR ] Division operator `/` is not allowed. Use the `div` function provided in the QPI instead.\n",
            "Division"
        },
        {
            "test_fail_div_assign.h",
            "[ ERROR ] Division operator `/` is not allowed. Use the `div` function provided in the QPI instead.\n",
            "DivisionAssign"
        },
        {
            "test_fail_mod.h",
            "[ ERROR ] Modulo operator `%` is not allowed. Use the `mod` function provided in the QPI instead.\n",
            "Modulo"
        },
        {
            "test_fail_mod_assign.h",
            "[ ERROR ] Modulo operator `%` is not allowed. Use the `mod` function provided in the QPI instead.\n",
            "ModuloAssign"
        },
        {
            "test_fail_dereferencing_arrow.h",
            "[ ERROR ] Dereferencing (operator `->` or `->*`) is not allowed.\n",
            "DereferencingArrow"
        },
        {
            "test_fail_dereferencing_arrow_star.h",
            "[ ERROR ] Dereferencing (operator `->` or `->*`) is not allowed.\n",
            "DereferencingArrowStar"
        },
        {
            "test_fail_const_cast.h",
            "[ ERROR ] `const_cast` is not allowed.\n",
            "ConstCast"
        },
    };

    INSTANTIATE_TEST_SUITE_P(CVST,
        ContractVerifySuccessTest,
        testing::ValuesIn(successTestInfos),
        [](const testing::TestParamInfo<ContractVerifySuccessTest::ParamType>& info)
        { return info.param.testName; }
    );

    INSTANTIATE_TEST_SUITE_P(CVFT,
        ContractVerifyFailureTest,
        testing::ValuesIn(failureTestInfos),
        [](const testing::TestParamInfo<ContractVerifyFailureTest::ParamType>& info)
        { return info.param.testName; }
    );

}  // namespace contractverify