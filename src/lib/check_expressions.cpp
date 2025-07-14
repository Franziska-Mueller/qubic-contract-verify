#include "check_expressions.h"

#include <iostream>
#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "check_functionlike.h"
#include "check_names_and_types.h"
#include "check_variables.h"
#include "defines.h"


namespace contractverify
{
    namespace
    {

        bool checkAtomicExpr(const cppast::CppAtomicExpr& expr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
        {
            switch (expr.atomicExpressionType())
            {
            case cppast::CppAtomicExprType::STRING_LITERAL:
                std::cout << "[ ERROR ] String literals are not allowed, found " 
                    << static_cast<const cppast::CppStringLiteralExpr&>(expr).value() << "." << std::endl;
                return false;
            case cppast::CppAtomicExprType::CHAR_LITERAL:
                std::cout << "[ ERROR ] Char literals are not allowed, found " 
                    << static_cast<const cppast::CppCharLiteralExpr&>(expr).value() << "." << std::endl;
                return false;
            case cppast::CppAtomicExprType::NUMBER_LITEREL:
                return true;
            case cppast::CppAtomicExprType::NAME:
                return isNameAllowed((static_cast<const cppast::CppNameExpr&>(expr)).value(), additionalScopePrefixes);
            case cppast::CppAtomicExprType::VARTYPE:
                return checkVarType((static_cast<const cppast::CppVartypeExpr&>(expr)).value(), stateStructName, scopeStack, additionalScopePrefixes);
            case cppast::CppAtomicExprType::LAMBDA:
                return checkLambda((static_cast<const cppast::CppLambdaExpr&>(expr)).lamda(), stateStructName, scopeStack, additionalScopePrefixes);
            default:
                std::cout << "[ ERROR ] Unknown atomic expression type: " << (int)expr.atomicExpressionType() << std::endl;
                return false;
            }
        }

        bool checkMonomialExpr(const cppast::CppMonomialExpr& expr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
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
                return checkExpr(expr.term(), stateStructName, scopeStack, additionalScopePrefixes);
            case cppast::CppUnaryOperator::DEREFER:
                std::cout << "[ ERROR ] Pointer dereferencing (unary operator `*`) is not allowed." << std::endl;
                return false;
            case cppast::CppUnaryOperator::REFER:
                std::cout << "[ ERROR ] Variable referencing (unary operator `&`) is not allowed." << std::endl;
                return false;
            case cppast::CppUnaryOperator::NEW:
                std::cout << "[ ERROR ] Allocation via `new` is not allowed." << std::endl;
                return false;
            case cppast::CppUnaryOperator::DELETE:
            case cppast::CppUnaryOperator::DELETE_AARAY:
                std::cout << "[ ERROR ] Deallocation via `delete` is not allowed." << std::endl;
                return false;
            case cppast::CppUnaryOperator::VARIADIC:
            case cppast::CppUnaryOperator::VARIADIC_SIZE_OF:
                std::cout << "[ ERROR ] Variadic expressions are not allowed." << std::endl;
                return false;
            default:
                std::cout << "[ ERROR ] Unknown unary operator: " << (int)expr.oper() << std::endl;
                return false;
            }
        }

        bool checkBinomialExpr(const cppast::CppBinomialExpr& expr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
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
                return checkExpr(expr.term1(), stateStructName, scopeStack, additionalScopePrefixes)
                    && checkExpr(expr.term2(), stateStructName, scopeStack, additionalScopePrefixes);
            case cppast::CppBinaryOperator::DIV:
            case cppast::CppBinaryOperator::DIV_ASSIGN:
                std::cout << "[ ERROR ] Division operator `/` is not allowed. Use the `div` function provided in the QPI instead." << std::endl;
                return false;
            case cppast::CppBinaryOperator::PERCENT:
            case cppast::CppBinaryOperator::PERCENT_ASSIGN:
                std::cout << "[ ERROR ] Modulo operator `%` is not allowed. Use the `mod` function provided in the QPI instead." << std::endl;
                return false;
            case cppast::CppBinaryOperator::ARRAY_INDEX:
                std::cout << "[ ERROR ] Plain arrays are not allowed, use the Array class provided by the QPI instead." << std::endl;
                return false;
            case cppast::CppBinaryOperator::PLACEMENT_NEW:
            case cppast::CppBinaryOperator::GLOBAL_PLACEMENT_NEW:
                std::cout << "[ ERROR ] Construction via placement `new` is not allowed." << std::endl;
                return false;
            case cppast::CppBinaryOperator::ARROW:
            case cppast::CppBinaryOperator::ARROW_STAR:
                std::cout << "[ ERROR ] Dereferencing (operator `->` or `->*`) is not allowed." << std::endl;
                return false;
            default:
                std::cout << "[ ERROR ] Unknown binary operator: " << (int)expr.oper() << std::endl;
                return false;
            }
        }

        bool checkTrinomialExpr(const cppast::CppTrinomialExpr& expr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
        {
            switch (expr.oper())
            {
            case cppast::CppTernaryOperator::CONDITIONAL:
                return checkExpr(expr.term1(), stateStructName, scopeStack, additionalScopePrefixes)
                    && checkExpr(expr.term2(), stateStructName, scopeStack, additionalScopePrefixes)
                    && checkExpr(expr.term3(), stateStructName, scopeStack, additionalScopePrefixes);
            default:
                std::cout << "[ ERROR ] Unknown ternary operator: " << (int)expr.oper() << std::endl;
                return false;
            }
        }

        bool checkFuncCallExpr(const cppast::CppFunctionCallExpr& expr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
        {
            RETURN_IF_FALSE(checkExpr(expr.function(), stateStructName, scopeStack, additionalScopePrefixes));
            for (size_t i = 0; i < expr.numArgs(); ++i)
                RETURN_IF_FALSE(checkExpr(expr.arg(i), stateStructName, scopeStack, additionalScopePrefixes));

            return true;
        }

        bool checkUniformInitializerExpr(const cppast::CppUniformInitializerExpr& expr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
        {
            RETURN_IF_FALSE(isNameAllowed(expr.name(), additionalScopePrefixes));
            for (size_t i = 0; i < expr.numArgs(); ++i)
                RETURN_IF_FALSE(checkExpr(expr.arg(i), stateStructName, scopeStack, additionalScopePrefixes));

            return true;
        }

        bool checkInitializerListExpr(const cppast::CppInitializerListExpr& expr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
        {
            for (size_t i = 0; i < expr.numArgs(); ++i)
                RETURN_IF_FALSE(checkExpr(expr.arg(i), stateStructName, scopeStack, additionalScopePrefixes));

            return true;
        }

        bool checkTypecastExpr(const cppast::CppTypecastExpr& expr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
        {
            switch (expr.castType())
            {
            case cppast::CppTypecastType::C_STYLE:
            case cppast::CppTypecastType::FUNCTION_STYLE:
            case cppast::CppTypecastType::STATIC:
            case cppast::CppTypecastType::DYNAMIC:
            case cppast::CppTypecastType::REINTERPRET:
                return checkVarType(expr.targetType(), stateStructName, scopeStack, additionalScopePrefixes)
                    && checkExpr(expr.inputExpresion(), stateStructName, scopeStack, additionalScopePrefixes);
            case cppast::CppTypecastType::CONST:
                std::cout << "[ ERROR ] `const_cast` is not allowed." << std::endl;
                return false;
            default:
                std::cout << "[ ERROR ] Unknown cast type: " << (int)expr.castType() << std::endl;
                return false;
            }
        }

    }  // namespace

    bool checkExpr(const cppast::CppExpression& expr, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        switch (expr.expressionType())
        {
        case cppast::CppExpressionType::ATOMIC:
            return checkAtomicExpr(static_cast<const cppast::CppAtomicExpr&>(expr), stateStructName, scopeStack, additionalScopePrefixes);
        case cppast::CppExpressionType::MONOMIAL:
            return checkMonomialExpr(static_cast<const cppast::CppMonomialExpr&>(expr), stateStructName, scopeStack, additionalScopePrefixes);
        case cppast::CppExpressionType::BINOMIAL:
            return checkBinomialExpr(static_cast<const cppast::CppBinomialExpr&>(expr), stateStructName, scopeStack, additionalScopePrefixes);
        case cppast::CppExpressionType::TRINOMIAL:
            return checkTrinomialExpr(static_cast<const cppast::CppTrinomialExpr&>(expr), stateStructName, scopeStack, additionalScopePrefixes);
        case cppast::CppExpressionType::FUNCTION_CALL:
            return checkFuncCallExpr(static_cast<const cppast::CppFunctionCallExpr&>(expr), stateStructName, scopeStack, additionalScopePrefixes);
        case cppast::CppExpressionType::UNIFORM_INITIALIZER:
            return checkUniformInitializerExpr(static_cast<const cppast::CppUniformInitializerExpr&>(expr), stateStructName, scopeStack, additionalScopePrefixes);
        case cppast::CppExpressionType::INITIALIZER_LIST:
            return checkInitializerListExpr(static_cast<const cppast::CppInitializerListExpr&>(expr), stateStructName, scopeStack, additionalScopePrefixes);
        case cppast::CppExpressionType::TYPECAST:
            return checkTypecastExpr(static_cast<const cppast::CppTypecastExpr&>(expr), stateStructName, scopeStack, additionalScopePrefixes);
        default:
            std::cout << "[ ERROR ] Unknown expression type: " << (int)expr.expressionType() << std::endl;
            return false;
        }
    }

}  // namespace contractverify