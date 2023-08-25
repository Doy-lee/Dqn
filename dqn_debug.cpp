DQN_API Dqn_String8 Dqn_Debug_CleanStackTrace(Dqn_String8 stack_trace)
{
    // NOTE: Remove the stacktrace's library invocations from the stack trace
    // which just adds noise to the actual trace we want to look at, e.g:
    //
    // dqn\b_stacktrace.h(198): b_stacktrace_get
    // dqn\b_stacktrace.h(156): b_stacktrace_get_string

    Dqn_String8BinarySplitResult split_line0 = Dqn_String8_BinarySplit(stack_trace, DQN_STRING8("\n"));
    Dqn_String8BinarySplitResult split_line1 = Dqn_String8_BinarySplit(split_line0.rhs, DQN_STRING8("\n"));

    Dqn_String8 result = split_line1.rhs;
    return result;
}

#if defined(DQN_LEAK_TRACING)
DQN_API void Dqn_Debug_TrackAlloc_(Dqn_String8 stack_trace, void *ptr, Dqn_usize size, bool leak_permitted)
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
            Dqn_String8 alloc_stack_trace       = Dqn_String8_Init(alloc->stack_trace, alloc->stack_trace_size);
            Dqn_String8 alloc_clean_stack_trace = Dqn_String8_Slice(alloc_stack_trace, g_dqn_library->stack_trace_offset_to_our_call_stack, alloc_stack_trace.size);
            Dqn_String8 clean_stack_trace       = Dqn_String8_Slice(stack_trace,       g_dqn_library->stack_trace_offset_to_our_call_stack, stack_trace.size);
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
                DQN_STRING_FMT(alloc_clean_stack_trace),
                DQN_STRING_FMT(clean_stack_trace));
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

DQN_API void Dqn_Debug_TrackDealloc_(Dqn_String8 stack_trace, void *ptr)
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
        Dqn_String8 alloc_stack_trace       = Dqn_String8_Init(alloc->stack_trace, alloc->stack_trace_size);
        Dqn_String8 alloc_clean_stack_trace = Dqn_String8_Slice(alloc_stack_trace, g_dqn_library->stack_trace_offset_to_our_call_stack, alloc_stack_trace.size);

        Dqn_String8 alloc_freed_stack_trace       = Dqn_String8_Init(alloc->freed_stack_trace, alloc->freed_stack_trace_size);
        Dqn_String8 alloc_freed_clean_stack_trace = Dqn_String8_Slice(alloc_freed_stack_trace, g_dqn_library->stack_trace_offset_to_our_call_stack, alloc_freed_stack_trace.size);

        Dqn_String8 dealloc_stack_trace           = Dqn_String8_Slice(stack_trace, g_dqn_library->stack_trace_offset_to_our_call_stack, stack_trace.size);

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
                         DQN_STRING_FMT(alloc_clean_stack_trace),
                         DQN_STRING_FMT(alloc_freed_clean_stack_trace),
                         DQN_STRING_FMT(dealloc_stack_trace));
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

            Dqn_String8 stack_trace = Dqn_String8_Init(alloc->stack_trace, alloc->stack_trace_size);
            Dqn_String8 clean_stack_trace = Dqn_String8_Slice(stack_trace, g_dqn_library->stack_trace_offset_to_our_call_stack, stack_trace.size);
            Dqn_Log_WarningF("Pointer (0x%p) leaked %_$$zu at:\n"
                             "%.*s",
                             alloc->ptr, alloc->size,
                             DQN_STRING_FMT(clean_stack_trace));
        }
    }

    if (leak_count) {
        Dqn_Log_WarningF("There were %I64u leaked allocations totalling %_$$I64u", leak_count, leaked_bytes);
    }
}
#endif /// defined(DQN_LEAK_TRACING)

// NOTE: [$LLOG] Dqn_Log  ==========================================================================
DQN_API Dqn_String8 Dqn_Log_MakeString(Dqn_Allocator allocator,
                                       bool colour,
                                       Dqn_String8 type,
                                       int log_type,
                                       Dqn_CallSite call_site,
                                       char const *fmt,
                                       va_list args)
{
    Dqn_usize     header_size_no_ansi_codes = 0;
    Dqn_String8   header                    = {};
    {
        DQN_LOCAL_PERSIST Dqn_usize max_type_length = 0;
        max_type_length                             = DQN_MAX(max_type_length, type.size);
        int type_padding                            = DQN_CAST(int)(max_type_length - type.size);

        Dqn_String8 colour_esc = {};
        Dqn_String8 bold_esc   = {};
        Dqn_String8 reset_esc  = {};
        if (colour) {
            bold_esc  = Dqn_Print_ESCBoldString;
            reset_esc = Dqn_Print_ESCResetString;
            switch (log_type) {
                case Dqn_LogType_Debug:                                                                          break;
                case Dqn_LogType_Info:    colour_esc = Dqn_Print_ESCColourFgU32String(Dqn_LogTypeColourU32_Info);    break;
                case Dqn_LogType_Warning: colour_esc = Dqn_Print_ESCColourFgU32String(Dqn_LogTypeColourU32_Warning); break;
                case Dqn_LogType_Error:   colour_esc = Dqn_Print_ESCColourFgU32String(Dqn_LogTypeColourU32_Error);   break;
            }
        }

        Dqn_String8 file_name            = Dqn_String8_FileNameFromPath(call_site.file);
        Dqn_DateHMSTimeString const time = Dqn_Date_HMSLocalTimeStringNow();
        header                           = Dqn_String8_InitF(allocator,
                                                             "%.*s "   // date
                                                             "%.*s "   // hms
                                                             "%.*s"    // colour
                                                             "%.*s"    // bold
                                                             "%.*s"    // type
                                                             "%*s"     // type padding
                                                             "%.*s"    // reset
                                                             " %.*s"   // file name
                                                             ":%05u ", // line number
                                                             DQN_CAST(uint32_t)time.date_size - 2, time.date + 2,   // date
                                                             DQN_CAST(uint32_t)time.hms_size,      time.hms,        // hms
                                                             DQN_CAST(uint32_t)colour_esc.size,    colour_esc.data, // colour
                                                             DQN_CAST(uint32_t)bold_esc.size,      bold_esc.data,   // bold
                                                             DQN_CAST(uint32_t)type.size,          type.data,       // type
                                                             DQN_CAST(uint32_t)type_padding,       "",              // type padding
                                                             DQN_CAST(uint32_t)reset_esc.size,     reset_esc.data,  // reset
                                                             DQN_CAST(uint32_t)file_name.size,     file_name.data,  // file name
                                                             call_site.line);                                       // line number
        header_size_no_ansi_codes = header.size - colour_esc.size - Dqn_Print_ESCResetString.size;
    }

    // NOTE: Header padding
    // =========================================================================
    Dqn_usize header_padding = 0;
    {
        DQN_LOCAL_PERSIST Dqn_usize max_header_length = 0;
        max_header_length                             = DQN_MAX(max_header_length, header_size_no_ansi_codes);
        header_padding                                = max_header_length - header_size_no_ansi_codes;
    }

    // NOTE: Construct final log
    // =========================================================================
    Dqn_String8 user_msg = Dqn_String8_InitFV(allocator, fmt, args);
    Dqn_String8 result   = Dqn_String8_Allocate(allocator, header.size + header_padding + user_msg.size, Dqn_ZeroMem_No);
    DQN_MEMCPY(result.data,                                header.data, header.size);
    DQN_MEMSET(result.data + header.size,                  ' ',         header_padding);
    DQN_MEMCPY(result.data + header.size + header_padding, user_msg.data, user_msg.size);
    return result;
}

DQN_FILE_SCOPE void Dqn_Log_FVDefault_(Dqn_String8 type, int log_type, void *user_data, Dqn_CallSite call_site, char const *fmt, va_list args)
{
    (void)log_type;
    (void)user_data;

    // NOTE: Open log file for appending if requested ==========================
    Dqn_TicketMutex_Begin(&g_dqn_library->log_file_mutex);
    if (g_dqn_library->log_to_file && !g_dqn_library->log_file) {
        Dqn_ThreadScratch scratch   = Dqn_Thread_GetScratch(nullptr);
        #if (defined(DQN_OS_WIN32) && !defined(DQN_NO_WIN)) || !defined(DQN_OS_WIN32)
        Dqn_String8        exe_dir  = Dqn_OS_EXEDir(scratch.arena);
        #else
        Dqn_String8        exe_dir  = DQN_STRING8(".");
        #endif
        Dqn_String8        log_file = Dqn_String8_InitF(scratch.allocator, "%.*s/dqn.log", DQN_STRING_FMT(exe_dir));
        fopen_s(DQN_CAST(FILE **)&g_dqn_library->log_file, log_file.data, "a");
    }
    Dqn_TicketMutex_End(&g_dqn_library->log_file_mutex);

    // NOTE: Generate the log header ===========================================
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String8 log_line      = Dqn_Log_MakeString(scratch.allocator,
                                                   !g_dqn_library->log_no_colour,
                                                   type,
                                                   log_type,
                                                   call_site,
                                                   fmt,
                                                   args);

    // NOTE: Print log =========================================================
    Dqn_Print_StdLn(Dqn_PrintStd_Out, log_line);

    Dqn_TicketMutex_Begin(&g_dqn_library->log_file_mutex);
    if (g_dqn_library->log_to_file && g_dqn_library->log_file) {
        fprintf(DQN_CAST(FILE *)g_dqn_library->log_file, "%.*s\n", DQN_STRING_FMT(log_line));
    }
    Dqn_TicketMutex_End(&g_dqn_library->log_file_mutex);
}

DQN_API void Dqn_Log_FVCallSite(Dqn_String8 type, Dqn_CallSite call_site, char const *fmt, va_list args)
{
    Dqn_LogProc *logging_function = g_dqn_library->log_callback ? g_dqn_library->log_callback : Dqn_Log_FVDefault_;
    logging_function(type, -1 /*log_type*/, g_dqn_library->log_user_data, call_site, fmt, args);
}

DQN_API void Dqn_Log_FCallSite(Dqn_String8 type, Dqn_CallSite call_site, char const  *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Log_FVCallSite(type, call_site, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Log_TypeFVCallSite(Dqn_LogType type, Dqn_CallSite call_site, char const *fmt, va_list args)
{
    Dqn_String8 type_string = DQN_STRING8("DQN-BAD-LOG-TYPE");
    switch (type) {
        case Dqn_LogType_Error:   type_string = DQN_STRING8("ERROR"); break;
        case Dqn_LogType_Info:    type_string = DQN_STRING8("INFO"); break;
        case Dqn_LogType_Warning: type_string = DQN_STRING8("WARN"); break;
        case Dqn_LogType_Debug:   type_string = DQN_STRING8("DEBUG"); break;
        case Dqn_LogType_Count:   type_string = DQN_STRING8("BADXX"); break;
    }

    Dqn_LogProc *logging_function = g_dqn_library->log_callback ? g_dqn_library->log_callback : Dqn_Log_FVDefault_;
    logging_function(type_string, type /*log_type*/, g_dqn_library->log_user_data, call_site, fmt, args);
}

DQN_API void Dqn_Log_TypeFCallSite(Dqn_LogType type, Dqn_CallSite call_site, char const  *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Log_TypeFVCallSite(type, call_site, fmt, args);
    va_end(args);
}

