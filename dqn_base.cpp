// NOTE: [$INTR] Intrinsics ========================================================================
#if !defined(DQN_PLATFORM_ARM64) && !defined(DQN_PLATFORM_EMSCRIPTEN)
#if defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
#include <cpuid.h>
#endif

Dqn_CPUIDRegisters Dqn_CPUID(int function_id)
{
    Dqn_CPUIDRegisters result = {};
    #if defined(DQN_COMPILER_MSVC)
    __cpuid(DQN_CAST(int *)result.array, function_id);
    #elif defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
    __get_cpuid(function_id, &result.array[0] /*eax*/, &result.array[1] /*ebx*/, &result.array[2] /*ecx*/ , &result.array[3] /*edx*/);
    #else
    #error "Compiler not supported"
    #endif
    return result;
}
#endif // !defined(DQN_PLATFORM_ARM64) && !defined(DQN_PLATFORM_EMSCRIPTEN)

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

// NOTE: [$TMUT] Dqn_TicketMutex ===================================================================
DQN_API void Dqn_TicketMutex_Begin(Dqn_TicketMutex *mutex)
{
    unsigned int ticket = Dqn_Atomic_AddU32(&mutex->ticket, 1);
    Dqn_TicketMutex_BeginTicket(mutex, ticket);
}

DQN_API void Dqn_TicketMutex_End(Dqn_TicketMutex *mutex)
{
    Dqn_Atomic_AddU32(&mutex->serving, 1);
}

DQN_API Dqn_uint Dqn_TicketMutex_MakeTicket(Dqn_TicketMutex *mutex)
{
    Dqn_uint result = Dqn_Atomic_AddU32(&mutex->ticket, 1);
    return result;
}

DQN_API void Dqn_TicketMutex_BeginTicket(Dqn_TicketMutex const *mutex, Dqn_uint ticket)
{
    DQN_ASSERTF(mutex->serving <= ticket,
                "Mutex skipped ticket? Was ticket generated by the correct mutex via MakeTicket? ticket = %u, "
                "mutex->serving = %u",
                ticket,
                mutex->serving);
    while (ticket != mutex->serving) {
        // NOTE: Use spinlock intrinsic
        _mm_pause();
    }
}

DQN_API bool Dqn_TicketMutex_CanLock(Dqn_TicketMutex const *mutex, Dqn_uint ticket)
{
    bool result = (ticket == mutex->serving);
    return result;
}

#if defined(DQN_COMPILER_MSVC) || defined(DQN_COMPILER_CLANG_CL)
    #if !defined(DQN_CRT_SECURE_NO_WARNINGS_PREVIOUSLY_DEFINED)
        #undef _CRT_SECURE_NO_WARNINGS
    #endif
#endif

// NOTE: [$PRIN] Dqn_Print =========================================================================
DQN_API Dqn_PrintStyle Dqn_Print_StyleColour(uint8_t r, uint8_t g, uint8_t b, Dqn_PrintBold bold)
{
    Dqn_PrintStyle result = {};
    result.bold           = bold;
    result.colour         = true;
    result.r              = r;
    result.g              = g;
    result.b              = b;
    return result;
}

DQN_API Dqn_PrintStyle Dqn_Print_StyleColourU32(uint32_t rgb, Dqn_PrintBold bold)
{
    uint8_t r             = (rgb >> 24) & 0xFF;
    uint8_t g             = (rgb >> 16) & 0xFF;
    uint8_t b             = (rgb >>  8) & 0xFF;
    Dqn_PrintStyle result = Dqn_Print_StyleColour(r, g, b, bold);
    return result;
}

DQN_API Dqn_PrintStyle Dqn_Print_StyleBold()
{
    Dqn_PrintStyle result = {};
    result.bold           = Dqn_PrintBold_Yes;
    return result;
}

DQN_API void Dqn_Print_Std(Dqn_PrintStd std_handle, Dqn_Str8 string)
{
    DQN_ASSERT(std_handle == Dqn_PrintStd_Out || std_handle == Dqn_PrintStd_Err);

    #if defined(DQN_OS_WIN32)
    // NOTE: Get the output handles from kernel ====================================================
    DQN_THREAD_LOCAL void *std_out_print_handle     = nullptr;
    DQN_THREAD_LOCAL void *std_err_print_handle     = nullptr;
    DQN_THREAD_LOCAL bool  std_out_print_to_console = false;
    DQN_THREAD_LOCAL bool  std_err_print_to_console = false;

    if (!std_out_print_handle) {
        unsigned long mode = 0; (void)mode;
        std_out_print_handle     = GetStdHandle(STD_OUTPUT_HANDLE);
        std_out_print_to_console = GetConsoleMode(std_out_print_handle, &mode) != 0;

        std_err_print_handle     = GetStdHandle(STD_ERROR_HANDLE);
        std_err_print_to_console = GetConsoleMode(std_err_print_handle, &mode) != 0;
    }

    // NOTE: Select the output handle ==============================================================
    void *print_handle    = std_out_print_handle;
    bool print_to_console = std_out_print_to_console;
    if (std_handle == Dqn_PrintStd_Err) {
        print_handle     = std_err_print_handle;
        print_to_console = std_err_print_to_console;
    }

    // NOTE: Write the string ======================================================================
    DQN_ASSERT(string.size < DQN_CAST(unsigned long)-1);
    unsigned long bytes_written = 0; (void)bytes_written;
    if (print_to_console) {
        WriteConsoleA(print_handle, string.data, DQN_CAST(unsigned long)string.size, &bytes_written, nullptr);
    } else {
        WriteFile(print_handle, string.data, DQN_CAST(unsigned long)string.size, &bytes_written, nullptr);
    }
    #else
    fprintf(std_handle == Dqn_PrintStd_Out ? stdout : stderr, "%.*s", DQN_STR_FMT(string));
    #endif
}

DQN_API void Dqn_Print_StdStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, Dqn_Str8 string)
{
    if (string.data && string.size) {
        if (style.colour)
            Dqn_Print_Std(std_handle, Dqn_Print_ESCColourFgStr8(style.r, style.g, style.b));
        if (style.bold == Dqn_PrintBold_Yes)
            Dqn_Print_Std(std_handle, Dqn_Print_ESCBoldStr8);
        Dqn_Print_Std(std_handle, string);
        if (style.colour || style.bold == Dqn_PrintBold_Yes)
            Dqn_Print_Std(std_handle, Dqn_Print_ESCResetStr8);
    }
}

DQN_FILE_SCOPE char *Dqn_Print_VSPrintfChunker_(const char *buf, void *user, int len)
{
    Dqn_Str8 string = {};
    string.data        = DQN_CAST(char *)buf;
    string.size        = len;

    Dqn_PrintStd std_handle = DQN_CAST(Dqn_PrintStd)DQN_CAST(uintptr_t)user;
    Dqn_Print_Std(std_handle, string);
    return (char *)buf;
}

DQN_API void Dqn_Print_StdF(Dqn_PrintStd std_handle, DQN_FMT_ATTRIB char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdFV(std_handle, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdFStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_ATTRIB char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdFVStyle(std_handle, style, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdFV(Dqn_PrintStd std_handle, DQN_FMT_ATTRIB char const *fmt, va_list args)
{
    char buffer[STB_SPRINTF_MIN];
    STB_SPRINTF_DECORATE(vsprintfcb)(Dqn_Print_VSPrintfChunker_, DQN_CAST(void *)DQN_CAST(uintptr_t)std_handle, buffer, fmt, args);
}

DQN_API void Dqn_Print_StdFVStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_ATTRIB char const *fmt, va_list args)
{
    if (fmt) {
        if (style.colour)
            Dqn_Print_Std(std_handle, Dqn_Print_ESCColourFgStr8(style.r, style.g, style.b));
        if (style.bold == Dqn_PrintBold_Yes)
            Dqn_Print_Std(std_handle, Dqn_Print_ESCBoldStr8);
        Dqn_Print_StdFV(std_handle, fmt, args);
        if (style.colour || style.bold == Dqn_PrintBold_Yes)
            Dqn_Print_Std(std_handle, Dqn_Print_ESCResetStr8);
    }
}

DQN_API void Dqn_Print_StdLn(Dqn_PrintStd std_handle, Dqn_Str8 string)
{
    Dqn_Print_Std(std_handle, string);
    Dqn_Print_Std(std_handle, DQN_STR8("\n"));
}

DQN_API void Dqn_Print_StdLnF(Dqn_PrintStd std_handle, DQN_FMT_ATTRIB char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdLnFV(std_handle, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdLnFV(Dqn_PrintStd std_handle, DQN_FMT_ATTRIB char const *fmt, va_list args)
{
    Dqn_Print_StdFV(std_handle, fmt, args);
    Dqn_Print_Std(std_handle, DQN_STR8("\n"));
}

DQN_API void Dqn_Print_StdLnStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, Dqn_Str8 string)
{
    Dqn_Print_StdStyle(std_handle, style, string);
    Dqn_Print_Std(std_handle, DQN_STR8("\n"));
}

DQN_API void Dqn_Print_StdLnFStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_ATTRIB char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdLnFVStyle(std_handle, style, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdLnFVStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_ATTRIB char const *fmt, va_list args)
{
    Dqn_Print_StdFVStyle(std_handle, style, fmt, args);
    Dqn_Print_Std(std_handle, DQN_STR8("\n"));
}

DQN_API Dqn_Str8 Dqn_Print_ESCColourStr8(Dqn_PrintESCColour colour, uint8_t r, uint8_t g, uint8_t b)
{
    DQN_THREAD_LOCAL char buffer[32];
    buffer[0]       = 0;
    Dqn_Str8 result = {};
    result.size     = STB_SPRINTF_DECORATE(snprintf)(buffer,
                                                     DQN_ARRAY_UCOUNT(buffer),
                                                     "\x1b[%d;2;%u;%u;%um",
                                                     colour == Dqn_PrintESCColour_Fg ? 38 : 48,
                                                     r, g, b);
    result.data     = buffer;
    return result;
}

DQN_API Dqn_Str8 Dqn_Print_ESCColourU32Str8(Dqn_PrintESCColour colour, uint32_t value)
{
    uint8_t r          = DQN_CAST(uint8_t)(value >> 24);
    uint8_t g          = DQN_CAST(uint8_t)(value >> 16);
    uint8_t b          = DQN_CAST(uint8_t)(value >>  8);
    Dqn_Str8 result = Dqn_Print_ESCColourStr8(colour, r, g, b);
    return result;
}

// NOTE: [$LLOG] Dqn_Log  ==========================================================================
DQN_API Dqn_Str8 Dqn_Log_MakeStr8(Dqn_Allocator              allocator,
                                  bool                       colour,
                                  Dqn_Str8                   type,
                                  int                        log_type,
                                  Dqn_CallSite               call_site,
                                  DQN_FMT_ATTRIB char const *fmt,
                                  va_list                    args)
{
    Dqn_usize header_size_no_ansi_codes = 0;
    Dqn_Str8  header                    = {};
    {
        DQN_LOCAL_PERSIST Dqn_usize max_type_length = 0;
        max_type_length                             = DQN_MAX(max_type_length, type.size);
        int type_padding                            = DQN_CAST(int)(max_type_length - type.size);

        Dqn_Str8 colour_esc = {};
        Dqn_Str8 bold_esc   = {};
        Dqn_Str8 reset_esc  = {};
        if (colour) {
            bold_esc  = Dqn_Print_ESCBoldStr8;
            reset_esc = Dqn_Print_ESCResetStr8;
            switch (log_type) {
                case Dqn_LogType_Debug:                                                                            break;
                case Dqn_LogType_Info:    colour_esc = Dqn_Print_ESCColourFgU32Str8(Dqn_LogTypeColourU32_Info);    break;
                case Dqn_LogType_Warning: colour_esc = Dqn_Print_ESCColourFgU32Str8(Dqn_LogTypeColourU32_Warning); break;
                case Dqn_LogType_Error:   colour_esc = Dqn_Print_ESCColourFgU32Str8(Dqn_LogTypeColourU32_Error);   break;
            }
        }

        Dqn_Str8 file_name             = Dqn_Str8_FileNameFromPath(call_site.file);
        Dqn_DateHMSTimeStr8 const time = Dqn_Date_LocalTimeHMSStr8Now();
        header                         = Dqn_Str8_InitF(allocator,
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
        header_size_no_ansi_codes = header.size - colour_esc.size - Dqn_Print_ESCResetStr8.size;
    }

    // NOTE: Header padding ========================================================================
    Dqn_usize header_padding = 0;
    {
        DQN_LOCAL_PERSIST Dqn_usize max_header_length = 0;
        max_header_length                             = DQN_MAX(max_header_length, header_size_no_ansi_codes);
        header_padding                                = max_header_length - header_size_no_ansi_codes;
    }

    // NOTE: Construct final log ===================================================================
    Dqn_Str8 user_msg = Dqn_Str8_InitFV(allocator, fmt, args);
    Dqn_Str8 result   = Dqn_Str8_Allocate(allocator, header.size + header_padding + user_msg.size, Dqn_ZeroMem_No);
    DQN_MEMCPY(result.data,                                header.data, header.size);
    DQN_MEMSET(result.data + header.size,                  ' ',         header_padding);
    DQN_MEMCPY(result.data + header.size + header_padding, user_msg.data, user_msg.size);
    return result;
}

DQN_FILE_SCOPE void Dqn_Log_FVDefault_(Dqn_Str8 type, int log_type, void *user_data, Dqn_CallSite call_site, DQN_FMT_ATTRIB char const *fmt, va_list args)
{
    Dqn_Library *lib = g_dqn_library;
    (void)log_type;
    (void)user_data;

    // NOTE: Open log file for appending if requested ==========================
    Dqn_TicketMutex_Begin(&lib->log_file_mutex);
    if (lib->log_to_file && !lib->log_file.handle && lib->log_file.error_size == 0) {
        Dqn_ThreadScratch scratch  = Dqn_Thread_GetScratch(nullptr);
        Dqn_Str8          log_path = Dqn_FsPath_ConvertF(scratch.arena, "%.*s/dqn.log", DQN_STR_FMT(lib->exe_dir));
        lib->log_file              = Dqn_Fs_OpenFile(log_path, Dqn_FsFileOpen_CreateAlways, Dqn_FsFileAccess_AppendOnly);
    }
    Dqn_TicketMutex_End(&lib->log_file_mutex);

    // NOTE: Generate the log header ===========================================
    Dqn_ThreadScratch scratch  = Dqn_Thread_GetScratch(nullptr);
    Dqn_Str8          log_line = Dqn_Log_MakeStr8(scratch.allocator, !lib->log_no_colour, type, log_type, call_site, fmt, args);

    // NOTE: Print log =========================================================
    Dqn_Print_StdLn(Dqn_PrintStd_Out, log_line);

    Dqn_TicketMutex_Begin(&lib->log_file_mutex);
    Dqn_Fs_WriteFile(&lib->log_file, log_line);
    Dqn_Fs_WriteFile(&lib->log_file, DQN_STR8("\n"));
    Dqn_TicketMutex_End(&lib->log_file_mutex);
}

DQN_API void Dqn_Log_FVCallSite(Dqn_Str8 type, Dqn_CallSite call_site, DQN_FMT_ATTRIB char const *fmt, va_list args)
{
    if (g_dqn_library) {
        Dqn_LogProc *logging_function = g_dqn_library->log_callback ? g_dqn_library->log_callback : Dqn_Log_FVDefault_;
        logging_function(type, -1 /*log_type*/, g_dqn_library->log_user_data, call_site, fmt, args);
    } else {
        // NOTE: Rarely taken branch, only when trying to use this library without initialising it
        Dqn_Print_StdLnFV(Dqn_PrintStd_Out, fmt, args);
    }
}

DQN_API void Dqn_Log_FCallSite(Dqn_Str8 type, Dqn_CallSite call_site, DQN_FMT_ATTRIB char const  *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Log_FVCallSite(type, call_site, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Log_TypeFVCallSite(Dqn_LogType type, Dqn_CallSite call_site, DQN_FMT_ATTRIB char const *fmt, va_list args)
{
    Dqn_Str8 type_string = DQN_STR8("DQN-BAD-LOG-TYPE");
    switch (type) {
        case Dqn_LogType_Error:   type_string = DQN_STR8("ERROR"); break;
        case Dqn_LogType_Info:    type_string = DQN_STR8("INFO"); break;
        case Dqn_LogType_Warning: type_string = DQN_STR8("WARN"); break;
        case Dqn_LogType_Debug:   type_string = DQN_STR8("DEBUG"); break;
        case Dqn_LogType_Count:   type_string = DQN_STR8("BADXX"); break;
    }
    Dqn_Log_FVCallSite(type_string, call_site, fmt, args);
}

DQN_API void Dqn_Log_TypeFCallSite(Dqn_LogType type, Dqn_CallSite call_site, DQN_FMT_ATTRIB char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Log_TypeFVCallSite(type, call_site, fmt, args);
    va_end(args);
}
