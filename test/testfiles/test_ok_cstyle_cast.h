using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct StateData
    {
        int dummy = (int) someVar;
    };
};
