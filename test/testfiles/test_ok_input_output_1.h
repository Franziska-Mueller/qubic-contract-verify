using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct SomeFunction_input
    {
        struct Helper
        {
            uint8 num;
        };

        Helper h;
    };
};