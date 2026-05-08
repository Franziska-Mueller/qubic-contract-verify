using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct StateData
    {
        uint32& dummy = const_cast<uint32&>(someConstIntVar);
    };
};
