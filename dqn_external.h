// NOTE: [$BSTK] b_stacktrace ======================================================================
#if defined(DQN_OS_WIN32)
    #define DQN_NO_WIN32_MIN_HEADER
    #define WIN32_MEAN_AND_LEAN
    #include <Windows.h>
    #include <shellscalingapi.h>
#endif

#define B_STACKTRACE_API static
#include "b_stacktrace.h"

// NOTE: [$OS_H] OS Headers ========================================================================
#if defined(DQN_OS_WIN32)
    #pragma comment(lib, "bcrypt")
    #pragma comment(lib, "wininet")

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
        #define MAX_PATH 260

        // NOTE: Wait/Synchronization
        #define INFINITE 0xFFFFFFFF // Infinite timeout

        // NOTE: FormatMessageA
        #define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000
        #define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200
        #define FORMAT_MESSAGE_FROM_HMODULE    0x00000800
        #define MAKELANGID(p, s) ((((unsigned short  )(s)) << 10) | (unsigned short  )(p))
        #define SUBLANG_DEFAULT 0x01    // user default
        #define LANG_NEUTRAL 0x00

        // NOTE: MultiByteToWideChar
        #define CP_UTF8 65001 // UTF-8 translation

        // NOTE: VirtualAlloc
        // NOTE: Allocation Type
        #define MEM_RESERVE 0x00002000
        #define MEM_COMMIT 0x00001000
        #define MEM_DECOMMIT 0x00004000
        #define MEM_RELEASE 0x00008000

        // NOTE: Protect
        #define PAGE_NOACCESS 0x01
        #define PAGE_READONLY 0x02
        #define PAGE_READWRITE 0x04
        #define PAGE_GUARD 0x100

        // NOTE: FindFirstFile
        #define INVALID_HANDLE_VALUE ((void *)(long *)-1)
        #define INVALID_FILE_ATTRIBUTES ((unsigned long)-1)
        #define FILE_ATTRIBUTE_NORMAL 0x00000080
        #define FIND_FIRST_EX_LARGE_FETCH 0x00000002
        #define FILE_ATTRIBUTE_DIRECTORY 0x00000010
        #define FILE_ATTRIBUTE_READONLY 0x00000001
        #define FILE_ATTRIBUTE_HIDDEN 0x00000002

        // NOTE: GetModuleFileNameW
        #define ERROR_INSUFFICIENT_BUFFER 122L

        // NOTE: MoveFile
        #define MOVEFILE_REPLACE_EXISTING 0x00000001
        #define MOVEFILE_COPY_ALLOWED 0x00000002

        // NOTE: Wininet
        typedef unsigned short INTERNET_PORT;
        #define INTERNET_OPEN_TYPE_PRECONFIG 0 // use registry configuration
        #define INTERNET_DEFAULT_HTTPS_PORT 443 // HTTPS
        #define INTERNET_SERVICE_HTTP 3
        #define INTERNET_OPTION_USER_AGENT 41
        #define INTERNET_FLAG_NO_AUTH 0x00040000  // no automatic authentication handling
        #define INTERNET_FLAG_SECURE 0x00800000  // use PCT/SSL if applicable (HTTP)

        // NOTE: CreateFile
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

        // NOTE: HttpAddRequestHeadersA
        #define HTTP_ADDREQ_FLAG_ADD_IF_NEW 0x10000000
        #define HTTP_ADDREQ_FLAG_ADD        0x20000000
        #define HTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA       0x40000000
        #define HTTP_ADDREQ_FLAG_COALESCE_WITH_SEMICOLON   0x01000000
        #define HTTP_ADDREQ_FLAG_COALESCE                  HTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA
        #define HTTP_ADDREQ_FLAG_REPLACE    0x80000000

        #define SW_MAXIMIZED 3
        #define SW_SHOW 5

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
#elif defined(DQN_OS_UNIX)
    #include <errno.h>        // errno
    #include <fcntl.h>        // O_RDONLY ... etc
    #include <linux/fs.h>     // FICLONE
    #include <sys/ioctl.h>    // ioctl
    #include <sys/types.h>    // pid_t
    #include <sys/random.h>   // getrandom
    #include <sys/stat.h>     // stat
    #include <sys/sendfile.h> // sendfile
    #include <sys/mman.h>     // mmap
    #include <time.h>         // clock_gettime, nanosleep
    #include <unistd.h>       // access, gettid
#endif

// NOTE: [$STBS] stb_sprintf =======================================================================
// stb_sprintf - v1.10 - public domain snprintf() implementation
// originally by Jeff Roberts / RAD Game Tools, 2015/10/20
// http://github.com/nothings/stb
//
// allowed types:  sc uidBboXx p AaGgEef n
// lengths      :  hh h ll j z t I64 I32 I
//
// Contributors:
//    Fabian "ryg" Giesen (reformatting)
//    github:aganm (attribute format)
//
// Contributors (bugfixes):
//    github:d26435
//    github:trex78
//    github:account-login
//    Jari Komppa (SI suffixes)
//    Rohit Nirmal
//    Marcin Wojdyr
//    Leonard Ritter
//    Stefano Zanotti
//    Adam Allison
//    Arvid Gerstmann
//    Markus Kolb
//
// LICENSE:
//
//   See end of file for license information.

#ifndef STB_SPRINTF_H_INCLUDE
#define STB_SPRINTF_H_INCLUDE

/*
Single file sprintf replacement.

Originally written by Jeff Roberts at RAD Game Tools - 2015/10/20.
Hereby placed in public domain.

This is a full sprintf replacement that supports everything that
the C runtime sprintfs support, including float/double, 64-bit integers,
hex floats, field parameters (%*.*d stuff), length reads backs, etc.

Why would you need this if sprintf already exists?  Well, first off,
it's *much* faster (see below). It's also much smaller than the CRT
versions code-space-wise. We've also added some simple improvements
that are super handy (commas in thousands, callbacks at buffer full,
for example). Finally, the format strings for MSVC and GCC differ
for 64-bit integers (among other small things), so this lets you use
the same format strings in cross platform code.

It uses the standard single file trick of being both the header file
and the source itself. If you just include it normally, you just get
the header file function definitions. To get the code, you include
it from a C or C++ file and define STB_SPRINTF_IMPLEMENTATION first.

It only uses va_args macros from the C runtime to do it's work. It
does cast doubles to S64s and shifts and divides U64s, which does
drag in CRT code on most platforms.

It compiles to roughly 8K with float support, and 4K without.
As a comparison, when using MSVC static libs, calling sprintf drags
in 16K.

API:
====
int stbsp_sprintf( char * buf, char const * fmt, ... )
int stbsp_snprintf( char * buf, int count, char const * fmt, ... )
  Convert an arg list into a buffer.  stbsp_snprintf always returns
  a zero-terminated string (unlike regular snprintf).

int stbsp_vsprintf( char * buf, char const * fmt, va_list va )
int stbsp_vsnprintf( char * buf, int count, char const * fmt, va_list va )
  Convert a va_list arg list into a buffer.  stbsp_vsnprintf always returns
  a zero-terminated string (unlike regular snprintf).

int stbsp_vsprintfcb( STBSP_SPRINTFCB * callback, void * user, char * buf, char const * fmt, va_list va )
    typedef char * STBSP_SPRINTFCB( char const * buf, void * user, int len );
  Convert into a buffer, calling back every STB_SPRINTF_MIN chars.
  Your callback can then copy the chars out, print them or whatever.
  This function is actually the workhorse for everything else.
  The buffer you pass in must hold at least STB_SPRINTF_MIN characters.
    // you return the next buffer to use or 0 to stop converting

void stbsp_set_separators( char comma, char period )
  Set the comma and period characters to use.

FLOATS/DOUBLES:
===============
This code uses a internal float->ascii conversion method that uses
doubles with error correction (double-doubles, for ~105 bits of
precision).  This conversion is round-trip perfect - that is, an atof
of the values output here will give you the bit-exact double back.

One difference is that our insignificant digits will be different than
with MSVC or GCC (but they don't match each other either).  We also
don't attempt to find the minimum length matching float (pre-MSVC15
doesn't either).

If you don't need float or doubles at all, define STB_SPRINTF_NOFLOAT
and you'll save 4K of code space.

64-BIT INTS:
============
This library also supports 64-bit integers and you can use MSVC style or
GCC style indicators (%I64d or %lld).  It supports the C99 specifiers
for size_t and ptr_diff_t (%jd %zd) as well.

EXTRAS:
=======
Like some GCCs, for integers and floats, you can use a ' (single quote)
specifier and commas will be inserted on the thousands: "%'d" on 12345
would print 12,345.

For integers and floats, you can use a "$" specifier and the number
will be converted to float and then divided to get kilo, mega, giga or
tera and then printed, so "%$d" 1000 is "1.0 k", "%$.2d" 2536000 is
"2.53 M", etc. For byte values, use two $:s, like "%$$d" to turn
2536000 to "2.42 Mi". If you prefer JEDEC suffixes to SI ones, use three
$:s: "%$$$d" -> "2.42 M". To remove the space between the number and the
suffix, add "_" specifier: "%_$d" -> "2.53M".

In addition to octal and hexadecimal conversions, you can print
integers in binary: "%b" for 256 would print 100.

PERFORMANCE vs MSVC 2008 32-/64-bit (GCC is even slower than MSVC):
===================================================================
"%d" across all 32-bit ints (4.8x/4.0x faster than 32-/64-bit MSVC)
"%24d" across all 32-bit ints (4.5x/4.2x faster)
"%x" across all 32-bit ints (4.5x/3.8x faster)
"%08x" across all 32-bit ints (4.3x/3.8x faster)
"%f" across e-10 to e+10 floats (7.3x/6.0x faster)
"%e" across e-10 to e+10 floats (8.1x/6.0x faster)
"%g" across e-10 to e+10 floats (10.0x/7.1x faster)
"%f" for values near e-300 (7.9x/6.5x faster)
"%f" for values near e+300 (10.0x/9.1x faster)
"%e" for values near e-300 (10.1x/7.0x faster)
"%e" for values near e+300 (9.2x/6.0x faster)
"%.320f" for values near e-300 (12.6x/11.2x faster)
"%a" for random values (8.6x/4.3x faster)
"%I64d" for 64-bits with 32-bit values (4.8x/3.4x faster)
"%I64d" for 64-bits > 32-bit values (4.9x/5.5x faster)
"%s%s%s" for 64 char strings (7.1x/7.3x faster)
"...512 char string..." ( 35.0x/32.5x faster!)
*/

#if defined(__clang__)
 #if defined(__has_feature) && defined(__has_attribute)
  #if __has_feature(address_sanitizer)
   #if __has_attribute(__no_sanitize__)
    #define STBSP__ASAN __attribute__((__no_sanitize__("address")))
   #elif __has_attribute(__no_sanitize_address__)
    #define STBSP__ASAN __attribute__((__no_sanitize_address__))
   #elif __has_attribute(__no_address_safety_analysis__)
    #define STBSP__ASAN __attribute__((__no_address_safety_analysis__))
   #endif
  #endif
 #endif
#elif defined(__GNUC__) && (__GNUC__ >= 5 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
 #if defined(__SANITIZE_ADDRESS__) && __SANITIZE_ADDRESS__
  #define STBSP__ASAN __attribute__((__no_sanitize_address__))
 #endif
#endif

#ifndef STBSP__ASAN
#define STBSP__ASAN
#endif

#ifdef STB_SPRINTF_STATIC
#define STBSP__PUBLICDEC static
#define STBSP__PUBLICDEF static STBSP__ASAN
#else
#ifdef __cplusplus
#define STBSP__PUBLICDEC extern "C"
#define STBSP__PUBLICDEF extern "C" STBSP__ASAN
#else
#define STBSP__PUBLICDEC extern
#define STBSP__PUBLICDEF STBSP__ASAN
#endif
#endif

#if defined(__has_attribute)
 #if __has_attribute(format)
   #define STBSP__ATTRIBUTE_FORMAT(fmt,va) __attribute__((format(printf,fmt,va)))
 #endif
#endif

#ifndef STBSP__ATTRIBUTE_FORMAT
#define STBSP__ATTRIBUTE_FORMAT(fmt,va)
#endif

#ifdef _MSC_VER
#define STBSP__NOTUSED(v)  (void)(v)
#else
#define STBSP__NOTUSED(v)  (void)sizeof(v)
#endif

#include <stdarg.h> // for va_arg(), va_list()
#include <stddef.h> // size_t, ptrdiff_t

#ifndef STB_SPRINTF_MIN
#define STB_SPRINTF_MIN 512 // how many characters per callback
#endif
typedef char *STBSP_SPRINTFCB(const char *buf, void *user, int len);

#ifndef STB_SPRINTF_DECORATE
#define STB_SPRINTF_DECORATE(name) stbsp_##name // define this before including if you want to change the names
#endif

STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(vsprintf)(char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(vsnprintf)(char *buf, int count, char const *fmt, va_list va);
STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(sprintf)(char *buf, char const *fmt, ...) STBSP__ATTRIBUTE_FORMAT(2,3);
STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...) STBSP__ATTRIBUTE_FORMAT(3,4);

STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEC void STB_SPRINTF_DECORATE(set_separators)(char comma, char period);
#endif // STB_SPRINTF_H_INCLUDE


