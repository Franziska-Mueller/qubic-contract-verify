using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct SomeFunction_input
    {
        uint8 smallNum;
        uint64 bigNum;

        struct NotUsed
        {
            // Collections are not allowed as member in input/output struct but this struct is not used as a member
            Collection<sint8, 16> coll;
        };

        Array<uint32, 4> arrayOfMediumNums;
        BitArray<8> eightBits;
    };
};