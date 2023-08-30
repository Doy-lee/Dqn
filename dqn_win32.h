#if defined(DQN_OS_WIN32)
#pragma comment(lib, "bcrypt")
#pragma comment(lib, "wininet")
#pragma comment(lib, "dbghelp")

#if defined(DQN_NO_WIN32_MIN_HEADER)
    #include <bcrypt.h>   // Dqn_OS_SecureRNGBytes -> BCryptOpenAlgorithmProvider ... etc
    #include <shellapi.h> // Dqn_Win_MakeProcessDPIAware -> SetProcessDpiAwareProc
    #if !defined(DQN_NO_WINNET)
        DQN_MSVC_WARNING_PUSH
        DQN_MSVC_WARNING_DISABLE(6553) // wininet.h|940 warning| The annotation for function 'InternetConnectA' on _Param_(8) does not apply to a value type.
        #include <wininet.h> // Dqn_Win_Net -> InternetConnect ... etc
        DQN_MSVC_WARNING_POP
    #endif // DQN_NO_WINNET
#elif !defined(_INC_WINDOWS)
    DQN_MSVC_WARNING_PUSH
    DQN_MSVC_WARNING_DISABLE(4201) // warning C4201: nonstandard extension used: nameless struct/union

    // NOTE: Windows typedefs ======================================================================
    typedef unsigned long    DWORD;
    typedef unsigned __int64 DWORD64;
    typedef int              BOOL;
    typedef char             CHAR;
    typedef void*            HANDLE;
    typedef unsigned long    ULONG;
    typedef unsigned __int64 ULONG64;
    typedef wchar_t          WCHAR;

    // NOTE: Windows Defines =======================================================================
    #define MAX_PATH 260

    // NOTE: Wait/Synchronization ==================================================================
    #define INFINITE 0xFFFFFFFF // Infinite timeout

    // NOTE: FormatMessageA ========================================================================
    #define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000
    #define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200
    #define FORMAT_MESSAGE_FROM_HMODULE    0x00000800
    #define MAKELANGID(p, s) ((((unsigned short  )(s)) << 10) | (unsigned short  )(p))
    #define SUBLANG_DEFAULT 0x01    // user default
    #define LANG_NEUTRAL 0x00

    // NOTE: MultiByteToWideChar
    #define CP_UTF8 65001 // UTF-8 translation

    // NOTE: VirtualAlloc ==========================================================================
    // NOTE: Allocation Type
    #define MEM_RESERVE  0x00002000
    #define MEM_COMMIT   0x00001000
    #define MEM_DECOMMIT 0x00004000
    #define MEM_RELEASE  0x00008000

    // NOTE: Protect
    #define PAGE_NOACCESS 0x01
    #define PAGE_READONLY 0x02
    #define PAGE_READWRITE 0x04
    #define PAGE_GUARD 0x100

    // NOTE: FindFirstFile =========================================================================
    #define INVALID_HANDLE_VALUE      ((void *)(long *)-1)
    #define INVALID_FILE_ATTRIBUTES   ((unsigned long)-1)
    #define FILE_ATTRIBUTE_NORMAL     0x00000080
    #define FIND_FIRST_EX_LARGE_FETCH 0x00000002
    #define FILE_ATTRIBUTE_DIRECTORY  0x00000010
    #define FILE_ATTRIBUTE_READONLY   0x00000001
    #define FILE_ATTRIBUTE_HIDDEN     0x00000002

    // NOTE: GetModuleFileNameW ====================================================================
    #define ERROR_INSUFFICIENT_BUFFER 122L

    // NOTE: MoveFile ==============================================================================
    #define MOVEFILE_REPLACE_EXISTING 0x00000001
    #define MOVEFILE_COPY_ALLOWED 0x00000002

    // NOTE: Wininet ===============================================================================
    typedef unsigned short INTERNET_PORT;
    #define INTERNET_OPEN_TYPE_PRECONFIG 0 // use registry configuration
    #define INTERNET_DEFAULT_HTTPS_PORT 443 // HTTPS
    #define INTERNET_SERVICE_HTTP 3
    #define INTERNET_OPTION_USER_AGENT 41
    #define INTERNET_FLAG_NO_AUTH 0x00040000  // no automatic authentication handling
    #define INTERNET_FLAG_SECURE 0x00800000  // use PCT/SSL if applicable (HTTP)

    // NOTE: CreateFile ============================================================================
    #define GENERIC_READ (0x80000000L)
    #define GENERIC_WRITE (0x40000000L)
    #define GENERIC_EXECUTE (0x20000000L)
    #define GENERIC_ALL (0x10000000L)
    #define FILE_ATTRIBUTE_NORMAL 0x00000080
    #define FILE_APPEND_DATA 4

    #define CREATE_NEW          1
    #define CREATE_ALWAYS       2
    #define OPEN_EXISTING       3
    #define OPEN_ALWAYS         4
    #define TRUNCATE_EXISTING   5

    #define STD_INPUT_HANDLE ((unsigned long)-10)
    #define STD_OUTPUT_HANDLE ((unsigned long)-11)
    #define STD_ERROR_HANDLE ((unsigned long)-12)

    #define INVALID_FILE_SIZE ((unsigned long)0xFFFFFFFF)

    #define HTTP_QUERY_RAW_HEADERS 21
    #define HTTP_QUERY_RAW_HEADERS_CRLF 22

    // NOTE: HttpAddRequestHeadersA ================================================================
    #define HTTP_ADDREQ_FLAG_ADD_IF_NEW 0x10000000
    #define HTTP_ADDREQ_FLAG_ADD        0x20000000
    #define HTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA       0x40000000
    #define HTTP_ADDREQ_FLAG_COALESCE_WITH_SEMICOLON   0x01000000
    #define HTTP_ADDREQ_FLAG_COALESCE                  HTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA
    #define HTTP_ADDREQ_FLAG_REPLACE    0x80000000

    #define SW_MAXIMIZED 3
    #define SW_SHOW 5

    // NOTE: Windows Structs =======================================================================
    typedef enum PROCESS_DPI_AWARENESS {
        PROCESS_DPI_UNAWARE           = 0,
        PROCESS_SYSTEM_DPI_AWARE      = 1,
        PROCESS_PER_MONITOR_DPI_AWARE = 2
    } PROCESS_DPI_AWARENESS;

    #define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((void *)-4)

    typedef union {
        struct {
            unsigned long LowPart;
            unsigned long HighPart;
        } DUMMYSTRUCTNAME;
        struct {
            unsigned long LowPart;
            unsigned long HighPart;
        } u;
        uint64_t QuadPart;
    } ULARGE_INTEGER;

    typedef struct
    {
        unsigned long dwLowDateTime;
        unsigned long dwHighDateTime;
    } FILETIME;

    typedef struct
    {
        unsigned long dwFileAttributes;
        FILETIME ftCreationTime;
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        unsigned long nFileSizeHigh;
        unsigned long nFileSizeLow;
    } WIN32_FILE_ATTRIBUTE_DATA;

    typedef enum
    {
        GetFileExInfoStandard,
        GetFileExMaxInfoLevel
    } GET_FILEEX_INFO_LEVELS;

    typedef struct {
        unsigned long nLength;
        void *lpSecurityDescriptor;
        bool bInheritHandle;
    } SECURITY_ATTRIBUTES;

    typedef struct {
      long left;
      long top;
      long right;
      long bottom;
    } RECT, *PRECT, *NPRECT, *LPRECT;

    typedef struct {
        union {
            unsigned long dwOemId;          // Obsolete field...do not use
            struct {
                uint16_t wProcessorArchitecture;
                uint16_t wReserved;
            } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME;
        unsigned long dwPageSize;
        void *lpMinimumApplicationAddress;
        void *lpMaximumApplicationAddress;
        unsigned long *dwActiveProcessorMask;
        unsigned long dwNumberOfProcessors;
        unsigned long dwProcessorType;
        unsigned long dwAllocationGranularity;
        uint16_t wProcessorLevel;
        uint16_t wProcessorRevision;
    } SYSTEM_INFO;

    typedef struct {
        unsigned short wYear;
        unsigned short wMonth;
        unsigned short wDayOfWeek;
        unsigned short wDay;
        unsigned short wHour;
        unsigned short wMinute;
        unsigned short wSecond;
        unsigned short wMilliseconds;
    } SYSTEMTIME;

    typedef struct {
        unsigned long dwFileAttributes;
        FILETIME ftCreationTime;
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        unsigned long nFileSizeHigh;
        unsigned long nFileSizeLow;
        unsigned long dwReserved0;
        unsigned long dwReserved1;
        wchar_t cFileName[MAX_PATH];
        wchar_t cAlternateFileName[14];
        #ifdef _MAC
        unsigned long dwFileType;
        unsigned long dwCreatorType;
        unsigned short wFinderFlags;
        #endif
    } WIN32_FIND_DATAW;

    typedef enum {
        FindExInfoStandard,
        FindExInfoBasic,
        FindExInfoMaxInfoLevel,
    } FINDEX_INFO_LEVELS;

    typedef enum {
        FindExSearchNameMatch,
        FindExSearchLimitToDirectories,
        FindExSearchLimitToDevices,
        FindExSearchMaxSearchOp
    } FINDEX_SEARCH_OPS;

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
    } INTERNET_SCHEME;

    typedef struct {
        unsigned long    dwStructSize;       // size of this structure. Used in version check
        char            *lpszScheme;         // pointer to scheme name
        unsigned long    dwSchemeLength;     // length of scheme name
        INTERNET_SCHEME  nScheme;            // enumerated scheme type (if known)
        char            *lpszHostName;       // pointer to host name
        unsigned long    dwHostNameLength;   // length of host name
        INTERNET_PORT    nPort;              // converted port number
        char            *lpszUserName;       // pointer to user name
        unsigned long    dwUserNameLength;   // length of user name
        char            *lpszPassword;       // pointer to password
        unsigned long    dwPasswordLength;   // length of password
        char            *lpszUrlPath;        // pointer to URL-path
        unsigned long    dwUrlPathLength;    // length of URL-path
        char            *lpszExtraInfo;      // pointer to extra information (e.g. ?foo or #foo)
        unsigned long    dwExtraInfoLength;  // length of extra information
    } URL_COMPONENTSA;

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

    // NOTE: minwinddef.h ==========================================================================
    typedef unsigned long  DWORD;
    typedef int            BOOL;
    typedef int            INT;
    typedef unsigned int   UINT;
    typedef unsigned short WORD;
    typedef unsigned char  BYTE;

    // NOTE: winnt.h Typedefs ======================================================================
    typedef void             VOID;
    typedef __int64          LONGLONG;
    typedef unsigned __int64 ULONGLONG;

    // NOTE: winnt.h Structs =======================================================================
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

    // NOTE: winnt.h Defines =======================================================================
    #define IMAGE_FILE_MACHINE_AMD64 0x8664  // AMD64 (K8)

    extern "C"
    {
    VOID   __declspec(dllimport) __stdcall RtlCaptureContext(CONTEXT *ContextRecord);
    HANDLE __declspec(dllimport) __stdcall GetCurrentProcess(void);
    HANDLE __declspec(dllimport) __stdcall GetCurrentThread(void);
    DWORD  __declspec(dllimport) __stdcall SymSetOptions(DWORD SymOptions);
    BOOL   __declspec(dllimport) __stdcall SymInitialize(HANDLE hProcess, const CHAR* UserSearchPath, BOOL fInvadeProcess);
    }

    // NOTE: DbgHelp.h =============================================================================

    // NOTE: DbgHelp.h Defines =====================================================================
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

    // NOTE: DbgHelp.h Structs =====================================================================
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

    // NOTE: DbgHelp.h Typedefs ====================================================================
    typedef BOOL   (__stdcall READ_PROCESS_MEMORY_ROUTINE64)(HANDLE hProcess, DWORD64 qwBaseAddress, VOID *lpBuffer, DWORD nSize, DWORD *lpNumberOfBytesRead);
    typedef VOID * (__stdcall FUNCTION_TABLE_ACCESS_ROUTINE64)(HANDLE ahProcess, DWORD64 AddrBase);
    typedef DWORD64(__stdcall GET_MODULE_BASE_ROUTINE64)(HANDLE hProcess, DWORD64 Address);
    typedef DWORD64(__stdcall TRANSLATE_ADDRESS_ROUTINE64)(HANDLE hProcess, HANDLE hThread, ADDRESS64 *lpaddr);

    // NOTE: DbgHelp.h Funtions ====================================================================
    extern "C"
    {
        __declspec(dllimport) BOOL    __stdcall StackWalk64(DWORD MachineType, HANDLE hProcess, HANDLE hThread, STACKFRAME64 *StackFrame, VOID *ContextRecord, READ_PROCESS_MEMORY_ROUTINE64 *ReadMemoryRoutine, FUNCTION_TABLE_ACCESS_ROUTINE64 *FunctionTableAccessRoutine, GET_MODULE_BASE_ROUTINE64 *GetModuleBaseRoutine, TRANSLATE_ADDRESS_ROUTINE64 *TranslateAddress);
        __declspec(dllimport) BOOL    __stdcall SymFromAddrW(HANDLE hProcess, DWORD64 Address, DWORD64 *Displacement, SYMBOL_INFOW *Symbol);
        __declspec(dllimport) VOID *  __stdcall SymFunctionTableAccess64(HANDLE hProcess, DWORD64 AddrBase);
        __declspec(dllimport) BOOL    __stdcall SymGetLineFromAddrW64(HANDLE hProcess, DWORD64 dwAddr, DWORD *pdwDisplacement, IMAGEHLP_LINEW64 *Line);
        __declspec(dllimport) DWORD64 __stdcall SymGetModuleBase64(HANDLE hProcess, DWORD64 qwAddr);
    };

    // NOTE: Windows.h ===============================================================================
    extern "C"
    {
    /*BOOL*/      int            __stdcall CreateDirectoryW           (wchar_t const *lpPathName, SECURITY_ATTRIBUTES *lpSecurityAttributes);
    /*BOOL*/      int            __stdcall RemoveDirectoryW           (wchar_t const *lpPathName);
    /*DWORD*/     unsigned long  __stdcall GetCurrentDirectoryW       (unsigned long nBufferLength, wchar_t *lpBuffer);

    /*BOOL*/      int            __stdcall FindNextFileW              (void *hFindFile, WIN32_FIND_DATAW *lpFindFileData);
    /*HANDLE*/    void *         __stdcall FindFirstFileExW           (wchar_t const *lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, void *lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, void *lpSearchFilter, unsigned long dwAdditionalFlags);
    /*DWORD*/     unsigned long  __stdcall GetFileAttributesExW       (wchar_t const *lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, WIN32_FILE_ATTRIBUTE_DATA *lpFileInformation);
    /*BOOL*/      int            __stdcall GetFileSizeEx              (void *hFile, LARGE_INTEGER *lpFileSize);

    /*BOOL*/      int            __stdcall MoveFileExW                (wchar_t const *lpExistingFileName, wchar_t const *lpNewFileName, unsigned long dwFlags);
    /*BOOL*/      int            __stdcall CopyFileW                  (wchar_t const *lpExistingFileName, wchar_t const *lpNewFileName, int bFailIfExists);
    /*BOOL*/      int            __stdcall DeleteFileW                (wchar_t const *lpExistingFileName);
    /*HANDLE*/    void *         __stdcall CreateFileW                (wchar_t const *lpFileName, unsigned long dwDesiredAccess, unsigned long dwShareMode, SECURITY_ATTRIBUTES *lpSecurityAttributes, unsigned long dwCreationDisposition, unsigned long dwFlagsAndAttributes, void *hTemplateFile);
    /*BOOL*/      int            __stdcall ReadFile                   (void *hFile, void *lpBuffer, unsigned long nNumberOfBytesToRead, unsigned long *lpNumberOfBytesRead, struct OVERLAPPED *lpOverlapped);
    /*BOOL*/      int            __stdcall WriteFile                  (void *hFile, void const *lpBuffer, unsigned long nNumberOfBytesToWrite, unsigned long *lpNumberOfBytesWritten, struct OVERLAPPED *lpOverlapped);
    /*BOOL*/      int            __stdcall CloseHandle                (void *hObject);

    /*BOOL*/      int            __stdcall WriteConsoleA              (void *hConsoleOutput, const char *lpBuffer, unsigned long nNumberOfCharsToWrite, unsigned long *lpNumberOfCharsWritten, void *lpReserved);
    /*BOOL*/      int            __stdcall AllocConsole               ();
    /*BOOL*/      int            __stdcall FreeConsole                ();
    /*BOOL*/      int            __stdcall AttachConsole              (unsigned long dwProcessId);
    /*HANDLE*/    void *         __stdcall GetStdHandle               (unsigned long nStdHandle);
    /*BOOL*/      int            __stdcall GetConsoleMode             (void *hConsoleHandle, unsigned long *lpMode);

    /*HMODULE*/   void *         __stdcall LoadLibraryA               (char const *lpFileName);
    /*BOOL*/      int            __stdcall FreeLibrary                (void *hModule);
    /*FARPROC*/   void *         __stdcall GetProcAddress             (void *hModule, char const *lpProcName);

    /*BOOL*/      int            __stdcall GetWindowRect              (void *hWnd, RECT *lpRect);
    /*BOOL*/      int            __stdcall SetWindowPos               (void *hWnd, void *hWndInsertAfter, int X, int Y, int cx, int cy, unsigned int uFlags);

    /*DWORD*/     unsigned long  __stdcall GetWindowModuleFileNameA   (void *hwnd, char *pszFileName, unsigned int cchFileNameMax);
    /*HMODULE*/   void *         __stdcall GetModuleHandleA           (char const *lpModuleName);
    /*DWORD*/     unsigned long  __stdcall GetModuleFileNameW         (void *hModule, wchar_t *lpFilename, unsigned long nSize);

    /*DWORD*/     unsigned long  __stdcall WaitForSingleObject        (void *hHandle, unsigned long dwMilliseconds);

    /*BOOL*/      int            __stdcall QueryPerformanceCounter    (LARGE_INTEGER *lpPerformanceCount);
    /*BOOL*/      int            __stdcall QueryPerformanceFrequency  (LARGE_INTEGER *lpFrequency);

    /*HANDLE*/    void *         __stdcall CreateThread               (SECURITY_ATTRIBUTES *lpThreadAttributes, size_t dwStackSize, unsigned long (*lpStartAddress)(void *), void *lpParameter, unsigned long dwCreationFlags, unsigned long *lpThreadId);
    /*HANDLE*/    void *         __stdcall CreateSemaphoreA           (SECURITY_ATTRIBUTES *lpSecurityAttributes, long lInitialCount, long lMaxCount, char *lpName);
    /*BOOL*/      int            __stdcall ReleaseSemaphore           (void *semaphore, long lReleaseCount, long *lpPreviousCount);
                  void           __stdcall Sleep                      (unsigned long dwMilliseconds);
    /*DWORD*/     unsigned long  __stdcall GetCurrentThreadId         ();

                  void *         __stdcall VirtualAlloc               (void *lpAddress, size_t dwSize, unsigned long flAllocationType, unsigned long flProtect);
    /*BOOL*/      int            __stdcall VirtualProtect             (void *lpAddress, size_t dwSize, unsigned long flNewProtect, unsigned long *lpflOldProtect);
    /*BOOL*/      int            __stdcall VirtualFree                (void *lpAddress, size_t dwSize, unsigned long dwFreeType);

                  void           __stdcall GetSystemInfo              (SYSTEM_INFO *system_info);
                  void           __stdcall GetSystemTime              (SYSTEMTIME  *lpSystemTime);
                  void           __stdcall GetSystemTimeAsFileTime    (FILETIME    *lpFileTime);
                  void           __stdcall GetLocalTime               (SYSTEMTIME  *lpSystemTime);

    /*DWORD*/     unsigned long  __stdcall FormatMessageA             (unsigned long dwFlags, void *lpSource, unsigned long dwMessageId, unsigned long dwLanguageId, char *lpBuffer, unsigned long nSize, va_list *Arguments);
    /*DWORD*/     unsigned long  __stdcall GetLastError               ();

                  int            __stdcall MultiByteToWideChar        (unsigned int CodePage, unsigned long dwFlags, char const *lpMultiByteStr, int cbMultiByte, wchar_t *lpWideCharStr, int cchWideChar);
                  int            __stdcall WideCharToMultiByte        (unsigned int CodePage, unsigned long dwFlags, wchar_t const *lpWideCharStr, int cchWideChar, char *lpMultiByteStr, int cbMultiByte, char const *lpDefaultChar, bool *lpUsedDefaultChar);

    /*NTSTATUS*/  long           __stdcall BCryptOpenAlgorithmProvider(void *phAlgorithm, wchar_t const *pszAlgId, wchar_t const *pszImplementation, unsigned long dwFlags);
    /*NTSTATUS*/  long           __stdcall BCryptGenRandom            (void *hAlgorithm, unsigned char *pbBuffer, unsigned long cbBuffer, unsigned long dwFlags);

    /*BOOLAPI*/   int            __stdcall InternetCrackUrlA          (char const *lpszUrl, unsigned long dwUrlLength, unsigned long dwFlags, URL_COMPONENTSA *lpUrlComponents);
    /*HANDLE*/    void *         __stdcall InternetOpenA              (char const *lpszAgent, unsigned long dwAccessType, char const *lpszProxy, char const *lpszProxyBypass, unsigned long dwFlags);
    /*HANDLE*/    void *         __stdcall InternetConnectA           (void *hInternet, char const *lpszServerName, INTERNET_PORT nServerPort, char const *lpszUserName, char const *lpszPassword, unsigned long dwService, unsigned long dwFlags, unsigned long *dwContext);
    /*BOOLAPI*/   int            __stdcall InternetSetOptionA         (void *hInternet, unsigned long dwOption, void *lpBuffer, unsigned long dwBufferLength);
    /*BOOLAPI*/   int            __stdcall InternetReadFile           (void *hFile, void *lpBuffer, unsigned long dwNumberOfBytesToRead, unsigned long *lpdwNumberOfBytesRead);
    /*BOOLAPI*/   int            __stdcall InternetCloseHandle        (void *hInternet);
    /*HANDLE*/    void *         __stdcall HttpOpenRequestA           (void *hConnect, char const *lpszVerb, char const *lpszObjectName, char const *lpszVersion, char const *lpszReferrer, char const **lplpszAcceptTypes, unsigned long dwFlags, unsigned long *dwContext);
    /*BOOLAPI*/   int            __stdcall HttpSendRequestA           (void *hRequest, char const *lpszHeaders, unsigned long dwHeadersLength, void *lpOptional, unsigned long dwOptionalLength);
    /*BOOLAPI*/   int            __stdcall HttpAddRequestHeadersA     (void *hRequest, char const *lpszHeaders, unsigned long dwHeadersLength, unsigned long dwModifiers);
    /*BOOL*/      int            __stdcall HttpQueryInfoA             (void *hRequest, unsigned long dwInfoLevel, void *lpBuffer, unsigned long *lpdwBufferLength, unsigned long *lpdwIndex);

    /*HINSTANCE*/ void *         __stdcall ShellExecuteA              (void *hwnd, char const *lpOperation, char const *lpFile, char const *lpParameters, char const *lpDirectory, int nShowCmd);
    /*BOOL*/      int            __stdcall ShowWindow                 (void *hWnd, int nCmdShow);
    }
    DQN_MSVC_WARNING_POP
#endif // !defined(_INC_WINDOWS)
#endif /// defined(DQN_OS_WIN32)
