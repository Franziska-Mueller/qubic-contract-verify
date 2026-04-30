using namespace QPI;

struct TESTCON : public ContractBase
{
public:

    using num8Bit = uint8;

    struct SomeFunction_input
    {
        num8Bit smallNum;
    };
};