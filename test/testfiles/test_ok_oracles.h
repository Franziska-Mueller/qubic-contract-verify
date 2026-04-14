using namespace QPI;

struct TESTCON2
{
};

struct TESTCON : public ContractBase
{

    /*************************************************/
    /* ORACLE CONTRACT QUERY AND SUBSCRIPION TESTING */
    /*************************************************/

    struct QueryPriceOracle_input
    {
        OI::Price::OracleQuery priceOracleQuery;
        uint32 timeoutMilliseconds;
    };
    struct QueryPriceOracle_output
    {
        sint64 oracleQueryId;
    };

    PUBLIC_PROCEDURE(QueryPriceOracle)
    {
        if (qpi.invocationReward() < OI::Price::getQueryFee(input.priceOracleQuery))
        {
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }
        output.oracleQueryId = QUERY_ORACLE(OI::Price, input.priceOracleQuery, NotifyPriceOracleReply, input.timeoutMilliseconds);
        if (output.oracleQueryId < 0)
        {
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
        }
    }

    struct SubscribePriceOracle_input
    {
        OI::Price::OracleQuery priceOracleQuery;
        uint32 subscriptionPeriodMilliseconds;
        bit notifyPreviousValue;
    };
    struct SubscribePriceOracle_output
    {
        sint32 oracleSubscriptionId;
    };

    PUBLIC_PROCEDURE(SubscribePriceOracle)
    {
        if (qpi.invocationReward() < OI::Price::getSubscriptionFee(input.priceOracleQuery, input.subscriptionPeriodMilliseconds))
        {
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }
        output.oracleSubscriptionId = SUBSCRIBE_ORACLE(OI::Price, input.priceOracleQuery, NotifyPriceOracleReply, input.subscriptionPeriodMilliseconds, input.notifyPreviousValue);
        if (output.oracleSubscriptionId < 0)
        {
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
        }
    }

    struct UnsubscribeOracle_input
    {
        sint32 subscriptionId;
    };
    struct UnsubscribeOracle_output
    {
        bit success;
    };

    PUBLIC_PROCEDURE(UnsubscribeOracle)
    {
        output.success = qpi.unsubscribeOracle(input.subscriptionId);
    }

    typedef OracleNotificationInput <  OI::Price > NotifyPriceOracleReply_input;
    typedef NoData NotifyPriceOracleReply_output;
    struct NotifyPriceOracleReply_locals
    {
        Logger log;
    };

    PRIVATE_PROCEDURE_WITH_LOCALS(NotifyPriceOracleReply)
    {
        locals.log = Logger{
            CONTRACT_INDEX, OI::Price::oracleInterfaceIndex, qpi.invocator(),
            id(input.subscriptionId, input.queryId, input.reply.numerator, input.reply.denominator),
            input.status, OI::Price::replyIsValid(input.reply)};
        LOG_INFO(locals.log);
    }

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES()
    {
        REGISTER_USER_PROCEDURE(QueryPriceOracle, 100);
        REGISTER_USER_PROCEDURE(SubscribePriceOracle, 101);
        REGISTER_USER_PROCEDURE(UnsubscribeOracle, 102);

        REGISTER_USER_PROCEDURE_NOTIFICATION(NotifyPriceOracleReply);
    }
};
