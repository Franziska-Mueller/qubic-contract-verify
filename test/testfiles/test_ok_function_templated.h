using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    template <typename T, uint32 someNumber>
    uint32 dummy()
    {
        return someNumber + 1;
    }
};