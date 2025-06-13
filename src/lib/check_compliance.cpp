#include <deque>
#include <iostream>
#include <variant>

#include <cppparser/cppparser.h>

#define RETURN_IF_FALSE(x) if(!x) return false


namespace contractverify
{
    namespace
    {
        enum ScopeSpec
        {
            STRUCT = 0,
            CLASS = 1,
            NAMESPACE = 2,
            BLOCK = 3,
        };

        bool checkEntity(const cppast::CppEntity& entity, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack);
        bool checkExpr(const cppast::CppExpression& expr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack);

        bool isInheritanceAllowed(const std::string& baseName)
        {
            if (baseName.compare("QpiContext") == 0)
            {
                std::cout << "inheritance from type " << baseName << " is not allowed!" << std::endl;
                return false;
            }
            return true;
        }

        bool isNameAllowed(const std::string& name)
        {
            // TODO: scope resolution :: -> only structs, enums, namespaces defined in contracts and qpi.h

            // names starting with double underscores are reserved for internal functions and compiler macros
            if (name.compare(0, 2, "__") == 0)
            {
                std::cout << "names starting with double underscores are reserved!" << std::endl;
                return false;
            }
            // variadic arguments are not allowed and parsed with a name ending in ...
            if (name.length() >= 3 && name.compare(name.length() - 3, 3, "...") == 0)
            {
                std::cout << "variadic arguments are not allowed!" << std::endl;
                return false;
            }
            return true;
        }

        bool isTypeAllowed(const std::string& type)
        {
            // TODO: scope resolution :: -> only structs, enums, namespaces defined in contracts and qpi.h

            if (type.length() >= 3 && type.compare(type.length() - 3, 3, "...") == 0)
            {
                std::cout << "variadic arguments or parameter packs are not allowed!" << std::endl;
                return false;
            }
            std::vector<std::string> forbiddenTypes = { "float", "double", "string", "char", "QpiContext"};
            for (const auto& forbiddenType : forbiddenTypes)
            {
                if (type.find(forbiddenType) != std::string::npos)
                {
                    std::cout << "Type " << forbiddenType << " is not allowed." << std::endl;
                    return false;
                }
            }

            return true;
        }

        bool checkVarType(const cppast::CppVarType& varType, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // if global scope this has to be const or constexpr
            if (scopeStack.empty())
            {
                const auto attr = varType.typeAttr() | (IsConst(varType) ? cppast::CppIdentifierAttrib::CONST : 0);
                if (!(attr & cppast::CppIdentifierAttrib::CONST || attr & cppast::CppIdentifierAttrib::CONST_EXPR))
                {
                    std::cout << "Global variables are not allowed. You may use global constants (const/constexpr)." << std::endl;
                    return false;
                }
            }

            if (varType.compound())
                RETURN_IF_FALSE(checkEntity(*varType.compound(), stateStructName, scopeStack));
            else
                RETURN_IF_FALSE(isTypeAllowed(varType.baseType()));

            if (varType.typeModifier().ptrLevel_ > 0)
            {
                std::cout << "Pointers are not allowed." << std::endl;
                return false;
            }

            if (varType.parameterPack() || varType.baseType().find("...") != std::string::npos)
            {
                std::cout << "Parameter packs are not allowed." << std::endl;
                return false;
            }

            return true;
        }

        bool checkVarDecl(const cppast::CppVarDecl& varDecl, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            RETURN_IF_FALSE(isNameAllowed(varDecl.name()));
            if (scopeStack.empty())
            {
                // global constant name has to start with stateStructName
                if (varDecl.name().compare(0, stateStructName.length(), stateStructName) != 0)
                {
                    std::cout << "name of global constant has to start with state struct name" << std::endl;
                    return false;
                }
            }

            if (!varDecl.arraySizes().empty())
            {
                std::cout << "Plain arrays are not allowed, use the Array class provided by the QPI instead." << std::endl;
                return false;
            }

            if (!varDecl.isInitialized())
                return true;

            if (varDecl.initializeType() == cppast::CppVarInitializeType::USING_EQUAL)
            {
                RETURN_IF_FALSE(checkExpr(*varDecl.assignValue(), stateStructName, scopeStack));
            }
            else if (varDecl.initializeType() == cppast::CppVarInitializeType::DIRECT_CONSTRUCTOR_CALL)
            {
                for (const auto& expr : varDecl.constructorCallArgs())
                    RETURN_IF_FALSE(checkExpr(*expr, stateStructName, scopeStack));
            }

            return true;
        }

        bool checkAtomicExpr(const cppast::CppAtomicExpr& expr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            switch (expr.atomicExpressionType())
            {
            case cppast::CppAtomicExprType::STRING_LITERAL:
                std::cout << "String literals are not allowed." << std::endl;
                return false;
            case cppast::CppAtomicExprType::CHAR_LITERAL:
                std::cout << "Char literals are not allowed." << std::endl;
                return false;
            case cppast::CppAtomicExprType::NUMBER_LITEREL:
                return true;
            case cppast::CppAtomicExprType::NAME:
                return isNameAllowed((static_cast<const cppast::CppNameExpr&>(expr)).value());
            case cppast::CppAtomicExprType::VARTYPE:
                return checkVarType((static_cast<const cppast::CppVartypeExpr&>(expr)).value(), stateStructName, scopeStack);
            case cppast::CppAtomicExprType::LAMBDA:
                // TODO: add checks for lambdas
                std::cout << "Lambda expressions are not supported yet." << std::endl;
                return false;
            default:
                std::cout << "Unknown atomic expression type: " << (int)expr.atomicExpressionType() << std::endl;
                return false;
            }
        }

        bool checkMonomialExpr(const cppast::CppMonomialExpr& expr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            switch (expr.oper())
            {
            case cppast::CppUnaryOperator::UNARY_PLUS:
            case cppast::CppUnaryOperator::UNARY_MINUS:
            case cppast::CppUnaryOperator::PREFIX_INCREMENT:
            case cppast::CppUnaryOperator::PREFIX_DECREMENT:
            case cppast::CppUnaryOperator::POSTFIX_INCREMENT:
            case cppast::CppUnaryOperator::POSTFIX_DECREMENT:
            case cppast::CppUnaryOperator::BIT_TOGGLE:
            case cppast::CppUnaryOperator::LOGICAL_NOT:
            case cppast::CppUnaryOperator::PARENTHESIZE:
            case cppast::CppUnaryOperator::SIZE_OF:
                return checkExpr(expr.term(), stateStructName, scopeStack);
            case cppast::CppUnaryOperator::DEREFER:
                std::cout << "Pointer dereferencing (unary operator `*`) is not allowed." << std::endl;
                return false;
            case cppast::CppUnaryOperator::REFER:
                std::cout << "Variable referencing (unary operator `&`) is not allowed." << std::endl;
                return false;
            case cppast::CppUnaryOperator::NEW:
                std::cout << "Allocation via `new` is not allowed." << std::endl;
                return false;
            case cppast::CppUnaryOperator::DELETE:
            case cppast::CppUnaryOperator::DELETE_AARAY:
                std::cout << "Deallocation via `delete` is not allowed." << std::endl;
                return false;
            case cppast::CppUnaryOperator::VARIADIC:
            case cppast::CppUnaryOperator::VARIADIC_SIZE_OF:
                std::cout << "Variadic expressions are not allowed." << std::endl;
                return false;
            default:
                std::cout << "Unknown unary operator: " << (int)expr.oper() << std::endl;
                return false;
            }
        }

        bool checkBinomialExpr(const cppast::CppBinomialExpr& expr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            switch (expr.oper())
            {
            case cppast::CppBinaryOperator::PLUS:
            case cppast::CppBinaryOperator::MINUS:
            case cppast::CppBinaryOperator::MUL:
            case cppast::CppBinaryOperator::AND:
            case cppast::CppBinaryOperator::OR:
            case cppast::CppBinaryOperator::XOR:
            case cppast::CppBinaryOperator::ASSIGN:
            case cppast::CppBinaryOperator::LESS:
            case cppast::CppBinaryOperator::GREATER:
            case cppast::CppBinaryOperator::COMMA:
            case cppast::CppBinaryOperator::LOGICAL_AND:
            case cppast::CppBinaryOperator::LOGICAL_OR:
            case cppast::CppBinaryOperator::PLUS_ASSIGN:
            case cppast::CppBinaryOperator::MINUS_ASSIGN:
            case cppast::CppBinaryOperator::MUL_ASSIGN:
            case cppast::CppBinaryOperator::XOR_ASSIGN:
            case cppast::CppBinaryOperator::AND_ASSIGN:
            case cppast::CppBinaryOperator::OR_ASSIGN:
            case cppast::CppBinaryOperator::LEFT_SHIFT:
            case cppast::CppBinaryOperator::RIGHT_SHIFT:
            case cppast::CppBinaryOperator::EXTRACTION:
            case cppast::CppBinaryOperator::EQUAL:
            case cppast::CppBinaryOperator::NOT_EQUAL:
            case cppast::CppBinaryOperator::LESS_EQUAL:
            case cppast::CppBinaryOperator::GREATER_EQUAL:
            case cppast::CppBinaryOperator::LSHIFT_ASSIGN:
            case cppast::CppBinaryOperator::RSHIFT_ASSIGN:
            case cppast::CppBinaryOperator::THREE_WAY_CMP:
            case cppast::CppBinaryOperator::USER_LITERAL:
            case cppast::CppBinaryOperator::DOT:
                return checkExpr(expr.term1(), stateStructName, scopeStack)
                    && checkExpr(expr.term2(), stateStructName, scopeStack);
            case cppast::CppBinaryOperator::DIV:
            case cppast::CppBinaryOperator::DIV_ASSIGN:
                std::cout << "Division operator `/` is not allowed. Use the function provided in the QPI instead." << std::endl;
                return false;
            case cppast::CppBinaryOperator::PERCENT:
            case cppast::CppBinaryOperator::PERCENT_ASSIGN:
                std::cout << "Modulo operator `%` is not allowed. Use the function provided in the QPI instead." << std::endl;
                return false;
            case cppast::CppBinaryOperator::ARRAY_INDEX:
                std::cout << "Plain arrays are not allowed, use the Array class provided by the QPI instead." << std::endl;
                return false;
            case cppast::CppBinaryOperator::PLACEMENT_NEW:
            case cppast::CppBinaryOperator::GLOBAL_PLACEMENT_NEW:
                std::cout << "Construction via placement `new` is not allowed." << std::endl;
                return false;
            case cppast::CppBinaryOperator::ARROW:
            case cppast::CppBinaryOperator::ARROW_STAR:
                std::cout << "Dereferencing (operator `->` or `->*`) is not allowed." << std::endl;
                return false;
            default:
                std::cout << "Unknown binary operator: " << (int)expr.oper() << std::endl;
                return false;
            }
        }

        bool checkTrinomialExpr(const cppast::CppTrinomialExpr& expr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            switch (expr.oper())
            {
            case cppast::CppTernaryOperator::CONDITIONAL:
                return checkExpr(expr.term1(), stateStructName, scopeStack) 
                    && checkExpr(expr.term2(), stateStructName, scopeStack)
                    && checkExpr(expr.term3(), stateStructName, scopeStack);
            default:
                std::cout << "Unknown ternary operator: " << (int)expr.oper() << std::endl;
                return false;
            }
        }

        bool checkFuncCallExpr(const cppast::CppFunctionCallExpr& expr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            RETURN_IF_FALSE(checkExpr(expr.function(), stateStructName, scopeStack));
            for (size_t i = 0; i < expr.numArgs(); ++i)
                RETURN_IF_FALSE(checkExpr(expr.arg(i), stateStructName, scopeStack));

            return true;
        }

        bool checkUniformInitializerExpr(const cppast::CppUniformInitializerExpr& expr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            RETURN_IF_FALSE(isNameAllowed(expr.name()));
            for (size_t i = 0; i < expr.numArgs(); ++i)
                RETURN_IF_FALSE(checkExpr(expr.arg(i), stateStructName, scopeStack));

            return true;
        }
        
        bool checkInitializerListExpr(const cppast::CppInitializerListExpr& expr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            for (size_t i = 0; i < expr.numArgs(); ++i)
                RETURN_IF_FALSE(checkExpr(expr.arg(i), stateStructName, scopeStack));

            return true;
        }

        bool checkTypecastExpr(const cppast::CppTypecastExpr& expr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            switch (expr.castType())
            {
            case cppast::CppTypecastType::C_STYLE:
            case cppast::CppTypecastType::FUNCTION_STYLE:
            case cppast::CppTypecastType::STATIC:
            case cppast::CppTypecastType::DYNAMIC:
            case cppast::CppTypecastType::REINTERPRET:
                return checkVarType(expr.targetType(), stateStructName, scopeStack)
                    && checkExpr(expr.inputExpresion(), stateStructName, scopeStack);
            case cppast::CppTypecastType::CONST:
                std::cout << "`const_cast` is not allowed." << std::endl;
                return false;
            default:
                std::cout << "Unknown cast type: " << (int)expr.castType() << std::endl;
                return false;
            }
        }

        bool checkExpr(const cppast::CppExpression& expr, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            switch (expr.expressionType())
            {
            case cppast::CppExpressionType::ATOMIC:
                return checkAtomicExpr(static_cast<const cppast::CppAtomicExpr&>(expr), stateStructName, scopeStack);
            case cppast::CppExpressionType::MONOMIAL:
                return checkMonomialExpr(static_cast<const cppast::CppMonomialExpr&>(expr), stateStructName, scopeStack);
            case cppast::CppExpressionType::BINOMIAL:
                return checkBinomialExpr(static_cast<const cppast::CppBinomialExpr&>(expr), stateStructName, scopeStack);
            case cppast::CppExpressionType::TRINOMIAL:
                return checkTrinomialExpr(static_cast<const cppast::CppTrinomialExpr&>(expr), stateStructName, scopeStack);
            case cppast::CppExpressionType::FUNCTION_CALL:
                return checkFuncCallExpr(static_cast<const cppast::CppFunctionCallExpr&>(expr), stateStructName, scopeStack);
            case cppast::CppExpressionType::UNIFORM_INITIALIZER:
                return checkUniformInitializerExpr(static_cast<const cppast::CppUniformInitializerExpr&>(expr), stateStructName, scopeStack);
            case cppast::CppExpressionType::INITIALIZER_LIST:
                return checkInitializerListExpr(static_cast<const cppast::CppInitializerListExpr&>(expr), stateStructName, scopeStack);
            case cppast::CppExpressionType::TYPECAST:
                return checkTypecastExpr(static_cast<const cppast::CppTypecastExpr&>(expr), stateStructName, scopeStack);
            default:
                std::cout << "Unknown expression type: " << (int)expr.expressionType() << std::endl;
                return false;
            }
        }

        // helper struct for visiting variants
        // refer to https://en.cppreference.com/w/cpp/utility/variant/visit2 for details
        template <class... Ts>
        struct Overloaded : Ts... { using Ts::operator()...; };

        bool checkTemplSpec(const cppast::CppTemplateParams& params, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            for (const auto& param : params)
            {
                if (param.paramType().has_value())
                {
                    RETURN_IF_FALSE(
                        std::visit(Overloaded{ 
                                [&](const std::unique_ptr<cppast::CppVarType>& varType) -> bool 
                                {
                                    return checkVarType(*varType, stateStructName, scopeStack);
                                },
                                [&](const std::unique_ptr<cppast::CppFunctionPointer>& funcPtr) -> bool 
                                {
                                    std::cout << "function pointer is not allowed!" << std::endl;
                                    return false;
                                } 
                            },
                            param.paramType().value()
                        )
                    );
                }

                RETURN_IF_FALSE(isNameAllowed(param.paramName()));

                RETURN_IF_FALSE(
                    std::visit(Overloaded{ 
                            [&](const std::unique_ptr<cppast::CppVarType>& varType) -> bool 
                            {
                                if (varType)
                                    return checkVarType(*varType, stateStructName, scopeStack);
                                else
                                    return true;
                            },
                            [&](const std::unique_ptr<cppast::CppExpression>& expr) -> bool 
                            {
                                if (expr)
                                    return checkExpr(*expr, stateStructName, scopeStack);
                                else
                                    return true;
                            } 
                        },
                        param.defaultArg()
                    )
                );
            }
            return true;
        }

        bool checkVar(const cppast::CppVar& var, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // TODO: input and output structs only use basic types

            if (!(scopeStack.empty() || scopeStack.back() == ScopeSpec::STRUCT || scopeStack.back() == ScopeSpec::CLASS))
            {
                std::cout << "local variables are not allowed" << std::endl;
                return false;
            }

            if (var.isTemplated())
                RETURN_IF_FALSE(checkTemplSpec(var.templateSpecification().value(), stateStructName, scopeStack));

            RETURN_IF_FALSE(checkVarType(var.varType(), stateStructName, scopeStack));
            RETURN_IF_FALSE(checkVarDecl(var.varDecl(), stateStructName, scopeStack));

            return true;
        }

        bool checkVarList(const cppast::CppVarList& varList, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            RETURN_IF_FALSE(checkVar(*varList.firstVar(), stateStructName, scopeStack));
            auto& varDeclList = varList.varDeclList();
            for (const auto& decl : varDeclList)
            {
                if (decl.ptrLevel_ > 0)
                {
                    std::cout << "Pointers are not allowed." << std::endl;
                    return false;
                }
                RETURN_IF_FALSE(checkVarDecl(decl, stateStructName, scopeStack));
            }

            return true;
        }

        bool checkCompound(const cppast::CppCompound& compound, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            if (IsNamespaceLike(compound))
            {
                if (compound.compoundType() == cppast::CppCompoundType::UNION)
                {
                    std::cout << "union is not allowed!" << std::endl;
                    return false;
                }
                if (compound.isTemplated())
                {
                    checkTemplSpec(compound.templateSpecification().value(), stateStructName, scopeStack);
                }
                RETURN_IF_FALSE(isNameAllowed(compound.name()));
            }
            if (!compound.inheritanceList().empty())
            {
                for (const auto& inheritanceInfo : compound.inheritanceList())
                {
                    RETURN_IF_FALSE(isInheritanceAllowed(inheritanceInfo.baseName));
                }
            }

            bool scopeStackPushed = true;
            switch (compound.compoundType())
            {
            case cppast::CppCompoundType::STRUCT:
                scopeStack.push_back(ScopeSpec::STRUCT);
                break;
            case cppast::CppCompoundType::CLASS:
                scopeStack.push_back(ScopeSpec::CLASS);
                break;
            case cppast::CppCompoundType::NAMESPACE:
                scopeStack.push_back(ScopeSpec::NAMESPACE);
                break;
            case cppast::CppCompoundType::BLOCK:
            case cppast::CppCompoundType::EXTERN_C_BLOCK:
                scopeStack.push_back(ScopeSpec::BLOCK);
                break;
            default:
                scopeStackPushed = false;
                break;
            }

            bool checkSucceeded = compound.visitAll([&](const cppast::CppEntity& ent) -> bool { return checkEntity(ent, stateStructName, scopeStack); });
            if (scopeStackPushed)
                scopeStack.pop_back();

            return checkSucceeded;
        }

        bool checkUsingNamespace(const cppast::CppUsingNamespaceDecl& decl, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // in global scope, only namespace QPI is allowed
            if (scopeStack.empty()) // global scope
            {
                if (decl.name().compare("QPI") != 0)
                {
                    std::cout << "in global scope, only QPI can be used for a using namespace declaration" << std::endl;
                    return false;
                }
            }

            // TODO: scope resolution :: -> only structs, enums, namespaces defined in contracts and qpi.h

            return true;
        }

        bool checkUsingDecl(const cppast::CppUsingDecl& decl, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            // in global scope not allowed, otherwise ok

            if (scopeStack.empty()) // global scope
            {
                std::cout << "using declaration is not allowed in global scope" << std::endl;
                return false;
            }

            if (decl.isTemplated())
                RETURN_IF_FALSE(checkTemplSpec(decl.templateSpecification().value(), stateStructName, scopeStack));

            // TODO: scope resolution :: -> only structs, enums, namespaces defined in contracts and qpi.h
            // -> is this contained in decl.name() ?

            RETURN_IF_FALSE(
                std::visit(Overloaded{ 
                        [&](const std::unique_ptr<cppast::CppVarType>& varType) -> bool
                        {
                            if (varType)
                            {
                                return checkVarType(*varType, stateStructName, scopeStack);
                            }
                            return true;
                        },
                        [&](const std::unique_ptr<cppast::CppFunctionPointer>& funcPtr) -> bool
                        {
                            if (funcPtr)
                            {
                                std::cout << "function pointer is not allowed!" << std::endl;
                                return false;
                            }
                            return true;
                        },
                        [&](const std::unique_ptr<cppast::CppCompound>& compound) -> bool
                        {
                            if (compound)
                            {
                                return checkCompound(*compound, stateStructName, scopeStack);
                            }
                            return true;
                        } 
                    },
                    decl.definition()
                )
            );

            return true;
        }

        bool checkGotoStatement(const cppast::CppGotoStatement& gotoStatement, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            return checkExpr(gotoStatement.label(), stateStructName, scopeStack);
        }

        bool checkFwdDecl(const cppast::CppForwardClassDecl& fwdDecl, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            if (fwdDecl.isTemplated())
                RETURN_IF_FALSE(checkTemplSpec(fwdDecl.templateSpecification().value(), stateStructName, scopeStack));
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
                    RETURN_IF_FALSE(isNameAllowed(memInit.memberName));
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

            RETURN_IF_FALSE(isNameAllowed(func.name()));

            const auto params = GetAllParams(func);
            if (!params.empty())
                RETURN_IF_FALSE(checkParamList(params, stateStructName, scopeStack));
            
            if (func.defn())
                RETURN_IF_FALSE(checkCompound(*func.defn(), stateStructName, scopeStack));

            return true;
        }

        bool checkReturn(const cppast::CppReturnStatement& returnStatement, const std::string& stateStructName, std::deque<ScopeSpec>& scopeStack)
        {
            if (returnStatement.hasReturnValue())
                RETURN_IF_FALSE(checkExpr(returnStatement.returnValue(), stateStructName, scopeStack));
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

            case cppast::CppEntityType::GOTO_STATEMENT:
                return checkGotoStatement((const cppast::CppGotoStatement&)entity, stateStructName, scopeStack);

            case cppast::CppEntityType::FORWARD_CLASS_DECL:
                return checkFwdDecl((const cppast::CppForwardClassDecl&)entity, stateStructName, scopeStack);

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

    std::string findStateStructName(const cppast::CppCompound& ast)
    {
        // Assumption: state struct is the first top-level struct that inherits from ContractBase
        std::string name = "";

        if (ast.compoundType() != cppast::CppCompoundType::FILE)
        {
            std::cout << "Need a top-level compound (compound type FILE) for finding the state struct name." << std::endl;
            return name;
        }

        // `visitAll` visits the entities sequentially, so we do not need any lock for `name`
        ast.visitAll([&](const cppast::CppEntity& entity) -> bool
            {
                if (name.empty() && entity.entityType() == cppast::CppEntityType::COMPOUND)
                {
                    const auto& compound = (const cppast::CppCompound&)entity;
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

