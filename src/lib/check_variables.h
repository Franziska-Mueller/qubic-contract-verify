#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool checkVarType(const cppast::CppVarType& varType, const std::string& stateStructName, AnalysisData& analysisData);

    bool checkVar(const cppast::CppVar& var, const std::string& stateStructName, AnalysisData& analysisData);

    bool checkVarList(const cppast::CppVarList& varList, const std::string& stateStructName, AnalysisData& analysisData);
}