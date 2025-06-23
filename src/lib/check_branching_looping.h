#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool checkIfBlock(const cppast::CppIfBlock& ifBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkForBlock(const cppast::CppForBlock& forBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkRangeForBlock(const cppast::CppRangeForBlock& forBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkWhileBlock(const cppast::CppWhileBlock& whileBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkDoWhileBlock(const cppast::CppDoWhileBlock& doWhileBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkSwitchBlock(const cppast::CppSwitchBlock& switchBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkGotoStatement(const cppast::CppGotoStatement& gotoStatement, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);
}