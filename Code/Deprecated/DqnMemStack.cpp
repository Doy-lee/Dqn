FILE_SCOPE void DqnMemStack_Test()
{
    LOG_HEADER();

    // Check Alignment
    if (1)
    {
        auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, 0, DqnMemTracker::Flag::Simple);

        i32 const ALIGN64            = 64;
        i32 const ALIGN16            = 16;
        i32 const ALIGN4             = 4;
        DqnMemStack::PushType push_type = DqnMemStack::PushType::Head;
        if (1)
        {
            u8 *result1 = (u8 *)stack.Push_(2, push_type, ALIGN4);
            u8 *result2 = (u8 *)DQN_ALIGN_POW_N(result1, ALIGN4);
            DQN_ASSERT(result1 == result2);
            stack.Pop(result1);
            DQN_ASSERT(stack.block->head == stack.block->memory);
        }

        if (1)
        {
            u8 *result1 = (u8 *)stack.Push_(120, push_type, ALIGN16);
            u8 *result2 = (u8 *)DQN_ALIGN_POW_N(result1, ALIGN16);
            DQN_ASSERT(result1 == result2);
            stack.Pop(result1);
            DQN_ASSERT(stack.block->head == stack.block->memory);
        }

        if (1)
        {
            u8 *result1 = (u8 *)stack.Push_(12, push_type, ALIGN64);
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
        auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, DqnMemStack::Flag::NonExpandable);
        auto *result1 = stack.Push_(DQN_MEGABYTE(2));
        DQN_ASSERT(result1 == nullptr);
        DQN_ASSERT(stack.block->prev_block == nullptr);

        stack.Free();
        Log(Status::Ok, "Check non-expandable flag prevents expansion.");
    }

    // Check Expansion
    if (1)
    {
        auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes);
        auto *old_block = stack.block;
        DQN_ASSERT(old_block);
        DQN_ASSERT(old_block->size == DQN_MEGABYTE(1));
        DQN_ASSERT(old_block->head == old_block->head);
        DQN_ASSERT(old_block->tail == old_block->tail);
        DQN_ASSERT(old_block->prev_block == nullptr);

        auto *result1 = stack.Push_(DQN_MEGABYTE(2));
        DQN_ASSERT(result1);
        DQN_ASSERT(stack.block->prev_block == old_block);
        DQN_ASSERT(stack.block != old_block);

        Log(Status::Ok, "Check memory stack allocates additional memory blocks.");
        stack.Free();
    }

    // Temporary Regions
    if (1)
    {
        // Check temporary regions
        if (1)
        {
            auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, 0, DqnMemTracker::Flag::Simple);

            DqnMemStack::Block *block_to_return_to = stack.block;
            auto head_before                       = block_to_return_to->head;
            auto tail_before                       = block_to_return_to->tail;
            if (1)
            {
                auto mem_guard1 = stack.MemRegionScope();
                auto *result2  = stack.Push_(100);
                auto *result3  = stack.Push_(100);
                auto *result4  = stack.Push_(100);
                DQN_ASSERT(result2 && result3 && result4);
                DQN_ASSERT(stack.block->head != head_before);
                DQN_ASSERT(stack.block->tail == tail_before);
                DQN_ASSERT(stack.block->memory == block_to_return_to->memory);

                // Force allocation of new block
                auto *result5 = stack.Push_(DQN_MEGABYTE(5));
                DQN_ASSERT(result5);
                DQN_ASSERT(stack.block != block_to_return_to);
                DQN_ASSERT(stack.mem_region_count == 1);
            }

            DQN_ASSERT(stack.block == block_to_return_to);
            DQN_ASSERT(stack.block->head == head_before);
            DQN_ASSERT(stack.block->tail == tail_before);

            stack.Free();
        }

        // Check temporary regions keep state
        if (1)
        {
            auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, 0, DqnMemTracker::Flag::Simple);
            DqnMemStack::Block *block_to_return_to = stack.block;
            auto head_before                     = block_to_return_to->head;
            auto tail_before                     = block_to_return_to->tail;
            if (1)
            {
                auto mem_guard1 = stack.MemRegionScope();
                auto *result2  = stack.Push_(100);
                auto *result3  = stack.Push_(100);
                auto *result4  = stack.Push_(100);
                DQN_ASSERT(result2 && result3 && result4);
                DQN_ASSERT(stack.block->head != head_before);
                DQN_ASSERT(stack.block->tail == tail_before);
                DQN_ASSERT(stack.block->memory == block_to_return_to->memory);

                // Force allocation of new block
                auto *result5 = stack.Push_(DQN_MEGABYTE(5));
                DQN_ASSERT(result5);
                DQN_ASSERT(stack.block != block_to_return_to);
                DQN_ASSERT(stack.mem_region_count == 1);
                stack.MemRegionSave(&mem_guard1);
            }

            DQN_ASSERT(stack.block != block_to_return_to);
            DQN_ASSERT(stack.block->prev_block == block_to_return_to);
            DQN_ASSERT(stack.mem_region_count == 0);

            stack.Free();
        }

        // Check temporary regions with tail and head pushes
        if (1)
        {
            auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, 0, DqnMemTracker::Flag::Simple);

            auto *pop1 = stack.Push_(222);
            auto *pop2 = stack.Push_(333, DqnMemStack::PushType::Tail);

            DqnMemStack::Block *block_to_return_to = stack.block;
            auto head_before = block_to_return_to->head;
            auto tail_before = block_to_return_to->tail;
            if (1)
            {
                auto mem_guard1 = stack.MemRegionScope();
                auto *result2  = stack.Push_(100);
                auto *result3  = stack.Push_(100, DqnMemStack::PushType::Tail);
                auto *result4  = stack.Push_(100);
                auto *result5  = stack.Push_(100, DqnMemStack::PushType::Tail);
                DQN_ASSERT(result2 && result3 && result4 && result5);
                DQN_ASSERT(result3 > result5);
                DQN_ASSERT(result2 < result4);
                DQN_ASSERT(stack.block->head > head_before && stack.block->head < stack.block->tail);
                DQN_ASSERT(stack.block->tail >= stack.block->head && stack.block->tail < (stack.block->memory + stack.block->size));
                DQN_ASSERT(stack.block->memory == block_to_return_to->memory);

                // Force allocation of new block
                auto *result6 = stack.Push_(DQN_MEGABYTE(5));
                DQN_ASSERT(result6);
                DQN_ASSERT(stack.block != block_to_return_to);
                DQN_ASSERT(stack.mem_region_count == 1);
            }

            DQN_ASSERT(stack.block == block_to_return_to);
            DQN_ASSERT(stack.block->head == head_before);
            DQN_ASSERT(stack.block->tail == tail_before);

            stack.Pop(pop1);
            stack.Pop(pop2);
            DQN_ASSERT(stack.block->head == stack.block->memory);
            DQN_ASSERT(stack.block->tail == stack.block->memory + stack.block->size);

            stack.Free();
        }
        Log(Status::Ok, "Temporary regions revert state and save state");
    }

    // Check Fixed Mem Init
    if (1)
    {
        // Check success
        if (1)
        {
            isize const buf_size = sizeof(DqnMemStack::Block) * 5;
            char buf[buf_size]   = {};
            auto stack          = DqnMemStack(&buf, buf_size, Dqn::ZeroMem::No);

            DQN_ASSERT(stack.block);
            DQN_ASSERT(stack.block->prev_block == false);
            DQN_ASSERT(stack.mem_region_count == 0);
            DQN_ASSERT(stack.flags == DqnMemStack::Flag::NonExpandable);

            auto *result1 = stack.Push_(32);
            DQN_ASSERT(result1);
            stack.Pop(result1);

            auto *result2 = stack.Push_(buf_size * 2);
            DQN_ASSERT(result2 == nullptr);
            DQN_ASSERT(stack.block);
            DQN_ASSERT(stack.block->prev_block == false);
            DQN_ASSERT(stack.mem_region_count == 0);
            DQN_ASSERT(stack.flags == DqnMemStack::Flag::NonExpandable);

            stack.Free();
        }

        Log(Status::Ok, "Checked fixed mem initialisation");
    }

    // Check Freeing Blocks
    if (1)
    {
        usize size           = 32;
        usize additional_size = DqnMemStack::MINIMUM_BLOCK_SIZE;

        auto stack   = DqnMemStack(size, Dqn::ZeroMem::Yes, 0);
        auto *block1 = stack.block;

        size += additional_size;
        auto *result1 = stack.Push_(size);
        auto *block2  = stack.block;

        size += additional_size;
        auto *result2 = stack.Push_(size);
        auto *block3  = stack.block;

        size += additional_size;
        auto *result3 = stack.Push_(size);
        auto *block4  = stack.block;

        size += additional_size;
        auto *result4 = stack.Push_(size);
        auto *block5  = stack.block;

        DQN_ASSERT(result1 && result2 && result3 && result4);
        DQN_ASSERT(block1 && block2 && block3 && block4 && block5);
        DQN_ASSERT(block5->prev_block == block4);
        DQN_ASSERT(block4->prev_block == block3);
        DQN_ASSERT(block3->prev_block == block2);
        DQN_ASSERT(block2->prev_block == block1);
        DQN_ASSERT(block1->prev_block == nullptr);

        DQN_ASSERT(stack.FreeBlock(block4));
        DQN_ASSERT(stack.block == block5);
        DQN_ASSERT(block5->prev_block == block3);
        DQN_ASSERT(block3->prev_block == block2);
        DQN_ASSERT(block2->prev_block == block1);
        DQN_ASSERT(block1->prev_block == nullptr);

        DQN_ASSERT(stack.FreeBlock(block5));
        DQN_ASSERT(stack.block == block3);
        DQN_ASSERT(block3->prev_block == block2);
        DQN_ASSERT(block2->prev_block == block1);
        DQN_ASSERT(block1->prev_block == nullptr);

        stack.Free();
        DQN_ASSERT(stack.block == nullptr);
        Log(Status::Ok, "Check freeing arbitrary blocks and freeing");
    }

    // Check bounds guard places magic values
    if (1)
    {
        auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, 0, DqnMemTracker::Flag::Simple);
        char *result = static_cast<char *>(stack.Push_(64));

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
        // Push_ to tail and head
        if (1)
        {
            DqnMemStack stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, 0, DqnMemTracker::Flag::Simple);

            auto *result1    = stack.Push_(100);
            auto *result2    = stack.Push_(100, DqnMemStack::PushType::Tail);
            auto *head_before = stack.block->head;
            auto *tail_before = stack.block->tail;
            DQN_ASSERT(result2 && result1);
            DQN_ASSERT(result2 != result1 && result1 < result2);

            stack.Pop(result2);
            DQN_ASSERT(head_before == stack.block->head)
            DQN_ASSERT(tail_before != stack.block->tail)

            stack.Pop(result1);
            DQN_ASSERT(stack.block->prev_block == false);
            DQN_ASSERT(stack.block->head == stack.block->memory);
            DQN_ASSERT(stack.block->tail == stack.block->memory + stack.block->size);
            stack.Free();
            Log(Status::Ok, "Push_, pop to tail and head.");
        }

        // Expansion with tail
        if (1)
        {
            // Push_ too much to tail causes expansion
            if (1)
            {
                DqnMemStack stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, 0, DqnMemTracker::Flag::Simple);

                auto *result1 = stack.Push_(100);
                DQN_ASSERT(stack.block->prev_block == nullptr);
                DQN_ASSERT(stack.block->head > stack.block->memory && stack.block->head < stack.block->tail);
                DQN_ASSERT(stack.block->tail == stack.block->memory + stack.block->size);
                auto *block_before = stack.block;

                auto *result2 = stack.Push_(DQN_MEGABYTE(1), DqnMemStack::PushType::Tail);
                DQN_ASSERT(result2 && result1);
                DQN_ASSERT(result2 != result1);
                DQN_ASSERT(stack.block->prev_block == block_before);
                DQN_ASSERT(stack.block != block_before);

                DQN_ASSERT(stack.block->head == stack.block->memory);
                DQN_ASSERT(stack.block->tail < stack.block->memory + stack.block->size &&
                           stack.block->tail >= stack.block->head);

                stack.Pop(result2);
                DQN_ASSERT(block_before == stack.block);

                stack.Pop(result1);
                DQN_ASSERT(block_before == stack.block);

                stack.Free();
            }

            // Push_ too much to tail fails to expand when non expandable
            if (1)
            {
                DqnMemStack stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, DqnMemStack::Flag::NonExpandable);

                auto *result1 = stack.Push_(100);
                DQN_ASSERT(stack.block->prev_block == nullptr);
                DQN_ASSERT(stack.block->head != stack.block->memory);
                DQN_ASSERT(stack.block->tail == stack.block->memory + stack.block->size);
                auto *block_before = stack.block;

                auto *result2 = stack.Push_(DQN_MEGABYTE(1), DqnMemStack::PushType::Tail);
                DQN_ASSERT(result2 == nullptr);
                DQN_ASSERT(stack.block->prev_block == nullptr);
                DQN_ASSERT(stack.block == block_before);
                DQN_ASSERT(stack.block->head > stack.block->memory && stack.block->head < stack.block->tail);
                DQN_ASSERT(stack.block->tail == stack.block->memory + stack.block->size);

                stack.Pop(result2);
                DQN_ASSERT(block_before == stack.block);

                stack.Pop(result1);
                DQN_ASSERT(block_before == stack.block);

                stack.Free();
            }

            Log(Status::Ok, "Non-Expanding and expanding stack with tail push.");
        }
    }

    // Check stack allocator mem api callbacks
    if (1)
    {
#if 0
        // Realloc in same block and allow it to grow in place.
        if (1)
        {
            // Using push on head
            if (1)
            {
                DqnMemStack stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, DqnMemStack::Flag::BoundsGuard);
                auto *api = &stack.myHeadAPI;

                auto *block_before = stack.block;
                auto *head_before  = stack.block->head;

                isize buf_size = 16;
                char *buf     = (char *)stack.Push_(buf_size);
                DqnMem_Set(buf, 'X', buf_size);
                for (auto i = 0; i < buf_size; i++) DQN_ASSERT(buf[i] == 'X');

                isize old_buf_size = buf_size;
                buf_size          = 32;
                buf        = (char *)api->Realloc(buf, old_buf_size, buf_size);
                for (auto i = 0; i < old_buf_size; i++) DQN_ASSERT(buf[i] == 'X');
                DqnMem_Set(buf, '@', buf_size);

                DQN_ASSERT(block_before == stack.block);
                DQN_ASSERT(head_before < stack.block->head);
                stack.Pop(buf);

                DQN_ASSERT(block_before == stack.block);
                DQN_ASSERT(head_before == stack.block->head);
                DQN_ASSERT(head_before == stack.block->memory);
                stack.Free();
            }

            // Using push on tail
            if (1)
            {
                DqnMemStack stack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, DqnMemStack::Flag::BoundsGuard);
                auto *api = &stack.myHeadAPI;

                auto *block_before = stack.block;
                auto *tail_before  = stack.block->tail;

                isize buf_size = 16;
                char *buf     = (char *)stack.Push_(buf_size, DqnMemStack::PushType::Tail);
                DqnMem_Set(buf, 'X', buf_size);
                for (auto i = 0; i < buf_size; i++) DQN_ASSERT(buf[i] == 'X');

                isize old_buf_size = buf_size;
                buf_size          = 32;
                buf        = (char *)api->Realloc(buf, old_buf_size, buf_size);
                for (auto i = 0; i < old_buf_size; i++) DQN_ASSERT(buf[i] == 'X');
                DqnMem_Set(buf, '@', buf_size);

                DQN_ASSERT(block_before == stack.block);
                DQN_ASSERT(tail_before > stack.block->tail);
                stack.Pop(buf);

                DQN_ASSERT(block_before == stack.block);
                DQN_ASSERT(tail_before == stack.block->tail);
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
                auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, DqnMemStack::Flag::BoundsGuard);
                auto *api = &stack.myHeadAPI;

                auto *block_before = stack.block;
                auto *head_before  = stack.block->head;

                isize buf_size = 16;
                char *buf     = (char *)stack.Push_(buf_size);
                DqnMem_Set(buf, 'X', buf_size);
                for (auto i = 0; i < buf_size; i++) DQN_ASSERT(buf[i] == 'X');

                isize old_buf_size = buf_size;
                buf_size          = DQN_MEGABYTE(2);
                buf              = (char *)api->Realloc(buf, old_buf_size, buf_size);
                for (auto i = 0; i < old_buf_size; i++) DQN_ASSERT(buf[i] == 'X');
                DqnMem_Set(buf, '@', buf_size);

                DQN_ASSERT(block_before == stack.block->prev_block);
                stack.Pop(buf);

                DQN_ASSERT(block_before == stack.block);
                DQN_ASSERT(head_before == stack.block->head);
                DQN_ASSERT(head_before == stack.block->memory);
                stack.Free();
            }

            // Using push on tail
            if (1)
            {
                DqnMemStack stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, DqnMemStack::Flag::BoundsGuard);
                auto *api = &stack.myHeadAPI;

                auto *block_before = stack.block;
                auto *tail_before  = stack.block->tail;

                isize buf_size = 16;
                char *buf     = (char *)stack.Push_(buf_size, DqnMemStack::PushType::Tail);
                DqnMem_Set(buf, 'X', buf_size);
                for (auto i = 0; i < buf_size; i++) DQN_ASSERT(buf[i] == 'X');

                isize old_buf_size = buf_size;
                buf_size          = DQN_MEGABYTE(2);
                buf              = (char *)api->Realloc(buf, old_buf_size, buf_size);
                for (auto i = 0; i < old_buf_size; i++)
                    DQN_ASSERT(buf[i] == 'X');
                DqnMem_Set(buf, '@', buf_size);

                DQN_ASSERT(block_before != stack.block);
                DQN_ASSERT(block_before == stack.block->prev_block);
                stack.Pop(buf);

                DQN_ASSERT(block_before == stack.block);
                DQN_ASSERT(tail_before == stack.block->tail);

                DQN_ASSERT(stack.block->head == stack.block->memory);
                stack.Free();
            }
            Log(Status::Ok, "Allocator MemAPI callback, realloc insufficient size so expand");
        }

        // TODO(doyle): Realloc to smaller size logic
#endif
    }
}

