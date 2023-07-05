// NOTE: Table Of Contents =========================================================================
// Index                    | Disable #define         | Description
// =================================================================================================
// [$FSYS] Dqn_Fs           |                         | Filesystem helpers
// [$DATE] Dqn_Date         |                         | Date-time helpers
// [$W32H] Win32 Min Header | DQN_NO_WIN32_MIN_HEADER | Minimal windows.h subset
// [$WIND] Dqn_Win          |                         | Windows OS helpers
// [$WINN] Dqn_WinNet       | DQN_NO_WINNET           | Windows internet download/query helpers
// [$OSYS] Dqn_OS           | DQN_NO_WIN              | Operating-system APIs
// =================================================================================================

// NOTE: [$FSYS] Dqn_Fs ============================================================================
// NOTE: FS Manipulation =======================================================
// TODO(dqn): We should have a Dqn_String8 interface and a CString interface
//
// NOTE: API ===================================================================
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

#define              Dqn_Fs_ReadCString8(path, path_size, file_size, allocator) Dqn_Fs_ReadCString8_(DQN_LEAK_TRACE path, path_size, file_size, allocator)
#define              Dqn_Fs_ReadString8(path, allocator) Dqn_Fs_ReadString8_(DQN_LEAK_TRACE path, allocator)

DQN_API bool         Dqn_Fs_WriteCString8(char const *file_path, Dqn_usize file_path_size, char const *buffer, Dqn_usize buffer_size);
DQN_API bool         Dqn_Fs_WriteString8 (Dqn_String8 file_path, Dqn_String8 buffer);

// NOTE: Internal ==================================================================================
DQN_API char        *Dqn_Fs_ReadCString8_(DQN_LEAK_TRACE_FUNCTION char const *path, Dqn_usize path_size, Dqn_usize *file_size, Dqn_Allocator allocator);
DQN_API Dqn_String8  Dqn_Fs_ReadString8_ (DQN_LEAK_TRACE_FUNCTION Dqn_String8 path, Dqn_Allocator allocator);

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
    Dqn_FsFileOpen_CreateAlways, ///< Create file if it does not exist, otherwise, zero out the file and open
    Dqn_FsFileOpen_OpenIfExist,  ///< Open file at path only if it exists
    Dqn_FsFileOpen_OpenAlways,   ///< Open file at path, create file if it does not exist
};

enum Dqn_FsFileAccess
{
    Dqn_FsFileAccess_Read       = 1 << 0,
    Dqn_FsFileAccess_Write      = 1 << 1,
    Dqn_FsFileAccess_Execute    = 1 << 2,
    Dqn_FsFileAccess_AppendOnly = 1 << 3, ///< This flag cannot be combined with any other acess mode
    Dqn_FsFileAccess_ReadWrite  = Dqn_FsFileAccess_Read      | Dqn_FsFileAccess_Write,
    Dqn_FsFileAccess_All        = Dqn_FsFileAccess_ReadWrite | Dqn_FsFileAccess_Execute,
};

DQN_API Dqn_FsFile Dqn_Fs_OpenFile (Dqn_String8 path, Dqn_FsFileOpen open_mode, uint32_t access);
DQN_API bool       Dqn_Fs_WriteFile(Dqn_FsFile *file, char const *buffer, Dqn_usize size);
DQN_API void       Dqn_Fs_CloseFile(Dqn_FsFile *file);

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

// @proc Dqn_FsPath_ConvertString8
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
DQN_API bool        Dqn_FsPath_Pop               (Dqn_FsPath *fs_path);
DQN_API Dqn_String8 Dqn_FsPath_BuildWithSeparator(Dqn_Arena *arena, Dqn_FsPath const *fs_path, Dqn_String8 path_separator);
DQN_API Dqn_String8 Dqn_FsPath_ConvertString8    (Dqn_Arena *arena, Dqn_String8 path);
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

// NOTE: [$W32H] Win32 Min Header ==================================================================
#if defined(DQN_OS_WIN32)
#if !defined(DQN_NO_WIN32_MIN_HEADER) && !defined(_INC_WINDOWS)
    // Taken from Windows.h
    // typedef unsigned long DWORD;
    // typedef unsigned short WORD;
    // typedef int BOOL;
    // typedef void * HWND;
    // typedef void * HANDLE;
    // typedef long NTSTATUS;

    typedef void * HMODULE;
    typedef union {
        struct {
            unsigned long LowPart;
            long          HighPart;
        };
        struct {
            unsigned long LowPart;
            long          HighPart;
        } u;
        uint64_t QuadPart;
    } LARGE_INTEGER;
#endif // !defined(DQN_NO_WIN32_MIN_HEADER) && !defined(_INC_WINDOWS)

// NOTE: [$WIND] Dqn_Win ===========================================================================
// NOTE: API =======================================================================================
// @proc Dqn_Win_LastErrorToBuffer, Dqn_Win_LastError
//   @desc Retrieve the latest error code and message Windows produced for the 
//   most recent Win32 API call.

// @proc Dqn_Win_MakeProcessDPIAware
//   @desc Call once at application start-up to ensure that the application is
//   DPI aware on Windows and ensure that application UI is scaled up
//   appropriately for the monitor.

struct Dqn_WinErrorMsg
{
    unsigned long code;
    char          data[DQN_KILOBYTES(64) - 1]; // Maximum error size
    unsigned long size;
};
DQN_API void            Dqn_Win_LastErrorToBuffer(Dqn_WinErrorMsg *msg);
DQN_API Dqn_WinErrorMsg Dqn_Win_LastError();
DQN_API void            Dqn_Win_MakeProcessDPIAware();

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

DQN_API int             Dqn_Win_CString8ToCString16        (char const *src, int src_size, wchar_t *dest, int dest_size);
DQN_API int             Dqn_Win_String8ToCString16         (Dqn_String8 src, wchar_t *dest, int dest_size);
DQN_API Dqn_String16    Dqn_Win_String8ToString16Allocator (Dqn_String8 src, Dqn_Allocator allocator);

DQN_API int             Dqn_Win_CString16ToCString8        (wchar_t const *src, int src_size, char *dest, int dest_size);
DQN_API Dqn_String8     Dqn_Win_CString16ToString8Allocator(wchar_t const *src, int src_size, Dqn_Allocator allocator);
DQN_API int             Dqn_Win_String16ToCString8         (Dqn_String16 src, char *dest, int dest_size);
DQN_API Dqn_String8     Dqn_Win_String16ToString8Allocator (Dqn_String16 src, Dqn_Allocator allocator);

// NOTE: Path navigatoin ===========================================================================
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

DQN_API Dqn_usize       Dqn_Win_EXEDirW       (wchar_t *buffer, Dqn_usize size);
DQN_API Dqn_String16    Dqn_Win_EXEDirWArena  (Dqn_Arena *arena);
DQN_API Dqn_String8     Dqn_Win_WorkingDir    (Dqn_Allocator allocator, Dqn_String8 suffix);
DQN_API Dqn_String16    Dqn_Win_WorkingDirW   (Dqn_Allocator allocator, Dqn_String16 suffix);
DQN_API bool            Dqn_Win_FolderIterate (Dqn_String8 path, Dqn_Win_FolderIterator *it);
DQN_API bool            Dqn_Win_FolderWIterate(Dqn_String16 path, Dqn_Win_FolderIteratorW *it);

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

/// Record time between two time-points using the OS's performance counter.
struct Dqn_OSTimer
{
    uint64_t start;
    uint64_t end;
};

DQN_API bool        Dqn_OS_SecureRNGBytes   (void *buffer, uint32_t size);
DQN_API Dqn_String8 Dqn_OS_EXEDir           (Dqn_Allocator allocator);
DQN_API void        Dqn_OS_SleepMs          (Dqn_uint milliseconds);
DQN_API uint64_t    Dqn_OS_PerfCounterNow   ();
DQN_API Dqn_f64     Dqn_OS_PerfCounterS     (uint64_t begin, uint64_t end);
DQN_API Dqn_f64     Dqn_OS_PerfCounterMs    (uint64_t begin, uint64_t end);
DQN_API Dqn_f64     Dqn_OS_PerfCounterMicroS(uint64_t begin, uint64_t end);
DQN_API Dqn_f64     Dqn_OS_PerfCounterNs    (uint64_t begin, uint64_t end);
DQN_API Dqn_OSTimer Dqn_OS_TimerBegin       ();
DQN_API void        Dqn_OS_TimerEnd         (Dqn_OSTimer *timer);
DQN_API Dqn_f64     Dqn_OS_TimerS           (Dqn_OSTimer timer);
DQN_API Dqn_f64     Dqn_OS_TimerMs          (Dqn_OSTimer timer);
DQN_API Dqn_f64     Dqn_OS_TimerMicroS      (Dqn_OSTimer timer);
DQN_API Dqn_f64     Dqn_OS_TimerNs          (Dqn_OSTimer timer);

// OS_TimedBlock provides a extremely primitive way of measuring the duration of
// code blocks, by sprinkling DQN_OS_TIMED_BLOCK_RECORD("record label"), you can
// measure the time between the macro and the next record call.
//
// Example: Record the duration of the for-loop below and print it at the end.
/*
   int main()
   {
       DQN_OS_TIMED_BLOCK_INIT("Profiling Region", 32); // name, records to allocate
       DQN_OS_TIMED_BLOCK_RECORD("a");
       for (int unused1_ = 0; unused1_ < 1000000; unused1_++)
       {
           for (int unused2_ = 0; unused2_ < 1000000; unused2_++)
           {
               (void)unused1_;
               (void)unused2_;
           }
       }
       DQN_OS_TIMED_BLOCK_RECORD("b");
       DQN_OS_TIMED_BLOCK_DUMP;
       return 0;
   }
*/
struct Dqn_OSTimedBlock
{
    char const *label;
    uint64_t     tick;
};

// Initialise a timing block region,
#define DQN_OS_TIMED_BLOCK_INIT(label, size) \
    Dqn_OSTimedBlock timings_[size];        \
    Dqn_usize timings_size_ = 0;          \
    DQN_OS_TIMED_BLOCK_RECORD(label)

// Add a timing record at the current location this macro is invoked.
// DQN_OS_TIMED_BLOCK_INIT must have been called in a scope visible to the macro
// prior.
// label: The label to give to the timing record
#define DQN_OS_TIMED_BLOCK_RECORD(label) timings_[timings_size_++] = {label, Dqn_OS_PerfCounterNow()}

// Dump the timing block via Dqn_Log
#define DQN_OS_TIMED_BLOCK_DUMP                                                                         \
    DQN_ASSERTF(timings_size_ < sizeof(timings_) / sizeof(timings_[0]),                              \
                   "Timings array indexed out-of-bounds, use a bigger size");                           \
    for (int timings_index_ = 0; timings_index_ < (timings_size_ - 1); timings_index_++) {              \
        Dqn_OSTimedBlock t1 = timings_[timings_index_ + 0];                                             \
        Dqn_OSTimedBlock t2 = timings_[timings_index_ + 1];                                             \
        DQN_LOG_D("%s -> %s: %fms", t1.label, t2.label, Dqn_OS_PerfCounterMs(t1.tick, t2.tick));        \
    }                                                                                                   \
                                                                                                        \
    if (timings_size_ >= 1) {                                                                           \
        Dqn_OSTimedBlock t1 = timings_[0];                                                              \
        Dqn_OSTimedBlock t2 = timings_[timings_size_ - 1];                                              \
        DQN_LOG_D("%s -> %s (total): %fms", t1.label, t2.label, Dqn_OS_PerfCounterMs(t1.tick, t2.tick));\
    }
