using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    id dummy()
    {
        return id {1, 2, 3, 0};
    }
};
