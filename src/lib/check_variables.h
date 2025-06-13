#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool checkVarType(const cppast::CppVarType& varType, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkVar(const cppast::CppVar& var, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkVarList(const cppast::CppVarList& varList, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);
}