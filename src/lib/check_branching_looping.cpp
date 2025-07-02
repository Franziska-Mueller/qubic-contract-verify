#include "check_branching_looping.h"

#include <iostream>
#include <stack>
#include <string>

#include <cppparser/cppparser.h>

#include "check_compliance.h"
#include "check_expressions.h"
#include "check_variables.h"
#include "defines.h"


namespace contractverify
{
    bool checkIfBlock(const cppast::CppIfBlock& ifBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        RETURN_IF_FALSE(checkEntity(*ifBlock.condition(), stateStructName, scopeStack, additionalScopePrefixes));

        if (ifBlock.body())
            RETURN_IF_FALSE(checkEntity(*ifBlock.body(), stateStructName, scopeStack, additionalScopePrefixes));

        if (ifBlock.elsePart())
            RETURN_IF_FALSE(checkEntity(*ifBlock.elsePart(), stateStructName, scopeStack, additionalScopePrefixes));

        return true;
    }

    bool checkForBlock(const cppast::CppForBlock& forBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        if (forBlock.start())
            RETURN_IF_FALSE(checkEntity(*forBlock.start(), stateStructName, scopeStack, additionalScopePrefixes));
        if (forBlock.stop())
            RETURN_IF_FALSE(checkExpr(*forBlock.stop(), stateStructName, scopeStack, additionalScopePrefixes));
        if (forBlock.step())
            RETURN_IF_FALSE(checkExpr(*forBlock.step(), stateStructName, scopeStack, additionalScopePrefixes));
        if (forBlock.body())
            RETURN_IF_FALSE(checkEntity(*forBlock.body(), stateStructName, scopeStack, additionalScopePrefixes));

        return true;
    }

    bool checkRangeForBlock(const cppast::CppRangeForBlock& forBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        if (forBlock.var())
            RETURN_IF_FALSE(checkVar(*forBlock.var(), stateStructName, scopeStack, additionalScopePrefixes));
        if (forBlock.expr())
            RETURN_IF_FALSE(checkExpr(*forBlock.expr(), stateStructName, scopeStack, additionalScopePrefixes));
        if (forBlock.body())
            RETURN_IF_FALSE(checkEntity(*forBlock.body(), stateStructName, scopeStack, additionalScopePrefixes));

        return true;
    }

    bool checkWhileBlock(const cppast::CppWhileBlock& whileBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        RETURN_IF_FALSE(checkEntity(*whileBlock.condition(), stateStructName, scopeStack, additionalScopePrefixes));

        if (whileBlock.body())
            RETURN_IF_FALSE(checkEntity(*whileBlock.body(), stateStructName, scopeStack, additionalScopePrefixes));

        return true;
    }

    bool checkDoWhileBlock(const cppast::CppDoWhileBlock& doWhileBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        if (doWhileBlock.body())
            RETURN_IF_FALSE(checkEntity(*doWhileBlock.body(), stateStructName, scopeStack, additionalScopePrefixes));

        RETURN_IF_FALSE(checkEntity(*doWhileBlock.condition(), stateStructName, scopeStack, additionalScopePrefixes));

        return true;
    }

    bool checkSwitchBlock(const cppast::CppSwitchBlock& switchBlock, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        RETURN_IF_FALSE(checkExpr(*switchBlock.condition(), stateStructName, scopeStack, additionalScopePrefixes));

        for (const auto& caseStmt : switchBlock.body())
        {
            if (caseStmt.caseExpr())
                RETURN_IF_FALSE(checkExpr(*caseStmt.caseExpr(), stateStructName, scopeStack, additionalScopePrefixes));
            if (caseStmt.body())
                RETURN_IF_FALSE(checkCompound(*caseStmt.body(), stateStructName, scopeStack, additionalScopePrefixes));
        }

        return true;
    }

    bool checkGotoStatement(const cppast::CppGotoStatement& gotoStatement, const std::string& stateStructName, std::stack<ScopeSpec>& scopeStack, std::vector<std::string>& additionalScopePrefixes)
    {
        return checkExpr(gotoStatement.label(), stateStructName, scopeStack, additionalScopePrefixes);
    }

}  // namespace contractverify