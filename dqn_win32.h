#if defined(DQN_OS_WIN32)
#pragma comment(lib, "bcrypt")
#pragma comment(lib, "wininet")
#pragma comment(lib, "dbghelp")

#if defined(DQN_NO_WIN32_MIN_HEADER)
    #include <bcrypt.h>   // Dqn_OS_SecureRNGBytes -> BCryptOpenAlgorithmProvider ... etc
    #include <shellapi.h> // Dqn_Win_MakeProcessDPIAware -> SetProcessDpiAwareProc
    #include <DbgHelp.h>
    #if !defined(DQN_NO_WINNET)
        DQN_MSVC_WARNING_PUSH
        DQN_MSVC_WARNING_DISABLE(6553) // wininet.h|940 warning| The annotation for function 'InternetConnectA' on _Param_(8) does not apply to a value type.
        #include <wininet.h> // Dqn_Win_Net -> InternetConnect ... etc
        DQN_MSVC_WARNING_POP
    #endif // DQN_NO_WINNET
#elif !defined(_INC_WINDOWS)
    DQN_MSVC_WARNING_PUSH
    DQN_MSVC_WARNING_DISABLE(4201) // warning C4201: nonstandard extension used: nameless struct/union

    // NOTE: basetsd.h =============================================================================
    typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;
    typedef ULONG_PTR        SIZE_T,    *PSIZE_T;
    typedef __int64          LONG_PTR,  *PLONG_PTR;
    typedef ULONG_PTR        DWORD_PTR, *PDWORD_PTR;
    typedef unsigned __int64 ULONG64,   *PULONG64;
    typedef unsigned __int64 DWORD64,   *PDWORD64;

    // NOTE: shared/minwindef.h ====================================================================
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

    #define MAX_PATH 260

    typedef struct _FILETIME {
        DWORD dwLowDateTime;
        DWORD dwHighDateTime;
    } FILETIME, *PFILETIME, *LPFILETIME;

    // NOTE: shared/winerror.h =====================================================================
    // NOTE: GetModuleFileNameW
    #define ERROR_INSUFFICIENT_BUFFER 122L // dderror

    // NOTE: um/winnls.h ===========================================================================
    // NOTE: MultiByteToWideChar
    #define CP_UTF8 65001 // UTF-8 translation

    // NOTE: um/winnt.h ============================================================================
    typedef void             VOID;
    typedef __int64          LONGLONG;
    typedef unsigned __int64 ULONGLONG;
    typedef void *           HANDLE;
    typedef char             CHAR;
    typedef short            SHORT;
    typedef long             LONG;
    typedef wchar_t          WCHAR; // wc, 16-bit UNICODE character
    typedef CHAR *           NPSTR, *LPSTR, *PSTR;

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
    #define FILE_ATTRIBUTE_READONLY  0x00000001
    #define FILE_ATTRIBUTE_HIDDEN    0x00000002
    #define FILE_ATTRIBUTE_SYSTEM    0x00000004
    #define FILE_ATTRIBUTE_DIRECTORY 0x00000010
    #define FILE_ATTRIBUTE_NORMAL    0x00000080

    // NOTE: STACKFRAME64
    #define IMAGE_FILE_MACHINE_AMD64 0x8664  // AMD64 (K8)

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

    extern "C"
    {
    __declspec(dllimport) VOID   __stdcall RtlCaptureContext(CONTEXT *ContextRecord);
    __declspec(dllimport) HANDLE __stdcall GetCurrentProcess(void);
    __declspec(dllimport) HANDLE __stdcall GetCurrentThread(void);
    __declspec(dllimport) DWORD  __stdcall SymSetOptions(DWORD SymOptions);
    __declspec(dllimport) BOOL   __stdcall SymInitialize(HANDLE hProcess, const CHAR* UserSearchPath, BOOL fInvadeProcess);
    }

    // NOTE: handleapi.h ===========================================================================
    #define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

    extern "C"
    {
    __declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE hObject);
    }

    // NOTE: consoleapi.h ===========================================================================
    extern "C"
    {
    __declspec(dllimport) BOOL __stdcall WriteConsoleA(HANDLE hConsoleOutput, const VOID* lpBuffer, DWORD nNumberOfCharsToWrite, DWORD *lpNumberOfCharsWritten, VOID *lpReserved);
    __declspec(dllimport) BOOL __stdcall AllocConsole(VOID);
    __declspec(dllimport) BOOL __stdcall FreeConsole(VOID);
    __declspec(dllimport) BOOL __stdcall AttachConsole(DWORD dwProcessId);
    __declspec(dllimport) BOOL __stdcall GetConsoleMode(HANDLE hConsoleHandle, DWORD *lpMode);
    }

    // NOTE: um/minwinbase.h =======================================================================
    // NOTE: FindFirstFile
    #define FIND_FIRST_EX_CASE_SENSITIVE 0x00000001
    #define FIND_FIRST_EX_LARGE_FETCH    0x00000002

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

    // NOTE: um/winbase.h ==========================================================================
    #define INFINITE 0xFFFFFFFF // Wait/Synchronisation: Infinite timeout

    #define STD_INPUT_HANDLE  ((DWORD)-10)
    #define STD_OUTPUT_HANDLE ((DWORD)-11)
    #define STD_ERROR_HANDLE  ((DWORD)-12)

    // NOTE: MoveFile
    #define MOVEFILE_REPLACE_EXISTING 0x00000001
    #define MOVEFILE_COPY_ALLOWED     0x00000002

    // NOTE: FormatMessageA
    #define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200
    #define FORMAT_MESSAGE_FROM_HMODULE   0x00000800
    #define FORMAT_MESSAGE_FROM_SYSTEM    0x00001000

    extern "C"
    {
    __declspec(dllimport) BOOL   __stdcall MoveFileExW     (const WCHAR *lpExistingFileName, const WCHAR *lpNewFileName, DWORD dwFlags);
    __declspec(dllimport) BOOL   __stdcall CopyFileW       (const WCHAR *lpExistingFileName, const WCHAR *lpNewFileName, BOOL bFailIfExists);
    __declspec(dllimport) HANDLE __stdcall CreateSemaphoreA(SECURITY_ATTRIBUTES *lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, const CHAR *lpName);
    __declspec(dllimport) DWORD  __stdcall FormatMessageA  (DWORD dwFlags, const VOID *lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPSTR lpBuffer, DWORD nSize, va_list *Arguments);
    }

    // NOTE: um/stringapiset.h =====================================================================
    extern "C"
    {
    __declspec(dllimport) int __stdcall MultiByteToWideChar(UINT CodePage, DWORD dwFlags, const CHAR *lpMultiByteStr, int cbMultiByte, WCHAR *lpWideCharStr, int cchWideChar);
    __declspec(dllimport) int __stdcall WideCharToMultiByte(UINT CodePage, DWORD dwFlags, const WCHAR *lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, const CHAR *lpDefaultChar, BOOL *lpUsedDefaultChar);
    }

    // NOTE: um/fileapi.h ==========================================================================
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

    // NOTE: um/processenv.h =======================================================================
    extern "C"
    {
    __declspec(dllimport) DWORD  __stdcall GetCurrentDirectoryW(DWORD nBufferLength, WCHAR *lpBuffer);
    __declspec(dllimport) HANDLE __stdcall GetStdHandle(DWORD nStdHandle);
    }

    // NOTE: um/sysinfoapi.h =======================================================================
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

    // NOTE: shared/windef.h =======================================================================
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

    // NOTE: um/winuser.h ==========================================================================
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
    }

    // NOTE: um/wininet.h ==========================================================================
    typedef WORD INTERNET_PORT;
    typedef VOID *HINTERNET;

    #define INTERNET_OPEN_TYPE_PRECONFIG             0   // use registry configuration
    #define INTERNET_INVALID_PORT_NUMBER             0   // use the protocol-specific default
    #define INTERNET_DEFAULT_FTP_PORT                21  // default for FTP servers
    #define INTERNET_DEFAULT_HTTP_PORT               80  //    "     "  HTTP   "
    #define INTERNET_DEFAULT_HTTPS_PORT              443 //    "     "  HTTPS  "
    #define INTERNET_SERVICE_HTTP                    3

    #define INTERNET_OPTION_USERNAME                 28
    #define INTERNET_OPTION_PASSWORD                 29
    #define INTERNET_OPTION_USER_AGENT               41

    #define INTERNET_FLAG_NO_AUTH                    0x00040000 // no automatic authentication handling
    #define INTERNET_FLAG_SECURE                     0x00800000 // use PCT/SSL if applicable (HTTP)

    #define HTTP_QUERY_RAW_HEADERS                   21  // special: all headers as ASCIIZ
    #define HTTP_QUERY_RAW_HEADERS_CRLF              22  // special: all headers

    #define HTTP_ADDREQ_FLAG_ADD_IF_NEW              0x10000000
    #define HTTP_ADDREQ_FLAG_ADD                     0x20000000
    #define HTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA     0x40000000
    #define HTTP_ADDREQ_FLAG_COALESCE_WITH_SEMICOLON 0x01000000
    #define HTTP_ADDREQ_FLAG_COALESCE                HTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA
    #define HTTP_ADDREQ_FLAG_REPLACE                 0x80000000

    typedef enum {
        INTERNET_SCHEME_PARTIAL = -2,
        INTERNET_SCHEME_UNKNOWN = -1,
        INTERNET_SCHEME_DEFAULT = 0,
        INTERNET_SCHEME_FTP,
        INTERNET_SCHEME_GOPHER,
        INTERNET_SCHEME_HTTP,
        INTERNET_SCHEME_HTTPS,
        INTERNET_SCHEME_FILE,
        INTERNET_SCHEME_NEWS,
        INTERNET_SCHEME_MAILTO,
        INTERNET_SCHEME_SOCKS,
        INTERNET_SCHEME_JAVASCRIPT,
        INTERNET_SCHEME_VBSCRIPT,
        INTERNET_SCHEME_RES,
        INTERNET_SCHEME_FIRST = INTERNET_SCHEME_FTP,
        INTERNET_SCHEME_LAST = INTERNET_SCHEME_RES
    } INTERNET_SCHEME, * LPINTERNET_SCHEME;

    typedef struct {
        DWORD   dwStructSize;       // size of this structure. Used in version check
        LPSTR   lpszScheme;         // pointer to scheme name
        DWORD   dwSchemeLength;     // length of scheme name
        INTERNET_SCHEME nScheme;    // enumerated scheme type (if known)
        LPSTR   lpszHostName;       // pointer to host name
        DWORD   dwHostNameLength;   // length of host name
        INTERNET_PORT nPort;        // converted port number
        LPSTR   lpszUserName;       // pointer to user name
        DWORD   dwUserNameLength;   // length of user name
        LPSTR   lpszPassword;       // pointer to password
        DWORD   dwPasswordLength;   // length of password
        LPSTR   lpszUrlPath;        // pointer to URL-path
        DWORD   dwUrlPathLength;    // length of URL-path
        LPSTR   lpszExtraInfo;      // pointer to extra information (e.g. ?foo or #foo)
        DWORD   dwExtraInfoLength;  // length of extra information
    } URL_COMPONENTSA, * LPURL_COMPONENTSA;

    extern "C"
    {
    __declspec(dllimport) BOOL      __stdcall InternetCrackUrlA     (CHAR const *lpszUrl, DWORD dwUrlLength, DWORD dwFlags, URL_COMPONENTSA *lpUrlComponents);
    __declspec(dllimport) HINTERNET __stdcall InternetOpenA         (CHAR const *lpszAgent, DWORD dwAccessType, CHAR const *lpszProxy, CHAR const *lpszProxyBypass, DWORD dwFlags);
    __declspec(dllimport) HINTERNET __stdcall InternetConnectA      (HINTERNET hInternet, CHAR const *lpszServerName, INTERNET_PORT nServerPort, CHAR const *lpszUserName, CHAR const *lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
    __declspec(dllimport) BOOL      __stdcall InternetSetOptionA    (HINTERNET hInternet, DWORD dwOption, VOID *lpBuffer, DWORD dwBufferLength);
    __declspec(dllimport) BOOL      __stdcall InternetReadFile      (HINTERNET hFile, VOID *lpBuffer, DWORD dwNumberOfBytesToRead, DWORD *lpdwNumberOfBytesRead);
    __declspec(dllimport) BOOL      __stdcall InternetCloseHandle   (HINTERNET hInternet);
    __declspec(dllimport) HINTERNET __stdcall HttpOpenRequestA      (HINTERNET hConnect, CHAR const *lpszVerb, CHAR const *lpszObjectName, CHAR const *lpszVersion, CHAR const *lpszReferrer, CHAR const *lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);
    __declspec(dllimport) BOOL      __stdcall HttpSendRequestA      (HINTERNET hRequest, CHAR const *lpszHeaders, DWORD dwHeadersLength, VOID *lpOptional, DWORD dwOptionalLength);
    __declspec(dllimport) BOOL      __stdcall HttpAddRequestHeadersA(HINTERNET hRequest, CHAR const *lpszHeaders, DWORD dwHeadersLength, DWORD dwModifiers);
    __declspec(dllimport) BOOL      __stdcall HttpQueryInfoA        (HINTERNET hRequest, DWORD dwInfoLevel, VOID *lpBuffer, DWORD *lpdwBufferLength, DWORD *lpdwIndex);
    }

    // NOTE: um/DbgHelp.h ==========================================================================
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
    };

    // NOTE: um/errhandlingapi.h ===================================================================
    extern "C"
    {
    __declspec(dllimport) DWORD __stdcall GetLastError(VOID);
    }

    // NOTE: um/libloaderapi.h =====================================================================
    extern "C"
    {
    __declspec(dllimport) HMODULE __stdcall LoadLibraryA      (const CHAR *lpLibFileName);
    __declspec(dllimport) BOOL    __stdcall FreeLibrary       (HMODULE hLibModule);
    __declspec(dllimport) void *  __stdcall GetProcAddress    (HMODULE hModule, const CHAR *lpProcName);
    __declspec(dllimport) HMODULE __stdcall GetModuleHandleA  (const CHAR *lpModuleName);
    __declspec(dllimport) DWORD   __stdcall GetModuleFileNameW(HMODULE hModule, WCHAR *lpFilename, DWORD nSize);
    }

    // NOTE: um/synchapi.h =========================================================================
    extern "C"
    {
    __declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
    __declspec(dllimport) BOOL  __stdcall ReleaseSemaphore   (HANDLE hSemaphore, LONG lReleaseCount, LONG *lpPreviousCount);
    __declspec(dllimport) VOID  __stdcall Sleep              (DWORD dwMilliseconds);
    }

    // NOTE: um/profileapi.h =======================================================================
    extern "C"
    {
    __declspec(dllimport) BOOL __stdcall QueryPerformanceCounter  (LARGE_INTEGER* lpPerformanceCount);
    __declspec(dllimport) BOOL __stdcall QueryPerformanceFrequency(LARGE_INTEGER* lpFrequency);
    }

    // NOTE: um/processthreadsapi.h ================================================================
    typedef DWORD (__stdcall *PTHREAD_START_ROUTINE)(
        VOID *lpThreadParameter
        );
    typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

    extern "C"
    {
    __declspec(dllimport) HANDLE __stdcall CreateThread(SECURITY_ATTRIBUTES *lpThreadAttributes, SIZE_T dwStackSize, PTHREAD_START_ROUTINE lpStartAddress, VOID *lpParameter, DWORD dwCreationFlags, DWORD *lpThreadId);
    __declspec(dllimport) DWORD  __stdcall GetCurrentThreadId(VOID);
    }

    // NOTE: um/memoryapi.h ========================================================================
    extern "C"
    {
    __declspec(dllimport) VOID * __stdcall VirtualAlloc  (VOID *lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
    __declspec(dllimport) BOOL   __stdcall VirtualProtect(VOID *lpAddress, SIZE_T dwSize, DWORD flNewProtect, DWORD *lpflOldProtect);
    __declspec(dllimport) BOOL   __stdcall VirtualFree   (VOID *lpAddress, SIZE_T dwSize, DWORD dwFreeType);
    }

    // NOTE: shared/bcrypt.h =======================================================================
    typedef VOID *BCRYPT_ALG_HANDLE;
    typedef LONG  NTSTATUS;

    extern "C"
    {
    __declspec(dllimport) NTSTATUS __stdcall BCryptOpenAlgorithmProvider(BCRYPT_ALG_HANDLE *phAlgorithm, const WCHAR *pszAlgId, const WCHAR *pszImplementation, ULONG dwFlags);
    __declspec(dllimport) NTSTATUS __stdcall BCryptGenRandom            (BCRYPT_ALG_HANDLE hAlgorithm, UCHAR *pbBuffer, ULONG cbBuffer, ULONG dwFlags);
    }

    // NOTE: um/shellapi.h =========================================================================
    extern "C"
    {
    __declspec(dllimport) HINSTANCE __stdcall ShellExecuteA(HWND hwnd, CHAR const *lpOperation, CHAR const *lpFile, CHAR const *lpParameters, CHAR const *lpDirectory, INT nShowCmd);
    }

    DQN_MSVC_WARNING_POP
#endif // !defined(_INC_WINDOWS)
#endif /// defined(DQN_OS_WIN32)
