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
        ASSERT_NE(ast, nullptr);

        //// TODO: find a "real" contract example that actually works, QUTIL breaks some of the rules, e.g. global constant names
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
        {
            "test_ok_using_namespace_local.h",
            "UsingNamespaceLocal"
        },
        {
            "test_ok_forward_declaration.h",
            "ForwardDeclaration"
        },
        {
            "test_ok_forward_declaration_templated.h",
            "ForwardDeclarationTemplated"
        },
        {
            "test_ok_if_block.h",
            "IfBlock"
        },
        {
            "test_ok_for_block.h",
            "ForBlock"
        },
        {
            "test_ok_while_block.h",
            "WhileBlock"
        },
        {
            "test_ok_do_while_block.h",
            "DoWhileBlock"
        },
        {
            "test_ok_switch_block.h",
            "SwitchBlock"
        },
        {
            "test_ok_goto.h",
            "Goto"
        },
        {
            "test_ok_global_constant.h",
            "GlobalConstant"
        },
        {
            "test_ok_lambda.h",
            "Lambda"
        },
        {
            "test_ok_function.h",
            "Function"
        },
        {
            "test_ok_function_templated.h",
            "FunctionTemplated"
        },
        {
            "test_ok_typedef_local.h",
            "TypedefLocal"
        },
        {
            "test_ok_scope_resolution_using_declaration.h",
            "ScopeResolutionUsingDeclaration"
        },
        {
            "test_ok_scope_resolution_local_enum.h",
            "ScopeResolutionLocalEnum"
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
            "[ ERROR ] Variadic arguments or parameter packs are not allowed.\n",
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
            "[ ERROR ] String literals are not allowed, found \"I am a string literal\".\n",
            "StringLiteral"
        },
        {
            "test_fail_char_literal.h",
            "[ ERROR ] Char literals are not allowed, found 'c'.\n",
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
            "test_fail_constructor.h",
            "[ ERROR ] Constructors are not allowed.\n",
            "Constructor"
        },
        {
            "test_fail_destructor.h",
            "[ ERROR ] Destructors are not allowed.\n",
            "Destructor"
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
        {
            "test_fail_using_namespace_global.h",
            "[ ERROR ] Only QPI can be used for a using namespace declaration in global scope.\n",
            "UsingNamespaceGlobal"
        },
        {
            "test_fail_using_declaration_global.h",
            "[ ERROR ] Using declaration is not allowed in global scope.\n",
            "UsingDeclarationGlobal"
        },
        {
            "test_fail_union.h",
            "[ ERROR ] `union` is not allowed.\n",
            "Union"
        },
        {
            "test_fail_preprocessor_include.h",
            "[ ERROR ] Preprocessor directives (character `#`) are not allowed.\n",
            "PreprocessorInclude"
        },
        {
            "test_fail_preprocessor_define.h",
            "[ ERROR ] Preprocessor directives (character `#`) are not allowed.\n",
            "PreprocessorDefine"
        },
        {
            "test_fail_typedef_global.h",
            "[ ERROR ] `typedef` is not allowed in global scope.\n",
            "TypedefGlobal"
        },
        {
            "test_fail_typedef_list_global.h",
            "[ ERROR ] `typedef` is not allowed in global scope.\n",
            "TypedefListGlobal"
        },
        {
            "test_fail_typedef_forbidden_type.h",
            "[ ERROR ] Pointers are not allowed.\n",
            "TypedefForbiddenType"
        },
        {
            "test_fail_typedef_list_forbidden_type.h",
            "[ ERROR ] Pointers are not allowed.\n",
            "TypedefListForbiddenType"
        },
        {
            "test_fail_namespace_alias.h",
            "[ ERROR ] Namespace alias is not allowed.\n",
            "NamespaceAlias"
        },
        {
            "test_fail_function_pointer.h",
            "[ ERROR ] Function pointers are not allowed.\n",
            "FunctionPointer"
        },
        {
            "test_fail_throw.h",
            "[ ERROR ] `throw` statement is not allowed.\n",
            "Throw"
        },
        {
            "test_fail_inheritance.h",
            "[ ERROR ] Inheritance from type QpiContext is not allowed.\n",
            "Inheritance"
        },
        {
            "test_fail_name_compound.h",
            "[ ERROR ] Names starting with double underscores are reserved.\n",
            "NameCompound"
        },
        {
            "test_fail_name_function.h",
            "[ ERROR ] Names starting with double underscores are reserved.\n",
            "NameFunction"
        },
        {
            "test_fail_name_param.h",
            "[ ERROR ] Names starting with double underscores are reserved.\n",
            "NameParam"
        },
        {
            "test_fail_name_var.h",
            "[ ERROR ] Names starting with double underscores are reserved.\n",
            "NameVar"
        },
        {
            "test_fail_type_float.h",
            "[ ERROR ] Type float is not allowed.\n",
            "TypeFloat"
        },
        {
            "test_fail_type_double.h",
            "[ ERROR ] Type double is not allowed.\n",
            "TypeDouble"
        },
        {
            "test_fail_type_char.h",
            "[ ERROR ] Type char is not allowed.\n",
            "TypeChar"
        },
        {
            "test_fail_global_constant.h",
            "[ ERROR ] Names declared in global scope (constants, structs/classes, functions) have to start with state struct name (TESTCON). Found invalid name: DOES_NOT_START_WITH_STATE_STRUCT_NAME\n",
            "GlobalConstant"
        },
        {
            "test_fail_global_variable.h",
            "[ ERROR ] Global variables are not allowed. You may use global constants (const/constexpr).\n",
            "GlobalVariable"
        },
        {
            "test_fail_global_function.h",
            "[ ERROR ] Names declared in global scope (constants, structs/classes, functions) have to start with state struct name (TESTCON). Found invalid name: doesNotStartWithStateStructName\n",
            "GlobalFunction"
        },
        {
            "test_fail_global_struct.h",
            "[ ERROR ] Names declared in global scope (constants, structs/classes, functions) have to start with state struct name (TESTCON). Found invalid name: DoesNotStartWithStateStructName\n",
            "GlobalStruct"
        },
        {
            "test_fail_global_class.h",
            "[ ERROR ] Names declared in global scope (constants, structs/classes, functions) have to start with state struct name (TESTCON). Found invalid name: DoesNotStartWithStateStructName\n",
            "GlobalClass"
        },
        {
            "test_fail_pointer_declaration.h",
            "[ ERROR ] Pointers are not allowed.\n",
            "PointerDeclaration"
        },
        {
            "test_fail_local_variable.h",
            "[ ERROR ] Local variables are not allowed, found variable with name fee.\n",
            "LocalVariable"
        },
        {
            "test_fail_scope_resolution_function_call.h",
            "[ ERROR ] Scope resolution with prefix someNamespace is not allowed.\n",
            "ScopeResolutionFunctionCall"
        },
        {
            "test_fail_scope_resolution_variable.h",
            "[ ERROR ] Scope resolution with prefix myNumbers is not allowed.\n",
            "ScopeResolutionVariable"
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