#include "check_functionlike.h"

#include <iostream>
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
        bool checkParamList(const std::vector<const cppast::CppEntity*>& params, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
        {
            for (const auto& param : params)
            {
                switch (param->entityType())
                {
                case cppast::CppEntityType::VAR:
                    RETURN_IF_FALSE(checkVar(*static_cast<const cppast::CppVar*>(param), stateStructName, scopeStack));
                    break;
                case cppast::CppEntityType::FUNCTION_PTR:
                    std::cout << "function pointer is not allowed!" << std::endl;
                    return false;
                default:
                    std::cout << "unknown CppEntity type in parameter list" << std::endl;
                    return false;
                }
            }
            return true;
        }

    }  // namespace

    bool checkTypeConverter(const cppast::CppTypeConverter& converter, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
    {
        if (converter.isTemplated())
            RETURN_IF_FALSE(checkTemplSpec(converter.templateSpecification().value(), stateStructName, scopeStack));

        RETURN_IF_FALSE(checkVarType(*converter.targetType(), stateStructName, scopeStack));

        if (converter.defn())
            RETURN_IF_FALSE(checkCompound(*converter.defn(), stateStructName, scopeStack));

        return true;
    }

    bool checkConstructor(const cppast::CppConstructor& constr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
    {
        if (constr.isTemplated())
            RETURN_IF_FALSE(checkTemplSpec(constr.templateSpecification().value(), stateStructName, scopeStack));

        const auto params = GetAllParams(constr);
        if (!params.empty())
            RETURN_IF_FALSE(checkParamList(params, stateStructName, scopeStack));

        if (constr.hasMemberInitList())
        {
            for (const auto& memInit : constr.memberInits())
            {
                RETURN_IF_FALSE(isNameAllowed(memInit.memberName));
                const std::vector<std::unique_ptr<cppast::CppExpression>>& memberInitArgs = memInit.memberInitInfo.args;
                for (const auto& arg : memberInitArgs)
                    RETURN_IF_FALSE(checkExpr(*arg, stateStructName, scopeStack));
            }
        }

        if (constr.defn())
            RETURN_IF_FALSE(checkCompound(*constr.defn(), stateStructName, scopeStack));

        return true;
    }

    bool checkDestructor(const cppast::CppDestructor& destr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
    {
        if (destr.isTemplated())
            RETURN_IF_FALSE(checkTemplSpec(destr.templateSpecification().value(), stateStructName, scopeStack));

        if (destr.defn())
            RETURN_IF_FALSE(checkCompound(*destr.defn(), stateStructName, scopeStack));

        return true;
    }

    bool checkFunction(const cppast::CppFunction& func, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
    {
        if (func.isTemplated())
            RETURN_IF_FALSE(checkTemplSpec(func.templateSpecification().value(), stateStructName, scopeStack));

        if (func.returnType())
            RETURN_IF_FALSE(checkVarType(*func.returnType(), stateStructName, scopeStack));

        RETURN_IF_FALSE(isNameAllowed(func.name()));

        const auto params = GetAllParams(func);
        if (!params.empty())
            RETURN_IF_FALSE(checkParamList(params, stateStructName, scopeStack));

        if (func.defn())
            RETURN_IF_FALSE(checkCompound(*func.defn(), stateStructName, scopeStack));

        return true;
    }

    bool checkReturn(const cppast::CppReturnStatement& returnStatement, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
    {
        if (returnStatement.hasReturnValue())
            RETURN_IF_FALSE(checkExpr(returnStatement.returnValue(), stateStructName, scopeStack));
        return true;
    }

}  // namespace contractverify