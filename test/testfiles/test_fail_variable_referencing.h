using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    auto func(uint32 dummy = 42)
    {
        return &dummy;
    }
};
