using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct StateData
    {
        int dummy = addOne(12);
    };
};
