using namespace QPI;

struct TESTCON : public ContractBase
{
    void dummyFunc()
    {
        throw MyException();
    }
};