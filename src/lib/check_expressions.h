#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool checkExpr(const cppast::CppExpression& expr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes);
}