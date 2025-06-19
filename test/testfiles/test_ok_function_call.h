using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    int dummy = addOne(12);
};