using namespace QPI;

struct TESTCON : public ContractBase
{
public:

    using SomeFunction_output = struct { HashSet<sint8, 32> myHashSet; };

};