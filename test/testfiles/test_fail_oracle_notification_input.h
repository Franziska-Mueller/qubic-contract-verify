using namespace QPI;

struct TESTCON2
{
};

struct TESTCON : public ContractBase
{
    typedef OracleNotificationInput<HashSet<id, 128>> NotifyPriceOracleReply_input;
    typedef NoData NotifyPriceOracleReply_output;
};
