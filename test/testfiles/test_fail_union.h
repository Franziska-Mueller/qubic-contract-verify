using namespace QPI;

union forbiddenUnion
{
    uint32 a;
    uint32 b;
};

struct TESTCON : public ContractBase {};