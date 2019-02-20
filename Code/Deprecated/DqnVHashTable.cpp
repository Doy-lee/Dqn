void DqnVHashTable_Test()
{
    LOG_HEADER();
    struct Block
    {
        int x;
    };
    using Height = u32;

    {
        Block block = {};

        DqnVHashTable<Height, Block> table = {};
        DQN_DEFER { table.Free(); };
        table.Set(12, block);

        Block *getResult      = table.Get(12);
        Block *operatorResult = table[12];
        DQN_ASSERT(operatorResult == getResult);
        DQN_ASSERT(operatorResult != nullptr);
        Log(Status::Ok, "Set and get element using key");

        table.Erase(12);
        getResult      = table.Get(12);
        operatorResult = table[12];

        DQN_ASSERT(operatorResult == getResult);
        DQN_ASSERT(operatorResult == nullptr);
        Log(Status::Ok, "Erase element using key");
    }

    {
        Block blocks[] = {{0}, {1}, {2}, {3}, {4}};
        DqnVHashTable<Height, Block> table = {};
        DQN_DEFER { table.Free(); };

        table.Set(1, blocks[0]);
        table.Set(2, blocks[1]);
        table.Set(3, blocks[2]);
        table.Set(4, blocks[3]);
        table.Set(5, blocks[4]);

        {
            bool blockSeen[DQN_ARRAY_COUNT(blocks)] = {};
            isize blocksSeen = 0;
            for (auto const &entry : table)
            {
                Block const *block = &entry.item;
                DQN_ASSERT(blockSeen[block->x] == false);
                blockSeen[block->x] = true;
                blocksSeen++;
            }
            DQN_ASSERT(blocksSeen == DQN_ARRAY_COUNT(blockSeen));
            Log(Status::Ok, "Auto iterator using prefix operator++");
        }

        {
            bool blockSeen[DQN_ARRAY_COUNT(blocks)] = {};
            isize blocksSeen = 0;
            for (auto it = table.begin(); it != table.end();)
            {
                Block *block = &it.entry->item;

                DQN_ASSERT(blockSeen[block->x] == false);
                blockSeen[block->x] = true;
                blocksSeen++;
                it = it + 1;
            }
            DQN_ASSERT(blocksSeen == DQN_ARRAY_COUNT(blockSeen));
            Log(Status::Ok, "Auto iterator using operator+");
        }

        {
            bool blockSeen[DQN_ARRAY_COUNT(blocks)] = {};
            isize blocksSeen = 0;
            for (auto it = table.begin(); it != table.end(); it++)
            {
                Block *block = &it.entry->item;

                DQN_ASSERT(blockSeen[block->x] == false);
                blockSeen[block->x] = true;
                blocksSeen++;
            }
            DQN_ASSERT(blocksSeen == DQN_ARRAY_COUNT(blockSeen));
            Log(Status::Ok, "Auto iterator using postfix operator++");
        }
    }
}
