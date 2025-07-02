#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool isInheritanceAllowed(const std::string& baseName, std::vector<std::string>& additionalScopePrefixes);

    bool isNameAllowed(const std::string& name, std::vector<std::string>& additionalScopePrefixes);

    bool isTypeAllowed(const std::string& type, std::vector<std::string>& additionalScopePrefixes);

    bool hasStateStructPrefix(const std::string& name, const std::string& stateStructName);

    bool isScopeResolutionAllowed(const std::string& name, std::vector<std::string>& additionalScopePrefixes);

    bool checkTemplSpec(const cppast::CppTemplateParams& params, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes);

    bool checkTypedef(const cppast::CppTypedefName& def, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes);

    bool checkTypedefList(const cppast::CppTypedefList& defList, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes);
}