using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    int dummyFunc(int x)
    {
        do
            x--;
        while (x > 10);

        return x;
    }
};