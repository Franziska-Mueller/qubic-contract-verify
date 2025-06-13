#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool checkCompliance(const cppast::CppCompound& compound, const std::string& stateStructName);

    bool checkEntity(const cppast::CppEntity& entity, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    bool checkCompound(const cppast::CppCompound& compound, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack);

    std::unique_ptr<cppast::CppCompound> parseAST(const std::string& filepath);

    std::string findStateStructName(const cppast::CppCompound& ast);

    cppparser::CppParser constructCppParser();
}