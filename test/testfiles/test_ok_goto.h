using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    uint32 dummyFunc(uint32 x)
    {
        goto returnStatement;
        return x;

    returnStatement:
        return 42;
    }
};