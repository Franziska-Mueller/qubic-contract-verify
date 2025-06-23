using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    PUBLIC_FUNCTION(GetFee)
    {
        int fee = 123;
    }
};