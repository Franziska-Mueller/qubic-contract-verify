using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct StateData
    {
        auto dummy = sizeof...(somePack);
    };
};
