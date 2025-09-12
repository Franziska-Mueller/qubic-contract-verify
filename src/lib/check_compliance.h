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

    std::unique_ptr<cppast::CppCompound> parseAST(const std::string& filepath);

    std::string findStateStructName(const cppast::CppCompound& ast);
}