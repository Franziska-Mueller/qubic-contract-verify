using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    uint32 cast(uint64 v)
    {
        return (uint32) v;
    }
};
