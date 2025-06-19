using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    void dummyFunc()
    {
        someGlobalVar /= 987;
    };
};