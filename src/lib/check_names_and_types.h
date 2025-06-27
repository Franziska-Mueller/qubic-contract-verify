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

    bool hasStateStructPrefix(const std::string& name, const std::string& stateStructName);

    bool checkTemplSpec(const cppast::CppTemplateParams& params, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkTypedef(const cppast::CppTypedefName& def, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkTypedefList(const cppast::CppTypedefList& defList, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);
}