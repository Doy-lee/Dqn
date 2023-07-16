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

DQN_API void Dqn_Print_Std(Dqn_PrintStd std_handle, Dqn_String8 string)
{
    DQN_ASSERT(std_handle == Dqn_PrintStd_Out || std_handle == Dqn_PrintStd_Err);

    #if defined(DQN_OS_WIN32)
    // NOTE: Get the output handles from kernel
    // =========================================================================
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

    // NOTE: Select the output handle
    // =========================================================================
    void *print_handle    = std_out_print_handle;
    bool print_to_console = std_out_print_to_console;
    if (std_handle == Dqn_PrintStd_Err) {
        print_handle     = std_err_print_handle;
        print_to_console = std_err_print_to_console;
    }

    // NOTE: Write the string
    // =========================================================================
    DQN_ASSERT(string.size < DQN_CAST(unsigned long)-1);
    unsigned long bytes_written = 0; (void)bytes_written;
    if (print_to_console) {
        WriteConsoleA(print_handle, string.data, DQN_CAST(unsigned long)string.size, &bytes_written, nullptr);
    } else {
        WriteFile(print_handle, string.data, DQN_CAST(unsigned long)string.size, &bytes_written, nullptr);
    }
    #else
    fprintf(std_handle == Dqn_PrintStd_Out ? stdout : stderr, "%.*s", DQN_STRING_FMT(string));
    #endif
}

DQN_API void Dqn_Print_StdStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, Dqn_String8 string)
{
    if (string.data && string.size) {
        if (style.colour)
            Dqn_Print_Std(std_handle, Dqn_Print_ESCColourFgString(style.r, style.g, style.b));
        if (style.bold == Dqn_PrintBold_Yes)
            Dqn_Print_Std(std_handle, Dqn_Print_ESCBoldString);
        Dqn_Print_Std(std_handle, string);
        if (style.colour || style.bold == Dqn_PrintBold_Yes)
            Dqn_Print_Std(std_handle, Dqn_Print_ESCResetString);
    }
}

DQN_FILE_SCOPE char *Dqn_Print_VSPrintfChunker_(const char *buf, void *user, int len)
{
    Dqn_String8 string = {};
    string.data        = DQN_CAST(char *)buf;
    string.size        = len;

    Dqn_PrintStd std_handle = DQN_CAST(Dqn_PrintStd)DQN_CAST(uintptr_t)user;
    Dqn_Print_Std(std_handle, string);
    return (char *)buf;
}

DQN_API void Dqn_Print_StdF(Dqn_PrintStd std_handle, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdFV(std_handle, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdFStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdFVStyle(std_handle, style, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdFV(Dqn_PrintStd std_handle, char const *fmt, va_list args)
{
    char buffer[STB_SPRINTF_MIN];
    STB_SPRINTF_DECORATE(vsprintfcb)(Dqn_Print_VSPrintfChunker_, DQN_CAST(void *)DQN_CAST(uintptr_t)std_handle, buffer, fmt, args);
}

DQN_API void Dqn_Print_StdFVStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, char const *fmt, va_list args)
{
    if (fmt) {
        if (style.colour)
            Dqn_Print_Std(std_handle, Dqn_Print_ESCColourFgString(style.r, style.g, style.b));
        if (style.bold == Dqn_PrintBold_Yes)
            Dqn_Print_Std(std_handle, Dqn_Print_ESCBoldString);
        Dqn_Print_StdFV(std_handle, fmt, args);
        if (style.colour || style.bold == Dqn_PrintBold_Yes)
            Dqn_Print_Std(std_handle, Dqn_Print_ESCResetString);
    }
}

DQN_API void Dqn_Print_StdLn(Dqn_PrintStd std_handle, Dqn_String8 string)
{
    Dqn_Print_Std(std_handle, string);
    Dqn_Print_Std(std_handle, DQN_STRING8("\n"));
}

DQN_API void Dqn_Print_StdLnF(Dqn_PrintStd std_handle, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdLnFV(std_handle, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdLnFV(Dqn_PrintStd std_handle, char const *fmt, va_list args)
{
    Dqn_Print_StdFV(std_handle, fmt, args);
    Dqn_Print_Std(std_handle, DQN_STRING8("\n"));
}

DQN_API void Dqn_Print_StdLnStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, Dqn_String8 string)
{
    Dqn_Print_StdStyle(std_handle, style, string);
    Dqn_Print_Std(std_handle, DQN_STRING8("\n"));
}

DQN_API void Dqn_Print_StdLnFStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdLnFVStyle(std_handle, style, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdLnFVStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, char const *fmt, va_list args)
{
    Dqn_Print_StdFVStyle(std_handle, style, fmt, args);
    Dqn_Print_Std(std_handle, DQN_STRING8("\n"));
}

DQN_API Dqn_String8 Dqn_Print_ESCColourString(Dqn_PrintESCColour colour, uint8_t r, uint8_t g, uint8_t b)
{
    DQN_THREAD_LOCAL char buffer[32];
    buffer[0]          = 0;
    Dqn_String8 result = {};
    result.size        = STB_SPRINTF_DECORATE(snprintf)(buffer,
                                                        DQN_ARRAY_UCOUNT(buffer),
                                                        "\x1b[%d;2;%u;%u;%um",
                                                        colour == Dqn_PrintESCColour_Fg ? 38 : 48,
                                                        r, g, b);
    result.data        = buffer;
    return result;
}

DQN_API Dqn_String8 Dqn_Print_ESCColourU32String(Dqn_PrintESCColour colour, uint32_t value)
{
    uint8_t r          = DQN_CAST(uint8_t)(value >> 24);
    uint8_t g          = DQN_CAST(uint8_t)(value >> 16);
    uint8_t b          = DQN_CAST(uint8_t)(value >>  8);
    Dqn_String8 result = Dqn_Print_ESCColourString(colour, r, g, b);
    return result;
}

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
                                                             time.date_size - 2, time.date + 2,
                                                             time.hms_size,      time.hms,
                                                             colour_esc.size,    colour_esc.data,
                                                             bold_esc.size,      bold_esc.data,
                                                             type.size,          type.data,
                                                             type_padding,       "",
                                                             reset_esc.size,     reset_esc.data,
                                                             file_name.size,     file_name.data,
                                                             call_site.line);
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

    // NOTE: Open log file for appending if requested
    // =========================================================================
    Dqn_TicketMutex_Begin(&dqn_library.log_file_mutex);
    if (dqn_library.log_to_file && !dqn_library.log_file) {
        Dqn_ThreadScratch scratch   = Dqn_Thread_GetScratch(nullptr);
        Dqn_String8        exe_dir  = Dqn_OS_EXEDir(scratch.allocator);
        Dqn_String8        log_file = Dqn_String8_InitF(scratch.allocator, "%.*s/dqn.log", DQN_STRING_FMT(exe_dir));
        fopen_s(DQN_CAST(FILE **)&dqn_library.log_file, log_file.data, "a");
    }
    Dqn_TicketMutex_End(&dqn_library.log_file_mutex);

    // NOTE: Generate the log header
    // =========================================================================
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String8 log_line      = Dqn_Log_MakeString(scratch.allocator,
                                                   !dqn_library.log_no_colour,
                                                   type,
                                                   log_type,
                                                   call_site,
                                                   fmt,
                                                   args);

    // NOTE: Print log
    // =========================================================================
    Dqn_Print_StdLn(Dqn_PrintStd_Out, log_line);

    Dqn_TicketMutex_Begin(&dqn_library.log_file_mutex);
    if (dqn_library.log_to_file) {
        fprintf(DQN_CAST(FILE *)dqn_library.log_file, "%.*s\n", DQN_STRING_FMT(log_line));
    }
    Dqn_TicketMutex_End(&dqn_library.log_file_mutex);
}

DQN_API void Dqn_Log_FVCallSite(Dqn_String8 type, Dqn_CallSite call_site, char const *fmt, va_list args)
{
    Dqn_LogProc *logging_function = dqn_library.log_callback ? dqn_library.log_callback : Dqn_Log_FVDefault_;
    logging_function(type, -1 /*log_type*/, dqn_library.log_user_data, call_site, fmt, args);
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

    Dqn_LogProc *logging_function = dqn_library.log_callback ? dqn_library.log_callback : Dqn_Log_FVDefault_;
    logging_function(type_string, type /*log_type*/, dqn_library.log_user_data, call_site, fmt, args);
}

DQN_API void Dqn_Log_TypeFCallSite(Dqn_LogType type, Dqn_CallSite call_site, char const  *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Log_TypeFVCallSite(type, call_site, fmt, args);
    va_end(args);
}
