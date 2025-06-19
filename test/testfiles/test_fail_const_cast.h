using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    int& dummy = const_cast<int&>(someConstIntVar);
};