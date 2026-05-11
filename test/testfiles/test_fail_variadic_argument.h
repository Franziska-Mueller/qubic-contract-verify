using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    uint32 dummyFunction(uint32 i...)
    {
        return 0;
    }
};
