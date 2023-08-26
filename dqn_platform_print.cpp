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

DQN_API void Dqn_Print_StdF(Dqn_PrintStd std_handle, DQN_FMT_STRING_ANNOTATE char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdFV(std_handle, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdFStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_STRING_ANNOTATE char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdFVStyle(std_handle, style, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdFV(Dqn_PrintStd std_handle, DQN_FMT_STRING_ANNOTATE char const *fmt, va_list args)
{
    char buffer[STB_SPRINTF_MIN];
    STB_SPRINTF_DECORATE(vsprintfcb)(Dqn_Print_VSPrintfChunker_, DQN_CAST(void *)DQN_CAST(uintptr_t)std_handle, buffer, fmt, args);
}

DQN_API void Dqn_Print_StdFVStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_STRING_ANNOTATE char const *fmt, va_list args)
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

DQN_API void Dqn_Print_StdLnF(Dqn_PrintStd std_handle, DQN_FMT_STRING_ANNOTATE char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdLnFV(std_handle, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdLnFV(Dqn_PrintStd std_handle, DQN_FMT_STRING_ANNOTATE char const *fmt, va_list args)
{
    Dqn_Print_StdFV(std_handle, fmt, args);
    Dqn_Print_Std(std_handle, DQN_STRING8("\n"));
}

DQN_API void Dqn_Print_StdLnStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, Dqn_String8 string)
{
    Dqn_Print_StdStyle(std_handle, style, string);
    Dqn_Print_Std(std_handle, DQN_STRING8("\n"));
}

DQN_API void Dqn_Print_StdLnFStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_STRING_ANNOTATE char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdLnFVStyle(std_handle, style, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdLnFVStyle(Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_STRING_ANNOTATE char const *fmt, va_list args)
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

