using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    uint32 dummyFunc(uint32 x)
    {
        while (x > 10)
            x--;
        return x;
    }
};