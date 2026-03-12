using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct StateData
    {
        int dummy = 8483%7;
    };
};
