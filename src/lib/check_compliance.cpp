#include <deque>
#include <iostream>

#include <cppparser/cppparser.h>


namespace contractverify
{
    namespace
    {
        enum ScopeSpec
        {
            STRUCT = 0,
            BLOCK = 1,
            NAMESPACE = 2,
        };

        bool checkEntity(const cppast::CppEntity& entity, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack);

        bool checkVar(const cppast::CppVar& var, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO: Implement this!
            // 
            // 1.) allowed if it is a global constant and name starts with name of state struct. State struct is the one that inherits from ContractBase
            // 2.) also struct members are allowed! entity.owner returns parent compound (STRUCT vs. BLOCK)

            // not allowed: pointers
            return true;
        }

        bool checkVarList(const cppast::CppVarList& varList, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO: Implement this!
            // 
            // 1.) allowed if it is a global constant and name starts with name of state struct. State struct is the one that inherits from ContractBase
            // 2.) also struct members are allowed! entity.owner returns parent compound (STRUCT vs. BLOCK)

            // not allowed: pointers
            return true;
        }

        bool checkUsingNamespace(const cppast::CppUsingNamespaceDecl& decl, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO: Implement this!
            // 
            // global: only allowed if the namespace is QPI
            // otherwise others are also ok
            return true;
        }

        bool checkUsingDecl(const cppast::CppUsingDecl& decl, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO: Implement this!
            // 
            // in functions ok, globally not
            return true;
        }

        bool checkTypeConverter(const cppast::CppTypeConverter& converter, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
            // const_cast is forbidden
            return false;
        }

        bool checkExpr(const cppast::CppExpression& expression, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO: Implement this!
            // 
            // forbidden:
            // - pointers
            // :: (scope resolution, except for structs, enums, and namespaces defined in contracts and qpi.h)

            return true;
        }

        bool checkConstructor(const cppast::CppConstructor& constr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
            return false;
        }

        bool checkDestructor(const cppast::CppDestructor& constr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
            return false;
        }

        bool checkFunction(const cppast::CppFunction& constr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
            return false;
        }

        bool checkReturn(const cppast::CppReturnStatement& returnStatement, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
            return true;
        }

        bool checkIfBlock(const cppast::CppIfBlock& ifBlock, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
            return true;
        }

        bool checkForBlock(const cppast::CppForBlock& forBlock, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
            return true;
        }

        bool checkWhileBlock(const cppast::CppWhileBlock& whileBlock, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
            return true;
        }

        bool checkDoWhileBlock(const cppast::CppDoWhileBlock& doWhileBlock, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
            return true;
        }

        bool checkSwitchBlock(const cppast::CppSwitchBlock& switchBlock, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
            return true;
        }

        bool checkCompound(const cppast::CppCompound& compound, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            return compound.visitAll([&](const cppast::CppEntity& ent) { return checkEntity(ent, stateStructName, scopeStack); });
        }

        bool checkEntity(const cppast::CppEntity& entity, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            switch (entity.entityType())
            {
            case cppast::CppEntityType::DOCUMENTATION_COMMENT:
                return true;

            case cppast::CppEntityType::ENTITY_ACCESS_SPECIFIER:
                return true;

            case cppast::CppEntityType::ENUM:
                return true;

            case cppast::CppEntityType::MACRO_CALL:
                return true;

            case cppast::CppEntityType::GOTO_STATEMENT:
                return true;

            case cppast::CppEntityType::FORWARD_CLASS_DECL:
                return true;

            case cppast::CppEntityType::PREPROCESSOR:
                std::cout << "preprocessor directives are not allowed!" << std::endl;
                return false;

            case cppast::CppEntityType::TYPEDEF_DECL:
                std::cout << "typedef is not allowed!" << std::endl;
                return false;

            case cppast::CppEntityType::TYPEDEF_DECL_LIST:
                std::cout << "typedef is not allowed!" << std::endl;
                return false;

            case cppast::CppEntityType::NAMESPACE_ALIAS:
                std::cout << "namespace alias is not allowed!" << std::endl;
                return false;

            case cppast::CppEntityType::FUNCTION_PTR:
                return false;

            case cppast::CppEntityType::THROW_STATEMENT:
                return false;

            case cppast::CppEntityType::BLOB:
                // not quite sure how something becomes a blob but we cannot do the analysis with it
                return false;

            case cppast::CppEntityType::COMPOUND:
                return checkCompound((const cppast::CppCompound&)entity, stateStructName, scopeStack);

            case cppast::CppEntityType::VAR:
                return checkVar((const cppast::CppVar&)entity, stateStructName, scopeStack);

            case cppast::CppEntityType::VAR_LIST:
                return checkVarList((const cppast::CppVarList&)entity, stateStructName, scopeStack);

            case cppast::CppEntityType::USING_NAMESPACE:
                return checkUsingNamespace((const cppast::CppUsingNamespaceDecl&)entity, stateStructName, scopeStack);

            case cppast::CppEntityType::USING_DECL:
                return checkUsingDecl((const cppast::CppUsingDecl&)entity, stateStructName, scopeStack);

            case cppast::CppEntityType::TYPE_CONVERTER:
                return checkTypeConverter((const cppast::CppTypeConverter&)entity, stateStructName, scopeStack);

            case cppast::CppEntityType::CONSTRUCTOR:
                return checkConstructor((const cppast::CppConstructor&)entity, stateStructName, scopeStack);

            case cppast::CppEntityType::DESTRUCTOR:
                return checkDestructor((const cppast::CppDestructor&)entity, stateStructName, scopeStack);

            case cppast::CppEntityType::FUNCTION:
                return checkFunction((const cppast::CppFunction&)entity, stateStructName, scopeStack);

            case cppast::CppEntityType::EXPRESSION:
                return checkExpr((const cppast::CppExpression&)entity, stateStructName, scopeStack);

            case cppast::CppEntityType::RETURN_STATEMENT:
                return checkReturn((const cppast::CppReturnStatement&)entity, stateStructName, scopeStack);

            case cppast::CppEntityType::IF_BLOCK:
                return checkIfBlock((const cppast::CppIfBlock&)entity, stateStructName, scopeStack);
            
            case cppast::CppEntityType::FOR_BLOCK:
                return checkForBlock((const cppast::CppForBlock&)entity, stateStructName, scopeStack);
            
            case cppast::CppEntityType::WHILE_BLOCK:
                return checkWhileBlock((const cppast::CppWhileBlock&)entity, stateStructName, scopeStack);
            
            case cppast::CppEntityType::DO_WHILE_BLOCK:
                return checkDoWhileBlock((const cppast::CppDoWhileBlock&)entity, stateStructName, scopeStack);
            
            case cppast::CppEntityType::SWITCH_BLOCK:
                return checkSwitchBlock((const cppast::CppSwitchBlock&)entity, stateStructName, scopeStack);

            default:
                // control should never reach here
                return false;
            }
        }
    }

    bool checkCompliance(const cppast::CppCompound& compound, const std::string& stateStructName)
    {
        std::deque<ScopeSpec> scopeStack = {}; // empty scope stack means global scope
        return checkEntity(compound, stateStructName, scopeStack);
    }

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

    std::unique_ptr<cppast::CppCompound> parseAST(const std::string& filepath)
    {
        cppparser::CppParser parser = constructCppParser();
        return parser.parseFile(filepath.c_str());
    }

}  // namespace contractverify

