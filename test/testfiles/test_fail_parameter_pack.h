using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    template<class... Ts>
    uint32 dummyFunction(Ts...)
    {
        return 0;
    }
};
