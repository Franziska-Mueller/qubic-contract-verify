struct TESTCON : public ContractBase
{
    enum TestType
    {
        OK,
        FAIL,
    };

    TestType getErrorType()
    {
        return TestType::FAIL;
    }
};