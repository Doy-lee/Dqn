////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   $$\      $$\ $$$$$$\ $$\   $$\  $$$$$$\   $$$$$$\
//   $$ | $\  $$ |\_$$  _|$$$\  $$ |$$ ___$$\ $$  __$$\
//   $$ |$$$\ $$ |  $$ |  $$$$\ $$ |\_/   $$ |\__/  $$ |
//   $$ $$ $$\$$ |  $$ |  $$ $$\$$ |  $$$$$ /  $$$$$$  |
//   $$$$  _$$$$ |  $$ |  $$ \$$$$ |  \___$$\ $$  ____/
//   $$$  / \$$$ |  $$ |  $$ |\$$$ |$$\   $$ |$$ |
//   $$  /   \$$ |$$$$$$\ $$ | \$$ |\$$$$$$  |$$$$$$$$\
//   \__/     \__|\______|\__|  \__| \______/ \________|
//
//   dqn_win32.h -- Windows replacement header
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(DQN_COMPILER_MSVC) || defined(DQN_COMPILER_CLANG_CL)
    #pragma comment(lib, "bcrypt")
    #pragma comment(lib, "winhttp")
    #pragma comment(lib, "dbghelp")
#endif

#if defined(DQN_NO_WIN32_MIN_HEADER) || defined(_INC_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>  // LONG
    #include <bcrypt.h>   // Dqn_OS_SecureRNGBytes -> BCryptOpenAlgorithmProvider ... etc
    #include <shellapi.h> // Dqn_Win_MakeProcessDPIAware -> SetProcessDpiAwareProc
    #include <winhttp.h>  // WinHttp*
    #include <DbgHelp.h>
#else
    DQN_MSVC_WARNING_PUSH
    DQN_MSVC_WARNING_DISABLE(4201) // warning C4201: nonstandard extension used: nameless struct/union

    // NOTE: basetsd.h /////////////////////////////////////////////////////////////////////////////
    typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;
    typedef ULONG_PTR        SIZE_T,    *PSIZE_T;
    typedef __int64          LONG_PTR,  *PLONG_PTR;
    typedef ULONG_PTR        DWORD_PTR, *PDWORD_PTR;
    typedef unsigned __int64 ULONG64,   *PULONG64;
    typedef unsigned __int64 DWORD64,   *PDWORD64;

    // NOTE: shared/minwindef.h ////////////////////////////////////////////////////////////////////
    struct HINSTANCE__ {
      int unused;
    };
    typedef struct HINSTANCE__ *HINSTANCE;

    typedef unsigned long  DWORD;
    typedef int            BOOL;
    typedef int            INT;
    typedef unsigned long  ULONG;
    typedef unsigned int   UINT;
    typedef unsigned short WORD;
    typedef unsigned char  BYTE;
    typedef unsigned char  UCHAR;
    typedef HINSTANCE      HMODULE; /* HMODULEs can be used in place of HINSTANCEs */
    typedef void *         HANDLE;
    typedef HANDLE         HLOCAL;

    #define MAX_PATH 260

    typedef struct _FILETIME {
        DWORD dwLowDateTime;
        DWORD dwHighDateTime;
    } FILETIME, *PFILETIME, *LPFILETIME;

    // NOTE: shared/winerror.h /////////////////////////////////////////////////////////////////////
    // NOTE: GetModuleFileNameW
    #define ERROR_INSUFFICIENT_BUFFER 122L // dderror

    // NOTE: um/winnls.h ///////////////////////////////////////////////////////////////////////////
    // NOTE: MultiByteToWideChar
    #define CP_UTF8 65001 // UTF-8 translation

    // NOTE: um/winnt.h ////////////////////////////////////////////////////////////////////////////
    typedef void             VOID;
    typedef __int64          LONGLONG;
    typedef unsigned __int64 ULONGLONG;
    typedef void *           HANDLE;
    typedef char             CHAR;
    typedef short            SHORT;
    typedef long             LONG;
    typedef wchar_t          WCHAR; // wc, 16-bit UNICODE character
    typedef CHAR *           NPSTR, *LPSTR, *PSTR;
    typedef WCHAR *          NWPSTR, *LPWSTR, *PWSTR;

    // NOTE: VirtualAlloc: Allocation Type
    #define MEM_RESERVE  0x00002000
    #define MEM_COMMIT   0x00001000
    #define MEM_DECOMMIT 0x00004000
    #define MEM_RELEASE  0x00008000

    // NOTE: VirtualAlloc: Page Permissions
    #define PAGE_NOACCESS 0x01
    #define PAGE_READONLY 0x02
    #define PAGE_READWRITE 0x04
    #define PAGE_GUARD 0x100

    // NOTE: FormatMessageA
    #define MAKELANGID(p, s) ((((WORD  )(s)) << 10) | (WORD  )(p))
    #define LANG_NEUTRAL    0x00
    #define SUBLANG_DEFAULT 0x01 // user default

    // NOTE: CreateFile
    #define GENERIC_READ     (0x80000000L)
    #define GENERIC_WRITE    (0x40000000L)
    #define GENERIC_EXECUTE  (0x20000000L)
    #define GENERIC_ALL      (0x10000000L)

    #define FILE_APPEND_DATA (0x0004) // file

    // NOTE: CreateFile/FindFirstFile
    #define FILE_SHARE_READ          0x00000001
    #define FILE_SHARE_WRITE         0x00000002
    #define FILE_SHARE_DELETE        0x00000004

    #define FILE_ATTRIBUTE_READONLY  0x00000001
    #define FILE_ATTRIBUTE_HIDDEN    0x00000002
    #define FILE_ATTRIBUTE_SYSTEM    0x00000004
    #define FILE_ATTRIBUTE_DIRECTORY 0x00000010
    #define FILE_ATTRIBUTE_NORMAL    0x00000080

    // NOTE: STACKFRAME64
    #define IMAGE_FILE_MACHINE_AMD64 0x8664  // AMD64 (K8)

    // NOTE: WaitForSingleObject
    #define WAIT_TIMEOUT            258L    // dderror
    #define STATUS_WAIT_0           ((DWORD   )0x00000000L)
    #define STATUS_ABANDONED_WAIT_0 ((DWORD   )0x00000080L)

    typedef union _ULARGE_INTEGER {
        struct {
            DWORD LowPart;
            DWORD HighPart;
        } DUMMYSTRUCTNAME;
        struct {
            DWORD LowPart;
            DWORD HighPart;
        } u;
        ULONGLONG QuadPart;
    } ULARGE_INTEGER;

    typedef union _LARGE_INTEGER {
        struct {
            DWORD LowPart;
            LONG HighPart;
        } DUMMYSTRUCTNAME;
        struct {
            DWORD LowPart;
            LONG HighPart;
        } u;
        LONGLONG QuadPart;
    } LARGE_INTEGER;

    typedef struct __declspec(align(16)) _M128A {
        ULONGLONG Low;
        LONGLONG High;
    } M128A, *PM128A;

    typedef struct __declspec(align(16)) _XSAVE_FORMAT {
        WORD   ControlWord;
        WORD   StatusWord;
        BYTE  TagWord;
        BYTE  Reserved1;
        WORD   ErrorOpcode;
        DWORD ErrorOffset;
        WORD   ErrorSelector;
        WORD   Reserved2;
        DWORD DataOffset;
        WORD   DataSelector;
        WORD   Reserved3;
        DWORD MxCsr;
        DWORD MxCsr_Mask;
        M128A FloatRegisters[8];
    #if defined(_WIN64)
        M128A XmmRegisters[16];
        BYTE  Reserved4[96];
    #else
        M128A XmmRegisters[8];
        BYTE  Reserved4[224];
    #endif
    } XSAVE_FORMAT, *PXSAVE_FORMAT;
    typedef XSAVE_FORMAT XMM_SAVE_AREA32, *PXMM_SAVE_AREA32;

    typedef struct __declspec(align(16)) _CONTEXT {
        DWORD64 P1Home;
        DWORD64 P2Home;
        DWORD64 P3Home;
        DWORD64 P4Home;
        DWORD64 P5Home;
        DWORD64 P6Home;
        DWORD   ContextFlags;
        DWORD   MxCsr;
        WORD    SegCs;
        WORD    SegDs;
        WORD    SegEs;
        WORD    SegFs;
        WORD    SegGs;
        WORD    SegSs;
        DWORD   EFlags;
        DWORD64 Dr0;
        DWORD64 Dr1;
        DWORD64 Dr2;
        DWORD64 Dr3;
        DWORD64 Dr6;
        DWORD64 Dr7;
        DWORD64 Rax;
        DWORD64 Rcx;
        DWORD64 Rdx;
        DWORD64 Rbx;
        DWORD64 Rsp;
        DWORD64 Rbp;
        DWORD64 Rsi;
        DWORD64 Rdi;
        DWORD64 R8;
        DWORD64 R9;
        DWORD64 R10;
        DWORD64 R11;
        DWORD64 R12;
        DWORD64 R13;
        DWORD64 R14;
        DWORD64 R15;
        DWORD64 Rip;

        union {
            XMM_SAVE_AREA32 FltSave;

            struct {
                M128A Header[2];
                M128A Legacy[8];
                M128A Xmm0;
                M128A Xmm1;
                M128A Xmm2;
                M128A Xmm3;
                M128A Xmm4;
                M128A Xmm5;
                M128A Xmm6;
                M128A Xmm7;
                M128A Xmm8;
                M128A Xmm9;
                M128A Xmm10;
                M128A Xmm11;
                M128A Xmm12;
                M128A Xmm13;
                M128A Xmm14;
                M128A Xmm15;
            } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME;

        M128A   VectorRegister[26];
        DWORD64 VectorControl;
        DWORD64 DebugControl;
        DWORD64 LastBranchToRip;
        DWORD64 LastBranchFromRip;
        DWORD64 LastExceptionToRip;
        DWORD64 LastExceptionFromRip;
    } CONTEXT;

    typedef struct _LIST_ENTRY {
       struct _LIST_ENTRY *Flink;
       struct _LIST_ENTRY *Blink;
    } LIST_ENTRY, *PLIST_ENTRY, PRLIST_ENTRY;

    typedef struct _RTL_CRITICAL_SECTION_DEBUG {
        WORD   Type;
        WORD   CreatorBackTraceIndex;
        struct _RTL_CRITICAL_SECTION *CriticalSection;
        LIST_ENTRY ProcessLocksList;
        DWORD EntryCount;
        DWORD ContentionCount;
        DWORD Flags;
        WORD   CreatorBackTraceIndexHigh;
        WORD   Identifier;
    } RTL_CRITICAL_SECTION_DEBUG, *PRTL_CRITICAL_SECTION_DEBUG, RTL_RESOURCE_DEBUG, *PRTL_RESOURCE_DEBUG;

    #pragma pack(push, 8)
    typedef struct _RTL_CRITICAL_SECTION {
        PRTL_CRITICAL_SECTION_DEBUG DebugInfo;

        //
        //  The following three fields control entering and exiting the critical
        //  section for the resource
        //

        LONG LockCount;
        LONG RecursionCount;
        HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
        HANDLE LockSemaphore;
        ULONG_PTR SpinCount;        // force size on 64-bit systems when packed
    } RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;
    #pragma pack(pop)

    typedef struct _MODLOAD_DATA {
        DWORD   ssize;                  // size of this struct
        DWORD   ssig;                   // signature identifying the passed data
        VOID   *data;                   // pointer to passed data
        DWORD   size;                   // size of passed data
        DWORD   flags;                  // options
    } MODLOAD_DATA, *PMODLOAD_DATA;

    #define SLMFLAG_VIRTUAL     0x1
    #define SLMFLAG_ALT_INDEX   0x2
    #define SLMFLAG_NO_SYMBOLS  0x4

    extern "C"
    {
    __declspec(dllimport) VOID    __stdcall RtlCaptureContext(CONTEXT *ContextRecord);
    __declspec(dllimport) HANDLE  __stdcall GetCurrentProcess(void);
    __declspec(dllimport) HANDLE  __stdcall GetCurrentThread(void);
    __declspec(dllimport) DWORD   __stdcall SymSetOptions(DWORD SymOptions);
    __declspec(dllimport) BOOL    __stdcall SymInitialize(HANDLE hProcess, const CHAR* UserSearchPath, BOOL fInvadeProcess);
    __declspec(dllimport) DWORD64 __stdcall SymLoadModuleEx(HANDLE hProcess, HANDLE hFile, CHAR const *ImageName, CHAR const *ModuleName, DWORD64 BaseOfDll, DWORD DllSize, MODLOAD_DATA *Data, DWORD Flags);
    __declspec(dllimport) BOOL    __stdcall SymUnloadModule64(HANDLE hProcess, DWORD64 BaseOfDll);
    }

    // NOTE: shared/windef.h ////////////////////////////////////////////////////////////////////
    typedef struct tagPOINT
    {
        LONG  x;
        LONG  y;
    } POINT, *PPOINT, *NPPOINT, *LPPOINT;

    // NOTE: handleapi.h ///////////////////////////////////////////////////////////////////////////
    #define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

    extern "C"
    {
    __declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE hObject);
    }

    // NOTE: consoleapi.h ///////////////////////////////////////////////////////////////////////////
    extern "C"
    {
    __declspec(dllimport) BOOL __stdcall WriteConsoleA(HANDLE hConsoleOutput, const VOID* lpBuffer, DWORD nNumberOfCharsToWrite, DWORD *lpNumberOfCharsWritten, VOID *lpReserved);
    __declspec(dllimport) BOOL __stdcall AllocConsole(VOID);
    __declspec(dllimport) BOOL __stdcall FreeConsole(VOID);
    __declspec(dllimport) BOOL __stdcall AttachConsole(DWORD dwProcessId);
    __declspec(dllimport) BOOL __stdcall GetConsoleMode(HANDLE hConsoleHandle, DWORD *lpMode);
    }

    // NOTE: um/minwinbase.h ///////////////////////////////////////////////////////////////////////
    // NOTE: FindFirstFile
    #define FIND_FIRST_EX_CASE_SENSITIVE 0x00000001
    #define FIND_FIRST_EX_LARGE_FETCH    0x00000002

    // NOTE: WaitFor..
    #define WAIT_FAILED      ((DWORD)0xFFFFFFFF)
    #define WAIT_OBJECT_0    ((STATUS_WAIT_0 ) + 0 )
    #define WAIT_ABANDONED   ((STATUS_ABANDONED_WAIT_0 ) + 0 )
    #define WAIT_ABANDONED_0 ((STATUS_ABANDONED_WAIT_0 ) + 0 )

    typedef enum _GET_FILEEX_INFO_LEVELS {
        GetFileExInfoStandard,
        GetFileExMaxInfoLevel
    } GET_FILEEX_INFO_LEVELS;

    typedef struct _SECURITY_ATTRIBUTES {
        DWORD nLength;
        VOID *lpSecurityDescriptor;
        BOOL  bInheritHandle;
    } SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

    typedef enum _FINDEX_INFO_LEVELS {
        FindExInfoStandard,
        FindExInfoBasic,
        FindExInfoMaxInfoLevel
    } FINDEX_INFO_LEVELS;

    typedef enum _FINDEX_SEARCH_OPS {
        FindExSearchNameMatch,
        FindExSearchLimitToDirectories,
        FindExSearchLimitToDevices,
        FindExSearchMaxSearchOp
    } FINDEX_SEARCH_OPS;

    typedef struct _WIN32_FIND_DATAW {
        DWORD    dwFileAttributes;
        FILETIME ftCreationTime;
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        DWORD    nFileSizeHigh;
        DWORD    nFileSizeLow;
        DWORD    dwReserved0;
        DWORD    dwReserved1;
        WCHAR    cFileName[ MAX_PATH ];
        WCHAR    cAlternateFileName[ 14 ];
        #ifdef _MAC
        DWORD    dwFileType;
        DWORD    dwCreatorType;
        WORD     wFinderFlags;
        #endif
    } WIN32_FIND_DATAW, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;

    typedef struct _SYSTEMTIME {
        WORD wYear;
        WORD wMonth;
        WORD wDayOfWeek;
        WORD wDay;
        WORD wHour;
        WORD wMinute;
        WORD wSecond;
        WORD wMilliseconds;
    } SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

    typedef struct _OVERLAPPED {
        ULONG_PTR Internal;
        ULONG_PTR InternalHigh;
        union {
            struct {
                DWORD Offset;
                DWORD OffsetHigh;
            } DUMMYSTRUCTNAME;
            VOID *Pointer;
        } DUMMYUNIONNAME;

        HANDLE  hEvent;
    } OVERLAPPED, *LPOVERLAPPED;

    typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;

    // NOTE: um/winbase.h //////////////////////////////////////////////////////////////////////////
    #define WAIT_FAILED   ((DWORD)0xFFFFFFFF)
    #define WAIT_OBJECT_0 ((STATUS_WAIT_0 ) + 0 )

    #define INFINITE 0xFFFFFFFF // Wait/Synchronisation: Infinite timeout

    #define STD_INPUT_HANDLE  ((DWORD)-10)
    #define STD_OUTPUT_HANDLE ((DWORD)-11)
    #define STD_ERROR_HANDLE  ((DWORD)-12)

    // NOTE: MoveFile
    #define MOVEFILE_REPLACE_EXISTING 0x00000001
    #define MOVEFILE_COPY_ALLOWED     0x00000002

    // NOTE: FormatMessageA
    #define FORMAT_MESSAGE_ALLOCATE_BUFFER 0x00000100
    #define FORMAT_MESSAGE_IGNORE_INSERTS  0x00000200
    #define FORMAT_MESSAGE_FROM_HMODULE    0x00000800
    #define FORMAT_MESSAGE_FROM_SYSTEM     0x00001000

    // NOTE: CreateProcessW
    #define STARTF_USESTDHANDLES       0x00000100

    extern "C"
    {
    __declspec(dllimport) BOOL   __stdcall MoveFileExW     (const WCHAR *lpExistingFileName, const WCHAR *lpNewFileName, DWORD dwFlags);
    __declspec(dllimport) BOOL   __stdcall CopyFileW       (const WCHAR *lpExistingFileName, const WCHAR *lpNewFileName, BOOL bFailIfExists);
    __declspec(dllimport) HANDLE __stdcall CreateSemaphoreA(SECURITY_ATTRIBUTES *lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, const CHAR *lpName);
    __declspec(dllimport) DWORD  __stdcall FormatMessageW  (DWORD dwFlags, VOID const *lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPWSTR lpBuffer, DWORD nSize, va_list *Arguments);
    __declspec(dllimport) HLOCAL __stdcall LocalFree       (HLOCAL hMem);
    }

    // NOTE: um/stringapiset.h /////////////////////////////////////////////////////////////////////
    extern "C"
    {
    __declspec(dllimport) int __stdcall MultiByteToWideChar(UINT CodePage, DWORD dwFlags, const CHAR *lpMultiByteStr, int cbMultiByte, WCHAR *lpWideCharStr, int cchWideChar);
    __declspec(dllimport) int __stdcall WideCharToMultiByte(UINT CodePage, DWORD dwFlags, const WCHAR *lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, const CHAR *lpDefaultChar, BOOL *lpUsedDefaultChar);
    }

    // NOTE: um/fileapi.h //////////////////////////////////////////////////////////////////////////
    #define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
    #define INVALID_FILE_ATTRIBUTES ((DWORD)-1)

    // NOTE: CreateFile
    #define CREATE_NEW        1
    #define CREATE_ALWAYS     2
    #define OPEN_EXISTING     3
    #define OPEN_ALWAYS       4
    #define TRUNCATE_EXISTING 5

    typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
        DWORD dwFileAttributes;
        FILETIME ftCreationTime;
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        DWORD nFileSizeHigh;
        DWORD nFileSizeLow;
    } WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

    extern "C"
    {
    __declspec(dllimport) BOOL   __stdcall CreateDirectoryW    (const WCHAR *lpPathName, SECURITY_ATTRIBUTES *lpSecurityAttributes);
    __declspec(dllimport) BOOL   __stdcall RemoveDirectoryW    (const WCHAR *lpPathName);
    __declspec(dllimport) BOOL   __stdcall FindNextFileW       (HANDLE hFindFile, WIN32_FIND_DATAW *lpFindFileData);
    __declspec(dllimport) HANDLE __stdcall FindFirstFileExW    (const WCHAR *lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, VOID *lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, VOID *lpSearchFilter, DWORD dwAdditionalFlags);
    __declspec(dllimport) BOOL   __stdcall GetFileAttributesExW(const WCHAR *lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, VOID *lpFileInformation);
    __declspec(dllimport) BOOL   __stdcall GetFileSizeEx       (HANDLE hFile, LARGE_INTEGER *lpFileSize);
    __declspec(dllimport) BOOL   __stdcall DeleteFileW         (const WCHAR *lpFileName);
    __declspec(dllimport) HANDLE __stdcall CreateFileW         (const WCHAR *lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, SECURITY_ATTRIBUTES *lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
    __declspec(dllimport) BOOL   __stdcall ReadFile            (HANDLE hFile, VOID *lpBuffer, DWORD nNumberOfBytesToRead, DWORD *lpNumberOfBytesRead, OVERLAPPED *lpOverlapped);
    __declspec(dllimport) BOOL   __stdcall WriteFile           (HANDLE hFile, const VOID *lpBuffer, DWORD nNumberOfBytesToWrite, DWORD *lpNumberOfBytesWritten, OVERLAPPED *lpOverlapped);

    }

    // NOTE: um/processenv.h ///////////////////////////////////////////////////////////////////////
    extern "C"
    {
    __declspec(dllimport) DWORD  __stdcall GetCurrentDirectoryW(DWORD nBufferLength, WCHAR *lpBuffer);
    __declspec(dllimport) HANDLE __stdcall GetStdHandle(DWORD nStdHandle);
    }

    // NOTE: um/sysinfoapi.h ///////////////////////////////////////////////////////////////////////
    typedef struct _SYSTEM_INFO {
        union {
            DWORD dwOemId;          // Obsolete field...do not use
            struct {
                WORD wProcessorArchitecture;
                WORD wReserved;
            } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME;
        DWORD dwPageSize;
        VOID *lpMinimumApplicationAddress;
        VOID *lpMaximumApplicationAddress;
        DWORD_PTR dwActiveProcessorMask;
        DWORD dwNumberOfProcessors;
        DWORD dwProcessorType;
        DWORD dwAllocationGranularity;
        WORD wProcessorLevel;
        WORD wProcessorRevision;
    } SYSTEM_INFO, *LPSYSTEM_INFO;

    extern "C"
    {
    __declspec(dllimport) VOID __stdcall GetSystemInfo(SYSTEM_INFO *lpSystemInfo);
    __declspec(dllimport) VOID __stdcall GetSystemTime(SYSTEMTIME *lpSystemTime);
    __declspec(dllimport) VOID __stdcall GetSystemTimeAsFileTime(FILETIME *lpSystemTimeAsFileTime);
    __declspec(dllimport) VOID __stdcall GetLocalTime(SYSTEMTIME *lpSystemTime);
    }

    // NOTE: um/timezoneapi.h //////////////////////////////////////////////////////////////////////
    extern "C"
    {
    __declspec(dllimport) BOOL __stdcall FileTimeToSystemTime(const FILETIME* lpFileTime, SYSTEMTIME *lpSystemTime);
    __declspec(dllimport) BOOL __stdcall SystemTimeToFileTime(const SYSTEMTIME* lpSystemTime, FILETIME *lpFileTime);
    }

    // NOTE: shared/windef.h ///////////////////////////////////////////////////////////////////////
    typedef struct tagRECT {
        LONG left;
        LONG top;
        LONG right;
        LONG bottom;
    } RECT;

    struct HWND__ {
      int unused;
    };
    typedef struct HWND__ *HWND;

    struct DPI_AWARENESS_CONTEXT__ {
        int unused;
    };
    typedef struct DPI_AWARENESS_CONTEXT__ *DPI_AWARENESS_CONTEXT;

    typedef enum DPI_AWARENESS {
        DPI_AWARENESS_INVALID           = -1,
        DPI_AWARENESS_UNAWARE           = 0,
        DPI_AWARENESS_SYSTEM_AWARE      = 1,
        DPI_AWARENESS_PER_MONITOR_AWARE = 2
    } DPI_AWARENESS;

    #define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)

    // NOTE: um/winuser.h //////////////////////////////////////////////////////////////////////////
    typedef struct tagWINDOWPLACEMENT {
        UINT  length;
        UINT  flags;
        UINT  showCmd;
        POINT ptMinPosition;
        POINT ptMaxPosition;
        RECT  rcNormalPosition;
    #ifdef _MAC
        RECT  rcDevice;
    #endif
    } WINDOWPLACEMENT;
    typedef WINDOWPLACEMENT *PWINDOWPLACEMENT, *LPWINDOWPLACEMENT;

    #define SW_HIDE           0
    #define SW_NORMAL         1
    #define SW_MAXIMIZE       3
    #define SW_SHOWNOACTIVATE 4
    #define SW_SHOW           5
    #define SW_FORCEMINIMIZE  11

    extern "C"
    {
    __declspec(dllimport) BOOL __stdcall GetWindowRect           (HWND hWnd, RECT *lpRect);
    __declspec(dllimport) BOOL __stdcall SetWindowPos            (HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
    __declspec(dllimport) UINT __stdcall GetWindowModuleFileNameA(HWND hwnd, LPSTR pszFileName, UINT cchFileNameMax);
    __declspec(dllimport) BOOL __stdcall ShowWindow              (HWND hWnd, int nCmdShow);
    __declspec(dllimport) BOOL __stdcall GetWindowPlacement      (HWND hWnd, WINDOWPLACEMENT *lpwndpl);

    }

    // NOTE: um/wininet.h //////////////////////////////////////////////////////////////////////////
    typedef WORD INTERNET_PORT;
    typedef VOID *HINTERNET;

    // NOTE: um/winhttp.h //////////////////////////////////////////////////////////////////////////
    #define WINHTTP_ACCESS_TYPE_DEFAULT_PROXY   0
    #define WINHTTP_ACCESS_TYPE_NO_PROXY        1
    #define WINHTTP_ACCESS_TYPE_NAMED_PROXY     3
    #define WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY 4

    #define INTERNET_DEFAULT_PORT           0           // use the protocol-specific default
    #define INTERNET_DEFAULT_HTTP_PORT      80          //    "     "  HTTP   "
    #define INTERNET_DEFAULT_HTTPS_PORT     443         //    "     "  HTTPS  "

    // NOTE: WinHttpOpen
    #define WINHTTP_FLAG_ASYNC              0x10000000  // this session is asynchronous (where supported)
    #define WINHTTP_FLAG_SECURE_DEFAULTS    0x30000000  // note that this flag also forces async

    // NOTE: WinHttpOpenRequest
    #define WINHTTP_FLAG_SECURE                0x00800000  // use SSL if applicable (HTTPS)
    #define WINHTTP_FLAG_ESCAPE_PERCENT        0x00000004  // if escaping enabled, escape percent as well
    #define WINHTTP_FLAG_NULL_CODEPAGE         0x00000008  // assume all symbols are ASCII, use fast convertion
    #define WINHTTP_FLAG_ESCAPE_DISABLE        0x00000040  // disable escaping
    #define WINHTTP_FLAG_ESCAPE_DISABLE_QUERY  0x00000080  // if escaping enabled escape path part, but do not escape query
    #define WINHTTP_FLAG_BYPASS_PROXY_CACHE    0x00000100  // add "pragma: no-cache" request header
    #define WINHTTP_FLAG_REFRESH               WINHTTP_FLAG_BYPASS_PROXY_CACHE
    #define WINHTTP_FLAG_AUTOMATIC_CHUNKING    0x00000200  // Send request without content-length header or chunked TE

    #define WINHTTP_NO_PROXY_NAME     NULL
    #define WINHTTP_NO_PROXY_BYPASS   NULL

    //
    // WINHTTP_QUERY_FLAG_NUMBER - if this bit is set in the dwInfoLevel parameter of
    // HttpQueryHeader(), then the value of the header will be converted to a number
    // before being returned to the caller, if applicable
    //
    #define WINHTTP_QUERY_FLAG_NUMBER                  0x20000000

    #define WINHTTP_QUERY_MIME_VERSION                 0
    #define WINHTTP_QUERY_CONTENT_TYPE                 1
    #define WINHTTP_QUERY_CONTENT_TRANSFER_ENCODING    2
    #define WINHTTP_QUERY_CONTENT_ID                   3
    #define WINHTTP_QUERY_CONTENT_DESCRIPTION          4
    #define WINHTTP_QUERY_CONTENT_LENGTH               5
    #define WINHTTP_QUERY_CONTENT_LANGUAGE             6
    #define WINHTTP_QUERY_ALLOW                        7
    #define WINHTTP_QUERY_PUBLIC                       8
    #define WINHTTP_QUERY_DATE                         9
    #define WINHTTP_QUERY_EXPIRES                      10
    #define WINHTTP_QUERY_LAST_MODIFIED                11
    #define WINHTTP_QUERY_MESSAGE_ID                   12
    #define WINHTTP_QUERY_URI                          13
    #define WINHTTP_QUERY_DERIVED_FROM                 14
    #define WINHTTP_QUERY_COST                         15
    #define WINHTTP_QUERY_LINK                         16
    #define WINHTTP_QUERY_PRAGMA                       17
    #define WINHTTP_QUERY_VERSION                      18  // special: part of status line
    #define WINHTTP_QUERY_STATUS_CODE                  19  // special: part of status line
    #define WINHTTP_QUERY_STATUS_TEXT                  20  // special: part of status line
    #define WINHTTP_QUERY_RAW_HEADERS                  21  // special: all headers as ASCIIZ
    #define WINHTTP_QUERY_RAW_HEADERS_CRLF             22  // special: all headers
    #define WINHTTP_QUERY_CONNECTION                   23
    #define WINHTTP_QUERY_ACCEPT                       24
    #define WINHTTP_QUERY_ACCEPT_CHARSET               25
    #define WINHTTP_QUERY_ACCEPT_ENCODING              26
    #define WINHTTP_QUERY_ACCEPT_LANGUAGE              27
    #define WINHTTP_QUERY_AUTHORIZATION                28
    #define WINHTTP_QUERY_CONTENT_ENCODING             29
    #define WINHTTP_QUERY_FORWARDED                    30
    #define WINHTTP_QUERY_FROM                         31
    #define WINHTTP_QUERY_IF_MODIFIED_SINCE            32
    #define WINHTTP_QUERY_LOCATION                     33
    #define WINHTTP_QUERY_ORIG_URI                     34
    #define WINHTTP_QUERY_REFERER                      35
    #define WINHTTP_QUERY_RETRY_AFTER                  36
    #define WINHTTP_QUERY_SERVER                       37
    #define WINHTTP_QUERY_TITLE                        38
    #define WINHTTP_QUERY_USER_AGENT                   39
    #define WINHTTP_QUERY_WWW_AUTHENTICATE             40
    #define WINHTTP_QUERY_PROXY_AUTHENTICATE           41
    #define WINHTTP_QUERY_ACCEPT_RANGES                42
    #define WINHTTP_QUERY_SET_COOKIE                   43
    #define WINHTTP_QUERY_COOKIE                       44
    #define WINHTTP_QUERY_REQUEST_METHOD               45  // special: GET/POST etc.
    #define WINHTTP_QUERY_REFRESH                      46
    #define WINHTTP_QUERY_CONTENT_DISPOSITION          47

    // NOTE: WinHttpQueryHeaders prettifiers for optional parameters.
    #define WINHTTP_HEADER_NAME_BY_INDEX           NULL
    #define WINHTTP_NO_OUTPUT_BUFFER               NULL
    #define WINHTTP_NO_HEADER_INDEX                NULL

    // NOTE: Http Response Status Codes
    #define HTTP_STATUS_CONTINUE            100 // OK to continue with request
    #define HTTP_STATUS_SWITCH_PROTOCOLS    101 // server has switched protocols in upgrade header

    #define HTTP_STATUS_OK                  200 // request completed
    #define HTTP_STATUS_CREATED             201 // object created, reason = new URI
    #define HTTP_STATUS_ACCEPTED            202 // async completion (TBS)
    #define HTTP_STATUS_PARTIAL             203 // partial completion
    #define HTTP_STATUS_NO_CONTENT          204 // no info to return
    #define HTTP_STATUS_RESET_CONTENT       205 // request completed, but clear form
    #define HTTP_STATUS_PARTIAL_CONTENT     206 // partial GET fulfilled
    #define HTTP_STATUS_WEBDAV_MULTI_STATUS 207 // WebDAV Multi-Status

    #define HTTP_STATUS_AMBIGUOUS           300 // server couldn't decide what to return
    #define HTTP_STATUS_MOVED               301 // object permanently moved
    #define HTTP_STATUS_REDIRECT            302 // object temporarily moved
    #define HTTP_STATUS_REDIRECT_METHOD     303 // redirection w/ new access method
    #define HTTP_STATUS_NOT_MODIFIED        304 // if-modified-since was not modified
    #define HTTP_STATUS_USE_PROXY           305 // redirection to proxy, location header specifies proxy to use
    #define HTTP_STATUS_REDIRECT_KEEP_VERB  307 // HTTP/1.1: keep same verb
    #define HTTP_STATUS_PERMANENT_REDIRECT  308 // Object permanently moved keep verb

    #define HTTP_STATUS_BAD_REQUEST         400 // invalid syntax
    #define HTTP_STATUS_DENIED              401 // access denied
    #define HTTP_STATUS_PAYMENT_REQ         402 // payment required
    #define HTTP_STATUS_FORBIDDEN           403 // request forbidden
    #define HTTP_STATUS_NOT_FOUND           404 // object not found
    #define HTTP_STATUS_BAD_METHOD          405 // method is not allowed
    #define HTTP_STATUS_NONE_ACCEPTABLE     406 // no response acceptable to client found
    #define HTTP_STATUS_PROXY_AUTH_REQ      407 // proxy authentication required
    #define HTTP_STATUS_REQUEST_TIMEOUT     408 // server timed out waiting for request
    #define HTTP_STATUS_CONFLICT            409 // user should resubmit with more info
    #define HTTP_STATUS_GONE                410 // the resource is no longer available
    #define HTTP_STATUS_LENGTH_REQUIRED     411 // the server refused to accept request w/o a length
    #define HTTP_STATUS_PRECOND_FAILED      412 // precondition given in request failed
    #define HTTP_STATUS_REQUEST_TOO_LARGE   413 // request entity was too large
    #define HTTP_STATUS_URI_TOO_LONG        414 // request URI too long
    #define HTTP_STATUS_UNSUPPORTED_MEDIA   415 // unsupported media type
    #define HTTP_STATUS_RETRY_WITH          449 // retry after doing the appropriate action.

    #define HTTP_STATUS_SERVER_ERROR        500 // internal server error
    #define HTTP_STATUS_NOT_SUPPORTED       501 // required not supported
    #define HTTP_STATUS_BAD_GATEWAY         502 // error response received from gateway
    #define HTTP_STATUS_SERVICE_UNAVAIL     503 // temporarily overloaded
    #define HTTP_STATUS_GATEWAY_TIMEOUT     504 // timed out waiting for gateway
    #define HTTP_STATUS_VERSION_NOT_SUP     505 // HTTP version not supported

    #define HTTP_STATUS_FIRST               HTTP_STATUS_CONTINUE
    #define HTTP_STATUS_LAST                HTTP_STATUS_VERSION_NOT_SUP

    #define WINHTTP_CALLBACK_STATUS_RESOLVING_NAME              0x00000001
    #define WINHTTP_CALLBACK_STATUS_NAME_RESOLVED               0x00000002
    #define WINHTTP_CALLBACK_STATUS_CONNECTING_TO_SERVER        0x00000004
    #define WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER         0x00000008
    #define WINHTTP_CALLBACK_STATUS_SENDING_REQUEST             0x00000010
    #define WINHTTP_CALLBACK_STATUS_REQUEST_SENT                0x00000020
    #define WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE          0x00000040
    #define WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED           0x00000080
    #define WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION          0x00000100
    #define WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED           0x00000200
    #define WINHTTP_CALLBACK_STATUS_HANDLE_CREATED              0x00000400
    #define WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING              0x00000800
    #define WINHTTP_CALLBACK_STATUS_DETECTING_PROXY             0x00001000
    #define WINHTTP_CALLBACK_STATUS_REDIRECT                    0x00004000
    #define WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE       0x00008000
    #define WINHTTP_CALLBACK_STATUS_SECURE_FAILURE              0x00010000
    #define WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE           0x00020000
    #define WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE              0x00040000
    #define WINHTTP_CALLBACK_STATUS_READ_COMPLETE               0x00080000
    #define WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE              0x00100000
    #define WINHTTP_CALLBACK_STATUS_REQUEST_ERROR               0x00200000
    #define WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE        0x00400000

    #define WINHTTP_CALLBACK_STATUS_GETPROXYFORURL_COMPLETE     0x01000000
    #define WINHTTP_CALLBACK_STATUS_CLOSE_COMPLETE              0x02000000
    #define WINHTTP_CALLBACK_STATUS_SHUTDOWN_COMPLETE           0x04000000
    #define WINHTTP_CALLBACK_STATUS_SETTINGS_WRITE_COMPLETE     0x10000000
    #define WINHTTP_CALLBACK_STATUS_SETTINGS_READ_COMPLETE      0x20000000

    #define WINHTTP_CALLBACK_FLAG_RESOLVE_NAME              (WINHTTP_CALLBACK_STATUS_RESOLVING_NAME       | WINHTTP_CALLBACK_STATUS_NAME_RESOLVED)
    #define WINHTTP_CALLBACK_FLAG_CONNECT_TO_SERVER         (WINHTTP_CALLBACK_STATUS_CONNECTING_TO_SERVER | WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER)
    #define WINHTTP_CALLBACK_FLAG_SEND_REQUEST              (WINHTTP_CALLBACK_STATUS_SENDING_REQUEST      | WINHTTP_CALLBACK_STATUS_REQUEST_SENT)
    #define WINHTTP_CALLBACK_FLAG_RECEIVE_RESPONSE          (WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE   | WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED)
    #define WINHTTP_CALLBACK_FLAG_CLOSE_CONNECTION          (WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION   | WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED)
    #define WINHTTP_CALLBACK_FLAG_HANDLES                   (WINHTTP_CALLBACK_STATUS_HANDLE_CREATED       | WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING)
    #define WINHTTP_CALLBACK_FLAG_DETECTING_PROXY           WINHTTP_CALLBACK_STATUS_DETECTING_PROXY
    #define WINHTTP_CALLBACK_FLAG_REDIRECT                  WINHTTP_CALLBACK_STATUS_REDIRECT
    #define WINHTTP_CALLBACK_FLAG_INTERMEDIATE_RESPONSE     WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE
    #define WINHTTP_CALLBACK_FLAG_SECURE_FAILURE            WINHTTP_CALLBACK_STATUS_SECURE_FAILURE
    #define WINHTTP_CALLBACK_FLAG_SENDREQUEST_COMPLETE      WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE
    #define WINHTTP_CALLBACK_FLAG_HEADERS_AVAILABLE         WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE
    #define WINHTTP_CALLBACK_FLAG_DATA_AVAILABLE            WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE
    #define WINHTTP_CALLBACK_FLAG_READ_COMPLETE             WINHTTP_CALLBACK_STATUS_READ_COMPLETE
    #define WINHTTP_CALLBACK_FLAG_WRITE_COMPLETE            WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE
    #define WINHTTP_CALLBACK_FLAG_REQUEST_ERROR             WINHTTP_CALLBACK_STATUS_REQUEST_ERROR

    #define WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE   WINHTTP_CALLBACK_STATUS_GETPROXYFORURL_COMPLETE

    #define WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS           (WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE       \
                                                            | WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE         \
                                                            | WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE            \
                                                            | WINHTTP_CALLBACK_STATUS_READ_COMPLETE             \
                                                            | WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE            \
                                                            | WINHTTP_CALLBACK_STATUS_REQUEST_ERROR             \
                                                            | WINHTTP_CALLBACK_STATUS_GETPROXYFORURL_COMPLETE)

    #define WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS         0xffffffff
    #define WINHTTP_INVALID_STATUS_CALLBACK                 ((WINHTTP_STATUS_CALLBACK)(-1L))

    typedef struct _WINHTTP_EXTENDED_HEADER
    {
        union
        {
            CHAR const *pwszName;
            WCHAR const *pszName;
        };
        union
        {
            WCHAR const *pwszValue;
            CHAR const *pszValue;
        };
    } WINHTTP_EXTENDED_HEADER, *PWINHTTP_EXTENDED_HEADER;

    typedef struct _WINHTTP_ASYNC_RESULT
    {
        DWORD *dwResult;  // indicates which async API has encountered an error
        DWORD dwError;    // the error code if the API failed
    } WINHTTP_ASYNC_RESULT, *LPWINHTTP_ASYNC_RESULT, *PWINHTTP_ASYNC_RESULT;

    typedef
    VOID
    (*WINHTTP_STATUS_CALLBACK)(
        HINTERNET hInternet,
        DWORD *dwContext,
        DWORD dwInternetStatus,
        VOID *lpvStatusInformation,
        DWORD dwStatusInformationLength
        );

    extern "C"
    {
    __declspec(dllimport) HINTERNET               __stdcall WinHttpOpen(WCHAR const *pszAgentW, DWORD dwAccessType, WCHAR const *pszProxyW, WCHAR const *pszProxyBypassW, DWORD dwFlags);
    __declspec(dllimport) BOOL                    __stdcall WinHttpCloseHandle(HINTERNET hInternet);
    __declspec(dllimport) HINTERNET               __stdcall WinHttpConnect(HINTERNET hSession, WCHAR const *pswzServerName, INTERNET_PORT nServerPort, DWORD dwReserved);
    __declspec(dllimport) BOOL                    __stdcall WinHttpReadData(HINTERNET hRequest, VOID *lpBuffer, DWORD dwNumberOfBytesToRead, DWORD *lpdwNumberOfBytesRead);
    __declspec(dllimport) HINTERNET               __stdcall WinHttpOpenRequest(HINTERNET hConnect, WCHAR const *pwszVerb, WCHAR const *pwszObjectName, WCHAR const *pwszVersion, WCHAR const *pwszReferrer, WCHAR const *ppwszAcceptTypes, DWORD dwFlags);
    __declspec(dllimport) BOOL                    __stdcall WinHttpSendRequest(HINTERNET hRequest, WCHAR const *lpszHeaders, DWORD dwHeadersLength, VOID *lpOptional, DWORD dwOptionalLength, DWORD dwTotalLength, DWORD_PTR dwContext);
    __declspec(dllimport) DWORD                   __stdcall WinHttpAddRequestHeadersEx(HINTERNET hRequest, DWORD dwModifiers, ULONGLONG ullFlags, ULONGLONG ullExtra, DWORD cHeaders, WINHTTP_EXTENDED_HEADER *pHeaders);
    __declspec(dllimport) BOOL                    __stdcall WinHttpSetCredentials(HINTERNET   hRequest,     // HINTERNET handle returned by WinHttpOpenRequest.
                                                                                  DWORD       AuthTargets,  // Only WINHTTP_AUTH_TARGET_SERVER and WINHTTP_AUTH_TARGET_PROXY are supported in this version and they are mutually exclusive
                                                                                  DWORD       AuthScheme,   // must be one of the supported Auth Schemes returned from WinHttpQueryAuthSchemes()
                                                                                  WCHAR *     pwszUserName, // 1) NULL if default creds is to be used, in which case pszPassword will be ignored
                                                                                  WCHAR *     pwszPassword, // 1) "" == Blank Password; 2)Parameter ignored if pszUserName is NULL; 3) Invalid to pass in NULL if pszUserName is not NULL
                                                                                  VOID *      pAuthParams);
    __declspec(dllimport) BOOL                    __stdcall WinHttpQueryHeaders(HINTERNET hRequest, DWORD dwInfoLevel, WCHAR const *pwszName, VOID *lpBuffer, DWORD *lpdwBufferLength, DWORD *lpdwIndex);
    __declspec(dllimport) BOOL                    __stdcall WinHttpReceiveResponse(HINTERNET hRequest, VOID *lpReserved);
    __declspec(dllimport) WINHTTP_STATUS_CALLBACK __stdcall WinHttpSetStatusCallback(HINTERNET hInternet, WINHTTP_STATUS_CALLBACK lpfnInternetCallback, DWORD dwNotificationFlags, DWORD_PTR dwReserved);
    }

    // NOTE: um/DbgHelp.h //////////////////////////////////////////////////////////////////////////
    #define SYMOPT_CASE_INSENSITIVE           0x00000001
    #define SYMOPT_UNDNAME                    0x00000002
    #define SYMOPT_DEFERRED_LOADS             0x00000004
    #define SYMOPT_NO_CPP                     0x00000008
    #define SYMOPT_LOAD_LINES                 0x00000010
    #define SYMOPT_OMAP_FIND_NEAREST          0x00000020
    #define SYMOPT_LOAD_ANYTHING              0x00000040
    #define SYMOPT_IGNORE_CVREC               0x00000080
    #define SYMOPT_NO_UNQUALIFIED_LOADS       0x00000100
    #define SYMOPT_FAIL_CRITICAL_ERRORS       0x00000200
    #define SYMOPT_EXACT_SYMBOLS              0x00000400
    #define SYMOPT_ALLOW_ABSOLUTE_SYMBOLS     0x00000800
    #define SYMOPT_IGNORE_NT_SYMPATH          0x00001000
    #define SYMOPT_INCLUDE_32BIT_MODULES      0x00002000
    #define SYMOPT_PUBLICS_ONLY               0x00004000
    #define SYMOPT_NO_PUBLICS                 0x00008000
    #define SYMOPT_AUTO_PUBLICS               0x00010000
    #define SYMOPT_NO_IMAGE_SEARCH            0x00020000
    #define SYMOPT_SECURE                     0x00040000
    #define SYMOPT_NO_PROMPTS                 0x00080000
    #define SYMOPT_OVERWRITE                  0x00100000
    #define SYMOPT_IGNORE_IMAGEDIR            0x00200000
    #define SYMOPT_FLAT_DIRECTORY             0x00400000
    #define SYMOPT_FAVOR_COMPRESSED           0x00800000
    #define SYMOPT_ALLOW_ZERO_ADDRESS         0x01000000
    #define SYMOPT_DISABLE_SYMSRV_AUTODETECT  0x02000000
    #define SYMOPT_READONLY_CACHE             0x04000000
    #define SYMOPT_SYMPATH_LAST               0x08000000
    #define SYMOPT_DISABLE_FAST_SYMBOLS       0x10000000
    #define SYMOPT_DISABLE_SYMSRV_TIMEOUT     0x20000000
    #define SYMOPT_DISABLE_SRVSTAR_ON_STARTUP 0x40000000
    #define SYMOPT_DEBUG                      0x80000000

    #define MAX_SYM_NAME                      2000

    typedef enum {
        AddrMode1616,
        AddrMode1632,
        AddrModeReal,
        AddrModeFlat
    } ADDRESS_MODE;

    typedef struct _tagADDRESS64 {
        DWORD64       Offset;
        WORD          Segment;
        ADDRESS_MODE  Mode;
    } ADDRESS64, *LPADDRESS64;


    typedef struct _KDHELP64 {
        DWORD64  Thread;
        DWORD    ThCallbackStack;
        DWORD    ThCallbackBStore;
        DWORD    NextCallback;
        DWORD    FramePointer;
        DWORD64  KiCallUserMode;
        DWORD64  KeUserCallbackDispatcher;
        DWORD64  SystemRangeStart;
        DWORD64  KiUserExceptionDispatcher;
        DWORD64  StackBase;
        DWORD64  StackLimit;
        DWORD    BuildVersion;
        DWORD    RetpolineStubFunctionTableSize;
        DWORD64  RetpolineStubFunctionTable;
        DWORD    RetpolineStubOffset;
        DWORD    RetpolineStubSize;
        DWORD64  Reserved0[2];
    } KDHELP64, *PKDHELP64;

    typedef struct _tagSTACKFRAME64 {
        ADDRESS64 AddrPC;         // program counter
        ADDRESS64 AddrReturn;     // return address
        ADDRESS64 AddrFrame;      // frame pointer
        ADDRESS64 AddrStack;      // stack pointer
        ADDRESS64 AddrBStore;     // backing store pointer
        VOID     *FuncTableEntry; // pointer to pdata/fpo or NULL
        DWORD64   Params[4];      // possible arguments to the function
        BOOL      Far;            // WOW far call
        BOOL      Virtual;        // is this a virtual frame?
        DWORD64   Reserved[3];
        KDHELP64  KdHelp;
    } STACKFRAME64;

    typedef struct _IMAGEHLP_LINEW64 {
        DWORD   SizeOfStruct; // set to sizeof(IMAGEHLP_LINE64)
        VOID   *Key;          // internal
        DWORD   LineNumber;   // line number in file
        WCHAR  *FileName;     // full filename
        DWORD64 Address;      // first instruction of line
    } IMAGEHLP_LINEW64;

    typedef struct _SYMBOL_INFOW {
        ULONG   SizeOfStruct;
        ULONG   TypeIndex; // Type Index of symbol
        ULONG64 Reserved[2];
        ULONG   Index;
        ULONG   Size;
        ULONG64 ModBase; // Base Address of module comtaining this symbol
        ULONG   Flags;
        ULONG64 Value;    // Value of symbol, ValuePresent should be 1
        ULONG64 Address;  // Address of symbol including base address of module
        ULONG   Register; // register holding value or pointer to value
        ULONG   Scope;    // scope of the symbol
        ULONG   Tag;      // pdb classification
        ULONG   NameLen;  // Actual length of name
        ULONG   MaxNameLen;
        WCHAR   Name[1]; // Name of symbol
    } SYMBOL_INFOW;

    typedef BOOL   (__stdcall READ_PROCESS_MEMORY_ROUTINE64)(HANDLE hProcess, DWORD64 qwBaseAddress, VOID *lpBuffer, DWORD nSize, DWORD *lpNumberOfBytesRead);
    typedef VOID * (__stdcall FUNCTION_TABLE_ACCESS_ROUTINE64)(HANDLE ahProcess, DWORD64 AddrBase);
    typedef DWORD64(__stdcall GET_MODULE_BASE_ROUTINE64)(HANDLE hProcess, DWORD64 Address);
    typedef DWORD64(__stdcall TRANSLATE_ADDRESS_ROUTINE64)(HANDLE hProcess, HANDLE hThread, ADDRESS64 *lpaddr);

    extern "C"
    {
    __declspec(dllimport) BOOL    __stdcall StackWalk64             (DWORD MachineType, HANDLE hProcess, HANDLE hThread, STACKFRAME64 *StackFrame, VOID *ContextRecord, READ_PROCESS_MEMORY_ROUTINE64 *ReadMemoryRoutine, FUNCTION_TABLE_ACCESS_ROUTINE64 *FunctionTableAccessRoutine, GET_MODULE_BASE_ROUTINE64 *GetModuleBaseRoutine, TRANSLATE_ADDRESS_ROUTINE64 *TranslateAddress);
    __declspec(dllimport) BOOL    __stdcall SymFromAddrW            (HANDLE hProcess, DWORD64 Address, DWORD64 *Displacement, SYMBOL_INFOW *Symbol);
    __declspec(dllimport) VOID *  __stdcall SymFunctionTableAccess64(HANDLE hProcess, DWORD64 AddrBase);
    __declspec(dllimport) BOOL    __stdcall SymGetLineFromAddrW64   (HANDLE hProcess, DWORD64 dwAddr, DWORD *pdwDisplacement, IMAGEHLP_LINEW64 *Line);
    __declspec(dllimport) DWORD64 __stdcall SymGetModuleBase64      (HANDLE hProcess, DWORD64 qwAddr);
    __declspec(dllimport) BOOL    __stdcall SymRefreshModuleList    (HANDLE hProcess);
    };

    // NOTE: um/errhandlingapi.h ///////////////////////////////////////////////////////////////////
    extern "C"
    {
    __declspec(dllimport) DWORD __stdcall GetLastError(VOID);
    }

    // NOTE: um/libloaderapi.h /////////////////////////////////////////////////////////////////////
    extern "C"
    {
    __declspec(dllimport) HMODULE __stdcall LoadLibraryA      (const CHAR *lpLibFileName);
    __declspec(dllimport) BOOL    __stdcall FreeLibrary       (HMODULE hLibModule);
    __declspec(dllimport) void *  __stdcall GetProcAddress    (HMODULE hModule, const CHAR *lpProcName);
    __declspec(dllimport) HMODULE __stdcall GetModuleHandleA  (const CHAR *lpModuleName);
    __declspec(dllimport) DWORD   __stdcall GetModuleFileNameW(HMODULE hModule, WCHAR *lpFilename, DWORD nSize);
    }

    // NOTE: um/synchapi.h /////////////////////////////////////////////////////////////////////////
    extern "C"
    {
    __declspec(dllimport) VOID  __stdcall InitializeCriticalSection            (CRITICAL_SECTION *lpCriticalSection);
    __declspec(dllimport) VOID  __stdcall EnterCriticalSection                 (CRITICAL_SECTION *lpCriticalSection);
    __declspec(dllimport) VOID  __stdcall LeaveCriticalSection                 (CRITICAL_SECTION *lpCriticalSection);
    __declspec(dllimport) BOOL  __stdcall InitializeCriticalSectionAndSpinCount(CRITICAL_SECTION *lpCriticalSection, DWORD dwSpinCount);
    __declspec(dllimport) BOOL  __stdcall InitializeCriticalSectionEx          (CRITICAL_SECTION *lpCriticalSection, DWORD dwSpinCount, DWORD Flags);
    __declspec(dllimport) DWORD __stdcall SetCriticalSectionSpinCount          (CRITICAL_SECTION *lpCriticalSection, DWORD dwSpinCount);
    __declspec(dllimport) BOOL  __stdcall TryEnterCriticalSection              (CRITICAL_SECTION *lpCriticalSection);
    __declspec(dllimport) VOID  __stdcall DeleteCriticalSection                (CRITICAL_SECTION *lpCriticalSection);
    __declspec(dllimport) DWORD __stdcall WaitForSingleObject                  (HANDLE hHandle, DWORD dwMilliseconds);
    __declspec(dllimport) BOOL  __stdcall ReleaseSemaphore                     (HANDLE hSemaphore, LONG lReleaseCount, LONG *lpPreviousCount);
    __declspec(dllimport) VOID  __stdcall Sleep                                (DWORD dwMilliseconds);
    }

    // NOTE: um/profileapi.h ///////////////////////////////////////////////////////////////////////
    extern "C"
    {
    __declspec(dllimport) BOOL __stdcall QueryPerformanceCounter  (LARGE_INTEGER* lpPerformanceCount);
    __declspec(dllimport) BOOL __stdcall QueryPerformanceFrequency(LARGE_INTEGER* lpFrequency);
    }

    // NOTE: um/processthreadsapi.h ////////////////////////////////////////////////////////////////
    typedef struct _PROCESS_INFORMATION {
        HANDLE hProcess;
        HANDLE hThread;
        DWORD dwProcessId;
        DWORD dwThreadId;
    } PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

    typedef struct _STARTUPINFOW {
        DWORD   cb;
        WCHAR  *lpReserved;
        WCHAR  *lpDesktop;
        WCHAR  *lpTitle;
        DWORD   dwX;
        DWORD   dwY;
        DWORD   dwXSize;
        DWORD   dwYSize;
        DWORD   dwXCountChars;
        DWORD   dwYCountChars;
        DWORD   dwFillAttribute;
        DWORD   dwFlags;
        WORD    wShowWindow;
        WORD    cbReserved2;
        BYTE   *lpReserved2;
        HANDLE  hStdInput;
        HANDLE  hStdOutput;
        HANDLE  hStdError;
    } STARTUPINFOW, *LPSTARTUPINFOW;

    typedef DWORD (__stdcall *PTHREAD_START_ROUTINE)(
        VOID *lpThreadParameter
        );
    typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

    extern "C"
    {
    __declspec(dllimport) BOOL   __stdcall CreateProcessW(WCHAR const *lpApplicationName, WCHAR *lpCommandLine, SECURITY_ATTRIBUTES *lpProcessAttributes, SECURITY_ATTRIBUTES *lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, VOID *lpEnvironment, WCHAR const *lpCurrentDirectory, STARTUPINFOW *lpStartupInfo, PROCESS_INFORMATION *lpProcessInformation);
    __declspec(dllimport) HANDLE __stdcall CreateThread(SECURITY_ATTRIBUTES *lpThreadAttributes, SIZE_T dwStackSize, PTHREAD_START_ROUTINE lpStartAddress, VOID *lpParameter, DWORD dwCreationFlags, DWORD *lpThreadId);
    __declspec(dllimport) DWORD  __stdcall GetCurrentThreadId(VOID);
    __declspec(dllimport) BOOL   __stdcall GetExitCodeProcess(HANDLE hProcess, DWORD *lpExitCode);
    __declspec(dllimport) void   __stdcall ExitProcess(UINT uExitCode);

    }

    // NOTE: um/memoryapi.h ////////////////////////////////////////////////////////////////////////
    extern "C"
    {
    __declspec(dllimport) VOID * __stdcall VirtualAlloc  (VOID *lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
    __declspec(dllimport) BOOL   __stdcall VirtualProtect(VOID *lpAddress, SIZE_T dwSize, DWORD flNewProtect, DWORD *lpflOldProtect);
    __declspec(dllimport) BOOL   __stdcall VirtualFree   (VOID *lpAddress, SIZE_T dwSize, DWORD dwFreeType);
    }

    // NOTE: shared/bcrypt.h ///////////////////////////////////////////////////////////////////////
    typedef VOID *BCRYPT_ALG_HANDLE;
    typedef LONG  NTSTATUS;

    extern "C"
    {
    __declspec(dllimport) NTSTATUS __stdcall BCryptOpenAlgorithmProvider(BCRYPT_ALG_HANDLE *phAlgorithm, const WCHAR *pszAlgId, const WCHAR *pszImplementation, ULONG dwFlags);
    __declspec(dllimport) NTSTATUS __stdcall BCryptGenRandom            (BCRYPT_ALG_HANDLE hAlgorithm, UCHAR *pbBuffer, ULONG cbBuffer, ULONG dwFlags);
    }

    // NOTE: um/shellapi.h /////////////////////////////////////////////////////////////////////////
    extern "C"
    {
    __declspec(dllimport) HINSTANCE __stdcall ShellExecuteA(HWND hwnd, CHAR const *lpOperation, CHAR const *lpFile, CHAR const *lpParameters, CHAR const *lpDirectory, INT nShowCmd);
    }

    // NOTE: um/debugapi.h /////////////////////////////////////////////////////////////////////////
    extern "C"
    {
    __declspec(dllimport) BOOL      __stdcall IsDebuggerPresent();
    }

    DQN_MSVC_WARNING_POP
#endif // !defined(_INC_WINDOWS)
