using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct SomeFunction_output
    {
        // unnamed structs as members are currently not allowed to make verification easier
        struct
        {
            uint8 un8;
            sint64 sn64;
        } twoNumbers;
    };
};