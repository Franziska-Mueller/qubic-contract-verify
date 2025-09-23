using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct Helper
    {
        uint8 num;
    };

    struct SomeFunction_input
    {
        Helper h;
    };
};