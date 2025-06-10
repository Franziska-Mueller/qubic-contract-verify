using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    template<class... Ts>
    int dummyFunction(Ts...)
    {
        return 0;
    }
};
