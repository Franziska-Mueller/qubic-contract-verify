struct TESTCON : public ContractBase
{
    struct StateData
    {
        X<QPI::MyInt, NotAllowed::Y> i;
    };
};
