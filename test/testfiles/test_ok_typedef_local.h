using namespace QPI;

struct TESTCON : public ContractBase
{
private:
    typedef uint32 MyInt;

public:
    PUBLIC_FUNCTION(GetFee)
    {
        typedef uint32 WholeNumber;
    }
};