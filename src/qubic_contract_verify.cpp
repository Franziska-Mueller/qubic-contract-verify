#include <string>
#include <iostream>

#include <cppparser/cppparser.h>

#include "check_compliance.h"


int main(int argc, char** argv)
{
    if (argc < 2) 
    {
        std::cerr << "[ ERROR ] Mandatory filepath argument was not provided." << std::endl
            << "Usage: ./contractverify <FILEPATH>" << std::endl;
        return 1;
    }
    else if (argc > 2)
    {
        std::cout << "[ WARNING ] Too many command line arguments provided, excessive arguments will be ignored." << std::endl;
    }
    auto filepath = std::string(argv[1]);

    std::unique_ptr<cppast::CppCompound> contractAST = contractverify::parseAST(filepath);

    if (!contractAST)
    {
        std::cout << "[ ERROR ] Abstract syntax tree could not be parsed from file " << filepath << std::endl;
        return 1;
    }

    if (contractverify::checkCompliance(*contractAST))
    {
        std::cout << "Contract compliance check PASSED" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "Contract compliance check FAILED" << std::endl;
        return 1;
    }
}