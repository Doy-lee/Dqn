Dqn_Library dqn_library;

// =================================================================================================
// [$DLIB] Dqn_Library          |                             | Library run-time behaviour configuration
// =================================================================================================
DQN_API Dqn_Library *Dqn_Library_Init(Dqn_Arena *arena)
{
    Dqn_Library *result = &dqn_library;
    Dqn_TicketMutex_Begin(&result->lib_mutex);
    if (!result->lib_init) {
        Dqn_ArenaCatalog_Init(&result->arena_catalog, arena ? arena : &result->arena_catalog_backup_arena);
        result->lib_init = true;
    }
    Dqn_TicketMutex_End(&result->lib_mutex);
    return result;
}

DQN_API void Dqn_Library_SetLogCallback(Dqn_LogProc *proc, void *user_data)
{
    dqn_library.log_callback  = proc;
    dqn_library.log_user_data = user_data;
}

DQN_API void Dqn_Library_SetLogFile(FILE *file)
{
    Dqn_TicketMutex_Begin(&dqn_library.log_file_mutex);
    dqn_library.log_file    = file;
    dqn_library.log_to_file = file ? true : false;
    Dqn_TicketMutex_End(&dqn_library.log_file_mutex);
}

DQN_API void Dqn_Library_DumpThreadContextArenaStat(Dqn_String8 file_path)
{
    #if defined(DQN_DEBUG_THREAD_CONTEXT)
    // NOTE: Open a file to write the arena stats to
    FILE *file = nullptr;
    fopen_s(&file, file_path.data, "a+b");
    if (file) {
        Dqn_Log_ErrorF("Failed to dump thread context arenas [file=%.*s]", DQN_STRING_FMT(file_path));
        return;
    }

    // NOTE: Copy the stats from library book-keeping
    // NOTE: Extremely short critical section, copy the stats then do our
    // work on it.
    Dqn_ArenaStat stats[Dqn_CArray_CountI(dqn_library.thread_context_arena_stats)];
    int stats_size = 0;

    Dqn_TicketMutex_Begin(&dqn_library.thread_context_mutex);
    stats_size = dqn_library.thread_context_arena_stats_count;
    DQN_MEMCPY(stats, dqn_library.thread_context_arena_stats, sizeof(stats[0]) * stats_size);
    Dqn_TicketMutex_End(&dqn_library.thread_context_mutex);

    // NOTE: Print the cumulative stat
    Dqn_DateHMSTimeString now = Dqn_Date_HMSLocalTimeStringNow();
    fprintf(file,
            "Time=%.*s %.*s | Thread Context Arenas | Count=%d\n",
            now.date_size, now.date,
            now.hms_size, now.hms,
            dqn_library.thread_context_arena_stats_count);

    // NOTE: Write the cumulative thread arena data
    {
        Dqn_ArenaStat stat = {};
        for (Dqn_usize index = 0; index < stats_size; index++) {
            Dqn_ArenaStat const *current = stats + index;
            stat.capacity += current->capacity;
            stat.used     += current->used;
            stat.wasted   += current->wasted;
            stat.blocks   += current->blocks;

            stat.capacity_hwm = DQN_MAX(stat.capacity_hwm, current->capacity_hwm);
            stat.used_hwm     = DQN_MAX(stat.used_hwm, current->used_hwm);
            stat.wasted_hwm   = DQN_MAX(stat.wasted_hwm, current->wasted_hwm);
            stat.blocks_hwm   = DQN_MAX(stat.blocks_hwm, current->blocks_hwm);
        }

        Dqn_ArenaStatString stats_string = Dqn_Arena_StatString(&stat);
        fprintf(file, "  [ALL] CURR %.*s\n", stats_string.size, stats_string.data);
    }

    // NOTE: Print individual thread arena data
    for (Dqn_usize index = 0; index < stats_size; index++) {
        Dqn_ArenaStat const *current = stats + index;
        Dqn_ArenaStatString current_string = Dqn_Arena_StatString(current);
        fprintf(file, "  [%03d] CURR %.*s\n", DQN_CAST(int)index, current_string.size, current_string.data);
    }

    fclose(file);
    Dqn_Log_InfoF("Dumped thread context arenas [file=%.*s]", DQN_STRING_FMT(file_path));
    #else
    (void)file_path;
    #endif // #if defined(DQN_DEBUG_THREAD_CONTEXT)
}

#if defined(DQN_LEAK_TRACING)
DQN_API void Dqn_Library_LeakTraceAdd(Dqn_CallSite call_site, void *ptr, Dqn_usize size)
{
    if (!ptr)
        return;

    Dqn_TicketMutex_Begin(&dqn_library.alloc_table_mutex);
    if (!Dqn_DSMap_IsValid(&dqn_library.alloc_table))
        dqn_library.alloc_table = Dqn_DSMap_Init<Dqn_LeakTrace>(4096);

    // NOTE: If the entry was not added, we are reusing a pointer that has been freed.
    // TODO: Add API for always making the item but exposing a var to indicate if the item was newly created or it already existed.
    Dqn_LeakTrace *trace  = Dqn_DSMap_Find(&dqn_library.alloc_table, Dqn_DSMap_KeyU64(DQN_CAST(uintptr_t)ptr));
    if (trace) {
        DQN_HARD_ASSERTF(trace->freed, "This pointer is already in the leak tracker, however it"
                                          " has not been freed yet. Somehow this pointer has been"
                                          " given to the allocation table and has not being marked"
                                          " freed with an equivalent call to LeakTraceMarkFree()"
                                          " [ptr=%p, size=%_$$d, file=\"%.*s:%u\","
                                          " function=\"%.*s\"]",
                                          ptr,
                                          size,
                                          DQN_STRING_FMT(trace->call_site.file),
                                          trace->call_site.line,
                                          DQN_STRING_FMT(trace->call_site.function));
    } else {
        trace = Dqn_DSMap_Make(&dqn_library.alloc_table, Dqn_DSMap_KeyU64(DQN_CAST(uintptr_t)ptr));
    }

    trace->ptr       = ptr;
    trace->size      = size;
    trace->call_site = call_site;
    Dqn_TicketMutex_End(&dqn_library.alloc_table_mutex);
}

DQN_API void Dqn_Library_LeakTraceMarkFree(Dqn_CallSite call_site, void *ptr)
{
    if (!ptr)
        return;

    Dqn_TicketMutex_Begin(&dqn_library.alloc_table_mutex);

    Dqn_LeakTrace *trace = Dqn_DSMap_Find(&dqn_library.alloc_table, Dqn_DSMap_KeyU64(DQN_CAST(uintptr_t)ptr));
    DQN_HARD_ASSERTF(trace, "Allocated pointer can not be removed as it does not exist in the"
                              " allocation table. When this memory was allocated, the pointer was"
                              " not added to the allocation table [ptr=%p]",
                              ptr);

    DQN_HARD_ASSERTF(!trace->freed,
                        "Double free detected, pointer was previously allocated at [ptr=%p, %_$$d, file=\"%.*s:%u\", function=\"%.*s\"]",
                        ptr,
                        trace->size,
                        DQN_STRING_FMT(trace->call_site.file),
                        trace->call_site.line,
                        DQN_STRING_FMT(trace->call_site.function));

    trace->freed           = true;
    trace->freed_size      = trace->size;
    trace->freed_call_site = call_site;
    Dqn_TicketMutex_End(&dqn_library.alloc_table_mutex);
}
#endif /// defined(DQN_LEAK_TRACING)

// =================================================================================================
// [$BITS] Dqn_Bit              |                             | Bitset manipulation
// =================================================================================================
DQN_API void Dqn_Bit_UnsetInplace(uint64_t *flags, uint64_t bitfield)
{
    *flags = (*flags & ~bitfield);
}

DQN_API void Dqn_Bit_SetInplace(uint64_t *flags, uint64_t bitfield)
{
    *flags = (*flags | bitfield);
}

DQN_API bool Dqn_Bit_IsSet(uint64_t bits, uint64_t bits_to_set)
{
    auto result = DQN_CAST(bool)((bits & bits_to_set) == bits_to_set);
    return result;
}

DQN_API bool Dqn_Bit_IsNotSet(uint64_t bits, uint64_t bits_to_check)
{
    auto result = !Dqn_Bit_IsSet(bits, bits_to_check);
    return result;
}

// =================================================================================================
// [$SAFE] Dqn_Safe             |                             | Safe arithmetic, casts, asserts
// =================================================================================================
DQN_API int64_t Dqn_Safe_AddI64(int64_t a, int64_t b)
{
    int64_t result = DQN_CHECKF(a <= INT64_MAX - b, "a=%zd, b=%zd", a, b) ? (a + b) : INT64_MAX;
    return result;
}

DQN_API int64_t Dqn_Safe_MulI64(int64_t a, int64_t b)
{
    int64_t result = DQN_CHECKF(a <= INT64_MAX / b, "a=%zd, b=%zd", a, b) ? (a * b) : INT64_MAX;
    return result;
}

DQN_API uint64_t Dqn_Safe_AddU64(uint64_t a, uint64_t b)
{
    uint64_t result = DQN_CHECKF(a <= UINT64_MAX - b, "a=%zu, b=%zu", a, b) ? (a + b) : UINT64_MAX;
    return result;
}

DQN_API uint64_t Dqn_Safe_SubU64(uint64_t a, uint64_t b)
{
    uint64_t result = DQN_CHECKF(a >= b, "a=%zu, b=%zu", a, b) ? (a - b) : 0;
    return result;
}

DQN_API uint64_t Dqn_Safe_MulU64(uint64_t a, uint64_t b)
{
    uint64_t result = DQN_CHECKF(a <= UINT64_MAX / b, "a=%zu, b=%zu", a, b) ? (a * b) : UINT64_MAX;
    return result;
}

DQN_API uint32_t Dqn_Safe_SubU32(uint32_t a, uint32_t b)
{
    uint32_t result = DQN_CHECKF(a >= b, "a=%u, b=%u", a, b) ? (a - b) : 0;
    return result;
}

// NOTE: Dqn_Safe_SaturateCastUSizeToI*
// -----------------------------------------------------------------------------
// INT*_MAX literals will be promoted to the type of uintmax_t as uintmax_t is
// the highest possible rank (unsigned > signed).
DQN_API int Dqn_Safe_SaturateCastUSizeToInt(Dqn_usize val)
{
    int result = DQN_CHECK(DQN_CAST(uintmax_t)val <= INT_MAX) ? DQN_CAST(int)val : INT_MAX;
    return result;
}

DQN_API int8_t Dqn_Safe_SaturateCastUSizeToI8(Dqn_usize val)
{
    int8_t result = DQN_CHECK(DQN_CAST(uintmax_t)val <= INT8_MAX) ? DQN_CAST(int8_t)val : INT8_MAX;
    return result;
}

DQN_API int16_t Dqn_Safe_SaturateCastUSizeToI16(Dqn_usize val)
{
    int16_t result = DQN_CHECK(DQN_CAST(uintmax_t)val <= INT16_MAX) ? DQN_CAST(int16_t)val : INT16_MAX;
    return result;
}

DQN_API int32_t Dqn_Safe_SaturateCastUSizeToI32(Dqn_usize val)
{
    int32_t result = DQN_CHECK(DQN_CAST(uintmax_t)val <= INT32_MAX) ? DQN_CAST(int32_t)val : INT32_MAX;
    return result;
}

DQN_API int64_t Dqn_Safe_SaturateCastUSizeToI64(Dqn_usize val)
{
    int64_t result = DQN_CHECK(DQN_CAST(uintmax_t)val <= INT64_MAX) ? DQN_CAST(int64_t)val : INT64_MAX;
    return result;
}

// NOTE: Dqn_Safe_SaturateCastUSizeToU*
// -----------------------------------------------------------------------------
// Both operands are unsigned and the lowest rank operand will be promoted to
// match the highest rank operand.
DQN_API uint8_t Dqn_Safe_SaturateCastUSizeToU8(Dqn_usize val)
{
    uint8_t result = DQN_CHECK(val <= UINT8_MAX) ? DQN_CAST(uint8_t)val : UINT8_MAX;
    return result;
}

DQN_API uint16_t Dqn_Safe_SaturateCastUSizeToU16(Dqn_usize val)
{
    uint16_t result = DQN_CHECK(val <= UINT16_MAX) ? DQN_CAST(uint16_t)val : UINT16_MAX;
    return result;
}

DQN_API uint32_t Dqn_Safe_SaturateCastUSizeToU32(Dqn_usize val)
{
    uint32_t result = DQN_CHECK(val <= UINT32_MAX) ? DQN_CAST(uint32_t)val : UINT32_MAX;
    return result;
}

DQN_API uint64_t Dqn_Safe_SaturateCastUSizeToU64(Dqn_usize val)
{
    uint64_t result = DQN_CHECK(val <= UINT64_MAX) ? DQN_CAST(uint64_t)val : UINT64_MAX;
    return result;
}

// NOTE: Dqn_Safe_SaturateCastU64ToU*
// -----------------------------------------------------------------------------
// Both operands are unsigned and the lowest rank operand will be promoted to
// match the highest rank operand.
DQN_API unsigned int Dqn_Safe_SaturateCastU64ToUInt(uint64_t val)
{
    unsigned int result = DQN_CHECK(val <= UINT8_MAX) ? DQN_CAST(unsigned int)val : UINT_MAX;
    return result;
}

DQN_API uint8_t Dqn_Safe_SaturateCastU64ToU8(uint64_t val)
{
    uint8_t result = DQN_CHECK(val <= UINT8_MAX) ? DQN_CAST(uint8_t)val : UINT8_MAX;
    return result;
}

DQN_API uint16_t Dqn_Safe_SaturateCastU64ToU16(uint64_t val)
{
    uint16_t result = DQN_CHECK(val <= UINT16_MAX) ? DQN_CAST(uint16_t)val : UINT16_MAX;
    return result;
}

DQN_API uint32_t Dqn_Safe_SaturateCastU64ToU32(uint64_t val)
{
    uint32_t result = DQN_CHECK(val <= UINT32_MAX) ? DQN_CAST(uint32_t)val : UINT32_MAX;
    return result;
}


// NOTE: Dqn_Safe_SaturateCastISizeToI*
// -----------------------------------------------------------------------------
// Both operands are signed so the lowest rank operand will be promoted to
// match the highest rank operand.
DQN_API int Dqn_Safe_SaturateCastISizeToInt(Dqn_isize val)
{
    DQN_ASSERT(val >= INT_MIN && val <= INT_MAX);
    int result = DQN_CAST(int)DQN_CLAMP(val, INT_MIN, INT_MAX);
    return result;
}

DQN_API int8_t Dqn_Safe_SaturateCastISizeToI8(Dqn_isize val)
{
    DQN_ASSERT(val >= INT8_MIN && val <= INT8_MAX);
    int8_t result = DQN_CAST(int8_t)DQN_CLAMP(val, INT8_MIN, INT8_MAX);
    return result;
}

DQN_API int16_t Dqn_Safe_SaturateCastISizeToI16(Dqn_isize val)
{
    DQN_ASSERT(val >= INT16_MIN && val <= INT16_MAX);
    int16_t result = DQN_CAST(int16_t)DQN_CLAMP(val, INT16_MIN, INT16_MAX);
    return result;
}

DQN_API int32_t Dqn_Safe_SaturateCastISizeToI32(Dqn_isize val)
{
    DQN_ASSERT(val >= INT32_MIN && val <= INT32_MAX);
    int32_t result = DQN_CAST(int32_t)DQN_CLAMP(val, INT32_MIN, INT32_MAX);
    return result;
}

DQN_API int64_t Dqn_Safe_SaturateCastISizeToI64(Dqn_isize val)
{
    DQN_ASSERT(val >= INT64_MIN && val <= INT64_MAX);
    int64_t result = DQN_CAST(int64_t)DQN_CLAMP(val, INT64_MIN, INT64_MAX);
    return result;
}

// NOTE: Dqn_Safe_SaturateCastISizeToU*
// -----------------------------------------------------------------------------
// If the value is a negative integer, we clamp to 0. Otherwise, we know that
// the value is >=0, we can upcast safely to bounds check against the maximum
// allowed value.
DQN_API unsigned int Dqn_Safe_SaturateCastISizeToUInt(Dqn_isize val)
{
    unsigned int result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT_MAX))
            result = DQN_CAST(unsigned int)val;
        else
            result = UINT_MAX;
    }
    return result;
}

DQN_API uint8_t Dqn_Safe_SaturateCastISizeToU8(Dqn_isize val)
{
    uint8_t result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT8_MAX))
            result = DQN_CAST(uint8_t)val;
        else
            result = UINT8_MAX;
    }
    return result;
}

DQN_API uint16_t Dqn_Safe_SaturateCastISizeToU16(Dqn_isize val)
{
    uint16_t result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT16_MAX))
            result = DQN_CAST(uint16_t)val;
        else
            result = UINT16_MAX;
    }
    return result;
}

DQN_API uint32_t Dqn_Safe_SaturateCastISizeToU32(Dqn_isize val)
{
    uint32_t result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT32_MAX))
            result = DQN_CAST(uint32_t)val;
        else
            result = UINT32_MAX;
    }
    return result;
}

DQN_API uint64_t Dqn_Safe_SaturateCastISizeToU64(Dqn_isize val)
{
    uint64_t result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT64_MAX))
            result = DQN_CAST(uint64_t)val;
        else
            result = UINT64_MAX;
    }
    return result;
}

// NOTE: Dqn_Safe_SaturateCastI64To*
// -----------------------------------------------------------------------------
// Both operands are signed so the lowest rank operand will be promoted to
// match the highest rank operand.
DQN_API Dqn_isize Dqn_Safe_SaturateCastI64ToISize(int64_t val)
{
    DQN_CHECK(val >= DQN_ISIZE_MIN && val <= DQN_ISIZE_MAX);
    Dqn_isize result = DQN_CAST(int64_t)DQN_CLAMP(val, DQN_ISIZE_MIN, DQN_ISIZE_MAX);
    return result;
}

DQN_API int8_t Dqn_Safe_SaturateCastI64ToI8(int64_t val)
{
    DQN_CHECK(val >= INT8_MIN && val <= INT8_MAX);
    int8_t result = DQN_CAST(int8_t)DQN_CLAMP(val, INT8_MIN, INT8_MAX);
    return result;
}

DQN_API int16_t Dqn_Safe_SaturateCastI64ToI16(int64_t val)
{
    DQN_CHECK(val >= INT16_MIN && val <= INT16_MAX);
    int16_t result = DQN_CAST(int16_t)DQN_CLAMP(val, INT16_MIN, INT16_MAX);
    return result;
}

DQN_API int32_t Dqn_Safe_SaturateCastI64ToI32(int64_t val)
{
    DQN_CHECK(val >= INT32_MIN && val <= INT32_MAX);
    int32_t result = DQN_CAST(int32_t)DQN_CLAMP(val, INT32_MIN, INT32_MAX);
    return result;
}

// NOTE: Dqn_Safe_SaturateCastIntTo*
// -----------------------------------------------------------------------------
DQN_API int8_t Dqn_Safe_SaturateCastIntToI8(int val)
{
    DQN_CHECK(val >= INT8_MIN && val <= INT8_MAX);
    int8_t result = DQN_CAST(int8_t)DQN_CLAMP(val, INT8_MIN, INT8_MAX);
    return result;
}

DQN_API int16_t Dqn_Safe_SaturateCastIntToI16(int val)
{
    DQN_CHECK(val >= INT16_MIN && val <= INT16_MAX);
    int16_t result = DQN_CAST(int16_t)DQN_CLAMP(val, INT16_MIN, INT16_MAX);
    return result;
}

DQN_API uint8_t Dqn_Safe_SaturateCastIntToU8(int val)
{
    uint8_t result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT8_MAX))
            result = DQN_CAST(uint8_t)val;
        else
            result = UINT8_MAX;
    }
    return result;
}

DQN_API uint16_t Dqn_Safe_SaturateCastIntToU16(int val)
{
    uint16_t result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT16_MAX))
            result = DQN_CAST(uint16_t)val;
        else
            result = UINT16_MAX;
    }
    return result;
}

DQN_API uint32_t Dqn_Safe_SaturateCastIntToU32(int val)
{
    static_assert(sizeof(val) <= sizeof(uint32_t), "Sanity check to allow simplifying of casting");
    uint32_t result = 0;
    if (DQN_CHECK(val >= 0))
        result = DQN_CAST(uint32_t)val;
    return result;
}

DQN_API uint64_t Dqn_Safe_SaturateCastIntToU64(int val)
{
    static_assert(sizeof(val) <= sizeof(uint64_t), "Sanity check to allow simplifying of casting");
    uint64_t result = 0;
    if (DQN_CHECK(val >= 0))
        result = DQN_CAST(uint64_t)val;
    return result;
}

// =================================================================================================
// [$TCTX] Dqn_ThreadContext    |                             | Per-thread data structure e.g. temp arenas
// =================================================================================================
Dqn_ThreadScratch::Dqn_ThreadScratch(DQN_LEAK_TRACE_FUNCTION Dqn_ThreadContext *context, uint8_t context_index)
{
    index       = context_index;
    allocator   = context->temp_allocators[index];
    arena       = context->temp_arenas[index];
    temp_memory = Dqn_Arena_BeginTempMemory(arena);
    #if defined(DQN_LEAK_TRACING)
    leak_site__ = DQN_LEAK_TRACE_ARG_NO_COMMA;
    #endif
}

Dqn_ThreadScratch::~Dqn_ThreadScratch()
{
    #if defined(DQN_DEBUG_THREAD_CONTEXT)
    temp_arenas_stat[index] = arena->stats;
    #endif
    DQN_ASSERT(destructed == false);
    #if defined(DQN_LEAK_TRACING)
    Dqn_Arena_EndTempMemory_(leak_site__, temp_memory);
    #else
    Dqn_Arena_EndTempMemory_(temp_memory);
    #endif
    destructed = true;
}

DQN_API uint32_t Dqn_Thread_GetID()
{
    #if defined(DQN_OS_WIN32)
    unsigned long result = GetCurrentThreadId();
    #else
    pid_t result = gettid();
    assert(gettid() >= 0);
    #endif
    return (uint32_t)result;
}

DQN_API Dqn_ThreadContext *Dqn_Thread_GetContext_(DQN_LEAK_TRACE_FUNCTION_NO_COMMA)
{

    thread_local Dqn_ThreadContext result = {};
    if (!result.init) {
        result.init = true;
        DQN_ASSERTF(dqn_library.lib_init, "Library must be initialised by calling Dqn_Library_Init(nullptr)");

        // NOTE: Setup permanent arena
        Dqn_ArenaCatalog *catalog = &dqn_library.arena_catalog;
        result.allocator          = Dqn_Arena_Allocator(result.arena);
        result.arena              = Dqn_ArenaCatalog_AllocF(catalog,
                                                            DQN_GIGABYTES(1) /*size*/,
                                                            DQN_KILOBYTES(64) /*commit*/,
                                                            "Thread %u Arena",
                                                            Dqn_Thread_GetID());

        // NOTE: Setup temporary arenas
        for (uint8_t index = 0; index < DQN_THREAD_CONTEXT_ARENAS; index++) {
            result.temp_arenas[index]     = Dqn_ArenaCatalog_AllocF(catalog,
                                                                    DQN_GIGABYTES(1) /*size*/,
                                                                    DQN_KILOBYTES(64) /*commit*/,
                                                                    "Thread %u Temp Arena %u",
                                                                    Dqn_Thread_GetID(),
                                                                    index);
            result.temp_allocators[index] = Dqn_Arena_Allocator(result.temp_arenas[index]);
        }
    }
    return &result;
}

// TODO: Is there a way to handle conflict arenas without the user needing to
// manually pass it in?
DQN_API Dqn_ThreadScratch Dqn_Thread_GetScratch_(DQN_LEAK_TRACE_FUNCTION void const *conflict_arena)
{
    static_assert(DQN_THREAD_CONTEXT_ARENAS < (uint8_t)-1, "We use UINT8_MAX as a sentinel value");
    Dqn_ThreadContext *context = Dqn_Thread_GetContext_(DQN_LEAK_TRACE_ARG_NO_COMMA);
    uint8_t context_index      = (uint8_t)-1;
    for (uint8_t index = 0; index < DQN_THREAD_CONTEXT_ARENAS; index++) {
        Dqn_Arena *arena = context->temp_arenas[index];
        if (!conflict_arena || arena != conflict_arena) {
            context_index = index;
            break;
        }
    }

    DQN_ASSERT(context_index != (uint8_t)-1);
    return Dqn_ThreadScratch(DQN_LEAK_TRACE_ARG context, context_index);
}

