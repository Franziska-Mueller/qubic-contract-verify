#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool checkTypeConverter(const cppast::CppTypeConverter& converter, const std::string& stateStructName, AnalysisData& analysisData);

    bool checkFunction(const cppast::CppFunction& func, const std::string& stateStructName, AnalysisData& analysisData);

    bool checkReturn(const cppast::CppReturnStatement& returnStatement, const std::string& stateStructName, AnalysisData& analysisData);

    bool checkLambda(const cppast::CppLambda& lambda, const std::string& stateStructName, AnalysisData& analysisData);
}