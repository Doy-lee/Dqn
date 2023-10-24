// NOTE: [$ASAN] Dqn_Asan ========================================================================== ===
DQN_API void Dqn_ASAN_PoisonMemoryRegion(void const volatile *ptr, Dqn_usize size)
{
    #if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
    __asan_poison_memory_region(ptr, size);
    if (DQN_ASAN_VET_POISON) {
        DQN_HARD_ASSERT(__asan_address_is_poisoned(ptr));
        DQN_HARD_ASSERT(__asan_address_is_poisoned((char *)ptr + (size - 1)));
    }
    #else
    (void)ptr; (void)size;
    #endif
}

DQN_API void Dqn_ASAN_UnpoisonMemoryRegion(void const volatile *ptr, Dqn_usize size)
{
    #if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
    __asan_unpoison_memory_region(ptr, size);
    if (DQN_ASAN_VET_POISON) {
        DQN_HARD_ASSERT(__asan_region_is_poisoned((void *)ptr, size) == 0);
    }
    #else
    (void)ptr; (void)size;
    #endif
}

DQN_API Dqn_StackTraceWalkResult Dqn_StackTrace_Walk(Dqn_Arena *arena, uint16_t limit)
{
    Dqn_StackTraceWalkResult result = {};
    #if defined(DQN_OS_WIN32)
    if (!arena)
        return result;

    static Dqn_TicketMutex mutex = {};
    Dqn_TicketMutex_Begin(&mutex);

    HANDLE thread  = GetCurrentThread();
    result.process = GetCurrentProcess();

    for (static bool init = false; !init; init = true) {
        SymSetOptions(SYMOPT_LOAD_LINES);
        if (!SymInitialize(result.process, nullptr /*UserSearchPath*/, true /*fInvadeProcess*/)) {
            Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
            Dqn_WinError error        = Dqn_Win_LastError(scratch.arena);
            Dqn_Log_ErrorF("SymInitialize failed, stack trace can not be generated (%lu): %.*s\n", error.code, DQN_STR_FMT(error.msg));
        }
    }

    CONTEXT context;
    RtlCaptureContext(&context);

    STACKFRAME64 frame     = {};
    frame.AddrPC.Offset    = context.Rip;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrFrame.Offset = context.Rbp;
    frame.AddrFrame.Mode   = AddrModeFlat;
    frame.AddrStack.Offset = context.Rsp;
    frame.AddrStack.Mode   = AddrModeFlat;

    Dqn_ThreadScratch scratch     = Dqn_Thread_GetScratch(arena);
    Dqn_List<uint64_t> raw_frames = Dqn_List_Init<uint64_t>(scratch.arena, 32 /*chunk size*/);

    while (raw_frames.count < limit) {
        if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64,
                         result.process,
                         thread,
                         &frame,
                         &context,
                         nullptr /*ReadMemoryRoutine*/,
                         SymFunctionTableAccess64,
                         SymGetModuleBase64,
                         nullptr /*TranslateAddress*/)) {
            break;
        }

        // NOTE: It might be useful one day to use frame.AddrReturn.Offset.
        // If AddrPC.Offset == AddrReturn.Offset then we can detect recursion.
        Dqn_List_Add(&raw_frames, frame.AddrPC.Offset);
    }
    Dqn_TicketMutex_End(&mutex);

    result.base_addr = Dqn_Arena_NewArray(arena, uint64_t, raw_frames.count, Dqn_ZeroMem_No);
    for (Dqn_ListChunk<uint64_t> *chunk = raw_frames.head; chunk; chunk = chunk->next) {
        DQN_MEMCPY(result.base_addr + result.size, chunk->data, sizeof(*chunk->data) * chunk->count);
        result.size += DQN_CAST(uint16_t)chunk->count;
    }
    #else
    (void)limit; (void)arena;
    #endif
    return result;
}

DQN_API bool Dqn_StackTrace_WalkResultIterate(Dqn_StackTraceWalkResultIterator *it, Dqn_StackTraceWalkResult *walk)
{
    bool result = false;
    if (!it || !walk || !walk->base_addr || !walk->process)
        return result;

    if (it->index >= walk->size)
        return false;

    result                  = true;
    it->raw_frame.process   = walk->process;
    it->raw_frame.base_addr = walk->base_addr[it->index++];
    return result;
}

DQN_API Dqn_StackTraceFrame Dqn_StackTrace_RawFrameToFrame(Dqn_Arena *arena, Dqn_StackTraceRawFrame raw_frame)
{
    #if defined(DQN_OS_WIN32)
    // NOTE: Get line+filename =====================================================================

    // TODO: Why does zero-initialising this with `line = {};` cause
    // SymGetLineFromAddr64 function to fail once we are at
    // __scrt_commain_main_seh and hit BaseThreadInitThunk frame? The
    // line and file number are still valid in the result which we use, so,
    // we silently ignore this error.
    IMAGEHLP_LINEW64 line;
    line.SizeOfStruct       = sizeof(line);
    DWORD line_displacement = 0;
    SymGetLineFromAddrW64(raw_frame.process, raw_frame.base_addr, &line_displacement, &line);

    // NOTE: Get function name =====================================================================

    alignas(SYMBOL_INFOW) char buffer[sizeof(SYMBOL_INFOW) + (MAX_SYM_NAME * sizeof(wchar_t))] = {};
    SYMBOL_INFOW *symbol = DQN_CAST(SYMBOL_INFOW *)buffer;
    symbol->SizeOfStruct = sizeof(*symbol);
    symbol->MaxNameLen   = sizeof(buffer) - sizeof(*symbol);

    uint64_t symbol_displacement = 0; // Offset to the beginning of the symbol to the address
    SymFromAddrW(raw_frame.process, raw_frame.base_addr, &symbol_displacement, symbol);

    // NOTE: Construct result ======================================================================

    Dqn_Str16 file_name16     = Dqn_Str16{line.FileName, Dqn_CStr16_Size(line.FileName)};
    Dqn_Str16 function_name16 = Dqn_Str16{symbol->Name, symbol->NameLen};

    Dqn_StackTraceFrame result = {};
    result.address             = raw_frame.base_addr;
    result.line_number         = line.LineNumber;
    result.file_name           = Dqn_Win_Str16ToStr8(arena, file_name16);
    result.function_name       = Dqn_Win_Str16ToStr8(arena, function_name16);
    #else
    Dqn_StackTraceFrame result = {};
    #endif
    return result;
}

DQN_API Dqn_Slice<Dqn_StackTraceFrame> Dqn_StackTrace_GetFrames(Dqn_Arena *arena, uint16_t limit)
{
    Dqn_Slice<Dqn_StackTraceFrame> result = {};
    Dqn_ThreadScratch scratch             = Dqn_Thread_GetScratch(arena);
    Dqn_StackTraceWalkResult walk         = Dqn_StackTrace_Walk(scratch.arena, limit);

    if (!walk.size)
        return result;

    Dqn_usize slice_index = 0;
    result                = Dqn_Slice_Alloc<Dqn_StackTraceFrame>(arena, walk.size, Dqn_ZeroMem_No);
    for (Dqn_StackTraceWalkResultIterator it = {}; Dqn_StackTrace_WalkResultIterate(&it, &walk); ) {
        result.data[slice_index++] = Dqn_StackTrace_RawFrameToFrame(arena, it.raw_frame);
    }
    return result;
}

DQN_API void Dqn_StackTrace_Print(uint16_t limit)
{
    Dqn_ThreadScratch scratch                  = Dqn_Thread_GetScratch(nullptr);
    Dqn_Slice<Dqn_StackTraceFrame> stack_trace = Dqn_StackTrace_GetFrames(scratch.arena, limit);
    for (Dqn_StackTraceFrame& frame : stack_trace)
        Dqn_Print_ErrLnF("%.*s(%I64u): %.*s", DQN_STR_FMT(frame.file_name), frame.line_number, DQN_STR_FMT(frame.function_name));
}

// NOTE: [$DEBG] Dqn_Debug =========================================================================
#if defined(DQN_LEAK_TRACING)
DQN_API void Dqn_Debug_TrackAlloc_(Dqn_Str8 stack_trace, void *ptr, Dqn_usize size, bool leak_permitted)
{
    if (!ptr)
        return;

    if (g_dqn_library->alloc_tracking_disabled)
        return;

    // NOTE: In this function we can create alloc records and hence it's
    // possible for the alloc table to resize. This can cause a nested call
    // into the tracking alloc function and dead-lock. We don't
    // care about tracking these alloc records for the alloc table itself so we 
    // disable alloc tracking for the duration of this function.
    // TODO(doyle): @robust This is not thread safe.
    Dqn_TicketMutex_Begin(&g_dqn_library->alloc_table_mutex);
    g_dqn_library->alloc_tracking_disabled = true;

    DQN_DEFER {
        g_dqn_library->alloc_tracking_disabled = false;
        Dqn_TicketMutex_End(&g_dqn_library->alloc_table_mutex);
    };

    // NOTE: If the entry was not added, we are reusing a pointer that has been freed.
    // TODO: Add API for always making the item but exposing a var to indicate if the item was newly created or it already existed.
    Dqn_DSMap<Dqn_AllocRecord> *alloc_table = &g_dqn_library->alloc_table;
    Dqn_DSMapKey key                        = Dqn_DSMap_KeyU64(alloc_table, DQN_CAST(uintptr_t)ptr);
    Dqn_AllocRecord *alloc                  = Dqn_DSMap_Find(alloc_table, key);
    if (alloc) {
        if ((alloc->flags & Dqn_AllocRecordFlag_Freed) == 0) {
            Dqn_Str8 alloc_stack_trace       = Dqn_Str8_Init(alloc->stack_trace, alloc->stack_trace_size);
            Dqn_Str8 alloc_clean_stack_trace = Dqn_Str8_Slice(alloc_stack_trace, g_dqn_library->stack_trace_offset_to_our_call_stack, alloc_stack_trace.size);
            Dqn_Str8 clean_stack_trace       = Dqn_Str8_Slice(stack_trace,       g_dqn_library->stack_trace_offset_to_our_call_stack, stack_trace.size);
            DQN_HARD_ASSERTF(
                alloc->flags & Dqn_AllocRecordFlag_Freed,
                "\n\nThis pointer is already in the leak tracker, however it has not "
                "been freed yet. This same pointer is being ask to be tracked "
                "twice in the allocation table, e.g. one if its previous free "
                "calls has not being marked freed with an equivalent call to "
                "Dqn_Debug_TrackDealloc()\n"
                "\n"
                "The pointer (0x%p) originally allocated %_$$zu at:\n"
                "\n"
                "%.*s"
                "\n"
                "The pointer is being allocated again at:\n"
                "%.*s"
                ,
                ptr, alloc->size,
                DQN_STR_FMT(alloc_clean_stack_trace),
                DQN_STR_FMT(clean_stack_trace));
        }

        // NOTE: Pointer was reused, clean up the prior entry
        free(alloc->stack_trace);
        free(alloc->freed_stack_trace);
        *alloc = {};
    } else {
        alloc = Dqn_DSMap_Make(alloc_table, key, /*found*/ nullptr);
    }

    alloc->ptr              = ptr;
    alloc->size             = size;
    alloc->stack_trace      = stack_trace.data;
    alloc->stack_trace_size = DQN_CAST(uint16_t)stack_trace.size;

    // TODO(doyle): @robust The global flag is not multi-thread safe
    if (leak_permitted || g_dqn_library->alloc_is_allowed_to_leak)
        alloc->flags |= Dqn_AllocRecordFlag_LeakPermitted;
}

DQN_API void Dqn_Debug_TrackDealloc_(Dqn_Str8 stack_trace, void *ptr)
{
    if (!ptr || g_dqn_library->alloc_tracking_disabled)
        return;

    Dqn_TicketMutex_Begin(&g_dqn_library->alloc_table_mutex);
    DQN_DEFER { Dqn_TicketMutex_End(&g_dqn_library->alloc_table_mutex); };

    Dqn_DSMap<Dqn_AllocRecord> *alloc_table = &g_dqn_library->alloc_table;
    Dqn_DSMapKey key                        = Dqn_DSMap_KeyU64(alloc_table, DQN_CAST(uintptr_t)ptr);
    Dqn_AllocRecord *alloc                  = Dqn_DSMap_Find(alloc_table, key);

    DQN_HARD_ASSERTF(alloc, "Allocated pointer can not be removed as it does not exist in the "
                            "allocation table. When this memory was allocated, the pointer was "
                            "not added to the allocation table [ptr=%p]",
                            ptr);

    if (alloc->flags & Dqn_AllocRecordFlag_Freed) {
        Dqn_Str8 alloc_stack_trace       = Dqn_Str8_Init(alloc->stack_trace, alloc->stack_trace_size);
        Dqn_Str8 alloc_clean_stack_trace = Dqn_Str8_Slice(alloc_stack_trace, g_dqn_library->stack_trace_offset_to_our_call_stack, alloc_stack_trace.size);

        Dqn_Str8 alloc_freed_stack_trace       = Dqn_Str8_Init(alloc->freed_stack_trace, alloc->freed_stack_trace_size);
        Dqn_Str8 alloc_freed_clean_stack_trace = Dqn_Str8_Slice(alloc_freed_stack_trace, g_dqn_library->stack_trace_offset_to_our_call_stack, alloc_freed_stack_trace.size);

        Dqn_Str8 dealloc_stack_trace           = Dqn_Str8_Slice(stack_trace, g_dqn_library->stack_trace_offset_to_our_call_stack, stack_trace.size);

        DQN_HARD_ASSERTF((alloc->flags & Dqn_AllocRecordFlag_Freed) == 0,
                         "\n\nDouble free detected, pointer to free was already marked "
                         " as freed. Either the pointer was reallocated but not"
                         " traced, or, the pointer was freed twice.\n"
                         "\n"
                         "The pointer (0x%p) originally allocated %_$$zu at:\n"
                         "\n"
                         "%.*s"
                         "\n"
                         "The pointer was freed at:\n"
                         "\n"
                         "%.*s"
                         "\n"
                         "The pointer is being freed again at:\n"
                         "%.*s"
                         ,
                         ptr, alloc->freed_size,
                         DQN_STR_FMT(alloc_clean_stack_trace),
                         DQN_STR_FMT(alloc_freed_clean_stack_trace),
                         DQN_STR_FMT(dealloc_stack_trace));
    }

    alloc->flags                  |= Dqn_AllocRecordFlag_Freed;
    alloc->freed_size              = alloc->size;
    alloc->freed_stack_trace       = stack_trace.data;
    alloc->freed_stack_trace_size  = DQN_CAST(uint16_t)stack_trace.size;
}

DQN_API void Dqn_Debug_DumpLeaks()
{
    uint64_t leak_count   = 0;
    uint64_t leaked_bytes = 0;
    for (Dqn_usize index = 1; index < g_dqn_library->alloc_table.occupied; index++) {
        Dqn_DSMapSlot<Dqn_AllocRecord> *slot = g_dqn_library->alloc_table.slots + index;
        Dqn_AllocRecord *alloc               = &slot->value;
        bool alloc_leaked                    = (alloc->flags & Dqn_AllocRecordFlag_Freed) == 0;
        bool leak_permitted                  = (alloc->flags & Dqn_AllocRecordFlag_LeakPermitted);
        if (alloc_leaked && !leak_permitted) {
            leaked_bytes += alloc->size;
            leak_count++;

            Dqn_Str8 stack_trace       = Dqn_Str8_Init(alloc->stack_trace, alloc->stack_trace_size);
            Dqn_Str8 clean_stack_trace = Dqn_Str8_Slice(stack_trace, g_dqn_library->stack_trace_offset_to_our_call_stack, stack_trace.size);
            Dqn_Log_WarningF("Pointer (0x%p) leaked %_$$zu at:\n"
                             "%.*s",
                             alloc->ptr, alloc->size,
                             DQN_STR_FMT(clean_stack_trace));
        }
    }

    if (leak_count) {
        Dqn_Log_WarningF("There were %I64u leaked allocations totalling %_$$I64u", leak_count, leaked_bytes);
    }
}
#endif // defined(DQN_LEAK_TRACING)
