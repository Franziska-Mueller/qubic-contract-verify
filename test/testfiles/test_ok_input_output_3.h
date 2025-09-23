using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct SomeFunction_output
    {
        struct Helper
        {
            uint8 num;
        };
    };

    struct SomeOtherFunction_input
    {
        SomeFunction_output::Helper h;
    };
};