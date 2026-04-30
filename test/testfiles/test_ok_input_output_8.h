using namespace QPI;

struct TESTCON : public ContractBase
{
public:

    struct SomeFunction_input
    {
        using num8BitHashSet = HashSet<sint8, 32>; // unrelated using declaration in input struct should not cause an error

        uint8 smallNum;
    };
};