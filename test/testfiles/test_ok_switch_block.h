using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    bool dummyFunc(int x)
    {
        switch (x)
        {
        case 0:
            return false;
        default:
            return true;
        }
    }
};