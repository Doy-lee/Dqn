// =================================================================================================
// [$ALLO] Dqn_Allocator        |                             | Generic allocator interface
// =================================================================================================
DQN_API void *Dqn_Allocator_Alloc_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, size_t size, uint8_t align, Dqn_ZeroMem zero_mem)
{
    void *result = NULL;
    if (allocator.alloc) {
        result = allocator.alloc(DQN_LEAK_TRACE_ARG size, align, zero_mem, allocator.user_context);
    } else {
        result = DQN_ALLOC(size);
        Dqn_Library_LeakTraceAdd(DQN_LEAK_TRACE_ARG result, size);
    }
    return result;
}

DQN_API void Dqn_Allocator_Dealloc_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, void *ptr, size_t size)
{
    if (allocator.dealloc) {
        allocator.dealloc(DQN_LEAK_TRACE_ARG ptr, size, allocator.user_context);
    } else {
        DQN_DEALLOC(ptr, size);
        Dqn_Library_LeakTraceMarkFree(DQN_LEAK_TRACE_ARG ptr);
    }
}

// =================================================================================================
// [$VMEM] Dqn_VMem             |                             | Virtual memory allocation
// =================================================================================================
DQN_FILE_SCOPE uint32_t Dqn_VMem_ConvertPageToOSFlags_(uint32_t protect)
{
    DQN_ASSERT((protect & ~(Dqn_VMemPage_ReadWrite | Dqn_VMemPage_Guard)) == 0);
    DQN_ASSERT(protect != 0);
    uint32_t result = 0;

    #if defined(DQN_OS_WIN32)
    if (protect & Dqn_VMemPage_NoAccess) {
        result = PAGE_NOACCESS;
    } else {
        if (protect & Dqn_VMemPage_ReadWrite) {
            result = PAGE_READWRITE;
        }  else if (protect & Dqn_VMemPage_Read) {
            result = PAGE_READONLY;
        } else if (protect & Dqn_VMemPage_Write) {
            Dqn_Log_WarningF("Windows does not support write-only pages, granting read+write access");
            result = PAGE_READWRITE;
        }
    }

    if (protect & Dqn_VMemPage_Guard)
        result |= PAGE_GUARD;

    DQN_ASSERTF(result != PAGE_GUARD, "Page guard is a modifier, you must also specify a page permission like read or/and write");
    #else
    if (protect & (Dqn_VMemPage_NoAccess | Dqn_VMemPage_Guard)) {
        result = PROT_NONE;
    } else {
        if (protect & Dqn_VMemPage_Read)
            result = PROT_READ;
        if (protect & Dqn_VMemPage_Write)
            result = PROT_WRITE;
    }
    #endif

    return result;
}

DQN_API void *Dqn_VMem_Reserve(Dqn_usize size, Dqn_VMemCommit commit, uint32_t page_flags)
{
    unsigned long os_page_flags = Dqn_VMem_ConvertPageToOSFlags_(page_flags);

    #if defined(DQN_OS_WIN32)
    unsigned long flags = MEM_RESERVE | (commit == Dqn_VMemCommit_Yes ? MEM_COMMIT : 0);
    void *result        = VirtualAlloc(nullptr, size, flags, os_page_flags);

    #elif defined(DQN_OS_UNIX)
    if (commit == Dqn_VMemCommit_Yes)
        os_page_flags |= (PROT_READ | PROT_WRITE);

    void *result = mmap(nullptr, size, os_page_flags, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (result == MAP_FAILED)
        result = nullptr;

    #else
        #error "Missing implementation for Dqn_VMem_Reserve"
    #endif
    return result;
}

DQN_API bool Dqn_VMem_Commit(void *ptr, Dqn_usize size, uint32_t page_flags)
{
    unsigned long os_page_flags = Dqn_VMem_ConvertPageToOSFlags_(page_flags);
    #if defined(DQN_OS_WIN32)
    bool result = VirtualAlloc(ptr, size, MEM_COMMIT, os_page_flags) != nullptr;
    #elif defined(DQN_OS_UNIX)
    bool result = mprotect(ptr, size, os_page_flags) == 0;
    #else
        #error "Missing implementation for Dqn_VMem_Commit"
    #endif
    return result;
}

DQN_API void Dqn_VMem_Decommit(void *ptr, Dqn_usize size)
{
    #if defined(DQN_OS_WIN32)
    VirtualFree(ptr, size, MEM_DECOMMIT);
    #elif defined(DQN_OS_UNIX)
    mprotect(ptr, size, PROT_NONE);
    madvise(ptr, size, MADV_FREE);
    #else
        #error "Missing implementation for Dqn_VMem_Decommit"
    #endif
}

DQN_API void Dqn_VMem_Release(void *ptr, Dqn_usize size)
{
    #if defined(DQN_OS_WIN32)
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
    #elif defined(DQN_OS_UNIX)
    munmap(ptr, size);
    #else
        #error "Missing implementation for Dqn_VMem_Release"
    #endif
}

DQN_API int Dqn_VMem_Protect(void *ptr, Dqn_usize size, uint32_t page_flags)
{
    if (!ptr || size == 0)
        return 0;

    static Dqn_String8 const ALIGNMENT_ERROR_MSG =
        DQN_STRING8("Page protection requires pointers to be page aligned because we "
                    "can only guard memory at a multiple of the page boundary.");
    DQN_ASSERTF(Dqn_IsPowerOfTwoAligned(DQN_CAST(uintptr_t)ptr, DQN_VMEM_PAGE_GRANULARITY), "%s", ALIGNMENT_ERROR_MSG.data);
    DQN_ASSERTF(Dqn_IsPowerOfTwoAligned(size,                   DQN_VMEM_PAGE_GRANULARITY), "%s", ALIGNMENT_ERROR_MSG.data);

    unsigned long os_page_flags = Dqn_VMem_ConvertPageToOSFlags_(page_flags);
    #if defined(DQN_OS_WIN32)
    unsigned long prev_flags = 0;
    int result = VirtualProtect(ptr, size, os_page_flags, &prev_flags);
    (void)prev_flags;
    if (result == 0) {
        Dqn_WinErrorMsg error = Dqn_Win_LastError();
        DQN_ASSERTF(result, "VirtualProtect failed (%d): %.*s", error.code, error.size, error.data);
    }
    #else
    int result = mprotect(result->memory, result->size, os_page_flags);
    DQN_ASSERTF(result == 0, "mprotect failed (%d)", errno);
    #endif

    return result;
}

// =================================================================================================
// [$AREN] Dqn_Arena            |                             | Growing bump allocator
// =================================================================================================
DQN_API void Dqn_Arena_CommitFromBlock(Dqn_ArenaBlock *block, Dqn_usize size, Dqn_ArenaCommit commit)
{
    Dqn_usize commit_size = 0;
    switch (commit) {
        case Dqn_ArenaCommit_GetNewPages: {
            commit_size = Dqn_PowerOfTwoAlign(size, DQN_VMEM_COMMIT_GRANULARITY);
        } break;

        case Dqn_ArenaCommit_EnsureSpace: {
            DQN_ASSERT(block->commit >= block->used);
            Dqn_usize const unused_commit_space = block->commit - block->used;
            if (unused_commit_space < size)
                commit_size = Dqn_PowerOfTwoAlign(size - unused_commit_space, DQN_VMEM_COMMIT_GRANULARITY);
        } break;
    }

    if (commit_size) {
        uint32_t page_flags = Dqn_VMemPage_ReadWrite;
        char *commit_ptr = DQN_CAST(char *)block->memory + block->commit;
        Dqn_VMem_Commit(commit_ptr, commit_size, page_flags);
        block->commit += commit_size;

        DQN_ASSERTF(block->commit < block->size,
                    "Block size should be PoT aligned and its alignment should be greater than the commit granularity [block_size=%_$$d, block_commit=%_$$d]",
                    block->size, block->commit);

        // NOTE: Guard new pages being allocated from the block
        if (block->arena->use_after_free_guard)
            Dqn_VMem_Protect(commit_ptr, commit_size, page_flags | Dqn_VMemPage_Guard);
    }
}

DQN_API void *Dqn_Arena_AllocateFromBlock(Dqn_ArenaBlock *block, Dqn_usize size, uint8_t align, Dqn_ZeroMem zero_mem)
{
    if (!block)
        return nullptr;

    DQN_ASSERTF((align & (align - 1)) == 0, "Power of two alignment required");
    align = DQN_MAX(align, 1);

    DQN_ASSERT(block->hwm_used <= block->commit);
    DQN_ASSERT(block->hwm_used >= block->used);
    DQN_ASSERTF(block->commit >= block->used,
                "Internal error: Committed size must always be greater than the used size [commit=%_$$zd, used=%_$$zd]",
                block->commit, block->used);

    // NOTE: Calculate how much we need to pad the next pointer to divvy out
    // (only if it is unaligned)
    uintptr_t next_ptr     = DQN_CAST(uintptr_t)block->memory + block->used;
    Dqn_usize align_offset = 0;
    if (next_ptr & (align - 1))
        align_offset = (align - (next_ptr & (align - 1)));

    Dqn_usize allocation_size = size + align_offset;
    if ((block->used + allocation_size) > block->size)
        return nullptr;

    void *result = DQN_CAST(char *)next_ptr + align_offset;
    if (zero_mem == Dqn_ZeroMem_Yes) {
        // NOTE: Newly commit pages are always 0-ed out, we only need to
        // memset the memory that are being reused.
        Dqn_usize const reused_bytes = DQN_MIN(block->hwm_used - block->used, allocation_size);
        DQN_MEMSET(DQN_CAST(void *)next_ptr, DQN_MEMSET_BYTE, reused_bytes);
    }

    // NOTE: Ensure requested bytes are backed by physical pages from the OS
    Dqn_Arena_CommitFromBlock(block, allocation_size, Dqn_ArenaCommit_EnsureSpace);

    // NOTE: Unlock the pages requested
    if (block->arena->use_after_free_guard)
        Dqn_VMem_Protect(result, allocation_size, Dqn_VMemPage_ReadWrite);

    // NOTE: Update arena
    Dqn_Arena *arena       = block->arena;
    arena->stats.used     += allocation_size;
    arena->stats.used_hwm  = DQN_MAX(arena->stats.used_hwm, arena->stats.used);
    block->used           += allocation_size;
    block->hwm_used        = DQN_MAX(block->hwm_used, block->used);

    DQN_ASSERTF(block->used   <= block->commit,                   "Internal error: Committed size must be greater than used size [used=%_$$zd, commit=%_$$zd]", block->used, block->commit);
    DQN_ASSERTF(block->commit <= block->size,                     "Internal error: Allocation exceeded block capacity [commit=%_$$zd, size=%_$$zd]", block->commit, block->size);
    DQN_ASSERTF(((DQN_CAST(uintptr_t)result) & (align - 1)) == 0, "Internal error: Pointer alignment failed [address=%p, align=%x]", result, align);

    return result;
}

DQN_FILE_SCOPE void *Dqn_Arena_AllocatorAlloc_(DQN_LEAK_TRACE_FUNCTION size_t size, uint8_t align, Dqn_ZeroMem zero_mem, void *user_context)
{
    void *result = NULL;
    if (!user_context)
        return result;

    Dqn_Arena *arena = DQN_CAST(Dqn_Arena *)user_context;
    result           = Dqn_Arena_Allocate_(DQN_LEAK_TRACE_ARG arena, size, align, zero_mem);
    return result;
}

DQN_FILE_SCOPE void Dqn_Arena_AllocatorDealloc_(DQN_LEAK_TRACE_FUNCTION void *ptr, size_t size, void *user_context)
{
    // NOTE: No-op, arenas batch allocate and batch deallocate. Call free on the
    // underlying arena, since we can't free individual pointers.
    DQN_LEAK_TRACE_UNUSED;
    (void)ptr;
    (void)size;
    (void)user_context;
}

DQN_API Dqn_Allocator Dqn_Arena_Allocator(Dqn_Arena *arena)
{
    Dqn_Allocator result = {};
    if (arena) {
        result.user_context = arena;
        result.alloc        = Dqn_Arena_AllocatorAlloc_;
        result.dealloc      = Dqn_Arena_AllocatorDealloc_;
    }
    return result;
}

struct Dqn_ArenaBlockResetInfo_
{
    bool      free_memory;
    Dqn_usize used_value;
};

// Calculate the size in bytes required to allocate the memory for the block
// (*not* including the memory required for the user in the block!)
#define Dqn_Arena_BlockMetadataSize_(arena) ((arena)->use_after_free_guard ? (Dqn_PowerOfTwoAlign(sizeof(Dqn_ArenaBlock), DQN_VMEM_PAGE_GRANULARITY)) : sizeof(Dqn_ArenaBlock))

DQN_API void Dqn_Arena_BlockReset_(DQN_LEAK_TRACE_FUNCTION Dqn_ArenaBlock *block, Dqn_ZeroMem zero_mem, Dqn_ArenaBlockResetInfo_ reset_info)
{
    if (!block)
        return;

    if (zero_mem == Dqn_ZeroMem_Yes)
        DQN_MEMSET(block->memory, DQN_MEMSET_BYTE, block->commit);

    if (reset_info.free_memory) {
        Dqn_usize block_metadata_size = Dqn_Arena_BlockMetadataSize_(block->arena);
        Dqn_VMem_Release(block, block_metadata_size + block->size);
        Dqn_Library_LeakTraceMarkFree(DQN_LEAK_TRACE_ARG block);
    } else {
        block->used = reset_info.used_value;
        // NOTE: Guard all the committed pages again
        if (block->arena->use_after_free_guard)
            Dqn_VMem_Protect(block->memory, block->commit, Dqn_VMemPage_ReadWrite | Dqn_VMemPage_Guard);
    }
}

DQN_API void Dqn_Arena_Reset(Dqn_Arena *arena, Dqn_ZeroMem zero_mem)
{
    if (!arena)
        return;

    // NOTE: Zero all the blocks until we reach the first block in the list
    for (Dqn_ArenaBlock *block = arena->head; block; block = block->next) {
        Dqn_ArenaBlockResetInfo_ reset_info = {};
        Dqn_Arena_BlockReset_(DQN_LEAK_TRACE block, zero_mem, reset_info);
    }

    arena->curr         = arena->head;
    arena->stats.used   = 0;
    arena->stats.wasted = 0;
}

DQN_API Dqn_ArenaTempMemory Dqn_Arena_BeginTempMemory(Dqn_Arena *arena)
{
    Dqn_ArenaTempMemory result = {};
    if (arena) {
        arena->temp_memory_count++;
        result.arena     = arena;
        result.head      = arena->head;
        result.curr      = arena->curr;
        result.tail      = arena->tail;
        result.curr_used = (arena->curr) ? arena->curr->used : 0;
        result.stats     = arena->stats;
    }
    return result;
}

DQN_API void Dqn_Arena_EndTempMemory_(DQN_LEAK_TRACE_FUNCTION Dqn_ArenaTempMemory scope)
{
    if (!scope.arena)
        return;

    Dqn_Arena *arena = scope.arena;
    if (!DQN_CHECKF(arena->temp_memory_count > 0, "End temp memory has been called without a matching begin pair on the arena"))
        return;

    // NOTE: Revert arena stats
    arena->temp_memory_count--;
    arena->stats.capacity = scope.stats.capacity;
    arena->stats.used     = scope.stats.used;
    arena->stats.wasted   = scope.stats.wasted;
    arena->stats.blocks   = scope.stats.blocks;

    // NOTE: Revert the current block to the scope's current block
    arena->head = scope.head;
    arena->curr = scope.curr;
    if (arena->curr) {
        Dqn_ArenaBlock *curr                = arena->curr;
        Dqn_ArenaBlockResetInfo_ reset_info = {};
        reset_info.used_value               = scope.curr_used;
        Dqn_Arena_BlockReset_(DQN_LEAK_TRACE_ARG curr, Dqn_ZeroMem_No, reset_info);
    }

    // NOTE: Free the tail blocks until we reach the scope's tail block
    while (arena->tail != scope.tail) {
        Dqn_ArenaBlock *tail                = arena->tail;
        arena->tail                         = tail->prev;
        Dqn_ArenaBlockResetInfo_ reset_info = {};
        reset_info.free_memory              = true;
        Dqn_Arena_BlockReset_(DQN_LEAK_TRACE_ARG tail, Dqn_ZeroMem_No, reset_info);
    }

    // NOTE: Reset the usage of all the blocks between the tail and current block's
    if (arena->tail) {
        arena->tail->next = nullptr;
        for (Dqn_ArenaBlock *block = arena->tail; block && block != arena->curr; block = block->prev) {
            Dqn_ArenaBlockResetInfo_ reset_info = {};
            Dqn_Arena_BlockReset_(DQN_LEAK_TRACE_ARG block, Dqn_ZeroMem_No, reset_info);
        }
    }
}

Dqn_ArenaTempMemoryScope_::Dqn_ArenaTempMemoryScope_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena)
{
    temp_memory = Dqn_Arena_BeginTempMemory(arena);
    #if defined(DQN_LEAK_TRACING)
    leak_site__ = DQN_LEAK_TRACE_ARG_NO_COMMA;
    #endif
}

Dqn_ArenaTempMemoryScope_::~Dqn_ArenaTempMemoryScope_()
{
    #if defined(DQN_LEAK_TRACING)
    Dqn_Arena_EndTempMemory_(leak_site__, temp_memory);
    #else
    Dqn_Arena_EndTempMemory_(temp_memory);
    #endif
}

DQN_API Dqn_ArenaStatString Dqn_Arena_StatString(Dqn_ArenaStat const *stat)
{
    // NOTE: We use a non-standard format string that is only usable via
    // stb sprintf that GCC warns about as an error. This pragma mutes that.
    #if defined(DQN_COMPILER_GCC)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wformat="
        #pragma GCC diagnostic ignored "-Wformat-extra-args"
    #elif defined(DQN_COMPILER_W32_CLANG)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wformat"
        #pragma GCC diagnostic ignored "-Wformat-invalid-specifier"
        #pragma GCC diagnostic ignored "-Wformat-extra-args"
    #endif

    Dqn_ArenaStatString result = {};
    int size16 = STB_SPRINTF_DECORATE(snprintf)(result.data, DQN_ARRAY_ICOUNT(result.data),
                                                "ArenaStat{"
                                                    "used/hwm=%_$$zd/%_$$zd, "
                                                    "cap/hwm=%_$$zd/%_$$zd, "
                                                    "wasted/hwm=%_$$zd/%_$$zd, "
                                                    "blocks/hwm=%_$$u/%_$$u, "
                                                    "syscalls=%'zd"
                                                "}",
                                                stat->used,     stat->used_hwm,
                                                stat->capacity, stat->capacity_hwm,
                                                stat->wasted,   stat->wasted_hwm,
                                                stat->blocks,   stat->blocks_hwm,
                                                stat->syscalls);
    result.size = Dqn_Safe_SaturateCastIntToU16(size16);

    #if defined(DQN_COMPILER_GCC)
        #pragma GCC diagnostic pop
    #elif defined(DQN_COMPILER_W32_CLANG)
        #pragma GCC diagnostic pop
    #endif

    return result;
}

DQN_API void Dqn_Arena_LogStats(Dqn_Arena const *arena)
{
    Dqn_ArenaStatString string = Dqn_Arena_StatString(&arena->stats);
    Dqn_Log_InfoF("%.*s\n", DQN_STRING_FMT(string));
}

DQN_API Dqn_ArenaBlock *Dqn_Arena_Grow_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, Dqn_usize size, Dqn_usize commit, uint8_t flags)
{
    DQN_ASSERT(commit <= size);
    if (!arena || size == 0)
        return nullptr;

    Dqn_usize block_metadata_size = Dqn_Arena_BlockMetadataSize_(arena);
    commit                        = DQN_MIN(commit, size);
    Dqn_usize reserve_aligned     = Dqn_PowerOfTwoAlign(size   + block_metadata_size, DQN_VMEM_RESERVE_GRANULARITY);
    Dqn_usize commit_aligned      = Dqn_PowerOfTwoAlign(commit + block_metadata_size, DQN_VMEM_COMMIT_GRANULARITY);
    DQN_ASSERT(commit_aligned < reserve_aligned);

    // NOTE: If the commit amount is the same as reserve size we can save one
    // syscall by asking the OS to reserve+commit in the same call.
    Dqn_VMemCommit commit_on_reserve = reserve_aligned == commit_aligned ? Dqn_VMemCommit_Yes : Dqn_VMemCommit_No;
    uint32_t page_flags              = Dqn_VMemPage_ReadWrite;
    auto *result                     = DQN_CAST(Dqn_ArenaBlock *)Dqn_VMem_Reserve(reserve_aligned, commit_on_reserve, page_flags);
    if (result) {
        // NOTE: Commit the amount requested by the user if we did not commit
        // on reserve the initial range.
        if (commit_on_reserve == Dqn_VMemCommit_No) {
            Dqn_VMem_Commit(result, commit_aligned, page_flags);
        } else {
            DQN_ASSERT(commit_aligned == reserve_aligned);
        }

        // NOTE: Sanity check memory is zero-ed out
        DQN_ASSERT(result->used == 0);
        DQN_ASSERT(result->next == nullptr);
        DQN_ASSERT(result->prev == nullptr);

        // NOTE: Setup the block
        result->memory = DQN_CAST(uint8_t *)result + block_metadata_size;
        result->size   = reserve_aligned - block_metadata_size;
        result->commit = commit_aligned - block_metadata_size;
        result->flags  = flags;
        result->arena  = arena;

        // NOTE: Reset the block (this will guard the memory pages if required, otherwise no-op).
        Dqn_ArenaBlockResetInfo_ reset_info = {};
        Dqn_Arena_BlockReset_(DQN_LEAK_TRACE_ARG result, Dqn_ZeroMem_No, reset_info);

        // NOTE: Attach the block to the arena
        if (arena->tail) {
            arena->tail->next = result;
            result->prev      = arena->tail;
        } else {
            DQN_ASSERT(!arena->curr);
            arena->curr = result;
            arena->head = result;
        }
        arena->tail = result;

        // NOTE: Update stats
        arena->stats.syscalls += (commit_on_reserve ? 1 : 2);
        arena->stats.blocks   += 1;
        arena->stats.capacity += arena->curr->size;

        arena->stats.blocks_hwm   = DQN_MAX(arena->stats.blocks_hwm,   arena->stats.blocks);
        arena->stats.capacity_hwm = DQN_MAX(arena->stats.capacity_hwm, arena->stats.capacity);

        Dqn_Library_LeakTraceAdd(DQN_LEAK_TRACE_ARG result, size);
    }

    return result;
}

DQN_API void *Dqn_Arena_Allocate_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, Dqn_usize size, uint8_t align, Dqn_ZeroMem zero_mem)
{
    DQN_ASSERTF((align & (align - 1)) == 0, "Power of two alignment required");
    align = DQN_MAX(align, 1);

    if (arena->use_after_free_guard) {
        size  = Dqn_PowerOfTwoAlign(size, DQN_VMEM_PAGE_GRANULARITY);
        align = 1;
    }

    void *result = nullptr;
    for (; !result;) {
        while (arena->curr && (arena->curr->flags & Dqn_ArenaBlockFlags_Private))
            arena->curr = arena->curr->next;

        result = Dqn_Arena_AllocateFromBlock(arena->curr, size, align, zero_mem);
        if (!result) {
            if (!arena->curr || arena->curr == arena->tail) {
                Dqn_usize allocation_size = size + (align - 1);
                if (!Dqn_Arena_Grow(DQN_LEAK_TRACE_ARG arena, allocation_size, allocation_size /*commit*/, 0 /*flags*/)) {
                    break;
                }
            }

            if (arena->curr != arena->tail)
                arena->curr = arena->curr->next;
        }
    }

    if (result)
        DQN_ASSERT((arena->curr->flags & Dqn_ArenaBlockFlags_Private) == 0);

    return result;
}

DQN_API void *Dqn_Arena_Copy_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, void *src, Dqn_usize size, uint8_t align)
{
    void *result = Dqn_Arena_Allocate_(DQN_LEAK_TRACE_ARG arena, size, align, Dqn_ZeroMem_No);
    DQN_MEMCPY(result, src, size);
    return result;
}

DQN_API void *Dqn_Arena_CopyZ_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, void *src, Dqn_usize size, uint8_t align)
{
    void *result = Dqn_Arena_Allocate_(DQN_LEAK_TRACE_ARG arena, size + 1, align, Dqn_ZeroMem_No);
    DQN_MEMCPY(result, src, size);
    (DQN_CAST(char *)result)[size] = 0;
    return result;
}

DQN_API void Dqn_Arena_Free_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, Dqn_ZeroMem zero_mem)
{
    if (!arena)
        return;

    if (!DQN_CHECKF(arena->temp_memory_count == 0, "You cannot free an arena whilst in an temp memory region"))
        return;

    while (arena->tail) {
        Dqn_ArenaBlock *block               = arena->tail;
        arena->tail                         = block->prev;
        Dqn_ArenaBlockResetInfo_ reset_info = {};
        reset_info.free_memory              = true;
        Dqn_Arena_BlockReset_(DQN_LEAK_TRACE_ARG block, zero_mem, reset_info);
    }

    arena->curr           = arena->tail = nullptr;
    arena->stats.capacity = 0;
    arena->stats.used     = 0;
    arena->stats.wasted   = 0;
    arena->stats.blocks   = 0;
}

// =================================================================================================
// [$ACAT] Dqn_ArenaCatalog     |                             | Collate, create & manage arenas in a catalog
// =================================================================================================
DQN_API void Dqn_ArenaCatalog_Init(Dqn_ArenaCatalog *catalog, Dqn_Arena *arena)
{
    catalog->arena         = arena;
    catalog->sentinel.next = &catalog->sentinel;
    catalog->sentinel.prev = &catalog->sentinel;
}

DQN_API void Dqn_ArenaCatalog_Add(Dqn_ArenaCatalog *catalog, Dqn_Arena *arena)
{
    // NOTE: We could use an atomic for appending to the sentinel but it is such
    // a rare operation to append to the catalog that we don't bother.
    Dqn_TicketMutex_Begin(&catalog->ticket_mutex);

    // NOTE: Create item in the catalog
    Dqn_ArenaCatalogItem *result = Dqn_Arena_New(catalog->arena, Dqn_ArenaCatalogItem, Dqn_ZeroMem_Yes);
    result->arena                = arena;

    // NOTE: Add to the catalog (linked list)
    Dqn_ArenaCatalogItem *sentinel = &catalog->sentinel;
    result->next                   = sentinel;
    result->prev                   = sentinel->prev;
    result->next->prev             = result;
    result->prev->next             = result;
    Dqn_TicketMutex_End(&catalog->ticket_mutex);

    Dqn_Atomic_AddU32(&catalog->arena_count, 1);
}

DQN_API Dqn_Arena *Dqn_ArenaCatalog_Alloc(Dqn_ArenaCatalog *catalog, Dqn_usize byte_size, Dqn_usize commit)
{
    Dqn_TicketMutex_Begin(&catalog->ticket_mutex);
    Dqn_Arena *result = Dqn_Arena_New(catalog->arena, Dqn_Arena, Dqn_ZeroMem_Yes);
    Dqn_TicketMutex_End(&catalog->ticket_mutex);

    Dqn_Arena_Grow(result, byte_size, commit, 0 /*flags*/);
    Dqn_ArenaCatalog_Add(catalog, result);
    return result;
}

DQN_API Dqn_Arena *Dqn_ArenaCatalog_AllocFV(Dqn_ArenaCatalog *catalog, Dqn_usize byte_size, Dqn_usize commit, char const *fmt, va_list args)
{
    Dqn_Arena *result = Dqn_ArenaCatalog_Alloc(catalog, byte_size, commit);
    result->label     = Dqn_String8_InitFV(Dqn_Arena_Allocator(result), fmt, args);
    return result;
}

DQN_API Dqn_Arena *Dqn_ArenaCatalog_AllocF(Dqn_ArenaCatalog *catalog, Dqn_usize byte_size, Dqn_usize commit, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Arena *result = Dqn_ArenaCatalog_AllocFV(catalog, byte_size, commit, fmt, args);
    va_end(args);
    return result;
}

