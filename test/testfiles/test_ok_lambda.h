using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    int dummyFunc(int x)
    {
        return ([](int i) -> int { return i + 1; })(x);
    }
};