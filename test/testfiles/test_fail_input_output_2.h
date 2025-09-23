using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct Helper
    {
        UnknownType t;
    };

    struct SomeFunction_input
    {
        Helper h;
    };
};