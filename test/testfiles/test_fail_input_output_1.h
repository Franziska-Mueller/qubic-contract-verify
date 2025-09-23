using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct SomeFunction_input
    {
        uint8 smallNum;
        uint64 bigNum;

        struct NotAllowed
        {
            // Collections are not allowed as member in input/output struct, also not transitively
            Collection<sint8, 16> coll;
        };

        NotAllowed n;

        Array<uint32, 4> arrayOfMediumNums;
        BitArray<8> eightBits;
    };
};