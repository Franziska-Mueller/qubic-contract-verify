using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    uint32 dummyFunc(uint32 x)
    {
        for (i = 0; i < 10; ++i)
            x++;
        return x;
    }
};