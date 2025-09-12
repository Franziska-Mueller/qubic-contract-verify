#include "check_functionlike.h"

#include <iostream>
#include <ranges>
#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "check_compliance.h"
#include "check_expressions.h"
#include "check_names_and_types.h"
#include "check_variables.h"
#include "defines.h"


namespace contractverify
{
    namespace
    {
        bool checkParamList(const std::vector<const cppast::CppEntity*>& params, const std::string& stateStructName, AnalysisData& analysisData)
        {
            for (const auto& param : params)
            {
                switch (param->entityType())
                {
                case cppast::CppEntityType::VAR:
                    RETURN_IF_FALSE(checkVar(*static_cast<const cppast::CppVar*>(param), stateStructName, analysisData));
                    break;
                case cppast::CppEntityType::FUNCTION_PTR:
                    std::cout << "[ ERROR ] Function pointers are not allowed." << std::endl;
                    return false;
                default:
                    std::cout << "[ ERROR ] Unknown CppEntityType encountered in parameter list." << std::endl;
                    return false;
                }
            }
            return true;
        }

    }  // namespace

    bool checkTypeConverter(const cppast::CppTypeConverter& converter, const std::string& stateStructName, AnalysisData& analysisData)
    {
        if (converter.isTemplated())
            RETURN_IF_FALSE(checkTemplSpec(converter.templateSpecification().value(), stateStructName, analysisData));

        RETURN_IF_FALSE(checkVarType(*converter.targetType(), stateStructName, analysisData));

        if (converter.defn())
            RETURN_IF_FALSE(checkCompound(*converter.defn(), stateStructName, analysisData));

        return true;
    }

    bool checkFunction(const cppast::CppFunction& func, const std::string& stateStructName, AnalysisData& analysisData)
    {
        if (analysisData.scopeStack.empty()) // global function name has to start with stateStructName
            RETURN_IF_FALSE(hasStateStructPrefix(func.name(), stateStructName));
        
        analysisData.scopeStack.push(ScopeSpec::FUNC_SIG);

        if (func.isTemplated())
            RETURN_IF_FALSE(checkTemplSpec(func.templateSpecification().value(), stateStructName, analysisData));

        if (func.returnType())
            RETURN_IF_FALSE(checkVarType(*func.returnType(), stateStructName, analysisData));

        RETURN_IF_FALSE(isNameAllowed(func.name(), analysisData.additionalScopePrefixes));

        const auto params = GetAllParams(func);
        if (!params.empty())
            RETURN_IF_FALSE(checkParamList(params, stateStructName, analysisData));

        if (func.defn())
            RETURN_IF_FALSE(checkCompound(*func.defn(), stateStructName, analysisData));

        analysisData.scopeStack.pop();
        return true;
    }

    bool checkReturn(const cppast::CppReturnStatement& returnStatement, const std::string& stateStructName, AnalysisData& analysisData)
    {
        if (returnStatement.hasReturnValue())
            RETURN_IF_FALSE(checkExpr(returnStatement.returnValue(), stateStructName, analysisData));
        return true;
    }

    bool checkLambda(const cppast::CppLambda& lambda, const std::string& stateStructName, AnalysisData& analysisData)
    {
        analysisData.scopeStack.push(ScopeSpec::FUNC_SIG);

        if (lambda.captures())
            RETURN_IF_FALSE(checkExpr(*lambda.captures(), stateStructName, analysisData));

        if (lambda.returnType())
            RETURN_IF_FALSE(checkVarType(*lambda.returnType(), stateStructName, analysisData));

        const std::vector<std::unique_ptr<cppast::CppEntity>>& params = lambda.params();
        if (!params.empty())
        {
            auto getPtr = [](const std::unique_ptr<cppast::CppEntity>& uptr) -> const cppast::CppEntity*
                { return uptr.get(); };
            auto paramPtrs = params | std::views::transform(getPtr);
            // update this with std::ranges::to<std::vector> once we use C++23
            RETURN_IF_FALSE(checkParamList(std::vector<const cppast::CppEntity*>(paramPtrs.begin(), paramPtrs.end()), stateStructName, analysisData));
        }

        if (lambda.defn())
            RETURN_IF_FALSE(checkCompound(*lambda.defn(), stateStructName, analysisData));

        analysisData.scopeStack.pop();
        return true;
    }

}  // namespace contractverify