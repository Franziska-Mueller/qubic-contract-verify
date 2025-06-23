using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    int dummyFunc(int x)
    {
        while (x > 10)
            x--;
        return x;
    }
};