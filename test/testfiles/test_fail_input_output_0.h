using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct SomeFunction_output
    {
        HashSet<sint8, 32> set;
    };
};