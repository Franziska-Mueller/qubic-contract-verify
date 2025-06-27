#include "check_names_and_types.h"

#include <iostream>
#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "check_expressions.h"
#include "check_variables.h"
#include "defines.h"


namespace contractverify
{
    bool isInheritanceAllowed(const std::string& baseName)
    {
        if (baseName.compare("QpiContext") == 0)
        {
            std::cout << "[ ERROR ] Inheritance from type " << baseName << " is not allowed." << std::endl;
            return false;
        }
        return true;
    }

    bool isNameAllowed(const std::string& name)
    {
        // TODO: scope resolution :: -> only structs, enums, namespaces defined in contracts and qpi.h

        // names starting with double underscores are reserved for internal functions and compiler macros
        if (name.compare(0, 2, "__") == 0)
        {
            std::cout << "[ ERROR ] Names starting with double underscores are reserved." << std::endl;
            return false;
        }
        // variadic arguments are not allowed and parsed with a name ending in ...
        if (name.length() >= 3 && name.compare(name.length() - 3, 3, "...") == 0)
        {
            std::cout << "[ ERROR ] Variadic arguments are not allowed." << std::endl;
            return false;
        }
        return true;
    }

    bool isTypeAllowed(const std::string& type)
    {
        // TODO: scope resolution :: -> only structs, enums, namespaces defined in contracts and qpi.h

        if (type.length() >= 3 && type.compare(type.length() - 3, 3, "...") == 0)
        {
            std::cout << "[ ERROR ] Variadic arguments or parameter packs are not allowed." << std::endl;
            return false;
        }
        std::vector<std::string> forbiddenTypes = { "float", "double", "string", "char", "QpiContext" };
        for (const auto& forbiddenType : forbiddenTypes)
        {
            if (type.find(forbiddenType) != std::string::npos)
            {
                std::cout << "[ ERROR ] Type " << forbiddenType << " is not allowed." << std::endl;
                return false;
            }
        }

        return true;
    }

    bool hasStateStructPrefix(const std::string& name, const std::string& stateStructName)
    {
        if (name.compare(0, stateStructName.length(), stateStructName) != 0)
        {
            std::cout << "[ ERROR ] Names declared in global scope (constants, structs/classes, functions) have to start with state struct name ("
                << stateStructName << "). Found invalid name: " << name << std::endl;
            return false;
        }
        return true;
    }

    bool isScopeResolutionAllowed(const std::string& name)
    {
        if (name.find("::") != std::string::npos)
        {
            // TODO
        }
        return true;
    }

    bool checkTemplSpec(const cppast::CppTemplateParams& params, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
    {
        scopeStack.push(ScopeSpec::TEMPL_SPEC);

        for (const auto& param : params)
        {
            if (param.paramType().has_value())
            {
                RETURN_IF_FALSE(
                    std::visit(Overloaded{
                            [&](const std::unique_ptr<cppast::CppVarType>& varType) -> bool
                            {
                                return checkVarType(*varType, stateStructName, scopeStack);
                            },
                            [&](const std::unique_ptr<cppast::CppFunctionPointer>& funcPtr) -> bool
                            {
                                std::cout << "[ ERROR ] Function pointers are not allowed." << std::endl;
                                return false;
                            }
                        },
                        param.paramType().value()
                    )
                );
            }

            RETURN_IF_FALSE(isNameAllowed(param.paramName()));

            RETURN_IF_FALSE(
                std::visit(Overloaded{
                        [&](const std::unique_ptr<cppast::CppVarType>& varType) -> bool
                        {
                            if (varType)
                                return checkVarType(*varType, stateStructName, scopeStack);
                            else
                                return true;
                        },
                        [&](const std::unique_ptr<cppast::CppExpression>& expr) -> bool
                        {
                            if (expr)
                                return checkExpr(*expr, stateStructName, scopeStack);
                            else
                                return true;
                        }
                    },
                    param.defaultArg()
                )
            );
        }

        scopeStack.pop();
        return true;
    }

    bool checkTypedef(const cppast::CppTypedefName& def, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
    {
        if (scopeStack.empty())
        {
            std::cout << "[ ERROR ] `typedef` is not allowed in global scope." << std::endl;
            return false;
        }

        scopeStack.push(ScopeSpec::TYPEDEF);
        RETURN_IF_FALSE(checkVar(*def.var(), stateStructName, scopeStack));
        scopeStack.pop();

        return true;
    }

    bool checkTypedefList(const cppast::CppTypedefList& defList, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
    {
        if (scopeStack.empty())
        {
            std::cout << "[ ERROR ] `typedef` is not allowed in global scope." << std::endl;
            return false;
        }

        scopeStack.push(ScopeSpec::TYPEDEF);
        RETURN_IF_FALSE(checkVarList(defList.varList(), stateStructName, scopeStack));
        scopeStack.pop();

        return true;
    }

}  // namespace contractverify