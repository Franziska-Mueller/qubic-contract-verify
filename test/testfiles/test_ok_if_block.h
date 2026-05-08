using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    uint32 dummyFunc(uint32 x)
    {
        if (x > 0)
            return 1;
        else
            return -1;
    }
};