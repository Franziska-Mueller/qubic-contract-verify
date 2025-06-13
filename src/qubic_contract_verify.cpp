#include <string>
#include <iostream>

#include <cppparser/cppparser.h>

#include "check_compliance.h"


int main(int argc, char** argv)
{
    cppparser::CppParser parser = contractverify::constructCppParser();

    if (argc < 2) 
    {
        std::cerr << "Mandatory filepath argument was not provided" << std::endl
            << "Usage: ./contractverify <FILEPATH>" << std::endl;
        return 1;
    }
    else if (argc > 2)
    {
        std::cout << "Too many command line arguments provided, excessive arguments will be ignored" << std::endl;
    }
    auto filepath = std::string(argv[1]);

    std::unique_ptr<cppast::CppCompound> progUnit = parser.parseFile(filepath.c_str());

    if (!progUnit)
        return 1;

    std::string stateStructName = contractverify::findStateStructName(*progUnit);

    if (contractverify::checkCompliance(*progUnit, stateStructName))
        return 0;
    else
        return 1;
}