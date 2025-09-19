using namespace QPI;

struct TESTCON : public ContractBase
{
public:

    struct Helper
    {
        typedef uint8 num8Bit;
    };
    
    struct SomeFunction_input
    {
        Helper::num8Bit smallNum;
    };
};