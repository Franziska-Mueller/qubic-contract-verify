#pragma once

#define RETURN_IF_FALSE(x) { if(!x) return false; } // wrapped in braces to make it work correctly when used inside an `if` block that is followed by `else`


namespace contractverify
{
    enum ScopeSpec
    {
        STRUCT = 0,
        CLASS = 1,
        NAMESPACE = 2,
        BLOCK = 3,
        TEMPL_SPEC = 4,  // this is needed to distinguish variables in template specs from normal variable declarations
        FUNC_SIG = 5,  // this is needed to distinguish variables/types in param lists/return types from normal variable declarations
        TYPEDEF = 6,  // this is needed to distinguish local variables (forbidden) from local typedefs (allowed)
    };

    // helper struct for visiting variants
    // refer to https://en.cppreference.com/w/cpp/utility/variant/visit2 for details
    template <class... Ts>
    struct Overloaded : Ts... { using Ts::operator()...; };

    static const std::vector<std::string> knownMacroNames = {
            "PUBLIC_FUNCTION",
            "PUBLIC_FUNCTION_WITH_LOCALS",
            "PUBLIC_PROCEDURE",
            "PUBLIC_PROCEDURE_WITH_LOCALS",
            "END_TICK",
            "END_TICK_WITH_LOCALS",
            "BEGIN_TICK",
            "BEGIN_TICK_WITH_LOCALS",
            "BEGIN_EPOCH",
            "BEGIN_EPOCH_WITH_LOCALS",
            "END_EPOCH",
            "END_EPOCH_WITH_LOCALS",
            "REGISTER_USER_FUNCTIONS_AND_PROCEDURES",
            "REGISTER_USER_FUNCTION",
            "REGISTER_USER_PROCEDURE",
    };

    static const std::vector<std::string> allowedScopePrefixes = {
        // QPI and names defined in qpi.h
        "QPI",
        "ProposalTypes",
        "TransferType",
        "AssetIssuanceSelect",
        "AssetOwnershipSelect",
        "AssetPossessionSelect",
        // other contract names
        "QUOTTERY",
        "QX",
        "TESTEXA",
        "TESTEXB",
        // the following names are defined and used in the same contract file
        // -> TODO: add structs/enums from same file to whitelist before checking compliance
        "QBAYLogInfo",
        "QuotteryLogInfo",
        "AssetAskOrders_output",
        "AssetBidOrders_output",
        "EntityAskOrders_output",
        "EntityBidOrders_output",
    };
}