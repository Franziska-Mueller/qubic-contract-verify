using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    int dummyFunc(int x)
    {
        goto returnStatement;
        return x;

    returnStatement:
        return 42;
    }
};