#include <string>

#include <cppparser/cppparser.h>

#include "check_compliance.h"


cppparser::CppParser constructCppParser()
{
    cppparser::CppParser parser;
    parser.addKnownMacros({
        "PUBLIC_FUNCTION",
        "PUBLIC_FUNCTION_WITH_LOCALS",
        "PUBLIC_PROCEDURE",
        "PUBLIC_PROCEDURE_WITH_LOCALS",
        "END_TICK",
        "END_TICK_WITH_LOCALS",
        "BEGIN_TICK",
        "BEGIN_TICK_WITH_LOCALS",
        "REGISTER_USER_FUNCTIONS_AND_PROCEDURES",
        "REGISTER_USER_FUNCTION",
        "REGISTER_USER_PROCEDURE", 
    });

    return parser;
}

int main(int argc, char** argv)
{
    cppparser::CppParser parser = constructCppParser();

    std::string inputPath = "D:\\Programming\\Repos\\Qubic_core\\src\\contracts\\QUtil.h";

    /* Do non - semantic checks first:
    forbidden characters/words:
    - [ and ]
    - # (includes, pragmas, conditional compilation etc.)
    - float, double
    - / (division), % (modulo)
    - " (strings), ' (chars)
    - ... (variadic arguments)
    - __ (double underscores, reserved for internal functions and compiler macros)
    - QpiContext
    - const_cast
    - typedef
    - union
    */

    std::unique_ptr<cppast::CppCompound> progUnit = parser.parseFile(inputPath.c_str());

    if (!progUnit)
        return 1;

    std::string stateStructName = "QUTIL";
    // TODO
    // std::string stateStructName = findStateStructName(*progUnit);

    if (checkCompliance(*progUnit, stateStructName))
        return 0;
    else
        return 1;
}