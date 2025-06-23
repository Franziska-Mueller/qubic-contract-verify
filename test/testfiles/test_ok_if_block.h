using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    int dummyFunc(int x)
    {
        if (x > 0)
            return 1;
        else
            return -1;
    }
};