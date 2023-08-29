// NOTE: [$ALLO] Dqn_Allocator =====================================================================
DQN_API void *Dqn_Allocator_Alloc(Dqn_Allocator allocator, size_t size, uint8_t align, Dqn_ZeroMem zero_mem)
{
    void *result = NULL;
    if (allocator.alloc) {
        result = allocator.alloc(size, align, zero_mem, allocator.user_context);
    } else {
        result = DQN_ALLOC(size);
    }
    return result;
}

DQN_API void Dqn_Allocator_Dealloc(Dqn_Allocator allocator, void *ptr, size_t size)
{
    if (allocator.dealloc) {
        allocator.dealloc(ptr, size, allocator.user_context);
    } else {
        DQN_DEALLOC(ptr, size);
    }
}

// NOTE: [$VMEM] Dqn_VMem ==========================================================================
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

    Dqn_Debug_TrackAlloc(result, size, (page_flags & Dqn_VMemPage_AllocRecordLeakPermitted));
    return result;
}

DQN_API bool Dqn_VMem_Commit(void *ptr, Dqn_usize size, uint32_t page_flags)
{
    bool result = false;
    if (!ptr || size == 0)
        return false;

    unsigned long os_page_flags = Dqn_VMem_ConvertPageToOSFlags_(page_flags);
    #if defined(DQN_OS_WIN32)
    result = VirtualAlloc(ptr, size, MEM_COMMIT, os_page_flags) != nullptr;
    #elif defined(DQN_OS_UNIX)
    result = mprotect(ptr, size, os_page_flags) == 0;
    #else
        #error "Missing implementation for Dqn_VMem_Commit"
    #endif
    return result;
}

DQN_API void Dqn_VMem_Decommit(void *ptr, Dqn_usize size)
{
    #if defined(DQN_OS_WIN32)

    // NOTE: This is a decommit call, which is explicitly saying to free the
    // pages but not the VADs, you would use VMem_Release to release everything.
    DQN_MSVC_WARNING_PUSH
    DQN_MSVC_WARNING_DISABLE(6250) // Calling 'VirtualFree' without the MEM_RELEASE flag might free memory but not address descriptors (VADs).  This causes address space leaks.
    VirtualFree(ptr, size, MEM_DECOMMIT);
    DQN_MSVC_WARNING_POP

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
    Dqn_Debug_TrackDealloc(ptr);
}

DQN_API int Dqn_VMem_Protect(void *ptr, Dqn_usize size, uint32_t page_flags)
{
    if (!ptr || size == 0)
        return 0;

    static Dqn_String8 const ALIGNMENT_ERROR_MSG =
        DQN_STRING8("Page protection requires pointers to be page aligned because we "
                    "can only guard memory at a multiple of the page boundary.");
    DQN_ASSERTF(Dqn_IsPowerOfTwoAligned(DQN_CAST(uintptr_t)ptr, g_dqn_library->os_page_size), "%s", ALIGNMENT_ERROR_MSG.data);
    DQN_ASSERTF(Dqn_IsPowerOfTwoAligned(size,                   g_dqn_library->os_page_size), "%s", ALIGNMENT_ERROR_MSG.data);

    unsigned long os_page_flags = Dqn_VMem_ConvertPageToOSFlags_(page_flags);
    #if defined(DQN_OS_WIN32)
    unsigned long prev_flags = 0;
    int result = VirtualProtect(ptr, size, os_page_flags, &prev_flags);
    (void)prev_flags;
    if (result == 0) {
        #if defined(DQN_NO_WIN)
        DQN_ASSERTF(result, "VirtualProtect failed");
        #else
        Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
        Dqn_WinError error        = Dqn_Win_LastError(scratch.arena);
        DQN_ASSERTF(result, "VirtualProtect failed (%d): %.*s", error.code, DQN_STRING_FMT(error.msg));
        #endif
    }
    #else
    int result = mprotect(result->memory, result->size, os_page_flags);
    DQN_ASSERTF(result == 0, "mprotect failed (%d)", errno);
    #endif

    return result;
}

// NOTE: [$MEMB] Dqn_MemBlock ======================================================================
DQN_API Dqn_MemBlockSizeRequiredResult Dqn_MemBlock_SizeRequired(Dqn_MemBlock const *block, Dqn_usize size, uint8_t alignment, uint32_t flags)
{
    DQN_ASSERT(alignment > 0 && Dqn_IsPowerOfTwo(alignment));

    Dqn_MemBlockSizeRequiredResult result = {};
    result.alloc_size                     = size;
    Dqn_MemBlockFlag block_flags          = DQN_CAST(Dqn_MemBlockFlag)((block ? block->flags : 0) | flags);
    uint8_t ptr_alignment                 = alignment;

    if (DQN_ASAN_POISON) {
        // NOTE: Guard a page after with poisoned memory. The first allocation
        // is always guarded with poison-ed memory to prevent read/writes behind
        // the block of memory.
        if ((block_flags & Dqn_MemBlockFlag_AllocsAreContiguous) == 0) {
            result.alloc_size = Dqn_AlignUpPowerOfTwo(size + DQN_ASAN_POISON_GUARD_SIZE, DQN_ASAN_POISON_ALIGNMENT);
        }
        ptr_alignment = DQN_MAX(alignment, DQN_ASAN_POISON_ALIGNMENT);
    }

    if (block) {
        uintptr_t address      = DQN_CAST(uintptr_t)block->data + block->used;
        uintptr_t next_address = Dqn_AlignUpPowerOfTwo(address, ptr_alignment);
        result.data_offset     = next_address - DQN_CAST(uintptr_t)block->data;
        Dqn_usize new_used     = result.data_offset + result.alloc_size;
        result.block_size      = new_used - block->used;
    } else {
        result.block_size      = result.alloc_size + (ptr_alignment - 1);
    }

    return result;
}

Dqn_usize Dqn_MemBlock_MetadataSize()
{
    Dqn_usize init_poison_page = DQN_ASAN_POISON ? DQN_ASAN_POISON_GUARD_SIZE : 0;
    Dqn_usize poison_alignment = DQN_ASAN_POISON ? DQN_ASAN_POISON_ALIGNMENT  : 0;
    Dqn_usize result           = Dqn_AlignUpPowerOfTwo(sizeof(Dqn_MemBlock), poison_alignment) + init_poison_page;
    return result;
}

DQN_API Dqn_MemBlock *Dqn_MemBlock_Init(Dqn_usize reserve, Dqn_usize commit, uint32_t flags)
{
    DQN_ASSERTF(g_dqn_library->os_page_size,                   "Library needs to be initialised by calling Dqn_Library_Init()");
    DQN_ASSERTF(Dqn_IsPowerOfTwo(g_dqn_library->os_page_size), "Invalid page size");
    DQN_ASSERTF((flags & ~Dqn_MemBlockFlag_All) == 0,          "Invalid flag combination, must adhere to Dqn_MemBlockFlags");

    if (reserve == 0)
        return nullptr;

    Dqn_usize metadata_size    = Dqn_MemBlock_MetadataSize();
    Dqn_usize reserve_aligned  = Dqn_AlignUpPowerOfTwo(metadata_size + reserve, g_dqn_library->os_page_size);
    Dqn_usize commit_aligned   = Dqn_AlignUpPowerOfTwo(metadata_size + commit,  g_dqn_library->os_page_size);
    commit_aligned             = DQN_MIN(commit_aligned, reserve_aligned);

    // NOTE: Avoid 1 syscall by committing on reserve if amounts are equal
    Dqn_VMemCommit commit_on_reserve = commit_aligned == reserve_aligned ? Dqn_VMemCommit_Yes : Dqn_VMemCommit_No;
    auto *result                     = DQN_CAST(Dqn_MemBlock *)Dqn_VMem_Reserve(reserve_aligned, commit_on_reserve, Dqn_VMemPage_ReadWrite);
    if (result) {
        // NOTE: Commit pages if we did not commit on the initial range.
        if (!commit_on_reserve)
            Dqn_VMem_Commit(result, commit_aligned, Dqn_VMemPage_ReadWrite);

        result->data   = DQN_CAST(uint8_t *)result + metadata_size;
        result->size   = reserve_aligned           - metadata_size;
        result->commit = commit_aligned            - metadata_size;
        result->flags  = DQN_CAST(uint8_t)flags;

        // NOTE: Poison (guard page + commit). We do *not* poison the entire
        // block, only the commit pages. Since we may reserve large amounts of
        // space vs commit we'd waste time marking those pages as poisoned as
        // reads or writes outside of committed pages will page fault.
        if (DQN_ASAN_POISON) {
            DQN_ASSERT(Dqn_IsPowerOfTwoAligned(result->data, DQN_ASAN_POISON_ALIGNMENT));
            DQN_ASSERT(Dqn_IsPowerOfTwoAligned(result->size, DQN_ASAN_POISON_ALIGNMENT));
            void *poison_ptr          = DQN_CAST(void *)Dqn_AlignUpPowerOfTwo(DQN_CAST(char *)result + sizeof(Dqn_MemBlock), DQN_ASAN_POISON_ALIGNMENT);
            Dqn_usize bytes_to_poison = DQN_ASAN_POISON_GUARD_SIZE + result->commit;
            Dqn_ASAN_PoisonMemoryRegion(poison_ptr, bytes_to_poison);
        }
    }
    return result;
}

DQN_API void *Dqn_MemBlock_Alloc(Dqn_MemBlock *block, Dqn_usize size, uint8_t alignment, Dqn_ZeroMem zero_mem)
{
    DQN_ASSERT(zero_mem == Dqn_ZeroMem_Yes || zero_mem == Dqn_ZeroMem_No);

    void *result = nullptr;
    if (!block)
        return result;

    Dqn_MemBlockSizeRequiredResult size_required = Dqn_MemBlock_SizeRequired(block, size, alignment, Dqn_MemBlockFlag_Nil);
    Dqn_usize new_used = size_required.data_offset + size_required.alloc_size;
    if (new_used > block->size)
        return result;

    result      = DQN_CAST(char *)block->data + size_required.data_offset;
    block->used = new_used;
    DQN_ASSERT(Dqn_IsPowerOfTwoAligned(result, alignment));

    if (DQN_ASAN_POISON)
        Dqn_ASAN_UnpoisonMemoryRegion(result, size);

    if (zero_mem == Dqn_ZeroMem_Yes) {
        Dqn_usize reused_bytes = DQN_MIN(block->commit - size_required.data_offset, size);
        DQN_MEMSET(result, DQN_MEMSET_BYTE, reused_bytes);
    }

    if (block->commit < block->used) {
        Dqn_usize commit_size = Dqn_AlignUpPowerOfTwo(block->used - block->commit, g_dqn_library->os_page_size);
        void *commit_ptr      = (void *)Dqn_AlignUpPowerOfTwo((char *)block->data + block->commit, g_dqn_library->os_page_size);
        block->commit        += commit_size;
        Dqn_VMem_Commit(commit_ptr, commit_size, Dqn_VMemPage_ReadWrite);
        DQN_ASSERT(block->commit <= block->size);

        if (DQN_ASAN_POISON) { // NOTE: Poison newly committed pages that aren't being used.
            void *poison_ptr          = DQN_CAST(char *)block->data + block->used;
            Dqn_usize bytes_to_poison = block->commit - block->used;
            Dqn_ASAN_PoisonMemoryRegion(poison_ptr, bytes_to_poison);
        }
    }

    return result;
}

DQN_API void Dqn_MemBlock_Free(Dqn_MemBlock *block)
{
    if (!block)
        return;
    Dqn_usize release_size = block->size + Dqn_MemBlock_MetadataSize();
    if (DQN_ASAN_POISON)
        Dqn_ASAN_UnpoisonMemoryRegion(block, release_size);
    Dqn_VMem_Release(block, release_size);
}

DQN_API void Dqn_MemBlock_Pop(Dqn_MemBlock *block, Dqn_usize size)
{
    if (!block)
        return;
    Dqn_usize size_adjusted = DQN_MIN(size, block->used);
    Dqn_usize to            = block->used - size_adjusted;
    Dqn_MemBlock_PopTo(block, to);
}

DQN_API void Dqn_MemBlock_PopTo(Dqn_MemBlock *block, Dqn_usize to)
{
    if (!block || to >= block->used)
        return;

    if (DQN_ASAN_POISON) {
        void *poison_ptr          = DQN_CAST(char *)block->data + to;
        void *end_ptr             = DQN_CAST(void *)Dqn_AlignUpPowerOfTwo((DQN_CAST(uintptr_t)block->data + block->used), DQN_ASAN_POISON_ALIGNMENT);
        uintptr_t bytes_to_poison = DQN_CAST(uintptr_t)end_ptr - DQN_CAST(uintptr_t)poison_ptr;
        Dqn_ASAN_PoisonMemoryRegion(poison_ptr, bytes_to_poison);
    }
    block->used = to;
}

// NOTE: [$AREN] Dqn_Arena =========================================================================
DQN_FILE_SCOPE void *Dqn_Arena_AllocatorAlloc(size_t size, uint8_t align, Dqn_ZeroMem zero_mem, void *user_context)
{
    void *result = NULL;
    if (!user_context)
        return result;

    Dqn_Arena *arena = DQN_CAST(Dqn_Arena *)user_context;
    result           = Dqn_Arena_Alloc(arena, size, align, zero_mem);
    return result;
}

DQN_FILE_SCOPE void Dqn_Arena_AllocatorDealloc(void *, size_t, void *)
{
    // NOTE: No-op, arenas batch allocate and batch deallocate. Call free on the
    // underlying arena, since we can't free individual pointers.
}

DQN_API Dqn_Allocator Dqn_Arena_Allocator(Dqn_Arena *arena)
{
    Dqn_Allocator result = {};
    if (arena) {
        result.user_context = arena;
        result.alloc        = Dqn_Arena_AllocatorAlloc;
        result.dealloc      = Dqn_Arena_AllocatorDealloc;
    }
    return result;
}

DQN_API Dqn_MemBlock *Dqn_Arena_Grow(Dqn_Arena *arena, Dqn_usize reserve, Dqn_usize commit, uint8_t flags)
{
    if (!arena)
        return nullptr;

    uint8_t mem_block_flags = flags;
    if (arena->allocs_are_allowed_to_leak)
        mem_block_flags |= Dqn_MemBlockFlag_AllocRecordLeakPermitted;

    Dqn_MemBlock *result = Dqn_MemBlock_Init(reserve, commit, mem_block_flags);
    if (result) {
        if (!arena->head)
            arena->head = result;

        if (arena->tail)
            arena->tail->next = result;

        if (!arena->curr)
            arena->curr = result;

        result->prev   = arena->tail;
        arena->tail    = result;
        arena->blocks += 1;
    }
    return result;
}

DQN_API void *Dqn_Arena_Alloc(Dqn_Arena *arena, Dqn_usize size, uint8_t align, Dqn_ZeroMem zero_mem)
{
    DQN_ASSERT(Dqn_IsPowerOfTwo(align));

    void *result = nullptr;
    if (!arena || size == 0 || align == 0)
        return result;

    for (;;) {
        while (arena->curr && (arena->curr->flags & Dqn_MemBlockFlag_ArenaPrivate))
            arena->curr = arena->curr->next;

        if (!arena->curr) {
            Dqn_MemBlockSizeRequiredResult size_required = Dqn_MemBlock_SizeRequired(nullptr, size, align, Dqn_MemBlockFlag_Nil);
            Dqn_usize block_size = size_required.block_size;
            if (!Dqn_Arena_Grow(arena, block_size, block_size, Dqn_MemBlockFlag_Nil))
                return result;
            DQN_ASSERT(arena->curr);
        }

        result = Dqn_MemBlock_Alloc(arena->curr, size, align, zero_mem);
        if (result)
            break;

        arena->curr = arena->curr->next;
    }

    return result;
}

DQN_API void *Dqn_Arena_Copy(Dqn_Arena *arena, void *src, Dqn_usize size, uint8_t align)
{
    void *result = Dqn_Arena_Alloc(arena, size, align, Dqn_ZeroMem_No);
    DQN_MEMCPY(result, src, size);
    return result;
}

DQN_API void *Dqn_Arena_CopyZ(Dqn_Arena *arena, void *src, Dqn_usize size, uint8_t align)
{
    void *result = Dqn_Arena_Alloc(arena, size + 1, align, Dqn_ZeroMem_No);
    DQN_MEMCPY(result, src, size);
    (DQN_CAST(char *)result)[size] = 0;
    return result;
}

DQN_API void Dqn_Arena_Free(Dqn_Arena *arena)
{
    if (!arena)
        return;

    for (Dqn_MemBlock *block = arena->head; block; ) {
        Dqn_MemBlock *next = block->next;
        Dqn_MemBlock_Free(block);
        block = next;
    }

    arena->curr   = arena->head = arena->tail = nullptr;
    arena->blocks = 0;
}

DQN_API Dqn_ArenaTempMemory Dqn_Arena_BeginTempMemory(Dqn_Arena *arena)
{
    Dqn_ArenaTempMemory result = {};
    if (arena) {
        result.arena     = arena;
        result.head      = arena->head;
        result.curr      = arena->curr;
        result.tail      = arena->tail;
        result.curr_used = (arena->curr) ? arena->curr->used : 0;
        result.blocks    = arena->blocks;
    }
    return result;
}

DQN_API void Dqn_Arena_EndTempMemory(Dqn_ArenaTempMemory temp_memory, bool cancel)
{
    if (cancel || !temp_memory.arena)
        return;

    // NOTE: The arena has been freed or invalidly manipulated (e.g. freed)
    // since the temp memory started as the head cannot change once it is
    // captured in the temp memory.
    Dqn_Arena *arena = temp_memory.arena;
    if (arena->head != temp_memory.head)
        return;

    // NOTE: Revert the current block to the temp_memory's current block
    arena->blocks = temp_memory.blocks;
    arena->head   = temp_memory.head;
    arena->curr   = temp_memory.curr;
    Dqn_MemBlock_PopTo(arena->curr, temp_memory.curr_used);

    // NOTE: Free the tail blocks until we reach the temp_memory's tail block
    while (arena->tail != temp_memory.tail) {
        Dqn_MemBlock *tail = arena->tail;
        arena->tail        = tail->prev;
        Dqn_MemBlock_Free(tail);
    }

    // NOTE: Chop the restored tail link
    if (arena->tail)
        arena->tail->next = nullptr;

    // NOTE: Reset the usage of all the blocks between the tail and current block's
    for (Dqn_MemBlock *block = arena->tail; block && (block != arena->curr); block = block->prev)
        Dqn_MemBlock_PopTo(block, 0);
}

Dqn_ArenaTempMemoryScope::Dqn_ArenaTempMemoryScope(Dqn_Arena *arena)
{
    temp_memory = Dqn_Arena_BeginTempMemory(arena);
}

Dqn_ArenaTempMemoryScope::~Dqn_ArenaTempMemoryScope()
{
    Dqn_Arena_EndTempMemory(temp_memory, cancel);
}

DQN_API Dqn_ArenaStatString Dqn_Arena_StatString(Dqn_ArenaStat const *stat)
{
    // NOTE: We use a non-standard format string that is only usable via
    // stb sprintf that GCC warns about as an error. This pragma mutes that.
    DQN_GCC_WARNING_PUSH
    DQN_GCC_WARNING_DISABLE("-Wformat-invalid-specifier")
    DQN_GCC_WARNING_DISABLE("-Wformat-extra-args")
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
    DQN_GCC_WARNING_POP
    return result;
}

// NOTE: [$ACAT] Dqn_ArenaCatalog ==================================================================
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

