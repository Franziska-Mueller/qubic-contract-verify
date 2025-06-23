using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    int dummyFunc(int x)
    {
        for (i = 0; i < 10; ++i)
            x++;
        return x;
    }
};