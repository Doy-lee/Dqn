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

    DqnJson result               = DqnJson_Get(DqnSlice<const char>(json, DQN_ARRAY_COUNT(json)), DQN_SLICE("result"));
    DqnJson cumulativeDifficulty = DqnJson_Get(result, DQN_SLICE("cumulative_difficulty"));
    DqnJson difficulty           = DqnJson_Get(result, DQN_SLICE("difficulty"));
    DqnJson name                 = DqnJson_Get(result, DQN_SLICE("name"));
    DqnJson arrayOfObjects       = DqnJson_Get(result, DQN_SLICE("array_of_objects"));
    DqnJson time                 = DqnJson_Get(result, DQN_SLICE("time"));
    DqnJson embedObject          = DqnJson_Get(result, DQN_SLICE("embed_object"));
    DqnJson bits                 = DqnJson_Get(result, DQN_SLICE("bits"));
    DqnJson hex                  = DqnJson_Get(result, DQN_SLICE("hex"));
    DqnJson extra                = DqnJson_Get(result, DQN_SLICE("extra"));
    DqnJson serialise            = DqnJson_Get(result, DQN_SLICE("serialise"));

    DQN_ASSERT(DQN_SLICE_CMP(cumulativeDifficulty.value, DQN_SLICE("282912831023"), Dqn::IgnoreCase::No));
    DQN_ASSERT(DQN_SLICE_CMP(difficulty.value, DQN_SLICE("18293"), Dqn::IgnoreCase::No));
    DQN_ASSERT(DQN_SLICE_CMP(name.value, DQN_SLICE("\"Block\""), Dqn::IgnoreCase::No));

    {
        DQN_ASSERT(arrayOfObjects.IsArray() && arrayOfObjects.numEntries == 2);
        isize count = 0;
        while(DqnJson it = DqnJson_GetNextArrayItem(&arrayOfObjects))
        {
            DqnJson hash = DqnJson_Get(it, DQN_SLICE("hash"));
            DqnJson time2 = DqnJson_Get(it, DQN_SLICE("time"));
            if (count == 0)
            {
                DQN_ASSERT(DQN_SLICE_CMP(hash.value, DQN_SLICE("\"83abdc3f\""), Dqn::IgnoreCase::No));
                DQN_ASSERT(DQN_SLICE_CMP(time2.value, DQN_SLICE("102981029381"), Dqn::IgnoreCase::No));
            }
            else
            {
                DQN_ASSERT(DQN_SLICE_CMP(hash.value, DQN_SLICE("\"12acf73d\""), Dqn::IgnoreCase::No));
                DQN_ASSERT(DQN_SLICE_CMP(time2.value, DQN_SLICE("123761239789"), Dqn::IgnoreCase::No));
            }
            ++count;
        }

    }

    {
        DqnJson proof     = DqnJson_Get(embedObject, DQN_SLICE("proof"));
        DqnJson signature = DqnJson_Get(embedObject, DQN_SLICE("signature"));
        DQN_ASSERT(DQN_SLICE_CMP(proof.value, DQN_SLICE("\"axcbde\""), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_SLICE_CMP(signature.value, DQN_SLICE("\"l9382kjabmznmx129aslzejs\""), Dqn::IgnoreCase::No));
    }

    DQN_ASSERT(DQN_SLICE_CMP(time.value, DQN_SLICE("3498573485"), Dqn::IgnoreCase::No));

    {
        DQN_ASSERT(bits.IsArray() && bits.numEntries == 7);
        DqnJson bitsArray[7];
        isize bitsIndex = 0;

        while(DqnJson it = DqnJson_GetNextArrayItem(&bits))
            bitsArray[bitsIndex++] = it;

        DQN_ASSERT(bitsIndex == DQN_ARRAY_COUNT(bitsArray));
        DQN_ASSERT(DQN_SLICE_CMP(bitsArray[0].value, DQN_SLICE("1"), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_SLICE_CMP(bitsArray[1].value, DQN_SLICE("0"), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_SLICE_CMP(bitsArray[2].value, DQN_SLICE("1"), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_SLICE_CMP(bitsArray[3].value, DQN_SLICE("1"), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_SLICE_CMP(bitsArray[4].value, DQN_SLICE("0"), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_SLICE_CMP(bitsArray[5].value, DQN_SLICE("1"), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_SLICE_CMP(bitsArray[6].value, DQN_SLICE("0"), Dqn::IgnoreCase::No));
    }

    {
        DQN_ASSERT(hex.IsArray() && hex.numEntries == 4);
        DqnJson hexArray[4];
        isize hexIndex = 0;

        while(DqnJson it = DqnJson_GetNextArrayItem(&hex))
            hexArray[hexIndex++] = it;

        DQN_ASSERT(hexIndex == DQN_ARRAY_COUNT(hexArray));
        DQN_ASSERT(DQN_SLICE_CMP(hexArray[0].value, DQN_SLICE("\"AF\""), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_SLICE_CMP(hexArray[1].value, DQN_SLICE("\"BE\""), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_SLICE_CMP(hexArray[2].value, DQN_SLICE("\"0C\""), Dqn::IgnoreCase::No));
        DQN_ASSERT(DQN_SLICE_CMP(hexArray[3].value, DQN_SLICE("\"FF\""), Dqn::IgnoreCase::No));
    }

    {
        DQN_ASSERT(extra.IsArray() && extra.numEntries == 1);
        while(DqnJson it = DqnJson_GetNextArrayItem(&extra))
        {
            DQN_ASSERT(DQN_SLICE_CMP(it.value, DQN_SLICE("123"), Dqn::IgnoreCase::No));
        }
    }
    Log(Status::Ok, "DqnJson sample structure parsed");
}
