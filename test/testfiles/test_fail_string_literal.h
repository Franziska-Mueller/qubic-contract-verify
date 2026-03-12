using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct StateData
    {
        auto forbiddenString = "I am a string literal";
    };
};
