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
        bool checkVarDecl(const cppast::CppVarDecl& varDecl, const std::string& stateStructName, AnalysisData& analysisData)
        {
            RETURN_IF_FALSE(isNameAllowed(varDecl.name(), analysisData.additionalScopePrefixes));
            if (analysisData.scopeStack.empty()) // global constant name has to start with stateStructName
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
                RETURN_IF_FALSE(checkExpr(*varDecl.assignValue(), stateStructName, analysisData));
            }
            else if (varDecl.initializeType() == cppast::CppVarInitializeType::DIRECT_CONSTRUCTOR_CALL)
            {
                for (const auto& expr : varDecl.constructorCallArgs())
                    RETURN_IF_FALSE(checkExpr(*expr, stateStructName, analysisData));
            }

            return true;
        }

    }  // namespace

    bool checkVarType(const cppast::CppVarType& varType, const std::string& stateStructName, AnalysisData& analysisData)
    {
        // if global scope this has to be const or constexpr
        if (analysisData.scopeStack.empty())
        {
            const auto attr = varType.typeAttr() | (IsConst(varType) ? cppast::CppIdentifierAttrib::CONST : 0);
            if (!(attr & cppast::CppIdentifierAttrib::CONST || attr & cppast::CppIdentifierAttrib::CONST_EXPR))
            {
                std::cout << "[ ERROR ] Global variables are not allowed. You may use global constants (const/constexpr)." << std::endl;
                return false;
            }
        }

        if (varType.compound())
        {
            RETURN_IF_FALSE(checkEntity(*varType.compound(), stateStructName, analysisData))
            // unnamed structs as member types are currently not allowed in IO types
            if (analysisData.isDirectlyInClassOrStruct())
            {
                analysisData.allowedAsIOStruct.top() = false;
            }
        }
        else
        {
            RETURN_IF_FALSE(isTypeAllowed(varType.baseType(), analysisData.additionalScopePrefixes))
            if (analysisData.isDirectlyInClassOrStruct())
            {
                if (!isTypeAllowedAsIO(varType.baseType(), analysisData))
                {
                    analysisData.allowedAsIOStruct.top() = false;
                }
            }
        }

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

    bool checkVar(const cppast::CppVar& var, const std::string& stateStructName, AnalysisData& analysisData)
    {
        if (!(analysisData.scopeStack.empty() || analysisData.scopeStack.top() == ScopeSpec::STRUCT || analysisData.scopeStack.top() == ScopeSpec::CLASS
            || analysisData.scopeStack.top() == ScopeSpec::FUNC_SIG || analysisData.scopeStack.top() == ScopeSpec::TYPEDEF))
        {
            std::cout << "[ ERROR ] Local variables are not allowed, found variable with name " << var.name() << "." << std::endl;
            return false;
        }

        if (var.isTemplated())
        {
            RETURN_IF_FALSE(checkTemplSpec(var.templateSpecification().value(), stateStructName, analysisData));
            if (analysisData.isDirectlyInClassOrStruct())
            {
                // templated variable is not allowed in IO type
                analysisData.allowedAsIOStruct.top() = false;
            }
        }

        RETURN_IF_FALSE(checkVarType(var.varType(), stateStructName, analysisData));
        RETURN_IF_FALSE(checkVarDecl(var.varDecl(), stateStructName, analysisData));

        return true;
    }

    bool checkVarList(const cppast::CppVarList& varList, const std::string& stateStructName, AnalysisData& analysisData)
    {
        RETURN_IF_FALSE(checkVar(*varList.firstVar(), stateStructName, analysisData));
        auto& varDeclList = varList.varDeclList();
        for (const auto& decl : varDeclList)
        {
            if (decl.ptrLevel_ > 0)
            {
                std::cout << "[ ERROR ] Pointers are not allowed." << std::endl;
                return false;
            }
            RETURN_IF_FALSE(checkVarDecl(decl, stateStructName, analysisData));
        }

        return true;
    }

}  // namespace contractverify