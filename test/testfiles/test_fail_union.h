using namespace QPI;

union forbiddenUnion
{
    int a;
    int b;
};

struct TESTCON : public ContractBase {};