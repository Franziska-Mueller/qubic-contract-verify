using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    int dummy = 42;
    auto addr = &dummy;
};