FILE_SCOPE void DqnMemStack_Test()
{
    LOG_HEADER();

    // Check Alignment
    if (1)
    {
        auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::BoundsGuard);

        i32 const ALIGN64            = 64;
        i32 const ALIGN16            = 16;
        i32 const ALIGN4             = 4;
        DqnMemStack::AllocTo allocTo = DqnMemStack::AllocTo::Head;
        if (1)
        {
            u8 *result1 = (u8 *)stack.Push(2, allocTo, ALIGN4);
            u8 *result2 = (u8 *)DQN_ALIGN_POW_N(result1, ALIGN4);
            DQN_ASSERT(result1 == result2);
            stack.Pop(result1);
            DQN_ASSERT(stack.block->head == stack.block->memory);
        }

        if (1)
        {
            u8 *result1 = (u8 *)stack.Push(120, allocTo, ALIGN16);
            u8 *result2 = (u8 *)DQN_ALIGN_POW_N(result1, ALIGN16);
            DQN_ASSERT(result1 == result2);
            stack.Pop(result1);
            DQN_ASSERT(stack.block->head == stack.block->memory);
        }

        if (1)
        {
            u8 *result1 = (u8 *)stack.Push(12, allocTo, ALIGN64);
            u8 *result2 = (u8 *)DQN_ALIGN_POW_N(result1, ALIGN64);
            DQN_ASSERT(result1 == result2);
            stack.Pop(result1);
            DQN_ASSERT(stack.block->head == stack.block->memory);
        }

        stack.Free();
        Log(Status::Ok, "Check allocated alignment to 4, 16, 64");
    }

    // Check Non-Expandable
    if (1)
    {
        auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::NonExpandable);
        auto *result1 = stack.Push(DQN_MEGABYTE(2));
        DQN_ASSERT(result1 == nullptr);
        DQN_ASSERT(stack.block->prevBlock == nullptr);

        stack.Free();
        Log(Status::Ok, "Check non-expandable flag prevents expansion.");
    }

    // Check Expansion
    if (1)
    {
        auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes);
        DQN_ASSERT(stack.tracker.boundsGuardSize == 0);

        auto *oldBlock = stack.block;
        DQN_ASSERT(oldBlock);
        DQN_ASSERT(oldBlock->size == DQN_MEGABYTE(1));
        DQN_ASSERT(oldBlock->head == oldBlock->head);
        DQN_ASSERT(oldBlock->tail == oldBlock->tail);
        DQN_ASSERT(oldBlock->prevBlock == nullptr);

        auto *result1 = stack.Push(DQN_MEGABYTE(2));
        DQN_ASSERT(result1);
        DQN_ASSERT(stack.block->prevBlock == oldBlock);
        DQN_ASSERT(stack.block != oldBlock);

        Log(Status::Ok, "Check memory stack allocates additional memory blocks.");
        stack.Free();
    }

    // Temporary Regions
    if (1)
    {
        // Check temporary regions
        if (1)
        {
            auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::BoundsGuard);

            DqnMemStack::Block *blockToReturnTo = stack.block;
            auto headBefore                     = blockToReturnTo->head;
            auto tailBefore                     = blockToReturnTo->tail;
            if (1)
            {
                auto memGuard1 = stack.TempRegionGuard();
                auto *result2  = stack.Push(100);
                auto *result3  = stack.Push(100);
                auto *result4  = stack.Push(100);
                DQN_ASSERT(result2 && result3 && result4);
                DQN_ASSERT(stack.block->head != headBefore);
                DQN_ASSERT(stack.block->tail == tailBefore);
                DQN_ASSERT(stack.block->memory == blockToReturnTo->memory);

                // Force allocation of new block
                auto *result5 = stack.Push(DQN_MEGABYTE(5));
                DQN_ASSERT(result5);
                DQN_ASSERT(stack.block != blockToReturnTo);
                DQN_ASSERT(stack.tempRegionCount == 1);
            }

            DQN_ASSERT(stack.block == blockToReturnTo);
            DQN_ASSERT(stack.block->head == headBefore);
            DQN_ASSERT(stack.block->tail == tailBefore);

            stack.Free();
        }

        // Check temporary regions keep state
        if (1)
        {
            auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::BoundsGuard);
            DqnMemStack::Block *blockToReturnTo = stack.block;
            auto headBefore                     = blockToReturnTo->head;
            auto tailBefore                     = blockToReturnTo->tail;
            if (1)
            {
                auto memGuard1 = stack.TempRegionGuard();
                auto *result2  = stack.Push(100);
                auto *result3  = stack.Push(100);
                auto *result4  = stack.Push(100);
                DQN_ASSERT(result2 && result3 && result4);
                DQN_ASSERT(stack.block->head != headBefore);
                DQN_ASSERT(stack.block->tail == tailBefore);
                DQN_ASSERT(stack.block->memory == blockToReturnTo->memory);

                // Force allocation of new block
                auto *result5 = stack.Push(DQN_MEGABYTE(5));
                DQN_ASSERT(result5);
                DQN_ASSERT(stack.block != blockToReturnTo);
                DQN_ASSERT(stack.tempRegionCount == 1);
                memGuard1.region.keepHeadChanges = true;
            }

            DQN_ASSERT(stack.block != blockToReturnTo);
            DQN_ASSERT(stack.block->prevBlock == blockToReturnTo);
            DQN_ASSERT(stack.tempRegionCount == 0);

            stack.Free();
        }

        // Check temporary regions with tail and head pushes
        if (1)
        {
            auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::BoundsGuard);

            auto *pop1 = stack.Push(222);
            auto *pop2 = stack.Push(333, DqnMemStack::AllocTo::Tail);

            DqnMemStack::Block *blockToReturnTo = stack.block;
            auto headBefore = blockToReturnTo->head;
            auto tailBefore = blockToReturnTo->tail;
            if (1)
            {
                auto memGuard1 = stack.TempRegionGuard();
                auto *result2  = stack.Push(100);
                auto *result3  = stack.Push(100, DqnMemStack::AllocTo::Tail);
                auto *result4  = stack.Push(100);
                auto *result5  = stack.Push(100, DqnMemStack::AllocTo::Tail);
                DQN_ASSERT(result2 && result3 && result4 && result5);
                DQN_ASSERT(result3 > result5);
                DQN_ASSERT(result2 < result4);
                DQN_ASSERT(stack.block->head > headBefore && stack.block->head < stack.block->tail);
                DQN_ASSERT(stack.block->tail >= stack.block->head && stack.block->tail < (stack.block->memory + stack.block->size));
                DQN_ASSERT(stack.block->memory == blockToReturnTo->memory);

                // Force allocation of new block
                auto *result6 = stack.Push(DQN_MEGABYTE(5));
                DQN_ASSERT(result6);
                DQN_ASSERT(stack.block != blockToReturnTo);
                DQN_ASSERT(stack.tempRegionCount == 1);
            }

            DQN_ASSERT(stack.block == blockToReturnTo);
            DQN_ASSERT(stack.block->head == headBefore);
            DQN_ASSERT(stack.block->tail == tailBefore);

            stack.Pop(pop1);
            stack.Pop(pop2);
            DQN_ASSERT(stack.block->head == stack.block->memory);
            DQN_ASSERT(stack.block->tail == stack.block->memory + stack.block->size);

            stack.Free();
        }
        Log(Status::Ok, "Temporary regions return state and/or keep changes if requested.");
    }

    // Check Fixed Mem Init
    if (1)
    {
        // Check success
        if (1)
        {
            isize const bufSize = sizeof(DqnMemStack::Block) * 5;
            char buf[bufSize]   = {};
            auto stack          = DqnMemStack(&buf, bufSize, Dqn::ZeroClear::No);

            DQN_ASSERT(stack.block);
            DQN_ASSERT(stack.block->prevBlock == false);
            DQN_ASSERT(stack.tempRegionCount == 0);
            DQN_ASSERT(stack.flags == DqnMemStack::Flag::NonExpandable);

            auto *result1 = stack.Push(32);
            DQN_ASSERT(result1);
            stack.Pop(result1);

            auto *result2 = stack.Push(bufSize * 2);
            DQN_ASSERT(result2 == nullptr);
            DQN_ASSERT(stack.block);
            DQN_ASSERT(stack.block->prevBlock == false);
            DQN_ASSERT(stack.tempRegionCount == 0);
            DQN_ASSERT(stack.flags == DqnMemStack::Flag::NonExpandable);

            stack.Free();
        }

        Log(Status::Ok, "Checked fixed mem initialisation");
    }

    // Check Freeing Blocks
    if (1)
    {
        usize size           = 32;
        usize additionalSize = DqnMemStack::MINIMUM_BLOCK_SIZE;
        DqnMemAPI heap = DqnMemAPI::HeapAllocator();

        auto stack   = DqnMemStack(size, Dqn::ZeroClear::Yes, 0, &heap);
        auto *block1 = stack.block;

        size += additionalSize;
        auto *result1 = stack.Push(size);
        auto *block2  = stack.block;

        size += additionalSize;
        auto *result2 = stack.Push(size);
        auto *block3  = stack.block;

        size += additionalSize;
        auto *result3 = stack.Push(size);
        auto *block4  = stack.block;

        size += additionalSize;
        auto *result4 = stack.Push(size);
        auto *block5  = stack.block;

        DQN_ASSERT(result1 && result2 && result3 && result4);
        DQN_ASSERT(block1 && block2 && block3 && block4 && block5);
        DQN_ASSERT(block5->prevBlock == block4);
        DQN_ASSERT(block4->prevBlock == block3);
        DQN_ASSERT(block3->prevBlock == block2);
        DQN_ASSERT(block2->prevBlock == block1);
        DQN_ASSERT(block1->prevBlock == nullptr);

        DQN_ASSERT(stack.FreeMemBlock(block4));
        DQN_ASSERT(stack.block == block5);
        DQN_ASSERT(block5->prevBlock == block3);
        DQN_ASSERT(block3->prevBlock == block2);
        DQN_ASSERT(block2->prevBlock == block1);
        DQN_ASSERT(block1->prevBlock == nullptr);

        DQN_ASSERT(stack.FreeMemBlock(block5));
        DQN_ASSERT(stack.block == block3);
        DQN_ASSERT(block3->prevBlock == block2);
        DQN_ASSERT(block2->prevBlock == block1);
        DQN_ASSERT(block1->prevBlock == nullptr);

        stack.Free();
        DQN_ASSERT(stack.memAPI->bytesAllocated == 0);
        DQN_ASSERT(stack.block == nullptr);
        Log(Status::Ok, "Check freeing arbitrary blocks and freeing");
    }

    // Check bounds guard places magic values
    if (1)
    {
        auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::BoundsGuard);
        char *result = static_cast<char *>(stack.Push(64));

        // TODO(doyle): check head and tail are adjacent to the bounds of the allocation
        u32 *head = stack.tracker.PtrToHeadGuard(result);
        u32 *tail = stack.tracker.PtrToTailGuard(result);
        DQN_ASSERT(*head == DqnMemTracker::HEAD_GUARD_VALUE);
        DQN_ASSERT(*tail == DqnMemTracker::TAIL_GUARD_VALUE);

        stack.Free();
        Log(Status::Ok, "Bounds guards are placed adjacent and have magic values.");
    }

    if (1)
    {
        // Push to tail and head
        if (1)
        {
            DqnMemStack stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::BoundsGuard);

            auto *result1    = stack.Push(100);
            auto *result2    = stack.Push(100, DqnMemStack::AllocTo::Tail);
            auto *headBefore = stack.block->head;
            auto *tailBefore = stack.block->tail;
            DQN_ASSERT(result2 && result1);
            DQN_ASSERT(result2 != result1 && result1 < result2);

            stack.Pop(result2);
            DQN_ASSERT(headBefore == stack.block->head)
            DQN_ASSERT(tailBefore != stack.block->tail)

            stack.Pop(result1);
            DQN_ASSERT(stack.block->prevBlock == false);
            DQN_ASSERT(stack.block->head == stack.block->memory);
            DQN_ASSERT(stack.block->tail == stack.block->memory + stack.block->size);
            stack.Free();
            Log(Status::Ok, "Push, pop to tail and head.");
        }

        // Expansion with tail
        if (1)
        {
            // Push too much to tail causes expansion
            if (1)
            {
                DqnMemStack stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::BoundsGuard);

                auto *result1 = stack.Push(100);
                DQN_ASSERT(stack.block->prevBlock == nullptr);
                DQN_ASSERT(stack.block->head > stack.block->memory && stack.block->head < stack.block->tail);
                DQN_ASSERT(stack.block->tail == stack.block->memory + stack.block->size);
                auto *blockBefore = stack.block;

                auto *result2 = stack.Push(DQN_MEGABYTE(1), DqnMemStack::AllocTo::Tail);
                DQN_ASSERT(result2 && result1);
                DQN_ASSERT(result2 != result1);
                DQN_ASSERT(stack.block->prevBlock == blockBefore);
                DQN_ASSERT(stack.block != blockBefore);

                DQN_ASSERT(stack.block->head == stack.block->memory);
                DQN_ASSERT(stack.block->tail < stack.block->memory + stack.block->size &&
                           stack.block->tail >= stack.block->head);

                stack.Pop(result2);
                DQN_ASSERT(blockBefore == stack.block);

                stack.Pop(result1);
                DQN_ASSERT(blockBefore == stack.block);

                stack.Free();
            }

            // Push too much to tail fails to expand when non expandable
            if (1)
            {
                DqnMemStack stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::NonExpandable);

                auto *result1 = stack.Push(100);
                DQN_ASSERT(stack.block->prevBlock == nullptr);
                DQN_ASSERT(stack.block->head != stack.block->memory);
                DQN_ASSERT(stack.block->tail == stack.block->memory + stack.block->size);
                auto *blockBefore = stack.block;

                auto *result2 = stack.Push(DQN_MEGABYTE(1), DqnMemStack::AllocTo::Tail);
                DQN_ASSERT(result2 == nullptr);
                DQN_ASSERT(stack.block->prevBlock == nullptr);
                DQN_ASSERT(stack.block == blockBefore);
                DQN_ASSERT(stack.block->head > stack.block->memory && stack.block->head < stack.block->tail);
                DQN_ASSERT(stack.block->tail == stack.block->memory + stack.block->size);

                stack.Pop(result2);
                DQN_ASSERT(blockBefore == stack.block);

                stack.Pop(result1);
                DQN_ASSERT(blockBefore == stack.block);

                stack.Free();
            }

            Log(Status::Ok, "Non-Expanding and expanding stack with tail push.");
        }
    }

    // Check stack allocator mem api callbacks
    if (1)
    {
        // Realloc in same block and allow it to grow in place.
        if (1)
        {
            // Using push on head
            if (1)
            {
                DqnMemStack stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::BoundsGuard);
                auto *api = &stack.myHeadAPI;

                auto *blockBefore = stack.block;
                auto *headBefore  = stack.block->head;

                isize bufSize = 16;
                char *buf     = (char *)stack.Push(bufSize);
                DqnMem_Set(buf, 'X', bufSize);
                for (auto i = 0; i < bufSize; i++) DQN_ASSERT(buf[i] == 'X');

                isize oldBufSize = bufSize;
                bufSize          = 32;
                buf        = (char *)api->Realloc(buf, oldBufSize, bufSize);
                for (auto i = 0; i < oldBufSize; i++) DQN_ASSERT(buf[i] == 'X');
                DqnMem_Set(buf, '@', bufSize);

                DQN_ASSERT(blockBefore == stack.block);
                DQN_ASSERT(headBefore < stack.block->head);
                stack.Pop(buf);

                DQN_ASSERT(blockBefore == stack.block);
                DQN_ASSERT(headBefore == stack.block->head);
                DQN_ASSERT(headBefore == stack.block->memory);
                stack.Free();
            }

            // Using push on tail
            if (1)
            {
                DqnMemStack stack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::BoundsGuard);
                auto *api = &stack.myHeadAPI;

                auto *blockBefore = stack.block;
                auto *tailBefore  = stack.block->tail;

                isize bufSize = 16;
                char *buf     = (char *)stack.Push(bufSize, DqnMemStack::AllocTo::Tail);
                DqnMem_Set(buf, 'X', bufSize);
                for (auto i = 0; i < bufSize; i++) DQN_ASSERT(buf[i] == 'X');

                isize oldBufSize = bufSize;
                bufSize          = 32;
                buf        = (char *)api->Realloc(buf, oldBufSize, bufSize);
                for (auto i = 0; i < oldBufSize; i++) DQN_ASSERT(buf[i] == 'X');
                DqnMem_Set(buf, '@', bufSize);

                DQN_ASSERT(blockBefore == stack.block);
                DQN_ASSERT(tailBefore > stack.block->tail);
                stack.Pop(buf);

                DQN_ASSERT(blockBefore == stack.block);
                DQN_ASSERT(tailBefore == stack.block->tail);
                DQN_ASSERT(stack.block->head == stack.block->memory);
                stack.Free();
            }
            Log(Status::Ok, "Allocator MemAPI callback, realloc grow in place");
        }

        // Realloc in same block and insufficient size and expand
        if (1)
        {
            // Using push on head
            if (1)
            {
                auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::BoundsGuard);
                auto *api = &stack.myHeadAPI;

                auto *blockBefore = stack.block;
                auto *headBefore  = stack.block->head;

                isize bufSize = 16;
                char *buf     = (char *)stack.Push(bufSize);
                DqnMem_Set(buf, 'X', bufSize);
                for (auto i = 0; i < bufSize; i++) DQN_ASSERT(buf[i] == 'X');

                isize oldBufSize = bufSize;
                bufSize          = DQN_MEGABYTE(2);
                buf              = (char *)api->Realloc(buf, oldBufSize, bufSize);
                for (auto i = 0; i < oldBufSize; i++) DQN_ASSERT(buf[i] == 'X');
                DqnMem_Set(buf, '@', bufSize);

                DQN_ASSERT(blockBefore == stack.block->prevBlock);
                stack.Pop(buf);

                DQN_ASSERT(blockBefore == stack.block);
                DQN_ASSERT(headBefore == stack.block->head);
                DQN_ASSERT(headBefore == stack.block->memory);
                stack.Free();
            }

            // Using push on tail
            if (1)
            {
                DqnMemStack stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroClear::Yes, DqnMemStack::Flag::BoundsGuard);
                auto *api = &stack.myHeadAPI;

                auto *blockBefore = stack.block;
                auto *tailBefore  = stack.block->tail;

                isize bufSize = 16;
                char *buf     = (char *)stack.Push(bufSize, DqnMemStack::AllocTo::Tail);
                DqnMem_Set(buf, 'X', bufSize);
                for (auto i = 0; i < bufSize; i++) DQN_ASSERT(buf[i] == 'X');

                isize oldBufSize = bufSize;
                bufSize          = DQN_MEGABYTE(2);
                buf              = (char *)api->Realloc(buf, oldBufSize, bufSize);
                for (auto i = 0; i < oldBufSize; i++)
                    DQN_ASSERT(buf[i] == 'X');
                DqnMem_Set(buf, '@', bufSize);

                DQN_ASSERT(blockBefore != stack.block);
                DQN_ASSERT(blockBefore == stack.block->prevBlock);
                stack.Pop(buf);

                DQN_ASSERT(blockBefore == stack.block);
                DQN_ASSERT(tailBefore == stack.block->tail);

                DQN_ASSERT(stack.block->head == stack.block->memory);
                stack.Free();
            }
            Log(Status::Ok, "Allocator MemAPI callback, realloc insufficient size so expand");
        }

        // TODO(doyle): Realloc to smaller size logic
    }
}

