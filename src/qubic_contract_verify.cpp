#include <string>

#include <cppparser/cppparser.h>

#include "check_compliance.h"


int main(int argc, char** argv)
{
    cppparser::CppParser parser = contractverify::constructCppParser();

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

    if (contractverify::checkCompliance(*progUnit, stateStructName))
        return 0;
    else
        return 1;
}