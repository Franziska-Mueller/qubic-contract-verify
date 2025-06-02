#pragma once

#include <cppparser/cppparser.h>

namespace contractverify
{
    bool checkCompliance(const cppast::CppCompound& compound, const std::string& stateStructName);

    std::unique_ptr<cppast::CppCompound> parseAST(const std::string& filepath);

    cppparser::CppParser constructCppParser();
}