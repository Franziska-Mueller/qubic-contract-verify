using namespace QPI;

struct TESTCON : public ContractBase
{
public:

    typedef uint8 num8BitArray[10];
    
    struct SomeFunction_input
    {
        num8BitArray smallNum;
    };
};