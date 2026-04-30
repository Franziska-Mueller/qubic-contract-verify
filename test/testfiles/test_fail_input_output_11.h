using namespace QPI;

struct TESTCON : public ContractBase
{
public:

    typedef struct
    {
        HashSet<sint8, 32> myHashSet;
    } SomeFunction_input;
};