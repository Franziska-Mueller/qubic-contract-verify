#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool checkCompliance(const cppast::CppCompound& compound);

    bool checkCompliance(const cppast::CppCompound& compound, const std::string& stateStructName);

    bool checkEntity(const cppast::CppEntity& entity, const std::string& stateStructName, AnalysisData& analysisData);

    bool checkCompound(const cppast::CppCompound& compound, const std::string& stateStructName, AnalysisData& analysisData);

    // Tries to parse an abstract syntax tree from the specified file. Returns nullptr if the file does not exist or parsing fails. 
    std::unique_ptr<cppast::CppCompound> parseAST(const std::string& filepath);

    // Finds the state struct name in an AST assuming that the state struct is the first top-level struct that inherits from ContractBase.
    // Returns an empty string if it cannot find such a struct.
    std::string findStateStructName(const cppast::CppCompound& ast);
}