#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool checkIfBlock(const cppast::CppIfBlock& ifBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes);

    bool checkForBlock(const cppast::CppForBlock& forBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes);

    bool checkRangeForBlock(const cppast::CppRangeForBlock& forBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes);

    bool checkWhileBlock(const cppast::CppWhileBlock& whileBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes);

    bool checkDoWhileBlock(const cppast::CppDoWhileBlock& doWhileBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes);

    bool checkSwitchBlock(const cppast::CppSwitchBlock& switchBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes);

    bool checkGotoStatement(const cppast::CppGotoStatement& gotoStatement, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes);
}