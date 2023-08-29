#if !defined(DQN_NO_FS)
#if defined(DQN_OS_WIN32) && defined(DQN_NO_WIN)
    #error "Filesystem APIs requires Windows API, DQN_NO_WIN must not be defined"
#endif
// NOTE: [$FSYS] Dqn_Fs ============================================================================
// NOTE: FS Manipulation ===========================================================================
// TODO(dqn): We should have a Dqn_String8 interface and a CString interface
//
// NOTE: API =======================================================================================
// @proc Dqn_FsDelete
//   @desc Delete the item specified at the path. This function *CAN* not delete directories unless
//   the directory is empty.
//   @return True if deletion was successful, false otherwise

enum Dqn_FsInfoType
{
    Dqn_FsInfoType_Unknown,
    Dqn_FsInfoType_Directory,
    Dqn_FsInfoType_File,
};

struct Dqn_FsInfo
{
    bool           exists;
    Dqn_FsInfoType type;
    uint64_t       create_time_in_s;
    uint64_t       last_write_time_in_s;
    uint64_t       last_access_time_in_s;
    uint64_t       size;
};

DQN_API bool         Dqn_Fs_Exists   (Dqn_String8 path);
DQN_API bool         Dqn_Fs_DirExists(Dqn_String8 path);
DQN_API Dqn_FsInfo   Dqn_Fs_GetInfo  (Dqn_String8 path);
DQN_API bool         Dqn_Fs_Copy     (Dqn_String8 src, Dqn_String8 dest, bool overwrite);
DQN_API bool         Dqn_Fs_MakeDir  (Dqn_String8 path);
DQN_API bool         Dqn_Fs_Move     (Dqn_String8 src, Dqn_String8 dest, bool overwrite);
DQN_API bool         Dqn_Fs_Delete   (Dqn_String8 path);

// NOTE: R/W Entire File ===========================================================================
// NOTE: API =======================================================================================
// @proc Dqn_Fs_WriteString8, Dqn_Fs_WriteCString8
//   @desc Write the string to a file at the path overwriting if necessary.

// @proc Dqn_Fs_ReadString8, Dqn_Fs_ReadCString8
//   @desc Read the file at the path to a string.

DQN_API bool         Dqn_Fs_WriteCString8(char const *file_path, Dqn_usize file_path_size, char const *buffer, Dqn_usize buffer_size);
DQN_API bool         Dqn_Fs_Write        (Dqn_String8 file_path, Dqn_String8 buffer);
DQN_API char        *Dqn_Fs_ReadCString8 (char const *path, Dqn_usize path_size, Dqn_usize *file_size, Dqn_Allocator allocator);
DQN_API Dqn_String8  Dqn_Fs_Read         (Dqn_String8 path, Dqn_Allocator allocator);

// NOTE: R/W Stream API ============================================================================
// NOTE: API =======================================================================================
// @proc Dqn_Fs_OpenFile
//   @desc Open a handle to the file

// @proc Dqn_Fs_WriteFile
//   @desc Append to the file specified by the handle with the given buffer.

// @proc Dqn_Fs_CloseFile
//   @desc Close the file at specified by the handle

struct Dqn_FsFile
{
    void    *handle;
    char     error[512];
    uint16_t error_size;
};

enum Dqn_FsFileOpen
{
    Dqn_FsFileOpen_CreateAlways, // Create file if it does not exist, otherwise, zero out the file and open
    Dqn_FsFileOpen_OpenIfExist,  // Open file at path only if it exists
    Dqn_FsFileOpen_OpenAlways,   // Open file at path, create file if it does not exist
};

enum Dqn_FsFileAccess
{
    Dqn_FsFileAccess_Read       = 1 << 0,
    Dqn_FsFileAccess_Write      = 1 << 1,
    Dqn_FsFileAccess_Execute    = 1 << 2,
    Dqn_FsFileAccess_AppendOnly = 1 << 3, // This flag cannot be combined with any other access mode
    Dqn_FsFileAccess_ReadWrite  = Dqn_FsFileAccess_Read      | Dqn_FsFileAccess_Write,
    Dqn_FsFileAccess_All        = Dqn_FsFileAccess_ReadWrite | Dqn_FsFileAccess_Execute,
};

DQN_API Dqn_FsFile Dqn_Fs_OpenFile       (Dqn_String8 path, Dqn_FsFileOpen open_mode, uint32_t access);
DQN_API bool       Dqn_Fs_WriteFileBuffer(Dqn_FsFile *file, void const *data, Dqn_usize size);
DQN_API bool       Dqn_Fs_WriteFile      (Dqn_FsFile *file, Dqn_String8 buffer);
DQN_API bool       Dqn_Fs_WriteFileFV    (Dqn_FsFile *file, DQN_FMT_STRING_ANNOTATE char const *fmt, va_list args);
DQN_API bool       Dqn_Fs_WriteFileF     (Dqn_FsFile *file, DQN_FMT_STRING_ANNOTATE char const *fmt, ...);
DQN_API void       Dqn_Fs_CloseFile      (Dqn_FsFile *file);
#endif // !defined(DQN_NO_FS)

// NOTE: File system paths =========================================================================
// Helper data structure for building paths suitable for OS consumption.
//
// NOTE: API =======================================================================================
// @proc Dqn_FsPath_AddRef, Dqn_FsPath_Add
//   @desc Append a path to the file path. The passed in path can be specify
//   both a single level or multiple directories with different path separators.
//   The path will be decomposed into individual sections in the function.
//
//   For example passing
//     - "path/to/your/desired/folder" is valid
//     - "path" is valid
//     - "path/to\your/desired\folder" is valid

// @proc Dqn_FsPath_Pop
//   @desc Remove the last appended path level from the current path stored in
//   the FsPath.
//
//   For example "path/to/your/desired/folder" popped produces
//   "path/to/your/desired"

// @proc Dqn_FsPath_Convert
//   @desc Convert the path specified in the string to the OS native separated
//   path.

#if !defined(Dqn_FsPathOSSeperator)
    #if defined(DQN_OS_WIN32)
        #define Dqn_FsPathOSSeperator "\\"
    #else
        #define Dqn_FsPathOSSeperator "/"
    #endif
    #define Dqn_FsPathOSSeperatorString DQN_STRING8(Dqn_FsPathOSSeperator)
#endif

struct Dqn_FsPathLink
{
    Dqn_String8     string;
    Dqn_FsPathLink *next;
    Dqn_FsPathLink *prev;
};

struct Dqn_FsPath
{
    Dqn_FsPathLink *head;
    Dqn_FsPathLink *tail;
    Dqn_usize       string_size;
    uint16_t        links_size;
};

DQN_API bool        Dqn_FsPath_AddRef            (Dqn_Arena *arena, Dqn_FsPath *fs_path, Dqn_String8 path);
DQN_API bool        Dqn_FsPath_Add               (Dqn_Arena *arena, Dqn_FsPath *fs_path, Dqn_String8 path);
DQN_API bool        Dqn_FsPath_AddF              (Dqn_Arena *arena, Dqn_FsPath *fs_path, DQN_FMT_STRING_ANNOTATE char const *fmt, ...);
DQN_API bool        Dqn_FsPath_Pop               (Dqn_FsPath *fs_path);
DQN_API Dqn_String8 Dqn_FsPath_BuildWithSeparator(Dqn_Arena *arena, Dqn_FsPath const *fs_path, Dqn_String8 path_separator);
DQN_API Dqn_String8 Dqn_FsPath_Convert           (Dqn_Arena *arena, Dqn_String8 path);
DQN_API Dqn_String8 Dqn_FsPath_ConvertF          (Dqn_Arena *arena, DQN_FMT_STRING_ANNOTATE char const *fmt, ...);
#define             Dqn_FsPath_BuildFwdSlash(arena, fs_path)  Dqn_FsPath_BuildWithSeparator(arena, fs_path, DQN_STRING8("/"))
#define             Dqn_FsPath_BuildBackSlash(arena, fs_path) Dqn_FsPath_BuildWithSeparator(arena, fs_path, DQN_STRING8("\\"))

#if !defined(Dqn_FsPath_Build)
    #if defined(DQN_OS_WIN32)
        #define Dqn_FsPath_Build(arena, fs_path) Dqn_FsPath_BuildBackSlash(arena, fs_path)
    #else
        #define Dqn_FsPath_Build(arena, fs_path) Dqn_FsPath_BuildFwdSlash(arena, fs_path)
    #endif
#endif

// NOTE: [$DATE] Dqn_Date ==========================================================================
// NOTE: API =======================================================================================
// @proc Dqn_Date_EpochTime
//    @desc Produce the time elapsed since the Unix epoch
//    (e.g. 1970-01-01T00:00:00Z) in seconds

struct Dqn_DateHMSTimeString
{
    char    date[DQN_ARRAY_UCOUNT("YYYY-MM-SS")];
    uint8_t date_size;

    char    hms[DQN_ARRAY_UCOUNT("HH:MM:SS")];
    uint8_t hms_size;
};

struct Dqn_DateHMSTime
{
    uint8_t day;
    uint8_t month;
    int16_t year;

    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
};

DQN_API Dqn_DateHMSTime       Dqn_Date_HMSLocalTimeNow      ();
DQN_API Dqn_DateHMSTimeString Dqn_Date_HMSLocalTimeStringNow(char date_separator = '-', char hms_separator = ':');
DQN_API Dqn_DateHMSTimeString Dqn_Date_HMSLocalTimeString   (Dqn_DateHMSTime time, char date_separator = '-', char hms_separator = ':');
DQN_API uint64_t              Dqn_Date_EpochTime            ();

#if defined(DQN_OS_WIN32)
#if !defined(DQN_NO_WIN)
// NOTE: [$WIND] Dqn_Win ===========================================================================
// NOTE: API =======================================================================================
// @proc Dqn_Win_LastErrorToBuffer, Dqn_Win_LastError
//   @desc Retrieve the latest error code and message Windows produced for the 
//   most recent Win32 API call.

// @proc Dqn_Win_MakeProcessDPIAware
//   @desc Call once at application start-up to ensure that the application is
//   DPI aware on Windows and ensure that application UI is scaled up
//   appropriately for the monitor.

struct Dqn_WinError
{
    unsigned long code;
    Dqn_String8   msg;
};
DQN_API Dqn_WinError Dqn_Win_LastError(Dqn_Arena *arena);
DQN_API void         Dqn_Win_MakeProcessDPIAware();

// NOTE: Windows String8 <-> String16 ===========================================
// Convert a UTF8 <-> UTF16 string.
//
// The exact size buffer required for this function can be determined by
// calling this function with the 'dest' set to null and 'dest_size' set to 0,
// the return size is the size required for conversion not-including space for
// the null-terminator. This function *always* null-terminates the input
// buffer.
//
// Returns the number of u8's (for UTF16->8) OR u16's (for UTF8->16)
// written/required for conversion. 0 if there was a conversion error and can be
// queried using 'Dqn_Win_LastError'

DQN_API Dqn_String16 Dqn_Win_String8ToString16(Dqn_Arena *arena, Dqn_String8 src);
DQN_API int          Dqn_Win_String8ToString16Buffer(Dqn_String16 src, char *dest, int dest_size);
DQN_API Dqn_String8  Dqn_Win_String16ToString8(Dqn_Arena *arena, Dqn_String16 src);
DQN_API int          Dqn_Win_String16ToString8Buffer(Dqn_String16 src, char *dest, int dest_size);

// NOTE: Path navigation ===========================================================================
// NOTE: API =======================================================================================
// @proc Dqn_Win_EXEDirW, Dqn_Win_EXEDirWArena
//   @desc Evaluate the current executable's directory that is running when this
//   function is called.
//   @param[out] buffer The buffer to write the executable directory into. Set
//   this to null to calculate the required buffer size for the directory.
//   @param[in] size The size of the buffer given. Set this to 0 to calculate the
//   required buffer size for the directory.
//   @return The length of the executable directory string. If this return value
//   exceeds the capacity of the 'buffer', the 'buffer' is untouched.

// @proc Dqn_Win_WorkingDir, Dqn_Win_WorkingDirW
//   @param[in] suffix (Optional) A suffix to append to the current working directory

// @proc Dqn_Win_FolderIterate, Dqn_Win_FolderWIterate
//   @desc Iterate the files in the specified folder at the path
#if 0
    for (Dqn_WinFolderIterator it = {}; Dqn_Win_FolderIterate("C:/your/path/", &it); ) {
        printf("%.*s\n", DQN_STRING_FMT(it.file_name));
    }
#endif

struct Dqn_Win_FolderIteratorW
{
    void         *handle;
    Dqn_String16 file_name;
    wchar_t      file_name_buf[512];
};

struct Dqn_Win_FolderIterator
{
    void        *handle;
    Dqn_String8  file_name;
    char         file_name_buf[512];
};

DQN_API Dqn_String16    Dqn_Win_EXEDirW       (Dqn_Arena *arena);
DQN_API Dqn_String8     Dqn_Win_WorkingDir    (Dqn_Allocator allocator, Dqn_String8 suffix);
DQN_API Dqn_String16    Dqn_Win_WorkingDirW   (Dqn_Allocator allocator, Dqn_String16 suffix);
DQN_API bool            Dqn_Win_FolderIterate (Dqn_String8 path, Dqn_Win_FolderIterator *it);
DQN_API bool            Dqn_Win_FolderWIterate(Dqn_String16 path, Dqn_Win_FolderIteratorW *it);
#endif // !defined(DQN_NO_WIN)

#if !defined(DQN_NO_WINNET)
// NOTE: [$WINN] Dqn_WinNet ========================================================================
// TODO(dqn): Useful options to expose in the handle
// https://docs.microsoft.com/en-us/windows/win32/wininet/option-flags
// INTERNET_OPTION_CONNECT_RETRIES -- default is 5 retries
// INTERNET_OPTION_CONNECT_TIMEOUT -- milliseconds
// INTERNET_OPTION_RECEIVE_TIMEOUT
// INTERNET_OPTION_SEND_TIMEOUT
//
// NOTE: API =======================================================================================
// @proc Dqn_Win_NetHandleInitHTTPMethod, Dqn_Win_NetHandleInitHTTPMethodCString
//   @desc Setup a handle to the URL with the given HTTP verb.
//
//   This function is the same as calling Dqn_Win_NetHandleInit() followed by
//   Dqn_Win_NetHandleSetHTTPMethod().
//
//   @param http_method The HTTP request type, e.g. "GET" or "POST" e.t.c

// @proc Dqn_Win_NetHandleSetHTTPMethod
//   @desc Set the HTTP request method for the given handle. This function can
//   be used on a pre-existing valid handle that has at the minimum been 
//   initialised.

enum Dqn_WinNetHandleState
{
    Dqn_WinNetHandleState_Invalid,
    Dqn_WinNetHandleState_Initialised,
    Dqn_WinNetHandleState_HttpMethodReady,
    Dqn_WinNetHandleState_RequestFailed,
    Dqn_WinNetHandleState_RequestGood,
};

// The number of bytes each pump of the connection downloads at most. If this is
// zero we default to DQN_WIN_NET_HANDLE_DOWNLOAD_SIZE.
#if !defined(DQN_WIN_NET_HANDLE_DOWNLOAD_SIZE)
    #define DQN_WIN_NET_HANDLE_DOWNLOAD_SIZE 4096
#endif

struct Dqn_WinNetHandle
{
    // NOTE: We copy out the host name because it needs to be null-terminated.
    // Luckily, we can assume a DNS domain won't exceed 256 characters so this
    // will generally always work.
    char host_name[256];
    int  host_name_size;

    // NOTE: Everything after the domain/host name part of the string i.e. the
    // '/test' part of the full url 'mywebsite.com/test'.
    // TODO(dqn): I don't want to make our network API allocate here so we don't
    // copy the string since we require that the string is null-terminated so
    // then taking a pointer to the input string should work .. maybe this is
    // ok?
    char *url;
    int url_size;

    // NOTE: docs.microsoft.com/en-us/windows/win32/wininet/setting-and-retrieving-internet-options#scope-of-hinternet-handle
    // These handles have three levels:
    //
    // The root HINTERNET handle (created by a call to InternetOpen) would contain all the Internet options that affect this instance of WinINet.
    // HINTERNET handles that connect to a server (created by a call to InternetConnect)
    // HINTERNET handles associated with a resource or enumeration of resources on a particular server.
    //
    // More detailed information about the HINTERNET dependency is listed here
    // NOTE: https://docs.microsoft.com/en-us/windows/win32/wininet/appendix-a-hinternet-handles
    void *internet_open_handle;
    void *internet_connect_handle;
    void *http_handle;
    Dqn_WinNetHandleState state;
};

enum Dqn_WinNetHandleRequestHeaderFlag
{
    Dqn_WinNetHandleRequestHeaderFlag_Add,
    Dqn_WinNetHandleRequestHeaderFlag_AddIfNew,
    Dqn_WinNetHandleRequestHeaderFlag_Replace,
    Dqn_WinNetHandleRequestHeaderFlag_Count,
};

struct Dqn_WinNetHandleResponse
{
    Dqn_String8  raw_headers;
    Dqn_String8 *headers;
    Dqn_usize    headers_size;

    // NOTE: Headers pulled from the 'raw_headers' for convenience
    uint64_t     content_length;
    Dqn_String8  content_type;
};

DQN_API Dqn_WinNetHandle         Dqn_Win_NetHandleInitCString             (char const *url, int url_size);
DQN_API Dqn_WinNetHandle         Dqn_Win_NetHandleInit                    (Dqn_String8 url);
DQN_API Dqn_WinNetHandle         Dqn_Win_NetHandleInitHTTPMethodCString   (char const *url, int url_size, char const *http_method);
DQN_API Dqn_WinNetHandle         Dqn_Win_NetHandleInitHTTPMethod          (Dqn_String8 url, Dqn_String8 http_method);
DQN_API void                     Dqn_Win_NetHandleClose                   (Dqn_WinNetHandle *handle);
DQN_API bool                     Dqn_Win_NetHandleIsValid                 (Dqn_WinNetHandle const *handle);
DQN_API void                     Dqn_Win_NetHandleSetUserAgentCString     (Dqn_WinNetHandle *handle, char const *user_agent, int user_agent_size);
DQN_API bool                     Dqn_Win_NetHandleSetHTTPMethod           (Dqn_WinNetHandle *handle, char const *method);
DQN_API bool                     Dqn_Win_NetHandleSetRequestHeaderCString8(Dqn_WinNetHandle *handle, char const *header, int header_size, uint32_t mode);
DQN_API bool                     Dqn_Win_NetHandleSetRequestHeaderString8 (Dqn_WinNetHandle *handle, Dqn_String8 header, uint32_t mode);
DQN_API Dqn_WinNetHandleResponse Dqn_Win_NetHandleSendRequest             (Dqn_WinNetHandle *handle, Dqn_Allocator allocator, char const *post_data, unsigned long post_data_size);
DQN_API bool                     Dqn_Win_NetHandlePump                    (Dqn_WinNetHandle *handle, char *dest, int dest_size, size_t *download_size);
DQN_API char *                   Dqn_Win_NetHandlePumpCString8            (Dqn_WinNetHandle *handle, Dqn_Arena *arena, size_t *download_size);
DQN_API Dqn_String8              Dqn_Win_NetHandlePumpString8             (Dqn_WinNetHandle *handle, Dqn_Arena *arena);
DQN_API void                     Dqn_Win_NetHandlePumpToCRTFile           (Dqn_WinNetHandle *handle, FILE *file);
DQN_API char *                   Dqn_Win_NetHandlePumpToAllocCString      (Dqn_WinNetHandle *handle, size_t *download_size);
DQN_API Dqn_String8              Dqn_Win_NetHandlePumpToAllocString       (Dqn_WinNetHandle *handle);
#endif // !defined(DQN_NO_WINNET)
#endif // defined(DQN_OS_WIN32)

// NOTE: [$OSYS] Dqn_OS ============================================================================
// NOTE: API =======================================================================================
// @proc Dqn_OS_SecureRNGBytes
//   @desc Generate cryptographically secure bytes

// @proc Dqn_OS_EXEDir
//   @desc Retrieve the executable directory without the trailing '/' or 
//   ('\' for windows). If this fails an empty string is returned.

// @proc Dqn_OS_PerfCounterFrequency
//   @desc Get the number of ticks in the performance counter per second for the
//   operating system you're running on. This value can be used to calculate 
//   duration from OS performance counter ticks.

// @proc Dqn_OS_EstimateTSCPerSecond
//   @desc Estimate how many timestamp count's (TSC) there are per second. TSC
//   is evaluated by calling __rdtsc() or the equivalent on the platform. This
//   value can be used to convert TSC durations into seconds.
//
//   @param duration_ms_to_gauge_tsc_frequency How many milliseconds to spend
//   measuring the TSC rate of the current machine. 100ms is sufficient to
//   produce a fairly accurate result with minimal blocking in applications.

/// Record time between two time-points using the OS's performance counter.
struct Dqn_OSTimer
{
    uint64_t start;
    uint64_t end;
};

DQN_API bool        Dqn_OS_SecureRNGBytes      (void *buffer, uint32_t size);
#if (defined(DQN_OS_WIN32) && !defined(DQN_NO_WIN)) || !defined(DQN_OS_WIN32)
DQN_API Dqn_String8 Dqn_OS_EXEDir              (Dqn_Arena* arena);
#endif
DQN_API void        Dqn_OS_SleepMs             (Dqn_uint milliseconds);
DQN_API uint64_t    Dqn_OS_PerfCounterNow      ();
DQN_API uint64_t    Dqn_OS_PerfCounterFrequency();
DQN_API Dqn_f64     Dqn_OS_PerfCounterS        (uint64_t begin, uint64_t end);
DQN_API Dqn_f64     Dqn_OS_PerfCounterMs       (uint64_t begin, uint64_t end);
DQN_API Dqn_f64     Dqn_OS_PerfCounterMicroS   (uint64_t begin, uint64_t end);
DQN_API Dqn_f64     Dqn_OS_PerfCounterNs       (uint64_t begin, uint64_t end);
DQN_API Dqn_OSTimer Dqn_OS_TimerBegin          ();
DQN_API void        Dqn_OS_TimerEnd            (Dqn_OSTimer *timer);
DQN_API Dqn_f64     Dqn_OS_TimerS              (Dqn_OSTimer timer);
DQN_API Dqn_f64     Dqn_OS_TimerMs             (Dqn_OSTimer timer);
DQN_API Dqn_f64     Dqn_OS_TimerMicroS         (Dqn_OSTimer timer);
DQN_API Dqn_f64     Dqn_OS_TimerNs             (Dqn_OSTimer timer);
DQN_API uint64_t    Dqn_OS_EstimateTSCPerSecond(uint64_t duration_ms_to_gauge_tsc_frequency);

// NOTE: [$TCTX] Dqn_ThreadContext =================================================================
// Each thread is assigned in their thread-local storage (TLS) scratch and
// permanent arena allocators. These can be used for allocations with a lifetime
// scoped to the lexical scope or for storing data permanently using the arena
// paradigm.
//
// TLS in this implementation is implemented using the `thread_local` C/C++
// keyword.
//
// NOTE: API
//
// @proc Dqn_Thread_GetContext
//   @desc Get the current thread's context- this contains all the metadata for managing
//   the thread scratch data. In general you probably want Dqn_Thread_GetScratch()
//   which ensures you get a usable scratch arena for temporary allocations
//   without having to worry about selecting the right arena from the state.
//
// @proc Dqn_Thread_GetScratch
//   @desc Retrieve the per-thread temporary arena allocator that is reset on scope
//   exit.
//
//   The scratch arena must be deconflicted with any existing arenas in the
//   function to avoid trampling over each other's memory. Consider the situation
//   where the scratch arena is passed into the function. Inside the function, if
//   the same arena is reused then, if both arenas allocate, when the inner arena
//   is reset, this will undo the passed in arena's allocations in the function.
//
//   @param[in] conflict_arena A pointer to the arena currently being used in the
//   function

struct Dqn_ThreadContext
{
    Dqn_b32        init;

    // Scratch memory arena's for the calling thread
    Dqn_Arena     *scratch_arenas[2];

    // Allocators that use the corresponding arena from the thread context.
    // Provided for convenience when interfacing with allocator interfaces.
    Dqn_Allocator  scratch_allocators[2];
};

struct Dqn_ThreadScratch
{
    Dqn_ThreadScratch(Dqn_ThreadContext *context, uint8_t context_index);
    ~Dqn_ThreadScratch();

    Dqn_Allocator        allocator;
    Dqn_Arena           *arena;
    Dqn_b32              destructed;
    Dqn_ArenaTempMemory  temp_memory;
};

// NOTE: Context ===================================================================================
DQN_API uint32_t           Dqn_Thread_GetID();
DQN_API Dqn_ThreadContext *Dqn_Thread_GetContext();
DQN_API Dqn_ThreadScratch  Dqn_Thread_GetScratch(void const *conflict_arena);
