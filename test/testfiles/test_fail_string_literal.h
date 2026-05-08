using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    void func()
    {
        otherFunc("I am a string literal");
    }
};
