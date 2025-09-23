using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct SomeFunction_output
    {
        struct Helper
        {
            UnknownType t;
        };
    };

    struct SomeOtherFunction_input
    {
        SomeFunction_output::Helper h;
    };
};