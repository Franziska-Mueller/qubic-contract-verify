using namespace QPI;

struct TESTCON : public ContractBase
{
public:

    typedef uint8 num8Bit;
        
    struct SomeFunction_input
    {
        num8Bit smallNum;
    };

    struct Nesting
    {
        struct SomeOtherFunction_input
        {
            num8Bit otherSmallNum;
        };
    };
};