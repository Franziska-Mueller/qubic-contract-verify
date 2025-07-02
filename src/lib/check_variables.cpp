#include "check_variables.h"

#include <iostream>
#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "check_compliance.h"
#include "check_expressions.h"
#include "check_names_and_types.h"
#include "defines.h"


namespace contractverify
{
    namespace
    {
        bool checkVarDecl(const cppast::CppVarDecl& varDecl, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
        {
            RETURN_IF_FALSE(isNameAllowed(varDecl.name(), additionalScopePrefixes));
            if (scopeStack.empty()) // global constant name has to start with stateStructName
                RETURN_IF_FALSE(hasStateStructPrefix(varDecl.name(), stateStructName));

            if (!varDecl.arraySizes().empty())
            {
                std::cout << "[ ERROR ] Plain arrays are not allowed, use the Array class provided by the QPI instead." << std::endl;
                return false;
            }

            if (!varDecl.isInitialized())
                return true;

            if (varDecl.initializeType() == cppast::CppVarInitializeType::USING_EQUAL)
            {
                RETURN_IF_FALSE(checkExpr(*varDecl.assignValue(), stateStructName, scopeStack, additionalScopePrefixes));
            }
            else if (varDecl.initializeType() == cppast::CppVarInitializeType::DIRECT_CONSTRUCTOR_CALL)
            {
                for (const auto& expr : varDecl.constructorCallArgs())
                    RETURN_IF_FALSE(checkExpr(*expr, stateStructName, scopeStack, additionalScopePrefixes));
            }

            return true;
        }

    }  // namespace

    bool checkVarType(const cppast::CppVarType& varType, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        // if global scope this has to be const or constexpr
        if (scopeStack.empty())
        {
            const auto attr = varType.typeAttr() | (IsConst(varType) ? cppast::CppIdentifierAttrib::CONST : 0);
            if (!(attr & cppast::CppIdentifierAttrib::CONST || attr & cppast::CppIdentifierAttrib::CONST_EXPR))
            {
                std::cout << "[ ERROR ] Global variables are not allowed. You may use global constants (const/constexpr)." << std::endl;
                return false;
            }
        }

        if (varType.compound())
            RETURN_IF_FALSE(checkEntity(*varType.compound(), stateStructName, scopeStack, additionalScopePrefixes))
        else
            RETURN_IF_FALSE(isTypeAllowed(varType.baseType(), additionalScopePrefixes))

        if (varType.typeModifier().ptrLevel_ > 0)
        {
            std::cout << "[ ERROR ] Pointers are not allowed." << std::endl;
            return false;
        }

        if (varType.parameterPack() || varType.baseType().find("...") != std::string::npos)
        {
            std::cout << "[ ERROR ] Parameter packs are not allowed." << std::endl;
            return false;
        }

        return true;
    }

    bool checkVar(const cppast::CppVar& var, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        // TODO: input and output structs only use basic types

        if (!(scopeStack.empty() || scopeStack.top() == ScopeSpec::STRUCT || scopeStack.top() == ScopeSpec::CLASS 
            || scopeStack.top() == ScopeSpec::FUNC_SIG || scopeStack.top() == ScopeSpec::TYPEDEF))
        {
            std::cout << "[ ERROR ] Local variables are not allowed." << std::endl;
            return false;
        }

        if (var.isTemplated())
            RETURN_IF_FALSE(checkTemplSpec(var.templateSpecification().value(), stateStructName, scopeStack, additionalScopePrefixes));

        RETURN_IF_FALSE(checkVarType(var.varType(), stateStructName, scopeStack, additionalScopePrefixes));
        RETURN_IF_FALSE(checkVarDecl(var.varDecl(), stateStructName, scopeStack, additionalScopePrefixes));

        return true;
    }

    bool checkVarList(const cppast::CppVarList& varList, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        RETURN_IF_FALSE(checkVar(*varList.firstVar(), stateStructName, scopeStack, additionalScopePrefixes));
        auto& varDeclList = varList.varDeclList();
        for (const auto& decl : varDeclList)
        {
            if (decl.ptrLevel_ > 0)
            {
                std::cout << "[ ERROR ] Pointers are not allowed." << std::endl;
                return false;
            }
            RETURN_IF_FALSE(checkVarDecl(decl, stateStructName, scopeStack, additionalScopePrefixes));
        }

        return true;
    }

}  // namespace contractverify