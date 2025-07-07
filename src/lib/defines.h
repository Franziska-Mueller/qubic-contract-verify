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
        "INITIALIZE",
        "INITIALIZE_WITH_LOCALS",
        "BEGIN_EPOCH",
        "BEGIN_EPOCH_WITH_LOCALS",
        "END_EPOCH",
        "END_EPOCH_WITH_LOCALS",
        "BEGIN_TICK",
        "BEGIN_TICK_WITH_LOCALS",
        "END_TICK",
        "END_TICK_WITH_LOCALS",
        "PRE_ACQUIRE_SHARES",
        "PRE_ACQUIRE_SHARES_WITH_LOCALS",
        "PRE_RELEASE_SHARES",
        "PRE_RELEASE_SHARES_WITH_LOCALS",
        "POST_ACQUIRE_SHARES",
        "POST_ACQUIRE_SHARES_WITH_LOCALS",
        "POST_RELEASE_SHARES",
        "POST_RELEASE_SHARES_WITH_LOCALS",
        "POST_INCOMING_TRANSFER",
        "POST_INCOMING_TRANSFER_WITH_LOCALS",
        "EXPAND",
        "LOG_DEBUG",
        "LOG_ERROR",
        "LOG_INFO",
        "LOG_WARNING",
        "PRIVATE_FUNCTION",
        "PRIVATE_FUNCTION_WITH_LOCALS",
        "PRIVATE_PROCEDURE",
        "PRIVATE_PROCEDURE_WITH_LOCALS",
        "PUBLIC_FUNCTION",
        "PUBLIC_FUNCTION_WITH_LOCALS",
        "PUBLIC_PROCEDURE",
        "PUBLIC_PROCEDURE_WITH_LOCALS",
        "REGISTER_USER_FUNCTIONS_AND_PROCEDURES",
        "REGISTER_USER_FUNCTION",
        "REGISTER_USER_PROCEDURE",
        "CALL",
        "CALL_OTHER_CONTRACT_FUNCTION",
        "INVOKE_OTHER_CONTRACT_PROCEDURE",
        "QUERY_ORACLE",
        "SELF",
        "SELF_INDEX",
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
        //"QBAYLogInfo",
        //"QuotteryLogInfo",
        //"AssetAskOrders_output",
        //"AssetBidOrders_output",
        //"EntityAskOrders_output",
        //"EntityBidOrders_output",
    };
}