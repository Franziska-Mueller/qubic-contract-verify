struct Foo
{
    struct OracleQuery
    {
        uint64 value;
    };


    static bool doSomething(const OracleQuery& query, uint32 notifyPeriodInMilliseconds)
    {
        HashMap<sint64, sint64, 1024> forbidden;
        sint64 allowed;
        return forbidden.get(0, allowed);
    }
};
