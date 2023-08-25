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

// TODO(doyle): Use our temp scratch arenas instead of a massive array on the 
// stack.
// NOTE: Max size from MSDN, using \\? syntax, but the ? bit can be expanded
// even more so the max size is kind of not well defined.
#if defined(DQN_OS_WIN32) && !defined(DQN_OS_WIN32_MAX_PATH)
    #define DQN_OS_WIN32_MAX_PATH 32767 + 128 /*fudge*/
#endif

#if !defined(DQN_NO_FS)
// NOTE: [$FSYS] Dqn_Fs ============================================================================
#if defined(DQN_OS_WIN32)
DQN_API uint64_t Dqn_Win__FileTimeToSeconds(FILETIME const *time)
{
    ULARGE_INTEGER time_large_int = {};
    time_large_int.u.LowPart      = time->dwLowDateTime;
    time_large_int.u.HighPart     = time->dwHighDateTime;
    uint64_t result                = (time_large_int.QuadPart / 10000000ULL) - 11644473600ULL;
    return result;
}
#endif

DQN_API bool Dqn_Fs_Exists(Dqn_String8 path)
{
    bool result = false;
    if (!Dqn_String8_IsValid(path))
        return result;

    #if defined(DQN_OS_WIN32)
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String16 path16       = Dqn_Win_String8ToString16(scratch.arena, path);
    if (path16.size) {
        WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
        if (GetFileAttributesExW(path16.data, GetFileExInfoStandard, &attrib_data)) {
            result = (attrib_data.dwFileAttributes != INVALID_FILE_ATTRIBUTES) &&
                     !(attrib_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        }
    }
    #elif defined(DQN_OS_UNIX)
    struct stat stat_result;
    if (lstat(path.data, &stat_result) != -1)
        result = S_ISREG(stat_result.st_mode) || S_ISLNK(stat_result.st_mode);
    #else
    #error Unimplemented
    #endif

    return result;
}

DQN_API bool Dqn_Fs_DirExists(Dqn_String8 path)
{
    bool result = false;
    if (!Dqn_String8_IsValid(path))
        return result;

    #if defined(DQN_OS_WIN32)
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String16 path16       = Dqn_Win_String8ToString16(scratch.arena, path);
    if (path16.size) {
        WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
        if (GetFileAttributesExW(path16.data, GetFileExInfoStandard, &attrib_data)) {
            result = (attrib_data.dwFileAttributes != INVALID_FILE_ATTRIBUTES) &&
                     (attrib_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        }
    }

    #elif defined(DQN_OS_UNIX)
    struct stat stat_result;
    if (lstat(path.data, &stat_result) != -1)
        result = S_ISDIR(stat_result.st_mode);
    #else
    #error Unimplemented
    #endif
    return result;
}

DQN_API Dqn_FsInfo Dqn_Fs_GetInfo(Dqn_String8 path)
{
    Dqn_FsInfo result = {};
    if (!Dqn_String8_IsValid(path))
        return result;

    #if defined(DQN_OS_WIN32)
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String16 path16       = Dqn_Win_String8ToString16(scratch.arena, path);

    WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
    if (!GetFileAttributesExW(path16.data, GetFileExInfoStandard, &attrib_data))
        return result;

    result.exists                = true;
    result.create_time_in_s      = Dqn_Win__FileTimeToSeconds(&attrib_data.ftCreationTime);
    result.last_access_time_in_s = Dqn_Win__FileTimeToSeconds(&attrib_data.ftLastAccessTime);
    result.last_write_time_in_s  = Dqn_Win__FileTimeToSeconds(&attrib_data.ftLastWriteTime);

    LARGE_INTEGER large_int = {};
    large_int.u.HighPart    = DQN_CAST(int32_t)attrib_data.nFileSizeHigh;
    large_int.u.LowPart     = attrib_data.nFileSizeLow;
    result.size             = (uint64_t)large_int.QuadPart;

    if (attrib_data.dwFileAttributes != INVALID_FILE_ATTRIBUTES) {
        if (attrib_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            result.type = Dqn_FsInfoType_Directory;
        } else {
            result.type = Dqn_FsInfoType_File;
        }
    }

    #elif defined(DQN_OS_UNIX)
    struct stat file_stat;
    if (lstat(path.data, &file_stat) != -1) {
        result.exists                = true;
        result.size                  = file_stat.st_size;
        result.last_access_time_in_s = file_stat.st_atime;
        result.last_write_time_in_s  = file_stat.st_mtime;

        // TODO(dqn): Seems linux does not support creation time via stat. We
        // shoddily deal with this.
        result.create_time_in_s = DQN_MIN(result.last_access_time_in_s, result.last_write_time_in_s);
    }

    #else
    #error Unimplemented
    #endif

    return result;
}

DQN_API bool Dqn_Fs_Copy(Dqn_String8 src, Dqn_String8 dest, bool overwrite)
{
    bool result = false;
    #if defined(DQN_OS_WIN32)
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String16      src16   = Dqn_Win_String8ToString16(scratch.arena, src);
    Dqn_String16      dest16  = Dqn_Win_String8ToString16(scratch.arena, dest);

    int fail_if_exists = overwrite == false;
    result = CopyFileW(src16.data, dest16.data, fail_if_exists) != 0;

    if (!result) {
        Dqn_WinError error = Dqn_Win_LastError(scratch.arena);
        Dqn_Log_ErrorF("Failed to copy the file\n\nSource: %.*s\nDestination: %.*s\n\nWindows reported: %.*s",
                       DQN_STRING_FMT(src),
                       DQN_STRING_FMT(dest),
                       DQN_STRING_FMT(error.msg));
    }

    #elif defined(DQN_OS_UNIX)
    int src_fd  = open(src.data, O_RDONLY);
    int dest_fd = open(dest.data, O_WRONLY | O_CREAT | (overwrite ? O_TRUNC : 0));

    if (src_fd != -1 && dest_fd != -1) {
        struct stat stat_existing;
        fstat(src_fd, &stat_existing);
        Dqn_usize bytes_written = sendfile64(dest_fd, src_fd, 0, stat_existing.st_size);
        result = (bytes_written == stat_existing.st_size);
    }

    if (src_fd != -1)
        close(src_fd);

    if (dest_fd != -1)
        close(dest_fd);
    #else
    #error Unimplemented
    #endif

    return result;
}

DQN_API bool Dqn_Fs_MakeDir(Dqn_String8 path)
{
    Dqn_ThreadScratch scratch   = Dqn_Thread_GetScratch(nullptr);
    bool result                 = true;

    #if defined(DQN_OS_WIN32)
    Dqn_String16 path16 = Dqn_Win_String8ToString16(scratch.arena, path);

    // NOTE: Go back from the end of the string to all the directories in the
    // string, and try to create them. Since Win32 API cannot create
    // intermediate directories that don't exist in a path we need to go back
    // and record all the directories until we encounter one that exists.
    //
    // From that point onwards go forwards and make all the directories
    // inbetween by null-terminating the string temporarily, creating the
    // directory and so forth until we reach the end.
    //
    // If we find a file at some point in the path we fail out because the
    // series of directories can not be made if a file exists with the same
    // name.
    for (Dqn_usize index = 0; index < path16.size; index++) {
        bool first_char = index == (path16.size - 1);
        wchar_t ch      = path16.data[index];
        if (ch == '/' || ch == '\\' || first_char) {
            wchar_t temp = path16.data[index];
            if (!first_char)
                path16.data[index] = 0; // Temporarily null terminate it

            WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
            bool successful = GetFileAttributesExW(path16.data, GetFileExInfoStandard, &attrib_data); // Check

            if (successful) {
                if (attrib_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    // NOTE: The directory exists, continue iterating the path
                } else {
                    // NOTE: There's some kind of file that exists at the path
                    // but it's not a directory. This request to make a 
                    // directory is invalid.
                    return false;
                }
            } else {
                // NOTE: There's nothing that exists at this path, we can create
                // a directory here
                result |= (CreateDirectoryW(path16.data, nullptr) == 0);
            }

            if (!first_char)
                path16.data[index] = temp; // Undo null termination
        }
    }

    #elif defined(DQN_OS_UNIX)

    // TODO(doyle): Implement this without using the path indexes, it's not 
    // necessary. See Windows implementation.
    Dqn_usize path_indexes_size = 0;
    uint16_t path_indexes[64]   = {};

    Dqn_String8 copy = Dqn_String8_Copy(scratch.arena, path);
    for (Dqn_usize index = copy.size - 1; index < copy.size; index--) {
        bool first_char = index == (copy.size - 1);
        char ch         = copy.data[index];
        if (ch == '/' || first_char) {
            char temp = copy.data[index];

            if (!first_char)
                copy.data[index] = 0; // Temporarily null terminate it

            bool is_file = Dqn_Fs_Exists(copy);

            if (!first_char)
                copy.data[index] = temp; // Undo null termination

            if (is_file) {
                // NOTE: There's something that exists in at this path, but
                // it's not a directory. This request to make a directory is
                // invalid.
                return false;
            } else {
                if (Dqn_Fs_DirExists(copy)) {
                    // NOTE: We found a directory, we can stop here and start
                    // building up all the directories that didn't exist up to
                    // this point.
                    break;
                } else {
                    // NOTE: There's nothing that exists at this path, we can
                    // create a directory here
                    path_indexes[path_indexes_size++] = DQN_CAST(uint16_t)index;
                }
            }
        }
    }

    for (Dqn_usize index = path_indexes_size - 1; result && index < path_indexes_size; index--) {
        uint16_t path_index = path_indexes[index];
        char temp = copy.data[path_index];

        if (index != 0) copy.data[path_index] = 0;
        result |= mkdir(copy.data, 0774) == 0;
        if (index != 0) copy.data[path_index] = temp;
    }

    #else
    #error Unimplemented
    #endif

    return result;
}

DQN_API bool Dqn_Fs_Move(Dqn_String8 src, Dqn_String8 dest, bool overwrite)
{
    bool result = false;

    #if defined(DQN_OS_WIN32)
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String16      src16   = Dqn_Win_String8ToString16(scratch.arena, src);
    Dqn_String16      dest16  = Dqn_Win_String8ToString16(scratch.arena, dest);

    unsigned long flags = MOVEFILE_COPY_ALLOWED;
    if (overwrite) {
        flags |= MOVEFILE_REPLACE_EXISTING;
    }

    result = MoveFileExW(src16.data, dest16.data, flags) != 0;
    if (!result) {
        Dqn_ThreadScratch inner_scratch = Dqn_Thread_GetScratch(scratch.arena);
        Dqn_WinError error              = Dqn_Win_LastError(inner_scratch.arena);
        Dqn_Log_ErrorF("Failed to move the file\n\nSource: %.*s\nDestination: %.*s\n\nWindows reported: %.*s",
                       DQN_STRING_FMT(src),
                       DQN_STRING_FMT(dest),
                       DQN_STRING_FMT(error.msg));
    }

    #elif defined(DQN_OS_UNIX)
    // See: https://github.com/gingerBill/gb/blob/master/gb.h
    bool file_moved = true;
    if (link(src.data, dest.data) == -1)
    {
        // NOTE: Link can fail if we're trying to link across different volumes
        // so we fall back to a binary directory.
        file_moved |= Dqn_Fs_Copy(src, dest, overwrite);
    }

    if (file_moved)
        result = (unlink(src.data) != -1); // Remove original file

    #else
    #error Unimplemented
    #endif
    return result;
}

DQN_API bool Dqn_Fs_Delete(Dqn_String8 path)
{
    bool result = false;
    if (!Dqn_String8_IsValid(path))
        return result;

    #if defined(DQN_OS_WIN32)
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String16      path16  = Dqn_Win_String8ToString16(scratch.arena, path);
    if (path16.size) {
        result = DeleteFileW(path16.data);
        if (!result)
            result = RemoveDirectoryW(path16.data);
    }
    #elif defined(DQN_OS_UNIX)
    result = remove(path.data) == 0;
    #else
    #error Unimplemented
    #endif
    return result;
}

// NOTE: R/W Entire File ===========================================================================
DQN_API char *Dqn_Fs_ReadCString8(char const *path, Dqn_usize path_size, Dqn_usize *file_size, Dqn_Allocator allocator)
{
    char *result = nullptr;
    if (!path)
        return result;

    if (path_size <= 0)
        path_size = Dqn_CString8_Size(path);

    (void)allocator;
    (void)file_size;

    #if defined(DQN_OS_WIN32)
    // NOTE: Convert to UTF16 ==================================================
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(allocator.user_context);
    Dqn_String8       path8   = Dqn_String8_Init(path, path_size);
    Dqn_String16      path16  = Dqn_Win_String8ToString16(scratch.arena, path8);

    // NOTE: Get the file handle ===============================================
    void *file_handle = CreateFileW(/*LPCWSTR               lpFileName*/            path16.data,
                                    /*DWORD                 dwDesiredAccess*/       GENERIC_READ,
                                    /*DWORD                 dwShareMode*/           0,
                                    /*LPSECURITY_ATTRIBUTES lpSecurityAttributes*/  nullptr,
                                    /*DWORD                 dwCreationDisposition*/ OPEN_EXISTING,
                                    /*DWORD                 dwFlagsAndAttributes*/  FILE_ATTRIBUTE_READONLY,
                                    /*HANDLE                hTemplateFile*/         nullptr);
    if (file_handle == INVALID_HANDLE_VALUE) {
        Dqn_WinError error = Dqn_Win_LastError(scratch.arena);
        Dqn_Log_ErrorF("Failed to open file for reading [file=%.*s, reason=%.*s]", DQN_STRING_FMT(path8), DQN_STRING_FMT(error.msg));
        return nullptr;
    }
    DQN_DEFER { CloseHandle(file_handle); };

    // NOTE: Query the file size
    // -------------------------------------------------------------------------
    LARGE_INTEGER win_file_size;
    if (!GetFileSizeEx(file_handle, &win_file_size)) {
        Dqn_WinError error = Dqn_Win_LastError(scratch.arena);
        Dqn_Log_ErrorF("Failed to query file size [file=%.*s, reason=%.*s]", DQN_STRING_FMT(path8), DQN_STRING_FMT(error.msg));
        return nullptr;
    }

    unsigned long const bytes_desired = DQN_CAST(unsigned long)win_file_size.QuadPart;
    if (!DQN_CHECKF(bytes_desired == win_file_size.QuadPart,
                    "Current implementation doesn't support >4GiB, implement Win32 overlapped IO")) {
        return nullptr;
    }

    // NOTE: Read the file from disk
    // -------------------------------------------------------------------------
    result = DQN_CAST(char *)Dqn_Allocator_Alloc(allocator,
                                                 bytes_desired,
                                                 alignof(char),
                                                 Dqn_ZeroMem_No);
    unsigned long bytes_read    = 0;
    unsigned long read_result   = ReadFile(/*HANDLE hFile*/               file_handle,
                                           /*LPVOID lpBuffer*/            result,
                                           /*DWORD nNumberOfBytesToRead*/ bytes_desired,
                                           /*LPDWORD lpNumberOfByesRead*/ &bytes_read,
                                           /*LPOVERLAPPED lpOverlapped*/  nullptr);

    if (read_result == 0) {
        Dqn_Allocator_Dealloc(allocator, result, bytes_desired);
        Dqn_WinError error = Dqn_Win_LastError(scratch.arena);
        Dqn_Log_ErrorF("'ReadFile' failed to load file to memory [file=%.*s, reason=%.*s]", DQN_STRING_FMT(path8), DQN_STRING_FMT(error.msg));
        return nullptr;
    }

    if (bytes_read != bytes_desired) {
        Dqn_WinError error = Dqn_Win_LastError(scratch.arena);
        Dqn_Allocator_Dealloc(allocator, result, bytes_desired);
        Dqn_Log_ErrorF("'ReadFile' failed to read all bytes into file [file=%.*s, bytes_desired=%u, bytes_read=%u, reason=%.*s]",
                  DQN_STRING_FMT(path8),
                  bytes_desired,
                  bytes_read,
                  DQN_STRING_FMT(error.msg));
        return nullptr;
    }

    if (file_size) {
        *file_size = Dqn_Safe_SaturateCastI64ToISize(bytes_read);
    }
    #else
    Dqn_usize file_size_ = 0;
    if (!file_size)
        file_size = &file_size_;

    FILE *file_handle = fopen(path, "rb");
    if (!file_handle) {
        Dqn_Log_ErrorF("Failed to open file '%s' using fopen\n", path);
        return result;
    }

    DQN_DEFER { fclose(file_handle); };
    fseek(file_handle, 0, SEEK_END);
    *file_size = ftell(file_handle);

    if (DQN_CAST(long)(*file_size) == -1L) {
        Dqn_Log_ErrorF("Failed to determine '%s' file size using ftell\n", file);
        return result;
    }

    rewind(file_handle);
    result = DQN_CAST(char *)Dqn_Allocator_Alloc(allocator,
                                                 *file_size,
                                                 alignof(char),
                                                 Dqn_ZeroMem_No);
    if (!result) {
        Dqn_Log_ErrorF("Failed to allocate %td bytes to read file '%s'\n", *file_size + 1, file);
        return result;
    }

    result[*file_size] = 0;
    if (fread(result, DQN_CAST(size_t)(*file_size), 1, file_handle) != 1) {
        Dqn_Allocator_Dealloc(allocator, result, *file_size);
        Dqn_Log_ErrorF("Failed to read %td bytes into buffer from '%s'\n", *file_size, file);
        return result;
    }
    #endif
    return result;
}

DQN_API Dqn_String8 Dqn_Fs_Read(Dqn_String8 path, Dqn_Allocator allocator)
{
    Dqn_usize   file_size = 0;
    char *      string    = Dqn_Fs_ReadCString8(path.data, path.size, &file_size, allocator);
    Dqn_String8 result    = Dqn_String8_Init(string, file_size);
    return result;
}

DQN_API bool Dqn_Fs_WriteCString8(char const *path, Dqn_usize path_size, char const *buffer, Dqn_usize buffer_size)
{
    bool result = false;
    if (!path || !buffer || buffer_size <= 0)
        return result;

    #if defined(DQN_OS_WIN32)
    if (path_size <= 0)
        path_size = Dqn_CString8_Size(path);

    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String8       path8   = Dqn_String8_Init(path, path_size);
    Dqn_String16      path16  = Dqn_Win_String8ToString16(scratch.arena, path8);

    void *file_handle = CreateFileW(/*LPCWSTR               lpFileName*/            path16.data,
                                    /*DWORD                 dwDesiredAccess*/       GENERIC_WRITE,
                                    /*DWORD                 dwShareMode*/           0,
                                    /*LPSECURITY_ATTRIBUTES lpSecurityAttributes*/  nullptr,
                                    /*DWORD                 dwCreationDisposition*/ CREATE_ALWAYS,
                                    /*DWORD                 dwFlagsAndAttributes*/  FILE_ATTRIBUTE_NORMAL,
                                    /*HANDLE                hTemplateFile*/         nullptr);

    if (file_handle == INVALID_HANDLE_VALUE) {
        Dqn_WinError error = Dqn_Win_LastError(scratch.arena);
        Dqn_Log_ErrorF("Failed to open file for writing [file=%.*s, reason=%.*s]", DQN_STRING_FMT(path8), DQN_STRING_FMT(error.msg));
        return result;
    }
    DQN_DEFER { CloseHandle(file_handle); };

    unsigned long bytes_written = 0;
    result                      = WriteFile(file_handle, buffer, DQN_CAST(unsigned long)buffer_size, &bytes_written, nullptr /*lpOverlapped*/);
    DQN_ASSERT(bytes_written == buffer_size);
    return result;
    #else
    // TODO(dqn): Use OS apis
    (void)path_size;

    FILE *file_handle = nullptr;
    fopen_s(&file_handle, path, "w+b");
    if (!file_handle) {
        Dqn_Log_ErrorF("Failed to 'fopen' to get the file handle [file=%s]", path);
        return result;
    }
    DQN_DEFER { fclose(file_handle); };

    result = fwrite(buffer, buffer_size, 1 /*count*/, file_handle) == 1 /*count*/;
    if (!result)
        Dqn_Log_ErrorF("Failed to 'fwrite' memory to file [file=%s]", path);

    return result;
    #endif
}

DQN_API bool Dqn_Fs_Write(Dqn_String8 file_path, Dqn_String8 buffer)
{
    bool result = Dqn_Fs_WriteCString8(file_path.data, file_path.size, buffer.data, buffer.size);
    return result;
}

// NOTE: R/W Stream API ============================================================================
DQN_API Dqn_FsFile Dqn_Fs_OpenFile(Dqn_String8 path, Dqn_FsFileOpen open_mode, uint32_t access)
{
    Dqn_FsFile result = {};
    if (!Dqn_String8_IsValid(path) || path.size <= 0)
        return result;

    if ((access & ~Dqn_FsFileAccess_All) || ((access & Dqn_FsFileAccess_All) == 0)) {
        DQN_INVALID_CODE_PATH;
        return result;
    }

    #if defined(DQN_OS_WIN32)
    unsigned long create_flag = 0;
    switch (open_mode) {
        case Dqn_FsFileOpen_CreateAlways: create_flag = CREATE_ALWAYS; break;
        case Dqn_FsFileOpen_OpenIfExist:  create_flag = OPEN_EXISTING; break;
        case Dqn_FsFileOpen_OpenAlways:   create_flag = OPEN_ALWAYS;   break;
        default: DQN_INVALID_CODE_PATH; return result;
    }

    unsigned long access_mode = 0;
    if (access & Dqn_FsFileAccess_AppendOnly) {
        DQN_ASSERTF((access & ~Dqn_FsFileAccess_AppendOnly) == 0,
                    "Append can only be applied exclusively to the file, other access modes not permitted");
        access_mode = FILE_APPEND_DATA;
    } else {
        if (access & Dqn_FsFileAccess_Read)
            access_mode |= GENERIC_READ;
        if (access & Dqn_FsFileAccess_Write)
            access_mode |= GENERIC_WRITE;
        if (access & Dqn_FsFileAccess_Execute)
            access_mode |= GENERIC_EXECUTE;
    }

    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String16      path16  = Dqn_Win_String8ToString16(scratch.arena, path);
    void *handle              = CreateFileW(/*LPCWSTR               lpFileName*/            path16.data,
                                            /*DWORD                 dwDesiredAccess*/       access_mode,
                                            /*DWORD                 dwShareMode*/           0,
                                            /*LPSECURITY_ATTRIBUTES lpSecurityAttributes*/  nullptr,
                                            /*DWORD                 dwCreationDisposition*/ create_flag,
                                            /*DWORD                 dwFlagsAndAttributes*/  FILE_ATTRIBUTE_NORMAL,
                                            /*HANDLE                hTemplateFile*/         nullptr);

    if (handle == INVALID_HANDLE_VALUE) {
        Dqn_WinError error = Dqn_Win_LastError(scratch.arena);
        result.error_size =
            DQN_CAST(uint16_t) Dqn_SNPrintFDotTruncate(result.error,
                                                       DQN_ARRAY_UCOUNT(result.error),
                                                       "Open file failed: %.*s for \"%.*s\"",
                                                       DQN_STRING_FMT(error.msg),
                                                       DQN_STRING_FMT(path));
        return result;
    }
    #else
    if (access & Dqn_FsFileAccess_Execute) {
        result.error_size = DQN_CAST(uint16_t) Dqn_SNPrintFDotTruncate(
            result.error,
            DQN_ARRAY_UCOUNT(result.error),
            "Open file failed: execute access not supported for \"%.*s\"",
            DQN_STRING_FMT(path));
        DQN_INVALID_CODE_PATH; // TODO: Not supported via fopen
        return result;
    }

    // NOTE: fopen interface is not as expressive as the Win32
    // We will fopen the file beforehand to setup the state/check for validity
    // before closing and reopening it if valid with the correct request access
    // permissions.
    {
        void *handle = nullptr;
        switch (open_mode) {
            case Dqn_FsFileOpen_CreateAlways: handle = fopen(path, "w"); break;
            case Dqn_FsFileOpen_OpenIfExist:  handle = fopen(path, "r"); break;
            case Dqn_FsFileOpen_OpenAlways:   handle = fopen(path, "a"); break;
            default: DQN_INVALID_CODE_PATH; break;
        }
        if (!handle) {
            result.error_size = DQN_CAST(uint16_t) Dqn_SNPrintF2DotsOnOverflow(
                result.error,
                DQN_ARRAY_UCOUNT(result.error),
                "Open file failed: Could not open file in requested mode %d for \"%.*s\"",
                open_mode,
                DQN_STRING_FMT(path));
            return result;
        }
        fclose(handle);
    }

    char const *fopen_mode = nullptr;
    if (access & Dqn_FsFileAccess_AppendOnly) {
        fopen_mode = "a+";
    } else if (access & Dqn_FsFileAccess_Write) {
        fopen_mode = "w+";
    } else if (access & Dqn_FsFileAccess_Read) {
        fopen_mode = "r+";
    }

    void *handle = fopen(path, fopen_mode);
    if (!handle) {
        result.error_size = DQN_CAST(uint16_t) Dqn_SNPrintF2DotsOnOverflow(
            result.error,
            DQN_ARRAY_UCOUNT(result.error),
            "Open file failed: Could not open file in fopen mode \"%s\" for \"%.*s\"",
            fopen_mode,
            DQN_STRING_FMT(path));
        return result;
    }
    #endif
    result.handle = handle;
    return result;
}

DQN_API bool Dqn_Fs_WriteFile(Dqn_FsFile *file, char const *buffer, Dqn_usize size)
{
    if (!file || !file->handle || !buffer || size <= 0 || file->error_size)
        return false;

    bool result = true;
    #if defined(DQN_OS_WIN32)
    char const *end = buffer + size;
    for (char const *ptr = buffer; result && ptr != end; ) {
        unsigned long write_size = DQN_CAST(unsigned long)DQN_MIN((unsigned long)-1, end - ptr);
        unsigned long bytes_written  = 0;
        result = WriteFile(file->handle, ptr, write_size, &bytes_written, nullptr /*lpOverlapped*/) != 0;
        ptr   += bytes_written;
    }

    if (!result) {
        Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
        Dqn_WinError error = Dqn_Win_LastError(scratch.arena);
        file->error_size =
            DQN_CAST(uint16_t) Dqn_SNPrintFDotTruncate(file->error,
                                                       DQN_ARRAY_UCOUNT(file->error),
                                                       "Write file failed: %.*s for %.*s",
                                                       DQN_STRING_FMT(error.msg));
    }
    #else
    result = fwrite(buffer, DQN_CAST(Dqn_usize)size, 1 /*count*/, file->handle) == 1 /*count*/;
    #endif
    return result;
}

DQN_API void Dqn_Fs_CloseFile(Dqn_FsFile *file)
{
    if (!file || !file->handle || file->error_size)
        return;

    #if defined(DQN_OS_WIN32)
    CloseHandle(file->handle);
    #else
    fclose(file->handle);
    #endif
    *file = {};
}
#endif // !defined(DQN_NO_FS)

DQN_API bool Dqn_FsPath_AddRef(Dqn_Arena *arena, Dqn_FsPath *fs_path, Dqn_String8 path)
{
    if (!arena || !fs_path || !Dqn_String8_IsValid(path))
        return false;

    if (path.size <= 0)
        return true;

    Dqn_String8 const delimiter_array[] = {
        DQN_STRING8("\\"),
        DQN_STRING8("/")
    };
    for (;;) {
        Dqn_String8BinarySplitResult delimiter = Dqn_String8_BinarySplitArray(path, delimiter_array, DQN_ARRAY_UCOUNT(delimiter_array));
        for (; delimiter.lhs.data; delimiter   = Dqn_String8_BinarySplitArray(delimiter.rhs, delimiter_array, DQN_ARRAY_UCOUNT(delimiter_array))) {
            if (delimiter.lhs.size <= 0)
                continue;

            Dqn_FsPathLink *link = Dqn_Arena_New(arena, Dqn_FsPathLink, Dqn_ZeroMem_Yes);
            if (!link)
                return false;

            link->string = delimiter.lhs;
            link->prev   = fs_path->tail;
            if (fs_path->tail) {
                fs_path->tail->next = link;
            } else {
                fs_path->head = link;
            }
            fs_path->tail         = link;
            fs_path->links_size  += 1;
            fs_path->string_size += delimiter.lhs.size;
        }

        if (!delimiter.lhs.data)
            break;
    }

    return true;
}

DQN_API bool Dqn_FsPath_Add(Dqn_Arena *arena, Dqn_FsPath *fs_path, Dqn_String8 path)
{
    Dqn_String8 copy = Dqn_String8_Copy(Dqn_Arena_Allocator(arena), path);
    bool result      = Dqn_FsPath_AddRef(arena, fs_path, copy);
    return result;
}

DQN_API bool Dqn_FsPath_AddF(Dqn_Arena *arena, Dqn_FsPath *fs_path, DQN_FMT_STRING_ANNOTATE char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_String8 path = Dqn_String8_InitFV(Dqn_Arena_Allocator(arena), fmt, args);
    va_end(args);
    bool result = Dqn_FsPath_AddRef(arena, fs_path, path);
    return result;
}

DQN_API bool Dqn_FsPath_Pop(Dqn_FsPath *fs_path)
{
    if (!fs_path)
        return false;

    if (fs_path->tail) {
        DQN_ASSERT(fs_path->head);
        fs_path->links_size  -= 1;
        fs_path->string_size -= fs_path->tail->string.size;
        fs_path->tail         = fs_path->tail->prev;
        if (fs_path->tail) {
            fs_path->tail->next = nullptr;
        } else {
            fs_path->head = nullptr;
        }
    } else {
        DQN_ASSERT(!fs_path->head);
    }

    return true;
}

DQN_API Dqn_String8 Dqn_FsPath_Convert(Dqn_Arena *arena, Dqn_String8 path)
{
    Dqn_FsPath fs_path = {};
    Dqn_FsPath_AddRef(arena, &fs_path, path);
    Dqn_String8 result = Dqn_FsPath_Build(arena, &fs_path);
    return result;
}

DQN_API Dqn_String8 Dqn_FsPath_ConvertF(Dqn_Arena *arena, DQN_FMT_STRING_ANNOTATE char const *fmt, ...)
{
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(arena);
    va_list args;
    va_start(args, fmt);
    Dqn_String8 path = Dqn_String8_InitFV(scratch.allocator, fmt, args);
    va_end(args);
    Dqn_String8 result = Dqn_FsPath_Convert(arena, path);
    return result;
}

DQN_API Dqn_String8 Dqn_FsPath_BuildWithSeparator(Dqn_Arena *arena, Dqn_FsPath const *fs_path, Dqn_String8 path_separator)
{
    Dqn_String8 result = {};
    if (!fs_path || fs_path->links_size <= 0)
        return result;

    // NOTE: Each link except the last one needs the path separator appended to it, '/' or '\\'
    Dqn_usize string_size = fs_path->string_size + ((fs_path->links_size - 1) * path_separator.size);
    result                = Dqn_String8_Allocate(Dqn_Arena_Allocator(arena), string_size, Dqn_ZeroMem_No);
    if (result.data) {
        char *dest = result.data;
        for (Dqn_FsPathLink *link = fs_path->head; link; link = link->next) {
            Dqn_String8 string = link->string;
            DQN_MEMCPY(dest, string.data, string.size);
            dest += string.size;

            if (link != fs_path->tail) {
                DQN_MEMCPY(dest, path_separator.data, path_separator.size);
                dest += path_separator.size;
            }
        }
    }

    result.data[string_size] = 0;
    return result;
}

// NOTE: [$DATE] Dqn_Date ==========================================================================
DQN_API Dqn_DateHMSTime Dqn_Date_HMSLocalTimeNow()
{
    Dqn_DateHMSTime result = {};
#if defined(DQN_OS_WIN32)
    SYSTEMTIME sys_time;
    GetLocalTime(&sys_time);
    result.hour    = DQN_CAST(uint8_t)sys_time.wHour;
    result.minutes = DQN_CAST(uint8_t)sys_time.wMinute;
    result.seconds = DQN_CAST(uint8_t)sys_time.wSecond;

    result.day   = DQN_CAST(uint8_t)sys_time.wDay;
    result.month = DQN_CAST(uint8_t)sys_time.wMonth;
    result.year  = DQN_CAST(int16_t)sys_time.wYear;
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    // NOTE: localtime_r is used because it is thread safe
    // See: https://linux.die.net/man/3/localtime
    // According to POSIX.1-2004, localtime() is required to behave as though
    // tzset(3) was called, while localtime_r() does not have this requirement.
    // For portable code tzset(3) should be called before localtime_r().
    for (static bool once = true; once; once = false)
        tzset();

    struct tm time = {};
    localtime_r(&ts.tv_sec, &time);

    result.hour    = time.tm_hour;
    result.minutes = time.tm_min;
    result.seconds = time.tm_sec;

    result.day   = DQN_CAST(uint8_t)time.tm_mday;
    result.month = DQN_CAST(uint8_t)time.tm_mon + 1;
    result.year  = 1900 + DQN_CAST(int16_t)time.tm_year;
#endif

    return result;
}

DQN_API Dqn_DateHMSTimeString Dqn_Date_HMSLocalTimeString(Dqn_DateHMSTime time, char date_separator, char hms_separator)
{
    Dqn_DateHMSTimeString result = {};
    result.hms_size              = DQN_CAST(uint8_t) STB_SPRINTF_DECORATE(snprintf)(result.hms,
                                                                                    DQN_ARRAY_ICOUNT(result.hms),
                                                                                    "%02d%c%02d%c%02d",
                                                                                    time.hour,
                                                                                    hms_separator,
                                                                                    time.minutes,
                                                                                    hms_separator,
                                                                                    time.seconds);

    result.date_size = DQN_CAST(uint8_t) STB_SPRINTF_DECORATE(snprintf)(result.date,
                                                                       DQN_ARRAY_ICOUNT(result.date),
                                                                       "%d%c%02d%c%02d",
                                                                       time.year,
                                                                       date_separator,
                                                                       time.month,
                                                                       date_separator,
                                                                       time.day);

    DQN_ASSERT(result.hms_size < DQN_ARRAY_UCOUNT(result.hms));
    DQN_ASSERT(result.date_size < DQN_ARRAY_UCOUNT(result.date));
    return result;
}

DQN_API Dqn_DateHMSTimeString Dqn_Date_HMSLocalTimeStringNow(char date_separator, char hms_separator)
{
    Dqn_DateHMSTime time         = Dqn_Date_HMSLocalTimeNow();
    Dqn_DateHMSTimeString result = Dqn_Date_HMSLocalTimeString(time, date_separator, hms_separator);
    return result;
}

DQN_API uint64_t Dqn_Date_EpochTime()
{
#if defined(DQN_OS_WIN32)
   const uint64_t UNIX_TIME_START  = 0x019DB1DED53E8000; //January 1, 1970 (start of Unix epoch) in "ticks"
   const uint64_t TICKS_PER_SECOND = 10'000'000; // Filetime returned is in intervals of 100 nanoseconds

   FILETIME file_time;
   GetSystemTimeAsFileTime(&file_time);

   LARGE_INTEGER date_time;
   date_time.LowPart  = file_time.dwLowDateTime;
   date_time.HighPart = file_time.dwHighDateTime;
   uint64_t result     = (date_time.QuadPart - UNIX_TIME_START) / TICKS_PER_SECOND;

#elif defined(DQN_OS_UNIX)
    uint64_t result = time(nullptr);
#else
    #error Unimplemented
#endif

    return result;
}

#if defined(DQN_OS_WIN32)
#if !defined(DQN_NO_WIN)
// NOTE: [$WIND] Dqn_Win ===========================================================================
DQN_API Dqn_WinError Dqn_Win_LastError(Dqn_Arena *arena)
{
    Dqn_WinError result = {};
    result.code        = GetLastError();
    if (arena) {
        unsigned long flags             = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
        void *module_to_get_errors_from = nullptr;

        if (result.code >= 12000 && result.code <= 12175) { // WinINET Errors
            flags |= FORMAT_MESSAGE_FROM_HMODULE;
            module_to_get_errors_from = GetModuleHandleA("wininet.dll");
        }

        int32_t size = FormatMessageA(flags,
                                      module_to_get_errors_from,   // LPCVOID lpSource,
                                      result.code,                 // unsigned long   dwMessageId,
                                      0,                           // unsigned long   dwLanguageId,
                                      nullptr,                     // LPSTR   lpBuffer,
                                      0,                           // unsigned long   nSize,
                                      nullptr);                    // va_list * Arguments

        if (size) {
            Dqn_String8 buffer = Dqn_String8_Allocate(Dqn_Arena_Allocator(arena), size, Dqn_ZeroMem_No);
            int32_t buffer_size = DQN_CAST(int32_t)buffer.size;
            int32_t new_size   = FormatMessageA(flags,
                                                module_to_get_errors_from, // LPCVOID lpSource,
                                                result.code,               // unsigned long   dwMessageId,
                                                0,                         // unsigned long   dwLanguageId,
                                                buffer.data,               // LPSTR   lpBuffer,
                                                buffer_size,               // unsigned long   nSize,
                                                nullptr);                  // va_list * Arguments
            if (DQN_CHECK(new_size == size)) {
                result.msg = buffer;
            }
        }
    }
    return result;
}

DQN_API void Dqn_Win_MakeProcessDPIAware()
{
    typedef bool SetProcessDpiAwareProc(void);
    typedef bool SetProcessDpiAwarenessProc(PROCESS_DPI_AWARENESS);
    typedef bool SetProcessDpiAwarenessContextProc(void * /*DPI_AWARENESS_CONTEXT*/);

    // NOTE(doyle): Taken from cmuratori/refterm snippet on DPI awareness. It
    // appears we can make this robust by just loading user32.dll and using
    // GetProcAddress on the DPI function. If it's not there, we're on an old
    // version of windows, so we can call an older version of the API.
    void *lib_handle = LoadLibraryA("user32.dll");
    if (!lib_handle)
        return;

    if (auto *set_process_dpi_awareness_context = DQN_CAST(SetProcessDpiAwarenessContextProc *)GetProcAddress(DQN_CAST(HMODULE)lib_handle, "SetProcessDpiAwarenessContext")) {
        set_process_dpi_awareness_context(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    } else if (auto *set_process_dpi_awareness = DQN_CAST(SetProcessDpiAwarenessProc *)GetProcAddress(DQN_CAST(HMODULE)lib_handle, "SetProcessDpiAwareness")) {
        set_process_dpi_awareness(PROCESS_PER_MONITOR_DPI_AWARE);
    } else if (auto *set_process_dpi_aware = DQN_CAST(SetProcessDpiAwareProc *)GetProcAddress(DQN_CAST(HMODULE)lib_handle, "SetProcessDpiAware")) {
        set_process_dpi_aware();
    }
}

// NOTE: Windows UTF8 to String16 ==============================================
DQN_API Dqn_String16 Dqn_Win_String8ToString16(Dqn_Arena *arena, Dqn_String8 src)
{
    Dqn_String16 result = {};
    if (!arena || !Dqn_String8_IsValid(src))
        return result;

    int required_size = MultiByteToWideChar(CP_UTF8, 0 /*dwFlags*/, src.data, DQN_CAST(int)src.size, nullptr /*dest*/, 0 /*dest size*/);
    if (required_size <= 0)
        return result;

    wchar_t *buffer = Dqn_Arena_NewArray(arena, wchar_t, required_size + 1, Dqn_ZeroMem_No);
    if (!buffer)
        return result;

    int chars_written = MultiByteToWideChar(CP_UTF8, 0 /*dwFlags*/, src.data, DQN_CAST(int)src.size, buffer, required_size);
    if (DQN_CHECK(chars_written == required_size)) {
        result.data              = buffer;
        result.size              = chars_written;
        result.data[result.size] = 0;
    }
    return result;
}

DQN_API int Dqn_Win_String8ToString16Buffer(Dqn_String8 src, wchar_t *dest, Dqn_usize dest_size)
{
    int result = 0;
    if (!Dqn_String8_IsValid(src))
        return result;

    result = MultiByteToWideChar(CP_UTF8, 0 /*dwFlags*/, src.data, DQN_CAST(int)src.size, nullptr /*dest*/, 0 /*dest size*/);
    if (result <= 0 || result > dest_size || !dest)
        return result;

    result = MultiByteToWideChar(CP_UTF8, 0 /*dwFlags*/, src.data, DQN_CAST(int)src.size, dest, DQN_CAST(int)dest_size);
    dest[DQN_MIN(result, dest_size - 1)] = 0;
    return result;
}

// NOTE: Windows String16 To UTF8 ==================================================================
DQN_API int Dqn_Win_String16ToString8Buffer(Dqn_String16 src, char *dest, Dqn_usize dest_size)
{
    int result = 0;
    if (!Dqn_String8_IsValid(src))
        return result;

    int src_size = Dqn_Safe_SaturateCastISizeToInt(src.size);
    if (src_size <= 0)
        return result;

    result = WideCharToMultiByte(CP_UTF8, 0 /*dwFlags*/, src.data, src_size, nullptr /*dest*/, 0 /*dest size*/, nullptr, nullptr);
    if (result <= 0 || result > dest_size || !dest)
        return result;

    result = WideCharToMultiByte(CP_UTF8, 0 /*dwFlags*/, src.data, src_size, dest, DQN_CAST(int)dest_size, nullptr, nullptr);
    dest[DQN_MIN(result, dest_size - 1)] = 0;
    return result;
}

DQN_API Dqn_String8 Dqn_Win_String16ToString8(Dqn_Arena *arena, Dqn_String16 src)
{
    Dqn_String8 result = {};
    if (!arena || !Dqn_String8_IsValid(src))
        return result;

    int src_size = Dqn_Safe_SaturateCastISizeToInt(src.size);
    if (src_size <= 0)
        return result;

    int required_size = WideCharToMultiByte(CP_UTF8, 0 /*dwFlags*/, src.data, src_size, nullptr /*dest*/, 0 /*dest size*/, nullptr, nullptr);
    if (required_size <= 0)
        return result;

    // NOTE: String allocate ensures there's one extra byte for
    // null-termination already so no-need to +1 the required size
    Dqn_ArenaTempMemory temp_memory = Dqn_Arena_BeginTempMemory(arena);
    Dqn_String8 buffer = Dqn_String8_Allocate(Dqn_Arena_Allocator(arena), required_size, Dqn_ZeroMem_No);
    if (!Dqn_String8_IsValid(buffer))
        return result;

    int chars_written = WideCharToMultiByte(CP_UTF8, 0 /*dwFlags*/, src.data, src_size, buffer.data, DQN_CAST(int)buffer.size, nullptr, nullptr);
    if (DQN_CHECK(chars_written == required_size)) {
        result                   = buffer;
        result.data[result.size] = 0;
    } else {
        // NOTE: Revert the temp memory because we encountered an error
        Dqn_Arena_EndTempMemory(temp_memory, false /*cancel*/);
    }

    return result;
}

// NOTE: Windows Executable Directory ==========================================
DQN_API Dqn_String16 Dqn_Win_EXEDirW(Dqn_Arena *arena)
{
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(arena);
    Dqn_String16 result       = {};
    Dqn_usize module_size     = 0;
    wchar_t *module_path      = nullptr;
    do {
        module_size += 256;
        module_path = Dqn_Arena_NewArray(scratch.arena, wchar_t, module_size, Dqn_ZeroMem_No);
        if (!module_path)
            return result;
        module_size = DQN_CAST(Dqn_usize)GetModuleFileNameW(nullptr /*module*/, module_path, DQN_CAST(int)module_size);
    } while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

    Dqn_usize index_of_last_slash = 0;
    for (Dqn_usize index = module_size - 1; !index_of_last_slash && index < module_size; index--)
        index_of_last_slash = module_path[index] == '\\' ? index : 0;

    result.data = Dqn_Arena_NewArray(arena, wchar_t, index_of_last_slash + 1, Dqn_ZeroMem_No);
    result.size = index_of_last_slash;
    DQN_MEMCPY(result.data, module_path, sizeof(wchar_t) * result.size);
    result.data[result.size] = 0;
    return result;
}

DQN_API Dqn_String8 Dqn_Win_WorkingDir(Dqn_Allocator allocator, Dqn_String8 suffix)
{
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(allocator.user_context);
    Dqn_String16 suffix16     = Dqn_Win_String8ToString16(scratch.arena, suffix);
    Dqn_String16 dir16        = Dqn_Win_WorkingDirW(Dqn_Arena_Allocator(scratch.arena), suffix16);
    Dqn_String8 result        = Dqn_Win_String16ToString8(scratch.arena, dir16);
    return result;
}

DQN_API Dqn_String16 Dqn_Win_WorkingDirW(Dqn_Allocator allocator, Dqn_String16 suffix)
{
    DQN_ASSERT(suffix.size >= 0);
    Dqn_String16 result = {};

    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(allocator.user_context);

    // NOTE: required_size is the size required *including* the null-terminator
    unsigned long required_size = GetCurrentDirectoryW(0, nullptr);
    unsigned long desired_size  = required_size + DQN_CAST(unsigned long) suffix.size;

    wchar_t *scratch_w_path = Dqn_Arena_NewArray(scratch.arena, wchar_t, desired_size, Dqn_ZeroMem_No);
    if (!scratch_w_path)
        return result;

    unsigned long bytes_written_wo_null_terminator = GetCurrentDirectoryW(desired_size, scratch_w_path);
    if ((bytes_written_wo_null_terminator + 1) != required_size) {
        // TODO(dqn): Error
        return result;
    }

    wchar_t *w_path = Dqn_Allocator_NewArray(allocator, wchar_t, desired_size, Dqn_ZeroMem_No);
    if (!w_path)
        return result;

    if (suffix.size) {
        DQN_MEMCPY(w_path, scratch_w_path, sizeof(*scratch_w_path) * bytes_written_wo_null_terminator);
        DQN_MEMCPY(w_path + bytes_written_wo_null_terminator, suffix.data, sizeof(suffix.data[0]) * suffix.size);
        w_path[desired_size] = 0;
    }

    result = Dqn_String16{w_path, DQN_CAST(Dqn_usize)(desired_size - 1)};
    return result;
}

DQN_API bool Dqn_Win_FolderWIterate(Dqn_String16 path, Dqn_Win_FolderIteratorW *it)
{
    WIN32_FIND_DATAW find_data = {};
    if (it->handle) {
        if (FindNextFileW(it->handle, &find_data) == 0)
            return false;
    } else {
        it->handle = FindFirstFileExW(path.data,             /*LPCWSTR lpFileName,*/
                                      FindExInfoStandard,    /*FINDEX_INFO_LEVELS fInfoLevelId,*/
                                      &find_data,            /*LPVOID lpFindFileData,*/
                                      FindExSearchNameMatch, /*FINDEX_SEARCH_OPS fSearchOp,*/
                                      nullptr,               /*LPVOID lpSearchFilter,*/
                                      FIND_FIRST_EX_LARGE_FETCH /*unsigned long dwAdditionalFlags)*/);

        if (it->handle == INVALID_HANDLE_VALUE)
            return false;
    }

    it->file_name_buf[0] = 0;
    it->file_name        = Dqn_String16{it->file_name_buf, 0};

    do {
        if (find_data.cFileName[0] == '.' || (find_data.cFileName[0] == '.' && find_data.cFileName[1] == '.'))
            continue;

        it->file_name.size = Dqn_CString16_Size(find_data.cFileName);
        DQN_ASSERT(it->file_name.size < (DQN_ARRAY_UCOUNT(it->file_name_buf) - 1));
        DQN_MEMCPY(it->file_name.data, find_data.cFileName, it->file_name.size * sizeof(wchar_t));
        it->file_name_buf[it->file_name.size] = 0;
        break;
    } while (FindNextFileW(it->handle, &find_data) != 0);

    return it->file_name.size > 0;
}

DQN_API bool Dqn_Win_FolderIterate(Dqn_String8 path, Dqn_Win_FolderIterator *it)
{
    if (!Dqn_String8_IsValid(path) || !it || path.size <= 0)
        return false;

    Dqn_ThreadScratch scratch       = Dqn_Thread_GetScratch(nullptr);
    Dqn_Win_FolderIteratorW wide_it = {};
    Dqn_String16 path16             = {};
    if (it->handle) {
        wide_it.handle = it->handle;
    } else {
        bool needs_asterisks = Dqn_String8_EndsWith(path, DQN_STRING8("\\")) ||
                               Dqn_String8_EndsWith(path, DQN_STRING8("/"));
        bool has_glob        = Dqn_String8_EndsWith(path, DQN_STRING8("\\*")) ||
                               Dqn_String8_EndsWith(path, DQN_STRING8("/*"));

        Dqn_String8 adjusted_path = path;
        if (!has_glob) {
            // NOTE: We are missing the glob for enumerating the files, we will
            // add those characters in this branch, so overwrite the null
            // character, add the glob and re-null terminate the buffer.
            if (needs_asterisks)
                adjusted_path = Dqn_FsPath_ConvertF(scratch.arena, "%.*s*", DQN_STRING_FMT(path));
            else
                adjusted_path = Dqn_FsPath_ConvertF(scratch.arena, "%.*s/*", DQN_STRING_FMT(path));
        }

        path16 = Dqn_Win_String8ToString16(scratch.arena, adjusted_path);
        if (path16.size <= 0) // Conversion error
            return false;
    }

    bool result = Dqn_Win_FolderWIterate(path16, &wide_it);
    it->handle  = wide_it.handle;
    if (result) {
        int size      = Dqn_Win_String16ToString8Buffer(wide_it.file_name, it->file_name_buf, DQN_ARRAY_UCOUNT(it->file_name_buf));
        it->file_name = Dqn_String8_Init(it->file_name_buf, size);
    }

    return result;
}
#endif // !defined(DQN_NO_WIN)

#if !defined(DQN_NO_WINNET)
// NOTE: [$WINN] Dqn_WinNet ========================================================================
DQN_API Dqn_WinNetHandle Dqn_Win_NetHandleInitCString(char const *url, int url_size)
{
    URL_COMPONENTSA components  = {};
    components.dwStructSize     = sizeof(components);
    components.dwHostNameLength = url_size;
    components.dwUrlPathLength  = url_size;

    // Seperate the URL into bits and bobs
    Dqn_WinNetHandle result = {};
    if (!InternetCrackUrlA(url, url_size, 0 /*flags*/, &components)) {
        Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
        Dqn_WinError error        = Dqn_Win_LastError(scratch.arena);
        Dqn_Log_ErrorF("InternetCrackUrlA failed [reason=%.*s]", DQN_STRING_FMT(error.msg));
        return result;
    }

    if (components.lpszHostName == nullptr) {
        Dqn_Log_ErrorF("Windows returnd a null host-name after trying to crack the URL.\n\nURL: %.*s", url_size, url);
        return result;
    }

    if (url[url_size] != 0) {
        Dqn_Log_ErrorF("URL '%.*s' must be null-terminated", url_size, url);
        return result;
    }

    if (components.dwHostNameLength > (DQN_ARRAY_UCOUNT(result.host_name) - 1)) {
        Dqn_Log_ErrorF("Host name is longer than the maximum supported [max=%d]", DQN_ARRAY_UCOUNT(result.host_name) - 1);
        return result;
    }

    result.host_name_size = components.dwHostNameLength;
    DQN_MEMCPY(result.host_name, components.lpszHostName, result.host_name_size);
    result.host_name[result.host_name_size] = 0;

    result.url_size = components.dwUrlPathLength;
    result.url      = components.lpszUrlPath;

    // Create the Win32 networking handles we need
    result.internet_open_handle = InternetOpenA("Generic/Win32",
                                                INTERNET_OPEN_TYPE_PRECONFIG,
                                                nullptr /*proxy*/,
                                                nullptr /*proxy bypass*/,
                                                0 /*flags*/);

    result.internet_connect_handle = InternetConnectA(result.internet_open_handle,
                                                      result.host_name,
                                                      INTERNET_DEFAULT_HTTPS_PORT,
                                                      nullptr /*username*/,
                                                      nullptr /*password*/,
                                                      INTERNET_SERVICE_HTTP,
                                                      0 /*flags*/,
                                                      0 /*context*/);

    result.state = Dqn_WinNetHandleState_Initialised;
    return result;
}

DQN_API Dqn_WinNetHandle Dqn_Win_NetHandleInit(Dqn_String8 url)
{
    Dqn_WinNetHandle result = Dqn_Win_NetHandleInitCString(url.data, DQN_CAST(int)url.size);
    return result;
}

DQN_API Dqn_WinNetHandle Dqn_Win_NetHandleInitHTTPMethodCString(char const *url, int url_size, char const *http_method)
{
    Dqn_WinNetHandle result = Dqn_Win_NetHandleInitCString(url, url_size);
    Dqn_Win_NetHandleSetHTTPMethod(&result, http_method);
    return result;
}

DQN_API Dqn_WinNetHandle Dqn_Win_NetHandleInitHTTPMethod(Dqn_String8 url, Dqn_String8 http_method)
{
    Dqn_WinNetHandle result = Dqn_Win_NetHandleInit(url);
    Dqn_Win_NetHandleSetHTTPMethod(&result, http_method.data);
    return result;
}

DQN_API void Dqn_Win_NetHandleClose(Dqn_WinNetHandle *handle)
{
    if (!Dqn_Win_NetHandleIsValid(handle))
        return;

    InternetCloseHandle(handle->internet_open_handle);
    InternetCloseHandle(handle->internet_connect_handle);
    InternetCloseHandle(handle->http_handle);
    handle->internet_open_handle    = nullptr;
    handle->internet_connect_handle = nullptr;
    handle->http_handle             = nullptr;
}

DQN_API bool Dqn_Win_NetHandleIsValid(Dqn_WinNetHandle const *handle)
{
    bool result = handle && handle->state >= Dqn_WinNetHandleState_Initialised;
    return result;
}

DQN_API void Dqn_Win_NetHandleSetUserAgentCString(Dqn_WinNetHandle *handle, char const *user_agent, int user_agent_size)
{
    if (!Dqn_Win_NetHandleIsValid(handle))
        return;

    InternetSetOptionA(handle->internet_open_handle, INTERNET_OPTION_USER_AGENT, (void *)user_agent, user_agent_size);
}

DQN_API bool Dqn_Win_NetHandleSetHTTPMethod(Dqn_WinNetHandle *handle,
                                            char const *http_verb)
{
    if (!Dqn_Win_NetHandleIsValid(handle))
        return false;

    if (handle->http_handle) {
        InternetCloseHandle(handle->http_handle);
        handle->http_handle = nullptr;
        handle->state       = Dqn_WinNetHandleState_Initialised;
    }

    if (handle->state != Dqn_WinNetHandleState_Initialised)
        return false;

    handle->http_handle = HttpOpenRequestA(handle->internet_connect_handle,
                                           http_verb,
                                           handle->url,
                                           nullptr /*http version*/,
                                           nullptr /*referrer*/,
                                           nullptr,
                                           INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_SECURE,
                                           0 /*context*/);
    handle->state = Dqn_WinNetHandleState_HttpMethodReady;
    return true;
}

DQN_API bool Dqn_Win_NetHandleSetRequestHeaderCString8(Dqn_WinNetHandle *handle,
                                                       char const *header,
                                                       uint32_t header_size,
                                                       uint32_t mode)
{
    if (!Dqn_Win_NetHandleIsValid(handle) || !header || header_size <= 0)
        return false;

    if (mode >= Dqn_WinNetHandleRequestHeaderFlag_Count)
        return false;

    if (handle->state < Dqn_WinNetHandleState_HttpMethodReady)
        return false;

    if (!DQN_CHECK(handle->http_handle))
        return false;

    unsigned long modifier = 0;
    if (mode == 0) {
    } else if (mode == Dqn_WinNetHandleRequestHeaderFlag_Add) {
        modifier = HTTP_ADDREQ_FLAG_ADD;
    } else if (mode == Dqn_WinNetHandleRequestHeaderFlag_AddIfNew) {
        modifier = HTTP_ADDREQ_FLAG_ADD_IF_NEW;
    } else if (mode == Dqn_WinNetHandleRequestHeaderFlag_Replace) {
        modifier = HTTP_ADDREQ_FLAG_REPLACE;
    } else {
        Dqn_Log_ErrorF("Unrecognised flag for adding a request header");
        return false;
    }

    bool result = HttpAddRequestHeadersA(
      handle->http_handle,
      header,
      header_size,
      modifier
    );

    return result;
}

DQN_API bool Dqn_Win_NetHandleSetRequestHeaderString8(Dqn_WinNetHandle *handle, Dqn_String8 header, uint32_t mode)
{
    bool result = Dqn_Win_NetHandleSetRequestHeaderCString8(handle, header.data, Dqn_Safe_SaturateCastISizeToUInt(header.size), mode);
    return result;
}

DQN_API Dqn_WinNetHandleResponse Dqn_Win_NetHandleSendRequest(Dqn_WinNetHandle *handle, Dqn_Allocator allocator, char const *post_data, unsigned long post_data_size)
{
    Dqn_WinNetHandleResponse result = {};
    if (!Dqn_Win_NetHandleIsValid(handle))
        return result;

    if (handle->state != Dqn_WinNetHandleState_HttpMethodReady)
        return result;

    if (!handle->http_handle)
        return result;

    if (!HttpSendRequestA(handle->http_handle, nullptr /*headers*/, 0 /*headers length*/, (char *)post_data, post_data_size)) {
        handle->state = Dqn_WinNetHandleState_RequestFailed;
        Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
        Dqn_WinError error        = Dqn_Win_LastError(scratch.arena);
        Dqn_Log_ErrorF("Failed to send request to %.*s [reason=%.*s]",
                       handle->host_name_size,
                       handle->host_name,
                       DQN_STRING_FMT(error.msg));
        return result;
    }

    handle->state = Dqn_WinNetHandleState_RequestGood;
    unsigned long buffer_size = 0;
    int query_result          = HttpQueryInfoA(handle->http_handle, HTTP_QUERY_RAW_HEADERS_CRLF, nullptr, &buffer_size, nullptr);
    if (!DQN_CHECK(query_result != ERROR_INSUFFICIENT_BUFFER))
        return result;

    result.raw_headers = Dqn_String8_Allocate(allocator, buffer_size, Dqn_ZeroMem_No);
    if (!result.raw_headers.data)
        return result;

    query_result = HttpQueryInfoA(handle->http_handle, HTTP_QUERY_RAW_HEADERS_CRLF, result.raw_headers.data, &buffer_size, nullptr);
    if (!query_result) {
        Dqn_Allocator_Dealloc(allocator, result.raw_headers.data, buffer_size);
        return result;
    }

    Dqn_String8 delimiter     = DQN_STRING8("\r\n");
    Dqn_usize splits_required = Dqn_String8_Split(result.raw_headers, delimiter, nullptr, 0);
    result.headers            = Dqn_Allocator_NewArray(allocator, Dqn_String8, splits_required, Dqn_ZeroMem_No);
    result.headers_size       = Dqn_String8_Split(result.raw_headers, delimiter, result.headers, splits_required);

    bool found_content_type   = false;
    bool found_content_length = false;
    for (Dqn_usize header_index = 0; header_index < result.headers_size; header_index++) {
        Dqn_String8 header = result.headers[header_index];

        Dqn_String8BinarySplitResult key_value_split = Dqn_String8_BinarySplit(header, DQN_STRING8(":"));
        Dqn_String8 value                            = key_value_split.lhs;
        Dqn_String8 key                              = key_value_split.rhs;

        key   = Dqn_String8_TrimWhitespaceAround(key);
        value = Dqn_String8_TrimWhitespaceAround(value);

        if (Dqn_String8_EqInsensitive(key, DQN_STRING8("Content-Type"))) {
            DQN_ASSERT(!found_content_type);
            if (!found_content_type) {
                found_content_type  = true;
                result.content_type = value;
            }
        } else if (Dqn_String8_EqInsensitive(key, DQN_STRING8("Content-Length"))) {
            DQN_ASSERT(!found_content_length);
            if (!found_content_length) {
                found_content_length = true;
                result.content_length = Dqn_String8_ToU64(value, 0 /*separator*/).value;
            }
        }

        if (found_content_type && found_content_length)
            break;
    }

    return result;
}

DQN_API bool Dqn_Win_NetHandlePump(Dqn_WinNetHandle *handle,
                                   char *dest,
                                   int dest_size,
                                   size_t *download_size)
{
    if (!Dqn_Win_NetHandleIsValid(handle))
        return false;

    if (handle->state != Dqn_WinNetHandleState_RequestGood)
        return false;

    bool result = true;
    unsigned long bytes_read;
    if (InternetReadFile(handle->http_handle, dest, dest_size, &bytes_read)) {
        if (bytes_read == 0)
            result = false;
        *download_size = bytes_read;
    } else {
        *download_size = 0;
        result = false;
    }

    if (!result) {
        // NOTE: If it's false here, we've finished downloading/the pumping the
        // handled finished. We can reset the handle state to allow the user to
        // re-use this handle by calling the function again with new post data.
        // IF they need to set a new URL/resource location then they need to
        // make a new handle for that otherwise they can re-use this handle to
        // hit that same end point.
        handle->state = Dqn_WinNetHandleState_Initialised;
        InternetCloseHandle(handle->http_handle);
        handle->http_handle = nullptr;
    }

    return result;
}

struct Dqn_Win_NetChunk
{
    char             data[DQN_WIN_NET_HANDLE_DOWNLOAD_SIZE];
    size_t           size;
    Dqn_Win_NetChunk *next;
};

DQN_API char *Dqn_Win_NetHandlePumpCString8(Dqn_WinNetHandle *handle, Dqn_Arena *arena, size_t *download_size)
{
    if (handle->state != Dqn_WinNetHandleState_RequestGood)
        return nullptr;

    Dqn_ThreadScratch scratch     = Dqn_Thread_GetScratch(arena);
    size_t total_size             = 0;
    Dqn_Win_NetChunk *first_chunk = nullptr;
    for (Dqn_Win_NetChunk *last_chunk = nullptr;;) {
        Dqn_Win_NetChunk *chunk = Dqn_Arena_New(scratch.arena, Dqn_Win_NetChunk, Dqn_ZeroMem_Yes);
        bool pump_result        = Dqn_Win_NetHandlePump(handle, chunk->data, DQN_WIN_NET_HANDLE_DOWNLOAD_SIZE, &chunk->size);
        if (chunk->size) {
            total_size += chunk->size;
            if (first_chunk) {
                last_chunk->next = chunk;
                last_chunk       = chunk;
            } else {
                first_chunk = chunk;
                last_chunk  = chunk;
            }
        }

        if (!pump_result)
            break;
    }

    char *result     = Dqn_Arena_NewArray(arena, char, total_size + 1 /*null-terminator*/, Dqn_ZeroMem_No);
    char *result_ptr = result;
    for (Dqn_Win_NetChunk *chunk = first_chunk; chunk; chunk = chunk->next) {
        DQN_MEMCPY(result_ptr, chunk->data, chunk->size);
        result_ptr += chunk->size;
    }

    *download_size     = total_size;
    result[total_size] = 0;
    return result;
}

DQN_API Dqn_String8 Dqn_Win_NetHandlePumpString8(Dqn_WinNetHandle *handle, Dqn_Arena *arena)
{
    size_t     size     = 0;
    char *     download = Dqn_Win_NetHandlePumpCString8(handle, arena, &size);
    Dqn_String8 result  = Dqn_String8_Init(download, size);
    return result;
}

DQN_API void Dqn_Win_NetHandlePumpToCRTFile(Dqn_WinNetHandle *handle, FILE *file)
{
    for (bool keep_pumping = true; keep_pumping;) {
        char buffer[DQN_WIN_NET_HANDLE_DOWNLOAD_SIZE];
        size_t buffer_size = 0;
        keep_pumping = Dqn_Win_NetHandlePump(handle, buffer, sizeof(buffer), &buffer_size);
        fprintf(file, "%.*s", (int)buffer_size, buffer);
    }
}

DQN_API char *Dqn_Win_NetHandlePumpToAllocCString(Dqn_WinNetHandle *handle, size_t *download_size)
{
    size_t            total_size  = 0;
    Dqn_Win_NetChunk *first_chunk = nullptr;
    for (Dqn_Win_NetChunk *last_chunk = nullptr;;) {
        auto *chunk      = DQN_CAST(Dqn_Win_NetChunk *)Dqn_VMem_Reserve(sizeof(Dqn_Win_NetChunk), Dqn_VMemCommit_Yes, Dqn_VMemPage_ReadWrite);
        bool pump_result = Dqn_Win_NetHandlePump(handle, chunk->data, DQN_WIN_NET_HANDLE_DOWNLOAD_SIZE, &chunk->size);
        if (chunk->size) {
            total_size += chunk->size;
            if (first_chunk) {
                last_chunk->next = chunk;
                last_chunk       = chunk;
            } else {
                first_chunk = chunk;
                last_chunk  = chunk;
            }
        }

        if (!pump_result)
            break;
    }

    auto *result     = DQN_CAST(char *)Dqn_VMem_Reserve(total_size * sizeof(char), Dqn_VMemCommit_Yes, Dqn_VMemPage_ReadWrite);
    char *result_ptr = result;
    for (Dqn_Win_NetChunk *chunk = first_chunk; chunk;) {
        DQN_MEMCPY(result_ptr, chunk->data, chunk->size);
        result_ptr += chunk->size;

        Dqn_Win_NetChunk *prev_chunk = chunk;
        chunk                        = chunk->next;
        Dqn_VMem_Release(prev_chunk, sizeof(*prev_chunk));
    }

    *download_size     = total_size;
    result[total_size] = 0;
    return result;
}

DQN_API Dqn_String8 Dqn_Win_NetHandlePumpToAllocString(Dqn_WinNetHandle *handle)
{
    size_t     download_size = 0;
    char *     download      = Dqn_Win_NetHandlePumpToAllocCString(handle, &download_size);
    Dqn_String8 result       = Dqn_String8_Init(download, download_size);
    return result;
}
#endif // !defined(DQN_NO_WINNET)
#endif // defined(DQN_OS_WIN32)

// NOTE: [$OSYS] Dqn_OS ============================================================================
DQN_API bool Dqn_OS_SecureRNGBytes(void *buffer, uint32_t size)
{
    if (!buffer || size < 0)
        return false;

    if (size == 0)
        return true;

#if defined(DQN_OS_WIN32)
    bool init = true;
    Dqn_TicketMutex_Begin(&g_dqn_library->win32_bcrypt_rng_mutex);
    if (!g_dqn_library->win32_bcrypt_rng_handle)
    {
        wchar_t const BCRYPT_ALGORITHM[] = L"RNG";
        long /*NTSTATUS*/ init_status = BCryptOpenAlgorithmProvider(&g_dqn_library->win32_bcrypt_rng_handle, BCRYPT_ALGORITHM, nullptr /*implementation*/, 0 /*flags*/);
        if (!g_dqn_library->win32_bcrypt_rng_handle || init_status != 0)
        {
            Dqn_Log_ErrorF("Failed to initialise random number generator, error: %d", init_status);
            init = false;
        }
    }
    Dqn_TicketMutex_End(&g_dqn_library->win32_bcrypt_rng_mutex);

    if (!init)
        return false;

    long gen_status = BCryptGenRandom(g_dqn_library->win32_bcrypt_rng_handle, DQN_CAST(unsigned char *)buffer, size, 0 /*flags*/);
    if (gen_status != 0)
    {
        Dqn_Log_ErrorF("Failed to generate random bytes: %d", gen_status);
        return false;
    }

#else
    DQN_ASSERTF(size <= 32,
                "We can increase this by chunking the buffer and filling 32 bytes at a time. *Nix guarantees 32 "
                "bytes can always be fulfilled by this system at a time");
    // TODO(doyle): https://github.com/jedisct1/libsodium/blob/master/src/libsodium/randombytes/sysrandom/randombytes_sysrandom.c
    // TODO(doyle): https://man7.org/linux/man-pages/man2/getrandom.2.html
    int read_bytes = 0;
    do
    {
        read_bytes = getrandom(buffer, size, 0);

     // NOTE: EINTR can not be triggered if size <= 32 bytes
    } while (read_bytes != size || errno == EAGAIN);
#endif

    return true;
}

#if (defined(DQN_OS_WIN32) && !defined(DQN_NO_WIN)) || !defined(DQN_OS_WIN32)
DQN_API Dqn_String8 Dqn_OS_EXEDir(Dqn_Arena *arena)
{
    Dqn_String8 result = {};
    if (!arena)
        return result;

    #if defined(DQN_OS_WIN32)

    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(arena);
    Dqn_String16 exe_dir16    = Dqn_Win_EXEDirW(scratch.arena);
    result                    = Dqn_Win_String16ToString8(arena, exe_dir16);

    #elif defined(DQN_OS_UNIX)

    int required_size_wo_null_terminator = 0;
    for (int try_size = 128;; try_size *= 2) {
        auto scoped_arena = Dqn_ArenaTempMemoryScope(arena);
        char *try_buf     = Dqn_Arena_NewArray(arena, char, try_size, Dqn_ZeroMem_No);
        int bytes_written = readlink("/proc/self/exe", try_buf, try_size);
        if (bytes_written == -1) {
            // Failed, we're unable to determine the executable directory
            break;
        } else if (bytes_written == try_size) {
            // Try again, if returned size was equal- we may of prematurely
            // truncated according to the man pages
            continue;
        } else {
            // readlink will give us the path to the executable. Once we
            // determine the correct buffer size required to get the full file
            // path, we do some post-processing on said string and extract just
            // the directory.

            // TODO(dqn): It'd be nice if there's some way of keeping this
            // try_buf around, memcopy the byte and trash the try_buf from the
            // arena. Instead we just get the size and redo the call one last
            // time after this "calculate" step.
            DQN_ASSERTF(bytes_written < try_size, "bytes_written can never be greater than the try size, function writes at most try_size");
            required_size_wo_null_terminator = bytes_written;

            for (Dqn_isize index_of_last_slash = bytes_written; index_of_last_slash >= 0; index_of_last_slash--) {
                if (try_buf[index_of_last_slash] == '/') {
                    // NOTE: We take the index of the last slash and not
                    // (index_of_last_slash + 1) because we want to exclude the
                    // trailing backslash as a convention of this library.
                    required_size_wo_null_terminator = index_of_last_slash;
                    break;
                }
            }
            break;
        }
    }

    if (required_size_wo_null_terminator) {
        Dqn_ArenaTempMemory scope = Dqn_Arena_BeginTempMemory(arena);
        char *exe_path = Dqn_Arena_NewArray(arena, char, required_size_wo_null_terminator + 1, Dqn_ZeroMem_No);
        exe_path[required_size_wo_null_terminator] = 0;

        int bytes_written = readlink("/proc/self/exe", exe_path, required_size_wo_null_terminator);
        if (bytes_written == -1) {
            // Note that if read-link fails again can be because there's
            // a potential race condition here, our exe or directory could have
            // been deleted since the last call, so we need to be careful.
            Dqn_Arena_EndTempMemory(scope);
        } else {
            result = Dqn_String8_Init(exe_path, required_size_wo_null_terminator);
        }
    }
    #else
    #error Unimplemented
    #endif
    return result;
}
#endif

DQN_API void Dqn_OS_SleepMs(Dqn_uint milliseconds)
{
    #if defined(DQN_OS_WIN32)
    Sleep(milliseconds);
    #else
    struct timespec ts;
    ts.tv_sec  = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1'000'000;
    nanosleep(&ts, nullptr);
    #endif
}

DQN_FILE_SCOPE void Dqn_OS_PerfCounter_Init()
{
    // TODO(doyle): Move this to Dqn_Library_Init
    #if defined(DQN_OS_WIN32)
    if (g_dqn_library->win32_qpc_frequency.QuadPart == 0)
        QueryPerformanceFrequency(&g_dqn_library->win32_qpc_frequency);
    #endif
}

DQN_API Dqn_f64 Dqn_OS_PerfCounterS(uint64_t begin, uint64_t end)
{
    Dqn_OS_PerfCounter_Init();
    uint64_t ticks  = end - begin;
    #if defined(DQN_OS_WIN32)
    Dqn_f64 result = ticks / DQN_CAST(Dqn_f64)g_dqn_library->win32_qpc_frequency.QuadPart;
    #else
    Dqn_f64 result = ticks / 1'000'000'000;
    #endif
    return result;
}

DQN_API Dqn_f64 Dqn_OS_PerfCounterMs(uint64_t begin, uint64_t end)
{
    Dqn_OS_PerfCounter_Init();
    uint64_t ticks  = end - begin;
    #if defined(DQN_OS_WIN32)
    Dqn_f64 result = (ticks * 1'000) / DQN_CAST(Dqn_f64)g_dqn_library->win32_qpc_frequency.QuadPart;
    #else
    Dqn_f64 result = ticks / DQN_CAST(Dqn_f64)1'000'000;
    #endif
    return result;
}

DQN_API Dqn_f64 Dqn_OS_PerfCounterMicroS(uint64_t begin, uint64_t end)
{
    Dqn_OS_PerfCounter_Init();
    uint64_t ticks  = end - begin;
    #if defined(DQN_OS_WIN32)
    Dqn_f64 result = (ticks * 1'000'000) / DQN_CAST(Dqn_f64)g_dqn_library->win32_qpc_frequency.QuadPart;
    #else
    Dqn_f64 result = ticks / DQN_CAST(Dqn_f64)1'000;
    #endif
    return result;
}

DQN_API Dqn_f64 Dqn_OS_PerfCounterNs(uint64_t begin, uint64_t end)
{
    Dqn_OS_PerfCounter_Init();
    uint64_t ticks  = end - begin;
    #if defined(DQN_OS_WIN32)
    Dqn_f64 result = (ticks * 1'000'000'000) / DQN_CAST(Dqn_f64)g_dqn_library->win32_qpc_frequency.QuadPart;
    #else
    Dqn_f64 result = ticks;
    #endif
    return result;
}

DQN_API uint64_t Dqn_OS_PerfCounterFrequency()
{
    uint64_t result = 0;
    #if defined(DQN_OS_WIN32)
    LARGE_INTEGER integer = {};
    QueryPerformanceFrequency(&integer);
    result = integer.QuadPart;
    #else
    // NOTE: On Linux we use clock_gettime(CLOCK_MONOTONIC_RAW) which
    // increments at nanosecond granularity.
    result = 1'000'000'000;
    #endif
    return result;
}

DQN_API uint64_t Dqn_OS_PerfCounterNow()
{
    uint64_t result = 0;
    #if defined(DQN_OS_WIN32)
    LARGE_INTEGER integer = {};
    QueryPerformanceCounter(&integer);
    result = integer.QuadPart;
    #else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    result = DQN_CAST(uint64_t)ts.tv_sec * 1'000'000'000 + DQN_CAST(uint64_t)ts.tv_nsec;
    #endif

    return result;
}

DQN_API Dqn_OSTimer Dqn_OS_TimerBegin()
{
    Dqn_OSTimer result = {};
    result.start     = Dqn_OS_PerfCounterNow();
    return result;
}

DQN_API void Dqn_OS_TimerEnd(Dqn_OSTimer *timer)
{
    timer->end = Dqn_OS_PerfCounterNow();
}

DQN_API Dqn_f64 Dqn_OS_TimerS(Dqn_OSTimer timer)
{
    Dqn_f64 result = Dqn_OS_PerfCounterS(timer.start, timer.end);
    return result;
}

DQN_API Dqn_f64 Dqn_OS_TimerMs(Dqn_OSTimer timer)
{
    Dqn_f64 result = Dqn_OS_PerfCounterMs(timer.start, timer.end);
    return result;
}

DQN_API Dqn_f64 Dqn_OS_TimerMicroS(Dqn_OSTimer timer)
{
    Dqn_f64 result = Dqn_OS_PerfCounterMicroS(timer.start, timer.end);
    return result;
}

DQN_API Dqn_f64 Dqn_OS_TimerNs(Dqn_OSTimer timer)
{
    Dqn_f64 result = Dqn_OS_PerfCounterNs(timer.start, timer.end);
    return result;
}

DQN_API uint64_t Dqn_OS_EstimateTSCPerSecond(uint64_t duration_ms_to_gauge_tsc_frequency)
{
    uint64_t os_frequency      = Dqn_OS_PerfCounterFrequency();
    uint64_t os_target_elapsed = duration_ms_to_gauge_tsc_frequency * os_frequency / 1000ULL;
    uint64_t tsc_begin         = Dqn_CPU_TSC();
    uint64_t os_elapsed        = 0;
    for (uint64_t os_begin = Dqn_OS_PerfCounterNow(); os_elapsed < os_target_elapsed; )
        os_elapsed = Dqn_OS_PerfCounterNow() - os_begin;
    uint64_t tsc_end     = Dqn_CPU_TSC();
    uint64_t tsc_elapsed = tsc_end - tsc_begin;
    uint64_t result      = tsc_elapsed / os_elapsed * os_frequency;
    return result;
}

// NOTE: [$TCTX] Dqn_ThreadContext =================================================================
Dqn_ThreadScratch::Dqn_ThreadScratch(Dqn_ThreadContext *context, uint8_t context_index)
{
    index       = context_index;
    allocator   = context->temp_allocators[index];
    arena       = context->temp_arenas[index];
    temp_memory = Dqn_Arena_BeginTempMemory(arena);
}

Dqn_ThreadScratch::~Dqn_ThreadScratch()
{
    #if defined(DQN_DEBUG_THREAD_CONTEXT)
    temp_arenas_stat[index] = arena->stats;
    #endif
    DQN_ASSERT(destructed == false);
    Dqn_Arena_EndTempMemory(temp_memory, /*cancel*/ false);
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

DQN_API Dqn_ThreadContext *Dqn_Thread_GetContext()
{
    DQN_THREAD_LOCAL Dqn_ThreadContext result = {};
    if (!result.init) {
        result.init = true;
        DQN_ASSERTF(g_dqn_library->lib_init, "Library must be initialised by calling Dqn_Library_Init(nullptr)");

        // NOTE: Setup permanent arena
        Dqn_ArenaCatalog *catalog = &g_dqn_library->arena_catalog;
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
DQN_API Dqn_ThreadScratch Dqn_Thread_GetScratch(void const *conflict_arena)
{
    static_assert(DQN_THREAD_CONTEXT_ARENAS < (uint8_t)-1, "We use UINT8_MAX as a sentinel value");
    Dqn_ThreadContext *context = Dqn_Thread_GetContext();
    uint8_t context_index      = (uint8_t)-1;
    for (uint8_t index = 0; index < DQN_THREAD_CONTEXT_ARENAS; index++) {
        Dqn_Arena *arena = context->temp_arenas[index];
        if (!conflict_arena || arena != conflict_arena) {
            context_index = index;
            break;
        }
    }

    DQN_ASSERT(context_index != (uint8_t)-1);
    return Dqn_ThreadScratch(context, context_index);
}

