using namespace QPI;

struct TESTCON : public ContractBase
{
public:

    typedef uint8* num8BitPtr;
    
    struct SomeFunction_input
    {
        num8BitPtr smallNum;
    };
};