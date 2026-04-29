using namespace QPI;

struct TESTCON : public ContractBase
{
public:
    enum class EAuctionType : uint8
    {
        English = 0,
        Dutch = 1,
    };

    struct GetAuction_output
    {
        EAuctionType auctionType;
    };
};
