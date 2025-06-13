#include "check_branching_looping.h"

#include <iostream>
#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "check_compliance.h"
#include "check_expressions.h"
#include "defines.h"


namespace contractverify
{
    bool checkIfBlock(const cppast::CppIfBlock& ifBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
    {
        RETURN_IF_FALSE(checkEntity(*ifBlock.condition(), stateStructName, scopeStack));

        if (ifBlock.body())
            RETURN_IF_FALSE(checkEntity(*ifBlock.body(), stateStructName, scopeStack));

        if (ifBlock.elsePart())
            RETURN_IF_FALSE(checkEntity(*ifBlock.elsePart(), stateStructName, scopeStack));

        return true;
    }

    bool checkForBlock(const cppast::CppForBlock& forBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
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

    bool checkWhileBlock(const cppast::CppWhileBlock& whileBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
    {
        RETURN_IF_FALSE(checkEntity(*whileBlock.condition(), stateStructName, scopeStack));

        if (whileBlock.body())
            RETURN_IF_FALSE(checkEntity(*whileBlock.body(), stateStructName, scopeStack));

        return true;
    }

    bool checkDoWhileBlock(const cppast::CppDoWhileBlock& doWhileBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
    {
        if (doWhileBlock.body())
            RETURN_IF_FALSE(checkEntity(*doWhileBlock.body(), stateStructName, scopeStack));

        RETURN_IF_FALSE(checkEntity(*doWhileBlock.condition(), stateStructName, scopeStack));

        return true;
    }

    bool checkSwitchBlock(const cppast::CppSwitchBlock& switchBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
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

    bool checkGotoStatement(const cppast::CppGotoStatement& gotoStatement, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack)
    {
        return checkExpr(gotoStatement.label(), stateStructName, scopeStack);
    }

}  // namespace contractverify