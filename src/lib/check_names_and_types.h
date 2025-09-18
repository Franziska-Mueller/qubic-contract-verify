#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool isInheritanceAllowed(const std::string& baseName, const std::vector<std::string>& additionalScopePrefixes);

    bool isNameAllowed(const std::string& name, const std::vector<std::string>& additionalScopePrefixes);

    bool isTypeAllowed(const std::string& type, const std::vector<std::string>& additionalScopePrefixes);

    bool hasStateStructPrefix(const std::string& name, const std::string& stateStructName);

    bool isScopeResolutionAllowed(const std::string& name, const std::vector<std::string>& additionalScopePrefixes);

    bool isInputOutputType(const std::string& name);

    bool isTypeAllowedAsIO(const std::string& type, const AnalysisData& analysisData);

    bool checkTemplSpec(const cppast::CppTemplateParams& params, const std::string& stateStructName, AnalysisData& analysisData);

    bool checkTypedef(const cppast::CppTypedefName& def, const std::string& stateStructName, AnalysisData& analysisData);

    bool checkTypedefList(const cppast::CppTypedefList& defList, const std::string& stateStructName, AnalysisData& analysisData);
}