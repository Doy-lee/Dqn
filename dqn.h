// Dqn.h Usage
// =================================================================================================
/*
    // Define this wherever you want access to DQN code that uses the platform.
    #define DQN_PLATFORM_HEADER         // Enable function prototypes for xplatform/platform code
    #define DQN_IMPLEMENTATION          // Enable the implementation
    #define DQN_PLATFORM_IMPLEMENTATION // Enable platform specific implementation on Win32 you must link against user32.lib and kernel32.lib
    #define DQN_MAKE_STATIC             // Make all functions be static
    #include "dqn.h"
 */

// Conventions
// All data structures fields are exposed by default, with exceptions here and there. The rationale
// is I rarely go into libraries and start changing values around in fields unless I know the
// implementation OR we're required to fill out a struct for some function.

// Just treat all struct fields to be internal and read-only unless explicitly stated otherwise.

// Table Of Contents #TOC #TableOfContents
// =================================================================================================
// You can search by #<entry> to jump straight to the section.
// The first match is the public API, the next matche(s) are the implementation

// #Portable Code
// #DqnSprintf     Cross-platform Sprintf Implementation (Public Domain lib stb_sprintf)
// #DqnAssert      Assertions and Logging
// #DqnSlice       Slices
// #DqnChar        Char  Operations (IsDigit(), IsAlpha() etc)
// #DqnStr         Str   Operations (Str_Len(), Str_Copy() etc)
// #DqnWChar       WChar Operations (IsDigit(), IsAlpha() etc)
// #DqnRnd         Random Number Generator (ints and floats)
// #Dqn_*          Utility code, (qsort, quick file reading)
// #DqnMem         Memory Allocation
// #DqnMemAPI      Custom memory API for Dqn Data Structures
// #DqnPool        Pool objects
// #DqnArray       Dynamic Array using Templates
// #DqnMemStack    Memory Allocator, Push, Pop Style
// #DqnHash        Hashing using Murmur
// #DqnMath        Simple Math Helpers (Lerp etc.)
// #DqnV2          2D  Math Vectors
// #DqnV3          3D  Math Vectors
// #DqnV4          4D  Math Vectors
// #DqnMat4        4x4 Math Matrix
// #DqnRect        Rectangles
// #DqnString      String library
// #DqnFixedString Fixed sized strings at compile time.
// #DqnLogger
// #DqnJson        Zero Allocation Json Parser

// #XPlatform (Win32 & Unix)
// #DqnVArray     Array backed by virtual memory
// #DqnVHashTable Hash Table using templates backed by virtual memory
// #DqnFile       File I/O (Read, Write, Delete)
// #DqnTimer      High Resolution Timer
// #DqnLock       Mutex Synchronisation
// #DqnJobQueue   Multithreaded Job Queue
// #DqnAtomic     Interlocks/Atomic Operations
// #DqnOS         Common Platform API helpers

// #Platform
//   - #DqnWin32     Common Win32 API Helpers

// TODO
// - DqnMemStack
//   - Allow 0 size memblock stack initialisation/block-less stack for situations where you don't
//     care about specifying a size upfront
//   - Default block size for when new blocks are required
//
// - Win32
//   - Get rid of reliance on MAX_PATH
//
// - Mbuildake lib compile and run on Linux with GCC using -03
// - Make DqnV* operations be static to class for consistency?

// Preprocessor Checks
// =================================================================================================
// This needs to be above the portable layer so  that, if the user requests a platform
// implementation, platform specific implementations in the portable layer will get activated.
#if (defined(_WIN32) || defined(_WIN64))
    #define DQN_IS_WIN32 1
#else
    #define DQN_IS_UNIX 1
#endif

#if defined(DQN_PLATFORM_IMPLEMENTATION)
    #define DQN__XPLATFORM_LAYER 1
#endif

// #Portable Code
// =================================================================================================
#ifndef DQN_H
#define DQN_H

#ifdef DQN_MAKE_STATIC
    #define DQN_FILE_SCOPE static
#else
    #define DQN_FILE_SCOPE
#endif

#include <stdint.h> // For standard types
#include <stddef.h> // For standard types
#include <string.h> // memmove
#include <stdarg.h> // va_list
#include <float.h>  // FLT_MAX
#define LOCAL_PERSIST static
#define FILE_SCOPE    static

using usize = size_t;
using isize = ptrdiff_t;

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8  = uint8_t;

using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8  = int8_t;

using b32 = i32;

using f64 = double;
using f32 = float;

#define DQN_F32_MIN   -FLT_MAX
#define DQN_I64_MAX  INT64_MAX
#define DQN_U64_MAX UINT64_MAX

#define DQN_TERABYTE(val) (DQN_GIGABYTE(val) * 1024LL)

#define DQN_TERABYTE(val) (DQN_GIGABYTE(val) * 1024LL)
#define DQN_GIGABYTE(val) (DQN_MEGABYTE(val) * 1024LL)
#define DQN_MEGABYTE(val) (DQN_KILOBYTE(val) * 1024LL)
#define DQN_KILOBYTE(val) ((val) * 1024LL)

#define DQN_DAY_TO_S(val) ((DQN_HOUR_TO_S(val)) * 24)
#define DQN_HOUR_TO_S(val) ((DQN_MINUTE_TO_S(val)) * 60)
#define DQN_MINUTE_TO_S(val) ((val) * 60)

#define DQN_ALIGN_POW_N(val, align) ((((usize)val) + ((usize)align-1)) & (~(usize)(align-1)))
#define DQN_ALIGN_POW_4(val)        DQN_ALIGN_POW_N(val, 4)

#define DQN_INVALID_CODE_PATH 0
#define DQN_CHAR_COUNT(charArray) DQN_ARRAY_COUNT(charArray) - 1
#define DQN_ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

#define DQN_PI 3.14159265359f
#define DQN_SQUARED(x) ((x) * (x))
#define DQN_ABS(x) (((x) < 0) ? (-(x)) : (x))
#define DQN_DEGREES_TO_RADIANS(x) ((x * (DQN_PI / 180.0f)))
#define DQN_RADIANS_TO_DEGREES(x) ((x * (180.0f / DQN_PI)))

#define DQN_CLAMP(value, min, max) DQN_MIN(DQN_MAX(value, min), max)
#define DQN_MAX(a, b) ((a) < (b) ? (b) : (a))
#define DQN_MIN(a, b) ((a) < (b) ? (a) : (b))
#define DQN_SWAP(type, a, b) do { type tmp = a; a = b; b = tmp; } while(0)

// NOTE: Directives don't get replaced if there's a stringify or paste (# or ##) so TOKEN_COMBINE2 is needed
//       to let directives get expanded (i.e. __COUNTER__), then we can combine.
#define DQN_TOKEN_COMBINE(x, y) x ## y
#define DQN_TOKEN_COMBINE2(x, y) DQN_TOKEN_COMBINE(x, y)

// Produce a unique name with prefix and counter. i.e. where prefix is "data" then it gives "data1"
#define DQN_UNIQUE_NAME(prefix) DQN_TOKEN_COMBINE2(prefix, __COUNTER__)
#define DQN_FOR_EACH(i, lim) for (isize (i) = 0; (i) < (isize)(lim); ++(i))

#define DQN_SIZEOF(decl) (isize)sizeof(decl)

template <typename Proc>
struct DqnDefer__
{
     DqnDefer__(Proc p) : proc(p) { }
    ~DqnDefer__() { proc(); }
    Proc proc;
};

template <typename Proc>
DqnDefer__<Proc> DqnDeferFunc__(Proc p)
{
    return DqnDefer__<Proc>(p);
}

#define DQN_DEFER(code) auto DQN_UNIQUE_NAME(dqnDeferLambda__) = DqnDeferFunc__([&]()->void {code;})

// #Dqn Namespace
namespace Dqn
{
enum struct ZeroClear   { No = 0, Yes = 1};
enum struct IgnoreCase  { No = 0, Yes = 1};
FILE_SCOPE const bool IS_DEBUG = true;
}; // namespace Dqn

// #External Code
// =================================================================================================
#if defined(DQN_PLATFORM_HEADER) && defined(DQN_IS_WIN32) && !defined(_WINDOWS_)
using WORD      = unsigned short;
using DWORD     = unsigned long;
using BOOL      = int;
using LONG      = long;
using LONGLONG  = long long;
using HANDLE    = void *;
using HMODULE   = HANDLE;
using HWND      = HANDLE;
using UINT      = unsigned int;
using ULONG     = unsigned long;
using ULONGLONG = unsigned long long;
using DWORD64   = unsigned long long;
using BYTE      = unsigned char;

u32    const MB_OK                         = 0x00000000L;
HANDLE const INVALID_HANDLE_VALUE          = ((HANDLE)(LONG *)-1);
u32    const MAX_PATH                      = 260;
u32    const INFINITE                      = 0xFFFFFFFF;
u32    const CP_UTF8                       = 65001;
u32    const FORMAT_MESSAGE_IGNORE_INSERTS = 0x00000200;
u32    const FORMAT_MESSAGE_FROM_SYSTEM    = 0x00001000;
u32    const MEM_COMMIT                    = 0x00001000;
u32    const MEM_RESERVE                   = 0x00002000;
u32    const PAGE_READWRITE                = 0x04;
u32    const MEM_DECOMMIT                  = 0x4000;
u32    const MEM_RELEASE                   = 0x8000;
u32    const GENERIC_READ                  = 0x80000000L;
u32    const GENERIC_WRITE                 = 0x40000000L;
u32    const GENERIC_EXECUTE               = 0x20000000L;
u32    const GENERIC_ALL                   = 0x10000000L;
u32    const CREATE_NEW                    = 1;
u32    const CREATE_ALWAYS                 = 2;
u32    const OPEN_EXISTING                 = 3;
u32    const OPEN_ALWAYS                   = 4;
u32    const TRUNCATE_EXISTING             = 5;
u32    const FILE_ATTRIBUTE_NORMAL         = 0x00000080;

struct RECT
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
};

union LARGE_INTEGER
{
    struct { DWORD LowPart; LONG HighPart; };
    struct { DWORD LowPart; LONG HighPart; } u;
    LONGLONG QuadPart;
};

union ULARGE_INTEGER
{
  struct { DWORD LowPart; DWORD HighPart; };
  struct { DWORD LowPart; DWORD HighPart; } u;
  ULONGLONG QuadPart;
};

struct SECURITY_ATTRIBUTES
{
    DWORD length;
    void *securityDescriptor;
    BOOL inheritHandle;
};

struct PROCESS_INFORMATION
{
    void *hProcess;
    void *hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
};


struct FILETIME
{
  DWORD dwLowDateTime;
  DWORD dwHighDateTime;
};

struct WIN32_FILE_ATTRIBUTE_DATA
{
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
};

enum GET_FILEEX_INFO_LEVELS
{
    GetFileExInfoStandard,
    GetFileExMaxInfoLevel
};

struct WIN32_FIND_DATAW
{
  DWORD    dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD    nFileSizeHigh;
  DWORD    nFileSizeLow;
  DWORD    dwReserved0;
  DWORD    dwReserved1;
  wchar_t  cFileName[MAX_PATH];
  wchar_t  cAlternateFileName[14];
};

struct LIST_ENTRY {
   struct LIST_ENTRY *Flink;
   struct LIST_ENTRY *Blink;
};

struct RTL_CRITICAL_SECTION_DEBUG
{
    WORD Type;
    WORD CreatorBackTraceIndex;
    struct CRITICAL_SECTION *CriticalSection;
    LIST_ENTRY ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
    DWORD Flags;
    WORD CreatorBackTraceIndexHigh;
    WORD SpareWORD;
};

struct CRITICAL_SECTION
{
    RTL_CRITICAL_SECTION_DEBUG *DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    ULONG *SpinCount;
};

struct OVERLAPPED {
  ULONG *Internal;
  ULONG *InternalHigh;
  union {
    struct {
      DWORD Offset;
      DWORD OffsetHigh;
    };
    void  *Pointer;
  };
  HANDLE    hEvent;
};

struct SYSTEM_INFO {
  union
  {
    DWORD  dwOemId;
    struct
    {
      WORD wProcessorArchitecture;
      WORD wReserved;
    };
  };
  DWORD  dwPageSize;
  void  *lpMinimumApplicationAddress;
  void  *lpMaximumApplicationAddress;
  DWORD *dwActiveProcessorMask;
  DWORD  dwNumberOfProcessors;
  DWORD  dwProcessorType;
  DWORD  dwAllocationGranularity;
  WORD   wProcessorLevel;
  WORD   wProcessorRevision;
};

enum LOGICAL_PROCESSOR_RELATIONSHIP
{
    RelationProcessorCore,
    RelationNumaNode,
    RelationCache,
    RelationProcessorPackage,
    RelationGroup,
    RelationAll = 0xffff
};

typedef unsigned long *KAFFINITY;
struct GROUP_AFFINITY {
    KAFFINITY Mask;
    WORD   Group;
    WORD   Reserved[3];
};

struct PROCESSOR_RELATIONSHIP
{
    BYTE  Flags;
    BYTE  EfficiencyClass;
    BYTE  Reserved[20];
    WORD  GroupCount;
    GROUP_AFFINITY GroupMask[1];
};

struct NUMA_NODE_RELATIONSHIP {
    DWORD NodeNumber;
    BYTE  Reserved[20];
    GROUP_AFFINITY GroupMask;
};

enum PROCESSOR_CACHE_TYPE
{
    CacheUnified,
    CacheInstruction,
    CacheData,
    CacheTrace
};

struct CACHE_RELATIONSHIP
{
    BYTE Level;
    BYTE Associativity;
    WORD LineSize;
    DWORD CacheSize;
    PROCESSOR_CACHE_TYPE Type;
    BYTE Reserved[20];
    GROUP_AFFINITY GroupMask;
};

struct PROCESSOR_GROUP_INFO
{
    BYTE MaximumProcessorCount;
    BYTE ActiveProcessorCount;
    BYTE Reserved[38];
    KAFFINITY ActiveProcessorMask;
};

struct GROUP_RELATIONSHIP
{
    WORD MaximumGroupCount;
    WORD ActiveGroupCount;
    BYTE Reserved[20];
    PROCESSOR_GROUP_INFO GroupInfo[1];
};

struct SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX
{
    LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
    DWORD Size;
    union
    {
        PROCESSOR_RELATIONSHIP Processor;
        NUMA_NODE_RELATIONSHIP NumaNode;
        CACHE_RELATIONSHIP Cache;
        GROUP_RELATIONSHIP Group;
    };
};


typedef DWORD (*LPTHREAD_START_ROUTINE)(void *lpThreadParameter);

DWORD64 __rdtsc();

void    DeleteCriticalSection           (CRITICAL_SECTION *lpCriticalSection);
BOOL    DeleteFileA                     (char    const *lpFileName); // TODO(doyle): Wide versions only
BOOL    DeleteFileW                     (wchar_t const *lpFileName);
BOOL    CloseHandle                     (HANDLE hObject);
BOOL    CopyFileA                       (char    const *lpExistingFileName, char const *lpNewFileName, BOOL bFailIfExists);
BOOL    CopyFileW                       (wchar_t const *lpExistingFileName, wchar_t const *lpNewFileName, BOOL bFailIfExists);
BOOL    CloseHandle                     (HANDLE *hObject);
HANDLE  CreateFileW                     (wchar_t const *lpFileName,
                                         DWORD dwDesiredAccess,
                                         DWORD dwShareMode,
                                         SECURITY_ATTRIBUTES *lpSecurityAttributes,
                                         DWORD dwCreationDisposition,
                                         DWORD dwFlagsAndAttributes,
                                         HANDLE hTemplateFile);
HANDLE  CreateSemaphoreA                (SECURITY_ATTRIBUTES *lpSemaphoreAttributes,
                                         long lInitialCount,
                                         long lMaximumCount,
                                         char const *lpName);
HANDLE  CreateThread                    (SECURITY_ATTRIBUTES *lpThreadAttributes,
                                         size_t dwStackSize,
                                         LPTHREAD_START_ROUTINE lpStartAddress,
                                         void *lpParameter,
                                         DWORD dwCreationFlags,
                                         DWORD *lpThreadId);
void    EnterCriticalSection            (CRITICAL_SECTION *lpCriticalSection);
BOOL    FindClose                       (HANDLE hFindFile);
HANDLE  FindFirstFileW                  (wchar_t const *lpFileName, WIN32_FIND_DATAW *lpFindFileData);
BOOL    FindNextFileW                   (HANDLE hFindFile, WIN32_FIND_DATAW *lpFindFileData);
DWORD   FormatMessageA                  (DWORD dwFlags,
                                         void const *lpSource,
                                         DWORD dwMessageId,
                                         DWORD dwLanguageId,
                                         char *lpBuffer,
                                         DWORD nSize,
                                         va_list *Arguments);
BOOL    GetClientRect                   (HWND hWnd, RECT *lpRect);
BOOL    GetExitCodeProcess              (HANDLE *hProcess, DWORD *lpExitCode);
BOOL    GetFileSizeEx                   (HANDLE hFile, LARGE_INTEGER *lpFileSize);
BOOL    GetFileAttributesExW            (wchar_t const *lpFileName,
                                         GET_FILEEX_INFO_LEVELS fInfoLevelId,
                                         void *lpFileInformation);
DWORD   GetLastError                    (void);
DWORD   GetModuleFileNameA              (HMODULE hModule, char *lpFilename, DWORD nSize);
void    GetNativeSystemInfo             (SYSTEM_INFO *lpSystemInfo);
BOOL    GetLogicalProcessorInformationEx(LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType,
                                         SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *Buffer,
                                         DWORD *ReturnedLength);
BOOL    InitializeCriticalSectionEx     (CRITICAL_SECTION *lpCriticalSection,
                                         DWORD dwSpinCount,
                                         DWORD Flags);
long    InterlockedAdd                  (long volatile *Addend, long Value);
long    InterlockedCompareExchange      (long volatile *Destination, long Exchange, long Comparand);
void    LeaveCriticalSection            (CRITICAL_SECTION *lpCriticalSection);
int     MessageBoxA                     (HWND hWnd, char const *lpText, char const *lpCaption, UINT uType);
int     MultiByteToWideChar             (unsigned int CodePage,
                                         DWORD dwFlags,
                                         char const *lpMultiByteStr,
                                         int cbMultiByte,
                                         wchar_t *lpWideCharStr,
                                         int cchWideChar);
void    OutputDebugStringA              (char const *lpOutputString);
BOOL    ReadFile                        (HANDLE hFile,
                                         void *lpBuffer,
                                         DWORD nNumberOfBytesToRead,
                                         DWORD *lpNumberOfBytesRead,
                                         OVERLAPPED *lpOverlapped);
BOOL    ReleaseSemaphore                (HANDLE hSemaphore, long lReleaseCount, long *lpPreviousCount);
BOOL    QueryPerformanceFrequency       (LARGE_INTEGER *lpFrequency);
BOOL    QueryPerformanceCounter         (LARGE_INTEGER *lpPerformanceCount);
DWORD   WaitForSingleObject             (HANDLE *hHandle, DWORD dwMilliseconds);
DWORD   WaitForSingleObjectEx           (HANDLE hHandle, DWORD dwMilliseconds, BOOL bAlertable);
int     WideCharToMultiByte             (unsigned int CodePage,
                                         DWORD dwFlags,
                                         wchar_t const *lpWideCharStr,
                                         int cchWideChar,
                                         char *lpMultiByteStr,
                                         int cbMultiByte,
                                         char const *lpDefaultChar,
                                         BOOL *lpUsedDefaultChar);
void    Sleep                           (DWORD dwMilliseconds);
BOOL    WriteFile                       (HANDLE hFile,
                                         void *const lpBuffer,
                                         DWORD nNumberOfBytesToWrite,
                                         DWORD *lpNumberOfBytesWritten,
                                         OVERLAPPED *lpOverlapped);
void   *VirtualAlloc                    (void *lpAddress,
                                         size_t dwSize,
                                         DWORD  flAllocationType,
                                         DWORD  flProtect);
BOOL    VirtualFree                     (void *lpAddress,
                                         size_t dwSize,
                                         DWORD  dwFreeType);
#endif // defined(DQN_PLATFORM_HEADER) && defined(DQN_IS_WIN32) && !defined(_WINDOWS_)
#ifndef STB_SPRINTF_H_INCLUDE
#define STB_SPRINTF_H_INCLUDE
#define STB_SPRINTF_DECORATE(name) Dqn_##name

////////////////////////////////////////////////////////////////////////////////
// #DqnSprintf Public API - Cross-platform Sprintf Implementation
////////////////////////////////////////////////////////////////////////////////
// stb_sprintf - v1.05 - public domain snprintf() implementation
// originally by Jeff Roberts / RAD Game Tools, 2015/10/20
// http://github.com/nothings/stb
//
// allowed types:  sc uidBboXx p AaGgEef n
// lengths      :  h ll j z t I64 I32 I
//
// Contributors:
//    Fabian "ryg" Giesen (reformatting)
//
// Contributors (bugfixes):
//    github:d26435
//    github:trex78
//    Jari Komppa (SI suffixes)
//    Rohit Nirmal
//    Marcin Wojdyr
//    Leonard Ritter
//
// LICENSE:
//
//   See end of file for license information.

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

#if defined(__has_feature)
   #if __has_feature(address_sanitizer)
      #define STBI__ASAN __attribute__((no_sanitize("address")))
   #endif
#endif
#ifndef STBI__ASAN
#define STBI__ASAN
#endif

#ifdef STB_SPRINTF_STATIC
#define STBSP__PUBLICDEC static
#define STBSP__PUBLICDEF static STBI__ASAN
#else
#ifdef __cplusplus
#define STBSP__PUBLICDEC extern "C"
#define STBSP__PUBLICDEF extern "C" STBI__ASAN
#else
#define STBSP__PUBLICDEC extern
#define STBSP__PUBLICDEF STBI__ASAN
#endif
#endif

#include <stdarg.h> // for va_list()

#ifndef STB_SPRINTF_MIN
#define STB_SPRINTF_MIN 512 // how many characters per callback
#endif
typedef char *STBSP_SPRINTFCB(char *buf, void *user, int len);

#ifndef STB_SPRINTF_DECORATE
#define STB_SPRINTF_DECORATE(name) stbsp_##name // define this before including if you want to change the names
#endif

// Convert a va_list arg list into a buffer. vsnprintf always returns a zero-terminated string (unlike regular snprintf).
// return: The number of characters copied into the buffer
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintf)(char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsnprintf)(char *buf, int count, char const *fmt, va_list va);

// Write format string to buffer. It always writes the whole string and appends a null.
// return: The number of characters copied into the buffer not including the null terminator.
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(sprintf)(char *buf, char const *fmt, ...);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...);

// snprintf() always returns a zero-terminated string (unlike regular snprintf).
// Convert into a buffer, calling back every STB_SPRINTF_MIN chars.
// Your callback can then copy the chars out, print them or whatever.
// This function is actually the workhorse for everything else.
// The buffer you pass in must hold at least STB_SPRINTF_MIN characters.
// You return the next buffer to use or 0 to stop converting
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va);

// Set the comma and period characters to use.
STBSP__PUBLICDEF void STB_SPRINTF_DECORATE(set_separators)(char comma, char period);
#endif // STB_SPRINTF_H_INCLUDE

// #DqnAssert API
// =================================================================================================
// NOTE: "## __VA_ARGS__" is a GCC hack. Zero variadic arguments won't compile
// because there will be a trailing ',' at the end. Pasting it swallows it. MSVC
// implicitly swallows the trailing comma.

// Always assert are enabled in release mode.
#define DQN_ALWAYS_ASSERT(expr) DQN_ASSERTM(expr, "");
#define DQN_ALWAYS_ASSERTM(expr, msg, ...) DQN_ASSERTM(expr, msg, ## __VA_ARGS__)

#define DQN_ASSERT(expr) DQN_ASSERTM(expr, "");
#define DQN_ASSERTM(expr, msg, ...)                                                                \
    do                                                                                             \
    {                                                                                              \
        if (!(expr))                                                                               \
        {                                                                                          \
            DqnLogExpr(__FILE__, __func__, __LINE__, #expr, msg, ## __VA_ARGS__);                  \
            (*((int *)0)) = 0;                                                                     \
        }                                                                                          \
    } while (0)

#define DQN_LOGE(msg, ...) DqnLog(__FILE__, __func__, __LINE__, msg, ## __VA_ARGS__);
#define DQN_LOGD(msg, ...) DqnLog(__FILE__, __func__, __LINE__, msg, ## __VA_ARGS__);

// Use macro above.
DQN_FILE_SCOPE void DqnLog    (char const *file, char const *function_name, i32 const line_num, char const *msg, ...);
DQN_FILE_SCOPE void DqnLogExpr(char const *file, char const *function_name, i32 const line_num, char const *expr, char const *msg, ...);

// Assert at compile time by making a type that is invalid depending on the expression result
#define DQN_COMPILE_ASSERT(expr)                DQN_COMPILE_ASSERT_INTERNAL(expr, DQN_UNIQUE_NAME(DqnCompileAssertInternal_))
#define DQN_COMPILE_ASSERT_INTERNAL(expr, name) typedef char name[((int)(expr)) * 2 - 1];

DQN_COMPILE_ASSERT(sizeof(isize) == sizeof(usize));

// #DqnSlice API
// =================================================================================================
template <typename T>
struct DqnSlice
{
    T   *data;
    i32  len;

    DqnSlice() = default;
    DqnSlice(T *str, i32 len) { this->data = str; this->len = len; }
};
#define DQN_SLICE(literal) DqnSlice<char const>(literal, DQN_CHAR_COUNT(literal))

#define DQN_SLICE_CMP(a, b, ignoreCase)  (a.len == b.len && (DqnStr_Cmp(a.data, b.data, a.len, ignoreCase) == 0))
// #DqnChar API
// =================================================================================================
DQN_FILE_SCOPE char DqnChar_ToLower     (char c);
DQN_FILE_SCOPE char DqnChar_ToUpper     (char c);
DQN_FILE_SCOPE bool DqnChar_IsDigit     (char c);
DQN_FILE_SCOPE bool DqnChar_IsAlpha     (char c);
DQN_FILE_SCOPE bool DqnChar_IsAlphaNum  (char c);
DQN_FILE_SCOPE bool DqnChar_IsEndOfLine (char c);
DQN_FILE_SCOPE bool DqnChar_IsWhitespace(char c);

DQN_FILE_SCOPE char *DqnChar_TrimWhitespaceAround(char const *src, i32 src_len, i32 *new_len);
DQN_FILE_SCOPE char *DqnChar_SkipWhitespace      (char const *ptr);

// TODO(doyle): this is NOT UTF8 safe
// ch:        Char to find
// len:       The length of the string stored in ptr, (doesn't care if it includes null terminator)
// len_to_char: The length to the char from end of the ptr, i.e. (ptr + len)
// return:    The ptr to the last char, null if it could not find.
DQN_FILE_SCOPE char *DqnChar_FindLastChar  (char *ptr, char ch, i32 len, u32 *len_to_char);

// Finds up to the first [\r]\n and destroy the line, giving you a null terminated line at the newline.
// returns: The value to advance the ptr by, this can be different from the line
//          length if there are new lines or leading whitespaces in the next line
DQN_FILE_SCOPE i32   DqnChar_FindNextLine(char *ptr, i32 *line_len);
DQN_FILE_SCOPE char *DqnChar_GetNextLine (char *ptr, i32 *line_len);

// #DqnStr API
// =================================================================================================
// num_bytes_to_cmp: If -1, cmp runs until \0 is encountered.
// return:            0 if equal. 0 < if a is before b, > 0 if a is after b
DQN_FILE_SCOPE        i32            DqnStr_Cmp                   (char const *a, char const *b, i32 num_bytes_to_cmp = -1, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No);

// str_len: Len of string, if -1, StrLen is used.
// return: Pointer in str to the last slash, if none then the original string.
DQN_FILE_SCOPE        char          *DqnStr_GetPtrToLastSlash     (char const *str, i32 str_len = -1);

// return: String length not including the nullptr terminator. 0 if invalid args.
DQN_FILE_SCOPE        i32            DqnStr_Len                   (char const *a);
DQN_FILE_SCOPE        i32            DqnStr_LenUTF8               (u32 const *a, i32 *len_in_bytes = nullptr);

// return: String length starting from a, up to and not including the first delimiter character.
DQN_FILE_SCOPE        i32            DqnStr_LenDelimitWith        (char const *a, char delimiter);

// return: The dest argument, nullptr if args invalid (i.e. nullptr pointers or numChars < 0)
DQN_FILE_SCOPE        void           DqnStr_Reverse               (char *buf, isize buf_size);

// return: Number of bytes in codepoint, 0 if *a becomes invalid or end of stream.
DQN_FILE_SCOPE        i32            DqnStr_ReadUTF8Codepoint     (u32 const *a, u32 *out_codepoint);

// return: The offset into the src to first char of the found string. Returns -1 if not found
DQN_FILE_SCOPE        i32            DqnStr_FindFirstOccurence    (char const *src, i32 src_len, char const *find, i32 find_len, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No);
DQN_FILE_SCOPE        bool           DqnStr_EndsWith              (char const *src, i32 src_len, char const *find, i32 find_len, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No);

// return: Helper function that returns the pointer to the first occurence, nullptr if not found.
DQN_FILE_SCOPE        char          *DqnStr_GetFirstOccurence     (char const *src, i32 src_len, char const *find, i32 find_len, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No);
DQN_FILE_SCOPE        bool           DqnStr_HasSubstring          (char const *src, i32 src_len, char const *find, i32 find_len, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No);

DQN_FILE_SCOPE        DqnSlice<char> DqnStr_RemoveLeadTrailChar   (char const *str, i32 str_len, char lead_char, char trail_char);
DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailQuotes (DqnSlice<char> slice);
DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailBraces (DqnSlice<char> slice);
DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailQuotes (char const *str, i32 str_len);
DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailBraces (char const *str, i32 str_len);


#define DQN_I32_MAX_STR_SIZE 11
#define DQN_I64_MAX_STR_SIZE 21
// Return the len of the derived string. If buf is nullptr and or buf_size is 0 the function returns the
// required string length for the integer
// TODO NOTE(doyle): Parsing stops when a non-digit is encountered, so numbers with ',' don't work atm.
DQN_FILE_SCOPE        i32 Dqn_I64ToStr(i64 const value, char *buf, i32 buf_size);
DQN_FILE_SCOPE        i64 Dqn_StrToI64(char const *buf, i64 buf_size);
DQN_FILE_SCOPE inline i64 Dqn_StrToI64(DqnSlice<char const> buf) { return Dqn_StrToI64(buf.data, buf.len); }
DQN_FILE_SCOPE inline i64 Dqn_StrToI64(DqnSlice<char> buf)       { return Dqn_StrToI64(buf.data, buf.len); }

// WARNING: Not robust, precision errors and whatnot but good enough!
DQN_FILE_SCOPE f32 Dqn_StrToF32(char const *buf, i64 buf_size);

// Both return the number of bytes read, return 0 if invalid codepoint or UTF8
DQN_FILE_SCOPE u32 Dqn_UCSToUTF8(u32 *dest, u32 character);
DQN_FILE_SCOPE u32 Dqn_UTF8ToUCS(u32 *dest, u32 character);

// #DqnWChar API
// =================================================================================================
// NOTE: See above for documentation
DQN_FILE_SCOPE bool     DqnWChar_IsDigit          (wchar_t c);
DQN_FILE_SCOPE wchar_t  DqnWChar_ToLower          (wchar_t c);

DQN_FILE_SCOPE wchar_t *DqnWChar_SkipWhitespace   (wchar_t *ptr);
DQN_FILE_SCOPE wchar_t *DqnWChar_FindLastChar     (wchar_t *ptr, wchar_t ch, i32 len, u32 *len_to_char);
DQN_FILE_SCOPE i32      DqnWChar_GetNextLine      (wchar_t *ptr, i32 *line_len);

DQN_FILE_SCOPE i32      DqnWStr_Cmp               (wchar_t const *a, wchar_t const *b);
DQN_FILE_SCOPE i32      DqnWStr_FindFirstOccurence(wchar_t const *src, i32 src_len, wchar_t const *find, i32 find_len);
DQN_FILE_SCOPE bool     DqnWStr_HasSubstring      (wchar_t const *src, i32 src_len, wchar_t const *find, i32 find_len);
DQN_FILE_SCOPE i32      DqnWStr_Len               (wchar_t const *a);
DQN_FILE_SCOPE i32      DqnWStr_LenDelimitWith    (wchar_t const *a, wchar_t delimiter);
DQN_FILE_SCOPE void     DqnWStr_Reverse           (wchar_t *buf, i32 buf_size);

DQN_FILE_SCOPE i32      Dqn_WStrToI32             (wchar_t const *buf, i32 buf_size);
DQN_FILE_SCOPE i32      Dqn_I32ToWStr             (i32 value, wchar_t *buf, i32 buf_size);

// #DqnRnd API
// =================================================================================================
struct DqnRndPCG // PCG (Permuted Congruential Generator)
{
    u64 state[2];

    DqnRndPCG();
    DqnRndPCG(u32 seed);

    u32 Next ();                 // return: A random number  N between [0, 0xFFFFFFFF]
    f32 Nextf();                 // return: A random float   N between [0.0, 1.0f]
    i32 Range(i32 min, i32 max); // return: A random integer N between [min, max]
};

// #Dqn_* API
// =================================================================================================
// return: The number of splits in the array. If array is null this returns the required size of the array.
i32 Dqn_SplitString(char const *src, i32 src_len, char split_char, DqnSlice<char> *array = nullptr, i32 size = 0);

// Util function that uses Dqn_SplitString
// return: The number of splits, splitting by "split_char" would generate.
i32 Dqn_GetNumSplits(char const *src, i32 src_len, char split_char);

DQN_FILE_SCOPE inline bool Dqn_BitIsSet (u32 bits, u32 flag);
DQN_FILE_SCOPE inline u32  Dqn_BitSet   (u32 bits, u32 flag);
DQN_FILE_SCOPE inline u32  Dqn_BitUnset (u32 bits, u32 flag);
DQN_FILE_SCOPE inline u32  Dqn_BitToggle(u32 bits, u32 flag);

template <typename T> using DqnQuickSort_LessThanProc =                  bool (*) (T const &a, T const &b, void *user_context);
#define DQN_QUICK_SORT_LESS_THAN_PROC(name) template <typename T> inline bool name(T const &a, T const &b, void *user_context)
DQN_QUICK_SORT_LESS_THAN_PROC(DqnQuickSort_DefaultLessThan)
{
    (void)user_context;
    bool result = a < b;
    return result;
}

template <typename T, DqnQuickSort_LessThanProc<T> IsLessThan = DqnQuickSort_DefaultLessThan<T>>
DQN_FILE_SCOPE void DqnQuickSort(T *array, isize size, void *user_context)
{
    if (!array || size <= 1) return;

#if 1
    // Insertion Sort, under 24->32 is an optimal amount
    const i32 QUICK_SORT_THRESHOLD = 24;
    if (size < QUICK_SORT_THRESHOLD)
    {
        i32 item_to_insert_index = 1;
        while (item_to_insert_index < size)
        {
            for (i32 check_index = 0; check_index < item_to_insert_index; check_index++)
            {
                if (!IsLessThan(array[check_index], array[item_to_insert_index], user_context))
                {
                    T item_to_insert = array[item_to_insert_index];
                    for (i32 i   = item_to_insert_index; i > check_index; i--)
                        array[i] = array[i - 1];

                    array[check_index] = item_to_insert;
                    break;
                }
            }
            item_to_insert_index++;
        }

        return;
    }
#endif

    auto state          = DqnRndPCG();
    auto last_index      = size - 1;
    auto pivot_index     = (i64)state.Range(0, (i32)last_index);
    auto partition_index = 0;
    auto start_index     = 0;

    // Swap pivot with last index, so pivot is always at the end of the array.
    // This makes logic much simpler.
    DQN_SWAP(T, array[last_index], array[pivot_index]);
    pivot_index = last_index;

    // 4^, 8, 7, 5, 2, 3, 6
    if (IsLessThan(array[start_index], array[pivot_index], user_context)) partition_index++;
    start_index++;

    // 4, |8, 7, 5^, 2, 3, 6*
    // 4, 5, |7, 8, 2^, 3, 6*
    // 4, 5, 2, |8, 7, ^3, 6*
    // 4, 5, 2, 3, |7, 8, ^6*
    for (auto check_index = start_index; check_index < last_index; check_index++)
    {
        if (IsLessThan(array[check_index], array[pivot_index], user_context))
        {
            DQN_SWAP(T, array[partition_index], array[check_index]);
            partition_index++;
        }
    }

    // Move pivot to right of partition
    // 4, 5, 2, 3, |6, 8, ^7*
    DQN_SWAP(T, array[partition_index], array[pivot_index]);
    DqnQuickSort<T, IsLessThan>(array, partition_index, user_context);

    // Skip the value at partion index since that is guaranteed to be sorted.
    // 4, 5, 2, 3, (x), 8, 7
    i32 one_after_partition_index = partition_index + 1;
    DqnQuickSort<T, IsLessThan>(array + one_after_partition_index, (size - one_after_partition_index), user_context);
}

template <typename T>
DQN_FILE_SCOPE void DqnQuickSort(T *array, isize size)
{
    if (!array || size <= 1) return;

#if 1
    // Insertion Sort, under 24->32 is an optimal amount
    const i32 QUICK_SORT_THRESHOLD = 24;
    if (size < QUICK_SORT_THRESHOLD)
    {
        i32 item_to_insert_index = 1;
        while (item_to_insert_index < size)
        {
            for (i32 check_index = 0; check_index < item_to_insert_index; check_index++)
            {
                if (!(array[check_index] < array[item_to_insert_index]))
                {
                    T item_to_insert = array[item_to_insert_index];
                    for (i32 i   = item_to_insert_index; i > check_index; i--)
                        array[i] = array[i - 1];

                    array[check_index] = item_to_insert;
                    break;
                }
            }
            item_to_insert_index++;
        }

        return;
    }
#endif

    auto state          = DqnRndPCG();
    auto last_index      = size - 1;
    auto pivot_index     = (i64)state.Range(0, (i32)last_index);
    auto partition_index = 0;
    auto start_index     = 0;

    // Swap pivot with last index, so pivot is always at the end of the array.
    // This makes logic much simpler.
    DQN_SWAP(T, array[last_index], array[pivot_index]);
    pivot_index = last_index;

    // 4^, 8, 7, 5, 2, 3, 6
    if (array[start_index] < array[pivot_index]) partition_index++;
    start_index++;

    // 4, |8, 7, 5^, 2, 3, 6*
    // 4, 5, |7, 8, 2^, 3, 6*
    // 4, 5, 2, |8, 7, ^3, 6*
    // 4, 5, 2, 3, |7, 8, ^6*
    for (auto check_index = start_index; check_index < last_index; check_index++)
    {
        if (array[check_index] < array[pivot_index])
        {
            DQN_SWAP(T, array[partition_index], array[check_index]);
            partition_index++;
        }
    }

    // Move pivot to right of partition
    // 4, 5, 2, 3, |6, 8, ^7*
    DQN_SWAP(T, array[partition_index], array[pivot_index]);
    DqnQuickSort(array, partition_index);

    // Skip the value at partion index since that is guaranteed to be sorted.
    // 4, 5, 2, 3, (x), 8, 7
    i32 one_after_partition_index = partition_index + 1;
    DqnQuickSort(array + one_after_partition_index, (size - one_after_partition_index));
}


template <typename T> using DqnBSearch_LessThanProc = bool (*)(const T&, const T&);
template <typename T> using DqnBSearch_EqualsProc   = bool (*)(const T&, const T&);
#define DQN_BSEARCH_LESS_THAN_PROC(name) template <typename T> inline bool name(T const &a, T const &b)
#define DQN_BSEARCH_EQUALS_PROC(name)    template <typename T> inline bool name(T const &a, T const &b)
DQN_BSEARCH_LESS_THAN_PROC(DqnBSearch_DefaultLessThan) { return a < b;  }
DQN_BSEARCH_EQUALS_PROC   (DqnBSearch_DefaultEquals)   { return a == b; }

enum struct DqnBSearchType
{
    Match,    // Return the index of the first item that matches the find value
    MinusOne, // Return the index of the first item lower than the find value
    PlusOne,  // Return the index of the first item higher than the find value

    MatchOrMinusOne, // Return the index of the matching item if not found the first item lower
    MatchOrPlusOne,  // Return the index of the matching item if not found the first item higher
};

// type:   The matching behaviour of the binary search,
// return: -1 if element not found, otherwise index of the element.
//         For higher and lower bounds return -1 if there is no element higher/lower than the
//         find value (i.e. -1 if the 0th element is the find val for lower bound).
template <typename T,
          DqnBSearch_LessThanProc<T> IsLessThan = DqnBSearch_DefaultLessThan<T>,
          DqnBSearch_EqualsProc<T> Equals       = DqnBSearch_DefaultEquals<T>>
DQN_FILE_SCOPE i64
DqnBSearch(T const *array, isize size, T const &find, DqnBSearchType type = DqnBSearchType::Match)
{
    if (size == 0 || !array)
    {
        return -1;
    }

    isize start = 0;
    isize end   = size - 1;
    isize mid   = static_cast<isize>((start + end) * 0.5f);

    while (start <= end)
    {
        if (Equals(array[mid], find))
        {
            if (type == DqnBSearchType::Match ||
                type == DqnBSearchType::MatchOrMinusOne ||
                type == DqnBSearchType::MatchOrPlusOne)
            {
                return mid;
            }
            else if (type == DqnBSearchType::MinusOne)
            {
                // NOTE: We can always -1 because at worst case, 0 index will go to -1 which is
                // correct behaviour.
                return mid - 1;
            }
            else
            {
                return ((mid + 1) >= size) ? -1 : mid + 1;
            }
        }
        else if (IsLessThan(array[mid], find)) start = mid + 1;
        else                                   end   = mid - 1;
        mid = static_cast<isize>((start + end) * 0.5f);
    }

    if (type == DqnBSearchType::Match)
    {
        return -1;
    }
    if (type == DqnBSearchType::MinusOne || type == DqnBSearchType::MatchOrMinusOne)
    {
        return (IsLessThan(find, array[mid])) ? -1 : mid;
    }
    else
    {
        return (IsLessThan(find, array[mid])) ? mid : -1;
    }
}

DQN_FILE_SCOPE inline i64 DqnBSearch(i64 const *array, i64 size, i64 find, DqnBSearchType type = DqnBSearchType::Match) { return DqnBSearch<i64>(array, size, find, type); }

// #DqnMem API
// =================================================================================================
// TODO(doyle): Use platform allocation, fallback to malloc if platform not defined
DQN_FILE_SCOPE void *DqnMem_Alloc  (usize size);
DQN_FILE_SCOPE void *DqnMem_Calloc (usize size);
DQN_FILE_SCOPE void  DqnMem_Clear  (void *memory, u8 clear_val, usize size);
DQN_FILE_SCOPE void *DqnMem_Realloc(void *memory, usize new_size);
DQN_FILE_SCOPE void  DqnMem_Free   (void *memory);
DQN_FILE_SCOPE void  DqnMem_Copy   (void *dest, void const *src, usize num_bytes_to_copy);
DQN_FILE_SCOPE void *DqnMem_Set    (void *dest, u8 value,        usize num_bytes_to_set);
DQN_FILE_SCOPE int   DqnMem_Cmp(void const *src, void const *dest, usize num_bytes);

// #DqnMemAPI API
// =================================================================================================
// You only need to care about this API if you want to use custom mem-alloc routines in the data
// structures! Otherwise it already has a default one to use.

// How To Use:
// 1. Implement the allocator function, where DqnMemAPI::Request will tell you the request.
//    - (NOTE) The callback should return the resulting data into DqnMemAPI::Result
// 2. Create a DqnMemAPI struct with a function ptr to your allocator
//    - (OPTIONAL) Set the user context to your book-keeping/mem allocating service
// 3. Initialise any data structure that supports a DqnMemAPI with your struct.

// That's it! Done :) Of course, changing mem_api's after initialisation is invalid since the
// pointers belonging to your old routine may not be tracked in your new mem_api. So you're at your
// own discretion there.

class DqnMemAPI
{
public:
    enum struct Type
    {
        Uninitialised,
        Alloc,
        Calloc,
        Realloc,
        Free
    };

    struct Request
    {
        void *user_context;
        Type type;
        union
        {
            struct Alloc_
            {
                bool zero_clear;
                isize request_size;
            } alloc;

            struct Free_
            {
                void  *ptr_to_free;
                isize size_to_free;
            } free;

            struct Realloc_
            {
                isize new_size;
                isize old_size;
                void *old_mem_ptr;
            } realloc;
        } e;
    };

    typedef void *Allocator(DqnMemAPI *, DqnMemAPI::Request);

    Allocator *allocator;
    void      *user_context;

    isize     bytes_allocated;
    isize     lifetime_bytes_allocated;
    isize     lifetime_bytes_freed;

    static DqnMemAPI HeapAllocator ();

    enum struct StackPushType
    {
        Head,
        Tail,
    };
    static DqnMemAPI StackAllocator(struct DqnMemStack *stack, StackPushType type = StackPushType::Head);

    void *Realloc(void  *old_ptr,    isize old_size, isize new_size);
    void *Alloc  (isize  size,      Dqn::ZeroClear clear = Dqn::ZeroClear::Yes);
    void  Free   (void  *ptr_to_free, isize size_to_free = 0); // TODO(doyle): size_to_free opt is iffy
    bool  IsValid() const { return (this->allocator != nullptr); }
};

// #DqnPool API
// =================================================================================================
template <typename T, i16 SIZE>
struct DqnFixedPool
{
    struct Entry : public T
    {
        u16 nextIndex;
    };

    const static isize SENTINEL_INDEX = SIZE;

    Entry  pool[SIZE];
    i16    freeIndex;
    i16    numFree;

    DqnFixedPool() : freeIndex(0) , numFree(SIZE)
    {
        DQN_FOR_EACH(i, SIZE - 1)
        {
            Entry *entry     = pool + i;
            entry->nextIndex = i + 1;
        }
        Entry *last     = pool + (SIZE - 1);
        last->nextIndex = SENTINEL_INDEX;
    }

    T *GetNext()
    {
        if (freeIndex == SENTINEL_INDEX) return nullptr;
        Entry *result = pool + freeIndex;
        freeIndex     = result->nextIndex;
        numFree--;
        return result;
    }

    void Return(T *item)
    {
        auto *entry      = reinterpret_cast<Entry *>(item);
        entry->nextIndex = freeIndex;
        freeIndex        = entry - pool;
        numFree++;
    }
};

// #DqnPool API
// =================================================================================================
template <typename T>
struct DqnPool
{
    struct Entry : public T
    {
        u16 nextIndex;
    };

    Entry  *pool;
    i16     freeIndex;
    i16     numFree;
    i32     size;

    void UseMemory(Entry *pool_, isize size)
    {
        pool      = pool_;
        freeIndex = 0;
        numFree   = size;

        DQN_FOR_EACH(i, size - 1)
        {
            Entry *entry     = pool + i;
            entry->nextIndex = i + 1;
        }
        Entry *last     = pool + (size - 1);
        last->nextIndex = size;
    }

    T *GetNext()
    {
        if (freeIndex == size) return nullptr;
        Entry *result = pool + freeIndex;
        freeIndex     = result->nextIndex;
        numFree--;
        return result;
    }

    void Return(T *item)
    {
        auto *entry      = reinterpret_cast<Entry *>(item);
        entry->nextIndex = freeIndex;
        freeIndex        = entry - pool;
        numFree++;
    }
};

FILE_SCOPE DqnMemAPI DQN_DEFAULT_HEAP_ALLOCATOR_ = DqnMemAPI::HeapAllocator();
FILE_SCOPE DqnMemAPI *DQN_DEFAULT_HEAP_ALLOCATOR = &DQN_DEFAULT_HEAP_ALLOCATOR_;

// #DqnArray API
// =================================================================================================
template<typename T>
struct DqnArray
{
    DqnMemAPI                  *mem_api = DQN_DEFAULT_HEAP_ALLOCATOR;
    isize                       len;
    isize                       max;
    T                          *data;

     DqnArray        () = default;
     DqnArray        (DqnMemAPI *mem_api_)                         { *this = {}; this->mem_api = mem_api_; }
    // ~DqnArray        ()                                           { if (this->data && this->mem_api) this->mem_api->Free(data); }

    void  UseMemory  (T *data_, isize max_, isize len_ = 0)     { this->mem_api = nullptr; this->data = data_; this->max = max_; this->len = len_; }
    void  Clear      (Dqn::ZeroClear clear = Dqn::ZeroClear::No)  { if (!data) return; len = 0; if (clear == Dqn::ZeroClear::Yes) DqnMem_Clear(data, 0, sizeof(T) * max); }
    void  Free       ()                                           { if (data) { mem_api->Free(data); } *this = {}; }
    T    *Front      ()                                           { DQN_ASSERT(len > 0); return data + 0; }
    T    *Back       ()                                           { DQN_ASSERT(len > 0); return data + (len - 1); }
    void  Resize     (isize new_len)                             { if (new_len > max) Reserve(GrowCapacity_(new_len)); len = new_len; }
    void  Resize     (isize new_len, T const *v)                 { if (new_len > max) Reserve(GrowCapacity_(new_len)); if (new_len > len) for (isize n = len; n < new_len; n++) data[n] = *v; len = new_len; }
    void  Reserve    (isize new_max);
    T    *Make       (isize len = 1)                              { len += len; if (len > max) Reserve(GrowCapacity_(len)); return &data[len - len]; }
    T    *Push       (T const &v)                                 { return Insert(len, &v, 1); }
    T    *Push       (T const *v, isize len_items = 1)             { return Insert(len,  v, len_items); }
    void  Pop        ()                                           { if (len > 0) len--; }
    void  Erase      (isize index)                                { DQN_ASSERT(index >= 0 && index < len); data[index] = data[--len]; }
    void  EraseStable(isize index);
    T    *Insert     (isize index, T const *v)                    { return Insert(index,  v, 1); }
    T    *Insert     (isize index, T const &v)                    { return Insert(index, &v, 1); }
    T    *Insert     (isize index, T const *v, isize len_items);
    bool  Contains   (T const *v) const                           { T const *ptr = data;  T const *end = data + len; while (ptr < end) if (*ptr++ == *v) return true; return false; }

    T    &operator[] (isize i) const                              { DQN_ASSERT(i < len && i > 0); return this->data[i]; }
    T    *begin      ()                                           { return data; }
    T    *begin      () const                                     { return data; }
    T    *end        ()                                           { return data + len; }
    T    *end        () const                                     { return data + len; }

private:
    isize GrowCapacity_(isize size) const                         { isize new_max = max ? (max * 2) : 8; return new_max > size ? new_max : size; }
};

template<typename T> T *DqnArray<T>::Insert(isize index, T const *v, isize len_items)
{
    index                = DQN_MIN(DQN_MAX(index, 0), len);
    isize const new_len = len + len_items;

    if (new_len >= max)
      Reserve(GrowCapacity_(new_len));

    T *src  = data + index;
    T *dest = src + len_items;

    if (src < dest)
        memmove(dest, src, ((data + len) - src) * sizeof(T));

    len = new_len;
    for (isize i = 0; i < len_items; i++)
        src[i] = v[i];

    return src;
}

template <typename T> void DqnArray<T>::EraseStable(isize index)
{
    DQN_ASSERT(index >= 0 && index < len);
    isize const off = (data + index) - data;
    memmove(data + off, data + off + 1, ((usize)len - (usize)off - 1) * sizeof(T));
    len--;
}

template <typename T> void DqnArray<T>::Reserve(isize new_max)
{
    if (new_max <= max) return;

    if (data)
    {
        T *newData = (T *)mem_api->Realloc(data, max * sizeof(T), new_max * sizeof(T));
        data = newData;
        max  = new_max;
    }
    else
    {
      data = (T *)mem_api->Alloc(new_max * sizeof(T));
      max  = new_max;
    }

    DQN_ASSERT(data);
}

// #DqnMemTracker
// =================================================================================================
// Allocation Layout
// +--------------------+-------------------------------------------------------------------------+-----------------------------+-----------------+
// | Ptr From Allocator | Offset To Src | Alignment | Alloc Type | Alloc Amount | B. Guard (Opt.) | Aligned Ptr For Client      | B. Guard (Opt.) |
// +--------------------+-------------------------------------------------------------------------+-----------------------------+-----------------+
// Ptr From Allocator:     The pointer returned by the allocator, not aligned
// Alignment:              The pointer given to the client is aligned to this power of two boundary
// Alloc Type:             If the allocation was allocated from the head or tail of the memory block (mainly for memstacks).
// Alloc Amount:           The requested allocation amount by the client (so does not include metadata)
// B. Guard:               Bounds Guard value.
// Aligned Ptr For Client: The allocated memory for the client.
// Offset To Src:          Number of bytes to subtract from the "Aligned Ptr For Client" to return to the "Ptr From Allocator"

#pragma pack(push, 1)
struct DqnPtrHeader
{
    u8    offset_to_src_ptr; // Offset to subtract from the client ptr to receive the allocation ptr
    u8    alignment;
    u8    alloc_type;
    usize alloc_amount;
};
#pragma pack(pop)

struct DqnMemTracker
{
    static u32 const HEAD_GUARD_VALUE   = 0xCAFEBABE;
    static u32 const TAIL_GUARD_VALUE   = 0xDEADBEEF;

    DqnArray<void *> allocations;     // Read: When BoundsGuard is enabled, tracks all allocations.
    u32              bounds_guard_size; // Read: sizeof(GUARD_VALUE) OR 0 if BoundsGuard is disabled.

    void   Init                (bool bounds_guard);
    void   Free                ()                               { allocations.Free(); }
    void   AddAllocation       (char *ptr)                      { DQN_ASSERT(allocations.Push(ptr) != nullptr); }
    void   RemoveAllocation    (char *ptr);

    void   CheckAllocations    ()                         const;
    isize  GetAllocationSize   (isize size, u8 alignment) const { return sizeof(DqnPtrHeader) + bounds_guard_size + (alignment - 1) + size + bounds_guard_size; }

    // ptr: The ptr given to the client when allocating.
    u32          *PtrToHeadGuard (char *ptr) const { return reinterpret_cast<u32 *>(ptr - bounds_guard_size);  }
    u32          *PtrToTailGuard (char *ptr) const { return reinterpret_cast<u32 *>(ptr + PtrToHeader(ptr)->alloc_amount); }
    DqnPtrHeader *PtrToHeader    (char *ptr) const { return reinterpret_cast<DqnPtrHeader *>(ptr - bounds_guard_size - sizeof(DqnPtrHeader)); }
};

// #DqnMemStack API
// =================================================================================================
// DqnMemStack is an memory allocator in a stack like, push-pop style. It pre-allocates a block of
// memory at init and sub-allocates from this block to take advantage of memory locality.

// When an allocation requires a larger amount of memory than available in the block then the
// MemStack will allocate a new block of sufficient size for you in DqnMemStack_Push(..). This DOES
// mean that there will be wasted spaceat the end of each block and is a tradeoff for memory
// locality against optimal space usage.

// How To Use:
// DqnMemStack stack = {};
//             stack.Init(DQN_MEGABYTE(1), true, DqnMemStack::Flag::BoundsGuard);
// u8 *data =  stack.Push(128);

#define DQN_MEM_STACK_PUSH_STRUCT(memStack, Type, num) (Type *)(memStack)->Push(sizeof(Type) * num)
struct DqnMemStack
{
    static const i32 MINIMUM_BLOCK_SIZE = DQN_KILOBYTE(64);

    enum Flag
    {
        NonExpandable       = (1 << 0), // Disallow additional memory blocks when full.
        NonExpandableAssert = (1 << 1), // Assert when non-expandable is set and we run out of space
        BoundsGuard         = (1 << 2), // Track, check and add 4 byte guards on the boundaries of all allocations
        PushAssertsOnFail   = (1 << 3), // Assert when push() fails.
        All                 = (NonExpandable | NonExpandableAssert | BoundsGuard | PushAssertsOnFail),
    };

    struct Info // Statistics of the memory stack.
    {
        isize total_used;
        isize total_size;
        isize wasted_size;
        i32   num_blocks;
    };

    struct Block
    {
        char  *memory;    // Read
        isize  size;      // Read
        isize  used_;     // Read
        Block *prev_block; // Read Uses a linked list approach for additional blocks

        char *head;       // Read
        char *tail;       // Read

        Block(void *memory_, isize size_) : memory((char *)memory_), size(size_), prev_block(nullptr), head((char *)memory_), tail((char *)memory_ + size_) {}
    };

    DqnMemTracker  tracker;         // Read: Metadata for managing ptr allocation
    DqnMemAPI     *mem_api;          // Read: API used to add additional memory blocks to this stack.
    Block         *block;           // Read: Memory block allocated for the stack
    u32            flags;           // Read
    i32            tmp_region_count; // Read: The number of temp memory regions in use

    DqnMemStack() = default;

    // Uses fixed memory, will be sourced from the buffer and assert after buffer is full.
    DqnMemStack(void *mem, isize size, Dqn::ZeroClear clear, u32 flags_ = 0);

    // Memory Stack capable of expanding when full, but only if NonExpandable flag is not set.
    DqnMemStack(isize size, Dqn::ZeroClear clear, u32 flags_ = 0, DqnMemAPI *api = DQN_DEFAULT_HEAP_ALLOCATOR);

    // Allocation API
    // =============================================================================================
    enum struct AllocTo
    {
        Head, Tail
    };

    // Allocate memory from the MemStack.
    // alignment: Ptr returned from allocator is aligned to this value and MUST be power of 2.
    // return:    nullptr if out of space OR stack is using fixed memory/size OR stack full and platform malloc fails.
    void *Push          (isize size, AllocTo allocTo = AllocTo::Head, u8 alignment = 4);

    // Frees the given ptr. It MUST be the last allocated item in the stack, asserts otherwise.
    void  Pop           (void *ptr, Dqn::ZeroClear clear = Dqn::ZeroClear::No);

    // Frees all blocks belonging to this stack.
    void  Free          ();

    // Frees the specified block belonging to the stack.
    // return: FALSE if block doesn't belong this into calls DqnMem_Free() or invalid args.
    bool  FreeMemBlock  (Block *mem_block);

    // Frees the last-most memory block. If last block, free that block making the MemStack blockless.
    // Next allocate will attach a block.
    bool  FreeLastBlock ();

    // Reverts the stack and its usage back to the first block
    void  Reset         ();
    void  ResetTail     ();

    // Reset the current memory block usage to 0.
    void  ClearCurrBlock(Dqn::ZeroClear clear);
    Info  GetInfo       ()                      const;

    // Temporary Memory Regions API
    // =============================================================================================
    // TODO(doyle): Look into a way of "preventing/guarding" against anual calls to free/clear in temp regions
    struct TempRegion
    {
        DqnMemStack *stack;             // Stack associated with this TempRegion
        Block       *starting_block;     // Remember the block to revert to and its memory usage.
        char        *starting_block_head;
        char        *starting_block_tail;
        bool         keep_head_changes = false;
        bool         keep_tail_changes = false;
    };

    struct TempRegionGuard_
    {
         TempRegionGuard_(DqnMemStack *stack);
        ~TempRegionGuard_();
        TempRegion region;
    };

    // Revert all allocations between the Begin() and End() regions. Guard version is RAII'ed.
    TempRegion       TempRegionBegin      ();
    void             TempRegionEnd        (TempRegion region);
    TempRegionGuard_ TempRegionGuard      ();

    // Keep allocations that have occurred since Begin(). End() does not need to be called anymore.
    void             TempRegionKeepChanges(TempRegion region);
};

// #DqnHash API
// =================================================================================================
DQN_FILE_SCOPE u32 DqnHash_Murmur32Seed(void const *data,  usize len, u32 seed);
DQN_FILE_SCOPE u64 DqnHash_Murmur64Seed(void const *data_, usize len, u64 seed);

DQN_FILE_SCOPE inline u32 DqnHash_Murmur32(void const *data, usize len)
{
    return DqnHash_Murmur32Seed(data, len, 0x9747b28c);
}

DQN_FILE_SCOPE inline u64 DqnHash_Murmur64(void const *data, usize len)
{
    return DqnHash_Murmur64Seed(data, len, 0x9747b28c);
}

// #DqnMath API
// =================================================================================================
DQN_FILE_SCOPE f32 DqnMath_Lerp  (f32 a, f32 t, f32 b);
DQN_FILE_SCOPE f32 DqnMath_Sqrtf (f32 a);
DQN_FILE_SCOPE f32 DqnMath_Clampf(f32 val, f32 min, f32 max);

// #DqnV2 API
// =================================================================================================
union DqnV2i
{
    struct { i32 x, y; };
    struct { i32 w, h; };
    struct { i32 min, max; };
    i32 e[2];

    DqnV2i() = default;
    DqnV2i(i32 x_, i32 y_): x(x_), y(y_) {}
    DqnV2i(f32 x_, f32 y_): x((i32)x_), y((i32)y_) {}

    bool operator==(DqnV2i const &b) const { return (this->x == b.x) && (this->y == b.y); }
    bool operator!=(DqnV2i const &b) const { return !(*this == b); }
    bool operator>=(DqnV2i const &b) const { return (this->x >= b.x) && (this->y >= b.y); }
    bool operator<=(DqnV2i const &b) const { return (this->x <= b.x) && (this->y <= b.y); }
    bool operator< (DqnV2i const &b) const { return (this->x <  b.x) && (this->y <  b.y); }
    bool operator> (DqnV2i const &b) const { return (this->x >  b.x) && (this->y >  b.y); }
};

union DqnV2
{
    struct { f32 x, y; };
    struct { f32 w, h; };
    struct { f32 min, max; };
    f32 e[2];

    DqnV2() = default;
    DqnV2(f32 xy)        : x(xy),       y(xy)       {}
    DqnV2(f32 x_, f32 y_): x(x_),       y(y_)       {}
    DqnV2(i32 x_, i32 y_): x((f32)x_),  y((f32)y_)  {}
    DqnV2(DqnV2i a)      : x((f32)a.x), y((f32)a.y) {}

    bool operator==(DqnV2 const &b) const { return (this->x == b.x) && (this->y == b.y); }
    bool operator!=(DqnV2 const &b) const { return !(*this == b); }
    bool operator>=(DqnV2 const &b) const { return (this->x >= b.x) && (this->y >= b.y); }
    bool operator<=(DqnV2 const &b) const { return (this->x <= b.x) && (this->y <= b.y); }
    bool operator< (DqnV2 const &b) const { return (this->x <  b.x) && (this->y <  b.y); }
    bool operator> (DqnV2 const &b) const { return (this->x >  b.x) && (this->y >  b.y); }
};

DQN_FILE_SCOPE DqnV2 DqnV2_Add     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Sub     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Scalei  (DqnV2 a, i32 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Scalef  (DqnV2 a, f32 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Hadamard(DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE f32   DqnV2_Dot     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE bool  DqnV2_Equals  (DqnV2 a, DqnV2 b);

DQN_FILE_SCOPE f32   DqnV2_LengthSquared(const DqnV2 a, const DqnV2 b);
DQN_FILE_SCOPE f32   DqnV2_Length       (const DqnV2 a, const DqnV2 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Normalise    (const DqnV2 a);
DQN_FILE_SCOPE bool  DqnV2_Overlaps     (      DqnV2 a,       DqnV2 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Perpendicular(const DqnV2 a);

DQN_FILE_SCOPE DqnV2 DqnV2_ResizeKeepAspectRatio(DqnV2 src_size, DqnV2 target_size);
DQN_FILE_SCOPE DqnV2 DqnV2_ConstrainToRatio     (DqnV2 dim, DqnV2 ratio); // Resize the dimension to fit the aspect ratio provided. Downscale only.

DQN_FILE_SCOPE inline DqnV2  operator- (DqnV2  a, DqnV2 b) { return      DqnV2_Sub     (a, b);  }
DQN_FILE_SCOPE inline DqnV2  operator+ (DqnV2  a, DqnV2 b) { return      DqnV2_Add     (a, b);  }
DQN_FILE_SCOPE inline DqnV2  operator* (DqnV2  a, DqnV2 b) { return      DqnV2_Hadamard(a, b);  }
DQN_FILE_SCOPE inline DqnV2  operator* (DqnV2  a, f32   b) { return      DqnV2_Scalef  (a, b);  }
DQN_FILE_SCOPE inline DqnV2  operator* (DqnV2  a, i32   b) { return      DqnV2_Scalei  (a, b);  }
DQN_FILE_SCOPE inline DqnV2 &operator*=(DqnV2 &a, DqnV2 b) { return (a = DqnV2_Hadamard(a, b)); }
DQN_FILE_SCOPE inline DqnV2 &operator*=(DqnV2 &a, f32   b) { return (a = DqnV2_Scalef  (a, b)); }
DQN_FILE_SCOPE inline DqnV2 &operator*=(DqnV2 &a, i32   b) { return (a = DqnV2_Scalei  (a, b)); }
DQN_FILE_SCOPE inline DqnV2 &operator-=(DqnV2 &a, DqnV2 b) { return (a = DqnV2_Sub     (a, b)); }
DQN_FILE_SCOPE inline DqnV2 &operator+=(DqnV2 &a, DqnV2 b) { return (a = DqnV2_Add     (a, b)); }

// DqnV2i
DQN_FILE_SCOPE DqnV2i DqnV2i_Add     (DqnV2i a, DqnV2i b);
DQN_FILE_SCOPE DqnV2i DqnV2i_Sub     (DqnV2i a, DqnV2i b);
DQN_FILE_SCOPE DqnV2i DqnV2i_Scalei  (DqnV2i a, i32 b);
DQN_FILE_SCOPE DqnV2i DqnV2i_Scalef  (DqnV2i a, f32 b);
DQN_FILE_SCOPE DqnV2i DqnV2i_Hadamard(DqnV2i a, DqnV2i b);
DQN_FILE_SCOPE f32    DqnV2i_Dot     (DqnV2i a, DqnV2i b);
DQN_FILE_SCOPE bool   DqnV2i_Equals  (DqnV2i a, DqnV2i b);

DQN_FILE_SCOPE inline DqnV2i  operator- (DqnV2i  a, DqnV2i b) { return      DqnV2i_Sub     (a, b);  }
DQN_FILE_SCOPE inline DqnV2i  operator+ (DqnV2i  a, DqnV2i b) { return      DqnV2i_Add     (a, b);  }
DQN_FILE_SCOPE inline DqnV2i  operator* (DqnV2i  a, DqnV2i b) { return      DqnV2i_Hadamard(a, b);  }
DQN_FILE_SCOPE inline DqnV2i  operator* (DqnV2i  a, f32    b) { return      DqnV2i_Scalef  (a, b);  }
DQN_FILE_SCOPE inline DqnV2i  operator* (DqnV2i  a, i32    b) { return      DqnV2i_Scalei  (a, b);  }
DQN_FILE_SCOPE inline DqnV2i &operator*=(DqnV2i &a, DqnV2i b) { return (a = DqnV2i_Hadamard(a, b)); }
DQN_FILE_SCOPE inline DqnV2i &operator-=(DqnV2i &a, DqnV2i b) { return (a = DqnV2i_Sub     (a, b)); }
DQN_FILE_SCOPE inline DqnV2i &operator+=(DqnV2i &a, DqnV2i b) { return (a = DqnV2i_Add     (a, b)); }

// #DqnV3 API
// =================================================================================================
union DqnV3
{
    struct { f32 x, y, z; };
    struct { f32 r, g, b; };
    DqnV2 xy;
    f32 e[3];

    DqnV3() = default;
    DqnV3(f32 xyz)               : x(xyz),     y(xyz),     z(xyz)     {}
    DqnV3(f32 x_, f32 y_, f32 z_): x(x_),      y(y_),      z(z_)      {}
    DqnV3(i32 x_, i32 y_, i32 z_): x((f32)x_), y((f32)y_), z((f32)z_) {}
};

union DqnV3i
{
    struct { i32 x, y, z; };
    struct { i32 r, g, b; };
    i32 e[3];

    DqnV3i() = default;
    DqnV3i(i32 x_, i32 y_, i32 z_): x(x_),      y(y_),      z(z_)      {}
    DqnV3i(f32 x_, f32 y_, f32 z_): x((i32)x_), y((i32)y_), z((i32)z_) {}
};

// DqnV3
DQN_FILE_SCOPE DqnV3 DqnV3_Add     (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Sub     (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Scalei  (DqnV3 a, i32 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Scalef  (DqnV3 a, f32 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Hadamard(DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE f32   DqnV3_Dot     (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE bool  DqnV3_Equals  (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Cross   (DqnV3 a, DqnV3 b);

DQN_FILE_SCOPE DqnV3 DqnV3_Normalise    (DqnV3 a);
DQN_FILE_SCOPE f32   DqnV3_Length       (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE f32   DqnV3_LengthSquared(DqnV3 a, DqnV3 b);

DQN_FILE_SCOPE inline DqnV3  operator- (DqnV3  a, DqnV3 b) { return      DqnV3_Sub     (a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator+ (DqnV3  a, DqnV3 b) { return      DqnV3_Add     (a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator+ (DqnV3  a, f32   b) { return      DqnV3_Add     (a, DqnV3(b));    }
DQN_FILE_SCOPE inline DqnV3  operator* (DqnV3  a, DqnV3 b) { return      DqnV3_Hadamard(a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator* (DqnV3  a, f32   b) { return      DqnV3_Scalef  (a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator* (DqnV3  a, i32   b) { return      DqnV3_Scalei  (a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator/ (DqnV3  a, f32   b) { return      DqnV3_Scalef  (a, (1.0f/b));    }
DQN_FILE_SCOPE inline DqnV3 &operator*=(DqnV3 &a, DqnV3 b) { return (a = DqnV3_Hadamard(a, b));          }
DQN_FILE_SCOPE inline DqnV3 &operator*=(DqnV3 &a, f32   b) { return (a = DqnV3_Scalef  (a, b));          }
DQN_FILE_SCOPE inline DqnV3 &operator*=(DqnV3 &a, i32   b) { return (a = DqnV3_Scalei  (a, b));          }
DQN_FILE_SCOPE inline DqnV3 &operator-=(DqnV3 &a, DqnV3 b) { return (a = DqnV3_Sub     (a, b));          }
DQN_FILE_SCOPE inline DqnV3 &operator+=(DqnV3 &a, DqnV3 b) { return (a = DqnV3_Add     (a, b));          }
DQN_FILE_SCOPE inline bool   operator==(DqnV3  a, DqnV3 b) { return      DqnV3_Equals  (a, b);           }

// #DqnV4 API
// =================================================================================================
union DqnV4
{
    struct { f32 x, y, z, w; };
    DqnV3 xyz;
    DqnV2 xy;

    struct { f32 r, g, b, a; };
    DqnV3 rgb;

    f32    e[4];
    DqnV2 v2[2];

    DqnV4() = default;
    DqnV4(f32 xyzw)                      : x(xyzw),    y(xyzw),    z(xyzw),    w(xyzw)    {}
    DqnV4(f32 x_, f32 y_, f32 z_, f32 w_): x(x_),      y(y_),      z(z_),      w(w_)      {}
    DqnV4(i32 x_, i32 y_, i32 z_, i32 w_): x((f32)x_), y((f32)y_), z((f32)z_), w((f32)w_) {}
    DqnV4(DqnV3 a, f32 w_)               : x(a.x),     y(a.y),     z(a.z),     w(w_)      {}

};

DQN_FILE_SCOPE DqnV4 DqnV4_Add     (DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE DqnV4 DqnV4_Sub     (DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE DqnV4 DqnV4_Scalef  (DqnV4 a, f32 b);
DQN_FILE_SCOPE DqnV4 DqnV4_Scalei  (DqnV4 a, i32 b);
DQN_FILE_SCOPE DqnV4 DqnV4_Hadamard(DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE f32   DqnV4_Dot     (DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE bool  DqnV4_Equals  (DqnV4 a, DqnV4 b);

DQN_FILE_SCOPE inline DqnV4  operator- (DqnV4  a, DqnV4 b) { return      DqnV4_Sub     (a, b);            }
DQN_FILE_SCOPE inline DqnV4  operator+ (DqnV4  a, DqnV4 b) { return      DqnV4_Add     (a, b);            }
DQN_FILE_SCOPE inline DqnV4  operator+ (DqnV4  a, f32   b) { return      DqnV4_Add     (a, DqnV4(b));     }
DQN_FILE_SCOPE inline DqnV4  operator* (DqnV4  a, DqnV4 b) { return      DqnV4_Hadamard(a, b);            }
DQN_FILE_SCOPE inline DqnV4  operator* (DqnV4  a, f32   b) { return      DqnV4_Scalef  (a, b);            }
DQN_FILE_SCOPE inline DqnV4  operator* (DqnV4  a, i32   b) { return      DqnV4_Scalei  (a, b);            }
DQN_FILE_SCOPE inline DqnV4 &operator*=(DqnV4 &a, DqnV4 b) { return (a = DqnV4_Hadamard(a, b));           }
DQN_FILE_SCOPE inline DqnV4 &operator*=(DqnV4 &a, f32   b) { return (a = DqnV4_Scalef  (a, b));           }
DQN_FILE_SCOPE inline DqnV4 &operator*=(DqnV4 &a, i32   b) { return (a = DqnV4_Scalei  (a, b));           }
DQN_FILE_SCOPE inline DqnV4 &operator-=(DqnV4 &a, DqnV4 b) { return (a = DqnV4_Sub     (a, b));           }
DQN_FILE_SCOPE inline DqnV4 &operator+=(DqnV4 &a, DqnV4 b) { return (a = DqnV4_Add     (a, b));           }
DQN_FILE_SCOPE inline bool   operator==(DqnV4 &a, DqnV4 b) { return      DqnV4_Equals  (a, b);            }

// #DqnMat4 API
// =================================================================================================
typedef union DqnMat4
{
    // TODO(doyle): Row/column instead? More cache friendly since multiplication
    // prefers rows.
    DqnV4 col[4];
    f32   e[4][4]; // Column/row
} DqnMat4;

DQN_FILE_SCOPE DqnMat4 DqnMat4_Identity    ();

DQN_FILE_SCOPE DqnMat4 DqnMat4_Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar);
DQN_FILE_SCOPE DqnMat4 DqnMat4_Perspective (f32 fov_y_degrees, f32 aspect_ratio, f32 znear, f32 zfar);
DQN_FILE_SCOPE DqnMat4 DqnMat4_LookAt      (DqnV3 eye, DqnV3 center, DqnV3 up);

DQN_FILE_SCOPE DqnMat4 DqnMat4_Translate3f (f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnMat4 DqnMat4_TranslateV3 (DqnV3 vec);
DQN_FILE_SCOPE DqnMat4 DqnMat4_Rotate      (f32 radians, f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnMat4 DqnMat4_Scale       (f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnMat4 DqnMat4_ScaleV3     (DqnV3 scale);
DQN_FILE_SCOPE DqnMat4 DqnMat4_Mul         (DqnMat4 a, DqnMat4 b);
DQN_FILE_SCOPE DqnV4   DqnMat4_MulV4       (DqnMat4 a, DqnV4 b);

// #DqnRect API
// =================================================================================================
struct DqnRect
{
    DqnV2 min;
    DqnV2 max;

    DqnRect() = default;
    DqnRect(DqnV2 origin, DqnV2 size)   { this->min = origin;      this->max = origin + size; }
    DqnRect(f32 x, f32 y, f32 w, f32 h) { this->min = DqnV2(x, y); this->max = DqnV2(x + w, y + h); }
    DqnRect(i32 x, i32 y, i32 w, i32 h) { this->min = DqnV2(x, y); this->max = DqnV2(x + w, y + h); }

    f32   GetWidth ()                                    const { return max.w - min.w; }
    f32   GetHeight()                                    const { return max.h - min.h; }
    DqnV2 GetSize  ()                                    const { return max - min;     }
    void  GetSize  (f32 *const width, f32 *const height) const;
    DqnV2 GetCenter()                                    const;

    DqnRect ClipRect (DqnRect const clip)  const;
    DqnRect Move     (DqnV2   const shift) const;
    bool    ContainsP(DqnV2   const p)     const;
};

// #DqnString Public API - String library
// =================================================================================================
struct DqnString
{
    DqnMemAPI *mem_api = DQN_DEFAULT_HEAP_ALLOCATOR;
    int len           = 0;
    int max           = 0;
    char *str         = nullptr;

    DqnString() = default;
    DqnString(char *buf, int max_) : mem_api(nullptr), len(0), str(buf) { max = max_; NullTerminate(); }
    DqnString(char const *str_)                                         { Append(str_); }
    DqnString(char const *str_, int len_)                               { Append(str_, len_); }
    DqnString(DqnSlice<char const> const &other)                        { Append(other.data, other.len); }
    DqnString(DqnSlice<char> const &other)                              { Append(other.data, other.len); }
    DqnString(DqnString const &other)                                   { if (this == &other) return; *this = other; } // TODO(doyle): I can't decide on copy semantics

    DqnString &operator+=(char const *other)                            { Append(other); return *this; }
    DqnString &operator+=(DqnSlice<char const> const &other)            { Append(other.data, other.len); return *this; }
    DqnString &operator+=(DqnSlice<char> const &other)                  { Append(other.data, other.len); return *this; }
    DqnString &operator+=(DqnString const &other)                       { Append(other.str, other.len); return *this; }

    DqnString  operator+ (char const *other)                            { auto result = *this; result.Append(other); return result; }
    DqnString  operator+ (DqnSlice<char const> const &other)            { auto result = *this; result.Append(other.data, other.len); return result; }
    DqnString  operator+ (DqnSlice<char> const &other)                  { auto result = *this; result.Append(other.data, other.len); return result; }
    DqnString  operator+ (DqnString const &other)                       { auto result = *this; result.Append(other.str,  other.len); return result; }

    // Xprintf functions always modifies buffer and null-terminates even with insufficient buffer size.
    // return: The number of characters copied to the buffer
    int  Sprintf         (char const *fmt, ...)                         { va_list va; va_start(va, fmt); int result = VSprintf      (fmt, va); va_end(va); return result; }
    int  SprintfAppend   (char const *fmt, ...)                         { va_list va; va_start(va, fmt); int result = VSprintfAppend(fmt, va); va_end(va); return result; }

    int  VSprintf        (char const *fmt, va_list va)                  { return VSprintfAtOffset(fmt, va, 0  /*offset*/); }
    int  VSprintfAppend  (char const *fmt, va_list va)                  { return VSprintfAtOffset(fmt, va, len/*offset*/); }

    void NullTerminate   () { str[len] = 0; } // NOTE: If you modify the storage directly, this can be handy.
    void Clear           (Dqn::ZeroClear clear = Dqn::ZeroClear::No)    { if (clear == Dqn::ZeroClear::Yes) DqnMem_Set(str, 0, max); len = max = 0; NullTerminate(); }
    void Free            ()                                             { if (str) mem_api->Free(str); str = nullptr; }
    void Resize          (int new_max)                                  { if (new_max > max) Reserve(new_max); len = DQN_MIN(new_max, len); NullTerminate(); }
    void Reserve         (int new_max);

    void Append          (char const *src, int len_ = -1);
    int  VSprintfAtOffset(char const *fmt, va_list va, int offset)      { Reserve(len + VAListLen(fmt, va) + 1); int result = Dqn_vsnprintf(str + offset, max - len, fmt, va); len = (offset + result); return result; }

    static int VAListLen (char const *fmt, va_list va);
    static bool Cmp      (DqnString const *a, DqnString const *b,           Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No) { return (a->len == b->len) && (DqnStr_Cmp(a->str, b->str, a->len, ignore) == 0); }
    static bool Cmp      (DqnString const *a, DqnSlice<char const> const b, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No) { return (a->len == b.len)  && (DqnStr_Cmp(a->str, b.data, b.len, ignore) == 0);  }
    static bool Cmp      (DqnString const *a, DqnSlice<char> const b,       Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No) { return (a->len == b.len)  && (DqnStr_Cmp(a->str, b.data, b.len, ignore) == 0);  }

    // return: -1 if invalid, or if buf_size is 0 the required buffer length in wchar_t characters
    i32      ToWChar(wchar_t *const buf, i32 const buf_size) const;
    // return: String allocated using api.
    wchar_t *ToWChar(DqnMemAPI *const api = DQN_DEFAULT_HEAP_ALLOCATOR) const;

};

// #DqnFixedString Public API - Fixed sized strings at compile time
// =================================================================================================
FILE_SCOPE int DqnFixedString__Append  (char *dest, int dest_size, char const *src, int len = -1);

template <int MAX>
struct DqnFixedString
{
    int  len;
    char str[MAX];

    DqnFixedString(): len(0)                          { this->str[0] = 0; }
    DqnFixedString(char const *str)                   {                     this->len = DqnFixedString__Append(this->str, MAX, str, -1);               }
    DqnFixedString(char const *str, int len)          {                     this->len = DqnFixedString__Append(this->str, MAX, str, len);              }
    DqnFixedString(DqnSlice<char const> const &other) {                     this->len = DqnFixedString__Append(this->str, MAX, other.data, other.len); }
    DqnFixedString(DqnSlice<char> const &other)       {                     this->len = DqnFixedString__Append(this->str, MAX, other.data, other.len); }
    DqnFixedString(DqnFixedString const &other)       { if (this != &other) this->len = DqnFixedString__Append(this->str, MAX, other.str, other.len);  }

    DqnFixedString &operator+=(char const *other)                 { this->len += DqnFixedString__Append(this->str + this->len, MAX - this->len, other); return *this; }
    DqnFixedString &operator+=(DqnSlice<char const> const &other) { this->len += DqnFixedString__Append(this->str + this->len, MAX - this->len, other.data, other.len); return *this; }
    DqnFixedString &operator+=(DqnSlice<char> const &other)       { this->len += DqnFixedString__Append(this->str + this->len, MAX - this->len, other.data, other.len); return *this; }
    DqnFixedString &operator+=(DqnFixedString const &other)       { this->len += DqnFixedString__Append(this->str + this->len, MAX - this->len, other.str, other.len); return *this; }

    DqnFixedString  operator+ (char const *other)                 { auto result = *this; result.len += DqnFixedString__Append(result.str + result.len, MAX - result.len, other); return result; }
    DqnFixedString  operator+ (DqnSlice<char const> const &other) { auto result = *this; result.len += DqnFixedString__Append(result.str + result.len, MAX - result.len, other.data, other.len); return result; }
    DqnFixedString  operator+ (DqnSlice<char> const &other)       { auto result = *this; result.len += DqnFixedString__Append(result.str + result.len, MAX - result.len, other.data, other.len); return result; }
    DqnFixedString  operator+ (DqnFixedString const &other)       { auto result = *this; result.len += DqnFixedString__Append(result.str + result.len, MAX - result.len, other.str, other.len); return result; }

    // Xprintf functions always modifies buffer and null-terminates even with insufficient buffer size.
    // Asserts on failure if DQN_ASSERT is defined.
    // return: The number of characters copied to the buffer
    int  Sprintf         (char const *fmt, ...) { va_list va; va_start(va, fmt); int result = VSprintf      (fmt, va); va_end(va); return result; }
    int  SprintfAppend   (char const *fmt, ...) { va_list va; va_start(va, fmt); int result = VSprintfAppend(fmt, va); va_end(va); return result; }

    int  VSprintf        (char const *fmt, va_list va) { return VSprintfAtOffset(fmt, va, 0  /*offset*/); }
    int  VSprintfAppend  (char const *fmt, va_list va) { return VSprintfAtOffset(fmt, va, len/*offset*/); }

    void NullTerminate   () { str[len] = 0; } // NOTE: If you modify the storage directly, this can be handy.
    void Clear           (Dqn::ZeroClear clear = Dqn::ZeroClear::No) { if (clear == Dqn::ZeroClear::Yes) DqnMem_Set(str, 0, MAX); *this = {}; }

    int  VSprintfAtOffset(char const *fmt, va_list va, int offset) { char *start = str + offset; int result = Dqn_vsnprintf(start, static_cast<int>((str + MAX) - start), fmt, va); len = (offset + result); return result; }
};

using DqnFixedString16   = DqnFixedString<16>;
using DqnFixedString32   = DqnFixedString<32>;
using DqnFixedString64   = DqnFixedString<64>;
using DqnFixedString128  = DqnFixedString<128>;
using DqnFixedString256  = DqnFixedString<256>;
using DqnFixedString512  = DqnFixedString<512>;
using DqnFixedString1024 = DqnFixedString<1024>;
using DqnFixedString2048 = DqnFixedString<2048>;

// #DqnLogger Public API
// =================================================================================================
struct DqnLogger
{
#define LOG_TYPES \
    X(Warning, "WARN ") \
    X(Error, "ERROR") \
    X(Debug, "DEBUG")

#define X(type, prefix) type,
    enum struct Type { LOG_TYPES };
#undef X

#define X(type, prefix) prefix,
    static char const *TypePrefix(Type type)
    {
        LOCAL_PERSIST char const *type_string[] = {LOG_TYPES};
        return type_string[static_cast<i32>(type)];
    }
#undef X
#undef LOG_TYPES

    struct Context
    {
        char *filename;
        int   filename_len;
        char *function;
        int   function_len;
        int   line_number;
    };

    #define DQN_LOGGER_CONTEXT {__FILE__, DQN_CHAR_COUNT(__FILE__), __func__, DQN_CHAR_COUNT(__func__), __LINE__}
    #define DQN_LOGGER_D(logger, fmt, ...) logger.Log(DqnLogger::Type::Debug,   {__FILE__, DQN_CHAR_COUNT(__FILE__), __func__, DQN_CHAR_COUNT(__func__), __LINE__}, fmt, ## __VA_ARGS__)
    #define DQN_LOGGER_W(logger, fmt, ...) logger.Log(DqnLogger::Type::Warning, {__FILE__, DQN_CHAR_COUNT(__FILE__), __func__, DQN_CHAR_COUNT(__func__), __LINE__}, fmt, ## __VA_ARGS__)
    #define DQN_LOGGER_E(logger, fmt, ...) logger.Log(DqnLogger::Type::Error,   {__FILE__, DQN_CHAR_COUNT(__FILE__), __func__, DQN_CHAR_COUNT(__func__), __LINE__}, fmt, ## __VA_ARGS__)

    DqnFixedString1024 log_builder;
    b32                no_console; // Log to console if false.
    b32                no_print_error;
    b32                no_print_debug;
    b32                no_print_warning;

    // Build up a log line that gets prepended to the next log. When Log() is called and is then reset.
    // <file context> <prepend to log> <log message>
    void PrependToLog(char const *fmt, ...) { va_list va; va_start (va, fmt); log_builder.VSprintfAppend(fmt, va); va_end(va); }

    // return: A static string whose lifetime persists until the next log call.
    char const *Log(Type type, Context const log_context, char const *fmt, ...);
};

struct DqnJson
{
    enum struct Type
    {
        Object,
        ArrayOfObjects,
        ArrayOfPrimitives,
    };

    Type           type;
    DqnSlice<char> value;
    i32            num_entries;

    operator bool ()   const { return (value.data != nullptr); }
    bool     IsArray() const { return (type == Type::ArrayOfObjects || type == Type::ArrayOfPrimitives); }
    i64      ToI64()   const { return Dqn_StrToI64(value.data, value.len); }
};

// Zero allocation json finder. Returns the data of the value.
// If array, it returns a slice from [..] not-inclusive, if object, it returns a slice from {..} not-inclusive
// If just name value pair, it returns the literal with quotes or just the value if it is a primitive with quotes.
DQN_FILE_SCOPE DqnJson DqnJson_Get             (char const *buf, i32 buf_len, char const *find_property, i32 find_property_len);
DQN_FILE_SCOPE DqnJson DqnJson_Get             (DqnSlice<char>       const buf, DqnSlice<char>       const find_property);
DQN_FILE_SCOPE DqnJson DqnJson_Get             (DqnSlice<char>       const buf, DqnSlice<char const> const find_property);
DQN_FILE_SCOPE DqnJson DqnJson_Get             (DqnSlice<char const> const buf, DqnSlice<char const> const find_property);
DQN_FILE_SCOPE DqnJson DqnJson_Get             (DqnJson const input, DqnSlice<char const> const find_property);
DQN_FILE_SCOPE DqnJson DqnJson_Get             (DqnJson const input, DqnSlice<char>       const find_property);

// return:   The array item.
DQN_FILE_SCOPE DqnJson DqnJson_GetNextArrayItem(DqnJson *iterator);

#endif  /* DQN_H */

// #XPlatform (Win32 & Unix) API
// =================================================================================================
// Functions in the Cross Platform are guaranteed to be supported in both Unix
// and Win32

// NOTE(doyle): DQN_PLATFORM_HEADER is enabled by the user to have the function prototypes be
// visible. DQN_PLATFORM_H is like a normal header guard that ensures singular declaration of
// functions.
#ifdef DQN_PLATFORM_HEADER

#ifndef DQN_PLATFORM_H
#define DQN_PLATFORM_H

#if defined(DQN_IS_UNIX)
    #include <pthread.h>
    #include <semaphore.h>
#endif

// XPlatform > #DqnOS API
// =================================================================================================
DQN_FILE_SCOPE void *DqnOS_VAlloc(isize size, void *base_addr = nullptr);
DQN_FILE_SCOPE void  DqnOS_VFree (void *address, isize size);

// Uses a single call to DqnMem_Calloc() and DqnMem_Free(). Not completely platform "independent" for Unix.
// num_cores: num_threads_per_core: Can be nullptr, the function will just skip it.
DQN_FILE_SCOPE void DqnOS_GetThreadsAndCores(u32 *const num_cores, u32 *const num_threads_per_core);

// #XPlatform > #DqnVArray Array backed by virtual memory
// =================================================================================================
template<typename T>
struct DqnVArray
{
    isize  len;   // Read
    isize  max;   // Read
    T     *data;  // Read

          DqnVArray  () = default; // Zero is initialisation
          DqnVArray  (isize size)                                 { LazyInit(size); }
    void  LazyInit   (isize size)                                 { if (data) return; len = 0; max = size; data = (T *)DqnOS_VAlloc(max * sizeof(T)); DQN_ALWAYS_ASSERT(data); }
         // ~DqnVArray  ()                                           { if (data) DqnOS_VFree(data, sizeof(T) * max); }

    void  Clear      (Dqn::ZeroClear clear = Dqn::ZeroClear::No)  { if (data) { len = 0; if (clear == Dqn::ZeroClear::Yes) DqnMem_Clear(data, 0, sizeof(T) * max); } }
    void  Free       ()                                           { if (data) { DqnOS_VFree(data, sizeof(T) * max); } *this = {}; }
    T    *Front      ()                                           { return (len > 0) ? (data + 0)           : nullptr; }
    T    *Back       ()                                           { return (len > 0) ? (data + (len - 1)) : nullptr; }
    T    *Make       (isize num = 1)                              { LazyInit(1024); len += num; DQN_ASSERT(len <= max); return &data[len - num]; }
    T    *Push       (T const &v)                                 { return Insert(len, &v, 1); }
    T    *Push       (T const *v, isize num_items = 1)             { return Insert(len,  v, num_items); }
    void  Pop        ()                                           { if (len > 0) len--; }
    void  Erase      (isize index)                                { if (!data) return; DQN_ASSERT(index >= 0 && index < len); data[index] = data[--len]; }
    void  EraseStable(isize index);
    T    *Insert     (isize index, T const *v)                    { return Insert(index,  v, 1); }
    T    *Insert     (isize index, T const &v)                    { return Insert(index, &v, 1); }
    T    *Insert     (isize index, T const *v, isize num_items);
    bool  Contains   (T const *v) const                           { T const *ptr = data;  T const *end = data + len; while (ptr < end) { if (*ptr++ == *v) return true; } return false; }

    T    &operator[] (isize i) const                              { DQN_ASSERT(i < len && i > 0); return this->data[i]; }
    T    *begin      ()                                           { return data; }
    T    *end        ()                                           { return data + len; }
};

template<typename T> T *DqnVArray<T>::Insert(isize index, T const *v, isize num_items)
{
    LazyInit(1024);

    index                = DQN_CLAMP(index, 0, len);
    isize const new_len = len + num_items;
    DQN_ASSERT(new_len <= max);

    T *src  = data + index;
    T *dest = src + num_items;

    if (src < dest)
        memmove(dest, src, ((data + len) - src) * sizeof(T));

    len = new_len;
    for (isize i = 0; i < num_items; i++)
        src[i] = v[i];

    return src;
}

template <typename T> void DqnVArray<T>::EraseStable(isize index)
{
    if (!data) return;
    DQN_ASSERT(index >= 0 && index < len);
    isize const off = (data + index) - data;
    memmove(data + off, data + off + 1, ((usize)len - (usize)off - 1) * sizeof(T));
    len--;
}

// #XPlatform > #DqnVHashTable API
// =================================================================================================
template <typename Key> using DqnVHashTableHashingProc = isize(*)(isize count, Key const &data);
template <typename Key> using DqnVHashTableEqualsProc  = bool (*)(Key const &a, Key const &b);

const u64 DQN_VHASH_TABLE_DEFAULT_SEED = 0x9747B28CAB3F8A7B;
#define DQN_VHASH_TABLE_HASHING_PROC(name, Type) inline isize name(isize count, Type const &key)
#define DQN_VHASH_TABLE_EQUALS_PROC(name, Type) inline bool name(Type const &a, Type const &b)

template <typename T> DQN_VHASH_TABLE_HASHING_PROC(DqnVHashTableDefaultHash, T)   { return DqnHash_Murmur64Seed(&key, sizeof(key), DQN_VHASH_TABLE_DEFAULT_SEED) % count; }
template <typename T> DQN_VHASH_TABLE_EQUALS_PROC (DqnVHashTableDefaultEquals, T) { return (DqnMem_Cmp(&a, &b, sizeof(a)) == 0); }

// TODO(doyle): Fix this so we don't have to manually declare the fixed string sizes for hashing and equals
#define DQN_VHASH_TABLE_DEFAULT_FIXED_STRING_PROCS(StringCapacity)                                 \
    template <>                                                                                    \
    DQN_VHASH_TABLE_HASHING_PROC(DqnVHashTableDefaultHash<DqnFixedString<StringCapacity>>,         \
                                 DqnFixedString<StringCapacity>)                                   \
    {                                                                                              \
        return DqnHash_Murmur64Seed(key.str, key.len, DQN_VHASH_TABLE_DEFAULT_SEED) % count;       \
    }                                                                                              \
    template <>                                                                                    \
    DQN_VHASH_TABLE_EQUALS_PROC(DqnVHashTableDefaultEquals<DqnFixedString<StringCapacity>>,        \
                                DqnFixedString<StringCapacity>)                                    \
    {                                                                                              \
        return (a.len == b.len) && (DqnStr_Cmp(a.str, b.str, a.len, Dqn::IgnoreCase::No) == 0);    \
    }

DQN_VHASH_TABLE_DEFAULT_FIXED_STRING_PROCS(1024)
DQN_VHASH_TABLE_DEFAULT_FIXED_STRING_PROCS(512)
DQN_VHASH_TABLE_DEFAULT_FIXED_STRING_PROCS(256)
DQN_VHASH_TABLE_DEFAULT_FIXED_STRING_PROCS(128)
DQN_VHASH_TABLE_DEFAULT_FIXED_STRING_PROCS(64)
DQN_VHASH_TABLE_DEFAULT_FIXED_STRING_PROCS(32)
DQN_VHASH_TABLE_DEFAULT_FIXED_STRING_PROCS(16)
DQN_VHASH_TABLE_DEFAULT_FIXED_STRING_PROCS(8)


#define DQN_VHASH_TABLE_TEMPLATE                                                                   \
    template <typename Key,                                                                        \
              typename Item,                                                                       \
              DqnVHashTableHashingProc<Key> Hash,                                                  \
              DqnVHashTableEqualsProc<Key> Equals>

#define DQN_VHASH_TABLE_DECL DqnVHashTable<Key, Item, Hash, Equals>

template <typename Key,
          typename Item,
          DqnVHashTableHashingProc<Key> Hash  = DqnVHashTableDefaultHash<Key>,
          DqnVHashTableEqualsProc<Key> Equals = DqnVHashTableDefaultEquals<Key>>
struct DqnVHashTable
{
    struct Entry
    {
        union { Key  key;  Key  first; };
        union { Item item; Item second; };
    };

    struct Bucket
    {
        Entry entries[4];
        isize entryIndex;
    };

    Bucket    *buckets;
    isize      numBuckets;
    isize     *indexesOfUsedBuckets;
    isize      indexInUsedBuckets;

    DqnVHashTable       () = default;
    DqnVHashTable       (isize size)                                                    { LazyInit(size); }

    void       LazyInit (isize size = DQN_MAX(DQN_MEGABYTE(1)/sizeof(Bucket), 1024) );
    void       Free     ()                                  { if (buckets) DqnOS_VFree(buckets, sizeof(buckets) * numBuckets); *this = {}; }

    void       Erase    (Key const &key);                           // Delete the element matching key, does nothing if key not found.
    Entry     *GetEntry (Key const &key);                           // return: The (key, item) entry associated with the key, nullptr if key not in table yet.
    Item      *GetOrMake(Key const &key, bool *existed = nullptr);  // return: Item if found, otherwise make an entry (key, item) and return the ptr to the uninitialised item
    Item      *Get      (Key const &key)                   { Entry *entry  = GetEntry(key); return (entry) ? &entry->item : nullptr; }
    Item      *Set      (Key const &key, Item const &item) { Item  *result = GetOrMake(key); *result = item; return result; }

    Item      *operator[](Key const &key)                  { return Get(key); }

    struct Iterator
    {
        Entry  *entry;
        Iterator(DqnVHashTable *table_, isize indexInUsedBuckets_ = 0, isize indexInBucket_ = 0);
        Bucket *GetCurrBucket() const { return (table->buckets + table->indexesOfUsedBuckets[indexInUsedBuckets]); }
        Entry  *GetCurrEntry()  const { return GetCurrBucket()->entries + indexInBucket; }
        Item   *GetCurrItem ()  const { return &(GetCurrEntry()->item); }

        bool      operator!=(Iterator const &other) const { return !(indexInUsedBuckets == other.indexInUsedBuckets && indexInBucket == other.indexInBucket); }
        Entry    &operator* ()                      const { return *GetCurrEntry(); }
        Iterator &operator++();
        Iterator &operator--()                            { if (--indexInBucket < 0) { indexInBucket = 0; indexInUsedBuckets = DQN_MAX(--indexInUsedBuckets, 0); } entry = GetCurrEntry(); return *this; }
        Iterator  operator++(int)                         { Iterator result = *this; ++(*this); return result; }
        Iterator  operator--(int)                         { Iterator result = *this; --(*this); return result; }
        Iterator  operator+ (int offset)            const { Iterator result = *this; DQN_FOR_EACH(i, DQN_ABS(offset)) { (offset > 0) ? ++result : --result; } return result; } // TODO(doyle): Improve
        Iterator  operator- (int offset)            const { Iterator result = *this; DQN_FOR_EACH(i, DQN_ABS(offset)) { (offset > 0) ? --result : ++result; } return result; } // TODO(doyle): Improve

    private:
        DqnVHashTable *table;
        isize          indexInUsedBuckets;
        isize          indexInBucket;
    };

    Iterator   Begin() { return begin(); }
    Iterator   End()   { return end(); }
    Iterator   begin() { return Iterator(this); }
    Iterator   end()   { return Iterator(this, numBuckets, DQN_ARRAY_COUNT(this->buckets[0].entries)); }
};

DQN_VHASH_TABLE_TEMPLATE DQN_VHASH_TABLE_DECL::Iterator::Iterator(DqnVHashTable *table_,
                                                                  isize indexInUsedBuckets_,
                                                                  isize indexInBucket_)
: table(table_)
, indexInUsedBuckets(indexInUsedBuckets_)
, indexInBucket(indexInBucket_)
, entry(nullptr)
{
    bool sentinelIndex = (indexInUsedBuckets == table->numBuckets &&
                          indexInBucket == DQN_ARRAY_COUNT(table->buckets[0].entries));
    bool emptyTable = (table->indexInUsedBuckets == 0);
    if (emptyTable || sentinelIndex)
    {
        if (emptyTable)
        {
            this->indexInUsedBuckets = table->numBuckets;
            this->indexInBucket      = DQN_ARRAY_COUNT(table->buckets[0].entries);
        }
    }
    else
    {
        entry = GetCurrEntry();
    }
}

DQN_VHASH_TABLE_TEMPLATE typename DQN_VHASH_TABLE_DECL::Iterator &DQN_VHASH_TABLE_DECL::Iterator::operator++()
{
    if (++indexInBucket >= GetCurrBucket()->entryIndex)
    {
        indexInBucket = 0;
        indexInUsedBuckets++;
    }

    if (indexInUsedBuckets < table->indexInUsedBuckets)
        entry = GetCurrEntry();
    else
        *this = table->end();

    return *this;
}

DQN_VHASH_TABLE_TEMPLATE void DQN_VHASH_TABLE_DECL::LazyInit(isize size)
{
    *this                      = {};
    this->numBuckets           = size;
    this->buckets              = static_cast<Bucket *>(DqnOS_VAlloc(size * sizeof(*this->buckets)));
    this->indexesOfUsedBuckets = static_cast<isize *> (DqnOS_VAlloc(size * sizeof(*this->indexesOfUsedBuckets)));
    DQN_ASSERT(this->buckets && this->indexesOfUsedBuckets);
}

DQN_VHASH_TABLE_TEMPLATE typename DQN_VHASH_TABLE_DECL::Entry *
DQN_VHASH_TABLE_DECL::GetEntry(Key const &key)
{
    if (!buckets) return nullptr;

    isize index    = Hash(this->numBuckets, key);
    Bucket *bucket = this->buckets + index;

    Entry *result = nullptr;
    for (isize i = 0; i < bucket->entryIndex && !result; i++)
    {
        Entry *entry = bucket->entries + i;
        result       = Equals(entry->key, key) ? entry : nullptr;
    }

    return result;
}

DQN_VHASH_TABLE_TEMPLATE Item *DQN_VHASH_TABLE_DECL::GetOrMake(Key const &key, bool *existed)
{
    if (!this->buckets) LazyInit();

    isize index    = Hash(this->numBuckets, key);
    Bucket *bucket = this->buckets + index;

    Entry *entry = nullptr;
    for (isize i = 0; i < bucket->entryIndex && !entry; i++)
    {
        Entry *check = bucket->entries + i;
        entry        = Equals(check->key, key) ? check : nullptr;
    }

    if (existed)
        *existed = (entry != nullptr);

    if (!entry)
    {
        DQN_ALWAYS_ASSERTM(bucket->entryIndex < DQN_ARRAY_COUNT(bucket->entries),
                           "More than %zu collisions in hash table, increase the size of the table or buckets",
                           DQN_ARRAY_COUNT(bucket->entries));

        if (bucket->entryIndex == 0)
            this->indexesOfUsedBuckets[this->indexInUsedBuckets++] = index;

        entry      = bucket->entries + bucket->entryIndex++;
        entry->key = key;

        // TODO(doyle): A maybe case. We're using virtual memory, so you should
        // just initialise a larger size. It's essentially free ... maybe one
        // day we care about resizing the table but at the cost of a lot more code
        // complexity.
        isize const threshold = static_cast<isize>(0.75f * this->numBuckets);
        DQN_ALWAYS_ASSERTM(this->indexInUsedBuckets < threshold, "%zu >= %zu", this->indexInUsedBuckets, threshold);
    }

    Item *result = &entry->item;
    return result;
}

DQN_VHASH_TABLE_TEMPLATE void DQN_VHASH_TABLE_DECL::Erase(Key const &key)
{
    if (!buckets)
        return;

    isize index    = Hash(this->numBuckets, key);
    Bucket *bucket = this->buckets + index;

    DQN_FOR_EACH(i, bucket->entryIndex)
    {
        Entry *check  = bucket->entries + i;
        if (Equals(check->key, key))
        {
            for (isize j = i; j < (bucket->entryIndex - 1); ++j)
                bucket->entries[j] = bucket->entries[j + 1];

            if (--bucket->entryIndex == 0)
            {
                DQN_FOR_EACH(bucketIndex, this->indexInUsedBuckets)
                {
                    if (this->indexesOfUsedBuckets[bucketIndex] == index)
                    {
                        indexesOfUsedBuckets[bucketIndex] =
                            indexesOfUsedBuckets[--this->indexInUsedBuckets];
                    }
                }
            }

            DQN_ASSERT(this->indexInUsedBuckets >= 0);
            DQN_ASSERT(bucket->entryIndex >= 0);
            return;
        }
    }
}

// XPlatform > #DqnFile API
// =================================================================================================
struct DqnFile
{
    enum Flag
    {
        FileRead      = (1 << 0),
        FileWrite     = (1 << 1),
        Execute       = (1 << 2),
        All           = (1 << 3),
        FileReadWrite = FileRead | FileWrite
    };

    enum struct Action
    {
        OpenOnly,         // Only open file if it exists. Fails and returns false if file did not exist or could not open.
        CreateIfNotExist, // Try and create file. Return true if it was able to create. If it already exists, this fails.
        ClearIfExist,     // Clear the file contents to zero if it exists. Fails and returns false if file does not exist.
        ForceCreate,      // Always create, even if it exists
    };

    u32    flags;
    void  *handle;
    usize  size;

    // API
    // ==============================================================================================
    // NOTE: W(ide) versions of functions only work on Win32, since Unix is already UTF-8 compatible.

    // Open a handle for file read and writing. Deleting files does not need a handle. Handles should be
    // closed before deleting files otherwise the OS may not be able to delete the file.
    // return: FALSE if invalid args or failed to get handle (i.e. insufficient permission)
    bool   Open(char    const *path, u32 const flags_, Action const action);
    bool   Open(wchar_t const *path, u32 const flags_, Action const action);

    // fileOffset: The byte offset to starting writing from.
    // return:     The number of bytes written. 0 if invalid args or it failed to write.
    usize  Write(u8 const *buf, usize const num_bytes_to_write, usize const fileOffset);

    // IMPORTANT: You may want to allocate size+1 for null-terminating the file contents when reading into a buffer.
    // return: The number of bytes read. 0 if invalid args or it failed to read.
    usize  Read (u8 *buf, usize const num_bytes_to_read);

    // File close invalidates the handle after it is called.
    void   Close();
};

struct DqnFileInfo
{
    usize size;
    u64   create_time_in_s;
    u64   last_write_time_in_s;
    u64   last_access_time_in_s;
};

// Read entire file into the given buffer. To determine required buf_size size, use GetFileSize.
// NOTE: You want size + 1 and add the null-terminator yourself if you want a null terminated buffer.
// bytes_read: Pass in to get how many bytes of the buf was used. Basically the return value of Read
// return:    False if insufficient buf_size OR file access failure OR nullptr arguments.
DQN_FILE_SCOPE bool   DqnFile_ReadAll(char    const *path, u8 *buf, usize buf_size);
DQN_FILE_SCOPE bool   DqnFile_ReadAll(wchar_t const *path, u8 *buf, usize buf_size);

// Buffer is null-terminated and should be freed when done with.
// return: False if file access failure OR nullptr arguments.
DQN_FILE_SCOPE u8    *DqnFile_ReadAll(char    const *path, usize *buf_size, DqnMemAPI *api = DQN_DEFAULT_HEAP_ALLOCATOR);
DQN_FILE_SCOPE u8    *DqnFile_ReadAll(wchar_t const *path, usize *buf_size, DqnMemAPI *api = DQN_DEFAULT_HEAP_ALLOCATOR);
DQN_FILE_SCOPE u8    *DqnFile_ReadAll(wchar_t const *path, usize *buf_size, DqnMemStack *stack, DqnMemStack::AllocTo allocTo = DqnMemStack::AllocTo::Head);
DQN_FILE_SCOPE u8    *DqnFile_ReadAll(char    const *path, usize *buf_size, DqnMemStack *stack, DqnMemStack::AllocTo allocTo = DqnMemStack::AllocTo::Head);

DQN_FILE_SCOPE bool  DqnFile_WriteAll(char    const *path, u8 const *buf, usize const buf_size);
DQN_FILE_SCOPE bool  DqnFile_WriteAll(wchar_t const *path, u8 const *buf, usize const buf_size);

// return: False if file access failure
DQN_FILE_SCOPE bool   DqnFile_Size(char    const *path, usize *size);
DQN_FILE_SCOPE bool   DqnFile_Size(wchar_t const *path, usize *size);

DQN_FILE_SCOPE bool   DqnFile_MakeDir(char const *path);

// info:   Pass in to fill with file attributes.
// return: False if file access failure
DQN_FILE_SCOPE bool   DqnFile_GetInfo(char    const *path, DqnFileInfo *info);
DQN_FILE_SCOPE bool   DqnFile_GetInfo(wchar_t const *path, DqnFileInfo *info);

// NOTE: You can't delete a file unless the handle has been closed to it on Win32.
// return: False if file access failure
DQN_FILE_SCOPE bool   DqnFile_Delete (char    const *path);
DQN_FILE_SCOPE bool   DqnFile_Delete (wchar_t const *path);
DQN_FILE_SCOPE bool   DqnFile_Copy   (char    const *src, char    const *dest);
DQN_FILE_SCOPE bool   DqnFile_Copy   (wchar_t const *src, wchar_t const *dest);

// NOTE: Win32: Current directory is "*", Unix: "."
// num_files: Pass in a ref to a i32. The function fills it out with the number of entries.
// return:   An array of strings of the files in the directory in UTF-8. The directory lisiting is
//           allocated with malloc and must be freed using free() or the helper function ListDirFree()
DQN_FILE_SCOPE char **DqnFile_ListDir       (char const *dir, i32 *num_files, DqnMemAPI *api = DQN_DEFAULT_HEAP_ALLOCATOR);
DQN_FILE_SCOPE void   DqnFile_ListDirFree   (char **file_list, i32 num_files,  DqnMemAPI *api = DQN_DEFAULT_HEAP_ALLOCATOR);

struct DqnSmartFile : public DqnFile
{
    ~DqnSmartFile() { this->Close(); }
};

// XPlatform > #DqnCatalog API
// =================================================================================================
using DqnCatalogPath = DqnFixedString1024;
template <typename T> using DqnCatalogLoadProc = bool (*)(DqnCatalogPath const &file, T *data);
#define DQN_CATALOG_LOAD_PROC(name, type) bool name(DqnCatalogPath const &file, type *data)

#define DQN_CATALOG_TEMPLATE template <typename T, DqnCatalogLoadProc<T> LoadAsset>
#define DQN_CATALOG_DECL DqnCatalog<T, LoadAsset>

#if 0
struct RawBuf { char *buffer; int len; };
DQN_CATALOG_LOAD_PROC(CatalogRawLoad, RawBuf)
{
    size_t buf_size;
    uint8_t *buf = DqnFile_ReadAll(file.str, &buf_size);
    if (!buf) return false;
    data->buffer = reinterpret_cast<char *>(buf);
    data->len    = static_cast<int>(buf_size);
    return true;
}

int main(int, char)
{
    DqnCatalog<RawBuf, CatalogRawLoad> catalog = {};
    RawBuf *file = catalog.GetIfUpdated("path/to/file/");
    if (file) { (void)file; // do work on file }
    else      { // file not updated since last query }

    while (true) // Or event loop, poll the assets in the catalog
    {
        catalog.PollAssets();
    }
    catalog.Free();
}
#endif

DQN_CATALOG_TEMPLATE struct DqnCatalog
{
    struct Entry
    {
        T    data;
        u64  last_write_time_in_s;
        bool updated;
    };

    DqnVHashTable<DqnCatalogPath, Entry> asset_table;

    // Adds the file to the catalog if it has not been added yet.
    // return: Asset if an update has been detected and not consumed yet otherwise nullptr. Update is consumed after called.
    T     *GetIfUpdated(DqnCatalogPath const &file);
    Entry *GetEntry    (DqnCatalogPath const &file) { Entry *entry = asset_table.Get(file); return entry; }
    T     *Get         (DqnCatalogPath const &file) { Entry *entry = asset_table.Get(file); return (entry) ? &entry->data : nullptr; }
    void   Erase       (DqnCatalogPath const &file) { asset_table.Erase(file); };

    // return: Iterate all loaded assets for updates, true if atleast 1 asset was updated.
    bool   PollAssets  ();
    void   Free        () { asset_table.Free(); }

    // NOTE: Unlikely you will need to use. Prefer GetIfUpdated.
    // Manually invoke an update on the entry by querying its last write time on disk and updating accordingly.
    bool QueryAndUpdateAsset(DqnCatalogPath const &file, Entry *entry);
};

DQN_CATALOG_TEMPLATE bool DQN_CATALOG_DECL::QueryAndUpdateAsset(DqnCatalogPath const &file, Entry *entry)
{
    DqnFileInfo info = {};
    if (!DqnFile_GetInfo(file.str, &info))
    {
        DQN_LOGE("Catalog could not get file info for: %s\n", file.str);
        return false;
    }

    if (entry->last_write_time_in_s == info.last_write_time_in_s)
        return true;

    T newData = {};
    if (LoadAsset(file, &newData))
    {
        entry->last_write_time_in_s = info.last_write_time_in_s;
        entry->data             = newData;
        entry->updated          = true;
    }
    else
    {
        DQN_LOGE("Catalog could not load file: %s\n", file.str);
        return false;
    }

    return true;
}

DQN_CATALOG_TEMPLATE T *DQN_CATALOG_DECL::GetIfUpdated(DqnCatalogPath const &file)
{
    Entry *entry = this->asset_table.GetOrMake(file);
    if (QueryAndUpdateAsset(file, entry))
    {
        if (entry->updated) entry->updated = false;
        else                entry          = nullptr;
    }
    else
    {
        entry = nullptr;
    }

    return &entry->data;
}

DQN_CATALOG_TEMPLATE bool DQN_CATALOG_DECL::PollAssets()
{
    bool result = false;
    for (auto it = this->asset_table.Begin(); it != this->asset_table.End(); ++it)
    {
        DqnCatalogPath const *file = &it.entry->key;
        Entry *entry               = &it.entry->item;
        result |= QueryAndUpdateAsset(*file, entry);
    }

    return result;
}


// XPlatform > #DqnTimer API
// =================================================================================================
DQN_FILE_SCOPE f64  DqnTimer_NowInMs();
DQN_FILE_SCOPE f64  DqnTimer_NowInS ();

// XPlatform > #DqnLock API
// =================================================================================================
struct DqnLock
{
#if defined(DQN_IS_WIN32)
    CRITICAL_SECTION win32_handle;
#else
    pthread_mutex_t  unix_handle;
#endif

    // Win32 only, when trying to acquire a locked lock, it is the number of spins permitted
    // spinlocking on the lock before being blocked. Set before init if you want a different value.
    u32 win32_spin_count = 16000;

    bool Init   ();
    void Acquire();
    void Release();
    void Delete ();

    struct Guard_
    {
         Guard_(DqnLock *lock_) : lock(lock_) { lock->Acquire(); }
        ~Guard_()                             { lock->Release(); }

    private:
        DqnLock *lock;
    };

    // Create a lock guard on the lock this is invoked on.
    Guard_ Guard() { return Guard_(this); }
};

// XPlatform > #DqnJobQueue API
// =================================================================================================
// DqnJobQueue is a platform abstracted "lockless" multithreaded work queue. It will create threads
// and assign threads to complete the job via the job "callback" using the "user_data" supplied.

// Usage
// 1. Prepare your callback function for threads to execute following the 'DqnJob_Callback' function
//    signature.
// 2. Create a job queue with DqnJobQueue_InitWithMem()
// 3. Add jobs with DqnJobQueue_AddJob() and threads will be dispatched automatically.

// When all jobs are sent you can also utilise the main executing thread to complete jobs whilst you
// wait for all jobs to complete using DqnJobQueue_TryExecuteNextJob() or spinlock on
// DqnJobQueue_AllJobsComplete(). Alternatively you can combine both for the main thread to help
// complete work and not move on until all tasks are complete.

typedef void DqnJob_Callback(struct DqnJobQueue *const queue, void *const user_data);
struct DqnJob
{
    DqnJob_Callback *callback;
    void            *user_data;
};

struct DqnJobQueue
{
    // JobList Circular Array, is setup in Init()
    DqnJob *job_list;
    u32     size;

    // NOTE(doyle): Modified by main+worker threads
    i32   volatile jobToExecuteIndex;
    i32   volatile num_jobs_queued;

#if defined(DQN_IS_WIN32)
    void *semaphore;
#else
    sem_t semaphore;
#endif

    // NOTE: Modified by main thread ONLY
    i32 volatile jobInsertIndex;

    bool Init             (DqnJob *const job_list_, const u32 job_list_size, const u32 num_threads);
    bool AddJob           (const DqnJob job);

    void BlockAndCompleteAllJobs();
    bool TryExecuteNextJob();
    bool AllJobsComplete  ();
};

// TODO(doyle): Queue delete, thread delete

// queue:       Pass a pointer to a zero cleared DqnJobQueue struct
// job_list:     Pass in a pointer to an array of DqnJob's
// job_list_size: The number of elements in the job_list array
// num_threads:  The number of threads the queue should request from the OS for working on the queue
// return:      FALSE if invalid args i.e. nullptr ptrs or job_list_size & num_threads == 0
DQN_FILE_SCOPE bool DqnJobQueue_Init(DqnJobQueue *const queue, const DqnJob *const job_list,
                                     const u32 job_list_size, const u32 num_threads);

// return: FALSE if the job is not able to be added, this occurs if the queue is full.
DQN_FILE_SCOPE bool DqnJobQueue_AddJob(DqnJobQueue *const queue, const DqnJob job);

// Helper function that combines TryExecuteNextJob() and AllJobsComplete(), i.e.
// complete all work before moving on. Does nothing if queue is nullptr.
DQN_FILE_SCOPE void DqnJobQueue_BlockAndCompleteAllJobs(DqnJobQueue *const queue);

// return: TRUE if there was a job to execute (the calling thread executes it). FALSE if it could
//         not get a job. It may return FALSE whilst there are still jobs, this means that another thread
//         has taken the job before the calling thread could and should NOT be used to determine if there
//         are any remaining jobs left. That can only be definitively known using
//         DqnJobQueue_AllJobsComplete(). This is typically combined like so ..
//         while (DqnJobQueue_TryExecuteNextJob(queue) || !DqnJobQueue_AllJobsComplete(queue));
//         Return FALSE also if queue is a nullptr pointer.
DQN_FILE_SCOPE bool DqnJobQueue_TryExecuteNextJob(DqnJobQueue *const queue);
DQN_FILE_SCOPE bool DqnJobQueue_AllJobsComplete  (DqnJobQueue *const queue);

// XPlatform > #DqnAtomic API
// =================================================================================================
// All atomic operations generate a full read/write barrier. This is implicitly enforced by the
// OS calls, not explicitly in my code.

// swap_val:    The value to put into "dest", IF at point of read, "dest" has the value of "compare_val"
// compare_val: The value to check in "dest"
// return:     Return the original value that was in "dest"
DQN_FILE_SCOPE i32 DqnAtomic_CompareSwap32(i32 volatile *const dest, const i32 swap_val, const i32 compare_val);

// Add "value" to src
// return: The new value at src
DQN_FILE_SCOPE i32 DqnAtomic_Add32(i32 volatile *const src, const i32 value);

// #Platform Specific API
// =================================================================================================
// Functions here are only available for the #defined sections (i.e. all functions in
// DQN_IS_WIN32 only have a valid implementation in Win32.

#if defined(DQN_IS_WIN32)
// Platform > #DqnWin32 API
// =================================================================================================
#define DQN__WIN32_ERROR_BOX(text, title) MessageBoxA(nullptr, text, title, MB_OK);

// The function automatically null-terminates the output string.
// out:    A pointer to the buffer to receive the characters.
// out_len: The length/capacity of the buffer "out". If 0, the function returns the required length including null terminator.
// return: -1 if invalid, or if out_len is 0 the required buffer length.
DQN_FILE_SCOPE i32 DqnWin32_UTF8ToWChar(const char    *const in, wchar_t *const out, const i32 out_len);
DQN_FILE_SCOPE i32 DqnWin32_WCharToUTF8(const wchar_t *const in, char    *const out, const i32 out_len);

// "width" and "height" are optional and won't be used if not given by user.
// width & height: Pass in a pointer for function to fill out.
DQN_FILE_SCOPE void DqnWin32_GetClientDim     (HWND const window, LONG *width, LONG *height);
DQN_FILE_SCOPE void DqnWin32_GetRectDim       (RECT const rect,   LONG *width, LONG *height);

DQN_FILE_SCOPE char const *DqnWin32_GetLastError();

// TODO(doyle): #deprecate #delete Display Last Error is mostly useless.
// Displays error in the format <err_prefix>: <Win32 Error Message> in a Win32 Dialog Box.
// err_prefix: The message before the Win32 error, can be nullptr
DQN_FILE_SCOPE void DqnWin32_DisplayLastError (const char *const err_prefix);

// Asimilar to DqnWin32_DisplayLastError() a particular error can be specified in a Win32 Dialog Box.
DQN_FILE_SCOPE void DqnWin32_DisplayErrorCode (const DWORD error, const char *const err_prefix);

// Output text to the debug console. For visual studio this is the output window and not the console.
// ...: Variable args alike printf, powered by stb_sprintf
DQN_FILE_SCOPE void DqnWin32_OutputDebugString(const char *const fmt_str, ...);

// Get the full path of to the current processes executable, and return the char offset in the
// string to the last backslash, i.e. the directory.
// buf:    Filled with the path to the executable file.
// return: The offset to the last backslash. -1 if buf_len was not large enough or buf is null. (i.e.
//         buf + offsetToLastSlash + 1, gives C:/Path/)
DQN_FILE_SCOPE i32  DqnWin32_GetEXEDirectory(char *const buf, const u32 buf_len);
#endif // DQN_IS_WIN32
#endif // DQN_PLATFORM_H
#endif // DQN_PLATFORM_HEADER


#ifdef DQN_IMPLEMENTATION
// DQN_IMPLEMENTATION
// =================================================================================================
#include <math.h>    // TODO(doyle): For trigonometry functions (for now)
#include <stdlib.h>  // For calloc, malloc, free
#include <stdio.h>   // For printf

// NOTE: STB_SPRINTF is included when DQN_IMPLEMENTATION defined
// #define STB_SPRINTF_IMPLEMENTATION

// NOTE: DQN_INI_IMPLEMENTATION modified to be included when DQN_IMPLEMENTATION defined
// #define DQN_INI_IMPLEMENTATION
#define DQN_INI_STRLEN(s) DqnStr_Len(s)

// #DqnLog
// =================================================================================================
DQN_FILE_SCOPE void DqnLog(char const *file, char const *function_name, i32 line_num,
                           char const *msg, ...)
{
    // TODO(doyle): Compress this
    auto file_len = DqnStr_Len(file);
    for (auto i = file_len - 1; i >= 0; i--)
    {
        if (file[i] == '\\' || file[i] == '/')
        {
            file = file + i + 1;
            break;
        }
    }

    char user_msg[2048];
    user_msg[0] = '\0';

    va_list va;
    va_start(va, msg);
    {
        u32 num_copied = Dqn_vsprintf(user_msg, msg, va);
        if (num_copied > DQN_ARRAY_COUNT(user_msg))
        {
            (*((int *)0)) = 0;
        }
    }
    va_end(va);

    char const *const fmt_str = "%s:%s,%d: DqnLog: %s\n";
    fprintf(stderr, fmt_str, file, function_name, line_num, user_msg);

    #if defined(DQN_PLATFORM_IMPLEMENTATION) && defined(DQN_IS_WIN32)
        DqnWin32_OutputDebugString(fmt_str, file, function_name, line_num, user_msg);
    #endif
}

DQN_FILE_SCOPE void DqnLogExpr(char const *file, char const *function_name, i32 line_num,
                               char const *expr, char const *msg, ...)
{
    auto file_len = DqnStr_Len(file);
    for (isize i = file_len - 1; i >= 0; i--)
    {
        if (file[i] == '\\' || file[i] == '/')
        {
            file = file + i + 1;
            break;
        }
    }

    char user_msg[2048];
    user_msg[0] = '\0';

    va_list va;
    va_start(va, msg);
    {
        u32 num_copied = Dqn_vsprintf(user_msg, msg, va);
        if (num_copied > DQN_ARRAY_COUNT(user_msg))
        {
            (*((int *)0)) = 0;
        }
    }
    va_end(va);

    char const *const fmt_str = ":%s:%s,%d(%s): DqnLog: %s\n";
    fprintf(stderr, fmt_str, file, function_name, line_num, expr, user_msg);

    #if defined(DQN_PLATFORM_IMPLEMENTATION) && defined(DQN_IS_WIN32)
        DqnWin32_OutputDebugString(fmt_str, file, function_name, line_num, expr, user_msg);
    #endif
}

// #DqnMemory
// =================================================================================================
// NOTE: All memory allocations in dqn.h go through these functions. So they can
// be rerouted fairly easily especially for platform specific mallocs.
DQN_FILE_SCOPE void *DqnMem_Alloc(usize size)
{
    void *result = malloc(size);
    return result;
}

DQN_FILE_SCOPE void *DqnMem_Calloc(usize size)
{
    void *result = calloc(1, size);
    return result;
}

DQN_FILE_SCOPE void DqnMem_Clear(void *memory, u8 clear_val, usize size)
{
    if (memory)
    {
        DqnMem_Set(memory, clear_val, size);
    }
}

DQN_FILE_SCOPE void *DqnMem_Realloc(void *memory, usize new_size)
{
    void *result = realloc(memory, new_size);
    return result;
}

DQN_FILE_SCOPE void DqnMem_Free(void *memory)
{
    if (memory) free(memory);
}

DQN_FILE_SCOPE void DqnMem_Copy(void *dest, void const *src, usize num_bytes_to_copy)
{
    auto *to   = (u8 *)dest;
    auto *from = (u8 *)src;
    for (usize i = 0; i < num_bytes_to_copy; i++)
        to[i]   = from[i];
}

DQN_FILE_SCOPE void *DqnMem_Set(void *dest, u8 value, usize num_bytes_to_set)
{
    auto volatile *ptr = (u8 *)dest; // NOTE: Volatile so memset is not optimised out.
    for (usize i = 0; i < num_bytes_to_set; i++)
        ptr[i]  = value;

    return dest;
}

DQN_FILE_SCOPE int DqnMem_Cmp(void const *src, void const *dest, usize num_bytes)
{
    auto const *src_ptr  = static_cast<char const *>(src);
    auto const *dest_ptr = static_cast<char const *>(dest);

    usize i;
    for (i = 0; i < num_bytes; ++i)
    {
        if (src_ptr[i] != dest_ptr[i])
            break;
    }

    i = DQN_MIN(i, (num_bytes - 1));
    return (src_ptr[i] - dest_ptr[i]);
}

// #DqnMemAPI
// =================================================================================================
FILE_SCOPE void DqnMemAPI__ValidateRequest(DqnMemAPI::Request request_)
{
    DQN_ASSERT(request_.type != DqnMemAPI::Type::Uninitialised);

    if (request_.type == DqnMemAPI::Type::Alloc)
    {
        auto *request = &request_.e.alloc;
        DQN_ASSERT(request->request_size > 0);
        return;
    }

    if (request_.type == DqnMemAPI::Type::Realloc)
    {
        auto *request = &request_.e.realloc;
        DQN_ASSERT(request->old_size > 0);
        DQN_ASSERT(request->new_size > 0);
        DQN_ASSERT((request->new_size - request->old_size) != 0);
        DQN_ASSERT(request->old_mem_ptr);
        return;
    }
}

FILE_SCOPE void DqnMemAPI__UpdateAPIStatistics(DqnMemAPI *api, DqnMemAPI::Request *request_)
{
    if (request_->type == DqnMemAPI::Type::Alloc)
    {
        auto *request = &request_->e.alloc;
        api->bytes_allocated += request->request_size;
        api->lifetime_bytes_allocated += request->request_size;
        return;
    }

    if (request_->type == DqnMemAPI::Type::Realloc)
    {
        auto *request = &request_->e.realloc;
        api->lifetime_bytes_allocated += request->new_size;
        api->lifetime_bytes_freed += request->old_size;

        api->bytes_allocated += request->new_size;
        api->bytes_allocated -= request->old_size;
        return;
    }

    if (request_->type == DqnMemAPI::Type::Free)
    {
        auto *request = &request_->e.free;
        api->bytes_allocated -= request->size_to_free;
        api->lifetime_bytes_freed += request->size_to_free;
        return;
    }
}

FILE_SCOPE void *DqnMemAPI__HeapAllocatorCallback(DqnMemAPI *api, DqnMemAPI::Request request_)
{
    DqnMemAPI__ValidateRequest(request_);
    DQN_ASSERT(!request_.user_context);

    u8 *result   = nullptr;
    bool success = false;

    if (request_.type == DqnMemAPI::Type::Alloc)
    {
        auto const *request = &request_.e.alloc;

        if (request->zero_clear) result = (u8 *)DqnMem_Calloc(request->request_size);
        else                    result = (u8 *)DqnMem_Alloc(request->request_size);

        success = (result != nullptr);
    }
    else if (request_.type == DqnMemAPI::Type::Realloc)
    {
        auto const *request = &request_.e.realloc;
        if (request->new_size == request->old_size)
        {
            result = (u8 *)request->old_mem_ptr;
        }
        else
        {
            result  = (u8 *)DqnMem_Realloc(request->old_mem_ptr, request->new_size);
            success = (result != nullptr);
        }
    }
    else if (request_.type == DqnMemAPI::Type::Free)
    {
        auto *request = &request_.e.free;
        DqnMem_Free(request->ptr_to_free);
        success = (request->ptr_to_free != nullptr);
    }
    else
    {
        DQN_ASSERT(DQN_INVALID_CODE_PATH);
    }

    if (success)
    {
        DqnMemAPI__UpdateAPIStatistics(api, &request_);
    }

    return result;
}

struct DqnMemAPI__DqnMemStackContext
{
    enum Mode
    {
        PushToHead,
        PushToTail,
    };

    DqnMemStack *stack;
    Mode         mode;
};

FILE_SCOPE void *
DqnMemAPI__StackAllocatorCallback(DqnMemAPI *api, DqnMemAPI::Request request_, bool push_to_head)
{
    DqnMemAPI__ValidateRequest(request_);
    DQN_ASSERT(request_.user_context);

    DqnMemStack::AllocTo const allocTo = (push_to_head) ? DqnMemStack::AllocTo::Head : DqnMemStack::AllocTo::Tail;
    auto *const stack = (DqnMemStack *)(request_.user_context);
    void *result      = nullptr;
    bool success      = false;

    auto PtrIsLastAllocationInBlock =
        [](DqnMemTracker const *tracker, DqnMemStack::Block const *block, char *ptr) -> bool {

        DqnPtrHeader *header = tracker->PtrToHeader(ptr);
        bool result  = false;
        if (header->alloc_type == 0)
        {
            char const *ptrEnd     = ptr - header->offset_to_src_ptr + tracker->GetAllocationSize(header->alloc_amount, header->alignment);
            result                 = ptrEnd == block->head;
        }
        else
        {
            auto *actualPtr = ptr - header->offset_to_src_ptr;
            result          = actualPtr == block->tail;
        }

        return result;
    };

    if (request_.type == DqnMemAPI::Type::Alloc)
    {
        auto *request = &request_.e.alloc;
        result        = static_cast<char *>(stack->Push(request->request_size, allocTo));
        if (result)
        {
            success = true;
            if (request->zero_clear) DqnMem_Clear(result, 0, request->request_size);
        }
    }
    else if (request_.type == DqnMemAPI::Type::Realloc)
    {
        // IMPORTANT: This is a _naive_ realloc scheme for stack allocation.
        auto *request        = &request_.e.realloc;
        char *ptr            = static_cast<char *>(request->old_mem_ptr);
        DqnPtrHeader *header = stack->tracker.PtrToHeader(static_cast<char *>(request->old_mem_ptr));

        for (DqnMemStack::Block *block = stack->block; block; block = block->prev_block)
        {
            DQN_ASSERT(ptr >= block->memory && ptr <= (block->memory + block->size));
        }

        DqnMemStack::Block *const block = stack->block;
        isize const oldMemSize          = header->alloc_amount;
        isize const extraBytesReq       = request->new_size - oldMemSize;
        u8 alignment                    = header->alignment;
        DQN_ASSERT(extraBytesReq > 0);

        if (PtrIsLastAllocationInBlock(&stack->tracker, block, ptr))
        {
            bool enoughSpace = false;
            if (header->alloc_type == 0)
            {
                DQN_ASSERT((block->head + extraBytesReq) >= block->memory);

                enoughSpace = (block->head + extraBytesReq) < block->tail;
                if (enoughSpace)
                {
                    stack->Pop(ptr, Dqn::ZeroClear::No);
                    result = static_cast<char *>(stack->Push(request->new_size, DqnMemStack::AllocTo::Head, alignment));
                    DQN_ASSERT(stack->block == block && result == request->old_mem_ptr);
                    success = true;
                }
            }
            else
            {
                DQN_ASSERT((block->tail - extraBytesReq) < (block->memory + block->size));
                enoughSpace = (block->tail - extraBytesReq) > block->head;
                if (enoughSpace)
                {
                    stack->Pop(ptr, Dqn::ZeroClear::No);
                    result = static_cast<char *>(stack->Push(request->new_size, DqnMemStack::AllocTo::Tail, alignment));
                    DqnMem_Copy(result, ptr, oldMemSize);
                    (static_cast<char *>(result))[oldMemSize] = 0;

                    success = true;
                    DQN_ASSERT(stack->block == block);
                }
            }

            if (!enoughSpace)
            {
                // TODO(doyle): Does realloc need clear to zero flag as well?
                // Else, last allocation but not enough space in block. Create a new block and
                // copy
                DqnMemStack::Block *oldBlock = block;
                if (header->alloc_type == 0)
                {
                    result = static_cast<char *>(stack->Push(request->new_size, DqnMemStack::AllocTo::Head, alignment));
                }
                else
                {
                    result = static_cast<char *>(stack->Push(request->new_size, DqnMemStack::AllocTo::Tail, alignment));
                }

                if (result)
                {
                    DQN_ASSERT(stack->block->prev_block == oldBlock);
                    DQN_ASSERT(stack->block != oldBlock);
                    DqnMem_Copy(result, ptr, oldMemSize);

                    // Switch to old block, pop the ptr and return the new block on top.
                    auto *new_block = stack->block;
                    stack->block   = oldBlock;

                    stack->Pop(ptr, Dqn::ZeroClear::No);
                    stack->block = new_block;
                    success      = true;
                }
            }
        }
        else
        {
            if (request->new_size < request->old_size)
            {
                // NOTE: This is questionable behaviour. We don't reclaim data since it's not
                // well-defined in a stack allocator. This would cause gaps in memory.
                success = false; // Deny updating statistics.
                result  = ptr;
            }
            else
            {
                DQN_LOGE("Lost %$_d, the ptr to realloc is sandwiched between other allocations (LIFO)", oldMemSize);

                if (header->alloc_type == 0)
                {
                    result = (u8 *)stack->Push(request->new_size, DqnMemStack::AllocTo::Head, alignment);
                }
                else
                {
                    result = (u8 *)stack->Push(request->new_size, DqnMemStack::AllocTo::Tail, alignment);
                }

                if (result)
                {
                    success = true;
                    DqnMem_Copy(result, ptr, oldMemSize);
                }
            }
        }
    }
    else
    {
        auto *request = &request_.e.free;
        DQN_ASSERT(request_.type == DqnMemAPI::Type::Free);

        DqnMemStack::Block *block = stack->block;
        char *ptr                 = static_cast<char *>(request->ptr_to_free);
        DqnPtrHeader *header      = stack->tracker.PtrToHeader(ptr);

        if (PtrIsLastAllocationInBlock(&stack->tracker, block, ptr))
        {
            stack->Pop(ptr, Dqn::ZeroClear::No);
        }
        else
        {
            DQN_LOGE("Lost %$_d, the ptr to free is sandwiched between other allocations (LIFO)", header->alloc_amount);
        }
    }

    // TODO(doyle): Stats. Irrelevant now?
    (void)api;
    if (success)
    {
#if 0
        DqnMemAPI__UpdateAPIStatistics(api, &request_);
#endif
    }

    return result;
}

FILE_SCOPE void *DqnMemAPI__StackAllocatorCallbackPushToHead(DqnMemAPI *api, DqnMemAPI::Request request_)
{
    void *result = DqnMemAPI__StackAllocatorCallback(api, request_, true);
    return result;
}

FILE_SCOPE void *DqnMemAPI__StackAllocatorCallbackPushToTail(DqnMemAPI *api, DqnMemAPI::Request request_)
{
    void *result = DqnMemAPI__StackAllocatorCallback(api, request_, false);
    return result;
}

void *DqnMemAPI::Realloc(void *const old_ptr, isize old_size, isize new_size)
{
    Request request               = {};
    request.type                  = Type::Realloc;
    request.user_context          = this->user_context;
    request.e.realloc.new_size    = new_size;
    request.e.realloc.old_mem_ptr = old_ptr;
    request.e.realloc.old_size    = old_size;

    void *result = (void *)this->allocator(this, request);
    return result;
}

void *DqnMemAPI::Alloc(isize size, Dqn::ZeroClear clear)
{
    Request request              = {};
    request.type                 = Type::Alloc;
    request.user_context         = this->user_context;
    request.e.alloc.zero_clear   = (clear == Dqn::ZeroClear::Yes) ? true : false;
    request.e.alloc.request_size = size;

    void *result = (void *)this->allocator(this, request);
    return result;
}

void DqnMemAPI::Free(void *const ptr_to_free, isize size_to_free)
{
    Request request             = {};
    request.type                = Type::Free;
    request.user_context        = this->user_context;
    request.e.free.ptr_to_free  = ptr_to_free;
    request.e.free.size_to_free = size_to_free;
    this->allocator(this, request);
}

DqnMemAPI DqnMemAPI::HeapAllocator()
{
    DqnMemAPI result   = {0};
    result.allocator   = DqnMemAPI__HeapAllocatorCallback;
    result.user_context = nullptr;
    return result;
}

DqnMemAPI DqnMemAPI::StackAllocator(struct DqnMemStack *stack, StackPushType type)
{
    DQN_ASSERT(stack);
    DqnMemAPI result   = {0};
    result.allocator   = (type == StackPushType::Head)
                           ? DqnMemAPI__StackAllocatorCallbackPushToHead
                           : DqnMemAPI__StackAllocatorCallbackPushToTail;
    result.user_context = stack;
    return result;
}

// #DqnMemTracker
// =================================================================================================
void DqnMemTracker::Init(bool bounds_guard)
{
    // TODO(doyle): How to handle memory here.
    if (bounds_guard)
    {
        this->bounds_guard_size        = sizeof(HEAD_GUARD_VALUE);
        LOCAL_PERSIST DqnMemAPI heap = DqnMemAPI::HeapAllocator();
        this->allocations.mem_api     = &heap;
        this->allocations.Reserve(128);
    }
    else
    {
        this->bounds_guard_size = 0;
    }
}

void DqnMemTracker::RemoveAllocation(char *ptr)
{
    isize deleteIndex = -1;
    for (isize i = 0; i < this->allocations.len; i++)
    {
        if (allocations.data[i] == ptr)
        {
            deleteIndex = i;
            break;
        }
    }

    DQN_ALWAYS_ASSERTM(deleteIndex != -1, "Ptr %p was not in the tracked pointers list", ptr);
    this->allocations.EraseStable(deleteIndex);
}

void DqnMemTracker::CheckAllocations() const
{
    for (auto index = 0; index < this->allocations.len; index++)
    {
        char *ptr             = static_cast<char *>(this->allocations.data[index]);
        u32 const *head_guard = this->PtrToHeadGuard(ptr);
        u32 const *tail_guard = this->PtrToTailGuard(ptr);

        DQN_ASSERTM(*head_guard == HEAD_GUARD_VALUE,
                    "Bounds guard has been destroyed at the head end of the allocation! Expected: "
                    "%x, received: %x",
                    HEAD_GUARD_VALUE, *head_guard);

        DQN_ASSERTM(*tail_guard == TAIL_GUARD_VALUE,
                    "Bounds guard has been destroyed at the tail end of the allocation! Expected: "
                    "%x, received: %x",
                    TAIL_GUARD_VALUE, *tail_guard);
    }
}

// #DqnMemStack
// =================================================================================================
DQN_FILE_SCOPE DqnMemStack::Block *
DqnMemStack__AllocateBlock(isize size, Dqn::ZeroClear clear, DqnMemAPI *api)
{
    isize total_size = sizeof(DqnMemStack::Block) + size;
    auto *result    = static_cast<DqnMemStack::Block *>(api->Alloc(total_size, clear));
    DQN_ALWAYS_ASSERTM(result, "Allocated memory block was null");

    char *block_offset = reinterpret_cast<char *>(result) + sizeof(*result);
    *result           = DqnMemStack::Block(block_offset, size);
    return result;
}

DqnMemStack::DqnMemStack(void *mem, isize size, Dqn::ZeroClear clear, u32 flags_)
{
    DQN_ALWAYS_ASSERTM(mem, "Supplied fixed memory buffer is nullptr, initialise with fixed memory failed");
    DQN_ALWAYS_ASSERTM(size > DQN_SIZEOF(DqnMemStack::Block), "(%zu < %zu) Buffer too small for block metadata", size, DQN_SIZEOF(DqnMemStack::Block));
    *this = {};

    if (clear == Dqn::ZeroClear::Yes)
        DqnMem_Set(mem, 0, size);

    char *block_offset      = static_cast<char *>(mem) + sizeof(*this->block);
    isize const block_size = size - sizeof(*this->block);

    this->block     = static_cast<DqnMemStack::Block *>(mem);
    *this->block    = Block(block_offset, block_size);
    this->flags     = (flags_ | Flag::NonExpandable);

    bool bounds_guard = Dqn_BitIsSet(this->flags, Flag::BoundsGuard);
    this->tracker.Init(bounds_guard);
}

DqnMemStack::DqnMemStack(isize size, Dqn::ZeroClear clear, u32 flags_, DqnMemAPI *api)
{
    DQN_ALWAYS_ASSERTM(size > 0, "%zu <= 0", size);
    *this = {};

    this->block     = DqnMemStack__AllocateBlock(size, clear, api);
    this->flags     = flags_;
    this->mem_api    = api;
    bool bounds_guard = Dqn_BitIsSet(this->flags, Flag::BoundsGuard);
    this->tracker.Init(bounds_guard);
}

void *DqnMemStack::Push(isize size, AllocTo allocTo, u8 alignment)
{
    DQN_ASSERT(size >= 0 && (alignment % 2 == 0));
    DQN_ALWAYS_ASSERTM(alignment <= 128, "Alignment supported. Update metadata to use u16 for storing the offset!");

    if (size == 0)
        return nullptr;

    bool const push_to_head = (allocTo == AllocTo::Head);
    isize size_to_alloc  = this->tracker.GetAllocationSize(size, alignment);

    // Allocate New Block If Full
    // =============================================================================================
    bool need_new_block = true;
    if (this->block)
    {
        if (push_to_head) need_new_block = ((this->block->head + size_to_alloc) > this->block->tail);
        else              need_new_block = ((this->block->tail - size_to_alloc) < this->block->head);
    }

    if (need_new_block)
    {
        if (Dqn_BitIsSet(this->flags, Flag::NonExpandable) && this->block)
        {
            if (Dqn_BitIsSet(this->flags, Flag::NonExpandableAssert))
                DQN_ASSERTM(Dqn_BitIsSet(this->flags, Flag::NonExpandableAssert), "Allocator is non-expandable and has run out of memory");

            return nullptr;
        }

        if (!this->block && !this->mem_api) // we /ssume this is a zero initialised mem stack
        {
            this->mem_api = DQN_DEFAULT_HEAP_ALLOCATOR;
            this->tracker.Init(Dqn_BitIsSet(this->flags, DqnMemStack::Flag::BoundsGuard));
        }

        isize new_block_size  = DQN_MAX(size_to_alloc, MINIMUM_BLOCK_SIZE);
        Block *new_block      = DqnMemStack__AllocateBlock(new_block_size, Dqn::ZeroClear::No, this->mem_api);
        new_block->prev_block = this->block;
        this->block           = new_block;
    }

    // Calculate Ptr To Give Client
    // =============================================================================================
    char *src_ptr          = (push_to_head) ? (this->block->head) : (this->block->tail - size_to_alloc);
    char *unaligned_result = src_ptr + sizeof(DqnPtrHeader) + this->tracker.bounds_guard_size;
    char *aligned_result   = reinterpret_cast<char *>(DQN_ALIGN_POW_N(unaligned_result, alignment));

    isize const offset_to_ptr_header = aligned_result - unaligned_result;
    DQN_ASSERT(offset_to_ptr_header >= 0 && offset_to_ptr_header <= (alignment - 1));

    if (push_to_head)
    {
        this->block->head += size_to_alloc;
        DQN_ASSERT(this->block->head <= this->block->tail);
    }
    else
    {
        this->block->tail -= size_to_alloc;
        DQN_ASSERT(this->block->tail >= this->block->head);
    }

    // Instrument allocation with guards and tracker
    // =============================================================================================
    {
        auto *ptr_header           = reinterpret_cast<DqnPtrHeader *>(src_ptr + offset_to_ptr_header);
        ptr_header->offset_to_src_ptr = static_cast<u8>(aligned_result - src_ptr);
        ptr_header->alignment      = alignment;
        ptr_header->alloc_type      = (push_to_head) ? 0 : 1;
        ptr_header->alloc_amount    = size;

        if (Dqn_BitIsSet(this->flags, DqnMemStack::Flag::BoundsGuard))
        {
            u32 *head_guard = reinterpret_cast<u32 *>(aligned_result - sizeof(DqnMemTracker::HEAD_GUARD_VALUE));
            u32 *tail_guard = reinterpret_cast<u32 *>(aligned_result + ptr_header->alloc_amount);
            *head_guard     = DqnMemTracker::HEAD_GUARD_VALUE;
            *tail_guard     = DqnMemTracker::TAIL_GUARD_VALUE;
        }
    }

    // Debug check (alignment, bounds guard)
    // =============================================================================================
    {
        char *check_allignment = reinterpret_cast<char *>(DQN_ALIGN_POW_N(aligned_result, alignment));
        DQN_ASSERTM(check_allignment == aligned_result, "Adding bounds guard should not destroy alignment! %p != %p", aligned_result, check_allignment);

        if (Dqn_BitIsSet(this->flags, Flag::BoundsGuard))
        {
            this->tracker.AddAllocation(aligned_result);
            this->tracker.CheckAllocations();
        }
    }

    return aligned_result;
}

FILE_SCOPE void DqnMemStack__KillTrackedPtrsInRange(DqnMemTracker *tracker, char const *start, char const *end)
{
    if (start >= end) return;

    for (isize index = 0; index < tracker->allocations.len; index++)
    {
        char *ptr = static_cast<char *>(tracker->allocations.data[index]);
        if (ptr >= start && ptr < end)
        {
            tracker->allocations.EraseStable(index);
            index--;
        }
    }
}

FILE_SCOPE void DqnMemStack__KillTrackedPtrsInBlock(DqnMemTracker *tracker, DqnMemStack::Block const *block)
{
    char const *block_start = block->memory;
    char const *block_end   = block->memory + block->size;
    DqnMemStack__KillTrackedPtrsInRange(tracker, block_start, block_end);
}

void DqnMemStack::Pop(void *ptr, Dqn::ZeroClear clear)
{
    if (!ptr) return;

    char *byte_ptr           = static_cast<char *>(ptr);
    DqnPtrHeader *ptr_header = reinterpret_cast<DqnPtrHeader *>(byte_ptr - sizeof(*ptr_header));

    // Check instrumented data
    if (Dqn_BitIsSet(this->flags, Flag::BoundsGuard))
    {
        this->tracker.CheckAllocations();
        this->tracker.RemoveAllocation(byte_ptr);
        ptr_header = reinterpret_cast<DqnPtrHeader *>(reinterpret_cast<char *>(ptr_header) - this->tracker.bounds_guard_size);
    }

    isize full_allocation_size = this->tracker.GetAllocationSize(ptr_header->alloc_amount, ptr_header->alignment);
    char *start = byte_ptr - ptr_header->offset_to_src_ptr;
    char *end   = start + full_allocation_size;
    char const *block_end = this->block->memory + this->block->size;

    if (ptr_header->alloc_type == 0)
    {
        DQN_ASSERTM(end == this->block->head, "Pointer to pop was not the last allocation! %p != %p", end, this->block->head);
        this->block->head -= full_allocation_size;
        DQN_ASSERT(this->block->head >= this->block->memory);
    }
    else
    {
        DQN_ASSERTM(start == this->block->tail, "Pointer to pop was not the last allocation! %p != %p", start, this->block->tail);
        this->block->tail += full_allocation_size;
        DQN_ASSERT(this->block->tail <= block_end);
    }

    if (clear == Dqn::ZeroClear::Yes)
        DqnMem_Set(start, 0, end - start);

    if (this->block->tail == block_end && this->block->head == this->block->memory)
    {
        if (this->block->prev_block)
        {
            this->FreeLastBlock();
        }
    }
}

void DqnMemStack::Free()
{
    if (Dqn_BitIsSet(this->flags, Flag::BoundsGuard))
        this->tracker.allocations.Free();

    if (this->mem_api)
    {
        while (this->block)
            this->FreeLastBlock();
    }
}

bool DqnMemStack::FreeMemBlock(DqnMemStack::Block *mem_block)
{
    if (!mem_block || !this->block)
        return false;

    if (!this->mem_api)
        return false;

    DqnMemStack::Block **block_ptr = &this->block;

    while (*block_ptr && (*block_ptr) != mem_block)
        block_ptr = &((*block_ptr)->prev_block);

    if (*block_ptr)
    {
        DqnMemStack::Block *block_to_free = *block_ptr;
        (*block_ptr)                       = block_to_free->prev_block;

        if (Dqn_BitIsSet(this->flags, Flag::BoundsGuard))
        {
            DqnMemStack__KillTrackedPtrsInBlock(&this->tracker, block_to_free);
        }

        isize real_size = block_to_free->size + sizeof(DqnMemStack::Block);
        this->mem_api->Free(block_to_free, real_size);

        // No more blocks, then last block has been freed
        if (!this->block) DQN_ASSERT(this->tmp_region_count == 0);
        return true;
    }

    return false;
}

void DqnMemStack::ResetTail()
{
    char *start = this->block->tail;
    char *end   = this->block->memory + this->block->size;
    if (Dqn_BitIsSet(this->flags, Flag::BoundsGuard))
    {
        DqnMemStack__KillTrackedPtrsInRange(&this->tracker, start, end);
    }

    this->block->tail = end;
}

void DqnMemStack::Reset()
{
    while(this->block && this->block->prev_block)
    {
        this->FreeLastBlock();
    }
    this->ClearCurrBlock(Dqn::ZeroClear::No);
}


bool DqnMemStack::FreeLastBlock()
{
    bool result = this->FreeMemBlock(this->block);
    return result;
}

void DqnMemStack::ClearCurrBlock(Dqn::ZeroClear clear)
{
    if (this->block)
    {
        if (Dqn_BitIsSet(this->flags, Flag::BoundsGuard))
        {
            DqnMemStack__KillTrackedPtrsInBlock(&this->tracker, this->block);
        }

        this->block->head = this->block->memory;
        this->block->tail = this->block->memory + this->block->size;
        if (clear == Dqn::ZeroClear::Yes)
        {
            DqnMem_Clear(this->block->memory, 0, this->block->size);
        }

    }
}

DqnMemStack::Info DqnMemStack::GetInfo() const
{
    Info result = {};
    for (Block *block_ = this->block; block_; block_ = block_->prev_block)
    {
        char const *block_end = block_->memory + block_->size;
        isize usage_from_head  = block_->head - block_->memory;
        isize usage_from_tail  = block_end - block_->tail;

        result.total_used += usage_from_head + usage_from_tail;
        result.total_size += block_->size;
        result.wasted_size += (block_->size - usage_from_head - usage_from_tail);
        result.num_blocks++;
    }

    char const *block_end = this->block->memory + this->block->size;
    isize usage_from_head  = this->block->head - this->block->memory;
    isize usage_from_tail  = block_end - this->block->tail;
    result.wasted_size -= (this->block->size - usage_from_head - usage_from_tail); // Don't include the curr block

    return result;
}

DqnMemStack::TempRegion DqnMemStack::TempRegionBegin()
{
    TempRegion result        = {};
    result.stack             = this;
    result.starting_block     = this->block;
    result.starting_block_head = (this->block) ? this->block->head : nullptr;
    result.starting_block_tail = (this->block) ? this->block->tail : nullptr;

    this->tmp_region_count++;
    return result;
}

void DqnMemStack::TempRegionEnd(TempRegion region)
{
    DQN_ASSERT(region.stack == this);

    this->tmp_region_count--;
    DQN_ASSERT(this->tmp_region_count >= 0);

    if (region.keep_head_changes && region.keep_tail_changes)
    {
        return;
    }

    // Free blocks until you find the first block with changes in the head or tail, this is the
    // block we want to start preserving allocation data for keepHead/TailChanges.
    if (region.keep_head_changes)
    {
        while (this->block && this->block->head == this->block->memory)
            this->FreeLastBlock();
    }
    else if (region.keep_tail_changes)
    {
        while (this->block && this->block->tail == (this->block->memory + this->block->size))
            this->FreeLastBlock();
    }
    else
    {
        while (this->block != region.starting_block)
            this->FreeLastBlock();
    }

    for (Block *block_ = this->block; block_; block_ = block_->prev_block)
    {
        if (block_ == region.starting_block)
        {
            if (region.keep_head_changes)
            {
                block_->tail = region.starting_block_tail;
            }
            else if (region.keep_tail_changes)
            {
                block_->head = region.starting_block_head;
            }
            else
            {
                block_->head = region.starting_block_head;
                block_->tail = region.starting_block_tail;
            }

            if (Dqn_BitIsSet(this->flags, DqnMemStack::Flag::BoundsGuard))
            {
                char *block_start = this->block->head;
                char *block_end   = this->block->tail;
                DqnMemStack__KillTrackedPtrsInRange(&this->tracker, block_start, block_end);
            }
            break;
        }
        else
        {
            if (region.keep_head_changes || region.keep_tail_changes)
            {
                char *block_start = nullptr;
                char *block_end   = nullptr;
                if (region.keep_head_changes)
                {
                    block_start   = block_->tail;
                    block_end     = block_->memory + block_->size;
                    block_->tail = block_end;
                }
                else
                {
                    block_start   = block_->memory;
                    block_end     = block_->memory + block_->size;
                    block_->head = block_start;
                }

                if (Dqn_BitIsSet(this->flags, DqnMemStack::Flag::BoundsGuard))
                {
                    DqnMemStack__KillTrackedPtrsInRange(&this->tracker, block_start, block_end);
                }
            }
        }
    }
}

DqnMemStack::TempRegionGuard_ DqnMemStack::TempRegionGuard()
{
    return TempRegionGuard_(this);
}

DqnMemStack::TempRegionGuard_::TempRegionGuard_(DqnMemStack *stack)
{
    this->region = stack->TempRegionBegin();
}

DqnMemStack::TempRegionGuard_::~TempRegionGuard_()
{
    DqnMemStack *const stack = this->region.stack;
    stack->TempRegionEnd(this->region);
}

// #DqnHash
// =================================================================================================
// Taken from GingerBill single file library @ github.com/gingerbill/gb
u32 DqnHash_Murmur32Seed(void const *data, usize len, u32 seed)
{
    u32 const c1 = 0xcc9e2d51; u32 const c2 = 0x1b873593; u32 const r1 = 15;
    u32 const r2 = 13; u32 const m  = 5; u32 const n  = 0xe6546b64;

    usize i, nblocks = len / 4;
    u32 hash = seed, k1 = 0;
    u32 const *blocks = (u32 const *)data;
    u8 const *tail    = (u8 const *)(data) + nblocks * 4;

    for (i = 0; i < nblocks; i++) {
        u32 k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }

    switch (len & 3) {
    case 3: k1 ^= tail[2] << 16;
    case 2: k1 ^= tail[1] << 8;
    case 1: k1 ^= tail[0];

        k1 *= c1;
        k1 = (k1 << r1) | (k1 >> (32 - r1));
        k1 *= c2;
        hash ^= k1;
    }

    hash ^= len;        hash ^= (hash >> 16);
    hash *= 0x85ebca6b; hash ^= (hash >> 13);
    hash *= 0xc2b2ae35; hash ^= (hash >> 16);

    return hash;
}

u64 DqnHash_Murmur64Seed(void const *data_, usize len, u64 seed)
{
    u64 const m = 0xc6a4a7935bd1e995ULL;
    i32 const r = 47;

    u64 h = seed ^ (len * m);

    u64 const *data = (u64 const *)data_;
    u8 const *data2 = (u8 const *)data_;
    u64 const *end  = data + (len / 8);

    while (data != end) {
        u64 k = *data++;
        k *= m; k ^= k >> r; k *= m;
        h ^= k; h *= m;
    }

    switch (len & 7) {
    case 7: h ^= (u64)(data2[6]) << 48;
    case 6: h ^= (u64)(data2[5]) << 40;
    case 5: h ^= (u64)(data2[4]) << 32;
    case 4: h ^= (u64)(data2[3]) << 24;
    case 3: h ^= (u64)(data2[2]) << 16;
    case 2: h ^= (u64)(data2[1]) << 8;
    case 1: h ^= (u64)(data2[0]);
        h *= m;
    };

    h ^= h >> r; h *= m; h ^= h >> r;
    return h;
}

// #DqnMath
// =================================================================================================
DQN_FILE_SCOPE f32 DqnMath_Lerp(f32 a, f32 t, f32 b)
{
    /*
        Linear blend between two values. We having a starting point "a", and
        the distance to "b" is defined as (b - a). Then we can say

        a + t(b - a)

        As our linear blend fn. We start from "a" and choosing a t from 0->1
        will vary the value of (b - a) towards b. If we expand this, this
        becomes

        a + (t * b) - (a * t) == (1 - t)a + t*b
    */
    f32 result =  a + (b - a) * t;
    return result;
}

DQN_FILE_SCOPE f32 DqnMath_Sqrtf(f32 a)
{
    f32 result = sqrtf(a);
    return result;
}

DQN_FILE_SCOPE f32 DqnMath_Clampf(f32 val, f32 min, f32 max)
{
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

// #DqnV2
// =================================================================================================
DQN_FILE_SCOPE DqnV2 DqnV2_Add(DqnV2 a, DqnV2 b)
{
    DqnV2 result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] + b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Sub(DqnV2 a, DqnV2 b)
{
    DqnV2 result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] - b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Scalei(DqnV2 a, i32 b)
{
    DqnV2 result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Scalef(DqnV2 a, f32 b)
{
    DqnV2 result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Hadamard(DqnV2 a, DqnV2 b)
{
    DqnV2 result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b.e[i];

    return result;
}

DQN_FILE_SCOPE f32 DqnV2_Dot(DqnV2 a, DqnV2 b)
{
    /*
       DOT PRODUCT
       Two vectors with dot product equals |a||b|cos(theta)
       |a|   |d|
       |b| . |e| = (ad + be + cf)
       |c|   |f|
     */
    f32 result = 0;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result += (a.e[i] * b.e[i]);

    return result;
}

DQN_FILE_SCOPE bool DqnV2_Equals(DqnV2 a, DqnV2 b)
{
    bool result = true;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        if (a.e[i] != b.e[i]) result = false;
    return result;
}

DQN_FILE_SCOPE f32 DqnV2_LengthSquared(DqnV2 a, DqnV2 b)
{
    f32 x_     = b.x - a.x;
    f32 y_     = b.y - a.y;
    f32 result = (DQN_SQUARED(x_) + DQN_SQUARED(y_));
    return result;
}

DQN_FILE_SCOPE f32 DqnV2_Length(DqnV2 a, DqnV2 b)
{
    f32 len_sq = DqnV2_LengthSquared(a, b);
    if (len_sq == 0) return 0;

    f32 result = DqnMath_Sqrtf(len_sq);
    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Normalise(DqnV2 a)
{
    f32 magnitude = DqnV2_Length(DqnV2(0, 0), a);
    if (magnitude == 0) return DqnV2(0.0f);

    DqnV2 result = a * (1.0f / magnitude);
    return result;
}

DQN_FILE_SCOPE bool DqnV2_Overlaps(DqnV2 a, DqnV2 b)
{
    bool result = false;

    f32 lenOfA = a.max - a.min;
    f32 lenOfB = b.max - b.min;

    if (lenOfA > lenOfB)
    {
        DqnV2 tmp = a;
        a         = b;
        b         = tmp;
    }

    if ((a.min >= b.min && a.min <= b.max) || (a.max >= b.min && a.max <= b.max))
    {
        result = true;
    }

    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Perpendicular(DqnV2 a)
{
    DqnV2 result = DqnV2(a.y, -a.x);
    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_ResizeKeepAspectRatio(DqnV2 src_size, DqnV2 target_size)
{
    f32 ratio_a   = src_size.w / target_size.w;
    f32 ratio_b   = src_size.h / target_size.h;
    f32 ratio    = DQN_MIN(ratio_a, ratio_b);
    DqnV2 result = DqnV2_Scalef(target_size, ratio);
    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_ConstrainToRatio(DqnV2 dim, DqnV2 ratio)
{
    DqnV2 result                  = {0};
    f32 num_ratio_increments_to_width  = (f32)(dim.w / ratio.w);
    f32 num_ratio_increments_to_height = (f32)(dim.h / ratio.h);

    f32 least_increments_to_side =
        DQN_MIN(num_ratio_increments_to_height, num_ratio_increments_to_width);

    result.w = (f32)(ratio.w * least_increments_to_side);
    result.h = (f32)(ratio.h * least_increments_to_side);
    return result;
}

// #DqnV2i
// =================================================================================================
DQN_FILE_SCOPE DqnV2i DqnV2i_Add(DqnV2i a, DqnV2i b)
{
    DqnV2i result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] + b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_Sub(DqnV2i a, DqnV2i b)
{
    DqnV2i result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] - b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_Scalef(DqnV2i a, f32 b)
{
    DqnV2i result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = (i32)(a.e[i] * b);

    return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_Scalei(DqnV2i a, i32 b)
{
    DqnV2i result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_Hadamard(DqnV2i a, DqnV2i b)
{
    DqnV2i result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b.e[i];

    return result;
}

DQN_FILE_SCOPE f32 DqnV2i_Dot(DqnV2i a, DqnV2i b)
{
    /*
       DOT PRODUCT
       Two vectors with dot product equals |a||b|cos(theta)
       |a|   |d|
       |b| . |e| = (ad + be + cf)
       |c|   |f|
     */
    f32 result = 0;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result += (a.e[i] * b.e[i]);

    return result;
}

DQN_FILE_SCOPE bool DqnV2i_Equals(DqnV2i a, DqnV2i b)
{
    bool result = true;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        if (a.e[i] != b.e[i]) result = false;
    return result;
}

// #DqnV3
// =================================================================================================
DQN_FILE_SCOPE DqnV3 DqnV3_Add(DqnV3 a, DqnV3 b)
{
    DqnV3 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] + b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Sub(DqnV3 a, DqnV3 b)
{
    DqnV3 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] - b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Scalei(DqnV3 a, i32 b)
{
    DqnV3 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Scalef(DqnV3 a, f32 b)
{
    DqnV3 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Hadamard(DqnV3 a, DqnV3 b)
{
    DqnV3 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b.e[i];

    return result;
}

DQN_FILE_SCOPE f32 DqnV3_Dot(DqnV3 a, DqnV3 b)
{
    /*
       DOT PRODUCT
       Two vectors with dot product equals |a||b|cos(theta)
       |a|   |d|
       |b| . |e| = (ad + be + cf)
       |c|   |f|
     */
    f32 result = 0;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result += (a.e[i] * b.e[i]);

    return result;
}

DQN_FILE_SCOPE bool DqnV3_Equals(DqnV3 a, DqnV3 b)
{
    bool result = true;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        if (a.e[i] != b.e[i]) result = false;
    return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Cross(DqnV3 a, DqnV3 b)
{
    /*
       CROSS PRODUCT
       Generate a perpendicular vector to the 2 vectors
       |a|   |d|   |bf - ce|
       |b| x |e| = |cd - af|
       |c|   |f|   |ae - be|
     */
    DqnV3 result = {0};
    result.e[0] = (a.e[1] * b.e[2]) - (a.e[2] * b.e[1]);
    result.e[1] = (a.e[2] * b.e[0]) - (a.e[0] * b.e[2]);
    result.e[2] = (a.e[0] * b.e[1]) - (a.e[1] * b.e[0]);
    return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Normalise(DqnV3 a)
{
    f32 length    = DqnMath_Sqrtf(DQN_SQUARED(a.x) + DQN_SQUARED(a.y) + DQN_SQUARED(a.z));
    f32 inv_len = 1 / length;
    DqnV3 result  = a * inv_len;

    return result;
}

DQN_FILE_SCOPE f32 DqnV3_LengthSquared(DqnV3 a, DqnV3 b)
{
    f32 x      = b.x - a.x;
    f32 y      = b.y - a.y;
    f32 z      = b.z - a.z;
    f32 result = (DQN_SQUARED(x) + DQN_SQUARED(y) + DQN_SQUARED(z));
    return result;
}

DQN_FILE_SCOPE f32 DqnV3_Length(DqnV3 a, DqnV3 b)
{
    f32 len_sq = DqnV3_LengthSquared(a, b);
    if (len_sq == 0) return 0;

    f32 result = DqnMath_Sqrtf(len_sq);
    return result;
}

// #DqnV4
// =================================================================================================
DQN_FILE_SCOPE DqnV4 DqnV4_Add(DqnV4 a, DqnV4 b)
{
    DqnV4 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] + b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_Sub(DqnV4 a, DqnV4 b)
{
    DqnV4 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] - b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_Scalei(DqnV4 a, i32 b)
{
    DqnV4 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_Scalef(DqnV4 a, f32 b)
{
    DqnV4 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_Hadamard(DqnV4 a, DqnV4 b)
{
    DqnV4 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b.e[i];

    return result;
}

DQN_FILE_SCOPE f32 DqnV4_Dot(DqnV4 a, DqnV4 b)
{
    /*
       DOT PRODUCT
       Two vectors with dot product equals |a||b|cos(theta)
       |a|   |d|
       |b| . |e| = (ad + be + cf)
       |c|   |f|
     */
    f32 result = 0;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result += (a.e[i] * b.e[i]);

    return result;
}

DQN_FILE_SCOPE bool DqnV4_Equals(DqnV4 a, DqnV4 b)
{
    bool result = true;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        if (a.e[i] != b.e[i]) result = false;
    return result;
}

// #DqnMat4 Implementation
// =================================================================================================
DQN_FILE_SCOPE DqnMat4 DqnMat4_Identity()
{
    DqnMat4 result = {0, 0, 0, 0};
    result.e[0][0] = 1;
    result.e[1][1] = 1;
    result.e[2][2] = 1;
    result.e[3][3] = 1;
    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 znear,
                                            f32 zfar)
{
    DqnMat4 result = DqnMat4_Identity();
    result.e[0][0] = +2.0f / (right - left);
    result.e[1][1] = +2.0f / (top   - bottom);
    result.e[2][2] = -2.0f / (zfar  - znear);

    result.e[3][0] = -(right + left)   / (right - left);
    result.e[3][1] = -(top   + bottom) / (top   - bottom);
    result.e[3][2] = -(zfar  + znear)  / (zfar  - znear);

    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Perspective(f32 fov_y_degrees, f32 aspect_ratio, f32 znear, f32 zfar)
{
    f32 fov_y_radians         = DQN_DEGREES_TO_RADIANS(fov_y_degrees);
    f32 fov_y_radians_over_2    = fov_y_radians * 0.5f;
    f32 tan_fov_y_radians_over_2 = tanf(fov_y_radians_over_2);
    f32 znear_sub_zfar        = znear - zfar;

    DqnMat4 result = DqnMat4_Identity();
    result.e[0][0] = 1.0f / (aspect_ratio * tan_fov_y_radians_over_2);
    result.e[1][1] = 1.0f / tan_fov_y_radians_over_2;
    result.e[2][2] = (znear + zfar) / znear_sub_zfar;
    result.e[2][3] = -1.0f;
    result.e[3][2] = (2.0f * znear * zfar) / znear_sub_zfar;
    result.e[3][3] = 0.0f;

    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_LookAt(DqnV3 eye, DqnV3 center, DqnV3 up)
{
    DqnMat4 result = {0, 0, 0, 0};

    DqnV3 f = DqnV3_Normalise(DqnV3_Sub(eye, center));
    DqnV3 s = DqnV3_Normalise(DqnV3_Cross(up, f));
    DqnV3 u = DqnV3_Cross(f, s);

    result.e[0][0] = s.x;
    result.e[0][1] = u.x;
    result.e[0][2] = -f.x;

    result.e[1][0] = s.y;
    result.e[1][1] = u.y;
    result.e[1][2] = -f.y;

    result.e[2][0] = s.z;
    result.e[2][1] = u.z;
    result.e[2][2] = -f.z;

    result.e[3][0] = -DqnV3_Dot(s, eye);
    result.e[3][1] = -DqnV3_Dot(u, eye);
    result.e[3][2] = DqnV3_Dot(f, eye);
    result.e[3][3] = 1.0f;
    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Translate3f(f32 x, f32 y, f32 z)
{
    DqnMat4 result = DqnMat4_Identity();
    result.e[3][0] = x;
    result.e[3][1] = y;
    result.e[3][2] = z;
    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_TranslateV3(DqnV3 vec)
{
    DqnMat4 result = DqnMat4_Identity();
    result.e[3][0] = vec.x;
    result.e[3][1] = vec.y;
    result.e[3][2] = vec.z;
    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Rotate(f32 radians, f32 x, f32 y, f32 z)
{
    DqnMat4 result     = DqnMat4_Identity();
    f32 sin_val         = sinf(radians);
    f32 cos_val         = cosf(radians);
    f32 one_minux_cos_val = 1 - cos_val;

    DqnV3 axis = DqnV3_Normalise(DqnV3(x, y, z));

    result.e[0][0] = (axis.x * axis.x * one_minux_cos_val) + cos_val;
    result.e[0][1] = (axis.x * axis.y * one_minux_cos_val) + (axis.z * sin_val);
    result.e[0][2] = (axis.x * axis.z * one_minux_cos_val) - (axis.y * sin_val);

    result.e[1][0] = (axis.y * axis.x * one_minux_cos_val) - (axis.z * sin_val);
    result.e[1][1] = (axis.y * axis.y * one_minux_cos_val) + cos_val;
    result.e[1][2] = (axis.y * axis.z * one_minux_cos_val) + (axis.x * sin_val);

    result.e[2][0] = (axis.z * axis.x * one_minux_cos_val) + (axis.y * sin_val);
    result.e[2][1] = (axis.z * axis.y * one_minux_cos_val) - (axis.x * sin_val);
    result.e[2][2] = (axis.z * axis.z * one_minux_cos_val) + cos_val;

    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Scale(f32 x, f32 y, f32 z)
{
    DqnMat4 result = {0, 0, 0, 0};
    result.e[0][0] = x;
    result.e[1][1] = y;
    result.e[2][2] = z;
    result.e[3][3] = 1;
    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_ScaleV3(DqnV3 scale)
{
    DqnMat4 result = {0, 0, 0, 0};
    result.e[0][0] = scale.x;
    result.e[1][1] = scale.y;
    result.e[2][2] = scale.z;
    result.e[3][3] = 1;
    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Mul(DqnMat4 a, DqnMat4 b)
{
    DqnMat4 result = {0};
    for (u32 j = 0; j < 4; j++) {
        for (u32 i = 0; i < 4; i++)
        {
            result.e[j][i] = a.e[0][i] * b.e[j][0]
                           + a.e[1][i] * b.e[j][1]
                           + a.e[2][i] * b.e[j][2]
                           + a.e[3][i] * b.e[j][3];
        }
    }

    return result;
}

DQN_FILE_SCOPE DqnV4 DqnMat4_MulV4(DqnMat4 a, DqnV4 b)
{
    DqnV4 result = {0};
    result.x = (a.e[0][0] * b.x) + (a.e[1][0] * b.y) + (a.e[2][0] * b.z) + (a.e[3][0] * b.w);
    result.y = (a.e[0][1] * b.x) + (a.e[1][1] * b.y) + (a.e[2][1] * b.z) + (a.e[3][1] * b.w);
    result.z = (a.e[0][2] * b.x) + (a.e[1][2] * b.y) + (a.e[2][2] * b.z) + (a.e[3][2] * b.w);
    result.w = (a.e[0][3] * b.x) + (a.e[1][3] * b.y) + (a.e[2][3] * b.z) + (a.e[3][3] * b.w);

    return result;
}

// #DqnRect Implementation
// =================================================================================================
void DqnRect::GetSize(f32 *width, f32 *height) const
{
    DQN_ASSERT(this->min <= this->max);
    if (width) *width   = this->max.x - this->min.x;
    if (height) *height = this->max.y - this->min.y;
}

DqnV2 DqnRect::GetCenter() const
{
    DQN_ASSERT(this->min <= this->max);
    f32 sumX     = this->min.x + this->max.x;
    f32 sumY     = this->min.y + this->max.y;
    DqnV2 result = DqnV2(sumX, sumY) * 0.5f;
    return result;
}

DqnRect DqnRect::ClipRect(DqnRect clip) const
{
    DQN_ASSERT(this->min <= this->max);
    DQN_ASSERT(clip.min <= clip.max);
    DqnRect result = *this;

    if (clip.min.x > this->min.x && clip.min.x < this->max.x)
    {
        if (clip.min.y > this->min.y && clip.min.y < this->max.y)
        {
            result.min = clip.min;
        }
        else if (clip.max.y > this->min.y)
        {
            result.min.x = clip.min.x;
        }
    }

    if (clip.max.x > this->min.x && clip.max.x < this->max.x)
    {
        if (clip.max.y > this->min.y && clip.max.y < this->max.y)
        {
            result.max = clip.max;
        }
        else if (clip.min.y < this->max.y)
        {
            result.max.x = clip.max.x;
        }
    }

    return result;
}

DqnRect DqnRect::Move(DqnV2 shift) const
{
    DQN_ASSERT(this->min <= this->max);

    DqnRect result;
    result.min = this->min + shift;
    result.max = this->max + shift;
    return result;
}

bool DqnRect::ContainsP(DqnV2 p) const
{
    DQN_ASSERT(this->min <= this->max);

    bool outside_of_rect_x = false;
    if (p.x < this->min.x || p.x > this->max.w)
        outside_of_rect_x = true;

    bool outside_of_rect_y = false;
    if (p.y < this->min.y || p.y > this->max.h)
        outside_of_rect_y = true;

    if (outside_of_rect_x || outside_of_rect_y) return false;

    return true;
}

// #DqnChar Implementation
// =================================================================================================
DQN_FILE_SCOPE char DqnChar_ToLower(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        i32 shift_offset = 'a' - 'A';
        return (c + (char)shift_offset);
    }
    return c;
}

DQN_FILE_SCOPE char DqnChar_ToUpper(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        i32 shift_offset = 'a' - 'A';
        return (c - (char)shift_offset);
    }
    return c;
}

DQN_FILE_SCOPE bool DqnChar_IsEndOfLine(char c)
{
    bool result = (c == '\n');
    return result;
}

DQN_FILE_SCOPE bool DqnChar_IsWhitespace(char c)
{
    bool result = (c == ' ' || c == '\r' || c == '\n' || c == '\t');
    return result;
}

DQN_FILE_SCOPE char *DqnChar_TrimWhitespaceAround(char const *src, i32 src_len, i32 *new_len)
{
    if (!src)       return nullptr;
    if (src_len < 0) return (char *)src;

    char const *start = src;
    char const *end   = start + (src_len - 1);
    while(start[0] && DqnChar_IsWhitespace(start[0]))
    {
        start++;
    }

    i32 chars_skipped = (i32)(start - src);
    i32 updated_len   = src_len - chars_skipped;
    if (updated_len <= 0)
    {
        if (new_len) *new_len = 0;
        return nullptr;
    }

    while(end[0] && DqnChar_IsWhitespace(end[0]))
    {
        end--;
    }

    chars_skipped = (i32)((src + src_len - 1) - end);
    updated_len   = updated_len - chars_skipped;

    if (new_len) *new_len = updated_len;
    return (char *)start;
}

DQN_FILE_SCOPE bool DqnChar_IsDigit(char c)
{
    if (c >= '0' && c <= '9') return true;
    return false;
}

DQN_FILE_SCOPE bool DqnChar_IsAlpha(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return true;
    return false;
}

DQN_FILE_SCOPE bool DqnChar_IsAlphaNum(char c)
{
    if (DqnChar_IsAlpha(c) || DqnChar_IsDigit(c)) return true;
    return false;
}

DQN_FILE_SCOPE char *DqnChar_SkipWhitespace(char const *ptr)
{
    while (ptr && (*ptr == ' ' || *ptr == '\r' || *ptr == '\n' || *ptr == '\t')) ptr++;
    return (char *)ptr;
}

DQN_FILE_SCOPE char *DqnChar_FindLastChar(char *ptr, char ch, i32 len, u32 *len_to_char)
{
    for (i32 i = len - 1; i >= 0; i--)
    {
        if (ptr[i] == ch)
        {
            if (len_to_char) *len_to_char = (u32)len - i;
            return &ptr[i];
        }
    }

    return nullptr;
}

DQN_FILE_SCOPE i32 DqnChar_FindNextLine(char *ptr, i32 *line_len)
{
    i32 len = 0;
    ptr     = DqnChar_SkipWhitespace(ptr);

    // Advance pointer to first new line
    while (ptr && *ptr != 0 && *ptr != '\r' && *ptr != '\n')
    {
        ptr++;
        len++;
    }

    if (!ptr || *ptr == 0)
    {
        if (line_len) *line_len = len;
        return -1;
    }

    // Destroy all new lines
    i32 extra_chars = 0;
    while (ptr && (*ptr == '\r' || *ptr == '\n' || *ptr == ' '))
    {
        *ptr = 0;
        ptr++;
        extra_chars++;
    }

    if (line_len) *line_len = len;
    return len + extra_chars;
}

DQN_FILE_SCOPE char *DqnChar_GetNextLine (char *ptr, i32 *line_len)
{
    i32 offset_to_next_line = DqnChar_FindNextLine(ptr, line_len);

    char *result = nullptr;
    if (offset_to_next_line != -1)
    {
        result = ptr + offset_to_next_line;
    }

    return result;
}

// #DqnStr Implementation
// =================================================================================================
DQN_FILE_SCOPE i32 DqnStr_Cmp(char const *a, char const *b, i32 num_bytes_to_cmp, Dqn::IgnoreCase ignore)
{
    if (!a || !b)               return -1;
    if (!a)                     return -b[0];
    if (!b)                     return -a[0];
    if (num_bytes_to_cmp == 0) return 0;

    i32 bytes_cmped = 0;
    i32 result        = 0;
    if (ignore == Dqn::IgnoreCase::Yes)
    {
        while (a[0] && (DqnChar_ToLower(a[0]) == DqnChar_ToLower(b[0])))
        {
            a++; b++;
            if (++bytes_cmped == num_bytes_to_cmp) return 0;
        }
        result = DqnChar_ToLower(a[0]) - DqnChar_ToLower(b[0]);
    }
    else
    {
        while (a[0] && (a[0] == b[0]))
        {
            a++; b++;
            if (++bytes_cmped == num_bytes_to_cmp) return 0;
        }
        result = a[0] - b[0];
    }

    return result;
}

DQN_FILE_SCOPE char *DqnStr_GetPtrToLastSlash(char const *str, i32 str_len)
{
    char const *result       = str;
    if (str_len == -1) str_len = DqnStr_Len(str);

    for (auto i = str_len - 1; i >= 0; i--)
    {
        if (result[i] == '\\' || result[i] == '/')
        {
            result = result + i + 1;
            break;
        }
    }
    return (char *)result;
}

DQN_FILE_SCOPE i32 DqnStr_Len(char const *a)
{
    i32 result = 0;
    while (a && a[result]) result++;
    return result;
}

DQN_FILE_SCOPE i32 DqnStr_LenUTF8(u32 const *a, i32 *len_in_bytes)
{
    i32 utf8_len        = 0;
    i32 utf8_len_in_bytes = 0;
    u8 *byte_ptr        = (u8 *)a;
    while (true)
    {
        u32 codepoint           = 0;
        i32 num_bytes_in_codepoint = DqnStr_ReadUTF8Codepoint((u32 *)byte_ptr, &codepoint);

        if (num_bytes_in_codepoint == 0) break;
        utf8_len++;
        byte_ptr        += num_bytes_in_codepoint;
        utf8_len_in_bytes += num_bytes_in_codepoint;
    }

    if (len_in_bytes) *len_in_bytes = utf8_len_in_bytes;
    return utf8_len;
}

DQN_FILE_SCOPE i32 DqnStr_LenDelimitWith(char const *a, char delimiter)
{
    i32 result = 0;
    while (a && a[result] && a[result] != delimiter) result++;
    return result;
}

DQN_FILE_SCOPE i32 DqnStr_ReadUTF8Codepoint(u32 const *a, u32 *out_codepoint)
{
    u8 *byte = (u8 *)a;
    if (a && byte[0])
    {
        i32 num_bytes_in_char = 0;
        u32 actual_char     = 0;

        if (byte[0] <= 128)
        {
            actual_char     = byte[0];
            num_bytes_in_char = 1;
        }
        else if ((byte[0] & 0xE0) == 0xC0)
        {
            // Header 110xxxxx 10xxxxxx
            actual_char     = ((u32)(byte[0] & 0x3F) << 6)
                           | ((u32)(byte[1] & 0x1F) << 0);
            num_bytes_in_char = 2;
        }
        else if ((byte[0] & 0xF0) == 0xE0)
        {
            // Header 1110xxxx 10xxxxxx 10xxxxxx
            actual_char     = ((u32)(byte[0] & 0x0F) << 12)
                           | ((u32)(byte[1] & 0x3F) << 6 )
                           | ((u32)(byte[2] & 0x3F) << 0 );
            num_bytes_in_char = 3;
        }
        else if ((byte[0] & 0xF8) == 0xF0)
        {
            // Header 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            actual_char = ((u32)(byte[0] & 0x07) << 18)
                       | ((u32)(byte[1] & 0x3F) << 12)
                       | ((u32)(byte[2] & 0x3F) << 6 )
                       | ((u32)(byte[3] & 0x3F) << 0 );
            num_bytes_in_char = 4;
        }
        else
        {
            // NOTE: Malformed utf8 stream
        }

        if (out_codepoint) *out_codepoint = actual_char;
        return num_bytes_in_char;
    }

    return 0;
}

DQN_FILE_SCOPE void DqnStr_Reverse(char *buf, isize buf_size)
{
    if (!buf) return;
    isize mid = buf_size / 2;

    for (isize i = 0; i < mid; i++)
    {
        char tmp               = buf[i];
        buf[i]                 = buf[(buf_size - 1) - i];
        buf[(buf_size - 1) - i] = tmp;
    }
}

DQN_FILE_SCOPE bool DqnStr_EndsWith(char const *src, i32 src_len, char const *find, i32 find_len,
                                    Dqn::IgnoreCase ignore)
{
    if (!src || !find || find_len < 0 || src_len < 0) return false;

    if (src_len < find_len)
        return false;

    char const *src_end       = src + (src_len);
    char const *check_src_from = src_end - find_len;

    bool result = (DqnStr_Cmp(check_src_from, find, find_len, ignore) == 0);
    return result;
}

DQN_FILE_SCOPE i32 DqnStr_FindFirstOccurence(char const *src, i32 src_len,
                                             char const *find, i32 find_len, Dqn::IgnoreCase ignore)
{
    if (!src || !find)               return -1;
    if (src_len == 0 || find_len == 0) return -1;
    if (src_len < find_len)            return -1;

    for (i32 index_in_src = 0; index_in_src < src_len; index_in_src++)
    {
        // NOTE: As we scan through, if the src string we index into becomes
        // shorter than the substring we're checking then the substring is not
        // contained in the src string.
        i32 remaining_len_in_src = src_len - index_in_src;
        if (remaining_len_in_src < find_len) break;

        const char *src_substr = src + index_in_src;
        if (DqnStr_Cmp(src_substr, find, find_len, ignore) == 0)
        {
            return index_in_src;
        }
    }

    // NOTE(doyle): We have early exit, if we reach here, then the substring was
    // not found.
    return -1;
}

DQN_FILE_SCOPE char *DqnStr_GetFirstOccurence(char const *src, i32 src_len, char const *find,
                                              i32 find_len, Dqn::IgnoreCase ignore)
{
    i32 offset = DqnStr_FindFirstOccurence(src, src_len, find, find_len, ignore);
    if (offset == -1) return nullptr;

    char *result = (char *)(src + offset);
    return result;
}

DQN_FILE_SCOPE bool DqnStr_HasSubstring(char const *src, i32 src_len,
                                        char const *find, i32 find_len, Dqn::IgnoreCase ignore)
{
    if (DqnStr_FindFirstOccurence(src, src_len, find, find_len, ignore) == -1)
        return false;

    return true;
}

DQN_FILE_SCOPE i32 Dqn_I64ToStr(i64 value, char *buf, i32 buf_size)
{
    bool valid_buf = true;
    if (!buf || buf_size == 0) valid_buf = false;

    if (value == 0)
    {
        if (valid_buf)
        {
            buf[0] = '0';
            buf[1] = 0;
        }

        return 1;
    }
    
    i32 char_index = 0;
    bool negative           = false;
    if (value < 0) negative = true;

    if (negative)
    {
        if (valid_buf) buf[char_index] = '-';
        char_index++;
    }

    bool last_digit_decremented = false;
    i64 val = DQN_ABS(value);
    if (val < 0)
    {
        // TODO(doyle): This will occur if we are checking the smallest number
        // possible in i64 since the range of negative numbers is one more than
        // it is for positives, so ABS will fail.
        last_digit_decremented = true;
        val                  = DQN_ABS(val - 1);
        DQN_ASSERT(val >= 0);
    }

    if (valid_buf)
    {
        if (last_digit_decremented)
        {
            i64 rem = (val % 10) + 1;
            buf[char_index++] = (u8)rem + '0';
            val /= 10;
        }

        while (val != 0 && char_index < buf_size)
        {
            i64 rem          = val % 10;
            buf[char_index++] = (u8)rem + '0';
            val /= 10;
        }

        // NOTE(doyle): If string is negative, we only want to reverse starting
        // from the second character, so we don't put the negative sign at the
        // end
        if (negative)
        {
            DqnStr_Reverse(buf + 1, char_index - 1);
        }
        else
        {
            DqnStr_Reverse(buf, char_index);
        }
    }
    else
    {
        while (val != 0)
        {
            val /= 10;
            char_index++;
        }
    }

    buf[char_index] = 0;
    return char_index;
}

DQN_FILE_SCOPE i64 Dqn_StrToI64(char const *buf, i64 buf_size)
{
    if (!buf || buf_size == 0) return 0;

    i64 index = 0;
    while (buf[index] == ' ')
    {
        index++;
    }

    bool is_minus = false;
    if (buf[index] == '-' || buf[index] == '+')
    {
        if (buf[index] == '-') is_minus = true;
        index++;
    }
    else if (!DqnChar_IsDigit(buf[index]))
    {
        return 0;
    }

    i64 result = 0;
    for (auto i = index; i < buf_size; i++)
    {
        if (DqnChar_IsDigit(buf[i]))
        {
            result *= 10;
            result += (buf[i] - '0');
        }
        else
        {
            break;
        }
    }

    if (is_minus) result *= -1;

    return result;
}

DQN_FILE_SCOPE f32 Dqn_StrToF32(char const *buf, i64 buf_size)
{
    if (!buf || buf_size == 0) return 0;

    i64 index       = 0;
    bool is_minus = false;
    if (buf[index] == '-')
    {
        index++;
        is_minus = true;
    }

    bool is_past_decimal        = false;
    i64 num_digits_after_decimal = 0;
    i64 raw_num             = 0;

    f32 digit_shift_val      = 1.0f;
    f32 digit_shift_multiplier = 0.1f;
    for (auto i = index; i < buf_size; i++)
    {
        char ch = buf[i];
        if (ch == '.')
        {
            is_past_decimal = true;
            continue;
        }
        // Handle scientific notation
        else if (ch == 'e')
        {
            bool digit_shift_is_positive = true;
            if (i < buf_size)
            {
                if (buf[i + 1] == '-') digit_shift_is_positive = false;
                DQN_ASSERT(buf[i + 1] == '-' || buf[i + 1] == '+');
                i += 2;
            }

            i32 exponent_pow         = 0;
            bool scientific_notation = false;
            while (i < buf_size)
            {
                scientific_notation = true;
                char exponent_char    = buf[i];
                if (DqnChar_IsDigit(exponent_char))
                {
                    exponent_pow *= 10;
                    exponent_pow += (buf[i] - '0');
                }
                else
                {
                    i = buf_size;
                }

                i++;
            }

            // NOTE(doyle): If exponent not specified but this branch occurred,
            // the float string has a malformed scientific notation in the
            // string, i.e. "e" followed by no number.
            DQN_ASSERT(scientific_notation);

            if (digit_shift_is_positive)
            {
                num_digits_after_decimal -= exponent_pow;
            }
            else
            {
                num_digits_after_decimal += exponent_pow;
            }
        }
        else if (DqnChar_IsDigit(ch))
        {
            num_digits_after_decimal += (i32)is_past_decimal;
            raw_num *= 10;
            raw_num += (ch - '0');
        }
        else
        {
            break;
        }
    }

    for (isize i = 0; i < num_digits_after_decimal; i++)
        digit_shift_val *= digit_shift_multiplier;

    f32 result = (f32)raw_num;
    if (num_digits_after_decimal > 0) result *= digit_shift_val;
    if (is_minus) result *= -1;

    return result;
}

/*
    Encoding
    The following byte sequences are used to represent a character. The sequence
    to be used depends on the UCS code number of the character:

    The extra 1's are the headers used to identify the string as a UTF-8 string.
    UCS [0x00000000, 0x0000007F] -> UTF-8 0xxxxxxx
    UCS [0x00000080, 0x000007FF] -> UTF-8 110xxxxx 10xxxxxx
    UCS [0x00000800, 0x0000FFFF] -> UTF-8 1110xxxx 10xxxxxx 10xxxxxx
    UCS [0x00010000, 0x001FFFFF] -> UTF-8 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    UCS [0x00200000, 0x03FFFFFF] -> N/A   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    UCS [0x04000000, 0x7FFFFFFF] -> N/A   1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

    The xxx bit positions are filled with the bits of the character code number
    in binary representation. Only the shortest possible multibyte sequence
    which can represent the code number of the character can be used.

    The UCS code values 0xd800–0xdfff (UTF-16 surrogates) as well as 0xfffe and
    0xffff (UCS noncharacters) should not appear in conforming UTF-8 streams.
*/
DQN_FILE_SCOPE u32 Dqn_UCSToUTF8(u32 *dest, u32 character)
{
    if (!dest) return 0;

    u8 *byte_ptr = (u8 *)dest;

    // Character is within ASCII range, so it's an ascii character
    // UTF Bit Arrangement: 0xxxxxxx
    // Character          : 0xxxxxxx
    if (character >= 0 && character < 0x80)
    {
        byte_ptr[0] = (u8)character;
        return 1;
    }

    // UTF Header Bits    : 11000000 00xxxxxx
    // UTF Bit Arrangement: 000xxxxx 00xxxxxx
    // Character          : 00000xxx xxxxxxxx
    if (character < 0x800)
    {
        // Add the 2nd byte, 6 bits, OR the 0xC0 (11000000) header bits
        byte_ptr[1] = (u8)((character >> 6) | 0xC0);

        // Add the 1st byte, 6 bits, plus the 0x80 (10000000) header bits
        byte_ptr[0] = (u8)((character & 0x3F) | 0x80);

        return 2;
    }

    // UTF Header Bits     : 11100000 10000000 10000000
    // UTF Bit Arrangement : 0000xxxx 00xxxxxx 00xxxxxx
    // Character           : 00000000 xxxxxxxx xxxxxxxx
    if (character < 0x10000)
    {
        // Add the 3rd byte, 4 bits, OR the 0xE0 (11100000) header bits
        byte_ptr[2] = (u8)((character >> 12) | 0xE0);

        // Add the 2nd byte, 6 bits, OR the 0x80 (10000000) header bits
        byte_ptr[1] = (u8)((character >> 6) | 0x80);

        // Add the 1st byte, 6 bits, plus the 0x80 (10000000) header bits
        byte_ptr[0] = (u8)((character & 0x3F) | 0x80);

        return 3;
    }

    // UTF Header Bits     : 11110000 10000000 10000000 10000000
    // UTF Bit Arrangement : 00000xxx 00xxxxxx 00xxxxxx 00xxxxxx
    // Character           : 00000000 00000xxx xxxxxxxx xxxxxxxx
    if (character < 0x110000)
    {
        // Add the 4th byte, 3 bits, OR the 0xF0 (11110000) header bits
        byte_ptr[3] = (u8)((character >> 18) | 0xF0);

        // Add the 3rd byte, 6 bits, OR the 0x80 (10000000) header bits
        byte_ptr[2] = (u8)(((character >> 12) & 0x3F) | 0x80);

        // Add the 2nd byte, 6 bits, plus the 0x80 (10000000) header bits
        byte_ptr[1] = (u8)(((character >> 6) & 0x3F) | 0x80);

        // Add the 2nd byte, 6 bits, plus the 0x80 (10000000) header bits
        byte_ptr[0] = (u8)((character & 0x3F) | 0x80);

        return 4;
    }

    return 0;
}

DQN_FILE_SCOPE u32 Dqn_UTF8ToUCS(u32 *dest, u32 character)
{
    if (!dest) return 0;

    const u32 HEADER_BITS_4_BYTES = 0xF0808080u;
    const u32 HEADER_BITS_3_BYTES = 0xE08080u;
    const u32 HEADER_BITS_2_BYTES = 0xC000u;
    const u32 HEADER_BITS_1_BYTE  = 0x80u;

    // UTF Header Bits     : 11110000 10000000 10000000 10000000
    // UTF Bit Arrangement : 00000xxx 00xxxxxx 00xxxxxx 00xxxxxx
    // UCS                 : 00000000 00000xxx xxxxxxxx xxxxxxxx
    if ((character & HEADER_BITS_4_BYTES) == HEADER_BITS_4_BYTES)
    {
        u32 utf_without_header = HEADER_BITS_4_BYTES ^ character;

        u32 first_byte  = utf_without_header & 0x3F;
        u32 second_byte = (utf_without_header >> 8) & 0x3F;
        u32 third_byte  = (utf_without_header >> 16) & 0x3F;
        u32 fourth_byte = utf_without_header >> 24;

        u32 result =
            (fourth_byte << 18 | third_byte << 12 | second_byte << 6 | first_byte);
        *dest = result;

        return 4;
    }

    // UTF Header Bits     : 11100000 10000000 10000000
    // UTF Bit Arrangement : 0000xxxx 00xxxxxx 00xxxxxx
    // UCS                 : 00000000 xxxxxxxx xxxxxxxx
    if ((character & HEADER_BITS_3_BYTES) == HEADER_BITS_3_BYTES)
    {
        u32 utf_without_header = HEADER_BITS_3_BYTES ^ character;

        u32 first_byte  = utf_without_header & 0x3F;
        u32 second_byte = (utf_without_header >> 8) & 0x3F;
        u32 third_byte  = utf_without_header >> 16;

        u32 result = (third_byte << 12 | second_byte << 6 | first_byte);
        *dest = result;

        return 3;
    }

    // UTF Header Bits    : 11000000 00xxxxxx
    // UTF Bit Arrangement: 000xxxxx 00xxxxxx
    // UCS                : 00000xxx xxxxxxxx
    if ((character & HEADER_BITS_2_BYTES) == HEADER_BITS_2_BYTES)
    {
        u32 utf_without_header = HEADER_BITS_2_BYTES ^ character;

        u32 first_byte  = utf_without_header & 0x3F;
        u32 second_byte = utf_without_header >> 8;

        u32 result = (second_byte << 6 | first_byte);
        *dest = result;

        return 2;
    }

    // Character is within ASCII range, so it's an ascii character
    // UTF Bit Arrangement: 0xxxxxxx
    // UCS                : 0xxxxxxx
    if ((character & HEADER_BITS_1_BYTE) == 0)
    {
        u32 first_byte = (character & 0x3F);
        *dest          = first_byte;

        return 1;
    }

    return 0;
}

DQN_FILE_SCOPE DqnSlice<char> DqnStr_RemoveLeadTrailChar(char const *str, i32 str_len, char lead_char, char trail_char)
{
    str = DqnChar_TrimWhitespaceAround(str, str_len, &str_len);

    if (str[0] == lead_char)
    {
        str++;
        str_len--;
    }

    if (str[str_len - 1] == trail_char)
    {
        str_len--;
    }

    str = DqnChar_TrimWhitespaceAround(str, str_len, &str_len);
    DqnSlice<char> result = {(char *)str, str_len};
    return result;
}

DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailBraces(char const *str, i32 str_len)
{
    DqnSlice<char> result = DqnStr_RemoveLeadTrailChar(str, str_len, '{', '}');
    return result;
}

DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailQuotes(char const *str, i32 str_len)
{
    DqnSlice<char> result = DqnStr_RemoveLeadTrailChar(str, str_len, '"', '"');
    return result;
}

DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailBraces(DqnSlice<char> slice)
{
    DqnSlice<char> result = DqnStr_RemoveLeadTrailBraces(slice.data, slice.len);
    return result;
}

DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailQuotes(DqnSlice<char> slice)
{
    DqnSlice<char> result = DqnStr_RemoveLeadTrailQuotes(slice.data, slice.len);
    return result;
}

// #DqnWChar
// =================================================================================================
DQN_FILE_SCOPE bool DqnWChar_IsDigit(wchar_t c)
{
    if (c >= L'0' && c <= L'9') return true;
    return false;
}

DQN_FILE_SCOPE wchar_t DqnWChar_ToLower(wchar_t c)
{
    if (c >= L'A' && c <= L'Z')
    {
        i32 shift_offset = L'a' - L'A';
        return (c + (wchar_t)shift_offset);
    }

    return c;
}

DQN_FILE_SCOPE wchar_t *DqnWChar_SkipWhitespace(wchar_t *ptr)
{
    while (ptr && (*ptr == ' ' || *ptr == '\r' || *ptr == '\n')) ptr++;
    return ptr;
}

DQN_FILE_SCOPE wchar_t *DqnWChar_FindLastChar(wchar_t *ptr, wchar_t ch, i32 len, u32 *len_to_char)
{
    for (i32 i = len - 1; i >= 0; i--)
    {
        if (ptr[i] == ch)
        {
            if (len_to_char) *len_to_char = (u32)len - i;
            return &ptr[i];
        }
    }

    return nullptr;
}

DQN_FILE_SCOPE i32 DqnWChar_GetNextLine(wchar_t *ptr, i32 *line_len)
{
    i32 len = 0;
    ptr     = DqnWChar_SkipWhitespace(ptr);

    // Advance pointer to first new line
    while (ptr && *ptr != 0 && *ptr != '\r' && *ptr != '\n')
    {
        ptr++;
        len++;
    }

    if (!ptr || *ptr == 0) return -1;

    // Destroy all new lines
    i32 extra_chars = 0;
    while (ptr && (*ptr == '\r' || *ptr == '\n' || *ptr == ' '))
    {
        *ptr = 0;
        ptr++;
        extra_chars++;
    }

    if (line_len) *line_len = len;
    return len + extra_chars;
}

// #DqnWStr
// =================================================================================================
DQN_FILE_SCOPE i32 DqnWStr_Cmp(wchar_t const *a, wchar_t const *b)
{
    if (!a && !b) return -1;
    if (!a) return -1;
    if (!b) return -1;

    const wchar_t *a_ptr = a;
    const wchar_t *b_ptr = b;

    while ((*a_ptr) == (*b_ptr))
    {
        if (!(*a_ptr)) return 0;
        a_ptr++;
        b_ptr++;
    }

    return (((*a_ptr) < (*b_ptr)) ? -1 : 1);
}

DQN_FILE_SCOPE i32 DqnWStr_FindFirstOccurence(wchar_t const *src, i32 src_len,
                                              wchar_t const *find, i32 find_len)
{
    if (!src || !find)               return -1;
    if (src_len == 0 || find_len == 0) return -1;
    if (src_len < find_len)            return -1;

    for (i32 index_in_src = 0; index_in_src < src_len; index_in_src++)
    {
        // NOTE: As we scan through, if the src string we index into becomes
        // shorter than the substring we're checking then the substring is not
        // contained in the src string.
        i32 remaining_len_in_src = src_len - index_in_src;
        if (remaining_len_in_src < find_len) break;

        const wchar_t *src_substr = &src[index_in_src];
        i32 index = 0;
        for (;;)
        {
            if (DqnWChar_ToLower(src_substr[index]) ==
                DqnWChar_ToLower(find[index]))
            {
                index++;
                if (index >= find_len || !find[index])
                {
                    return index_in_src;
                }
            }
            else
            {
                break;
            }
        }
    }

    // NOTE(doyle): We have early exit, if we reach here, then the substring was
    // not found.
    return -1;
}

DQN_FILE_SCOPE bool DqnWStr_HasSubstring(wchar_t const *src, i32 src_len,
                                         wchar_t const *find, i32 find_len)
{
    if (DqnWStr_FindFirstOccurence(src, src_len, find, find_len) == -1)
        return false;

    return true;
}

DQN_FILE_SCOPE i32 DqnWStr_Len(wchar_t const *a)
{
    i32 result = 0;
    while (a && a[result]) result++;
    return result;
}

DQN_FILE_SCOPE i32 DqnWStr_LenDelimitWith(wchar_t const *a, wchar_t delimiter)
{
    i32 result = 0;
    while (a && a[result] && a[result] != delimiter) result++;
    return result;
}

DQN_FILE_SCOPE void DqnWStr_Reverse(wchar_t *buf, i32 buf_size)
{
    if (!buf) return;
    i32 mid = buf_size / 2;

    for (i32 i = 0; i < mid; i++)
    {
        wchar_t tmp            = buf[i];
        buf[i]                 = buf[(buf_size - 1) - i];
        buf[(buf_size - 1) - i] = tmp;
    }
}

DQN_FILE_SCOPE i32 Dqn_WStrToI32(wchar_t const *buf, i32 buf_size)
{
    if (!buf || buf_size == 0) return 0;

    i32 index       = 0;
    bool is_minus = false;
    if (buf[index] == L'-' || buf[index] == L'+')
    {
        if (buf[index] == L'-') is_minus = true;
        index++;
    }
    else if (!DqnWChar_IsDigit(buf[index]))
    {
        return 0;
    }

    i32 result = 0;
    for (i32 i = index; i < buf_size; i++)
    {
        if (DqnWChar_IsDigit(buf[i]))
        {
            result *= 10;
            result += (buf[i] - L'0');
        }
        else
        {
            break;
        }
    }

    if (is_minus) result *= -1;

    return result;
}

DQN_FILE_SCOPE i32 Dqn_I32ToWstr(i32 value, wchar_t *buf, i32 buf_size)
{
    if (!buf || buf_size == 0) return 0;

    if (value == 0)
    {
        buf[0] = L'0';
        return 0;
    }

    // NOTE(doyle): Max 32bit integer (+-)2147483647
    i32 char_index = 0;
    bool negative           = false;
    if (value < 0) negative = true;

    if (negative) buf[char_index++] = L'-';

    i32 val = DQN_ABS(value);
    while (val != 0 && char_index < buf_size)
    {
        i32 rem          = val % 10;
        buf[char_index++] = (u8)rem + '0';
        val /= 10;
    }

    // NOTE(doyle): If string is negative, we only want to reverse starting
    // from the second character, so we don't put the negative sign at the end
    if (negative)
    {
        DqnWStr_Reverse(buf + 1, char_index - 1);
    }
    else
    {
        DqnWStr_Reverse(buf, char_index);
    }

    return char_index;
}

// #DqnRnd
// =================================================================================================
// Public Domain library with thanks to Mattias Gustavsson
// https://github.com/mattiasgustavsson/libs/blob/master/docs/rnd.md

// Convert a randomized u32 value to a float value x in the range 0.0f <= x
// < 1.0f. Contributed by Jonatan Hedborg

// NOTE: This is to abide to strict aliasing rules.
union DqnRnd__U32F32
{
    u32 unsigned32;
    f32 float32;
};

FILE_SCOPE f32 DqnRnd__F32NormalizedFromU32(u32 value)
{
    u32 exponent = 127;
    u32 mantissa = value >> 9;

    union DqnRnd__U32F32 uf;
    uf.unsigned32 = (exponent << 23 | mantissa);
    return uf.float32 - 1.0f;
}

FILE_SCOPE u64 DqnRnd__Murmur3Avalanche64(u64 h)
{
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccd;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53;
    h ^= h >> 33;
    return h;
}

#if defined(DQN_IS_UNIX)
    #include <x86intrin.h> // __rdtsc
#endif

FILE_SCOPE u32 DqnRnd__MakeSeed()
{
#if defined(DQN_PLATFORM_IMPLEMENTATION) && (defined(DQN_IS_WIN32) || defined(DQN_IS_UNIX))
    i64 num_clock_cycles = __rdtsc();
    return (u32)num_clock_cycles;
#else
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return 0;
#endif
}

DqnRndPCG::DqnRndPCG()
{
    u32 seed = DqnRnd__MakeSeed();
    *this    = DqnRndPCG(seed);
}

DqnRndPCG::DqnRndPCG(u32 seed)
{
    u64 value      = (((u64)seed) << 1ULL) | 1ULL;
    value          = DqnRnd__Murmur3Avalanche64(value);
    this->state[0] = 0U;
    this->state[1] = (value << 1ULL) | 1ULL;
    this->Next();
    this->state[0] += DqnRnd__Murmur3Avalanche64(value);
    this->Next();
}

u32 DqnRndPCG::Next()
{
    u64 oldstate   = this->state[0];
    this->state[0] = oldstate * 0x5851f42d4c957f2dULL + this->state[1];
    u32 xorshifted = (u32)(((oldstate >> 18ULL) ^ oldstate) >> 27ULL);
    u32 rot        = (u32)(oldstate >> 59ULL);
    return (xorshifted >> rot) | (xorshifted << ((-(i32)rot) & 31));
}

f32 DqnRndPCG::Nextf()
{
    f32 result = DqnRnd__F32NormalizedFromU32(this->Next());
    return result;
}

i32 DqnRndPCG::Range(i32 min, i32 max)
{
    i32 const range = (max - min) + 1;
    if (range <= 0) return min;

    i32 const value = (i32)(this->Nextf() * range);
    i32 result      = min + value;
    return result;
}


// #Dqn_*
// =================================================================================================
DQN_FILE_SCOPE inline bool Dqn_BitIsSet(u32 bits, u32 flag)
{
    bool result = ((bits & flag) == flag);
    return result;
}

DQN_FILE_SCOPE inline u32 Dqn_BitSet(u32 bits, u32 flag)
{
    u32 result = (bits | flag);
    return result;
}

DQN_FILE_SCOPE inline u32 Dqn_BitUnset(u32 bits, u32 flag)
{
    u32 result = (bits & ~flag);
    return result;
}

DQN_FILE_SCOPE inline u32 Dqn_BitToggle(u32 bits, u32 flag)
{
    u32 result = (bits ^ flag);
    return result;
}

// #DqnString Impleemntation
// =================================================================================================
int DqnString::VAListLen(char const *fmt, va_list va)
{
    LOCAL_PERSIST char tmp[STB_SPRINTF_MIN];
    auto PrintCallback = [](char *buf, void * /*user*/, int /*len*/) -> char * { return buf; };
    int result = Dqn_vsprintfcb(PrintCallback, nullptr, tmp, fmt, va);
    return result;
}

void DqnString::Reserve(int new_max)
{
    if (new_max >= this->max)
    {
        char *new_ptr =
            (this->str)
                ? static_cast<char *>(mem_api->Realloc(this->str, sizeof(this->str[0]) * this->len, new_max * sizeof(this->str[0])))
                : static_cast<char *>(mem_api->Alloc(new_max * sizeof(this->str[0])));

        DQN_ALWAYS_ASSERT(new_ptr);
        this->str = new_ptr;
        this->max = new_max;
    }
}

void DqnString::Append(char const *src, int len_)
{
    if (len_ == -1)
        len_ = DqnStr_Len(src);

    Reserve(this->len + len_);
    DqnMem_Copy(this->str + this->len, src, len_);
    this->len += len_;
    this->str[this->len] = 0;
}

// #DqnFixedString Implementation
// =================================================================================================
// return: The number of bytes written to dest
FILE_SCOPE int DqnFixedString__Append(char *dest, int dest_size, char const *src, int len)
{
    if (len <= -1)
    {
        char *ptr = dest;
        char *end = ptr + dest_size;

        while (*src && ptr != end)
            *ptr++ = *src++;

        if (ptr == end)
        {
            DQN_ASSERT(!src[0]);
        }

        len = static_cast<i32>(ptr - dest);
    }
    else
    {
        DqnMem_Copy(dest, src, len);
    }

    DQN_ASSERT(len < dest_size && len >= 0);
    dest[len] = 0;
    return len;
}

// #DqnLogger Implementation
// =================================================================================================
char const *DqnLogger::Log(Type type, Context const log_context, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    DQN_DEFER(va_end(va));

    LOCAL_PERSIST DqnFixedString2048 fmt_msg;
    fmt_msg.Clear();

    char const *stripped_filename = nullptr;
    for (size_t i = log_context.filename_len; i >= 0 && !stripped_filename; i--)
        stripped_filename = (log_context.filename[i] == '\\') ? log_context.filename + (i + 1) : nullptr;

    fmt_msg.SprintfAppend("%s|%05d|%s| `%s`: ",
                          (stripped_filename) ? stripped_filename : log_context.filename,
                          log_context.line_number,
                          TypePrefix(type),
                          log_context.function);

    fmt_msg.SprintfAppend("%s", this->log_builder.str);
    this->log_builder.Clear();

    fmt_msg.VSprintfAppend(fmt, va);
    fmt_msg.SprintfAppend("\n");
    char const *result = fmt_msg.str;

    if (this->no_console) return result;
    if (this->no_print_error && type == Type::Error) return result;
    if (this->no_print_debug && type == Type::Debug) return result;
    if (this->no_print_warning && type == Type::Warning) return result;

    fprintf(stderr, "%s", result);
    return result;
}

// #Dqn
// =================================================================================================
i32 Dqn_GetNumSplits(char const *src, i32 src_len, char split_char)
{
    auto result = Dqn_SplitString(src, src_len, split_char, nullptr, 0);
    return result;
}

i32 Dqn_SplitString(char const *src, i32 src_len, char split_char, DqnSlice<char> *array, i32 size)
{
    // TODO(doyle): Const correctness
    i32 slice_len   = 0;
    i32 array_index = 0;
    for (auto i = 0; i < src_len; i++)
    {
        char *c = (char *)(src + i);
        if (*c == split_char)
        {
            DqnSlice<char> slice = {c - slice_len, slice_len};
            if (array)
            {
                if (array_index < size)
                {
                    array[array_index] = slice;
                }
            }
            array_index++;
            slice_len = 0;
        }
        else
        {
            slice_len++;
        }
    }

    DqnSlice<char> last_slice = {(char *)src + src_len - slice_len, slice_len};
    if (last_slice.len > 0 && array_index > 0)
    {
        if (array)
        {
            if (array_index < size)
            {
                array[array_index] = last_slice;
            }
        }

        array_index++;
    }

    return array_index;
}

// TODO(doyle): This should maybe be a tokenizer ...
DQN_FILE_SCOPE DqnJson DqnJson_Get(char const *buf, i32 buf_len, char const *find_property, i32 find_property_len)
{
    DqnJson result = {};
    if (!buf || buf_len == 0 || !find_property || find_property_len == 0) return result;

    char const *tmp = DqnChar_SkipWhitespace(buf);
    buf_len          = static_cast<int>((buf + buf_len) - tmp);
    buf             = tmp;

    bool const find_structure_in_global_scope = (find_property_len == 1 && (find_property[0] == '{' || find_property[0] == '['));

    if ((buf[0] == '{' || buf[1] == '[') && !find_structure_in_global_scope)
    {
        buf++;
        buf_len--;
    }

TryNext:
    char const *locate = nullptr;
    for (i32 index_in_buf = 0; index_in_buf < buf_len; ++index_in_buf)
    {
        i32 remaining_len_in_src = buf_len - index_in_buf;
        if (remaining_len_in_src < find_property_len) break;

        char const *buf_substr = buf + index_in_buf;
        if (!find_structure_in_global_scope)
        {
            if (buf_substr[0] == '{' || buf_substr[0] == '[')
            {
                int bracket_count     = 0;
                int brace_count       = 0;
                int *search_char_count = nullptr;
                if (buf_substr[0] == '[')
                {
                    bracket_count++;
                    search_char_count = &bracket_count;
                }
                else
                {
                    brace_count++;
                    search_char_count = &brace_count;
                }

                for (++index_in_buf; (*search_char_count) != 0; ++index_in_buf)
                {
                    buf_substr = buf + index_in_buf;
                    if (!buf_substr[0])
                        return result;

                    if      (buf_substr[0] == '{') ++brace_count;
                    else if (buf_substr[0] == '}') --brace_count;
                    else if (buf_substr[0] == '[') ++bracket_count;
                    else if (buf_substr[0] == ']') --bracket_count;
                    else                         continue;
                }
            }
        }

        if (DqnStr_Cmp(buf_substr, find_property, find_property_len, Dqn::IgnoreCase::No) == 0)
        {
            locate = buf + index_in_buf;
            break;
        }
    }
    if (!locate) return result;

    // NOTE: if find property is '{' we are looking for an object in array or the global scope etc
    // which doesn't have a specific property name
    char const *start_of_val = locate;
    char const *buf_ptr = start_of_val;
    if (!find_structure_in_global_scope)
    {
        // NOTE: When true, the find_property already includes the quotation marks, so don't need to check.
        if (!(find_property[0] == '"' && find_property[find_property_len - 1] == '"'))
        {
            if (locate[-1] != '"' || locate[find_property_len] != '"')
            {
                buf_len -= static_cast<i32>(((locate - buf) + find_property_len));
                buf     = locate + find_property_len;
                goto TryNext;
            }
        }

        if (!(locate[find_property_len + 1] && locate[find_property_len + 1] == ':'))
        {
            return result;
        }

        start_of_val = locate + find_property_len + 2;
        start_of_val = DqnChar_SkipWhitespace(start_of_val);
        buf_ptr       = start_of_val;
    }

    i32 brace_count = 0, bracket_count = 0;
    if (buf_ptr[0] == '[' || buf_ptr[0] == '{')
    {
        start_of_val++;

        i32 *search_char_count = nullptr;
        if (*buf_ptr++ == '[')
        {
            bracket_count++;
            search_char_count = &bracket_count;

            while(buf_ptr[0] != '{' && buf_ptr[0] != '[' && buf_ptr[0] != '"' && !DqnChar_IsAlphaNum(buf_ptr[0]) && !buf_ptr[0])
                buf_ptr++;

            if (!buf_ptr[0])
                return result;

            const b32 array_of_primitives = (DqnChar_IsAlphaNum(buf_ptr[0]) || buf_ptr[0] == '"');
            result.type = (array_of_primitives) ? DqnJson::Type::ArrayOfPrimitives : DqnJson::Type::ArrayOfObjects;
        }
        else
        {
            brace_count++;
            result.type       = DqnJson::Type::Object;
            search_char_count = &brace_count;
        }

        if (result.type == DqnJson::Type::ArrayOfPrimitives)
        {
            for (result.num_entries = 1;;)
            {
                while(buf_ptr[0] && (buf_ptr[0] != ',' && buf_ptr[0] != ']'))
                    buf_ptr++;

                if (buf_ptr[0] == ',')
                {
                    result.num_entries++;
                    buf_ptr++;
                    continue;
                }

                if (!buf_ptr[0])
                    return result;

                if (buf_ptr[0] == ']')
                    break;
            }
        }
        else
        {
            for (; (*search_char_count) != 0; ++buf_ptr)
            {
                if (!buf_ptr[0])
                    return result;

                if      (buf_ptr[0] == '{') ++brace_count;
                else if (buf_ptr[0] == '}') --brace_count;
                else if (buf_ptr[0] == '[') ++bracket_count;
                else if (buf_ptr[0] == ']') --bracket_count;
                else                         continue;

                if (result.type == DqnJson::Type::ArrayOfObjects)
                {
                    if (brace_count == 0 && bracket_count == 1)
                    {
                        result.num_entries++;
                    }
                }
                else
                {
                    if (brace_count == 1 && bracket_count == 0)
                    {
                        result.num_entries++;
                    }
                }
            }
            // Don't include the open and closing braces/brackets.
            buf_ptr--;
        }

    }
    else if (buf_ptr[0] == '"' || DqnChar_IsAlphaNum(buf_ptr[0]) || buf_ptr[0] == '-')
    {
        while(buf_ptr[0] && (buf_ptr[0] != '\n' && buf_ptr[0] != ',' && buf_ptr[0] != '}'))
            buf_ptr++;

        if (!buf_ptr[0])
            return result;

        result.num_entries = 1;
    }
    else
    {
        return result;
    }

    result.value.data = (char *)start_of_val;
    result.value.len  = static_cast<i32>(buf_ptr - result.value.data);
    result.value.data = DqnChar_TrimWhitespaceAround(result.value.data, result.value.len, &result.value.len);
    return result;
}

DQN_FILE_SCOPE DqnJson DqnJson_Get(DqnSlice<char> const buf, DqnSlice<char> const find_property)
{
    DqnJson result = DqnJson_Get(buf.data, buf.len, find_property.data, find_property.len);
    return result;
}

DQN_FILE_SCOPE DqnJson DqnJson_Get(DqnSlice<char const> const buf, DqnSlice<char const> const find_property)
{
    DqnJson result = DqnJson_Get(buf.data, buf.len, find_property.data, find_property.len);
    return result;
}

DQN_FILE_SCOPE DqnJson DqnJson_Get(DqnSlice<char> const buf, DqnSlice<char const> const find_property)
{
    DqnJson result = DqnJson_Get(buf.data, buf.len, find_property.data, find_property.len);
    return result;
}

DQN_FILE_SCOPE DqnJson DqnJson_Get(DqnJson const input, DqnSlice<char> const find_property)
{
    DqnJson result = DqnJson_Get(input.value.data, input.value.len, find_property.data, find_property.len);
    return result;
}

DQN_FILE_SCOPE DqnJson DqnJson_Get(DqnJson const input, DqnSlice<char const> const find_property)
{
    DqnJson result = DqnJson_Get(input.value.data, input.value.len, find_property.data, find_property.len);
    return result;
}

DQN_FILE_SCOPE DqnJson DqnJson_GetNextArrayItem(DqnJson *iterator)
{
    DqnJson result = {};
    if (!iterator->IsArray() || iterator->num_entries <= 0)
        return result;

    if (iterator->type == DqnJson::Type::ArrayOfObjects)
    {
        if (result = DqnJson_Get(iterator->value, DQN_SLICE("{")))
        {
            char const *end      = iterator->value.data + iterator->value.len;
            iterator->value.data = result.value.data + result.value.len;
            --iterator->num_entries;

            while (iterator->value.data[0] && *iterator->value.data++ != '}')
                ;

            iterator->value.data = DqnChar_SkipWhitespace(iterator->value.data);
            if (iterator->value.data[0] && iterator->value.data[0] == ',')
                iterator->value.data++;

            iterator->value.data = DqnChar_SkipWhitespace(iterator->value.data);
            iterator->value.len  = (iterator->value.data) ? static_cast<i32>(end - iterator->value.data) : 0;
        }
    }
    else
    {
        char const *end   = iterator->value.data + iterator->value.len;
        result.value.data = iterator->value.data;
        --iterator->num_entries;

        if (iterator->num_entries == 0)
        {
            while (iterator->value.data[0] && iterator->value.data[0] != ']')
                ++iterator->value.data;
        }
        else
        {
            while (iterator->value.data[0] && iterator->value.data[0] != ',')
                ++iterator->value.data;
        }


        result.value.len     = static_cast<i32>(iterator->value.data - result.value.data);
        iterator->value.data = DqnChar_SkipWhitespace(++iterator->value.data);
        iterator->value.len = (iterator->value.data) ? static_cast<i32>(end - iterator->value.data) : 0;
    }

    return result;
}


#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
// #DqnSprintf - STB_Sprintf
// =================================================================================================
#include <stdlib.h> // for va_arg()

#define stbsp__uint32 unsigned int
#define stbsp__int32 signed int

#ifdef _MSC_VER
#define stbsp__uint64 unsigned __int64
#define stbsp__int64 signed __int64
#else
#define stbsp__uint64 unsigned long long
#define stbsp__int64 signed long long
#endif
#define stbsp__uint16 unsigned short

#ifndef stbsp__uintptr
#if defined(__ppc64__) || defined(__aarch64__) || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64)
#define stbsp__uintptr stbsp__uint64
#else
#define stbsp__uintptr stbsp__uint32
#endif
#endif

#ifndef STB_SPRINTF_MSVC_MODE // used for MSVC2013 and earlier (MSVC2015 matches GCC)
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define STB_SPRINTF_MSVC_MODE
#endif
#endif

#ifdef STB_SPRINTF_NOUNALIGNED // define this before inclusion to force stbsp_sprintf to always use aligned accesses
#define STBSP__UNALIGNED(code)
#else
#define STBSP__UNALIGNED(code) code
#endif

#ifndef STB_SPRINTF_NOFLOAT
// internal float utility functions
static stbsp__int32 stbsp__real_to_str(char const **start, stbsp__uint32 *len, char *out, stbsp__int32 *decimal_pos, double value, stbsp__uint32 frac_digits);
static stbsp__int32 stbsp__real_to_parts(stbsp__int64 *bits, stbsp__int32 *expo, double value);
#define STBSP__SPECIAL 0x7000
#endif

static char stbsp__period = '.';
static char stbsp__comma = ',';
static char stbsp__digitpair[201] =
   "0001020304050607080910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576"
   "7778798081828384858687888990919293949596979899";

STBSP__PUBLICDEF void STB_SPRINTF_DECORATE(set_separators)(char pcomma, char pperiod)
{
   stbsp__period = pperiod;
   stbsp__comma = pcomma;
}

#define STBSP__LEFTJUST 1
#define STBSP__LEADINGPLUS 2
#define STBSP__LEADINGSPACE 4
#define STBSP__LEADING_0X 8
#define STBSP__LEADINGZERO 16
#define STBSP__INTMAX 32
#define STBSP__TRIPLET_COMMA 64
#define STBSP__NEGATIVE 128
#define STBSP__METRIC_SUFFIX 256
#define STBSP__HALFWIDTH 512
#define STBSP__METRIC_NOSPACE 1024
#define STBSP__METRIC_1024 2048
#define STBSP__METRIC_JEDEC 4096

static void stbsp__lead_sign(stbsp__uint32 fl, char *sign)
{
   sign[0] = 0;
   if (fl & STBSP__NEGATIVE) {
      sign[0] = 1;
      sign[1] = '-';
   } else if (fl & STBSP__LEADINGSPACE) {
      sign[0] = 1;
      sign[1] = ' ';
   } else if (fl & STBSP__LEADINGPLUS) {
      sign[0] = 1;
      sign[1] = '+';
   }
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va)
{
   static char hex[] = "0123456789abcdefxp";
   static char hexu[] = "0123456789ABCDEFXP";
   char *bf;
   char const *f;
   int tlen = 0;

   bf = buf;
   f = fmt;
   for (;;) {
      stbsp__int32 fw, pr, tz;
      stbsp__uint32 fl;

      // macros for the callback buffer stuff
      #define stbsp__chk_cb_bufL(bytes)                        \
         {                                                     \
            int len = (int)(bf - buf);                         \
            if ((len + (bytes)) >= STB_SPRINTF_MIN) {          \
               tlen += len;                                    \
               if (0 == (bf = buf = callback(buf, user, len))) \
                  goto done;                                   \
            }                                                  \
         }
      #define stbsp__chk_cb_buf(bytes)    \
         {                                \
            if (callback) {               \
               stbsp__chk_cb_bufL(bytes); \
            }                             \
         }
      #define stbsp__flush_cb()                      \
         {                                           \
            stbsp__chk_cb_bufL(STB_SPRINTF_MIN - 1); \
         } // flush if there is even one byte in the buffer
      #define stbsp__cb_buf_clamp(cl, v)                \
         cl = v;                                        \
         if (callback) {                                \
            int lg = STB_SPRINTF_MIN - (int)(bf - buf); \
            if (cl > lg)                                \
               cl = lg;                                 \
         }

      // fast copy everything up to the next % (or end of string)
      for (;;) {
         while (((stbsp__uintptr)f) & 3) {
         schk1:
            if (f[0] == '%')
               goto scandd;
         schk2:
            if (f[0] == 0)
               goto endfmt;
            stbsp__chk_cb_buf(1);
            *bf++ = f[0];
            ++f;
         }
         for (;;) {
            // Check if the next 4 bytes contain %(0x25) or end of string.
            // Using the 'hasless' trick:
            // https://graphics.stanford.edu/~seander/bithacks.html#HasLessInWord
            stbsp__uint32 v, c;
            v = *(stbsp__uint32 *)f;
            c = (~v) & 0x80808080;
            if (((v ^ 0x25252525) - 0x01010101) & c)
               goto schk1;
            if ((v - 0x01010101) & c)
               goto schk2;
            if (callback)
               if ((STB_SPRINTF_MIN - (int)(bf - buf)) < 4)
                  goto schk1;
            *(stbsp__uint32 *)bf = v;
            bf += 4;
            f += 4;
         }
      }
   scandd:

      ++f;

      // ok, we have a percent, read the modifiers first
      fw = 0;
      pr = -1;
      fl = 0;
      tz = 0;

      // flags
      for (;;) {
         switch (f[0]) {
         // if we have left justify
         case '-':
            fl |= STBSP__LEFTJUST;
            ++f;
            continue;
         // if we have leading plus
         case '+':
            fl |= STBSP__LEADINGPLUS;
            ++f;
            continue;
         // if we have leading space
         case ' ':
            fl |= STBSP__LEADINGSPACE;
            ++f;
            continue;
         // if we have leading 0x
         case '#':
            fl |= STBSP__LEADING_0X;
            ++f;
            continue;
         // if we have thousand commas
         case '\'':
            fl |= STBSP__TRIPLET_COMMA;
            ++f;
            continue;
         // if we have kilo marker (none->kilo->kibi->jedec)
         case '$':
            if (fl & STBSP__METRIC_SUFFIX) {
               if (fl & STBSP__METRIC_1024) {
                  fl |= STBSP__METRIC_JEDEC;
               } else {
                  fl |= STBSP__METRIC_1024;
               }
            } else {
               fl |= STBSP__METRIC_SUFFIX;
            }
            ++f;
            continue;
         // if we don't want space between metric suffix and number
         case '_':
            fl |= STBSP__METRIC_NOSPACE;
            ++f;
            continue;
         // if we have leading zero
         case '0':
            fl |= STBSP__LEADINGZERO;
            ++f;
            goto flags_done;
         default: goto flags_done;
         }
      }
   flags_done:

      // get the field width
      if (f[0] == '*') {
         fw = va_arg(va, stbsp__uint32);
         ++f;
      } else {
         while ((f[0] >= '0') && (f[0] <= '9')) {
            fw = fw * 10 + f[0] - '0';
            f++;
         }
      }
      // get the precision
      if (f[0] == '.') {
         ++f;
         if (f[0] == '*') {
            pr = va_arg(va, stbsp__uint32);
            ++f;
         } else {
            pr = 0;
            while ((f[0] >= '0') && (f[0] <= '9')) {
               pr = pr * 10 + f[0] - '0';
               f++;
            }
         }
      }

      // handle integer size overrides
      switch (f[0]) {
      // are we halfwidth?
      case 'h':
         fl |= STBSP__HALFWIDTH;
         ++f;
         break;
      // are we 64-bit (unix style)
      case 'l':
         ++f;
         if (f[0] == 'l') {
            fl |= STBSP__INTMAX;
            ++f;
         }
         break;
      // are we 64-bit on intmax? (c99)
      case 'j':
         fl |= STBSP__INTMAX;
         ++f;
         break;
      // are we 64-bit on size_t or ptrdiff_t? (c99)
      case 'z':
      case 't':
         fl |= ((sizeof(char *) == 8) ? STBSP__INTMAX : 0);
         ++f;
         break;
      // are we 64-bit (msft style)
      case 'I':
         if ((f[1] == '6') && (f[2] == '4')) {
            fl |= STBSP__INTMAX;
            f += 3;
         } else if ((f[1] == '3') && (f[2] == '2')) {
            f += 3;
         } else {
            fl |= ((sizeof(void *) == 8) ? STBSP__INTMAX : 0);
            ++f;
         }
         break;
      default: break;
      }

      // handle each replacement
      switch (f[0]) {
         #define STBSP__NUMSZ 512 // big enough for e308 (with commas) or e-307
         char num[STBSP__NUMSZ];
         char lead[8];
         char tail[8];
         char *s;
         char const *h;
         stbsp__uint32 l, n, cs;
         stbsp__uint64 n64;
#ifndef STB_SPRINTF_NOFLOAT
         double fv;
#endif
         stbsp__int32 dp;
         char const *sn;

      case 's':
         // get the string
         s = va_arg(va, char *);
         if (s == 0)
            s = (char *)"null";
         // get the length
         sn = s;
         for (;;) {
            if ((((stbsp__uintptr)sn) & 3) == 0)
               break;
         lchk:
            if (sn[0] == 0)
               goto ld;
            ++sn;
         }
         n = 0xffffffff;
         if (pr >= 0) {
            n = (stbsp__uint32)(sn - s);
            if (n >= (stbsp__uint32)pr)
               goto ld;
            n = ((stbsp__uint32)(pr - n)) >> 2;
         }
         while (n) {
            stbsp__uint32 v = *(stbsp__uint32 *)sn;
            if ((v - 0x01010101) & (~v) & 0x80808080UL)
               goto lchk;
            sn += 4;
            --n;
         }
         goto lchk;
      ld:

         l = (stbsp__uint32)(sn - s);
         // clamp to precision
         if (l > (stbsp__uint32)pr)
            l = pr;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         // copy the string in
         goto scopy;

      case 'c': // char
         // get the character
         s = num + STBSP__NUMSZ - 1;
         *s = (char)va_arg(va, int);
         l = 1;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;

      case 'n': // weird write-bytes specifier
      {
         int *d = va_arg(va, int *);
         *d = tlen + (int)(bf - buf);
      } break;

#ifdef STB_SPRINTF_NOFLOAT
      case 'A':              // float
      case 'a':              // hex float
      case 'G':              // float
      case 'g':              // float
      case 'E':              // float
      case 'e':              // float
      case 'f':              // float
         va_arg(va, double); // eat it
         s = (char *)"No float";
         l = 8;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;
#else
      case 'A': // hex float
      case 'a': // hex float
         h = (f[0] == 'A') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_parts((stbsp__int64 *)&n64, &dp, fv))
            fl |= STBSP__NEGATIVE;

         s = num + 64;

         stbsp__lead_sign(fl, lead);

         if (dp == -1023)
            dp = (n64) ? -1022 : 0;
         else
            n64 |= (((stbsp__uint64)1) << 52);
         n64 <<= (64 - 56);
         if (pr < 15)
            n64 += ((((stbsp__uint64)8) << 56) >> (pr * 4));
// add leading chars

#ifdef STB_SPRINTF_MSVC_MODE
         *s++ = '0';
         *s++ = 'x';
#else
         lead[1 + lead[0]] = '0';
         lead[2 + lead[0]] = 'x';
         lead[0] += 2;
#endif
         *s++ = h[(n64 >> 60) & 15];
         n64 <<= 4;
         if (pr)
            *s++ = stbsp__period;
         sn = s;

         // print the bits
         n = pr;
         if (n > 13)
            n = 13;
         if (pr > (stbsp__int32)n)
            tz = pr - n;
         pr = 0;
         while (n--) {
            *s++ = h[(n64 >> 60) & 15];
            n64 <<= 4;
         }

         // print the expo
         tail[1] = h[17];
         if (dp < 0) {
            tail[2] = '-';
            dp = -dp;
         } else
            tail[2] = '+';
         n = (dp >= 1000) ? 6 : ((dp >= 100) ? 5 : ((dp >= 10) ? 4 : 3));
         tail[0] = (char)n;
         for (;;) {
            tail[n] = '0' + dp % 10;
            if (n <= 3)
               break;
            --n;
            dp /= 10;
         }

         dp = (int)(s - sn);
         l = (int)(s - (num + 64));
         s = num + 64;
         cs = 1 + (3 << 24);
         goto scopy;

      case 'G': // float
      case 'g': // float
         h = (f[0] == 'G') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6;
         else if (pr == 0)
            pr = 1; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, (pr - 1) | 0x80000000))
            fl |= STBSP__NEGATIVE;

         // clamp the precision and delete extra zeros after clamp
         n = pr;
         if (l > (stbsp__uint32)pr)
            l = pr;
         while ((l > 1) && (pr) && (sn[l - 1] == '0')) {
            --pr;
            --l;
         }

         // should we use %e
         if ((dp <= -4) || (dp > (stbsp__int32)n)) {
            if (pr > (stbsp__int32)l)
               pr = l - 1;
            else if (pr)
               --pr; // when using %e, there is one digit before the decimal
            goto doexpfromg;
         }
         // this is the insane action to get the pr to match %g sematics for %f
         if (dp > 0) {
            pr = (dp < (stbsp__int32)l) ? l - dp : 0;
         } else {
            pr = -dp + ((pr > (stbsp__int32)l) ? l : pr);
         }
         goto dofloatfromg;

      case 'E': // float
      case 'e': // float
         h = (f[0] == 'E') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, pr | 0x80000000))
            fl |= STBSP__NEGATIVE;
      doexpfromg:
         tail[0] = 0;
         stbsp__lead_sign(fl, lead);
         if (dp == STBSP__SPECIAL) {
            s = (char *)sn;
            cs = 0;
            pr = 0;
            goto scopy;
         }
         s = num + 64;
         // handle leading chars
         *s++ = sn[0];

         if (pr)
            *s++ = stbsp__period;

         // handle after decimal
         if ((l - 1) > (stbsp__uint32)pr)
            l = pr + 1;
         for (n = 1; n < l; n++)
            *s++ = sn[n];
         // trailing zeros
         tz = pr - (l - 1);
         pr = 0;
         // dump expo
         tail[1] = h[0xe];
         dp -= 1;
         if (dp < 0) {
            tail[2] = '-';
            dp = -dp;
         } else
            tail[2] = '+';
#ifdef STB_SPRINTF_MSVC_MODE
         n = 5;
#else
         n = (dp >= 100) ? 5 : 4;
#endif
         tail[0] = (char)n;
         for (;;) {
            tail[n] = '0' + dp % 10;
            if (n <= 3)
               break;
            --n;
            dp /= 10;
         }
         cs = 1 + (3 << 24); // how many tens
         goto flt_lead;

      case 'f': // float
         fv = va_arg(va, double);
      doafloat:
         // do kilos
         if (fl & STBSP__METRIC_SUFFIX) {
            double divisor;
            divisor = 1000.0f;
            if (fl & STBSP__METRIC_1024)
               divisor = 1024.0;
            while (fl < 0x4000000) {
               if ((fv < divisor) && (fv > -divisor))
                  break;
               fv /= divisor;
               fl += 0x1000000;
            }
         }
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, pr))
            fl |= STBSP__NEGATIVE;
      dofloatfromg:
         tail[0] = 0;
         stbsp__lead_sign(fl, lead);
         if (dp == STBSP__SPECIAL) {
            s = (char *)sn;
            cs = 0;
            pr = 0;
            goto scopy;
         }
         s = num + 64;

         // handle the three decimal varieties
         if (dp <= 0) {
            stbsp__int32 i;
            // handle 0.000*000xxxx
            *s++ = '0';
            if (pr)
               *s++ = stbsp__period;
            n = -dp;
            if ((stbsp__int32)n > pr)
               n = pr;
            i = n;
            while (i) {
               if ((((stbsp__uintptr)s) & 3) == 0)
                  break;
               *s++ = '0';
               --i;
            }
            while (i >= 4) {
               *(stbsp__uint32 *)s = 0x30303030;
               s += 4;
               i -= 4;
            }
            while (i) {
               *s++ = '0';
               --i;
            }
            if ((stbsp__int32)(l + n) > pr)
               l = pr - n;
            i = l;
            while (i) {
               *s++ = *sn++;
               --i;
            }
            tz = pr - (n + l);
            cs = 1 + (3 << 24); // how many tens did we write (for commas below)
         } else {
            cs = (fl & STBSP__TRIPLET_COMMA) ? ((600 - (stbsp__uint32)dp) % 3) : 0;
            if ((stbsp__uint32)dp >= l) {
               // handle xxxx000*000.0
               n = 0;
               for (;;) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                     cs = 0;
                     *s++ = stbsp__comma;
                  } else {
                     *s++ = sn[n];
                     ++n;
                     if (n >= l)
                        break;
                  }
               }
               if (n < (stbsp__uint32)dp) {
                  n = dp - n;
                  if ((fl & STBSP__TRIPLET_COMMA) == 0) {
                     while (n) {
                        if ((((stbsp__uintptr)s) & 3) == 0)
                           break;
                        *s++ = '0';
                        --n;
                     }
                     while (n >= 4) {
                        *(stbsp__uint32 *)s = 0x30303030;
                        s += 4;
                        n -= 4;
                     }
                  }
                  while (n) {
                     if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                        cs = 0;
                        *s++ = stbsp__comma;
                     } else {
                        *s++ = '0';
                        --n;
                     }
                  }
               }
               cs = (int)(s - (num + 64)) + (3 << 24); // cs is how many tens
               if (pr) {
                  *s++ = stbsp__period;
                  tz = pr;
               }
            } else {
               // handle xxxxx.xxxx000*000
               n = 0;
               for (;;) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                     cs = 0;
                     *s++ = stbsp__comma;
                  } else {
                     *s++ = sn[n];
                     ++n;
                     if (n >= (stbsp__uint32)dp)
                        break;
                  }
               }
               cs = (int)(s - (num + 64)) + (3 << 24); // cs is how many tens
               if (pr)
                  *s++ = stbsp__period;
               if ((l - dp) > (stbsp__uint32)pr)
                  l = pr + dp;
               while (n < l) {
                  *s++ = sn[n];
                  ++n;
               }
               tz = pr - (l - dp);
            }
         }
         pr = 0;

         // handle k,m,g,t
         if (fl & STBSP__METRIC_SUFFIX) {
            char idx;
            idx = 1;
            if (fl & STBSP__METRIC_NOSPACE)
               idx = 0;
            tail[0] = idx;
            tail[1] = ' ';
            {
               if (fl >> 24) { // SI kilo is 'k', JEDEC and SI kibits are 'K'.
                  if (fl & STBSP__METRIC_1024)
                     tail[idx + 1] = "_KMGT"[fl >> 24];
                  else
                     tail[idx + 1] = "_kMGT"[fl >> 24];
                  idx++;
                  // If printing kibits and not in jedec, add the 'i'.
                  if (fl & STBSP__METRIC_1024 && !(fl & STBSP__METRIC_JEDEC)) {
                     tail[idx + 1] = 'i';
                     idx++;
                  }
                  tail[0] = idx;
               }
            }
         };

      flt_lead:
         // get the length that we copied
         l = (stbsp__uint32)(s - (num + 64));
         s = num + 64;
         goto scopy;
#endif

      case 'B': // upper binary
      case 'b': // lower binary
         h = (f[0] == 'B') ? hexu : hex;
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[0xb];
         }
         l = (8 << 4) | (1 << 8);
         goto radixnum;

      case 'o': // octal
         h = hexu;
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 1;
            lead[1] = '0';
         }
         l = (3 << 4) | (3 << 8);
         goto radixnum;

      case 'p': // pointer
         fl |= (sizeof(void *) == 8) ? STBSP__INTMAX : 0;
         pr = sizeof(void *) * 2;
         fl &= ~STBSP__LEADINGZERO; // 'p' only prints the pointer with zeros
                                    // fall through - to X

      case 'X': // upper hex
      case 'x': // lower hex
         h = (f[0] == 'X') ? hexu : hex;
         l = (4 << 4) | (4 << 8);
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[16];
         }
      radixnum:
         // get the number
         if (fl & STBSP__INTMAX)
            n64 = va_arg(va, stbsp__uint64);
         else
            n64 = va_arg(va, stbsp__uint32);

         s = num + STBSP__NUMSZ;
         dp = 0;
         // clear tail, and clear leading if value is zero
         tail[0] = 0;
         if (n64 == 0) {
            lead[0] = 0;
            if (pr == 0) {
               l = 0;
               cs = (((l >> 4) & 15)) << 24;
               goto scopy;
            }
         }
         // convert to string
         for (;;) {
            *--s = h[n64 & ((1 << (l >> 8)) - 1)];
            n64 >>= (l >> 8);
            if (!((n64) || ((stbsp__int32)((num + STBSP__NUMSZ) - s) < pr)))
               break;
            if (fl & STBSP__TRIPLET_COMMA) {
               ++l;
               if ((l & 15) == ((l >> 4) & 15)) {
                  l &= ~15;
                  *--s = stbsp__comma;
               }
            }
         };
         // get the tens and the comma pos
         cs = (stbsp__uint32)((num + STBSP__NUMSZ) - s) + ((((l >> 4) & 15)) << 24);
         // get the length that we copied
         l = (stbsp__uint32)((num + STBSP__NUMSZ) - s);
         // copy it
         goto scopy;

      case 'u': // unsigned
      case 'i':
      case 'd': // integer
         // get the integer and abs it
         if (fl & STBSP__INTMAX) {
            stbsp__int64 i64 = va_arg(va, stbsp__int64);
            n64 = (stbsp__uint64)i64;
            if ((f[0] != 'u') && (i64 < 0)) {
               n64 = (stbsp__uint64)-i64;
               fl |= STBSP__NEGATIVE;
            }
         } else {
            stbsp__int32 i = va_arg(va, stbsp__int32);
            n64 = (stbsp__uint32)i;
            if ((f[0] != 'u') && (i < 0)) {
               n64 = (stbsp__uint32)-i;
               fl |= STBSP__NEGATIVE;
            }
         }

#ifndef STB_SPRINTF_NOFLOAT
         if (fl & STBSP__METRIC_SUFFIX) {
            if (n64 < 1024)
               pr = 0;
            else if (pr == -1)
               pr = 1;
            fv = (double)(stbsp__int64)n64;
            goto doafloat;
         }
#endif

         // convert to string
         s = num + STBSP__NUMSZ;
         l = 0;

         for (;;) {
            // do in 32-bit chunks (avoid lots of 64-bit divides even with constant denominators)
            char *o = s - 8;
            if (n64 >= 100000000) {
               n = (stbsp__uint32)(n64 % 100000000);
               n64 /= 100000000;
            } else {
               n = (stbsp__uint32)n64;
               n64 = 0;
            }
            if ((fl & STBSP__TRIPLET_COMMA) == 0) {
               do {
                  s -= 2;
                  *(stbsp__uint16 *)s = *(stbsp__uint16 *)&stbsp__digitpair[(n % 100) * 2];
                  n /= 100;
               } while (n);
            }
            while (n) {
               if ((fl & STBSP__TRIPLET_COMMA) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = (char)(n % 10) + '0';
                  n /= 10;
               }
            }
            if (n64 == 0) {
               if ((s[0] == '0') && (s != (num + STBSP__NUMSZ)))
                  ++s;
               break;
            }
            while (s != o)
               if ((fl & STBSP__TRIPLET_COMMA) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = '0';
               }
         }

         tail[0] = 0;
         stbsp__lead_sign(fl, lead);

         // get the length that we copied
         l = (stbsp__uint32)((num + STBSP__NUMSZ) - s);
         if (l == 0) {
            *--s = '0';
            l = 1;
         }
         cs = l + (3 << 24);
         if (pr < 0)
            pr = 0;

      scopy:
         // get fw=leading/trailing space, pr=leading zeros
         if (pr < (stbsp__int32)l)
            pr = l;
         n = pr + lead[0] + tail[0] + tz;
         if (fw < (stbsp__int32)n)
            fw = n;
         fw -= n;
         pr -= l;

         // handle right justify and leading zeros
         if ((fl & STBSP__LEFTJUST) == 0) {
            if (fl & STBSP__LEADINGZERO) // if leading zeros, everything is in pr
            {
               pr = (fw > pr) ? fw : pr;
               fw = 0;
            } else {
               fl &= ~STBSP__TRIPLET_COMMA; // if no leading zeros, then no commas
            }
         }

         // copy the spaces and/or zeros
         if (fw + pr) {
            stbsp__int32 i;
            stbsp__uint32 c;

            // copy leading spaces (or when doing %8.4d stuff)
            if ((fl & STBSP__LEFTJUST) == 0)
               while (fw > 0) {
                  stbsp__cb_buf_clamp(i, fw);
                  fw -= i;
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i) {
                     *bf++ = ' ';
                     --i;
                  }
                  stbsp__chk_cb_buf(1);
               }

            // copy leader
            sn = lead + 1;
            while (lead[0]) {
               stbsp__cb_buf_clamp(i, lead[0]);
               lead[0] -= (char)i;
               while (i) {
                  *bf++ = *sn++;
                  --i;
               }
               stbsp__chk_cb_buf(1);
            }

            // copy leading zeros
            c = cs >> 24;
            cs &= 0xffffff;
            cs = (fl & STBSP__TRIPLET_COMMA) ? ((stbsp__uint32)(c - ((pr + cs) % (c + 1)))) : 0;
            while (pr > 0) {
               stbsp__cb_buf_clamp(i, pr);
               pr -= i;
               if ((fl & STBSP__TRIPLET_COMMA) == 0) {
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = '0';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x30303030;
                     bf += 4;
                     i -= 4;
                  }
               }
               while (i) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (cs++ == c)) {
                     cs = 0;
                     *bf++ = stbsp__comma;
                  } else
                     *bf++ = '0';
                  --i;
               }
               stbsp__chk_cb_buf(1);
            }
         }

         // copy leader if there is still one
         sn = lead + 1;
         while (lead[0]) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, lead[0]);
            lead[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy the string
         n = l;
         while (n) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, n);
            n -= i;
            STBSP__UNALIGNED(while (i >= 4) {
               *(stbsp__uint32 *)bf = *(stbsp__uint32 *)s;
               bf += 4;
               s += 4;
               i -= 4;
            })
            while (i) {
               *bf++ = *s++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy trailing zeros
         while (tz) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, tz);
            tz -= i;
            while (i) {
               if ((((stbsp__uintptr)bf) & 3) == 0)
                  break;
               *bf++ = '0';
               --i;
            }
            while (i >= 4) {
               *(stbsp__uint32 *)bf = 0x30303030;
               bf += 4;
               i -= 4;
            }
            while (i) {
               *bf++ = '0';
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy tail if there is one
         sn = tail + 1;
         while (tail[0]) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, tail[0]);
            tail[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // handle the left justify
         if (fl & STBSP__LEFTJUST)
            if (fw > 0) {
               while (fw) {
                  stbsp__int32 i;
                  stbsp__cb_buf_clamp(i, fw);
                  fw -= i;
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i--)
                     *bf++ = ' ';
                  stbsp__chk_cb_buf(1);
               }
            }
         break;

      default: // unknown, just copy code
         s = num + STBSP__NUMSZ - 1;
         *s = f[0];
         l = 1;
         fw = fl = 0;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;
      }
      ++f;
   }
endfmt:

   if (!callback)
      *bf = 0;
   else
      stbsp__flush_cb();

done:
   return tlen + (int)(bf - buf);
}

// cleanup
#undef STBSP__LEFTJUST
#undef STBSP__LEADINGPLUS
#undef STBSP__LEADINGSPACE
#undef STBSP__LEADING_0X
#undef STBSP__LEADINGZERO
#undef STBSP__INTMAX
#undef STBSP__TRIPLET_COMMA
#undef STBSP__NEGATIVE
#undef STBSP__METRIC_SUFFIX
#undef STBSP__NUMSZ
#undef stbsp__chk_cb_bufL
#undef stbsp__chk_cb_buf
#undef stbsp__flush_cb
#undef stbsp__cb_buf_clamp

// ============================================================================
//   wrapper functions

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(sprintf)(char *buf, char const *fmt, ...)
{
   int result;
   va_list va;
   va_start(va, fmt);
   result = STB_SPRINTF_DECORATE(vsprintfcb)(0, 0, buf, fmt, va);
   va_end(va);
   return result;
}

typedef struct stbsp__context {
   char *buf;
   int count;
   char tmp[STB_SPRINTF_MIN];
} stbsp__context;

static char *stbsp__clamp_callback(char *buf, void *user, int len)
{
   stbsp__context *c = (stbsp__context *)user;

   if (len > c->count)
      len = c->count;

   if (len) {
      if (buf != c->buf) {
         char *s, *d, *se;
         d = c->buf;
         s = buf;
         se = buf + len;
         do {
            *d++ = *s++;
         } while (s < se);
      }
      c->buf += len;
      c->count -= len;
   }

   if (c->count <= 0)
      return 0;
   return (c->count >= STB_SPRINTF_MIN) ? c->buf : c->tmp; // go direct into buffer if you can
}

static char * stbsp__count_clamp_callback( char * buf, void * user, int len )
{
    (void)buf;
   stbsp__context * c = (stbsp__context*)user;
   c->count += len;
   return c->tmp; // go direct into buffer if you can
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( vsnprintf )( char * buf, int count, char const * fmt, va_list va )
{
   stbsp__context c;
   int l;

   if ( (count == 0) && !buf )
   {
      c.count = 0;

      STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__count_clamp_callback, &c, c.tmp, fmt, va );
      l = c.count;
   }
   else
   {
      if ( count == 0 )
         return 0;

      c.buf = buf;
      c.count = count;

      STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__clamp_callback, &c, stbsp__clamp_callback(0,&c,0), fmt, va );

      // zero-terminate
      l = (int)( c.buf - buf );
      if ( l >= count ) // should never be greater, only equal (or less) than count
         l = count - 1;
      buf[l] = 0;
   }

   return l;
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...)
{
   int result;
   va_list va;
   va_start(va, fmt);

   result = STB_SPRINTF_DECORATE(vsnprintf)(buf, count, fmt, va);
   va_end(va);

   return result;
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintf)(char *buf, char const *fmt, va_list va)
{
   return STB_SPRINTF_DECORATE(vsprintfcb)(0, 0, buf, fmt, va);
}

// =======================================================================
//   low level float utility functions

#ifndef STB_SPRINTF_NOFLOAT

// copies d to bits w/ strict aliasing (this compiles to nothing on /Ox)
#define STBSP__COPYFP(dest, src)                   \
   {                                               \
      int cn;                                      \
      for (cn = 0; cn < 8; cn++)                   \
         ((char *)&dest)[cn] = ((char *)&src)[cn]; \
   }

// get float info
static stbsp__int32 stbsp__real_to_parts(stbsp__int64 *bits, stbsp__int32 *expo, double value)
{
   double d;
   stbsp__int64 b = 0;

   // load value and round at the frac_digits
   d = value;

   STBSP__COPYFP(b, d);

   *bits = b & ((((stbsp__uint64)1) << 52) - 1);
   *expo = (stbsp__int32)(((b >> 52) & 2047) - 1023);

   return (stbsp__int32)(b >> 63);
}

static double const stbsp__bot[23] = {
   1e+000, 1e+001, 1e+002, 1e+003, 1e+004, 1e+005, 1e+006, 1e+007, 1e+008, 1e+009, 1e+010, 1e+011,
   1e+012, 1e+013, 1e+014, 1e+015, 1e+016, 1e+017, 1e+018, 1e+019, 1e+020, 1e+021, 1e+022
};
static double const stbsp__negbot[22] = {
   1e-001, 1e-002, 1e-003, 1e-004, 1e-005, 1e-006, 1e-007, 1e-008, 1e-009, 1e-010, 1e-011,
   1e-012, 1e-013, 1e-014, 1e-015, 1e-016, 1e-017, 1e-018, 1e-019, 1e-020, 1e-021, 1e-022
};
static double const stbsp__negboterr[22] = {
   -5.551115123125783e-018,  -2.0816681711721684e-019, -2.0816681711721686e-020, -4.7921736023859299e-021, -8.1803053914031305e-022, 4.5251888174113741e-023,
   4.5251888174113739e-024,  -2.0922560830128471e-025, -6.2281591457779853e-026, -3.6432197315497743e-027, 6.0503030718060191e-028,  2.0113352370744385e-029,
   -3.0373745563400371e-030, 1.1806906454401013e-032,  -7.7705399876661076e-032, 2.0902213275965398e-033,  -7.1542424054621921e-034, -7.1542424054621926e-035,
   2.4754073164739869e-036,  5.4846728545790429e-037,  9.2462547772103625e-038,  -4.8596774326570872e-039
};
static double const stbsp__top[13] = {
   1e+023, 1e+046, 1e+069, 1e+092, 1e+115, 1e+138, 1e+161, 1e+184, 1e+207, 1e+230, 1e+253, 1e+276, 1e+299
};
static double const stbsp__negtop[13] = {
   1e-023, 1e-046, 1e-069, 1e-092, 1e-115, 1e-138, 1e-161, 1e-184, 1e-207, 1e-230, 1e-253, 1e-276, 1e-299
};
static double const stbsp__toperr[13] = {
   8388608,
   6.8601809640529717e+028,
   -7.253143638152921e+052,
   -4.3377296974619174e+075,
   -1.5559416129466825e+098,
   -3.2841562489204913e+121,
   -3.7745893248228135e+144,
   -1.7356668416969134e+167,
   -3.8893577551088374e+190,
   -9.9566444326005119e+213,
   6.3641293062232429e+236,
   -5.2069140800249813e+259,
   -5.2504760255204387e+282
};
static double const stbsp__negtoperr[13] = {
   3.9565301985100693e-040,  -2.299904345391321e-063,  3.6506201437945798e-086,  1.1875228833981544e-109,
   -5.0644902316928607e-132, -6.7156837247865426e-155, -2.812077463003139e-178,  -5.7778912386589953e-201,
   7.4997100559334532e-224,  -4.6439668915134491e-247, -6.3691100762962136e-270, -9.436808465446358e-293,
   8.0970921678014997e-317
};

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
static stbsp__uint64 const stbsp__powten[20] = {
   1,
   10,
   100,
   1000,
   10000,
   100000,
   1000000,
   10000000,
   100000000,
   1000000000,
   10000000000,
   100000000000,
   1000000000000,
   10000000000000,
   100000000000000,
   1000000000000000,
   10000000000000000,
   100000000000000000,
   1000000000000000000,
   10000000000000000000U
};
#define stbsp__tento19th ((stbsp__uint64)1000000000000000000)
#else
static stbsp__uint64 const stbsp__powten[20] = {
   1,
   10,
   100,
   1000,
   10000,
   100000,
   1000000,
   10000000,
   100000000,
   1000000000,
   10000000000ULL,
   100000000000ULL,
   1000000000000ULL,
   10000000000000ULL,
   100000000000000ULL,
   1000000000000000ULL,
   10000000000000000ULL,
   100000000000000000ULL,
   1000000000000000000ULL,
   10000000000000000000ULL
};
#define stbsp__tento19th (1000000000000000000ULL)
#endif

#define stbsp__ddmulthi(oh, ol, xh, yh)                            \
   {                                                               \
      double ahi = 0, alo, bhi = 0, blo;                           \
      stbsp__int64 bt;                                             \
      oh = xh * yh;                                                \
      STBSP__COPYFP(bt, xh);                                       \
      bt &= ((~(stbsp__uint64)0) << 27);                           \
      STBSP__COPYFP(ahi, bt);                                      \
      alo = xh - ahi;                                              \
      STBSP__COPYFP(bt, yh);                                       \
      bt &= ((~(stbsp__uint64)0) << 27);                           \
      STBSP__COPYFP(bhi, bt);                                      \
      blo = yh - bhi;                                              \
      ol = ((ahi * bhi - oh) + ahi * blo + alo * bhi) + alo * blo; \
   }

#define stbsp__ddtoS64(ob, xh, xl)          \
   {                                        \
      double ahi = 0, alo, vh, t;           \
      ob = (stbsp__int64)ph;                \
      vh = (double)ob;                      \
      ahi = (xh - vh);                      \
      t = (ahi - xh);                       \
      alo = (xh - (ahi - t)) - (vh + t);    \
      ob += (stbsp__int64)(ahi + alo + xl); \
   }

#define stbsp__ddrenorm(oh, ol) \
   {                            \
      double s;                 \
      s = oh + ol;              \
      ol = ol - (s - oh);       \
      oh = s;                   \
   }

#define stbsp__ddmultlo(oh, ol, xh, xl, yh, yl) ol = ol + (xh * yl + xl * yh);

#define stbsp__ddmultlos(oh, ol, xh, yl) ol = ol + (xh * yl);

static void stbsp__raise_to_power10(double *ohi, double *olo, double d, stbsp__int32 power) // power can be -323 to +350
{
   double ph, pl;
   if ((power >= 0) && (power <= 22)) {
      stbsp__ddmulthi(ph, pl, d, stbsp__bot[power]);
   } else {
      stbsp__int32 e, et, eb;
      double p2h, p2l;

      e = power;
      if (power < 0)
         e = -e;
      et = (e * 0x2c9) >> 14; /* %23 */
      if (et > 13)
         et = 13;
      eb = e - (et * 23);

      ph = d;
      pl = 0.0;
      if (power < 0) {
         if (eb) {
            --eb;
            stbsp__ddmulthi(ph, pl, d, stbsp__negbot[eb]);
            stbsp__ddmultlos(ph, pl, d, stbsp__negboterr[eb]);
         }
         if (et) {
            stbsp__ddrenorm(ph, pl);
            --et;
            stbsp__ddmulthi(p2h, p2l, ph, stbsp__negtop[et]);
            stbsp__ddmultlo(p2h, p2l, ph, pl, stbsp__negtop[et], stbsp__negtoperr[et]);
            ph = p2h;
            pl = p2l;
         }
      } else {
         if (eb) {
            e = eb;
            if (eb > 22)
               eb = 22;
            e -= eb;
            stbsp__ddmulthi(ph, pl, d, stbsp__bot[eb]);
            if (e) {
               stbsp__ddrenorm(ph, pl);
               stbsp__ddmulthi(p2h, p2l, ph, stbsp__bot[e]);
               stbsp__ddmultlos(p2h, p2l, stbsp__bot[e], pl);
               ph = p2h;
               pl = p2l;
            }
         }
         if (et) {
            stbsp__ddrenorm(ph, pl);
            --et;
            stbsp__ddmulthi(p2h, p2l, ph, stbsp__top[et]);
            stbsp__ddmultlo(p2h, p2l, ph, pl, stbsp__top[et], stbsp__toperr[et]);
            ph = p2h;
            pl = p2l;
         }
      }
   }
   stbsp__ddrenorm(ph, pl);
   *ohi = ph;
   *olo = pl;
}

// given a float value, returns the significant bits in bits, and the position of the
//   decimal point in decimal_pos.  +/-INF and NAN are specified by special values
//   returned in the decimal_pos parameter.
// frac_digits is absolute normally, but if you want from first significant digits (got %g and %e), or in 0x80000000
static stbsp__int32 stbsp__real_to_str(char const **start, stbsp__uint32 *len, char *out, stbsp__int32 *decimal_pos, double value, stbsp__uint32 frac_digits)
{
   double d;
   stbsp__int64 bits = 0;
   stbsp__int32 expo, e, ng, tens;

   d = value;
   STBSP__COPYFP(bits, d);
   expo = (stbsp__int32)((bits >> 52) & 2047);
   ng = (stbsp__int32)(bits >> 63);
   if (ng)
      d = -d;

   if (expo == 2047) // is nan or inf?
   {
      *start = (bits & ((((stbsp__uint64)1) << 52) - 1)) ? "NaN" : "Inf";
      *decimal_pos = STBSP__SPECIAL;
      *len = 3;
      return ng;
   }

   if (expo == 0) // is zero or denormal
   {
      if ((bits << 1) == 0) // do zero
      {
         *decimal_pos = 1;
         *start = out;
         out[0] = '0';
         *len = 1;
         return ng;
      }
      // find the right expo for denormals
      {
         stbsp__int64 v = ((stbsp__uint64)1) << 51;
         while ((bits & v) == 0) {
            --expo;
            v >>= 1;
         }
      }
   }

   // find the decimal exponent as well as the decimal bits of the value
   {
      double ph, pl;

      // log10 estimate - very specifically tweaked to hit or undershoot by no more than 1 of log10 of all expos 1..2046
      tens = expo - 1023;
      tens = (tens < 0) ? ((tens * 617) / 2048) : (((tens * 1233) / 4096) + 1);

      // move the significant bits into position and stick them into an int
      stbsp__raise_to_power10(&ph, &pl, d, 18 - tens);

      // get full as much precision from double-double as possible
      stbsp__ddtoS64(bits, ph, pl);

      // check if we undershot
      if (((stbsp__uint64)bits) >= stbsp__tento19th)
         ++tens;
   }

   // now do the rounding in integer land
   frac_digits = (frac_digits & 0x80000000) ? ((frac_digits & 0x7ffffff) + 1) : (tens + frac_digits);
   if ((frac_digits < 24)) {
      stbsp__uint32 dg = 1;
      if ((stbsp__uint64)bits >= stbsp__powten[9])
         dg = 10;
      while ((stbsp__uint64)bits >= stbsp__powten[dg]) {
         ++dg;
         if (dg == 20)
            goto noround;
      }
      if (frac_digits < dg) {
         stbsp__uint64 r;
         // add 0.5 at the right position and round
         e = dg - frac_digits;
         if ((stbsp__uint32)e >= 24)
            goto noround;
         r = stbsp__powten[e];
         bits = bits + (r / 2);
         if ((stbsp__uint64)bits >= stbsp__powten[dg])
            ++tens;
         bits /= r;
      }
   noround:;
   }

   // kill long trailing runs of zeros
   if (bits) {
      stbsp__uint32 n;
      for (;;) {
         if (bits <= 0xffffffff)
            break;
         if (bits % 1000)
            goto donez;
         bits /= 1000;
      }
      n = (stbsp__uint32)bits;
      while ((n % 1000) == 0)
         n /= 1000;
      bits = n;
   donez:;
   }

   // convert to string
   out += 64;
   e = 0;
   for (;;) {
      stbsp__uint32 n;
      char *o = out - 8;
      // do the conversion in chunks of U32s (avoid most 64-bit divides, worth it, constant denomiators be damned)
      if (bits >= 100000000) {
         n = (stbsp__uint32)(bits % 100000000);
         bits /= 100000000;
      } else {
         n = (stbsp__uint32)bits;
         bits = 0;
      }
      while (n) {
         out -= 2;
         *(stbsp__uint16 *)out = *(stbsp__uint16 *)&stbsp__digitpair[(n % 100) * 2];
         n /= 100;
         e += 2;
      }
      if (bits == 0) {
         if ((e) && (out[0] == '0')) {
            ++out;
            --e;
         }
         break;
      }
      while (out != o) {
         *--out = '0';
         ++e;
      }
   }

   *decimal_pos = tens;
   *start = out;
   *len = e;
   return ng;
}

#undef stbsp__ddmulthi
#undef stbsp__ddrenorm
#undef stbsp__ddmultlo
#undef stbsp__ddmultlos
#undef STBSP__SPECIAL
#undef STBSP__COPYFP

#endif // STB_SPRINTF_NOFLOAT

// clean up
#undef stbsp__uint16
#undef stbsp__uint32
#undef stbsp__int32
#undef stbsp__uint64
#undef stbsp__int64
#undef STBSP__UNALIGNED
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
#endif // DQN_IMPLEMENTATION

#if defined(DQN__XPLATFORM_LAYER)
// #XPlatform (Win32 & Unix)
// =================================================================================================
// Functions in the Cross Platform are guaranteed to be supported in both Unix
// and Win32

#ifdef DQN_IS_UNIX
    #include <stdio.h>    // Basic File I/O // TODO(doyle): Syscall versions

    #include <dirent.h>   // readdir()/opendir()/closedir()
    #include <sys/stat.h> // file size query
    #include <sys/time.h> // high resolution timer
    #include <time.h>     // timespec
    #include <unistd.h>   // unlink()
#endif

#define DQN_FILE__LIST_DIR(name) DQN_FILE_SCOPE char **name(char const *dir, i32 *num_files, DqnMemAPI *api)

// XPlatform > #DqnFile
// =================================================================================================
#ifdef DQN_IS_WIN32

FILE_SCOPE bool
DqnFile__Win32Open(wchar_t const *path, DqnFile *file, u32 flags, DqnFile::Action action)
{
    if (!file || !path) return false;

    u32 const WIN32_GENERIC_READ    = 0x80000000L;
    u32 const WIN32_GENERIC_WRITE   = 0x40000000L;
    u32 const WIN32_GENERIC_EXECUTE = 0x20000000L;
    u32 const WIN32_GENERIC_ALL     = 0x10000000L;

    u32 const WIN32_CREATE_NEW        = 1;
    u32 const WIN32_CREATE_ALWAYS     = 2;
    u32 const WIN32_OPEN_EXISTING     = 3;
    u32 const WIN32_OPEN_ALWAYS       = 4;
    u32 const WIN32_TRUNCATE_EXISTING = 5;

    u32 const WIN32_FILE_ATTRIBUTE_NORMAL = 0x00000080;

    DWORD win32_flag = 0;
    if (flags & DqnFile::Flag::All)
    {
        win32_flag = WIN32_GENERIC_ALL;
    }
    else
    {
        if (flags & DqnFile::Flag::FileRead)  win32_flag |= WIN32_GENERIC_READ;
        if (flags & DqnFile::Flag::FileWrite) win32_flag |= WIN32_GENERIC_WRITE;
        if (flags & DqnFile::Flag::Execute)   win32_flag |= WIN32_GENERIC_EXECUTE;
    }

    DWORD win32_action = 0;
    switch (action)
    {
        // Allow fall through
        default: DQN_ASSERT(DQN_INVALID_CODE_PATH);
        case DqnFile::Action::OpenOnly:         win32_action = WIN32_OPEN_EXISTING; break;
        case DqnFile::Action::ClearIfExist:     win32_action = WIN32_TRUNCATE_EXISTING; break;
        case DqnFile::Action::CreateIfNotExist: win32_action = WIN32_CREATE_NEW; break;
        case DqnFile::Action::ForceCreate:      win32_action = WIN32_CREATE_ALWAYS; break;
    }

    HANDLE handle = CreateFileW(path, win32_flag, 0, nullptr, win32_action,
                                WIN32_FILE_ATTRIBUTE_NORMAL, nullptr);

    if (handle == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    LARGE_INTEGER size;
    if (GetFileSizeEx(handle, &size) == 0)
    {
        file->Close();
        DqnWin32_DisplayLastError("GetFileSizeEx() failed");
        return false;
    }

    file->handle = handle;
    file->size   = (usize)size.QuadPart;
    file->flags  = flags;
    return true;
}

DQN_FILE__LIST_DIR(DqnFile__PlatformListDir)
{
    if (!dir) return nullptr;

    i32 curr_num_files = 0;
    wchar_t wide_dir[MAX_PATH] = {0};
    DqnWin32_UTF8ToWChar(dir, wide_dir, DQN_ARRAY_COUNT(wide_dir));

    // Enumerate number of files first
    {
        WIN32_FIND_DATAW find_data = {0};
        HANDLE find_handle = FindFirstFileW(wide_dir, &find_data);
        if (find_handle == INVALID_HANDLE_VALUE)
        {
            DQN__WIN32_ERROR_BOX("FindFirstFile() failed.", nullptr);
            return nullptr;
        }

        DQN_DEFER(FindClose(find_handle));
        bool stay_in_loop = true;
        while (stay_in_loop)
        {
            BOOL result = FindNextFileW(find_handle, &find_data);
            if (result == 0)
            {
                DWORD error = GetLastError();

                u32 const WIN32_ERROR_NO_MORE_FILES = 18L;
                if (error != WIN32_ERROR_NO_MORE_FILES)
                {
                    DqnWin32_DisplayErrorCode(error, "FindNextFileW() failed");
                }

                stay_in_loop = false;
            }
            else
            {
                curr_num_files++;
            }
        }
        FindClose(find_handle);
    }

    if (curr_num_files == 0)
    {
        *num_files = 0;
        return nullptr;
    }

    {
        WIN32_FIND_DATAW init_find = {};
        HANDLE find_handle = FindFirstFileW(wide_dir, &init_find);
        if (find_handle == INVALID_HANDLE_VALUE)
        {
            DQN__WIN32_ERROR_BOX("FindFirstFile() failed.", nullptr);
            *num_files = 0;
            return nullptr;
        }

        DQN_DEFER(FindClose(find_handle));
        char **list = (char **)api->Alloc(sizeof(*list) * (curr_num_files), Dqn::ZeroClear::Yes);

        if (!list)
        {
            DQN_LOGE("Memory allocation failed, required: %$_d", sizeof(*list) * curr_num_files);
            *num_files = 0;
            return nullptr;
        }

        for (auto i = 0; i < curr_num_files; i++)
        {
            // TODO(doyle): Max path is bad.
            list[i] = (char *)api->Alloc(sizeof(**list) * MAX_PATH, Dqn::ZeroClear::Yes);
            if (!list[i])
            {
                for (auto j = 0; j < i; j++)
                    api->Free(list[j]);

                DQN_LOGE("Memory allocation failed, required: %$_d", sizeof(**list) * MAX_PATH);
                *num_files = 0;
                return nullptr;
            }
        }

        i32 list_index = 0;
        WIN32_FIND_DATAW find_data = {0};
        while (FindNextFileW(find_handle, &find_data) != 0)
        {
            DqnWin32_WCharToUTF8(find_data.cFileName, list[list_index++], MAX_PATH);
        }

        *num_files = curr_num_files;

        return list;
    }
}
#endif // DQN_IS_WIN32

#ifdef DQN_IS_UNIX
FILE_SCOPE bool DqnFile__UnixGetFileSize(char const *path, usize *size)
{
    struct stat file_stat = {};
    stat(path, &file_stat);
    *size = file_stat.st_size;

    if (*size != 0)
      return true;

    // NOTE: Can occur in some instances where files are generated on demand, i.e. /proc/cpuinfo.
    // But there can also be zero-byte files, we can't be sure. So manual check by counting bytes
    if (FILE *file = fopen(path, "rb"))
    {
        DQN_DEFER(fclose(file));
        while (fgetc(file) != EOF)
        {
            (*size)++;
        }
    }

    return true;
}

FILE_SCOPE bool
DqnFile__UnixOpen(char const *path, DqnFile *file, u32 flags, DqnFile::Action action)
{
    char operation  = 0;
    bool update_flag = false;

    if (flags & DqnFile::Flag::FileWrite)
    {
        update_flag = true;
        switch (action)
        {
            default: DQN_ASSERT(DQN_INVALID_CODE_PATH);
            case DqnFile::Action::OpenOnly:
            {
                operation   = 'r';
            }
            break;

            case DqnFile::Action::CreateIfNotExist:
            case DqnFile::Action::ClearIfExist:
            case DqnFile::Action::ForceCreate:
            {
                operation   = 'w';
            }
            break;
        }
    }
    else if ((flags & DqnFile::Flag::FileRead) ||
             (flags & DqnFile::Flag::Execute))
    {
        if (flags & DqnFile::Flag::Execute)
        {
            // TODO(doyle): Logging, UNIX doesn't have execute param for file
            // handles. Execution goes through system()
        }
        operation = 'r';
    }
    DQN_ASSERT(operation != 0);

    // TODO(doyle): What about not reading as a binary file and appending to end of file.
    u32 mode_index     = 0;
    char mode[4]      = {};
    mode[mode_index++] = operation;

    if (update_flag) mode[mode_index++] = '+';

    mode[mode_index++] = 'b';
    DQN_ASSERT(mode_index <= DQN_ARRAY_COUNT(mode) - 1);

    // TODO(doyle): Use open syscall
    // TODO(doyle): Query errno
    if (!DqnFile__UnixGetFileSize(path, &file->size))
    {
        return false;
    }

    file->handle = fopen(path, mode);
    file->flags = flags;
    return true;
}

DQN_FILE__LIST_DIR(DqnFile__PlatformListDir)
{
    if (!dir) return nullptr;

    // Enumerate num files
    i32 curr_num_files = 0;
    {
        DIR *const dir_handle = opendir(dir);
        if (!dir_handle) return nullptr;
        DQN_DEFER(closedir(dir_handle));

        struct dirent *dir_file = readdir(dir_handle);
        while (dir_file)
        {
            curr_num_files++;
            dir_file = readdir(dir_handle);
        }
    }

    if (curr_num_files == 0)
    {
        *num_files = 0;
        return nullptr;
    }

    // Create file list
    {
        DIR *const dir_handle = opendir(dir);
        if (!dir_handle) return nullptr;
        DQN_DEFER(closedir(dir_handle));

        char **list = (char **)api->Alloc(sizeof(*list) * curr_num_files, Dqn::ZeroClear::Yes);
        if (!list)
        {
            DQN_LOGE("Memory allocation failed, required: %$_d", sizeof(*list) * curr_num_files);
            *num_files = 0;
            return nullptr;
        }

        struct dirent *dir_file = readdir(dir_handle);
        for (auto i = 0; i < curr_num_files; i++)
        {
            list[i] = (char *)api->Alloc(sizeof(**list) * DQN_ARRAY_COUNT(dir_file->d_name), Dqn::ZeroClear::Yes);
            if (!list[i])
            {
                for (auto j = 0; j < i; j++) api->Free(list[j]);

                DQN_LOGE("Memory allocation failed, required: %$_d", sizeof(**list) * DQN_ARRAY_COUNT(dir_file->d_name));
                *num_files = 0;
                return nullptr;
            }
        }

        u32 list_index = 0;
        *num_files      = curr_num_files;
        while (dir_file)
        {
            DqnMem_Copy(list[list_index++], dir_file->d_name, DQN_ARRAY_COUNT(dir_file->d_name));
            dir_file = readdir(dir_handle);
        }

        return list;
    }
}
#endif // DQN_IS_UNIX

bool DqnFile::Open(char const *path, u32 flags_, Action action)
{
    if (!path) return false;

#if defined(DQN_IS_WIN32)
    // TODO(doyle): MAX PATH is baad
    wchar_t wide_path[MAX_PATH] = {};
    DqnWin32_UTF8ToWChar(path, wide_path, DQN_ARRAY_COUNT(wide_path));
    return DqnFile__Win32Open(wide_path, this, flags_, action);
#else
    return DqnFile__UnixOpen(path, this, flags_, action);
#endif
}

bool DqnFile::Open(wchar_t const *path, u32 flags_, Action action)
{
    if (!path) return false;

#if defined(DQN_IS_WIN32)
    return DqnFile__Win32Open(path, this, flags_, action);

#else
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return false;
#endif
}

usize DqnFile::Write(u8 const *buf, usize num_bytes_to_write, usize fileOffset)
{
    // TODO(doyle): Implement when it's needed
    DQN_ASSERTM(fileOffset == 0, "File writing into offset is not implemented.");
    usize num_bytes_written = 0;

#if defined(DQN_IS_WIN32)
    DWORD bytes_to_write = (DWORD)num_bytes_to_write;
    DWORD bytes_written;
    BOOL result = WriteFile(this->handle, (void *)buf, bytes_to_write, &bytes_written, nullptr);

    num_bytes_written = (usize)bytes_written;
    // TODO(doyle): Better logging system
    if (result == 0)
    {
        DQN__WIN32_ERROR_BOX("ReadFile() failed.", nullptr);
    }

#else
    const usize ITEMS_TO_WRITE = 1;
    if (fwrite(buf, num_bytes_to_write, ITEMS_TO_WRITE, (FILE *)this->handle) == ITEMS_TO_WRITE)
    {
        rewind((FILE *)this->handle);
        num_bytes_written = ITEMS_TO_WRITE * num_bytes_to_write;
    }
#endif

    return num_bytes_written;
}

usize DqnFile::Read(u8 *buf, usize num_bytes_to_read)
{
    usize num_bytes_read = 0;
    if (buf && this->handle)
    {
#if defined(DQN_IS_WIN32)
        DWORD bytes_to_read = (DWORD)num_bytes_to_read;
        DWORD bytes_read    = 0;
        HANDLE win32_handle = this->handle;

        BOOL result = ReadFile(win32_handle, (void *)buf, bytes_to_read, &bytes_read, nullptr);

        num_bytes_read = (usize)bytes_read;
        // TODO(doyle): 0 also means it is completing async, but still valid
        if (result == 0)
        {
            DQN__WIN32_ERROR_BOX("ReadFile() failed.", nullptr);
        }

#else
        // TODO(doyle): Syscall version
        const usize ITEMS_TO_READ = 1;
        if (fread(buf, num_bytes_to_read, ITEMS_TO_READ, (FILE *)this->handle) == ITEMS_TO_READ)
        {
            rewind((FILE *)this->handle);
            num_bytes_read = ITEMS_TO_READ * num_bytes_to_read;
        }
        else
        {
            // TODO(doyle): Logging, failed read
        }
#endif
    }
    return num_bytes_read;
}

u8 *DqnFile_ReadAll(wchar_t const *path, usize *buf_size, DqnMemAPI *api)
{
    // TODO(doyle): Logging
    usize required_size = 0;
    if (!DqnFile_Size(path, &required_size) || required_size == 0)
        return nullptr;

    auto *buf = (u8 *)api->Alloc(required_size, Dqn::ZeroClear::No);
    if (DqnFile_ReadAll(path, buf, required_size))
    {
        *buf_size = required_size;
        return buf;
    }

    api->Free(buf, required_size);
    return nullptr;
}

DQN_FILE_SCOPE u8 *DqnFile_ReadAll(char const *path, usize *buf_size, DqnMemAPI *api)
{
    // TODO(doyle): Logging
    usize required_size = 0;
    if (!DqnFile_Size(path, &required_size) || required_size == 0)
        return nullptr;

    auto *buf = (u8 *)api->Alloc(required_size, Dqn::ZeroClear::No);
    if (DqnFile_ReadAll(path, buf, required_size))
    {
        *buf_size = required_size;
        return buf;
    }

    api->Free(buf, required_size);
    return nullptr;
}

DQN_FILE_SCOPE u8 *DqnFile_ReadAll(wchar_t const *path, usize *buf_size, DqnMemStack *stack, DqnMemStack::AllocTo allocTo)
{
    u8 *result = nullptr;
    DqnFile file = {};
    if (!file.Open(path, DqnFile::Flag::FileRead, DqnFile::Action::OpenOnly))
    {
        DQN_LOGE("Could not open file: %s", path);
        return result;
    }
    DQN_DEFER(file.Close());

    result = static_cast<u8 *>(stack->Push(file.size, allocTo));
    usize bytes_read = file.Read(result, file.size);
    if (bytes_read == file.size)
    {
        *buf_size = file.size;
    }
    else
    {
        DQN_LOGE("bytes_read != file.size", bytes_read, file.size);
    }

    return result;
}

DQN_FILE_SCOPE u8 *DqnFile_ReadAll(char const *path, usize *buf_size, DqnMemStack *stack, DqnMemStack::AllocTo allocTo)
{
    u8 *result = nullptr;
    DqnFile file = {};
    if (!file.Open(path, DqnFile::Flag::FileRead, DqnFile::Action::OpenOnly))
    {
        DQN_LOGE("Could not open file: %s", path);
        return result;
    }
    DQN_DEFER(file.Close());

    result = static_cast<u8 *>(stack->Push(file.size, allocTo));
    usize bytes_read = file.Read(result, file.size);
    if (bytes_read == file.size)
    {
        *buf_size = file.size;
    }
    else
    {
        DQN_LOGE("bytes_read != file.size", bytes_read, file.size);
    }

    return result;
}

DQN_FILE_SCOPE bool DqnFile_WriteAll(char const *path, u8 const *buf, usize const buf_size)
{
    DqnFile file = {};
    if (!file.Open(path, DqnFile::Flag::FileReadWrite, DqnFile::Action::ForceCreate))
    {
        DQN_LOGE("Could not open file at: %s", path);
        return false;
    }

    DQN_DEFER(file.Close());
    usize bytes_written = file.Write(buf, buf_size, 0);
    if (bytes_written != buf_size)
    {
        DQN_LOGE("Bytes written did not match the buffer size, %zu != %zu", bytes_written, buf_size);
        return false;
    }

    return true;
}

DQN_FILE_SCOPE bool DqnFile_WriteAll(wchar_t const *path, u8 const *buf, usize const buf_size)
{
    DqnFile file = {};
    if (!file.Open(path, DqnFile::Flag::FileReadWrite, DqnFile::Action::ForceCreate))
    {
        DQN_LOGE("Could not open file at: %s", path);
        return false;
    }

    DQN_DEFER(file.Close());
    usize bytes_written = file.Write(buf, buf_size, 0);
    if (bytes_written != buf_size)
    {
        DQN_LOGE("Bytes written did not match the buffer size, %zu != %zu", bytes_written, buf_size);
        return false;
    }

    return true;
}

DQN_FILE_SCOPE bool DqnFile_ReadAll(wchar_t const *path, u8 *buf, usize buf_size)
{
    DqnFile file = {};
    bool result = file.Open(path, DqnFile::Flag::FileRead, DqnFile::Action::OpenOnly);
    DQN_DEFER(file.Close());

    // TODO(doyle): Logging
    if (file.size > buf_size || !result)
    {
        DQN_LOGE("Insufficient buffer size given: %zu, required: %zu\n", buf_size, file.size);
        return false;
    }

    usize bytes_read = file.Read(buf, file.size);
    DQN_ASSERT(bytes_read == file.size);
    return result;
}

DQN_FILE_SCOPE bool DqnFile_ReadAll(char const *path, u8 *buf, usize buf_size)
{
    DqnFile file = {};
    bool result  = file.Open(path, DqnFile::Flag::FileRead, DqnFile::Action::OpenOnly);
    DQN_DEFER(file.Close());

    if (!result || file.size > buf_size)
    {
        return false;
    }

    usize bytes_read = file.Read(buf, file.size);
    DQN_ASSERTM(bytes_read == file.size, "%zu != %zu", bytes_read, file.size);
    return result;
}

void DqnFile::Close()
{
    if (this->handle)
    {
#if defined(DQN_IS_WIN32)
        CloseHandle(this->handle);
#else
        fclose((FILE *)this->handle);
#endif
        this->handle = nullptr;
    }

    this->size  = 0;
    this->flags = 0;
}

#if defined(DQN_IS_WIN32)
    DQN_COMPILE_ASSERT(sizeof(DWORD)  == sizeof(u32));
#endif

bool DqnFile_Size(wchar_t const *path, usize *size)
{
    DqnFileInfo info = {};
    if (DqnFile_GetInfo(path, &info))
    {
        *size = info.size;
        return true;
    }

    return false;
}

bool DqnFile_Size(char const *path, usize *size)
{
    // TODO(doyle): Logging
#if defined(DQN_IS_WIN32)
    // TODO(doyle): MAX PATH is baad
    wchar_t wide_path[MAX_PATH] = {0};
    DqnWin32_UTF8ToWChar(path, wide_path, DQN_ARRAY_COUNT(wide_path));
    return DqnFile_Size(wide_path, size);

#else
    // TODO(doyle): Error logging
    bool result = DqnFile__UnixGetFileSize(path, size);
    return result;
#endif
}

bool DqnFile_MakeDir(char const *path)
{
#if defined(DQN_IS_WIN32)
    // TODO(doyle): Handle error and this is super incomplete. Cannot create
    // directories recursively
    CreateDirectoryA(path, nullptr /*lpSecurityAttributes*/);
    return true;
#else
    return false;
#endif
}

bool DqnFile_GetInfo(wchar_t const *path, DqnFileInfo *info)
{
#if defined(DQN_IS_WIN32)
    auto FileTimeToSeconds = [](FILETIME const *time) -> i64 {
        ULARGE_INTEGER time_large_int = {};
        time_large_int.LowPart        = time->dwLowDateTime;
        time_large_int.HighPart       = time->dwHighDateTime;

        u64 result = (time_large_int.QuadPart / 10000000ULL) - 11644473600ULL;
        return result;
    };

    WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
    if (GetFileAttributesExW(path, GetFileExInfoStandard, &attrib_data))
    {
        info->create_time_in_s     = FileTimeToSeconds(&attrib_data.ftCreationTime);
        info->last_access_time_in_s = FileTimeToSeconds(&attrib_data.ftLastAccessTime);
        info->last_write_time_in_s  = FileTimeToSeconds(&attrib_data.ftLastWriteTime);

        // TODO(doyle): What if usize is < Quad.part?
        LARGE_INTEGER large_int = {};
        large_int.HighPart      = attrib_data.nFileSizeHigh;
        large_int.LowPart       = attrib_data.nFileSizeLow;
        info->size            = (usize)large_int.QuadPart;

        return true;
    }

#else
    // NOTE: Wide char not supported on unix
    DQN_ASSERT(DQN_INVALID_CODE_PATH);

#endif

    return false;
}

bool DqnFile_GetInfo(char const *path, DqnFileInfo *info)
{
#if defined(DQN_IS_WIN32)
    // TODO(doyle): MAX PATH is baad
    wchar_t wide_path[MAX_PATH] = {};
    DqnWin32_UTF8ToWChar(path, wide_path, DQN_ARRAY_COUNT(wide_path));
    return DqnFile_GetInfo(wide_path, info);

#else
    struct stat file_stat = {};
    if (stat(path, &file_stat))
    {
        return false;
    }

    info->size                  = file_stat.st_size;
    info->create_time_in_s      = 0;
    info->last_write_time_in_s  = file_stat.st_mtime;
    info->last_access_time_in_s = file_stat.st_atime;

    return true;
#endif
}


bool DqnFile_Delete(char const *path)
{
#if defined(DQN_IS_WIN32)
    bool result = DeleteFileA(path);
#else
    bool result = (unlink(path) == 0);
#endif
    return result;
}

bool DqnFile_Delete(wchar_t const *path)
{
#if defined(DQN_IS_WIN32)
    bool result = DeleteFileW(path);
    return result;
#else
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return false;
#endif
}

bool DqnFile_Copy(char const *src, char const *dest)
{
    // TODO(doyle): Logging
#if defined(DQN_IS_WIN32)
    BOOL result = (CopyFileA(src, dest, /*FailIfExist*/false) != 0);
    if (result == 0)
        DqnWin32_DisplayLastError("CopyFile failed: ");

    return (result != 0);

#else
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return false;
#endif
}

bool DqnFile_Copy(wchar_t const *src, wchar_t const *dest)
{
    // TODO(doyle): Logging
#if defined(DQN_IS_WIN32)
    BOOL result = (CopyFileW(src, dest, /*FailIfExist*/false) != 0);
    if (result == 0)
        DqnWin32_DisplayLastError("CopyFile failed: ");

    return result;
#else
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return false;

#endif
}

char **DqnFile_ListDir(char const *dir, i32 *num_files, DqnMemAPI *api)
{
    char **result = DqnFile__PlatformListDir(dir, num_files, api);
    return result;
}

void DqnFile_ListDirFree(char **file_list, i32 num_files, DqnMemAPI *api)
{
    if (file_list)
    {
        for (isize i = 0; i < num_files; i++)
        {
            if (file_list[i]) api->Free(file_list[i]);
            file_list[i] = nullptr;
        }

        api->Free(file_list);
    }
}

// XPlatform > #DqnTimer
// =================================================================================================
#if defined (DQN_IS_WIN32)
FILE_SCOPE f64 DqnTimerInternal_Win32QueryPerfCounterTimeInMs()
{
    LOCAL_PERSIST LARGE_INTEGER query_perf_freq = {0};
    if (query_perf_freq.QuadPart == 0)
    {
        QueryPerformanceFrequency(&query_perf_freq);
        DQN_ASSERT(query_perf_freq.QuadPart != 0);
    }

    LARGE_INTEGER qpc_result;
    QueryPerformanceCounter(&qpc_result);

    // Convert to microseconds first then divide by ticks per second then to milliseconds
    qpc_result.QuadPart *= 1000000;
    f64 timestamp = qpc_result.QuadPart / (f64)query_perf_freq.QuadPart;
    timestamp /= 1000.0f;
    return timestamp;
}
#endif

DQN_FILE_SCOPE f64 DqnTimer_NowInMs()
{
    f64 result = 0;
#if defined(DQN_IS_WIN32)
    result = DQN_MAX(DqnTimerInternal_Win32QueryPerfCounterTimeInMs(), 0);

#else
    struct timespec time_spec = {0};
    if (clock_gettime(CLOCK_MONOTONIC, &time_spec))
    {
        // TODO(doyle): Failed logging
        DQN_ASSERT(DQN_INVALID_CODE_PATH);
    }
    else
    {
        result = (f64)((time_spec.tv_sec * 1000.0f) + (time_spec.tv_nsec / 1000000.0f));
    }

#endif
    return result;
};

DQN_FILE_SCOPE f64 DqnTimer_NowInS() { return DqnTimer_NowInMs() / 1000.0f; }

// XPlatform > #DqnLock
// =================================================================================================
bool DqnLock::Init()
{
#if defined(DQN_IS_WIN32)
    if (InitializeCriticalSectionEx(&this->win32_handle, this->win32_spin_count, 0))
    {
        return true;
    }

#else
    // NOTE: Static initialise, pre-empt a lock so that it gets initialised as per documentation
    this->unix_handle = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
    this->Acquire();
    this->Release();
    return true;
#endif

    return false;
}

void DqnLock::Acquire()
{
#if defined(DQN_IS_WIN32)
    EnterCriticalSection(&this->win32_handle);
#else
    // TODO(doyle): Better error handling
    i32 error = pthread_mutex_lock(&this->unix_handle);
    DQN_ASSERT(error == 0);
#endif
}

void DqnLock::Release()
{
#if defined(DQN_IS_WIN32)
    LeaveCriticalSection(&this->win32_handle);
#else
    // TODO(doyle): better error handling
    i32 error = pthread_mutex_unlock(&this->unix_handle);
    DQN_ASSERT(error == 0);
#endif
}

void DqnLock::Delete()
{
#if defined(DQN_IS_WIN32)
    DeleteCriticalSection(&this->win32_handle);
#else
    i32 error = pthread_mutex_destroy(&this->unix_handle);
    DQN_ASSERT(error == 0);
#endif
}

// XPlatform > #DqnJobQueue
// =================================================================================================
typedef void *DqnThreadCallbackInternal(void *thread_param);
usize DQN_JOB_QUEUE_INTERNAL_THREAD_DEFAULT_STACK_SIZE = 0;

FILE_SCOPE u32 DqnJobQueueInternal_ThreadCreate(usize stackSize,
                                                DqnThreadCallbackInternal *thread_callback,
                                                void *thread_param, u32 num_threads)
{
    u32 num_threads_created = 0;

#if defined(DQN_IS_WIN32)
    DQN_ASSERT(stackSize == 0 || !thread_callback);
    for (u32 i = 0; i < num_threads; i++)
    {
        HANDLE handle = CreateThread(nullptr, stackSize, (LPTHREAD_START_ROUTINE)thread_callback,
                                     thread_param, 0, nullptr);
        CloseHandle(handle);
        num_threads_created++;
    }

#else
    // TODO(doyle): Better error handling
    pthread_attr_t attribute = {};
    DQN_ASSERT(pthread_attr_init(&attribute) == 0);

    // Allows us to use pthread_join() which lets us wait till a thread finishes execution
    DQN_ASSERT(pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE) == 0);

    // TODO(doyle): Persist thread handles
    for (u32 i = 0; i < num_threads; i++)
    {
        pthread_t thread = {};
        pthread_create(&thread, &attribute, thread_callback, thread_param);
        num_threads_created++;
    }

    DQN_ASSERT(pthread_attr_destroy(&attribute) == 0);
#endif

    DQN_ASSERT(num_threads_created == num_threads);
    return num_threads_created;
}


FILE_SCOPE void *DqnJobQueueInternal_ThreadCallback(void *thread_param)
{
    DqnJobQueue *queue = (DqnJobQueue *)thread_param;
    for (;;)
    {
        if (!DqnJobQueue_TryExecuteNextJob(queue))
        {
#if defined(DQN_IS_WIN32)
            WaitForSingleObjectEx(queue->semaphore, INFINITE, false);
#else
            sem_wait(&queue->semaphore);
#endif
        }
    }
}

FILE_SCOPE bool DqnJobQueueInternal_CreateSemaphore(DqnJobQueue *queue, u32 init_signal_count, u32 max_signal_count)
{
    if (!queue) return false;

#if defined(DQN_IS_WIN32)
    queue->semaphore = (void *)CreateSemaphoreA(nullptr, init_signal_count, max_signal_count, nullptr);
    DQN_ASSERT(queue->semaphore);

#else
    // TODO(doyle): Use max count for unix
    // TODO(doyle): Error handling
    const u32 UNIX_DONT_SHARE_BETWEEN_PROCESSES = 0;
    i32 error = sem_init(&queue->semaphore, UNIX_DONT_SHARE_BETWEEN_PROCESSES, 0);
    DQN_ASSERT(error == 0);

    for (u32 i = 0; i < init_signal_count; i++)
        DQN_ASSERT(sem_post(&queue->semaphore) == 0);
#endif

    return true;
}

FILE_SCOPE bool DqnJobQueueInternal_ReleaseSemaphore(DqnJobQueue *queue)
{
    DQN_ASSERT(queue);

#if defined(DQN_IS_WIN32)
    DQN_ASSERT(queue->semaphore);
    ReleaseSemaphore(queue->semaphore, 1, nullptr);

#else
    // TODO(doyle): Error handling
    DQN_ASSERT(sem_post(&queue->semaphore) == 0);
#endif

    return true;
}

DQN_FILE_SCOPE bool DqnJobQueue_Init(DqnJobQueue *queue, DqnJob *job_list, u32 job_list_size, u32 num_threads)
{
    if (!queue || !job_list || job_list_size == 0 || num_threads == 0) return false;
    queue->job_list = job_list;
    queue->size    = job_list_size;

    DQN_ASSERT(DqnJobQueueInternal_CreateSemaphore(queue, 0, num_threads));

    // Create threads
    u32 num_threads_created = DqnJobQueueInternal_ThreadCreate(
        DQN_JOB_QUEUE_INTERNAL_THREAD_DEFAULT_STACK_SIZE, DqnJobQueueInternal_ThreadCallback,
        (void *)queue, num_threads);
    DQN_ASSERT(num_threads == num_threads_created);

    return true;
}

DQN_FILE_SCOPE bool DqnJobQueue_AddJob(DqnJobQueue *queue, const DqnJob job)
{
    i32 newJobInsertIndex = (queue->jobInsertIndex + 1) % queue->size;
    if (newJobInsertIndex == queue->jobToExecuteIndex) return false;

    queue->job_list[queue->jobInsertIndex] = job;

    DqnAtomic_Add32(&queue->num_jobs_queued, 1);
    DQN_ASSERT(DqnJobQueueInternal_ReleaseSemaphore(queue));

    queue->jobInsertIndex = newJobInsertIndex;
    return true;
}

DQN_FILE_SCOPE void DqnJobQueue_BlockAndCompleteAllJobs(DqnJobQueue *queue)
{
    while (DqnJobQueue_TryExecuteNextJob(queue) || !DqnJobQueue_AllJobsComplete(queue))
        ;
}

DQN_FILE_SCOPE bool DqnJobQueue_TryExecuteNextJob(DqnJobQueue *queue)
{
    if (!queue) return false;

    i32 originalJobToExecute = queue->jobToExecuteIndex;
    if (originalJobToExecute != queue->jobInsertIndex)
    {
        i32 newJobIndexForNextThread = (originalJobToExecute + 1) % queue->size;
        i32 index = DqnAtomic_CompareSwap32(&queue->jobToExecuteIndex, newJobIndexForNextThread,
                                            originalJobToExecute);

        // NOTE: If we weren't successful at the interlock, another thread has
        // taken the work and we can't know if there's more work or not. So
        // irrespective of that result, return true to let the thread check
        // again for more work.
        if (index == originalJobToExecute)
        {
            DqnJob job = queue->job_list[index];
            job.callback(queue, job.user_data);
            DqnAtomic_Add32(&queue->num_jobs_queued, -1);
        }

        return true;
    }

    return false;
}

DQN_FILE_SCOPE bool DqnJobQueue_AllJobsComplete(DqnJobQueue *queue)
{
    if (!queue) return false;

    bool result = (queue->num_jobs_queued == 0);
    return result;
}

bool DqnJobQueue::Init(DqnJob *job_list_, u32 job_list_size, u32 num_threads)
{
    bool result = DqnJobQueue_Init(this, job_list_, job_list_size, num_threads);
    return result;
}

bool DqnJobQueue::AddJob           (const DqnJob job) { return DqnJobQueue_AddJob(this, job);             }
void DqnJobQueue::BlockAndCompleteAllJobs()           {        DqnJobQueue_BlockAndCompleteAllJobs(this); }
bool DqnJobQueue::TryExecuteNextJob()                 { return DqnJobQueue_TryExecuteNextJob(this);       }
bool DqnJobQueue::AllJobsComplete  ()                 { return DqnJobQueue_AllJobsComplete(this);         }

// XPlatform > #DqnAtomic
// =================================================================================================

#if defined(DQN_IS_WIN32)
    DQN_COMPILE_ASSERT(sizeof(LONG) == sizeof(i32));
#endif

DQN_FILE_SCOPE i32 DqnAtomic_CompareSwap32(i32 volatile *dest, i32 swap_val, i32 compare_val)
{
    i32 result = 0;
#if defined(DQN_IS_WIN32)
    result = (i32)InterlockedCompareExchange((LONG volatile *)dest, (LONG)swap_val, (LONG)compare_val);

#else
    result = __sync_val_compare_and_swap(dest, compare_val, swap_val);
#endif
    return result;
}

DQN_FILE_SCOPE i32 DqnAtomic_Add32(i32 volatile *src, i32 value)
{
    i32 result = 0;
#if defined(DQN_IS_WIN32)
    result = (i32)InterlockedAdd((LONG volatile *)src, value);

#else
    result = __sync_add_and_fetch(src, value);
#endif

    return result;
}

// XPlatform > #DqnOS
// =================================================================================================
#if defined(DQN_IS_UNIX)
#include <sys/mman.h>
#endif

void *DqnOS_VAlloc(isize size, void *base_addr)
{
    void *result = nullptr;
#if defined (DQN_IS_WIN32)
    result = VirtualAlloc(base_addr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    DQN_ASSERT(result);
#else
    result = mmap(
        base_addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1 /*fd*/, 0 /*offset into fd*/);
    DQN_ASSERT(result != MAP_FAILED);
#endif

    return result;
}

void DqnOS_VFree(void *address, isize size)
{
#if defined (DQN_IS_WIN32)
    BOOL result = VirtualFree(address, 0 /*size*/, MEM_RELEASE);
    (void)size;
    DQN_ASSERT(result);
#else
    int result = munmap(address, size);
    DQN_ASSERT(result != 0);
#endif
}

#define DQN_OS_GET_THREADS_AND_CORES(name) \
    DQN_FILE_SCOPE void name(u32 *const num_cores, u32 *const num_threads_per_core)

#if defined(DQN_IS_UNIX)
DQN_OS_GET_THREADS_AND_CORES(DqnOS_GetThreadsAndCores)
{
    if (!num_threads_per_core && !num_cores) return;

    // TODO(doyle): Not exactly standard

    usize fileSize = 0;
    if (u8 *read_buffer = DqnFile_ReadAll("/proc/cpuinfo", &fileSize))
    {
        char const *src_ptr = reinterpret_cast<char *>(read_buffer);
        usize src_len       = fileSize;

#define DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(ptr, len, offset)                                     \
    ptr += offset;                                                                                 \
    len -= offset

        if (num_threads_per_core)
        {
            *num_threads_per_core = 0;
            // Find the offset to the processor field and move to it
            DqnSlice<char const> processor = DQN_SLICE("processor");
            i32 processorOffset            = DqnStr_FindFirstOccurence(src_ptr, src_len, processor.data, processor.len);

            DQN_ASSERT(processorOffset != -1);
            DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(src_ptr, src_len, processorOffset);

            // Find the offset to the colon delimiter and advance to 1 after it
            i32 colon_offset = DqnStr_FindFirstOccurence(src_ptr, src_len, ":", 1) + 1;
            DQN_ASSERT(colon_offset != -1);
            DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(src_ptr, src_len, colon_offset);

            // Read num processors, i.e. logical cores/hyper threads
            *num_threads_per_core = Dqn_StrToI64(src_ptr, src_len);
            if (*num_threads_per_core == 0) *num_threads_per_core = 1;
        }

        if (num_cores)
        {
            *num_cores   = 0;
            // Find the offset to the cpu cores field and move to it
            DqnSlice<char const> cpuCores = DQN_SLICE("cpu cores");
            i32 cpu_cores_offset            = DqnStr_FindFirstOccurence(src_ptr, src_len, cpuCores.data, cpuCores.len);
            DQN_ASSERT(cpu_cores_offset != -1);

            DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(src_ptr, src_len, cpu_cores_offset);

            // Find the offset to the colon delimiter and advance to 1 after it
            i32 colon_offset = DqnStr_FindFirstOccurence(src_ptr, src_len, ":", 1) + 1;
            DQN_ASSERT(colon_offset != -1);
            DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(src_ptr, src_len, colon_offset);

            // Read num cores value, i.e. physical cores
            *num_cores = Dqn_StrToI64(src_ptr, src_len);
        }
        DQN_DEFAULT_HEAP_ALLOCATOR->Free(read_buffer);
    }
    else
    {
        // TODO(doyle): Out of mem
        DQN_ASSERT(DQN_INVALID_CODE_PATH);
    }
}
#endif // DQN_IS_UNIX

#if defined(DQN_IS_WIN32)
DQN_OS_GET_THREADS_AND_CORES(DqnOS_GetThreadsAndCores)
{
    if (num_threads_per_core)
    {
        SYSTEM_INFO system_info;
        GetNativeSystemInfo(&system_info);
        *num_threads_per_core = system_info.dwNumberOfProcessors;
    }

    if (num_cores)
    {
        *num_cores = 0;
        DWORD required_size = 0;
        u8 insufficient_buf = {0};
        GetLogicalProcessorInformationEx(
            RelationProcessorCore,
            (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)insufficient_buf,
            &required_size);

        auto *raw_proc_info_array = (u8 *)DqnMem_Calloc(required_size);
        if (!raw_proc_info_array)
        {
            DQN_LOGE("Could not allocate memory for array required: %$d\n");
            return;
        }

        if (GetLogicalProcessorInformationEx(
                RelationProcessorCore,
                (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)raw_proc_info_array,
                &required_size))
        {
            SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *logical_proc_info =
                (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)raw_proc_info_array;
            DWORD bytes_read = 0;

            do
            {
                // NOTE: High efficiency value has greater performance and less efficiency.
                PROCESSOR_RELATIONSHIP *procInfo = &logical_proc_info->Processor;
                // u32 efficiency                   = procInfo->EfficiencyClass;
                (*num_cores)++;
                DQN_ASSERT(logical_proc_info->Relationship == RelationProcessorCore);
                DQN_ASSERT(procInfo->GroupCount == 1);

                bytes_read += logical_proc_info->Size;
                logical_proc_info =
                    (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)((u8 *)logical_proc_info +
                                                                logical_proc_info->Size);
            } while (bytes_read < required_size);
        }
        else
        {
            DqnWin32_DisplayLastError("GetLogicalProcessorInformationEx() failed");
        }

        DqnMem_Free(raw_proc_info_array);
    }
}
#endif // DQN_IS_WIN32

#ifdef DQN_IS_WIN32
// #DqnWin32
// =================================================================================================
DQN_FILE_SCOPE i32 DqnWin32_UTF8ToWChar(char const *in, wchar_t *out, i32 out_len)
{
    i32 result = MultiByteToWideChar(CP_UTF8, 0, in, -1, out, out_len);

    if (result == 0xFFFD || 0)
    {
        DQN__WIN32_ERROR_BOX("WideCharToMultiByte() failed.", nullptr);
        return -1;
    }

    return result;
}

DQN_FILE_SCOPE i32 DqnWin32_WCharToUTF8(wchar_t const *in, char *out, i32 out_len)
{
    i32 result =
        WideCharToMultiByte(CP_UTF8, 0, in, -1, out, out_len, nullptr, nullptr);

    if (result == 0xFFFD || 0)
    {
        DQN__WIN32_ERROR_BOX("WideCharToMultiByte() failed.", nullptr);
        return -1;
    }

    return result;
}

DQN_FILE_SCOPE void DqnWin32_GetClientDim(HWND window, LONG *width, LONG *height)
{
    RECT rect;
    GetClientRect(window, &rect);
    if (width)  *width  = rect.right - rect.left;
    if (height) *height = rect.bottom - rect.top;
}

DQN_FILE_SCOPE void DqnWin32_GetRectDim(RECT const rect, LONG *width, LONG *height)
{
    if (width)  *width  = rect.right - rect.left;
    if (height) *height = rect.bottom - rect.top;
}

DQN_FILE_SCOPE char const *DqnWin32_GetLastError()
{
    LOCAL_PERSIST char err_msg[2048];
    err_msg[0] = 0;

    DWORD error = GetLastError();
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr, error, 0, err_msg, DQN_ARRAY_COUNT(err_msg), nullptr);

    return err_msg;
}

DQN_FILE_SCOPE void DqnWin32_DisplayLastError(char const *err_prefix)
{
    if (err_prefix)
    {
        char formatted_err[2048] = {0};
        Dqn_sprintf(formatted_err, "%s: %s", err_prefix, DqnWin32_GetLastError());
        DQN__WIN32_ERROR_BOX(formatted_err, nullptr);
    }
    else
    {
        DQN__WIN32_ERROR_BOX(DqnWin32_GetLastError(), nullptr);
    }
}

const i32 DQN__WIN32_INTERNAL_ERROR_MSG_SIZE = 2048;
DQN_FILE_SCOPE void DqnWin32_DisplayErrorCode(DWORD error, char const *err_prefix)
{
    char err_msg[DQN__WIN32_INTERNAL_ERROR_MSG_SIZE] = {0};
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr, error, 0, err_msg, DQN_ARRAY_COUNT(err_msg), nullptr);

    char formatted_err[2048] = {0};
    Dqn_sprintf(formatted_err, "%s: %s", err_prefix, err_msg);
    DQN__WIN32_ERROR_BOX(formatted_err, nullptr);
}

DQN_FILE_SCOPE void DqnWin32_OutputDebugString(char const *fmt_str, ...)
{
    char str[DQN__WIN32_INTERNAL_ERROR_MSG_SIZE] = {0};

    va_list va;
    va_start(va, fmt_str);
    {
        i32 num_copied = Dqn_vsprintf(str, fmt_str, va);
        DQN_ASSERT(num_copied < DQN_ARRAY_COUNT(str));
    }
    va_end(va);

    OutputDebugStringA(str);
}

DQN_FILE_SCOPE i32 DqnWin32_GetEXEDirectory(char *buf, u32 buf_len)
{
    if (!buf || buf_len == 0) return -1;
    u32 copied_len = GetModuleFileNameA(nullptr, buf, buf_len);
    if (copied_len == buf_len) return -1;

    // NOTE: Should always work if GetModuleFileName works and we're running an
    // executable.
    i32 last_slash_index = 0;
    for (i32 i = copied_len; i > 0; i--)
    {
        if (buf[i] == '\\')
        {
            last_slash_index = i;
            break;
        }
    }

    return last_slash_index;
}
#endif // DQN_IS_WIN32
#endif // DQN__XPLATFORM_LAYER
