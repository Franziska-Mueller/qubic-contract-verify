using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct SomeFunction_output
    {
        SlowAnySizeArray<sint8, 676> computorSomething;
    };
};