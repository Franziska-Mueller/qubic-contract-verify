using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    int dummy = *someGlobalIntPointer;
};