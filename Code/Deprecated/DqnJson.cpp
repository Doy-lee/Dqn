void DqnJson_Test()
{
    LOG_HEADER();
    char const json[] =
    R"FOO(
    {
        "result": {
            "cumulative_difficulty": 282912831023,
            "difficulty": 18293,
            "name": "Block",
            "array_of_objects": [{
                "hash": "83abdc3f",
                "time": 102981029381,
            }, {
                "hash": "12acf73d",
                "time": 123761239789,
            }],
            "time": 3498573485,
            "embed_object": {
                "proof": "axcbde",
                "signature": "l9382kjabmznmx129aslzejs"
            }
            "bits": [1, 0, 1, 1, 0, 1, 0],
            "hex": ["AF", "BE", "0C", "FF"],
            "extra": [123],
            "serialise": [],
        },
    }
    )FOO";

    DqnJson result           = DqnJson_Get(DqnSlice<const char>(json, DQN_ARRAY_COUNT(json)), DQN_BUFFER_STR_LIT("result"));
    DqnJson cum_difficulty   = DqnJson_Get(result, DQN_BUFFER_STR_LIT("cumulative_difficulty"));
    DqnJson difficulty       = DqnJson_Get(result, DQN_BUFFER_STR_LIT("difficulty"));
    DqnJson name             = DqnJson_Get(result, DQN_BUFFER_STR_LIT("name"));
    DqnJson array_of_objects = DqnJson_Get(result, DQN_BUFFER_STR_LIT("array_of_objects"));
    DqnJson time             = DqnJson_Get(result, DQN_BUFFER_STR_LIT("time"));
    DqnJson embed_object     = DqnJson_Get(result, DQN_BUFFER_STR_LIT("embed_object"));
    DqnJson bits             = DqnJson_Get(result, DQN_BUFFER_STR_LIT("bits"));
    DqnJson hex              = DqnJson_Get(result, DQN_BUFFER_STR_LIT("hex"));
    DqnJson extra            = DqnJson_Get(result, DQN_BUFFER_STR_LIT("extra"));
    DqnJson serialise        = DqnJson_Get(result, DQN_BUFFER_STR_LIT("serialise"));

    DQN_ASSERT(DQN_BUFFER_STRCMP(cum_difficulty.value, DQN_BUFFER_STR_LIT("282912831023"), Dqn::IgnoreCase::No));
    DQN_ASSERT(DQN_BUFFER_STRCMP(difficulty.value, DQN_BUFFER_STR_LIT("18293"), Dqn::IgnoreCase::No));
    DQN_ASSERT(DQN_BUFFER_STRCMP(name.value, DQN_BUFFER_STR_LIT("\"Block\""), Dqn::IgnoreCase::No));

    {
        DQN_ASSERT(array_of_objects.IsArray() && array_of_objects.num_entries == 2);
        isize count = 0;
        while(DqnJson it = DqnJson_GetNextArrayItem(&array_of_objects))
        {
            DqnJson hash = DqnJson_Get(it, DQN_BUFFER_STR_LIT("hash"));
            DqnJson time2 = DqnJson_Get(it, DQN_BUFFER_STR_LIT("time"));
            if (count == 0)
            {
                DQN_ASSERT(DQN_BUFFER_STRCMP(hash.value, DQN_BUFFER_STR_LIT("\"83abdc3f\""), Dqn::IgnoreCase::No));
                DQN_ASSERT(DQN_BUFFER_STRCMP(time2.value, DQN_BUFFER_STR_LIT("102981029381"), Dqn::IgnoreCase::No));
            }
            else
            {
                DQN_ASSERT(DQN_BUFFER_STRCMP(hash.value, DQN_BUFFER_STR_LIT("\"12acf73d\""), Dqn::IgnoreCase::No));
                DQN_ASSERT(DQN_BUFFER_STRCMP(time2.value, DQN_BUFFER_STR_LIT("123761239789"), Dqn::IgnoreCase::No));
            }
            ++count;
        }

    }

    {
        DqnJson proof     = DqnJson_Get(embed_object, DQN_BUFFER_STR_LIT("proof"));
        DqnJson signature = DqnJson_Get(embed_object, DQN_BUFFER_STR_LIT("signature"));
        DQN_ASSERT(DQN_BUFFER_STRCMP(proof.value, DQN_BUFFER_STR_LIT("\"axcbde\""), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_BUFFER_STRCMP(signature.value, DQN_BUFFER_STR_LIT("\"l9382kjabmznmx129aslzejs\""), Dqn::IgnoreCase::No));
    }

    DQN_ASSERT(DQN_BUFFER_STRCMP(time.value, DQN_BUFFER_STR_LIT("3498573485"), Dqn::IgnoreCase::No));

    {
        DQN_ASSERT(bits.IsArray() && bits.num_entries == 7);
        DqnJson bits_array[7];
        isize bitsIndex = 0;

        while(DqnJson it = DqnJson_GetNextArrayItem(&bits))
            bits_array[bitsIndex++] = it;

        DQN_ASSERT(bitsIndex == DQN_ARRAY_COUNT(bits_array));
        DQN_ASSERT(DQN_BUFFER_STRCMP(bits_array[0].value, DQN_BUFFER_STR_LIT("1"), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_BUFFER_STRCMP(bits_array[1].value, DQN_BUFFER_STR_LIT("0"), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_BUFFER_STRCMP(bits_array[2].value, DQN_BUFFER_STR_LIT("1"), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_BUFFER_STRCMP(bits_array[3].value, DQN_BUFFER_STR_LIT("1"), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_BUFFER_STRCMP(bits_array[4].value, DQN_BUFFER_STR_LIT("0"), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_BUFFER_STRCMP(bits_array[5].value, DQN_BUFFER_STR_LIT("1"), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_BUFFER_STRCMP(bits_array[6].value, DQN_BUFFER_STR_LIT("0"), Dqn::IgnoreCase::No));
    }

    {
        DQN_ASSERT(hex.IsArray() && hex.num_entries == 4);
        DqnJson hex_array[4];
        isize hex_index = 0;

        while(DqnJson it = DqnJson_GetNextArrayItem(&hex))
            hex_array[hex_index++] = it;

        DQN_ASSERT(hex_index == DQN_ARRAY_COUNT(hex_array));
        DQN_ASSERT(DQN_BUFFER_STRCMP(hex_array[0].value, DQN_BUFFER_STR_LIT("\"AF\""), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_BUFFER_STRCMP(hex_array[1].value, DQN_BUFFER_STR_LIT("\"BE\""), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_BUFFER_STRCMP(hex_array[2].value, DQN_BUFFER_STR_LIT("\"0C\""), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_BUFFER_STRCMP(hex_array[3].value, DQN_BUFFER_STR_LIT("\"FF\""), Dqn::IgnoreCase::No));
    }

    {
        DQN_ASSERT(extra.IsArray() && extra.num_entries == 1);
        while(DqnJson it = DqnJson_GetNextArrayItem(&extra))
        {
            DQN_ASSERT(DQN_BUFFER_STRCMP(it.value, DQN_BUFFER_STR_LIT("123"), Dqn::IgnoreCase::No));
        }
    }
    Log(Status::Ok, "DqnJson sample structure parsed");
}
