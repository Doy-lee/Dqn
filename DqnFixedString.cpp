void DqnFixedString_Test()
{
    LOG_HEADER();
    {
        DqnFixedString<512> str  = DQN_SLICE("hello world");
        DQN_ASSERT(DqnStr_Cmp(str.str, "hello world") == 0);

        Log(Status::Ok, "Copy constructor DqnSlice<char>");
    }

    {
        DqnFixedString<512> zero = {};
        DqnFixedString<512> str  = DQN_SLICE("hello world");
        str = zero;
        DQN_ASSERT(str.len == 0 && str.str[0] == 0);

        DqnSlice<char const> helloSlice = DQN_SLICE("hello");
        str = helloSlice;
        DQN_ASSERT(DqnStr_Cmp(str.str, "hello") == 0);

        Log(Status::Ok, "Copy constructor (DqnFixedString<>)");
    }

    {
        DqnFixedString<512> str = DQN_SLICE("hello world");
        DQN_ASSERT(str.Sprintf("hello %s", "sailor"));
        DQN_ASSERTM(DqnStr_Cmp(str.str, "hello sailor") == 0, "Result: %s", str.str);

        Log(Status::Ok, "Sprintf");
    }

    {
        {
            DqnFixedString<512> str = DQN_SLICE("hello world");
            DQN_ASSERT(str.Sprintf("hello %s", "sailor"));
            str += DQN_SLICE(".end");
            DQN_ASSERT(DqnStr_Cmp(str.str, "hello sailor.end") == 0);
        }

        {
            DqnFixedString<512> str = DQN_SLICE("hello world");
            DQN_ASSERT(str.Sprintf("hello %s", "sailor"));
            DQN_ASSERT(str.SprintfAppend(" %d, %d", 100, 200));
            DQN_ASSERT(DqnStr_Cmp(str.str, "hello sailor 100, 200") == 0);
        }

        Log(Status::Ok, "Concatenation, operator +=, SprintfAppend");
    }

    {
        DqnFixedString<512> str;
        str = "hello big world";
        DQN_ASSERT(DqnStr_Cmp(str.str, "hello big world") == 0);
        str = DqnFixedString<512>("goodbye", DQN_CHAR_COUNT("goodbye"));
        DQN_ASSERT(DqnStr_Cmp(str.str, "goodbye") == 0);

        Log(Status::Ok, "Copy constructor (char const *str, int len)");
    }

    {
        DqnFixedString<512> str = DQN_SLICE("hello world");
        DQN_ASSERT(str.Sprintf("hello %s", "sailor"));
        str = str + " end" + DQN_SLICE(" of");
        DQN_ASSERT(DqnStr_Cmp(str.str, "hello sailor end of") == 0);

        Log(Status::Ok, "Operator +");
    }
}
