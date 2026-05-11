struct TESTCON : public ContractBase
{
    uint32 dummyFunc(uint32 x)
    {
        return someNamespace::addOne(x);
    }
};