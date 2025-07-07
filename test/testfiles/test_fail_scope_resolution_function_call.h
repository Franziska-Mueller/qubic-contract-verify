struct TESTCON : public ContractBase
{
    int dummyFunc(int x)
    {
        return someNamespace::addOne(x);
    }
};