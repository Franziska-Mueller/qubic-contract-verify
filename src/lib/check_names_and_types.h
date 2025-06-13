#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool isInheritanceAllowed(const std::string& baseName);

    bool isNameAllowed(const std::string& name);

    bool isTypeAllowed(const std::string& type);

    bool checkTemplSpec(const cppast::CppTemplateParams& params, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);
}