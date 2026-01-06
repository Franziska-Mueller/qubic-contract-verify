#pragma once

#include <sstream>
#include <stack>
#include <vector>

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

    static std::string getScopedName(const std::vector<std::string>& names, int startIndex = 0)
    {
        std::stringstream result;
        for (int s = startIndex; s < names.size(); ++s)
        {
            result << names[s];
            if (s != names.size() - 1)
                result << "::";
        }
        return result.str();
    }

    struct AnalysisData
    {
        // data that will be collected while traversing the AST
        std::stack<ScopeSpec> scopeStack; // empty scope stack means global scope
        std::vector<std::string> scopeNames;
        std::stack<bool> allowedAsIOStruct; // this stack tracks whether the struct/class currently being analyzed may be allowed as input/output struct
        std::vector<std::string> additionalScopePrefixes;
        std::vector<std::vector<std::string>> additionalInputOutputTypes;

        bool isDirectlyInClassOrStruct() const
        {
            return !scopeStack.empty() && (scopeStack.top() == ScopeSpec::CLASS || scopeStack.top() == ScopeSpec::STRUCT);
        }
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
        "LOG_PAUSE",
        "LOG_RESUME",
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
        "STATIC_ASSERT",
        // shareholder voting macros
        "DEFINE_SHAREHOLDER_PROPOSAL_STORAGE",
        "IMPLEMENT_SetShareholderProposal",
        "IMPLEMENT_GetShareholderProposal",
        "IMPLEMENT_GetShareholderProposalIndices",
        "IMPLEMENT_GetShareholderProposalFees",
        "IMPLEMENT_SetShareholderVotes",
        "IMPLEMENT_GetShareholderVotes",
        "IMPLEMENT_GetShareholderVotingResults",
        "IMPLEMENT_SET_SHAREHOLDER_PROPOSAL",
        "IMPLEMENT_SET_SHAREHOLDER_VOTES",
        "IMPLEMENT_FinalizeShareholderStateVarProposals",
        "IMPLEMENT_DEFAULT_SHAREHOLDER_PROPOSAL_VOTING",
        "REGISTER_SHAREHOLDER_PROPSAL_VOTING",
        "REGISTER_GetShareholderProposalFees",
        "REGISTER_GetShareholderProposalIndices",
        "REGISTER_GetShareholderProposal",
        "REGISTER_GetShareholderVotes",
        "REGISTER_GetShareholderVotingResults",
        "REGISTER_SetShareholderProposal",
        "REGISTER_SetShareholderVotes",
    };

    static const std::vector<std::string> allowedScopePrefixes = {
        // QPI and names defined in qpi.h
        "QPI",
        "id",
        "ProposalTypes",
        "TransferType",
        "AssetIssuanceSelect",
        "AssetOwnershipSelect",
        "AssetPossessionSelect",
        // other contract names
        "QX",
        "QUOTTERY",
        "RANDOM",
        "QUTIL",
        "MLM",
        "GQMPROP",
        "SWATCH",
        "CCF",
        "QEARN",
        "QVAULT",
        "MSVAULT",
        "QBAY",
        "QSWAP",
        "NOST",
        "QDRAW",
        "RL",
        "QBOND",
        "QIP",
        "QRAFFLE",
        "TESTEXA",
        "TESTEXB",
        "QRP",
        "QTF",
        "QDUEL",
        "QRWA",
    };

    static const std::vector<std::string> allowedInputOutputTypes = {
        // types and structs defined in qpi.h
        "id",
        "DateAndTime",
        "Entity",
        "Asset",
        "NoData",
        "ProposalDataV1<true>",
        "ProposalDataV1<false>",
        "ProposalSingleVoteDataV1",
        "ProposalSummarizedVotingDataV1",
        "ProposalDataYesNo",
        "PreManagementRightsTransfer_input",
        "PreManagementRightsTransfer_output",
        "PostManagementRightsTransfer_input",
        "PostIncomingTransfer_input",
        // types defined in other contracts
        "TESTEXA::QueryQpiFunctions_input",
        "TESTEXA::QueryQpiFunctions_output",
        // Simple numeric types
        "bool",
        "bit",
        "sint8",
        "uint8",
        "sint16",
        "uint16",
        "sint32",
        "uint32",
        "sint64",
        "uint64",
        "uint128",
        // BitArray convenience definitions
        "bit_2",
        "bit_4",
        "bit_8",
        "bit_16",
        "bit_32",
        "bit_64",
        "bit_128",
        "bit_256",
        "bit_512",
        "bit_1024",
        "bit_2048",
        "bit_4096",
        // Array convenience definitions
        "sint8_2",
        "sint8_4",
        "sint8_8",
        "uint8_2",
        "uint8_4",
        "uint8_8",
        "sint16_2",
        "sint16_4",
        "sint16_8",
        "uint16_2",
        "uint16_4",
        "uint16_8",
        "sint32_2",
        "sint32_4",
        "sint32_8",
        "uint32_2",
        "uint32_4",
        "uint32_8",
        "sint64_2",
        "sint64_4",
        "sint64_8",
        "uint64_2",
        "uint64_4",
        "uint64_8",
        "id_2",
        "id_4",
        "id_8",

        // BitArray<SIZE>
        // Array of allowed type...
    };
}
