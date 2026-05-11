using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    PUBLIC_FUNCTION(GetFee)
    {
        uint32 fee = 123;
    }
};