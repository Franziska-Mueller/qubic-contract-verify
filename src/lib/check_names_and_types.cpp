#include "check_names_and_types.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <regex>
#include <stack>
#include <string>
#include <cctype>

#include <cppparser/cppparser.h>

#include "check_expressions.h"
#include "check_variables.h"
#include "defines.h"


namespace contractverify
{
    bool isInheritanceAllowed(std::string baseName, const std::vector<std::string>& additionalScopePrefixes)
    {
        // First remove all whitespace to simplify the parsing logic
        std::erase_if(baseName, [](unsigned char c) { return std::isspace(c); });

        if (baseName.compare("QpiContext") == 0)
        {
            std::cout << "[ ERROR ] Inheritance from type " << baseName << " is not allowed." << std::endl;
            return false;
        }

        RETURN_IF_FALSE(isScopeResolutionAllowed(baseName, additionalScopePrefixes));

        return true;
    }

    bool isNameAllowed(std::string name, const std::vector<std::string>& additionalScopePrefixes)
    {
        // First remove all whitespace to simplify the parsing logic
        std::erase_if(name, [](unsigned char c) { return std::isspace(c); });

        RETURN_IF_FALSE(isScopeResolutionAllowed(name, additionalScopePrefixes));

        // names starting with double underscores are reserved for internal functions and compiler macros
        if (name.compare(0, 2, "__") == 0)
        {
            std::cout << "[ ERROR ] Names starting with double underscores are reserved." << std::endl;
            return false;
        }
        // variadic arguments are not allowed and parsed with a name ending in ...
        if (name.length() >= 3 && name.compare(name.length() - 3, 3, "...") == 0)
        {
            std::cout << "[ ERROR ] Variadic arguments are not allowed." << std::endl;
            return false;
        }
        return true;
    }

    bool isTypeAllowed(std::string type, const std::vector<std::string>& additionalScopePrefixes)
    {
        // First remove all whitespace to simplify the parsing logic
        std::erase_if(type, [](unsigned char c) { return std::isspace(c); });

        RETURN_IF_FALSE(isScopeResolutionAllowed(type, additionalScopePrefixes));

        if (type.length() >= 3 && type.compare(type.length() - 3, 3, "...") == 0)
        {
            std::cout << "[ ERROR ] Variadic arguments or parameter packs are not allowed." << std::endl;
            return false;
        }
        std::vector<std::string> forbiddenTypes = { "float", "double", "string", "char", "QpiContext" };
        for (const auto& forbiddenType : forbiddenTypes)
        {
            if (type.find(forbiddenType) != std::string::npos)
            {
                std::cout << "[ ERROR ] Type " << forbiddenType << " is not allowed." << std::endl;
                return false;
            }
        }

        return true;
    }

    bool hasStateStructPrefix(const std::string& name, const std::string& stateStructName)
    {
        if (name.compare(0, stateStructName.length(), stateStructName) != 0)
        {
            std::cout << "[ ERROR ] Names declared in global scope (constants, structs/classes, functions) have to start with state struct name ("
                << stateStructName << "). Found invalid name: " << name << std::endl;
            return false;
        }
        return true;
    }

    bool isScopeResolutionAllowed(std::string name, const std::vector<std::string>& additionalScopePrefixes)
    {
        // Sometimes the scope resolution operator appears inside of brackets, for example Z<x::y, a::b>.
        // In these cases, only check the substring between the last opening bracket or comma and the scope resolution operator.
        // ASSUMPTION: The given text is syntactically correct, i.e. brackets are properly nested.
        // Note: For names with multiple scope resolution operators like a::b::c, only the outermost prefix (a) is validated.

        std::stack<std::size_t> openingPositions;  // Stack to track all opening bracket/parenthesis positions
        std::stack<std::size_t> relevantPositions;  // Stack to track lastRelevantPos for each nesting level
        std::stack<bool> foundScopeResolutions;  // Stack to track foundScopeResolution for each nesting level
        std::size_t lastRelevantPos = 0;  // Track position after last opening bracket or comma
        bool foundScopeResolution = false;  // Track if we've found any :: in current context

        // First remove all whitespace from name to simplify the parsing logic
        std::erase_if(name, [](unsigned char c) { return std::isspace(c); });

        for (std::size_t pos = 0; pos < name.length(); ++pos)
        {
            if (name[pos] == '(' || name[pos] == '<')
            {
                openingPositions.push(pos + 1);
                relevantPositions.push(lastRelevantPos);
                foundScopeResolutions.push(foundScopeResolution);
                lastRelevantPos = pos + 1;
                foundScopeResolution = false;  // Reset for new nesting level
            }
            else if (name[pos] == ')' || name[pos] == '>')
            {
                if (!openingPositions.empty())
                {
                    openingPositions.pop();
                    if (!relevantPositions.empty())
                    {
                        lastRelevantPos = relevantPositions.top();
                        relevantPositions.pop();
                    }
                    if (!foundScopeResolutions.empty())
                    {
                        foundScopeResolution = foundScopeResolutions.top();
                        foundScopeResolutions.pop();
                    }
                }
            }
            else if (name[pos] == ',' && !openingPositions.empty())
            {
                // Update lastRelevantPos to position after the comma
                lastRelevantPos = pos + 1;
                foundScopeResolution = false;  // Reset for new parameter
            }
            else if (pos + 1 < name.length() && name[pos] == ':' && name[pos + 1] == ':' && !foundScopeResolution)
            {
                // Found first :: in current context - extract the outermost prefix
                std::string prefix;

                if (!openingPositions.empty())
                {
                    // Inside brackets/parentheses: prefix starts after last opening bracket/parenthesis or comma
                    prefix = name.substr(lastRelevantPos, pos - lastRelevantPos);
                }
                else
                {
                    // Outside brackets/parentheses: prefix starts from beginning
                    prefix = name.substr(0, pos);
                }

                auto matchesPrefix = [&](const std::string& s) -> bool { return prefix.compare(s) == 0; };
                if (std::any_of(allowedScopePrefixes.begin(), allowedScopePrefixes.end(), matchesPrefix))
                {
                    // Valid prefix found, mark that we've validated this context
                    foundScopeResolution = true;
                    ++pos;  // Skip the second ':'
                    continue;
                }
                if (std::any_of(additionalScopePrefixes.begin(), additionalScopePrefixes.end(), matchesPrefix))
                {
                    // Valid prefix found, mark that we've validated this context
                    foundScopeResolution = true;
                    ++pos;  // Skip the second ':'
                    continue;
                }
                std::cout << "[ ERROR ] Scope resolution with prefix " << prefix << " is not allowed." << std::endl;
                return false;
            }
        }

        return true;
    }

    bool isInputOutputType(std::string name)
    {
        // First remove all whitespace
        std::erase_if(name, [](unsigned char c) { return std::isspace(c); });

        if (name.length() >= 6 && name.compare(name.length() - 6, 6, "_input") == 0)
            return true;
        if (name.length() >= 7 && name.compare(name.length() - 7, 7, "_output") == 0)
            return true;

        return false;
    }

    bool isTypeAllowedAsIO(std::string type, const AnalysisData& analysisData)
    {
        // Assumption: check that the type is allowed has passed, i.e. isTypeAllowed() returned true.

        // First remove all whitespace from name to simplify the parsing logic
        std::erase_if(type, [](unsigned char c) { return std::isspace(c); });

        auto matchesTypename = [&](const std::string& s) -> bool { return type.compare(s) == 0; };
        if (std::any_of(allowedInputOutputTypes.begin(), allowedInputOutputTypes.end(), matchesTypename))
            return true;

        // A special case are Array<type, size> and BitArray<size> defined in QPI.
        // These are allowed (in case of Array only if the element type is allowed).
        // Note that the size might be specified as name of a constant hence this needs to be considered in the regex.
        std::regex regexArray("Array<(\\w+),\\s*\\w+>");
        std::regex regexBitArray("BitArray<\\w+>");
        std::smatch match;
        if (std::regex_match(type, match, regexBitArray))
                return true;
        if (std::regex_match(type, match, regexArray))
                return isTypeAllowedAsIO(match[1].str(), analysisData);

        // Another special case are oracle related types:
        // - OI::*::OracleQuery
        // - OI::*::OracleReply
        // - OracleNotificationInput<OI::*>
        std::regex regexOracleQuery("OI::\\w+::OracleQuery");
        std::regex regexOracleReply("OI::\\w+::OracleReply");
        std::regex regexOracleNotificationInput("OracleNotificationInput<OI::\\w+>");
        if (std::regex_match(type, match, regexOracleQuery)
            || std::regex_match(type, match, regexOracleReply)
            || std::regex_match(type, match, regexOracleNotificationInput))
            return true;

        auto matchesScopedTypename = [&](const std::vector<std::string>& s) -> bool 
            { 
                /*
                Example:

                struct SomeContract : public ContractBase
                {
                    struct A
                    {
                        struct Helper
                        {
                            int32 i;
                        }

                        // additionalInputOutputTypes will already contain: SomeContract::A::Helper
                        // getFullyScopedName() will return SomeContract::A here

                        Helper h;
                    }

                    // when analyzing B, additionalInputOutputTypes will already contain: SomeContract::A::Helper, SomeContract::A

                    struct B
                    {
                        // getFullyScopedName() will return SomeContract::B here

                        A::Helper ah;
                    }
                }

                --> remove common prefix from allowed type and then compare with variable type
                */
                auto [itScopedName, itAllowedName] = std::mismatch(analysisData.scopeNames.begin(), analysisData.scopeNames.end(), s.begin(), s.end(), 
                    [&](const std::string& s1, const std::string& s2) { return s1.compare(s2) == 0; });

                std::string sCommonPrefixRemoved = getScopedName(s, std::distance(s.begin(), itAllowedName));

                return type.compare(sCommonPrefixRemoved) == 0; 
            };
        if (std::any_of(analysisData.additionalInputOutputTypes.begin(), analysisData.additionalInputOutputTypes.end(), matchesScopedTypename))
            return true;

        return false;
    }

    bool checkTemplSpec(const cppast::CppTemplateParams& params, const std::string& stateStructName, AnalysisData& analysisData)
    {
        analysisData.scopeStack.push(ScopeSpec::TEMPL_SPEC);

        for (const auto& param : params)
        {
            if (param.paramType().has_value())
            {
                RETURN_IF_FALSE(
                    std::visit(Overloaded{
                            [&](const std::unique_ptr<cppast::CppVarType>& varType) -> bool
                            {
                                return checkVarType(*varType, stateStructName, analysisData);
                            },
                            [&](const std::unique_ptr<cppast::CppFunctionPointer>& funcPtr) -> bool
                            {
                                std::cout << "[ ERROR ] Function pointers are not allowed." << std::endl;
                                return false;
                            }
                        },
                        param.paramType().value()
                    )
                );
            }

            RETURN_IF_FALSE(isNameAllowed(param.paramName(), analysisData.additionalScopePrefixes));

            RETURN_IF_FALSE(
                std::visit(Overloaded{
                        [&](const std::unique_ptr<cppast::CppVarType>& varType) -> bool
                        {
                            if (varType)
                                return checkVarType(*varType, stateStructName, analysisData);
                            else
                                return true;
                        },
                        [&](const std::unique_ptr<cppast::CppExpression>& expr) -> bool
                        {
                            if (expr)
                                return checkExpr(*expr, stateStructName, analysisData);
                            else
                                return true;
                        }
                    },
                    param.defaultArg()
                )
            );
        }

        analysisData.scopeStack.pop();
        return true;
    }

    bool checkTypedef(const cppast::CppTypedefName& def, const std::string& stateStructName, AnalysisData& analysisData)
    {
        if (analysisData.scopeStack.empty())
        {
            std::cout << "[ ERROR ] `typedef` is not allowed in global scope." << std::endl;
            return false;
        }

        analysisData.scopeStack.push(ScopeSpec::TYPEDEF);
        RETURN_IF_FALSE(checkVar(*def.var(), stateStructName, analysisData));
        analysisData.scopeStack.pop();

        return true;
    }

    bool checkTypedefList(const cppast::CppTypedefList& defList, const std::string& stateStructName, AnalysisData& analysisData)
    {
        if (analysisData.scopeStack.empty())
        {
            std::cout << "[ ERROR ] `typedef` is not allowed in global scope." << std::endl;
            return false;
        }

        analysisData.scopeStack.push(ScopeSpec::TYPEDEF);
        RETURN_IF_FALSE(checkVarList(defList.varList(), stateStructName, analysisData));
        analysisData.scopeStack.pop();

        return true;
    }

}  // namespace contractverify