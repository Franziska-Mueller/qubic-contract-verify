#include <string>
#include <iostream>

#include <cppparser/cppparser.h>

#include "check_compliance.h"

void printUsage()
{
    std::cerr
        << "\nUsage: ./contractverify [--oi] <FILEPATH>\n\n"
        << "    The switch --oi means that <FILEPATH> points to an oracle interface instead of a contract." << std::endl;
}

int main(int argc, char** argv)
{
    contractverify::FileType filetype = contractverify::FileType::CONTRACT;
    std::string filepath;

    if (argc < 2)
    {
        std::cerr << "[ ERROR ] Mandatory filepath argument was not provided." << std::endl;
        printUsage();
        return 1;
    }
    else if (argc == 2)
    {
        filepath = std::string(argv[1]);
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1], "--oi") != 0)
        {
            std::cerr << "[ ERROR ] Unsupported argument " << argv[1] << std::endl;
            printUsage();
            return 1;
        }
        filetype = contractverify::FileType::ORACLE_INTERFACE;
        filepath = std::string(argv[2]);
    }
    else
    {
        std::cerr << "[ ERROR ] Too many command line arguments provided." << std::endl;
        printUsage();
        return 1;
    }

    std::unique_ptr<cppast::CppCompound> contractAST = contractverify::parseAST(filepath);

    if (!contractAST)
    {
        std::cout << "[ ERROR ] Abstract syntax tree could not be parsed from file " << filepath << std::endl;
        return 1;
    }

    std::string filetypeString = (filetype == contractverify::FileType::CONTRACT) ? "Contract" : "Oracle interface";

    if (contractverify::checkCompliance(*contractAST, filetype))
    {
        std::cout << filetypeString << " compliance check PASSED" << std::endl;
        return 0;
    }
    else
    {
        std::cout << filetypeString << " compliance check FAILED" << std::endl;
        return 1;
    }
}