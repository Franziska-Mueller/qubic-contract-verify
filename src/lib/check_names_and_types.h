#pragma once

#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "defines.h"


namespace contractverify
{
    bool isInheritanceAllowed(std::string baseName, const std::vector<std::string>& additionalScopePrefixes);

    bool isNameAllowed(std::string name, const std::vector<std::string>& additionalScopePrefixes);

    bool isTypeAllowed(std::string type, const std::vector<std::string>& additionalScopePrefixes);

    bool hasStateStructPrefix(const std::string& name, const std::string& stateStructName);

    bool isScopeResolutionAllowed(std::string name, const std::vector<std::string>& additionalScopePrefixes);

    bool isInputOutputType(std::string name);

    bool isTypeAllowedAsIO(std::string type, const AnalysisData& analysisData);

    bool checkTemplSpec(const cppast::CppTemplateParams& params, const std::string& stateStructName, AnalysisData& analysisData);

    bool checkTypedef(const cppast::CppTypedefName& def, const std::string& stateStructName, AnalysisData& analysisData);

    bool checkTypedefList(const cppast::CppTypedefList& defList, const std::string& stateStructName, AnalysisData& analysisData);
}