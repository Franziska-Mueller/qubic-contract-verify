#include <deque>
#include <iostream>

#include <cppparser/cppparser.h>

#define RETURN_IF_FALSE(x) if(!x) return false


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

        bool checkCompound(const cppast::CppCompound& compound, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO: push onto scope stack
            return compound.visitAll([&](const cppast::CppEntity& ent) { return checkEntity(ent, stateStructName, scopeStack); });
            // TODO: pop from scope stack
        }

        bool checkVarType(const cppast::CppVarType& varType, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
            return true;
        }

        bool checkVar(const cppast::CppVar& var, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO: Implement this!
            // 
            // no global variables
            // 
            // 1.) allowed if it is a global constant and name starts with name of state struct. State struct is the one that inherits from ContractBase
            // 2.) also struct members are allowed! entity.owner returns parent compound (STRUCT vs. BLOCK)

            // not allowed: pointers, arrays, float/double, strings, chars, variadic/parameter pack, double underscores, QpiContext
            // scope resolution :: -> only structs, enums, namespaces defined in contracts and qpi.h

            // input and output structs only use basic types
            return true;
        }

        bool checkVarList(const cppast::CppVarList& varList, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO: Implement this!
            return true;
        }

        bool checkTemplSpec(const cppast::CppTemplateParams& params, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
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
            if (converter.isTemplated())
                RETURN_IF_FALSE(checkTemplSpec(converter.templateSpecification().value(), stateStructName, scopeStack));
            
            RETURN_IF_FALSE(checkVarType(*converter.targetType(), stateStructName, scopeStack));

            if (converter.defn())
                RETURN_IF_FALSE(checkCompound(*converter.defn(), stateStructName, scopeStack));

            return true;
        }

        bool checkExpr(const cppast::CppExpression& expression, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO: Implement this!
            // 
            // forbidden:
            // - pointers
            // :: (scope resolution, except for structs, enums, and namespaces defined in contracts and qpi.h)
            // arrays
            // float/double
            // / (div), % (mod)
            // strings, chars
            // (?) const_cast -> TypeConverter?

            return true;
        }

        bool checkParamList(const std::vector<const cppast::CppEntity*>& params, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            for (const auto& param : params)
            {
                switch (param->entityType())
                {
                    case cppast::CppEntityType::VAR:
                        RETURN_IF_FALSE(checkVar(*static_cast<const cppast::CppVar*>(param), stateStructName, scopeStack));
                        break;
                    case cppast::CppEntityType::FUNCTION_PTR:
                        std::cout << "function pointer is not allowed!" << std::endl;
                        return false;
                    default:
                        std::cout << "unknown CppEntity type in parameter list" << std::endl;
                        return false;
                }
            }
            return true;
        }

        bool checkConstructor(const cppast::CppConstructor& constr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            if (constr.isTemplated())
                RETURN_IF_FALSE(checkTemplSpec(constr.templateSpecification().value(), stateStructName, scopeStack));

            const auto params = GetAllParams(constr);
            if (!params.empty())
                RETURN_IF_FALSE(checkParamList(params, stateStructName, scopeStack));

            if (constr.hasMemberInitList())
            {
                for (const auto& memInit : constr.memberInits())
                {
                    // TODO:
                    // check that memInit.memberName does not start with double underscores
                    const std::vector<std::unique_ptr<cppast::CppExpression>>& memberInitArgs = memInit.memberInitInfo.args;
                    for (const auto& arg : memberInitArgs)
                        RETURN_IF_FALSE(checkExpr(*arg, stateStructName, scopeStack));
                }
            }

            if (constr.defn())
                RETURN_IF_FALSE(checkCompound(*constr.defn(), stateStructName, scopeStack));

            return true;
        }

        bool checkDestructor(const cppast::CppDestructor& destr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            if (destr.isTemplated())
                RETURN_IF_FALSE(checkTemplSpec(destr.templateSpecification().value(), stateStructName, scopeStack));

            if (destr.defn())
                RETURN_IF_FALSE(checkCompound(*destr.defn(), stateStructName, scopeStack));

            return true;
        }

        bool checkFunction(const cppast::CppFunction& func, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            if (func.isTemplated())
                RETURN_IF_FALSE(checkTemplSpec(func.templateSpecification().value(), stateStructName, scopeStack));

            if (func.returnType())
                RETURN_IF_FALSE(checkVarType(*func.returnType(), stateStructName, scopeStack));

            // TODO
            // check that func.name() does not start with double underscores

            const auto params = GetAllParams(func);
            if (!params.empty())
                RETURN_IF_FALSE(checkParamList(params, stateStructName, scopeStack));
            
            if (func.defn())
                RETURN_IF_FALSE(checkCompound(*func.defn(), stateStructName, scopeStack));

            return true;
        }

        bool checkReturn(const cppast::CppReturnStatement& returnStatement, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO
            return true;
        }

        bool checkIfBlock(const cppast::CppIfBlock& ifBlock, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            RETURN_IF_FALSE(checkEntity(*ifBlock.condition(), stateStructName, scopeStack));
            
            if (ifBlock.body())
                RETURN_IF_FALSE(checkEntity(*ifBlock.body(), stateStructName, scopeStack));
            
            if (ifBlock.elsePart())
                RETURN_IF_FALSE(checkEntity(*ifBlock.elsePart(), stateStructName, scopeStack));

            return true;
        }

        bool checkForBlock(const cppast::CppForBlock& forBlock, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            if (forBlock.start())
                RETURN_IF_FALSE(checkEntity(*forBlock.start(), stateStructName, scopeStack));
            if (forBlock.stop())
                RETURN_IF_FALSE(checkExpr(*forBlock.stop(), stateStructName, scopeStack));
            if (forBlock.step())
                RETURN_IF_FALSE(checkExpr(*forBlock.step(), stateStructName, scopeStack));
            if (forBlock.body())
                RETURN_IF_FALSE(checkEntity(*forBlock.body(), stateStructName, scopeStack));
            
            return true;
        }

        bool checkWhileBlock(const cppast::CppWhileBlock& whileBlock, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            RETURN_IF_FALSE(checkEntity(*whileBlock.condition(), stateStructName, scopeStack));

            if (whileBlock.body())
                RETURN_IF_FALSE(checkEntity(*whileBlock.body(), stateStructName, scopeStack));
            
            return true;
        }

        bool checkDoWhileBlock(const cppast::CppDoWhileBlock& doWhileBlock, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            if (doWhileBlock.body())
                RETURN_IF_FALSE(checkEntity(*doWhileBlock.body(), stateStructName, scopeStack));

            RETURN_IF_FALSE(checkEntity(*doWhileBlock.condition(), stateStructName, scopeStack));

            return true;
        }

        bool checkSwitchBlock(const cppast::CppSwitchBlock& switchBlock, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            RETURN_IF_FALSE(checkExpr(*switchBlock.condition(), stateStructName, scopeStack));
            
            for (const auto& caseStmt : switchBlock.body())
            {
                if (caseStmt.caseExpr())
                    RETURN_IF_FALSE(checkExpr(*caseStmt.caseExpr(), stateStructName, scopeStack));
                if (caseStmt.body())
                    RETURN_IF_FALSE(checkCompound(*caseStmt.body(), stateStructName, scopeStack));
            }

            return true;
        }

        bool checkEntity(const cppast::CppEntity& entity, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            switch (entity.entityType())
            {
            case cppast::CppEntityType::DOCUMENTATION_COMMENT:
                return true;

            case cppast::CppEntityType::ENTITY_ACCESS_SPECIFIER:
                // public, protected, private
                return true;

            case cppast::CppEntityType::ENUM:
                return true;

            case cppast::CppEntityType::MACRO_CALL:
                // macro arguments? but we are anyways restricted to the known macros
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
                std::cout << "function pointer is not allowed!" << std::endl;
                return false;

            case cppast::CppEntityType::THROW_STATEMENT:
                std::cout << "throw statement is not allowed!" << std::endl;
                return false;

            case cppast::CppEntityType::BLOB:
                // not quite sure how something becomes a blob but we cannot do the analysis with it
                std::cout << "CppEntity of type BLOB cannot be analyzed" << std::endl;
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
                std::cout << "unknown CppEntity encountered while analyzing the AST: " << (int)entity.entityType() << std::endl;
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

