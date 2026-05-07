using namespace QPI;

struct TESTCON : public ContractBase
{
public:

    struct DataX
    {
        sint64 foo;
        __USER_FUNCTION bar;
    };

    struct SomeFunction_input
    {
        SlowAnySizeArray<DataX, 676> computorSomething;
    };
};