using namespace QPI;

struct TESTCON : public ContractBase
{
public:

    using num8BitHashSet = HashSet<sint8, 32>;
    
    struct SomeFunction_input
    {
        num8BitHashSet set;
    };
};