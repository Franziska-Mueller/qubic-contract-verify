#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool checkTypeConverter(const cppast::CppTypeConverter& converter, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkConstructor(const cppast::CppConstructor& constr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkDestructor(const cppast::CppDestructor& destr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkFunction(const cppast::CppFunction& func, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkReturn(const cppast::CppReturnStatement& returnStatement, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkLambda(const cppast::CppLambda& lambda, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);
}