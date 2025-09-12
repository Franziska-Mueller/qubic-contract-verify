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
    bool checkIfBlock(const cppast::CppIfBlock& ifBlock, const std::string& stateStructName, AnalysisData& analysisData)
    {
        RETURN_IF_FALSE(checkEntity(*ifBlock.condition(), stateStructName, analysisData));

        if (ifBlock.body())
            RETURN_IF_FALSE(checkEntity(*ifBlock.body(), stateStructName, analysisData));

        if (ifBlock.elsePart())
            RETURN_IF_FALSE(checkEntity(*ifBlock.elsePart(), stateStructName, analysisData));

        return true;
    }

    bool checkForBlock(const cppast::CppForBlock& forBlock, const std::string& stateStructName, AnalysisData& analysisData)
    {
        if (forBlock.start())
            RETURN_IF_FALSE(checkEntity(*forBlock.start(), stateStructName, analysisData));
        if (forBlock.stop())
            RETURN_IF_FALSE(checkExpr(*forBlock.stop(), stateStructName, analysisData));
        if (forBlock.step())
            RETURN_IF_FALSE(checkExpr(*forBlock.step(), stateStructName, analysisData));
        if (forBlock.body())
            RETURN_IF_FALSE(checkEntity(*forBlock.body(), stateStructName, analysisData));

        return true;
    }

    bool checkRangeForBlock(const cppast::CppRangeForBlock& forBlock, const std::string& stateStructName, AnalysisData& analysisData)
    {
        if (forBlock.var())
            RETURN_IF_FALSE(checkVar(*forBlock.var(), stateStructName, analysisData));
        if (forBlock.expr())
            RETURN_IF_FALSE(checkExpr(*forBlock.expr(), stateStructName, analysisData));
        if (forBlock.body())
            RETURN_IF_FALSE(checkEntity(*forBlock.body(), stateStructName, analysisData));

        return true;
    }

    bool checkWhileBlock(const cppast::CppWhileBlock& whileBlock, const std::string& stateStructName, AnalysisData& analysisData)
    {
        RETURN_IF_FALSE(checkEntity(*whileBlock.condition(), stateStructName, analysisData));

        if (whileBlock.body())
            RETURN_IF_FALSE(checkEntity(*whileBlock.body(), stateStructName, analysisData));

        return true;
    }

    bool checkDoWhileBlock(const cppast::CppDoWhileBlock& doWhileBlock, const std::string& stateStructName, AnalysisData& analysisData)
    {
        if (doWhileBlock.body())
            RETURN_IF_FALSE(checkEntity(*doWhileBlock.body(), stateStructName, analysisData));

        RETURN_IF_FALSE(checkEntity(*doWhileBlock.condition(), stateStructName, analysisData));

        return true;
    }

    bool checkSwitchBlock(const cppast::CppSwitchBlock& switchBlock, const std::string& stateStructName, AnalysisData& analysisData)
    {
        RETURN_IF_FALSE(checkExpr(*switchBlock.condition(), stateStructName, analysisData));

        for (const auto& caseStmt : switchBlock.body())
        {
            if (caseStmt.caseExpr())
                RETURN_IF_FALSE(checkExpr(*caseStmt.caseExpr(), stateStructName, analysisData));
            if (caseStmt.body())
                RETURN_IF_FALSE(checkCompound(*caseStmt.body(), stateStructName, analysisData));
        }

        return true;
    }

    bool checkGotoStatement(const cppast::CppGotoStatement& gotoStatement, const std::string& stateStructName, AnalysisData& analysisData)
    {
        return checkExpr(gotoStatement.label(), stateStructName, analysisData);
    }

}  // namespace contractverify