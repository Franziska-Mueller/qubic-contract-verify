#include "check_compliance.h"

#include <iostream>
#include <stack>
#include <string>
#include <variant>

#include <cppparser/cppparser.h>

#include "check_branching_looping.h"
#include "check_expressions.h"
#include "check_functionlike.h"
#include "check_names_and_types.h"
#include "check_variables.h"
#include "defines.h"


namespace contractverify
{
    namespace
    {
        bool checkUsingNamespace(const cppast::CppUsingNamespaceDecl& decl, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
        {
            // in global scope, only namespace QPI is allowed
            if (scopeStack.empty()) // global scope
            {
                if (decl.name().compare("QPI") != 0)
                {
                    std::cout << "[ ERROR ] Only QPI can be used for a using namespace declaration in global scope." << std::endl;
                    return false;
                }
            }

            RETURN_IF_FALSE(isScopeResolutionAllowed(decl.name(), additionalScopePrefixes));

            return true;
        }

        bool checkUsingDecl(const cppast::CppUsingDecl& decl, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
        {
            // in global scope not allowed, otherwise ok

            if (scopeStack.empty()) // global scope
            {
                std::cout << "[ ERROR ] Using declaration is not allowed in global scope." << std::endl;
                return false;
            }

            if (decl.isTemplated())
                RETURN_IF_FALSE(checkTemplSpec(decl.templateSpecification().value(), stateStructName, scopeStack, additionalScopePrefixes));

            RETURN_IF_FALSE(isScopeResolutionAllowed(decl.name(), additionalScopePrefixes));

            RETURN_IF_FALSE(
                std::visit(Overloaded{ 
                        [&](const std::unique_ptr<cppast::CppVarType>& varType) -> bool
                        {
                            if (varType)
                            {
                                return checkVarType(*varType, stateStructName, scopeStack, additionalScopePrefixes);
                            }
                            return true;
                        },
                        [&](const std::unique_ptr<cppast::CppFunctionPointer>& funcPtr) -> bool
                        {
                            if (funcPtr)
                            {
                                std::cout << "[ ERROR ] Function pointers are not allowed." << std::endl;
                                return false;
                            }
                            return true;
                        },
                        [&](const std::unique_ptr<cppast::CppCompound>& compound) -> bool
                        {
                            if (compound)
                            {
                                return checkCompound(*compound, stateStructName, scopeStack, additionalScopePrefixes);
                            }
                            return true;
                        } 
                    },
                    decl.definition()
                )
            );

            return true;
        }

        bool checkFwdDecl(const cppast::CppForwardClassDecl& fwdDecl, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
        {
            if (fwdDecl.isTemplated())
                RETURN_IF_FALSE(checkTemplSpec(fwdDecl.templateSpecification().value(), stateStructName, scopeStack, additionalScopePrefixes));
            return true;
        }

    }  // namespace

    bool checkCompound(const cppast::CppCompound& compound, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        if (IsNamespaceLike(compound))
        {
            if (compound.compoundType() == cppast::CppCompoundType::UNION)
            {
                std::cout << "[ ERROR ] `union` is not allowed." << std::endl;
                return false;
            }
            if (compound.isTemplated())
            {
                checkTemplSpec(compound.templateSpecification().value(), stateStructName, scopeStack, additionalScopePrefixes);
            }
            RETURN_IF_FALSE(isNameAllowed(compound.name(), additionalScopePrefixes));
        }
        if (!compound.inheritanceList().empty())
        {
            for (const auto& inheritanceInfo : compound.inheritanceList())
            {
                RETURN_IF_FALSE(isInheritanceAllowed(inheritanceInfo.baseName, additionalScopePrefixes));
            }
        }

        bool scopeStackPushed = true;
        switch (compound.compoundType())
        {
        case cppast::CppCompoundType::STRUCT:
            if (scopeStack.empty()) // global struct name has to start with stateStructName  
                RETURN_IF_FALSE(hasStateStructPrefix(compound.name(), stateStructName));
            additionalScopePrefixes.push_back(compound.name());
            scopeStack.push(ScopeSpec::STRUCT);
            break;
        case cppast::CppCompoundType::CLASS:
            if (scopeStack.empty()) // global class name has to start with stateStructName
                RETURN_IF_FALSE(hasStateStructPrefix(compound.name(), stateStructName));
            additionalScopePrefixes.push_back(compound.name());
            scopeStack.push(ScopeSpec::CLASS);
            break;
        case cppast::CppCompoundType::NAMESPACE:
            scopeStack.push(ScopeSpec::NAMESPACE);
            break;
        case cppast::CppCompoundType::BLOCK:
        case cppast::CppCompoundType::EXTERN_C_BLOCK:
            scopeStack.push(ScopeSpec::BLOCK);
            break;
        default:
            scopeStackPushed = false;
            break;
        }

        bool checkSucceeded = compound.visitAll([&](const cppast::CppEntity& ent) -> bool { return checkEntity(ent, stateStructName, scopeStack, additionalScopePrefixes); });
        if (scopeStackPushed)
            scopeStack.pop();

        return checkSucceeded;
    }

    bool checkEntity(const cppast::CppEntity& entity, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        switch (entity.entityType())
        {
        case cppast::CppEntityType::DOCUMENTATION_COMMENT:
            return true;

        case cppast::CppEntityType::ENTITY_ACCESS_SPECIFIER:
            // public, protected, private
            return true;

        case cppast::CppEntityType::ENUM:
            additionalScopePrefixes.push_back(static_cast<const cppast::CppEnum&>(entity).name());
            return true;

        case cppast::CppEntityType::MACRO_CALL:
            // macro arguments? but we are anyways restricted to the known macros
            return true;

        case cppast::CppEntityType::LABEL:
            return true;

        case cppast::CppEntityType::PREPROCESSOR:
            std::cout << "[ ERROR ] Preprocessor directives (character `#`) are not allowed." << std::endl;
            return false;

        case cppast::CppEntityType::NAMESPACE_ALIAS:
            std::cout << "[ ERROR ] Namespace alias is not allowed." << std::endl;
            return false;

        case cppast::CppEntityType::FUNCTION_PTR:
            std::cout << "[ ERROR ] Function pointers are not allowed." << std::endl;
            return false;

        case cppast::CppEntityType::CONSTRUCTOR:
            std::cout << "[ ERROR ] Constructors are not allowed." << std::endl;
            return false;

        case cppast::CppEntityType::DESTRUCTOR:
            std::cout << "[ ERROR ] Destructors are not allowed." << std::endl;
            return false;

        case cppast::CppEntityType::THROW_STATEMENT:
            std::cout << "[ ERROR ] `throw` statement is not allowed." << std::endl;
            return false;

        case cppast::CppEntityType::TRY_BLOCK:
            std::cout << "[ ERROR ] `try` blocks are not allowed." << std::endl;
            return false;

        case cppast::CppEntityType::BLOB:
            // not quite sure how something becomes a blob but we cannot do the analysis with it
            std::cout << "[ ERROR ] CppEntity of type BLOB cannot be analyzed." << std::endl;
            return false;

        case cppast::CppEntityType::COMPOUND:
            return checkCompound(static_cast<const cppast::CppCompound&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::VAR:
            return checkVar(static_cast<const cppast::CppVar&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::VAR_LIST:
            return checkVarList(static_cast<const cppast::CppVarList&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::USING_NAMESPACE:
            return checkUsingNamespace(static_cast<const cppast::CppUsingNamespaceDecl&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::USING_DECL:
            return checkUsingDecl(static_cast<const cppast::CppUsingDecl&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::TYPEDEF_DECL:
            return checkTypedef(static_cast<const cppast::CppTypedefName&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::TYPEDEF_DECL_LIST:
            return checkTypedefList(static_cast<const cppast::CppTypedefList&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::GOTO_STATEMENT:
            return checkGotoStatement(static_cast<const cppast::CppGotoStatement&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::FORWARD_CLASS_DECL:
            return checkFwdDecl(static_cast<const cppast::CppForwardClassDecl&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::TYPE_CONVERTER:
            return checkTypeConverter(static_cast<const cppast::CppTypeConverter&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::FUNCTION:
            return checkFunction(static_cast<const cppast::CppFunction&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::LAMBDA:
            return checkLambda(static_cast<const cppast::CppLambda&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::EXPRESSION:
            return checkExpr(static_cast<const cppast::CppExpression&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::RETURN_STATEMENT:
            return checkReturn(static_cast<const cppast::CppReturnStatement&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::IF_BLOCK:
            return checkIfBlock(static_cast<const cppast::CppIfBlock&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::FOR_BLOCK:
            return checkForBlock(static_cast<const cppast::CppForBlock&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::RANGE_FOR_BLOCK:
            return checkRangeForBlock(static_cast<const cppast::CppRangeForBlock&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::WHILE_BLOCK:
            return checkWhileBlock(static_cast<const cppast::CppWhileBlock&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::DO_WHILE_BLOCK:
            return checkDoWhileBlock(static_cast<const cppast::CppDoWhileBlock&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        case cppast::CppEntityType::SWITCH_BLOCK:
            return checkSwitchBlock(static_cast<const cppast::CppSwitchBlock&>(entity), stateStructName, scopeStack, additionalScopePrefixes);

        default:
            // control should never reach here
            std::cout << "[ ERROR ] Unknown CppEntityType encountered while analyzing the AST: " << static_cast<int>(entity.entityType()) << std::endl;
            return false;
        }
    }

    bool checkCompliance(const cppast::CppCompound& compound, const std::string& stateStructName)
    {
        std::vector<std::string> additionalScopePrefixes = {}; // will be collected while traversing the AST
        std::stack<ScopeSpec> scopeStack = {}; // empty scope stack means global scope
        return checkEntity(compound, stateStructName, scopeStack, additionalScopePrefixes);
    }

    bool checkCompliance(const cppast::CppCompound& compound)
    {
        std::string stateStructName = contractverify::findStateStructName(compound);
        return checkCompliance(compound, stateStructName);
    }

    std::unique_ptr<cppast::CppCompound> parseAST(const std::string& filepath)
    {
        cppparser::CppParser parser;
        parser.addKnownMacros(knownMacroNames);
        return parser.parseFile(filepath.c_str());
    }

    std::string findStateStructName(const cppast::CppCompound& ast)
    {
        // Assumption: state struct is the first top-level struct that inherits from ContractBase
        std::string name = "";

        if (ast.compoundType() != cppast::CppCompoundType::FILE)
        {
            std::cout << "[ ERROR ] Need a top-level CppCompound (compound type FILE) for finding the state struct name." << std::endl;
            return name;
        }

        // `visitAll` visits the entities sequentially, so we do not need any lock for `name`
        ast.visitAll([&](const cppast::CppEntity& entity) -> bool
            {
                if (name.empty() && entity.entityType() == cppast::CppEntityType::COMPOUND)
                {
                    const auto& compound = static_cast<const cppast::CppCompound&>(entity);
                    if (compound.compoundType() == cppast::CppCompoundType::STRUCT)
                    {
                        for (const auto& baseClass : compound.inheritanceList())
                        {
                            if (baseClass.baseName.compare("ContractBase") == 0)
                            {
                                name = compound.name();
                                return true;
                            }
                        }
                    }
                }
                // need to return true in any case because `visitAll` interrupts when the callback returns false on an entity
                return true;
            }
        );

        return name;
    }

}  // namespace contractverify

