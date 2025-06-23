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
        TEMPL_SPEC = 4,  // this is needed to distinguish variables in template specs from global variables
    };

    // helper struct for visiting variants
    // refer to https://en.cppreference.com/w/cpp/utility/variant/visit2 for details
    template <class... Ts>
    struct Overloaded : Ts... { using Ts::operator()...; };
}