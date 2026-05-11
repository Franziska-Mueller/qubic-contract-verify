using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    uint32 dummyFunc(uint32 x)
    {
        return ([](uint32 i) -> uint32 { return i + 1; })(x);
    }
};