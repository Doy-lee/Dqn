void DqnVHashTable_Test()
{
    LOG_HEADER();
    struct Block
    {
        int x;
    };
    using Height = u32;
    Block block  = {};

    DqnVHashTable<Height, Block> table = {};
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
