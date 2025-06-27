using namespace QPI;

struct TESTCON : public ContractBase
{
private:
    typedef int MyInt;

public:
    PUBLIC_FUNCTION(GetFee)
    {
        typedef int WholeNumber;
    }
};