struct Bar
{
    static sint64 doSomething1()
    {
        sint64 allowed0 = 0;
        uint64 allowed1 = 1;
        sint32 allowed2 = 2;
        uint32 allowed3 = 3;
        return allowed0 + allowed1 + allowed2 + allowed3;
    }

    static sint64 doSomething2()
    {
        sint64 allowed0 = 0;
        uint64 allowed1 = 1;
        sint32 allowed2 = 2;
        uint32 allowed3 = 3;
        sint16 allowed4 = 4;
        uint16 allowed5 = 5;
        sint8 allowed6 = 6;
        uint8 allowed7 = 7;
        bool allowed8 = true;
        bit allowed9 = false;
        sint64 var11;
        return allowed0 + allowed1 + allowed2 + allowed3 + allowed4 + allowed5 + allowed6 + allowed7 + allowed8 + allowed9 + var11;
    }
};
