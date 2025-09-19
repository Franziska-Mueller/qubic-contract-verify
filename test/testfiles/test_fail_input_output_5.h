using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    struct SomeFunction_output
    {
        // variable templates are not allowed
        template<class T>
        constexpr T pi = T(3.1415926535897932385L);
    };
};