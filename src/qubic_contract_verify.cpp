#include <string>
#include <iostream>

#include <cppparser/cppparser.h>

#include "check_compliance.h"


int main(int argc, char** argv)
{
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

    std::unique_ptr<cppast::CppCompound> contractAST = contractverify::parseAST(filepath);

    if (!contractAST)
        return 1;

    if (contractverify::checkCompliance(*contractAST))
        return 0;
    else
        return 1;
}