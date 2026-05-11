using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct StateData
    {
        uint32 dummy = __restrictedFunction(43);
    };
};
