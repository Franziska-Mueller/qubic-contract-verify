using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    template <typename T, int someNumber>
    int dummy()
    {
        return someNumber + 1;
    }
};