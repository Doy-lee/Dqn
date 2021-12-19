#if !defined(DQN_H)
#define DQN_H
// -----------------------------------------------------------------------------
// NOTE: Dqn
// -----------------------------------------------------------------------------
// General all-purpose utility library.
//
// -----------------------------------------------------------------------------
// NOTE: Enabling Library Primitives
// -----------------------------------------------------------------------------
// Access to library primitives must be enabled using the following macros
//
// #define DQN_WITH_CRT_ALLOCATOR  // Dqn_CRTAllocator
// #define DQN_WITH_DSMAP          // Dqn_DSMap
// #define DQN_WITH_FIXED_ARRAY    // Dqn_FixedArray
// #define DQN_WITH_FIXED_STRING   // Dqn_FixedString
// #define DQN_WITH_HEX            // Dqn_Hex and friends ...
// #define DQN_WITH_JSON_WRITER    // Dqn_JsonWriter
// #define DQN_WITH_MAP            // Dqn_Map
// #define DQN_WITH_MATH           // Dqn_V2/3/4/Mat4 and friends ...
// #define DQN_WITH_WIN_NET        // Dqn_WinNet
//
// -----------------------------------------------------------------------------
// NOTE: Configuration
// -----------------------------------------------------------------------------
// #define DQN_IMPLEMENTATION
//     Define this in one and only one C++ file to enable the implementation
//     code of the header file
//
// #define DQN_NO_ASSERT
//     Turn all assertion macros to no-ops
//
// #define DQN_NO_WIN32_MINIMAL_HEADER
//     Define this to stop this library from defining a minimal subset of Win32
//     prototypes and definitions in this file. Useful for stopping redefinition
//     of symbols if another library includes "Windows.h"
//
// #define DQN_STATIC_API
//     Apply static to all function definitions and disable external linkage to
//     other translation units.
//
// #define DQN_STB_SPRINTF_HEADER_ONLY
//     Define this to stop this library from defining
//     STB_SPRINTF_IMPLEMENTATION. Useful if another library uses and includes
//     "stb_sprintf.h"
//
// #define DQN_MEMZERO_DEBUG_BYTE 0xDA
//     By defining 'DQN_MEMZERO_DEBUG_BYTE' to some byte value this will enable
//     functions that receive Dqn_ZeroMem::No to memset the non-zeroed memory to
//     'DQN_MEMZERO_DEBUG_BYTE' to help detect holding pointers to invalid memory,
//     i.e. holding pointers to an array element that was popped from the array.
//
// #define DQN_ALLOCATION_TRACING 1
//     When defined to 0 all tracing code is compiled out.
//
//     When defined to 1, some allocating calls in the library will automatically
//     get passed in the file name, function name, line number and an optional tag.
//
//     For data structures that have a 'Dqn_AllocationTracer' member, the caller
//     can set the 'Dqn_AllocationTracer' to log allocations every time they are
//     made in the data structure.
//
//     'Tagged' variants of functions accept as the last parameter, a 'tag' that is
//     the descriptor/name to describe the allocation. All extra parameters and
//     tags are compiled out when tracing is disabled.
//
// #define DQN_DEBUG_THREAD_CONTEXT
//     Define this macro to record allocation stats for arenas used in the
//     thread context. The thread context arena stats can be printed by using
//     Dqn_LibDumpThreadContextArenaStats.

// -------------------------------------------------------------------------------------------------
// NOTE: Compiler
// -------------------------------------------------------------------------------------------------
// NOTE: Warning! Order is important here, clang-cl on Windows defines _MSC_VER
#if defined(_MSC_VER)
    #if defined(__clang__)
        #define DQN_COMPILER_W32_CLANG
    #else
        #define DQN_COMPILER_W32_MSVC
        #pragma warning(push)
        #pragma warning(disable: 4201) // warning C4201: nonstandard extension used: nameless struct/union
    #endif
#elif defined(__clang__)
    #define DQN_COMPILER_CLANG
#elif defined(__GNUC__)
    #define DQN_COMPILER_GCC
#endif

#if defined(_WIN32)
    #define DQN_OS_WIN32
#else
    #define DQN_OS_UNIX
#endif

#if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    #if defined(_CRT_SECURE_NO_WARNINGS)
        #define DQN_CRT_SECURE_NO_WARNINGS_PREVIOUSLY_DEFINED
    #else
        #define _CRT_SECURE_NO_WARNINGS
    #endif
#endif

#include <stdarg.h> // va_list
#include <stdio.h>  // fprintf, FILE, stdout, stderr

// -------------------------------------------------------------------------------------------------
// NOTE: Overridable Macros
// -------------------------------------------------------------------------------------------------
#if !defined(DQN_CALLOC)
    #include <stdlib.h>
    #define DQN_CALLOC(count, size) calloc(count, size)
#endif

#if !defined(DQN_MALLOC)
    #include <stdlib.h>
    #define DQN_MALLOC(size) malloc(size)
#endif

#if !defined(DQN_REALLOC)
    #include <stdlib.h>
    #define DQN_REALLOC(ptr, size) realloc(ptr, size)
#endif

#if !defined(DQN_FREE)
    #include <stdlib.h>
    #define DQN_FREE(ptr) free(ptr)
#endif

#if !defined(DQN_MEMCOPY)
    #include <string.h>
    #define DQN_MEMCOPY(dest, src, count) memcpy(dest, src, count)
#endif

#if !defined(DQN_MEMSET)
    #include <string.h>
    #define DQN_MEMSET(dest, value, count) memset(dest, value, count)
#endif

#if !defined(DQN_MEMCMP)
    #include <string.h>
    #define DQN_MEMCMP(ptr1, ptr2, num) memcmp(ptr1, ptr2, num)
#endif

#if !defined(DQN_MEMMOVE)
    #include <string.h>
    #define DQN_MEMMOVE(dest, src, num) memmove(dest, src, num)
#endif

#if !defined(DQN_SQRTF)
    #include <math.h>
    #define DQN_SQRTF(val) sqrtf(val)
#endif

// -------------------------------------------------------------------------------------------------
// NOTE: Utility Macros
// -------------------------------------------------------------------------------------------------
#define DQN_CAST(val) (val)
#define DQN_SWAP(a, b)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        auto temp = a;                                                                                                  \
        a        = b;                                                                                                  \
        b        = temp;                                                                                                \
    } while (0)

// NOTE: Prefer the templated Dqn_Array/CharCount function for type-safety. I prefer
// the macro version for embedding within macros for debuggers. When we step
// into a function call using the macro like, DQN_STRING("...") which is very
// common, the debugger jumps into the templated functions which is a waste of
// time (they're bug free by virtue of templatizing).
#define DQN_ARRAY_COUNT(array) (sizeof(array)/(sizeof((array)[0])))
#define DQN_CHAR_COUNT(string) (sizeof(string) - 1)

#define DQN_LEN_AND_STR(string) DQN_CHAR_COUNT(str), string
#define DQN_STR_AND_LEN(string) string, DQN_CHAR_COUNT(string)
#define DQN_STR_AND_LEN_I(string) string, (int)DQN_CHAR_COUNT(string)
#define DQN_FOR_EACH(i, limit) for (Dqn_isize i = 0; i < (Dqn_isize)(limit); ++i)

#define DQN_BYTES(val) (val)
#define DQN_KILOBYTES(val) (1024ULL * DQN_BYTES(val))
#define DQN_MEGABYTES(val) (1024ULL * DQN_KILOBYTES(val))
#define DQN_GIGABYTES(val) (1024ULL * DQN_MEGABYTES(val))

#define DQN_MINS_TO_S(val) ((val) * 60ULL)
#define DQN_HOURS_TO_S(val) (DQN_MINS_TO_S(val) * 60ULL)
#define DQN_DAYS_TO_S(val) (DQN_HOURS_TO_S(val) * 24ULL)
#define DQN_YEARS_TO_S(val) (DQN_DAYS_TO_S(val) * 365ULL)
#define DQN_ISIZEOF(val) DQN_CAST(ptrdiff_t)sizeof(val)

#if !defined(DQN_DEBUG_BREAK)
    #if defined(NDEBUG)
        #define DQN_DEBUG_BREAK
    #else
        #if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
            #define DQN_DEBUG_BREAK __debugbreak()
        #elif defined(DQN_COMPILER_CLANG) || defined(DQN_COMPILER_GCC)
            #include <signal.h>
            #define DQN_DEBUG_BREAK raise(SIGTRAP)
        #elif
            #error "Unhandled compiler"
        #endif
    #endif
#endif

#define DQN_SECONDS_TO_MS(val) ((val) * 1000.0f)

#if defined(DQN_STATIC_API)
    #define DQN_API static
#else
    #define DQN_API
#endif
#define DQN_LOCAL_PERSIST static
#define DQN_FILE_SCOPE static

// -------------------------------------------------------------------------------------------------
// NOTE: Overridable Math Macros
// ------------------------------------------------------------------------------------------------
#ifndef DQN_SINF
    #include <math.h>
    #define DQN_SINF(val) sinf(val)
#endif // DQN_SINF

#ifndef DQN_COSF
    #include <math.h>
    #define DQN_COSF(val) cosf(val)
#endif // DQN_COSF

#ifndef DQN_TANF
    #include <math.h>
    #define DQN_TANF(val) tanf(val)
#endif // DQN_TANF

// -------------------------------------------------------------------------------------------------
// NOTE: Math Macros
// ------------------------------------------------------------------------------------------------
#define DQN_PI 3.14159265359f

#define DQN_DEGREE_TO_RADIAN(degrees) ((degrees) * (DQN_PI / 180.0f))
#define DQN_RADIAN_TO_DEGREE(radians) ((radians) * (180.f * DQN_PI))

#define DQN_ABS(val) (((val) < 0) ? (-(val)) : (val))
#define DQN_MAX(a, b) ((a > b) ? (a) : (b))
#define DQN_MIN(a, b) ((a < b) ? (a) : (b))
#define DQN_SQUARED(val) ((val) * (val))

// -------------------------------------------------------------------------------------------------
// NOTE: Assert Macro
// ------------------------------------------------------------------------------------------------
#define DQN_HARD_ASSERT(expr) DQN_HARD_ASSERT_MSG(expr, "")
#define DQN_HARD_ASSERT_MSG(expr, fmt, ...)                                                                            \
    if (!(expr))                                                                                                       \
    {                                                                                                                  \
        DQN_LOG_E("Assert: [" #expr "] " fmt, ##__VA_ARGS__);                                                          \
        DQN_DEBUG_BREAK;                                                                                               \
    }

#if defined(DQN_NO_ASSERT)
    #define DQN_ASSERT(expr)
    #define DQN_ASSERT_MSG(expr, fmt, ...)
#else
    #define DQN_ASSERT(expr) DQN_HARD_ASSERT_MSG(expr, "")
    #define DQN_ASSERT_MSG(expr, fmt, ...) DQN_HARD_ASSERT_MSG(expr, fmt, ##__VA_ARGS__)
#endif

#define DQN_INVALID_CODE_PATH_MSG(fmt, ...) DQN_ASSERT_MSG(0, fmt, ##__VA_ARGS__)
#define DQN_INVALID_CODE_PATH DQN_INVALID_CODE_PATH_MSG("Invalid code path triggered")

#define DQN_HARD_INVALID_CODE_PATH_MSG(fmt, ...) DQN_HARD_ASSERT_MSG(0, fmt, ##__VA_ARGS__)
#define DQN_HARD_INVALID_CODE_PATH DQN_HARD_INVALID_CODE_PATH_MSG("Invalid code path triggered")

// ------------------------------------------------------------------------------------------------
// NOTE: Typedefs
// ------------------------------------------------------------------------------------------------
// Use compiler builtins and define our own constants to avoid a dependency on stdint.h
typedef double Dqn_f64;
typedef float Dqn_f32;
typedef signed char Dqn_i8;
typedef unsigned char Dqn_u8;
typedef signed short Dqn_i16;
typedef unsigned short Dqn_u16;
typedef signed int Dqn_i32;
typedef unsigned int Dqn_u32;
typedef unsigned int Dqn_uint;
#if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
typedef signed __int64 Dqn_i64;
typedef unsigned __int64 Dqn_u64;
#else
typedef signed long long Dqn_i64;
typedef unsigned long long Dqn_u64;
#endif
typedef Dqn_i32 Dqn_b32;
typedef Dqn_i8 Dqn_b8;

#if defined(__ppc64__) || defined(__aarch64__) || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64)
    typedef Dqn_u64 Dqn_uintptr;
    typedef Dqn_i64 Dqn_intptr;
    typedef Dqn_u64 Dqn_usize;
    typedef Dqn_i64 Dqn_isize;
#else
    typedef Dqn_u32 Dqn_uintptr;
    typedef Dqn_i32 Dqn_intptr;
    typedef Dqn_u32 Dqn_usize;
    typedef Dqn_i32 Dqn_isize;
#endif

Dqn_f32 const DQN_F32_MAX = 3.402823466e+38F;
Dqn_f64 const DQN_F64_MAX = 1.7976931348623158e+308;

Dqn_i8  const DQN_I8_MAX  = 127;
Dqn_i8  const DQN_I8_MIN  = -DQN_I8_MAX - 1;
Dqn_i16 const DQN_I16_MAX = 32767;
Dqn_i16 const DQN_I16_MIN = -DQN_I16_MAX - 1;
Dqn_i32 const DQN_I32_MAX = 2147483647;
Dqn_i32 const DQN_I32_MIN = -DQN_I32_MAX - 1;
Dqn_i64 const DQN_I64_MAX = 9223372036854775807;
Dqn_i64 const DQN_I64_MIN = -DQN_I64_MAX - 1;

Dqn_u8  const DQN_U8_MAX  = 255;
Dqn_u16 const DQN_U16_MAX = 65535;
Dqn_u32 const DQN_U32_MAX = 4294967295;
Dqn_u64 const DQN_U64_MAX = 18446744073709551615ULL;

#if defined(__ppc64__) || defined(__aarch64__) || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64)
typedef Dqn_u64 Dqn_usize;
typedef Dqn_i64 Dqn_isize;
Dqn_isize const DQN_ISIZE_MAX = DQN_I64_MAX;
Dqn_usize const DQN_USIZE_MAX = DQN_U64_MAX;
#else
typedef Dqn_u32 Dqn_usize;
typedef Dqn_i32 Dqn_isize;
Dqn_isize const DQN_ISIZE_MAX = DQN_I32_MAX;
Dqn_usize const DQN_USIZE_MAX = DQN_U32_MAX;
#endif

static_assert(sizeof(int)     == sizeof(Dqn_i32),   "Sanity check int typedef is correct");
static_assert(sizeof(void *)  == sizeof(Dqn_usize), "Require: Pointer can be held in usize (size_t)");
static_assert(sizeof(Dqn_u64) == 8,                 "Sanity check u64 is 8 bytes");
static_assert(sizeof(Dqn_f64) == 8,                 "Sanity check f64 is 8 bytes");

// ------------------------------------------------------------------------------------------------
// NOTE: Win32 Minimal Header
// ------------------------------------------------------------------------------------------------
#if defined(DQN_OS_WIN32)
    #if defined(DQN_NO_WIN32_MINIMAL_HEADER)
    #else
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
            Dqn_i64 QuadPart;
        } LARGE_INTEGER;
    #endif // !defined(DQN_NO_WIN32_MINIMAL_HEADER)
#endif // !defined(DQN_OS_WIN32)

// ------------------------------------------------------------------------------------------------
// NOTE: Compile Time Utilities
// ------------------------------------------------------------------------------------------------
template <typename T, Dqn_isize N> constexpr Dqn_usize Dqn_ArrayCount   (T    const (&)[N]) { return N; }
template <typename T, Dqn_isize N> constexpr Dqn_isize Dqn_ArrayCountI  (T    const (&)[N]) { return N; }
template <typename T, int N>       constexpr Dqn_isize Dqn_ArrayCountInt(T    const (&)[N]) { return N; }
template <Dqn_isize N>             constexpr Dqn_usize Dqn_CharCount    (char const (&)[N]) { return N - 1; }
template <Dqn_isize N>             constexpr Dqn_isize Dqn_CharCountI   (char const (&)[N]) { return N - 1; }
template <int N>                   constexpr Dqn_isize Dqn_CharCountInt (char const (&)[N]) { return N - 1; }

// ------------------------------------------------------------------------------------------------
// NOTE: Defer Macro
// ------------------------------------------------------------------------------------------------
/*
    #include <stdio.h>
    int main()
    {
        DQN_DEFER { printf("Three ..\n"); };
        printf("One ..\n");
        printf("Two ..\n");

        // One ..
        // Two ..
        // Three ..

        return 0;
    }
*/
template <typename Procedure>
struct Dqn_Defer
{
    Procedure proc;
    Dqn_Defer(Procedure p) : proc(p) {}
    ~Dqn_Defer()                     { proc(); }
};

struct Dqn_DeferHelper
{
    template <typename Lambda>
    Dqn_Defer<Lambda> operator+(Lambda lambda) { return Dqn_Defer<Lambda>(lambda); };
};

#define DQN_TOKEN_COMBINE2(x, y) x ## y
#define DQN_TOKEN_COMBINE(x, y) DQN_TOKEN_COMBINE2(x, y)
#define DQN_UNIQUE_NAME(prefix) DQN_TOKEN_COMBINE(prefix, __LINE__)
#define DQN_DEFER const auto DQN_UNIQUE_NAME(defer_lambda_) = Dqn_DeferHelper() + [&]()

// ------------------------------------------------------------------------------------------------
// NOTE: Utility Enums
// ------------------------------------------------------------------------------------------------
enum struct Dqn_ZeroMem { No, Yes };

// Internal helper function that zeroes memory or memsets memory to
// DQN_MEMZERO_DEBUG_BYTE if zero_mem is Dqn_ZeroMem::No.
DQN_API void Dqn__ZeroMemBytes(void *ptr, Dqn_usize count, Dqn_ZeroMem zero_mem);
// -------------------------------------------------------------------------------------------------
// NOTE: Intrinsics
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_AtomicAdd/Exchange return the previous value store in the target
#if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    #include <intrin.h>
    #define Dqn_AtomicAddU32(target, value) _InterlockedExchangeAdd(DQN_CAST(long volatile *)target, value)
    #define Dqn_AtomicAddU64(target, value) _InterlockedExchangeAdd64(DQN_CAST(__int64 volatile *)target, value)
    #define Dqn_AtomicSubU32(target, value) DQN_CAST(unsigned)Dqn_AtomicAddU32(DQN_CAST(long volatile *)target, -DQN_CAST(long)value)
    #define Dqn_AtomicSubU64(target, value) DQN_CAST(Dqn_u64)Dqn_AtomicAddU64(target, -DQN_CAST(Dqn_i64)value)
    #define Dqn_AtomicCompareExchange64(dest, desired_val, prev_val) _InterlockedCompareExchange64(DQN_CAST(__int64 volatile *)dest, desired_val, prev_val)
    #define Dqn_AtomicCompareExchange32(dest, desired_val, prev_val) _InterlockedCompareExchange(DQN_CAST(long volatile *)dest, desired_val, prev_val)
    #define Dqn_CPUClockCycle() __rdtsc()
    #define Dqn_CompilerReadBarrierAndCPUReadFence _ReadBarrier(); _mm_lfence()
    #define Dqn_CompilerWriteBarrierAndCPUWriteFence _WriteBarrier(); _mm_sfence()
#elif defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
    #include <x86intrin.h>
    #define Dqn_AtomicAddU32(target, value) __atomic_fetch_add(target, value, __ATOMIC_ACQ_REL)
    #define Dqn_AtomicAddU64(target, value) __atomic_fetch_add(target, value, __ATOMIC_ACQ_REL)
    #define Dqn_AtomicSubU32(target, value) __atomic_fetch_sub(target, value, __ATOMIC_ACQ_REL)
    #define Dqn_AtomicSubU64(target, value) __atomic_fetch_sub(target, value, __ATOMIC_ACQ_REL)
    #if defined(DQN_COMPILER_GCC)
        #define Dqn_CPUClockCycle() __rdtsc()
    #else
        #define Dqn_CPUClockCycle() __builtin_readcyclecounter()
    #endif
    #define Dqn_CompilerReadBarrierAndCPUReadFence asm volatile("lfence" ::: "memory")
    #define Dqn_CompilerWriteBarrierAndCPUWriteFence asm volatile("sfence" ::: "memory")
#else
    #error "Compiler not supported"
#endif

// TODO(dqn): Force inline
inline Dqn_i64 Dqn_AtomicSetValue64(Dqn_i64 volatile *target, Dqn_i64 value)
{
#if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    Dqn_i64 result;
    do
    {
        result = *target;
    } while (Dqn_AtomicCompareExchange64(target, value, result) != result);
    return result;
#elif defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
    Dqn_i64 result = __sync_lock_test_and_set(target, value);
    return result;
#else
#error Unsupported compiler
#endif
}

inline long Dqn_AtomicSetValue32(long volatile *target, long value)
{
#if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    long result;
    do
    {
        result = *target;
    } while (Dqn_AtomicCompareExchange32(target, value, result) != result);
    return result;
#elif defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
    long result = __sync_lock_test_and_set(target, value);
    return result;
#else
#error Unsupported compiler
#endif
}

struct Dqn_CPUIDRegisters
{
    unsigned int array[4]; // eax, ebx, ecx, edx
};

struct Dqn_TicketMutex
{
    unsigned int volatile ticket;  // The next ticket ID to give out to the thread taking the mutex
    unsigned int volatile serving; // The ticket ID that the mutex will block until it receives
};

// Query the CPU's CPUID function and return the data in the registers
Dqn_CPUIDRegisters Dqn_CPUID                 (int function_id);
void               Dqn_TicketMutexBegin     (Dqn_TicketMutex *mutex);
void               Dqn_TicketMutexEnd       (Dqn_TicketMutex *mutex);

// NOTE: Advance API, more granular functions, the basic sequence to use the API is
/*
   Dqn_TicketMutex mutex = {};
   unsigned int ticket = Dqn_TicketMutexMakeTicket(&mutex);
   Dqn_TicketMutexBeginTicket(&mutex, ticket); // Blocking call until we attain the lock
   Dqn_TicketMutexEnd(&mutex);
 */
unsigned int       Dqn_TicketMutexMakeTicket   (Dqn_TicketMutex *mutex);
void               Dqn_TicketMutexBeginTicket  (const Dqn_TicketMutex *mutex, unsigned int ticket);
Dqn_b32            Dqn_TicketMutexCanLock      (const Dqn_TicketMutex *mutex, unsigned int ticket);

// -------------------------------------------------------------------------------------------------
// NOTE: stb_sprintf
// -------------------------------------------------------------------------------------------------
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
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Fmt
// -------------------------------------------------------------------------------------------------
// Utility functions for calculating the size of format strings. Uses stb_sprintf internally.
// return: The length of the format string without a null terminating byte, i.e. "hi" = 2
DQN_API Dqn_isize Dqn_FmtVLenNoNullTerminator(char const *fmt, va_list args);
DQN_API Dqn_isize Dqn_FmtLenNoNullTerminator (char const *fmt, ...);

// return: The length including 1 byte for the null terminating byte, i.e. "hi" = 3
DQN_API Dqn_isize Dqn_FmtVLen                (char const *fmt, va_list args);
DQN_API Dqn_isize Dqn_FmtLen                 (char const *fmt, ...);

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Log
// -------------------------------------------------------------------------------------------------
#define X_MACRO                                                                                                        \
    X_ENTRY(Debug,   "DBUG") \
    X_ENTRY(Error,   "ERR ") \
    X_ENTRY(Warning, "WARN") \
    X_ENTRY(Info,    "INFO") \
    X_ENTRY(Profile, "PROF") \
    X_ENTRY(Memory,  "MEM ")

enum struct Dqn_LogType
{
    #define X_ENTRY(enum_val, string) enum_val,
    X_MACRO
    #undef X_ENTRY
};

char const *Dqn_LogTypeString[] = {
    #define X_ENTRY(enum_val, string) string,
    X_MACRO
    #undef X_ENTRY
};
#undef X_MACRO

typedef void Dqn_LogProc(Dqn_LogType type, void *user_data, char const *file, Dqn_uint file_len, char const *func, Dqn_uint func_len, Dqn_uint line, char const *fmt, va_list va);

// ------------------------------------------------------------------------------------------------
// NOTE: Logging Macros
// ------------------------------------------------------------------------------------------------
// Macro logging functions, prefer this is you want to log messages
#define DQN_LOG_E(fmt, ...) Dqn_Log(Dqn_LogType::Error,   dqn__lib.log_user_data, DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_D(fmt, ...) Dqn_Log(Dqn_LogType::Debug,   dqn__lib.log_user_data, DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_W(fmt, ...) Dqn_Log(Dqn_LogType::Warning, dqn__lib.log_user_data, DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_I(fmt, ...) Dqn_Log(Dqn_LogType::Info,    dqn__lib.log_user_data, DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_M(fmt, ...) Dqn_Log(Dqn_LogType::Memory,  dqn__lib.log_user_data, DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_P(fmt, ...) Dqn_Log(Dqn_LogType::Profile, dqn__lib.log_user_data, DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG(log_type, fmt, ...) Dqn_Log(log_type,     dqn__lib.log_user_data, DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)

// Internal logging functions, prefer the logging macros above
DQN_API void Dqn_LogVDefault(Dqn_LogType type, void *user_data, char const *file, Dqn_uint file_len, char const *func, Dqn_uint func_len, Dqn_uint line, char const *fmt, va_list va);
DQN_API void Dqn_LogV       (Dqn_LogType type, void *user_data, char const *file, Dqn_uint file_len, char const *func, Dqn_uint func_len, Dqn_uint line, char const *fmt, va_list va);
DQN_API void Dqn_Log        (Dqn_LogType type, void *user_data, char const *file, Dqn_uint file_len, char const *func, Dqn_uint func_len, Dqn_uint line, char const *fmt, ...);

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Align
// -------------------------------------------------------------------------------------------------
// 'AlignAddressEnsuringSpace" aligns even if pointer is aligned, align it again, ensuring there's
// at minimum 1 byte and at most 'alignment' bytes of space between the aligned pointer and raw
// pointer. We do this to keep metadata exactly 1 byte behind the aligned pointer.
// 'AlignAddress' is the same as above except it's a no-op if the address is already aligned.
DQN_API Dqn_uintptr Dqn_AlignAddressEnsuringSpace(Dqn_uintptr address, Dqn_u8 alignment);
DQN_API Dqn_uintptr Dqn_AlignAddress             (Dqn_uintptr address, Dqn_u8 alignment);

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_AllocationTracer
// -------------------------------------------------------------------------------------------------
#if DQN_ALLOCATION_TRACING
    #define DQN_CALL_SITE(msg) , __FILE__, __func__, __LINE__, msg
    #define DQN_CALL_SITE_ARGS , char const *file_, char const *func_, int line_, char const *msg_
    #define DQN_CALL_SITE_ARGS_INPUT , file_, func_, line_, msg_
#else
    #define DQN_CALL_SITE(msg)
    #define DQN_CALL_SITE_ARGS
    #define DQN_CALL_SITE_ARGS_INPUT
#endif

struct Dqn_AllocationTrace
{
    void       *ptr;
    Dqn_usize   size;
    char const *file;
    char const *func;
    int         line;
    char const *msg;
};

struct Dqn_AllocationTracer
{
    // NOTE: Read only fields
    Dqn_TicketMutex              mutex;
    // Dqn_Map<Dqn_AllocationTrace> map;
};

void Dqn_AllocationTracerAdd   (Dqn_AllocationTracer *tracer, void *ptr, Dqn_usize size DQN_CALL_SITE_ARGS);
void Dqn_AllocationTracerRemove(Dqn_AllocationTracer *tracer, void *ptr);

#if defined(DQN_WITH_CRT_ALLOCATOR)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_CRTAllocator
// -------------------------------------------------------------------------------------------------
//
// CRT Style allocators that are for interfacing with foreign libraries that
// allow you to override malloc, realloc and free.
//
// Dqn_Arena is not designed to be used for replacing library
// allocation stubs that expect to use CRT style allocation, i.e. malloc and
// friends. This is by design, C libraries designed around that paradigm should
// not be shoe-horned into another allocation scheme as the library you're
// interfacing with has been designed with the liberal allocating and freeing
// style encouraged by the CRT.
//
#define DQN_CRT_ALLOCATOR_MALLOC(name) void *name(size_t size)
#define DQN_CRT_ALLOCATOR_REALLOC(name) void *name(void *ptr, size_t new_size)
#define DQN_CRT_ALLOCATOR_FREE(name) void name(void *ptr)
typedef DQN_CRT_ALLOCATOR_MALLOC(Dqn_CRTAllocatorMallocProc);
typedef DQN_CRT_ALLOCATOR_REALLOC(Dqn_CRTAllocatorReallocProc);
typedef DQN_CRT_ALLOCATOR_FREE(Dqn_CRTAllocatorFreeProc);
struct Dqn_CRTAllocator
{
    // NOTE: Configurable Fields: Set after zero initialization or initialization.
    Dqn_AllocationTracer         *tracer;  // (Optional) Initialize with Dqn_AllocationTracerInitWithMemory() to enable allocation tracing.
    Dqn_CRTAllocatorMallocProc  *malloc;  // (Optional) When nullptr, DQN_MALLOC is called
    Dqn_CRTAllocatorReallocProc *realloc; // (Optional) When nullptr, DQN_REALLOC is called
    Dqn_CRTAllocatorFreeProc    *free;    // (Optional) When nullptr, DQN_FREE is called

    // NOTE: Read Only Fields
    Dqn_u64 malloc_bytes;
    Dqn_u64 realloc_bytes;

    Dqn_u64 malloc_count;
    Dqn_u64 realloc_count;
    Dqn_u64 free_count;
};

DQN_API Dqn_CRTAllocator  Dqn_CRTAllocatorInitWithProcs(Dqn_CRTAllocatorMallocProc *allocate_proc, Dqn_CRTAllocatorReallocProc *realloc_proc, Dqn_CRTAllocatorFreeProc *free_proc);
DQN_API void              Dqn_CRTAllocatorFree         (Dqn_CRTAllocator *allocator, void *ptr);

#define                   Dqn_CRTAllocatorMalloc(allocator, size)       Dqn__CRTAllocatorMalloc(allocator, size, DQN_CALL_SITE(""))
#define                   Dqn_CRTAllocatorRealloc(allocator, ptr, size) Dqn__CRTAllocatorRealloc(allocator, ptr, size, DQN_CALL_SITE(""))

#define                   Dqn_CRTAllocatorTaggedMalloc(allocator, size, tag)       Dqn__CRTAllocatorMalloc(allocator, size, DQN_CALL_SITE(tag))
#define                   Dqn_CRTAllocatorTaggedRealloc(allocator, ptr, size, tag) Dqn__CRTAllocatorRealloc(allocator, ptr, size, DQN_CALL_SITE(tag))

DQN_API void             *Dqn__CRTAllocatorMalloc(Dqn_CRTAllocator *allocator, Dqn_usize size DQN_CALL_SITE_ARGS);
DQN_API void             *Dqn__CRTAllocatorRealloc(Dqn_CRTAllocator *allocator, void *ptr, Dqn_usize size DQN_CALL_SITE_ARGS);
#endif // DQN_WITH_CRT_ALLOCATOR

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Arena
// -------------------------------------------------------------------------------------------------
struct Dqn_ArenaMemBlock
{
    void              *memory;
    Dqn_isize          size;
    Dqn_isize          used;
    Dqn_ArenaMemBlock *prev;
    Dqn_ArenaMemBlock *next;
};

enum struct Dqn_ArenaMemProvider
{
    CRT,
    Virtual,
    UserMemory,
};

struct Dqn_ArenaStatsString
{
    char str[256];
    int  size;
};

struct Dqn_ArenaStats
{
    Dqn_isize bytes_allocated;
    Dqn_isize bytes_used;
    Dqn_isize bytes_wasted;
    Dqn_i32   block_count;
};

DQN_API Dqn_ArenaStatsString Dqn_ArenaStatsToString(Dqn_ArenaStats const *stats);

Dqn_usize const DQN_MEM_ARENA_DEFAULT_MIN_BLOCK_SIZE = DQN_KILOBYTES(4);
struct Dqn_Arena
{
    Dqn_ArenaMemProvider mem_provider;

    // NOTE: Read/Write
    Dqn_isize min_block_size; // (Optional): When 0, DQN_MEM_ARENA_DEFAULT_MIN_BLOCK_SIZE is used. Otherwise every new block will at minimum be sized to this value.

    // The following fields are should be set once after zero initialisation
    Dqn_AllocationTracer *tracer;

    // NOTE: Read Only
    Dqn_ArenaMemBlock *curr_mem_block;
    Dqn_ArenaMemBlock *top_mem_block;
    Dqn_ArenaStats     current_stats; // Current allocator stats, zero-ed when ResetUsage is called
    Dqn_ArenaStats     highest_stats; // Lifetime allocator stats, never zero-ed out
};

struct Dqn_ArenaScopeData
{
    Dqn_Arena *arena;
    Dqn_ArenaMemBlock *curr_mem_block;
    Dqn_ArenaMemBlock *top_mem_block;

    // NOTE: Fields to manually remember and restore once we end the scope
    Dqn_isize           curr_mem_block_used;
    Dqn_ArenaStats      current_stats;
};

// NOTE: Automatically start an undo region at the declaration of this variable
// and on scope exit, the arena will trigger the end of the undo region,
// reverting all allocations between the declaration of this variable and the
// end of it's scope.
struct Dqn_ArenaScope
{
    Dqn_ArenaScope(Dqn_Arena *arena);
    ~Dqn_ArenaScope();

    Dqn_Arena *arena; // For convenience
    Dqn_ArenaScopeData region;
};

// NOTE: Dqn_Arena can also be zero initialised and will default to the heap allocator with 0 size.
DQN_API Dqn_Arena Dqn_ArenaInitWithMemory(void *memory, Dqn_isize size);
DQN_API Dqn_Arena Dqn_ArenaInitWithCRT(Dqn_isize size DQN_CALL_SITE_ARGS);
DQN_API void      Dqn_ArenaFree(Dqn_Arena *arena);
DQN_API Dqn_b32   Dqn_ArenaReserve(Dqn_Arena *arena, Dqn_isize size DQN_CALL_SITE_ARGS);
DQN_API void      Dqn_ArenaResetUsage(Dqn_Arena *arena, Dqn_ZeroMem zero_mem);

// Allocations between a BeginScope and EndScope are reverted when EndScope is
// invoked. Calling BeginScope without an EndScope is well defined (incase the
// code decides it does not need to undo any allocations for whatever reason).
// Doing so ensures the Arena keeps the allocations that occured since
// BeginScope was called.
DQN_API Dqn_ArenaScopeData Dqn_ArenaBeginScope(Dqn_Arena *arena);
DQN_API void               Dqn_ArenaEndScope(Dqn_ArenaScopeData region);

#define Dqn_ArenaTaggedAllocate(arena, size, alignment, zero_mem, tag) Dqn__ArenaAllocate(arena, size, alignment, zero_mem DQN_CALL_SITE(tag))
#define Dqn_ArenaAllocate(arena, size, alignment, zero_mem) Dqn__ArenaAllocate(arena, size, alignment, zero_mem DQN_CALL_SITE(""))

#define Dqn_ArenaTaggedNew(arena, Type, zero_mem, tag) (Type *)Dqn__ArenaAllocate(arena, sizeof(Type), alignof(Type), zero_mem DQN_CALL_SITE(tag))
#define Dqn_ArenaNew(arena, Type, zero_mem) (Type *) Dqn__ArenaAllocate(arena, sizeof(Type), alignof(Type), zero_mem DQN_CALL_SITE(""))

#define Dqn_ArenaTaggedNewArray(arena, Type, count, zero_mem, tag) (Type *)Dqn__ArenaAllocate(arena, sizeof(Type) * count, alignof(Type), zero_mem DQN_CALL_SITE(tag))
#define Dqn_ArenaNewArray(arena, Type, count, zero_mem) (Type *)Dqn__ArenaAllocate(arena, sizeof(Type) * count, alignof(Type), zero_mem DQN_CALL_SITE(""))

#define Dqn_ArenaTaggedCopyNullTerminate(arena, Type, src, count, tag) (Type *)Dqn__ArenaCopyNullTerminate(arena, src, sizeof(*src) * count, alignof(Type) DQN_CALL_SITE(tag))
#define Dqn_ArenaCopyNullTerminate(arena, Type, src, count) (Type *)Dqn__ArenaCopyNullTerminate(arena, src, sizeof(*src) * count, alignof(Type) DQN_CALL_SITE(""))

#define Dqn_ArenaTaggedCopy(arena, Type, src, count, tag) (Type *)Dqn__ArenaCopy(arena, src, sizeof(*src) * count, alignof(Type) DQN_CALL_SITE(tag))
#define Dqn_ArenaCopy(arena, Type, src, count) (Type *)Dqn__ArenaCopy(arena, src, sizeof(*src) * count, alignof(Type) DQN_CALL_SITE(""))

DQN_API void *Dqn__ArenaCopy(Dqn_Arena *arena, void *src, Dqn_isize size, Dqn_u8 alignment DQN_CALL_SITE_ARGS);
DQN_API void *Dqn__ArenaCopyNullTerminate(Dqn_Arena *arena, void *src, Dqn_isize size, Dqn_u8 alignment DQN_CALL_SITE_ARGS);
DQN_API void *Dqn__ArenaAllocate(Dqn_Arena *arena, Dqn_isize size, Dqn_u8 alignment, Dqn_ZeroMem zero_mem DQN_CALL_SITE_ARGS);
DQN_API void  Dqn_ArenaLogStats(Dqn_Arena const *arena, char const *label);

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Map
// -------------------------------------------------------------------------------------------------
#if defined(DQN_WITH_MAP)
struct Dqn_Arena; // Foward declare

template <typename T>
struct Dqn_MapEntry
{
    Dqn_u64       hash;
    T             value;
    Dqn_MapEntry *next;
};

template <typename T>
struct Dqn_Map
{
    Dqn_Arena         *arena;
    Dqn_MapEntry<T>  **slots;
    Dqn_isize          size;  // The number of slots

    // NOTE: Sum count and chain_count for total items in the list.
    Dqn_isize          count;       // The total number of top-level slots in the 'values' list occupied
    Dqn_isize          chain_count; // The total number of chained elements in 'values'
    Dqn_MapEntry<T>   *free_list;
};

enum struct Dqn_MapCollideRule
{
    Overwrite,
    Chain,
    Fail,
};

template <typename T> Dqn_Map<T>       Dqn_MapInitWithArena(Dqn_Arena *arena, Dqn_isize size = 0);
template <typename T> Dqn_MapEntry<T> *Dqn_MapFindOrAdd(Dqn_Map<T> *map, Dqn_u64 hash, Dqn_MapCollideRule rule);
template <typename T> Dqn_MapEntry<T> *Dqn_MapAdd(Dqn_Map<T> *map, Dqn_u64 hash, T *value, Dqn_MapCollideRule rule);
template <typename T> Dqn_MapEntry<T> *Dqn_MapAddCopy(Dqn_Map<T> *map, Dqn_u64 hash, const T &value, Dqn_MapCollideRule rule);
template <typename T> Dqn_MapEntry<T> *Dqn_MapGet(Dqn_Map<T> *map, Dqn_u64 hash);
template <typename T> void             Dqn_MapErase(Dqn_Map<T> *map, Dqn_u64 hash, Dqn_ZeroMem zero_mem);
#endif // DQN_WITH_MAP

#if defined(DQN_WITH_DSMAP)
// Demitri Spanos (HMN) Hash Table
// 70% Max Load, PoT size, Linear Probing, Tombstoneless Deletes

#if !defined(DQN_DS_MAP_MIN_SIZE)
    #define DQN_DS_MAP_MIN_SIZE 4096
#endif

template <typename T>
struct Dqn_DSMapEntry
{
    Dqn_u64 hash;
    T       value;
    Dqn_u8  occupied;
};

template <typename T>
struct Dqn_DSMap
{
    Dqn_Arena  arena;
    Dqn_DSMapEntry<T>  *slots;
    Dqn_isize           size;  // The number of slots
    Dqn_isize           count; // The number of slots occupied in the list
};

// (Optional) DSMap can be zero initialised, it will default to a size of
// DQN_DS_MAP_MIN_SIZE elements, but if an initial size use the init function.
// size: A power of 2 size.
template <typename T> Dqn_DSMap<T>       Dqn_DSMapInit(Dqn_isize size);
template <typename T> void               Dqn_DSMapFree(Dqn_DSMap<T> *map);

template <typename T> Dqn_DSMapEntry<T> *Dqn_DSMapFind(Dqn_DSMap<T> *map, Dqn_u64 hash);
template <typename T> Dqn_DSMapEntry<T> *Dqn_DSMapFindOrAdd(Dqn_DSMap<T> *map, Dqn_u64 hash, Dqn_b32 find_only);
template <typename T> Dqn_DSMapEntry<T> *Dqn_DSMapAdd(Dqn_DSMap<T> *map, Dqn_u64 hash, T &value);
template <typename T> Dqn_DSMapEntry<T> *Dqn_DSMapAddCopy(Dqn_DSMap<T> *map, Dqn_u64 hash, T const &value);
template <typename T> Dqn_DSMapEntry<T> *Dqn_DSMapGet(Dqn_DSMap<T> *map, Dqn_u64 hash);
template <typename T> void               Dqn_DSMapErase(Dqn_DSMap<T> *map, Dqn_u64 hash, Dqn_ZeroMem zero_mem);
#endif // DQN_WITH_DSMAP

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Array
// -------------------------------------------------------------------------------------------------
template <typename T> struct Dqn_Array
{
    Dqn_Arena *arena;
    T                  *data;
    Dqn_isize           size;
    Dqn_isize           max;

    T const *begin() const { return data; }
    T const *end()   const { return data + size; }
    T *      begin()       { return data; }
    T *      end()         { return data + size; }
};

template <typename T> DQN_API Dqn_Array<T> Dqn_ArrayInitWithMemory(T *memory, Dqn_isize max, Dqn_isize size = 0);
#define                                    Dqn_ArrayInitWithArenaNoGrow(arena, Type, max, size, zero_mem) Dqn_Array_InitWithArenaNoGrow<Type>(arena, max, size, zero_mem DQN_CALL_SITE(""))

#define                                    Dqn_ArrayReserve(array, size) Dqn_Array_Reserve(array, size DQN_CALL_SITE(""))

#define                                    Dqn_ArrayAddArray(array, items, num) Dqn_Array_AddArray(array, items, num DQN_CALL_SITE(""))
#define                                    Dqn_ArrayAdd(array, item) Dqn_Array_Add(array, item DQN_CALL_SITE(""))
#define                                    Dqn_ArrayMake(array, num) Dqn_Array_Make(array, num DQN_CALL_SITE(""))
template <typename T> DQN_API void         Dqn_ArrayClear(Dqn_Array<T> *a, Dqn_ZeroMem zero_mem = Dqn_ZeroMem::No);

template <typename T> DQN_API void         Dqn_ArrayEraseStable(Dqn_Array<T> *a, Dqn_isize index);
template <typename T> DQN_API void         Dqn_ArrayEraseUnstable(Dqn_Array<T> *a, Dqn_isize index);

template <typename T> DQN_API void         Dqn_ArrayPop(Dqn_Array<T> *a, Dqn_isize num, Dqn_ZeroMem zero_mem = Dqn_ZeroMem::No);
template <typename T> DQN_API T *          Dqn_ArrayPeek(Dqn_Array<T> *a);

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_String
// -------------------------------------------------------------------------------------------------
#define DQN_STRING(string) Dqn_String{(char *)(string), (Dqn_isize)DQN_CHAR_COUNT(string), (Dqn_isize)DQN_CHAR_COUNT(string)}
#define DQN_STRINGW(string) Dqn_StringW{(wchar_t *)(string), (sizeof(string)/sizeof(string[0])) - 1}
#define DQN_STRING_FMT(string) (int)((string).size), (string).str
struct Dqn_String
{
    char      *str;
    Dqn_isize  size;
    Dqn_isize  cap;

    char const *begin() const { return str; }
    char const *end  () const { return str + size; }
    char       *begin()       { return str; }
    char       *end  ()       { return str + size; }
};

struct Dqn_StringW
{
    wchar_t   *str;
    Dqn_isize  size;

    wchar_t const *begin() const { return str; }
    wchar_t const *end  () const { return str + size; }
    wchar_t       *begin()       { return str; }
    wchar_t       *end  ()       { return str + size; }
};

// Make a string from a pre-existing string.
DQN_API Dqn_String Dqn_StringInit(char const *string, Dqn_isize size);
DQN_API Dqn_String Dqn_StringInitCString(char const *string);

// Make an empty string from a the buffer. 1 byte is reserved for the null-terminator
DQN_API Dqn_String Dqn_StringInitMemory(char *buf, Dqn_isize capacity);

// return: False if size is < 0 or the internal string is set to a nullptr
// otherwise true.
DQN_API Dqn_b32    Dqn_StringIsValid(Dqn_String in);

#define            Dqn_StringTaggedFmt(arena, tag, fmt, ...) Dqn__StringFmt(arena DQN_CALL_SITE(tag), fmt, ## __VA_ARGS__)
#define            Dqn_StringTaggedFmtV(arena, tag, fmt, ...) Dqn__StringFmtV(arena DQN_CALL_SITE(tag), fmt, ## __VA_ARGS__)

#define            Dqn_StringFmt(arena, fmt, ...) Dqn__StringFmt(arena DQN_CALL_SITE(""), fmt, ## __VA_ARGS__)
#define            Dqn_StringFmtV(arena, fmt, ...) Dqn__StringFmtV(arena DQN_CALL_SITE(""), fmt, ## __VA_ARGS__)

#define            Dqn_StringTaggedAllocate(arena, size, zero_mem, tag) Dqn_String_Allocate(arena, size, zero_mem DQN_CALL_SITE(tag))
#define            Dqn_StringTaggedCopy(src, arena, tag) Dqn_String_Copy(src, arena DQN_CALL_SITE(tag))
#define            Dqn_StringTaggedCopyCString(src, size, arena, tag) Dqn_String_CopyCString(src, size, arena DQN_CALL_SITE(tag))

#define            Dqn_StringAllocate(arena, size, zero_mem) Dqn_String_Allocate(arena, size, zero_mem DQN_CALL_SITE(""))
#define            Dqn_StringCopyCString(src, size, arena) Dqn_String_CopyCString(src, size, arena DQN_CALL_SITE(""))
#define            Dqn_StringCopy(src, arena) Dqn_String_Copy(src, arena DQN_CALL_SITE(""))

DQN_API Dqn_String Dqn__StringFmt(Dqn_Arena *arena DQN_CALL_SITE_ARGS, char const *fmt, ...);
DQN_API Dqn_String Dqn__StringFmtV(Dqn_Arena *arena, char const *fmt, va_list va DQN_CALL_SITE_ARGS);
DQN_API Dqn_String Dqn_String_Allocate(Dqn_Arena *arena, Dqn_isize size, Dqn_ZeroMem zero_mem);
DQN_API Dqn_String Dqn_String_CopyCString(char const *string, Dqn_isize size, Dqn_Arena *arena DQN_CALL_SITE_ARGS);
DQN_API Dqn_String Dqn_String_Copy(Dqn_String const src, Dqn_Arena *arena DQN_CALL_SITE_ARGS);

DQN_API Dqn_String Dqn_StringTrimWhitespaceAround(Dqn_String src);
DQN_API Dqn_b32    operator==(Dqn_String const &lhs, Dqn_String const &rhs);
DQN_API Dqn_b32    operator!=(Dqn_String const &lhs, Dqn_String const &rhs);

// Append to the string if there's enough capacity. No reallocation is permitted, fails if not enough space
DQN_API Dqn_b32    Dqn_StringAppendFmtV(Dqn_String *str, char const *fmt, va_list va);
DQN_API Dqn_b32    Dqn_StringAppendFmt(Dqn_String *str, char const *fmt, ...);

enum struct Dqn_StringEqCase
{
    Sensitive,
    Insensitive,
};

DQN_API Dqn_b32               Dqn_StringEq(Dqn_String const lhs, Dqn_String const rhs, Dqn_StringEqCase eq_case = Dqn_StringEqCase::Sensitive);
DQN_API Dqn_b32               Dqn_StringEqInsensitive(Dqn_String const lhs, Dqn_String const rhs);
DQN_API Dqn_b32               Dqn_StringStartsWith(Dqn_String string, Dqn_String prefix, Dqn_StringEqCase eq_case = Dqn_StringEqCase::Sensitive);
DQN_API Dqn_b32               Dqn_StringStartsWithInsensitive(Dqn_String string, Dqn_String prefix);
DQN_API Dqn_b32               Dqn_StringEndsWith(Dqn_String string, Dqn_String prefix, Dqn_StringEqCase eq_case = Dqn_StringEqCase::Sensitive);
DQN_API Dqn_b32               Dqn_StringEndsWithInsensitive(Dqn_String string, Dqn_String prefix);
DQN_API Dqn_Array<Dqn_String> Dqn_StringSplit(Dqn_String src, Dqn_Arena *arena);
DQN_API Dqn_String            Dqn_StringTrimPrefix(Dqn_String src, Dqn_String prefix, Dqn_StringEqCase eq_case = Dqn_StringEqCase::Sensitive);
DQN_API Dqn_String            Dqn_StringTrimSuffix(Dqn_String src, Dqn_String suffix, Dqn_StringEqCase eq_case = Dqn_StringEqCase::Sensitive);

// Trim UTF8 or UTF16 BOM i.e. 0xFFEF or 0xEFBBBF
// TODO(dqn): The trim assumes little endian architecture
DQN_API Dqn_String            Dqn_StringTrimByteOrderMark(Dqn_String src);
DQN_API Dqn_b32               Dqn_StringIsAllDigits(Dqn_String src);
DQN_API Dqn_b32               Dqn_StringIsAllHex(Dqn_String src);
DQN_API Dqn_b32               Dqn_StringContainsChar(Dqn_String src, char ch);

// Remove the substring denoted by the begin index and the size from the src
// string in-place using MEMMOVE to shift the string back.
DQN_API void                  Dqn_StringRemove(Dqn_String *in, Dqn_isize begin, Dqn_isize size);

// start_index: Set an index within the src string to start the search from, if not desired, set to 0
// return: The index of the matching find, -1 if it is not found
DQN_API Dqn_isize             Dqn_StringFindOffset(Dqn_String src, Dqn_String find, Dqn_isize start_index, Dqn_StringEqCase eq_case = Dqn_StringEqCase::Sensitive);

// start_index: Set an index within the src string to start the search from, if not desired, set to 0
// return: A string that points to the matching find, otherwise a 0 length string.
DQN_API Dqn_String            Dqn_StringFind(Dqn_String src, Dqn_String find, Dqn_isize start_index, Dqn_StringEqCase eq_case = Dqn_StringEqCase::Sensitive);

DQN_API Dqn_String            Dqn_StringReplace(Dqn_String src, Dqn_String find, Dqn_String replace, Dqn_isize start_index, Dqn_Arena *arena, Dqn_Arena *temp_arena, Dqn_StringEqCase eq_case = Dqn_StringEqCase::Sensitive);
DQN_API Dqn_String            Dqn_StringReplaceInsensitive(Dqn_String src, Dqn_String find, Dqn_String replace, Dqn_isize start_index, Dqn_Arena *arena, Dqn_Arena *temp_arena);

// Get the file name from a path by searching from the end of the string
// backwards to the first occuring path seperator '/' or '\'. If no path
// seperator is found, the original string is returned.
// path: Must point to a file path on the disk
// return: A string that is a slice of the path (i.e. range into the path string) representing the file name.
DQN_API Dqn_String            Dqn_StringFileNameFromPath(Dqn_String src, Dqn_String path);
DQN_API Dqn_u64               Dqn_StringToU64(Dqn_String str);
DQN_API Dqn_i64               Dqn_StringToI64(Dqn_String str);

// ------------------------------------------------------------------------------------------------
// NOTE: Dqn_Lib: Library book-keeping
// ------------------------------------------------------------------------------------------------
struct Dqn_Lib
{
    Dqn_LogProc       *LogCallback;
    void *             log_user_data;
    bool               log_no_output_file; // When true, stop the library from writing to a log file
    void *             log_file;           // TODO(dqn): Hmmm, how should we do this... ?
    Dqn_TicketMutex    log_file_mutex;     // Is locked when instantiating the log_file for the first time

#if defined(DQN_OS_WIN32)
    LARGE_INTEGER     win32_qpc_frequency;
    Dqn_TicketMutex   win32_bcrypt_rng_mutex;
    void             *win32_bcrypt_rng_handle;
#endif

#if defined(DQN_DEBUG_THREAD_CONTEXT)
    Dqn_TicketMutex thread_context_mutex;
    Dqn_ArenaStats  thread_context_arena_current_stats[256];
    Dqn_ArenaStats  thread_context_arena_highest_stats[256];
    Dqn_u8          thread_context_arena_stats_size;
#endif
};

extern Dqn_Lib dqn__lib;

// Update the default logging function, all logging functions will run through this callback
// proc: The new logging function, set to nullptr to revert back to the default logger.
// user_data: A user defined parameter to pass to the callback
DQN_API void Dqn_LibSetLogCallback(Dqn_LogProc *proc, void *user_data);

// file: Pass in nullptr to turn off writing logs to disk, otherwise point it to
// the FILE that you wish to write to.
DQN_API void Dqn_LibSetLogFile(void *file);
DQN_API void Dqn_LibDumpThreadContextArenaStats(Dqn_String file_path);

#if defined(DQN_WITH_FIXED_STRING)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_FixedString
// -------------------------------------------------------------------------------------------------
template <Dqn_isize MAX_>
struct Dqn_FixedString
{
    char      str[MAX_];
    Dqn_isize size;

    Dqn_b32 operator==(Dqn_FixedString const &other) const
    {
        if (size != other.size) return false;
        bool result = DQN_MEMCMP(data, other.data, size);
        return result;
    }

    Dqn_b32 operator!=(Dqn_FixedString const &other) const { return !(*this == other); }

    char const &operator[]   (Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i < size, "%d >= 0 && %d < %d", i, size); return data[i]; }
    char       &operator[]   (Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i < size, "%d >= 0 && %d < %d", i, size); return data[i]; }
    char const *begin        ()        const { return data; }
    char const *end          ()        const { return data + size; }
    char       *begin        ()              { return data; }
    char       *end          ()              { return data + size; }
};

template <Dqn_isize MAX_> DQN_API Dqn_FixedString<MAX_> Dqn_FixedStringFmt(char const *fmt, ...);
template <Dqn_isize MAX_> DQN_API Dqn_isize             Dqn_FixedStringMax(Dqn_FixedString<MAX_> *);
template <Dqn_isize MAX_> DQN_API void                  Dqn_FixedStringClear(Dqn_FixedString<MAX_> *str);
template <Dqn_isize MAX_> DQN_API Dqn_b32               Dqn_FixedStringAppendFmtV(Dqn_FixedString<MAX_> *str, char const *fmt, va_list va);
template <Dqn_isize MAX_> DQN_API Dqn_b32               Dqn_FixedStringAppendFmt(Dqn_FixedString<MAX_> *str, char const *fmt, ...);
template <Dqn_isize MAX_> DQN_API Dqn_b32               Dqn_FixedStringAppend(Dqn_FixedString<MAX_> *str, char const *src, Dqn_isize size = -1);
template <Dqn_isize MAX_> DQN_API Dqn_b32               Dqn_FixedStringAppend(Dqn_FixedString<MAX_> *str, Dqn_String src);
template <Dqn_isize MAX_> DQN_API Dqn_String            Dqn_FixedStringToString(Dqn_FixedString<MAX_> const *str);
#endif // DQN_WITH_FIXED_STRING

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_StringList
// -------------------------------------------------------------------------------------------------
struct Dqn_StringListNode
{
    Dqn_String          string;
    Dqn_StringListNode *next;
};

struct Dqn_StringList
{
    Dqn_isize           string_size;
    Dqn_StringListNode *head;
    Dqn_StringListNode *curr;
};

DQN_API Dqn_StringListNode *Dqn_StringListMakeNode(Dqn_Arena *arena, Dqn_isize size);
DQN_API void                Dqn_StringListAddNode(Dqn_StringList *list, Dqn_StringListNode *node);
DQN_API void                Dqn_StringListAppendFmtV(Dqn_StringList *list, Dqn_Arena *arena, char const *fmt, va_list args);
DQN_API void                Dqn_StringListAppendFmt(Dqn_StringList *list, Dqn_Arena *arena, char const *fmt, ...);

// Append a string to the list. The "Copy" variant will copy the string before
// appending and should be used if the string to be passed in has transient
// memory or will be further modified. The non "Copy" variant will avoid
// a memory allocation and just shallow copy the string into the node.
DQN_API void                Dqn_StringListAppendString(Dqn_StringList *list, Dqn_Arena *arena, Dqn_String string);
DQN_API void                Dqn_StringListAppendStringCopy(Dqn_StringList *list, Dqn_Arena *arena, Dqn_String string);
DQN_API void                Dqn_StringListAppendFmt(Dqn_StringList *list, Dqn_Arena *arena, char const *fmt, ...);
DQN_API Dqn_String          Dqn_StringListBuild(Dqn_StringList const *list, Dqn_Arena *arena);

#if defined(DQN_WITH_FIXED_ARRAY)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_FixedArray
// -------------------------------------------------------------------------------------------------
#define DQN_FIXED_ARRAY_TEMPLATE template <typename T, int MAX_>
#define DQN_FIXED_ARRAY_TEMPLATE_DECL Dqn_FixedArray<T, MAX_>
DQN_FIXED_ARRAY_TEMPLATE struct Dqn_FixedArray
{
    T         data[MAX_];
    Dqn_isize size;

    T       &operator[] (Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i <= size, "%I64d >= 0 && %I64d < %I64d", i, size); return data[i]; }
    T       *begin      ()                  { return data; }
    T       *end        ()                  { return data + size; }
    T       *operator+  (Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i <= size, "%I64d >= 0 && %I64d < %I64d", i, size); return data + i; }

    T const &operator[] (Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i <= size, "%I64d >= 0 && %I64d < %I64d", i, i, size); return data[i]; }
    T const *begin      ()            const { return data; }
    T const *end        ()            const { return data + size; }
    T const *operator+  (Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i <= size, "%I64d >= 0 && %I64d < %I64d", i, size); return data + i; }
};

DQN_FIXED_ARRAY_TEMPLATE DQN_API DQN_FIXED_ARRAY_TEMPLATE_DECL     Dqn_FixedArrayInit(T const *item, int num);
DQN_FIXED_ARRAY_TEMPLATE DQN_API Dqn_isize                         Dqn_FixedArrayMax(DQN_FIXED_ARRAY_TEMPLATE_DECL const *);

// Calculate the index of a item from the its pointer
DQN_FIXED_ARRAY_TEMPLATE DQN_API Dqn_isize                         Dqn_FixedArrayGetIndex(DQN_FIXED_ARRAY_TEMPLATE_DECL const *a, T const *entry);

// return: The newly added item, nullptr if failed
DQN_FIXED_ARRAY_TEMPLATE DQN_API T                                *Dqn_FixedArrayAdd(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T const *items, Dqn_isize num);
DQN_FIXED_ARRAY_TEMPLATE DQN_API T                                *Dqn_FixedArrayAdd(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T const &item);

// Bump the size of the array and return a pointer to 'num' uninitialised elements
DQN_FIXED_ARRAY_TEMPLATE DQN_API T                                *Dqn_FixedArrayMake(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize num);
DQN_FIXED_ARRAY_TEMPLATE DQN_API void                              Dqn_FixedArrayClear(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_ZeroMem zero_mem = Dqn_ZeroMem::No);
DQN_FIXED_ARRAY_TEMPLATE DQN_API void                              Dqn_FixedArrayEraseStable(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize index);
DQN_FIXED_ARRAY_TEMPLATE DQN_API void                              Dqn_FixedArrayEraseUnstable(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize index);
DQN_FIXED_ARRAY_TEMPLATE DQN_API void                              Dqn_FixedArrayPop(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize num = 1, Dqn_ZeroMem zero_mem = Dqn_ZeroMem::No);
DQN_FIXED_ARRAY_TEMPLATE DQN_API T                                *Dqn_FixedArrayPeek(DQN_FIXED_ARRAY_TEMPLATE_DECL *a);
DQN_FIXED_ARRAY_TEMPLATE DQN_API T                                 Dqn_FixedArrayPeekCopy(DQN_FIXED_ARRAY_TEMPLATE_DECL const *a);
template <typename T, int MAX_, typename IsEqual> DQN_API T       *Dqn_FixedArrayFind(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, IsEqual IsEqualProc);
template <typename T, int MAX_, typename IsEqual> DQN_API Dqn_b32  Dqn_FixedArrayFindElseMake(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T **entry, IsEqual IsEqualProc);
DQN_FIXED_ARRAY_TEMPLATE DQN_API T                                *Dqn_FixedArrayFind(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T *find);
#endif // DQN_WITH_FIXED_ARRAY

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_List - Chunked Linked Lists
// -------------------------------------------------------------------------------------------------
template <typename T>
struct Dqn_ListChunk
{
    T                *data;
    Dqn_isize         size;
    Dqn_isize         count;
    Dqn_ListChunk<T> *next;
    Dqn_ListChunk<T> *prev;
};

template <typename T>
struct Dqn_ListIterator
{
    Dqn_b32           init;             // (Internal): True if Dqn_ListIterate has been called at-least once on this iterator
    Dqn_ListChunk<T> *chunk;            // (Internal): The chunk that the iterator is reading from
    Dqn_isize         chunk_data_index; // (Internal): The index in the chunk the iterator is referencing
    T                *data;             // (Read):     Pointer to the data the iterator is referencing. Nullptr if invalid.
};

template <typename T>
struct Dqn_List
{
    Dqn_Arena        *arena;
    Dqn_isize         count;      // Cumulative count of all items made across all list chunks
    Dqn_isize         chunk_size; // When new ListChunk's are required, the minimum 'data' entries to allocate for that node.
    Dqn_ListChunk<T> *head;
    Dqn_ListChunk<T> *tail;
};

template <typename T> DQN_API Dqn_List<T>  Dqn_ListInitWithArena(Dqn_Arena *arena, Dqn_isize chunk_size = 128);

// Produce an iterator for the data in the list
/*
   Dqn_List<int> list = {};
   for (Dqn_ListIterator<int> it = {}; Dqn_ListIterate(&list, &it, 0);)
   {
       int *item = it.data;
   }
*/
// start_index: The index to start iterating from
template <typename T> DQN_API Dqn_b32  Dqn_ListIterate(Dqn_List<T> *list, Dqn_ListIterator<T> *iterator, Dqn_isize start_index);

// at_chunk: (Optional) The chunk that the index belongs to will be set in this parameter if given
// return: The element, or null pointer if it is not a valid index.
template <typename T> DQN_API T       *Dqn_ListAt(Dqn_List<T> *list, Dqn_isize index, Dqn_ListChunk<T> *at_chunk);

#define                                Dqn_ListTaggedMake(list, count, tag) Dqn__ListMake(list, count DQN_CALL_SITE(tag))
#define                                Dqn_ListMake(list, count) Dqn__ListMake(list, count DQN_CALL_SITE(""))
template <typename T> DQN_API T       *Dqn__ListMake(Dqn_List<T> *list, Dqn_isize count DQN_CALL_SITE_ARGS);

#if defined(DQN_WITH_MATH)
// -------------------------------------------------------------------------------------------------
// NOTE: Math
// -------------------------------------------------------------------------------------------------
struct Dqn_V2I
{
    Dqn_i32 x, y;

    Dqn_V2I() = default;
    Dqn_V2I(Dqn_f32 x_, Dqn_f32 y_): x((Dqn_i32)x_), y((Dqn_i32)y_) {}
    Dqn_V2I(Dqn_i32 x_, Dqn_i32 y_): x(x_), y(y_) {}
    Dqn_V2I(Dqn_i32 xy):             x(xy), y(xy) {}

    bool     operator!=(Dqn_V2I other) const { return !(*this == other);                }
    bool     operator==(Dqn_V2I other) const { return (x == other.x) && (y == other.y); }
    bool     operator>=(Dqn_V2I other) const { return (x >= other.x) && (y >= other.y); }
    bool     operator<=(Dqn_V2I other) const { return (x <= other.x) && (y <= other.y); }
    bool     operator< (Dqn_V2I other) const { return (x <  other.x) && (y <  other.y); }
    bool     operator> (Dqn_V2I other) const { return (x >  other.x) && (y >  other.y); }
    Dqn_V2I  operator- (Dqn_V2I other) const { Dqn_V2I result(x - other.x, y - other.y); return result; }
    Dqn_V2I  operator+ (Dqn_V2I other) const { Dqn_V2I result(x + other.x, y + other.y); return result; }
    Dqn_V2I  operator* (Dqn_V2I other) const { Dqn_V2I result(x * other.x, y * other.y); return result; }
    Dqn_V2I  operator* (Dqn_f32 other) const { Dqn_V2I result(x * other,   y * other);   return result; }
    Dqn_V2I  operator* (Dqn_i32 other) const { Dqn_V2I result(x * other,   y * other);   return result; }
    Dqn_V2I  operator/ (Dqn_V2I other) const { Dqn_V2I result(x / other.x, y / other.y); return result; }
    Dqn_V2I  operator/ (Dqn_f32 other) const { Dqn_V2I result(x / other,   y / other);   return result; }
    Dqn_V2I  operator/ (Dqn_i32 other) const { Dqn_V2I result(x / other,   y / other);   return result; }
    Dqn_V2I &operator*=(Dqn_V2I other)       { *this = *this * other;                    return *this;  }
    Dqn_V2I &operator*=(Dqn_f32 other)       { *this = *this * other;                    return *this;  }
    Dqn_V2I &operator*=(Dqn_i32 other)       { *this = *this * other;                    return *this;  }
    Dqn_V2I &operator-=(Dqn_V2I other)       { *this = *this - other;                    return *this;  }
    Dqn_V2I &operator+=(Dqn_V2I other)       { *this = *this + other;                    return *this;  }
};

struct Dqn_V2
{
    Dqn_f32 x, y;

    Dqn_V2() = default;
    Dqn_V2(Dqn_f32 a)           : x(a),           y(a)           {}
    Dqn_V2(Dqn_i32 a)           : x((Dqn_f32)a),  y((Dqn_f32)a)  {}
    Dqn_V2(Dqn_f32 x, Dqn_f32 y): x(x),           y(y)           {}
    Dqn_V2(Dqn_i32 x, Dqn_i32 y): x((Dqn_f32)x),  y((Dqn_f32)y)  {}
    Dqn_V2(Dqn_V2I a)           : x((Dqn_f32)a.x),y((Dqn_f32)a.y){}

    bool    operator!=(Dqn_V2  other) const { return !(*this == other);                }
    bool    operator==(Dqn_V2  other) const { return (x == other.x) && (y == other.y); }
    bool    operator>=(Dqn_V2  other) const { return (x >= other.x) && (y >= other.y); }
    bool    operator<=(Dqn_V2  other) const { return (x <= other.x) && (y <= other.y); }
    bool    operator< (Dqn_V2  other) const { return (x <  other.x) && (y <  other.y); }
    bool    operator> (Dqn_V2  other) const { return (x >  other.x) && (y >  other.y); }
    Dqn_V2  operator- (Dqn_V2  other) const { Dqn_V2 result(x - other.x, y - other.y); return result; }
    Dqn_V2  operator+ (Dqn_V2  other) const { Dqn_V2 result(x + other.x, y + other.y); return result; }
    Dqn_V2  operator* (Dqn_V2  other) const { Dqn_V2 result(x * other.x, y * other.y); return result; }
    Dqn_V2  operator* (Dqn_f32 other) const { Dqn_V2 result(x * other,   y * other);   return result; }
    Dqn_V2  operator* (Dqn_i32 other) const { Dqn_V2 result(x * other,   y * other);   return result; }
    Dqn_V2  operator/ (Dqn_V2  other) const { Dqn_V2 result(x / other.x, y / other.y); return result; }
    Dqn_V2  operator/ (Dqn_f32 other) const { Dqn_V2 result(x / other,   y / other);   return result; }
    Dqn_V2  operator/ (Dqn_i32 other) const { Dqn_V2 result(x / other,   y / other);   return result; }
    Dqn_V2 &operator*=(Dqn_V2  other)       { *this = *this * other;               return *this;  }
    Dqn_V2 &operator*=(Dqn_f32 other)       { *this = *this * other;               return *this;  }
    Dqn_V2 &operator*=(Dqn_i32 other)       { *this = *this * other;               return *this;  }
    Dqn_V2 &operator/=(Dqn_V2  other)       { *this = *this / other;               return *this;  }
    Dqn_V2 &operator/=(Dqn_f32 other)       { *this = *this / other;               return *this;  }
    Dqn_V2 &operator/=(Dqn_i32 other)       { *this = *this / other;               return *this;  }
    Dqn_V2 &operator-=(Dqn_V2  other)       { *this = *this - other;               return *this;  }
    Dqn_V2 &operator+=(Dqn_V2  other)       { *this = *this + other;               return *this;  }
};

struct Dqn_V3
{
    Dqn_f32 x, y, z;

    Dqn_V3() = default;
    Dqn_V3(Dqn_f32 a)                      : x(a),                  y(a),                  z(a)                  {}
    Dqn_V3(Dqn_i32 a)                      : x(DQN_CAST(Dqn_f32)a), y(DQN_CAST(Dqn_f32)a), z(DQN_CAST(Dqn_f32)a) {}
    Dqn_V3(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z): x(x),                  y(y),                  z(z)                  {}
    Dqn_V3(Dqn_i32 x, Dqn_i32 y, Dqn_f32 z): x(DQN_CAST(Dqn_f32)x), y(DQN_CAST(Dqn_f32)y), z(DQN_CAST(Dqn_f32)z) {}
    Dqn_V3(Dqn_V2  xy, Dqn_f32 z)          : x(xy.x),               y(xy.y),               z(z)                  {}

    bool   operator!= (Dqn_V3  other) const { return !(*this == other); }
    bool   operator== (Dqn_V3  other) const { return (x == other.x) && (y == other.y) && (z == other.z); }
    bool   operator>= (Dqn_V3  other) const { return (x >= other.x) && (y >= other.y) && (z >= other.z); }
    bool   operator<= (Dqn_V3  other) const { return (x <= other.x) && (y <= other.y) && (z <= other.z); }
    bool   operator<  (Dqn_V3  other) const { return (x <  other.x) && (y <  other.y) && (z <  other.z); }
    bool   operator>  (Dqn_V3  other) const { return (x >  other.x) && (y >  other.y) && (z >  other.z); }
    Dqn_V3 operator-  (Dqn_V3  other) const { Dqn_V3 result(x - other.x, y - other.y, z - other.z); return result; }
    Dqn_V3 operator+  (Dqn_V3  other) const { Dqn_V3 result(x + other.x, y + other.y, z + other.z); return result; }
    Dqn_V3 operator*  (Dqn_V3  other) const { Dqn_V3 result(x * other.x, y * other.y, z * other.z); return result; }
    Dqn_V3 operator*  (Dqn_f32 other) const { Dqn_V3 result(x * other,   y * other,   z * other);   return result; }
    Dqn_V3 operator*  (Dqn_i32 other) const { Dqn_V3 result(x * other,   y * other,   z * other);   return result; }
    Dqn_V3 operator/  (Dqn_V3  other) const { Dqn_V3 result(x / other.x, y / other.y, z / other.z); return result; }
    Dqn_V3 operator/  (Dqn_f32 other) const { Dqn_V3 result(x / other,   y / other,   z / other);   return result; }
    Dqn_V3 operator/  (Dqn_i32 other) const { Dqn_V3 result(x / other,   y / other,   z / other);   return result; }
    Dqn_V3 &operator*=(Dqn_V3  other)       { *this = *this * other; return *this;  }
    Dqn_V3 &operator*=(Dqn_f32 other)       { *this = *this * other; return *this;  }
    Dqn_V3 &operator*=(Dqn_i32 other)       { *this = *this * other; return *this;  }
    Dqn_V3 &operator/=(Dqn_V3  other)       { *this = *this / other; return *this;  }
    Dqn_V3 &operator/=(Dqn_f32 other)       { *this = *this / other; return *this;  }
    Dqn_V3 &operator/=(Dqn_i32 other)       { *this = *this / other; return *this;  }
    Dqn_V3 &operator-=(Dqn_V3  other)       { *this = *this - other; return *this;  }
    Dqn_V3 &operator+=(Dqn_V3  other)       { *this = *this + other; return *this;  }
};

union Dqn_V4
{
  struct { Dqn_f32 x, y, z, w; };
  struct { Dqn_f32 r, g, b, a; };
  struct { Dqn_V2 min; Dqn_V2 max; } v2;
  Dqn_V3 rgb;
  Dqn_f32 e[4];

  Dqn_V4() = default;
  Dqn_V4(Dqn_f32 xyzw)                              : x(xyzw),               y(xyzw),               z(xyzw),               w(xyzw) {}
  Dqn_V4(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z, Dqn_f32 w): x(x),                  y(y),                  z(z),                  w(w) {}
  Dqn_V4(Dqn_i32 x, Dqn_i32 y, Dqn_i32 z, Dqn_i32 w): x(DQN_CAST(Dqn_f32)x), y(DQN_CAST(Dqn_f32)y), z(DQN_CAST(Dqn_f32)z), w(DQN_CAST(Dqn_f32)w) {}
  Dqn_V4(Dqn_V3 xyz, Dqn_f32 w)                     : x(xyz.x),              y(xyz.y),              z(xyz.z),              w(w) {}
  Dqn_V4(Dqn_V2 v2)                                 : x(v2.x),               y(v2.y),               z(v2.x),               w(v2.y) {}

  bool    operator!=(Dqn_V4  other) const { return !(*this == other);            }
  bool    operator==(Dqn_V4  other) const { return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w); }
  bool    operator>=(Dqn_V4  other) const { return (x >= other.x) && (y >= other.y) && (z >= other.z) && (w >= other.w); }
  bool    operator<=(Dqn_V4  other) const { return (x <= other.x) && (y <= other.y) && (z <= other.z) && (w <= other.w); }
  bool    operator< (Dqn_V4  other) const { return (x <  other.x) && (y <  other.y) && (z <  other.z) && (w <  other.w); }
  bool    operator> (Dqn_V4  other) const { return (x >  other.x) && (y >  other.y) && (z >  other.z) && (w >  other.w); }
  Dqn_V4  operator- (Dqn_V4  other) const { Dqn_V4 result(x - other.x, y - other.y, z - other.z, w - other.w); return result;  }
  Dqn_V4  operator+ (Dqn_V4  other) const { Dqn_V4 result(x + other.x, y + other.y, z + other.z, w + other.w); return result;  }
  Dqn_V4  operator* (Dqn_V4  other) const { Dqn_V4 result(x * other.x, y * other.y, z * other.z, w * other.w); return result;  }
  Dqn_V4  operator* (Dqn_f32 other) const { Dqn_V4 result(x * other,   y * other,   z * other,   w * other);   return result;  }
  Dqn_V4  operator* (Dqn_i32 other) const { Dqn_V4 result(x * other,   y * other,   z * other,   w * other);   return result;  }
  Dqn_V4  operator/ (Dqn_f32 other) const { Dqn_V4 result(x / other,   y / other,   z / other,   w / other);   return result;  }
  Dqn_V4 &operator*=(Dqn_V4  other)       { *this = *this * other;                                             return *this;   }
  Dqn_V4 &operator*=(Dqn_f32 other)       { *this = *this * other;                                             return *this;   }
  Dqn_V4 &operator*=(Dqn_i32 other)       { *this = *this * other;                                             return *this;   }
  Dqn_V4 &operator-=(Dqn_V4  other)       { *this = *this - other;                                             return *this;   }
  Dqn_V4 &operator+=(Dqn_V4  other)       { *this = *this + other;                                             return *this;   }
};

// NOTE: Column major matrix
struct Dqn_M4
{
    Dqn_f32 columns[4][4];
};

DQN_API Dqn_V2I Dqn_V2ToV2I(Dqn_V2 a);
DQN_API Dqn_V2  Dqn_V2Min(Dqn_V2 a, Dqn_V2 b);
DQN_API Dqn_V2  Dqn_V2Max(Dqn_V2 a, Dqn_V2 b);
DQN_API Dqn_V2  Dqn_V2Abs(Dqn_V2 a);
DQN_API Dqn_f32 Dqn_V2Dot(Dqn_V2 a, Dqn_V2 b);
DQN_API Dqn_f32 Dqn_V2LengthSq(Dqn_V2 a, Dqn_V2 b);
DQN_API Dqn_V2  Dqn_V2Normalise(Dqn_V2 a);
DQN_API Dqn_V2  Dqn_V2Perpendicular(Dqn_V2 a);

DQN_API Dqn_f32 Dqn_V3LengthSq(Dqn_V3 a);
DQN_API Dqn_f32 Dqn_V3Length(Dqn_V3 a);
DQN_API Dqn_V3  Dqn_V3Normalise(Dqn_V3 a);

DQN_API Dqn_f32 Dqn_V4Dot(Dqn_V4 a, Dqn_V4 b);

DQN_API Dqn_M4  Dqn_M4Identity();
DQN_API Dqn_M4  Dqn_M4ScaleF(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z);
DQN_API Dqn_M4  Dqn_M4Scale(Dqn_V3 xyz);
DQN_API Dqn_M4  Dqn_M4TranslateF(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z);
DQN_API Dqn_M4  Dqn_M4Translate(Dqn_V3 xyz);
DQN_API Dqn_M4  Dqn_M4Transpose(Dqn_M4 mat);
DQN_API Dqn_M4  Dqn_M4Rotate(Dqn_V3 axis, Dqn_f32 radians);
DQN_API Dqn_M4  Dqn_M4Orthographic(Dqn_f32 left, Dqn_f32 right, Dqn_f32 bottom, Dqn_f32 top, Dqn_f32 z_near, Dqn_f32 z_far);
DQN_API Dqn_M4  Dqn_M4Perspective(Dqn_f32 fov /*radians*/, Dqn_f32 aspect, Dqn_f32 z_near, Dqn_f32 z_far);
DQN_API Dqn_M4  Dqn_M4Add(Dqn_M4 lhs, Dqn_M4 rhs);
DQN_API Dqn_M4  Dqn_M4Sub(Dqn_M4 lhs, Dqn_M4 rhs);
DQN_API Dqn_M4  Dqn_M4Mul(Dqn_M4 lhs, Dqn_M4 rhs);
DQN_API Dqn_M4  Dqn_M4Div(Dqn_M4 lhs, Dqn_M4 rhs);
DQN_API Dqn_M4  Dqn_M4AddF(Dqn_M4 lhs, Dqn_f32 rhs);
DQN_API Dqn_M4  Dqn_M4SubF(Dqn_M4 lhs, Dqn_f32 rhs);
DQN_API Dqn_M4  Dqn_M4MulF(Dqn_M4 lhs, Dqn_f32 rhs);
DQN_API Dqn_M4  Dqn_M4DivF(Dqn_M4 lhs, Dqn_f32 rhs);

#if defined(DQN_WITH_FIXED_STRING)
DQN_API Dqn_FixedString<256> Dqn_M4ColumnMajorString(Dqn_M4 mat);
#endif

struct Dqn_Rect
{
    Dqn_V2 min, max;
    Dqn_Rect() = default;
    Dqn_Rect(Dqn_V2  min, Dqn_V2 max)  : min(min), max(max) {}
    Dqn_Rect(Dqn_V2I min, Dqn_V2I max) : min(min), max(max) {}
    Dqn_Rect(Dqn_f32 x, Dqn_f32 y, Dqn_f32 max_x, Dqn_f32 max_y) : min(x, y), max(max_x, max_y) {}
    Dqn_b32 operator==(Dqn_Rect other) const { return (min == other.min) && (max == other.max); }
};

struct Dqn_RectI32
{
    Dqn_V2I min, max;
    Dqn_RectI32() = default;
    Dqn_RectI32(Dqn_V2I min, Dqn_V2I max) : min(min), max(max) {}
};

DQN_API Dqn_Rect Dqn_RectInitFromPosAndSize(Dqn_V2 pos, Dqn_V2 size);
DQN_API Dqn_V2   Dqn_RectCenter(Dqn_Rect rect);
DQN_API Dqn_b32  Dqn_RectContainsPoint(Dqn_Rect rect, Dqn_V2 p);
DQN_API Dqn_b32  Dqn_RectContainsRect(Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_V2   Dqn_RectSize(Dqn_Rect rect);
DQN_API Dqn_Rect Dqn_RectMove(Dqn_Rect src, Dqn_V2 move_amount);
DQN_API Dqn_Rect Dqn_RectMoveTo(Dqn_Rect src, Dqn_V2 dest);
DQN_API Dqn_b32  Dqn_RectIntersects(Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_Rect Dqn_RectIntersection(Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_Rect Dqn_RectUnion(Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_Rect Dqn_RectFromRectI32(Dqn_RectI32 a);
DQN_API Dqn_V2I  Dqn_RectI32_Size(Dqn_RectI32 rect);

// -------------------------------------------------------------------------------------------------
// NOTE: Math Utils
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_V2  Dqn_LerpV2(Dqn_V2 a, Dqn_f32 t, Dqn_V2 b);
DQN_API Dqn_f32 Dqn_LerpF32(Dqn_f32 a, Dqn_f32 t, Dqn_f32 b);
#endif // DQN_WITH_MATH

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Bit
// -------------------------------------------------------------------------------------------------
DQN_API void    Dqn_BitUnsetInplace(Dqn_u32 *flags, Dqn_u32 bitfield);
DQN_API void    Dqn_BitSetInplace(Dqn_u32 *flags, Dqn_u32 bitfield);
DQN_API Dqn_b32 Dqn_BitIsSet(Dqn_u32 bits, Dqn_u32 bits_to_set);
DQN_API Dqn_b32 Dqn_BitIsNotSet(Dqn_u32 bits, Dqn_u32 bits_to_check);

// -------------------------------------------------------------------------------------------------
// NOTE: Safe Arithmetic
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_i64   Dqn_SafeAddI64              (Dqn_i64 a, Dqn_i64 b);
DQN_API Dqn_i64   Dqn_SafeMulI64              (Dqn_i64 a, Dqn_i64 b);
DQN_API Dqn_u64   Dqn_SafeAddU64              (Dqn_u64 a, Dqn_u64 b);
DQN_API Dqn_u64   Dqn_SafeSubU64              (Dqn_u64 a, Dqn_u64 b);
DQN_API Dqn_u32   Dqn_SafeSubU32              (Dqn_u32 a, Dqn_u32 b);
DQN_API Dqn_u64   Dqn_SafeMulU64              (Dqn_u64 a, Dqn_u64 b);
DQN_API int       Dqn_SafeTruncateISizeToInt  (Dqn_isize val);
DQN_API Dqn_i32   Dqn_SafeTruncateISizeToI32  (Dqn_isize val);
DQN_API Dqn_i8    Dqn_SafeTruncateISizeToI8   (Dqn_isize val);
DQN_API Dqn_u32   Dqn_SafeTruncateUSizeToU32  (Dqn_usize val);
DQN_API int       Dqn_SafeTruncateUSizeToI32  (Dqn_usize val);
DQN_API int       Dqn_SafeTruncateUSizeToInt  (Dqn_usize val);
DQN_API Dqn_isize Dqn_SafeTruncateUSizeToISize(Dqn_usize val);
DQN_API Dqn_u32   Dqn_SafeTruncateU64ToU32    (Dqn_u64 val);
DQN_API Dqn_u16   Dqn_SafeTruncateU64ToU16    (Dqn_u64 val);
DQN_API Dqn_u8    Dqn_SafeTruncateU64ToU8     (Dqn_u64 val);
DQN_API Dqn_i64   Dqn_SafeTruncateU64ToI64    (Dqn_u64 val);
DQN_API Dqn_i32   Dqn_SafeTruncateU64ToI32    (Dqn_u64 val);
DQN_API Dqn_i16   Dqn_SafeTruncateU64ToI16    (Dqn_u64 val);
DQN_API Dqn_i8    Dqn_SafeTruncateU64ToI8     (Dqn_u64 val);
DQN_API int       Dqn_SafeTruncateU64ToInt    (Dqn_u64 val);

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Char
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_b32 Dqn_CharIsAlpha       (char ch);
DQN_API Dqn_b32 Dqn_CharIsDigit       (char ch);
DQN_API Dqn_b32 Dqn_CharIsAlphaNum    (char ch);
DQN_API Dqn_b32 Dqn_CharIsWhitespace  (char ch);
DQN_API Dqn_b32 Dqn_CharIsHex         (char ch);
DQN_API Dqn_u8  Dqn_CharHexToU8       (char ch);
DQN_API char    Dqn_CharToHex         (char ch);
DQN_API char    Dqn_CharToHexUnchecked(char ch);
DQN_API char    Dqn_CharToLower       (char ch);

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Char
// -------------------------------------------------------------------------------------------------
DQN_API int Dqn_UTF8EncodeCodepoint(Dqn_u8 utf8[4], Dqn_u32 codepoint);
DQN_API int Dqn_UTF16EncodeCodepoint(Dqn_u16 utf16[2], Dqn_u32 codepoint);

#if defined(DQN_WITH_HEX)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Hex
// -------------------------------------------------------------------------------------------------
DQN_API char const *Dqn_HexCStringTrimSpaceAnd0xPrefix(char const *hex, Dqn_isize size, Dqn_isize *real_size);
DQN_API Dqn_String  Dqn_HexStringTrimSpaceAnd0xPrefix (Dqn_String const string);

// Convert a hexadecimal string a 64 bit value. Asserts if the hex string is too
// big to be converted into a 64 bit number.
DQN_API Dqn_u64 Dqn_HexCStringToU64(char const *hex, Dqn_isize size);
DQN_API Dqn_u64 Dqn_HexStringToU64(Dqn_String hex);

// Convert a binary buffer into its hex representation into dest. The dest
// buffer must be large enough to contain the hex representation, i.e.
// atleast src_size * 2).
DQN_API void Dqn_HexBytesToHex(void const *src, int src_size, char *dest, int dest_size);

// Convert a hex buffer into its binary representation into dest. The dest
// buffer must be large enough to contain the binary representation, i.e.
// atleast ceil(hex_size / 2).
// hex_size: The size of the hex buffer. This function can handle an odd size
// hex string i.e. parsing "fff" works.
DQN_API void Dqn_HexToBytes(char const *hex, int hex_size, void *dest, int dest_size);

// Convert a series of bytes into a string. The CString variant returns a string
// with length (size * 2) with 1 extra byte for the null-terminator. The
// return: nullptr/invalid Dqn_String if the allocation failed, otherwise the hex string in ASCII.
DQN_API char       *Dqn_HexBytesToHexCStringArena(void const *bytes, Dqn_isize size, Dqn_Arena *arena);
DQN_API Dqn_String  Dqn_HexBytesToHexStringArena(void const *bytes, Dqn_isize size, Dqn_Arena *arena);
DQN_API char       *Dqn_HexU8ArrayToHexCStringArena(Dqn_Array<Dqn_u8> const array, Dqn_Arena *arena);
DQN_API Dqn_String  Dqn_HexU8ArrayToHexStringArena(Dqn_Array<Dqn_u8> const array, Dqn_Arena *arena);

// Convert a char string to a binary representation without any checks except assertions in debug.
// A leading "0x" is permitted and will be skipped as well as odd-sized strings.
DQN_API Dqn_u8            *Dqn_HexCStringToU8Bytes(char const *hex, Dqn_isize size, Dqn_isize *real_size, Dqn_Arena *arena);
DQN_API Dqn_Array<Dqn_u8>  Dqn_HexCStringToU8Array(char const *hex, Dqn_isize size, Dqn_Arena *arena);
DQN_API Dqn_Array<Dqn_u8>  Dqn_HexStringToU8Array(Dqn_String const hex, Dqn_Arena *arena);
#endif // DQN_WITH_HEX

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Str
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_b32     Dqn_CStringEquals(char const *a, char const *b, Dqn_isize a_size = -1, Dqn_isize b_size = -1);
DQN_API char const *Dqn_CStringFindMulti(char const *buf, char const *find_list[], Dqn_isize const *find_string_sizes, Dqn_isize find_size, Dqn_isize *match_index, Dqn_isize buf_size = -1);
DQN_API char const *Dqn_CStringFind(char const *buf, char const *find, Dqn_isize buf_size = -1, Dqn_isize find_size = -1, Dqn_b32 case_insensitive = false);
DQN_API char const *Dqn_CStringFileNameFromPath(char const *path, Dqn_isize size = -1, Dqn_isize *file_name_size = nullptr);
DQN_API Dqn_isize   Dqn_CStringSize(char const *a);
DQN_API Dqn_b32     Dqn_CStringMatch(char const *src, char const *find, int find_size);
DQN_API char const *Dqn_CStringSkipToChar(char const *src, char ch);
DQN_API char const *Dqn_CStringSkipToNextAlphaNum(char const *src);
DQN_API char const *Dqn_CStringSkipToNextDigit(char const *src);
DQN_API char const *Dqn_CStringSkipToNextChar(char const *src);
DQN_API char const *Dqn_CStringSkipToNextWord(char const *src);
DQN_API char const *Dqn_CStringSkipToNextWhitespace(char const *src);
DQN_API char const *Dqn_CStringSkipWhitespace(char const *src);
DQN_API char const *Dqn_CStringSkipToCharInPlace(char const **src, char ch);
DQN_API char const *Dqn_CStringSkipToNextAlphaNumInPlace(char const **src);
DQN_API char const *Dqn_CStringSkipToNextCharInPlace(char const **src);
DQN_API char const *Dqn_CStringSkipToNextWhitespaceInPlace(char const **src);
DQN_API char const *Dqn_CStringSkipToNextWordInPlace(char const **src);
DQN_API char const *Dqn_CStringSkipWhitespaceInPlace(char const **src);
DQN_API char const *Dqn_CStringTrimWhitespaceAround(char const *src, Dqn_isize size, Dqn_isize *new_size);
DQN_API char const *Dqn_CStringTrimPrefix(char const *src, Dqn_isize size, char const *prefix, Dqn_isize prefix_size, Dqn_isize *trimmed_size);
DQN_API Dqn_b32     Dqn_CStringIsAllDigits(char const *src, Dqn_isize size);

// separator: The separator between the thousand-th digits, i.e. separator = ',' converts '1,234' to '1234'.
DQN_API Dqn_u64     Dqn_CStringToU64(char const *buf, Dqn_isize size = -1, char separator = ',');
DQN_API Dqn_i64     Dqn_CStringToI64(char const *buf, Dqn_isize size = -1, char separator = ',');

DQN_API Dqn_isize   Dqn_LStringSize(wchar_t const *a);

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_File
// -------------------------------------------------------------------------------------------------
struct Dqn_FileInfo
{
    Dqn_u64 create_time_in_s;
    Dqn_u64 last_write_time_in_s;
    Dqn_u64 last_access_time_in_s;
    Dqn_u64 size;
    operator bool() const { return size > 0; }
};

// TODO(dqn): We should have a Dqn_String interface and a CString interface
DQN_API Dqn_b32      Dqn_FileExists(Dqn_String path);
DQN_API Dqn_b32      Dqn_FileDirExists(Dqn_String path);
DQN_API Dqn_FileInfo Dqn_FileGetInfo(Dqn_String path);
DQN_API Dqn_b32      Dqn_FileCopy(Dqn_String src, Dqn_String dest, Dqn_b32 overwrite);

// temp_arena: Only used on Linux otherwise ignored
DQN_API Dqn_b32      Dqn_FileMakeDir(Dqn_String path, Dqn_Arena *temp_arena);
DQN_API Dqn_b32      Dqn_FileMove(Dqn_String src, Dqn_String dest, Dqn_b32 overwrite);

// TODO(dqn): This doesn't work on directories unless you delete the files
// in that directory first.
DQN_API Dqn_b32      Dqn_FileDelete(Dqn_String path);

// file_size: (Optional) The size of the file in bytes, the allocated buffer is (file_size + 1 [null terminator]) in bytes.
// allocator: (Optional) When null, the buffer is allocated with DQN_MALLOC, result should be freed with DQN_FREE.
// return: nullptr if allocation failed.
#define             Dqn_FileArenaRead(file_path, file_path_size, file_size, arena) Dqn__FileArenaRead(file_path, file_path_size, file_size, arena DQN_CALL_SITE(""))
#define             Dqn_FileTaggedArenaReadFileToString(file_path, file_path_size, arena, tag) Dqn__FileArenaReadToString(file_path, file_path_size, arena DQN_CALL_SITE(tag))
#define             Dqn_FileArenaReadToString(file_path, file_path_size, arena) Dqn__FileArenaReadToString(file_path, file_path_size, arena DQN_CALL_SITE(""))
DQN_API Dqn_b32     Dqn_FileWriteFile(char const *file_path, Dqn_isize file_path_size, char const *buffer, Dqn_isize buffer_size);

DQN_API char       *Dqn__FileRead(char const *file_path, Dqn_isize file_path_size, Dqn_isize *file_size, Dqn_Arena *arena DQN_CALL_SITE_ARGS);
DQN_API char       *Dqn__FileArenaRead(char const *file_path, Dqn_isize file_path_size, Dqn_isize *file_size, Dqn_Arena *arena DQN_CALL_SITE_ARGS);
DQN_API Dqn_String  Dqn__FileArenaReadToString(char const *file, Dqn_isize file_path_size, Dqn_Arena *arena DQN_CALL_SITE_ARGS);

// -------------------------------------------------------------------------------------------------
// NOTE: Date
// -------------------------------------------------------------------------------------------------
struct Dqn_DateHMSTimeString
{
    char   date[DQN_ARRAY_COUNT("YYYY-MM-SS")];
    Dqn_i8 date_size;

    char   hms[DQN_ARRAY_COUNT("HH:MM:SS")];
    Dqn_i8 hms_size;
};

struct Dqn_DateHMSTime
{
    Dqn_i8 day;
    Dqn_i8 month;
    Dqn_i16 year;

    Dqn_i8 hour;
    Dqn_i8 minutes;
    Dqn_i8 seconds;
};

// @return The current time at the point of invocation
DQN_API Dqn_DateHMSTime       Dqn_DateHMSLocalTimeNow();
DQN_API Dqn_DateHMSTimeString Dqn_DateHMSLocalTimeStringNow(char date_separator = '-', char hms_separator = ':');

// return: The time elapsed since Unix epoch (1970-01-01T00:00:00Z) in seconds
DQN_API Dqn_u64 Dqn_DateEpochTime();

// -------------------------------------------------------------------------------------------------
// NOTE: OS
// -------------------------------------------------------------------------------------------------
// Generate cryptographically secure bytes
DQN_API Dqn_b32 Dqn_OSSecureRNGBytes(void *buffer, Dqn_u32 size);

// return: The directory without the trailing '/' or ('\' for windows). Empty
//         string with a nullptr if it fails.
DQN_API Dqn_String Dqn_OSExecutableDirectory(Dqn_Arena *arena);

// -------------------------------------------------------------------------------------------------
// NOTE: Utiltiies
// -------------------------------------------------------------------------------------------------
DQN_API void Dqn_SleepMs(Dqn_uint milliseconds);

DQN_API Dqn_u64 Dqn_PerfCounterNow   ();
DQN_API Dqn_f64 Dqn_PerfCounterS     (Dqn_u64 begin, Dqn_u64 end);
DQN_API Dqn_f64 Dqn_PerfCounterMs    (Dqn_u64 begin, Dqn_u64 end);
DQN_API Dqn_f64 Dqn_PerfCounterMicroS(Dqn_u64 begin, Dqn_u64 end);
DQN_API Dqn_f64 Dqn_PerfCounterNs    (Dqn_u64 begin, Dqn_u64 end);

struct Dqn_Timer // NOTE: Uses the PerfCounter API
{
    Dqn_u64 start;
    Dqn_u64 end;
};

DQN_API Dqn_Timer Dqn_TimerBegin ();
DQN_API void      Dqn_TimerEnd   (Dqn_Timer *timer);
DQN_API Dqn_f64   Dqn_TimerS     (Dqn_Timer timer);
DQN_API Dqn_f64   Dqn_TimerMs    (Dqn_Timer timer);
DQN_API Dqn_f64   Dqn_TimerMicroS(Dqn_Timer timer);
DQN_API Dqn_f64   Dqn_TimerNs    (Dqn_Timer timer);

struct Dqn_U64Str
{
    // Points to the start of the str in the buffer, not necessarily buf since
    // we write into the buffer in reverse
    char *str;
    char  buf[27]; // NOTE(dqn): 27 is the maximum size of Dqn_u64 including commas
    int   len;
};

// TODO(dqn): We need to rewrite this so that we just return a Dqn_U64Str which
// makes this a lot more ergonomic to use. Although the typical way we convert
// a number to a string is in reverse order, so I thought it'd be smart to write
// into the string buffer backwards and return a the char pointer to it, we
// should just write it in and memory copy it back so that it's at the start of
// the buffer. See Dqn_MoneyString for an example of this.
DQN_API char *Dqn_U64ToStr    (Dqn_u64 val, Dqn_U64Str *result, Dqn_b32 comma_sep);
DQN_API char *Dqn_U64ToTempStr(Dqn_u64 val, Dqn_b32 comma_sep = true);

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_ThreadContext
// -------------------------------------------------------------------------------------------------
// Utility functions for building applications by providing an in-built thread
// context that gives the user access to a temporary arena automatically without
// explicit setup on the end application's side.
#if !defined(DQN_THREAD_CONTEXT_ARENAS)
    #define DQN_THREAD_CONTEXT_ARENAS 2
#endif

struct Dqn_ThreadScratchData
{
    Dqn_Arena arena;
    Dqn_u8    arena_stats_index; // Index into Dqn_Lib's thread context arena stats array
};

struct Dqn_ThreadContext
{
    Dqn_b32 init;

    // Thread data that is handed out when the thread context is requested.
    Dqn_ThreadScratchData scratch_data[DQN_THREAD_CONTEXT_ARENAS];
};

struct Dqn_ThreadScratch
{
    Dqn_ThreadScratch(Dqn_ThreadScratchData *data)
    {
        destructed        = false;
        arena             = &data->arena;
        arena_stats_index = data->arena_stats_index;
        arena_scope       = Dqn_ArenaBeginScope(arena);
    }

    ~Dqn_ThreadScratch()
    {
    #if defined(DQN_DEBUG_THREAD_CONTEXT)
        dqn__lib.thread_context_arena_highest_stats[arena_stats_index] = arena->highest_stats;
        dqn__lib.thread_context_arena_current_stats[arena_stats_index] = arena->current_stats;
    #endif

        DQN_ASSERT(destructed == false);
        Dqn_ArenaEndScope(arena_scope);
        destructed = true;
    }

    Dqn_b32             destructed;
    Dqn_Arena          *arena;
    int                 arena_stats_index;
    Dqn_ArenaScopeData  arena_scope;
};

// A scratch thread context gives you a arena unique to the thread that is
// temporarily scoped to the block of code it is requested in. On scope exit,
// the arena will be freed. This is useful for very short-lived allocations.
// conflict_arena: Pass in any of the arenas currently being used in the
// function to ensure that the arena assigned to the new ThreadScratch
// will not overwrite allocations belonging to another scratch.
DQN_API Dqn_ThreadScratch Dqn_ThreadGetScratch(const Dqn_Arena *conflict_arena = nullptr);

// Get the current thread's context- this contains all the metadata for managing
// the thread scratch data. In general you probably want Dqn_ThreadGetScratch()
// which ensures you get a usable scratch arena for temporary allocations
// without having to worry about selecting the right arena from the state.
DQN_API Dqn_ThreadContext *Dqn_ThreadGetContext();

#if defined(DQN_WITH_JSON_WRITER)

#if !defined(DQN_WITH_FIXED_STRING)
    #error JsonWriter requires DQN_WITH_FIXED_STRING to be defined
#endif

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_JsonWriter
// -------------------------------------------------------------------------------------------------
// TODO(dqn): We need to write tests for this
struct Dqn_JsonWriter
{
    Dqn_u16         parent_field_count_stack[32];
    int             parent_field_count_stack_size;
    Dqn_Arena      *arena;
    Dqn_StringList  list;
    int             indent_level;
    int             spaces_per_indent;
};

DQN_API Dqn_JsonWriter Dqn_JsonWriterInit(Dqn_Arena *arena, int spaces_per_indent);
DQN_API Dqn_String     Dqn_JsonWriterBuild(Dqn_JsonWriter *writer, Dqn_Arena *arena);

DQN_API void           Dqn_JsonWriterBeginNamedObject(Dqn_JsonWriter *writer, Dqn_String name);
DQN_API void           Dqn_JsonWriterBeginObject(Dqn_JsonWriter *writer);
DQN_API void           Dqn_JsonWriterEndObject(Dqn_JsonWriter *writer);

DQN_API void           Dqn_JsonWriterBeginNamedArray(Dqn_JsonWriter *writer, Dqn_String name);
DQN_API void           Dqn_JsonWriterBeginArray(Dqn_JsonWriter *writer, Dqn_String name);
DQN_API void           Dqn_JsonWriterEndArray(Dqn_JsonWriter *writer);

DQN_API void           Dqn_JsonWriterNamedString(Dqn_JsonWriter *writer, Dqn_String key, Dqn_String value);
DQN_API void           Dqn_JsonWriterString(Dqn_JsonWriter *writer, Dqn_String value);

DQN_API void           Dqn_JsonWriterNamedU64(Dqn_JsonWriter *writer, Dqn_String key, Dqn_u64 value);
DQN_API void           Dqn_JsonWriterU64(Dqn_JsonWriter *writer, Dqn_u64 value);

// decimal_places: When < 0 show the maximum amount of decimal places
//                 When >=0 show the specified amount of decimal places
DQN_API void           Dqn_JsonWriterNamedF64(Dqn_JsonWriter *writer, Dqn_String key, Dqn_f64 value, int decimal_places = -1);
DQN_API void           Dqn_JsonWriterF64(Dqn_JsonWriter *writer, Dqn_f64 value, int decimal_places = -1);
#endif // DQN_WITH_JSON_WRITER

#if defined(DQN_OS_WIN32)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Win
// -------------------------------------------------------------------------------------------------
struct Dqn_WinErrorMsg
{
    unsigned long code;
    char          str[DQN_KILOBYTES(64) - 1]; // Maximum error size
    unsigned long size;
};
DQN_API Dqn_WinErrorMsg Dqn_WinLastError();

// Call once at application start-up to ensure that the application is DPI aware
// on Windows and ensure that application UI is scaled up appropriately for the
// monitor.
DQN_API void Dqn_WinMakeProcessDPIAware();

// Automatically dumps to DQN_LOG_E
#define Dqn_WinDumpLastError(fmt, ...) Dqn__WinDumpLastError(DQN_STRING(__FILE__), DQN_STRING(__func__), __LINE__, fmt, ##__VA_ARGS__)
DQN_API void Dqn__WinDumpLastError(Dqn_String file, Dqn_String function, Dqn_uint line, char const *fmt, ...);

// return: The size required not including the null-terminator
DQN_API int Dqn_WinUTF8ToWCharSizeRequired(Dqn_String src);

// Converts the UTF8 string into a wide string. This function always
// null-terminates the buffer. If you use the SizeRequired(...) function, this
// 'dest' buffer must be (SizeRequired + 1) in size since it returns a size not
// including the null-terminator, otherwise, this function will null-terminate
// the buffer deleting the last character.
// return: The size of the converted string not including the null terminator
DQN_API int         Dqn_WinUTF8ToWChar(Dqn_String src, wchar_t *dest, int dest_size);
DQN_API Dqn_StringW Dqn_WinArenaUTF8ToWChar(Dqn_String src, Dqn_Arena *arena);

// See: Dqn_WinUTF8ToWchar notes
DQN_API int         Dqn_WinWCharToUTF8SizeRequired(Dqn_StringW src);
DQN_API int         Dqn_WinWCharToUTF8(Dqn_StringW src, char *dest, int dest_size);
DQN_API Dqn_String  Dqn_WinArenaWCharToUTF8(Dqn_StringW src, Dqn_Arena *arena);

DQN_API Dqn_StringW Dqn_WinExecutableDirectoryW(Dqn_Arena *arena);

// size: (Optional) The size of the current directory string returned
// suffix: (Optional) A suffix to append to the current working directory
// suffix_size: (Optional) The size of the suffix to append
DQN_API Dqn_String  Dqn_WinCurrentDir(Dqn_Arena *arena, Dqn_Arena *temp_arena, Dqn_String suffix);
DQN_API Dqn_StringW Dqn_WinCurrentDirW(Dqn_Arena *arena, Dqn_StringW suffix);

struct Dqn_WinFolderIteratorW
{
    void       *handle;
    Dqn_StringW file_name;
    wchar_t     file_name_buf[512];
};

struct Dqn_WinFolderIterator
{
    void       *handle;
    Dqn_String file_name;
    char       file_name_buf[512];
};

DQN_API bool Dqn_WinFolderIterate(Dqn_String path, Dqn_WinFolderIterator *it);
DQN_API bool Dqn_WinFolderWIterate(Dqn_StringW path, Dqn_WinFolderIteratorW *it);

#if defined(DQN_WITH_WIN_NET)
enum struct Dqn_WinNetHandleState
{
    Invalid,
    Initialised,
    RequestFailed,
    RequestGood,
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
    int   url_size;

    // NOTE: docs.microsoft.com/en-us/windows/win32/wininet/setting-and-retrieving-internet-options#scope-of-hinternet-handle
    // These handles have three levels:
    //
    // The root HINTERNET handle (created by a call to InternetOpen) would contain all the Internet options that affect this instance of WinINet.
    // HINTERNET handles that connect to a server (created by a call to InternetConnect)
    // HINTERNET handles associated with a resource or enumeration of resources on a particular server.
    //
    // More detailed information about the HINTERNET dependency is listed here
    // NOTE: https://docs.microsoft.com/en-us/windows/win32/wininet/appendix-a-hinternet-handles
    void                 *internet_open_handle;
    void                 *internet_connect_handle;
    void                 *http_handle;
    Dqn_WinNetHandleState state;
};

// TODO(dqn): Useful options to expose in the handle
// https://docs.microsoft.com/en-us/windows/win32/wininet/option-flags
// INTERNET_OPTION_CONNECT_RETRIES -- default is 5 retries
// INTERNET_OPTION_CONNECT_TIMEOUT -- milliseconds
// INTERNET_OPTION_RECEIVE_TIMEOUT
// INTERNET_OPTION_SEND_TIMEOUT

// Initialise a new networking handle that is pointing to the specified URL.
// The URL string must be null-terminated because Windows is a C API and
// requires null-termination.
DQN_API Dqn_WinNetHandle Dqn_WinNetHandleInitCString(char const *url, int url_size);
DQN_API Dqn_WinNetHandle Dqn_WinNetHandleInitString(Dqn_String url);

DQN_API void             Dqn_WinNetHandleFinish(Dqn_WinNetHandle *handle);
DQN_API bool             Dqn_WinNetHandleIsValid(Dqn_WinNetHandle const *handle);
DQN_API void             Dqn_WinNetHandleSetUserAgentCString(Dqn_WinNetHandle *handle, char const *user_agent, int user_agent_size);
DQN_API bool             Dqn_WinNetHandlePump(Dqn_WinNetHandle *handle, char const *http_verb, char *post_data, int post_data_size, char *dest, int dest_size, size_t *download_size);
DQN_API char *           Dqn_WinNetHandlePumpToCString(Dqn_WinNetHandle *handle, char const *http_verb, char *post_data, int post_data_size, Dqn_Arena *arena, size_t *download_size);
DQN_API Dqn_String       Dqn_WinNetHandlePumpToString(Dqn_WinNetHandle *handle, char const *http_verb, char *post_data, int post_data_size, Dqn_Arena *arena);

DQN_API void             Dqn_WinNetHandlePumpToCRTFile(Dqn_WinNetHandle *handle, char const *http_verb, char *post_data, int post_data_size, FILE *file);
DQN_API char            *Dqn_WinNetHandlePumpToMallocCString(Dqn_WinNetHandle *handle, char const *http_verb, char *post_data, int post_data_size, size_t *download_size);
DQN_API Dqn_String       Dqn_WinNetHandlePumpToMallocString(Dqn_WinNetHandle *handle, char const *http_verb, char *post_data, int post_data_size);
#endif // DQN_WITH_WIN_NET
#endif // DQN_OS_WIN32

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_TimedBlock
// ------------------------------------------------------------------------------------------------
// TimedBlock provides a extremely primitive way of measuring the duration of
// code blocks, by sprinkling DQN_TIMED_BLOCK_RECORD("record label"), you can
// measure the time between the macro and the next record call.
//
// Example: Record the duration of the for-loop below and print it at the end.
/*
   int main()
   {
       DQN_TIMED_BLOCK_INIT("Profiling Region", 32); // name, records to allocate
       DQN_TIMED_BLOCK_RECORD("a");
       for (int unused1_ = 0; unused1_ < 1000000; unused1_++)
       {
           for (int unused2_ = 0; unused2_ < 1000000; unused2_++)
           {
               (void)unused1_;
               (void)unused2_;
           }
       }
       DQN_TIMED_BLOCK_RECORD("b");
       DQN_TIMED_BLOCK_DUMP;
       return 0;
   }
*/
struct Dqn_TimedBlock
{
    char const *label;
    Dqn_u64     tick;
};

// Initialise a timing block region,
#define DQN_TIMED_BLOCK_INIT(label, size)                                                                              \
    Dqn_TimedBlock timings_[size];                                                                                     \
    Dqn_usize      timings_size_ = 0;                                                                                  \
    DQN_TIMED_BLOCK_RECORD(label)

// Add a timing record at the current location this macro is invoked.
// DQN_TIMED_BLOCK_INIT must have been called in a scope visible to the macro
// prior.
// label: The label to give to the timing record
#define DQN_TIMED_BLOCK_RECORD(label) timings_[timings_size_++] = {label, Dqn_PerfCounterNow()}

// Dump the timing block via Dqn_Log
#define DQN_TIMED_BLOCK_DUMP                                                                                           \
    DQN_ASSERT_MSG(timings_size_ < sizeof(timings_) / sizeof(timings_[0]),                                             \
                   "Timings array indexed out-of-bounds, use a bigger size");                                          \
    for (int timings_index_ = 0; timings_index_ < (timings_size_ - 1); timings_index_++)                               \
    {                                                                                                                  \
        Dqn_TimedBlock t1 = timings_[timings_index_ + 0];                                                              \
        Dqn_TimedBlock t2 = timings_[timings_index_ + 1];                                                              \
        DQN_LOG_P("%s -> %s: %fms", t1.label, t2.label, Dqn_PerfCounterMs(t1.tick, t2.tick));                          \
    }                                                                                                                  \
                                                                                                                       \
    if (timings_size_ >= 1)                                                                                            \
    {                                                                                                                  \
        Dqn_TimedBlock t1 = timings_[0];                                                                               \
        Dqn_TimedBlock t2 = timings_[timings_size_ - 1];                                                               \
        DQN_LOG_P("%s -> %s (total): %fms", t1.label, t2.label, Dqn_PerfCounterMs(t1.tick, t2.tick));                  \
    }

// -------------------------------------------------------------------------------------------------
// NOTE: Hashing - Dqn_FNV1A[32|64]
// -------------------------------------------------------------------------------------------------
//
// Usage
//
// char buffer1[128] = {random bytes};
// char buffer2[128] = {random bytes};
// Dqn_u64 hash      = Dqn_FNV1A64Hash(buffer1, sizeof(buffer1));
// hash              = Dqn_FNV1A64Iterate(buffer2, sizeof(buffer2), hash); // subsequent hashing
//
#ifndef DQN_FNV1A32_SEED
    #define DQN_FNV1A32_SEED 2166136261U
#endif

#ifndef DQN_FNV1A64_SEED
    #define DQN_FNV1A64_SEED 14695981039346656037ULL
#endif

DQN_API Dqn_u32 Dqn_FNV1A32Hash       (void const *bytes, Dqn_isize size);
DQN_API Dqn_u64 Dqn_FNV1A64Hash       (void const *bytes, Dqn_isize size);
DQN_API Dqn_u32 Dqn_FNV1A32Iterate    (void const *bytes, Dqn_isize size, Dqn_u32 hash);
DQN_API Dqn_u64 Dqn_FNV1A64Iterate    (void const *bytes, Dqn_isize size, Dqn_u64 hash);

// -------------------------------------------------------------------------------------------------
// NOTE: Hashing - Dqn_MurmurHash3
// -------------------------------------------------------------------------------------------------
//
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author (Austin Appleby) hereby disclaims copyright to this source
// code.
//
// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.

struct Dqn_MurmurHash3128 { Dqn_u64 e[2]; };

DQN_API Dqn_u32            Dqn_MurmurHash3x86_32 (void const *key, int len, Dqn_u32 seed);
DQN_API Dqn_MurmurHash3128 Dqn_MurmurHash3x64_128(void const *key, int len, Dqn_u32 seed);
#define DQN_MURMUR_HASH3_U128_AS_U64(key, len, seed) (Dqn_MurmurHash3x64_128(key, len, seed).e[0])

// -------------------------------------------------------------------------------------------------
// NOTE: Template Implementation
// -------------------------------------------------------------------------------------------------

#if defined(DQN_WITH_MAP)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Map Template Implementation
// -------------------------------------------------------------------------------------------------
template <typename T>
Dqn_Map<T> Dqn_MapInitWithArena(Dqn_Arena *arena, Dqn_isize size)
{
    Dqn_Map<T> result = {};
    result.arena      = arena;

    Dqn_isize final_size          = size == 0 ? 4096 : size;
    result.slots                  = Dqn_ArenaNewArray(arena, Dqn_MapEntry<T> *, final_size, Dqn_ZeroMem::Yes);
    if (result.slots) result.size = final_size;
    return result;
}

template <typename T>
Dqn_MapEntry<T> *Dqn_MapFindOrAdd(Dqn_Map<T> *map, Dqn_u64 hash, Dqn_MapCollideRule rule)
{
    Dqn_isize         index = hash % map->size;
    Dqn_MapEntry<T> *result = map->slots[index];

    if (result)
    {
        if (rule == Dqn_MapCollideRule::Chain)
        {
            while (result->hash != hash)
            {
                if (result->next)
                    result = result->next;
                else
                {
                    map->chain_count++;
                    result->next = Dqn_ArenaNew(map->arena, Dqn_MapEntry<T>, Dqn_ZeroMem::Yes);
                    result       = result->next;
                    break;
                }
            }
        }
        else if (rule == Dqn_MapCollideRule::Fail)
        {
            result = nullptr;
        }
    }
    else
    {
        result = Dqn_ArenaNew(map->arena, Dqn_MapEntry<T>, Dqn_ZeroMem::Yes);
        map->count++;
        map->slots[index] = result;
    }

    if (result)
        result->hash = hash;

    return result;
}

template <typename T>
Dqn_MapEntry<T> *Dqn_MapAdd(Dqn_Map<T> *map, Dqn_u64 hash, T &value, Dqn_MapCollideRule rule)
{
    Dqn_MapEntry<T> *result = Dqn_MapFindOrAdd(map, hash, rule);
    if (result)
        result->value = value;

    return result;
}

template <typename T>
Dqn_MapEntry<T> *Dqn_MapAddCopy(Dqn_Map<T> *map, Dqn_u64 hash, T const &value, Dqn_MapCollideRule rule)
{
    Dqn_MapEntry<T> *result = Dqn_MapFindOrAdd(map, hash, rule);
    if (result)
        result->value = value;

    return result;
}

template <typename T>
Dqn_MapEntry<T> *Dqn_MapGet(Dqn_Map<T> *map, Dqn_u64 hash)
{
    Dqn_isize         index  = hash % map->size;
    Dqn_MapEntry<T> *result = nullptr;

    for (Dqn_MapEntry<T> *entry = map->slots[index]; entry; entry = entry->next)
    {
        if (entry->hash == hash)
        {
            result = entry;
            break;
        }
    }

    return result;
}

template <typename T>
void Dqn_MapErase(Dqn_Map<T> *map, Dqn_u64 hash, Dqn_ZeroMem zero_mem)
{
    Dqn_isize         index = hash % map->size;
    Dqn_MapEntry<T> **entry = &(map->slots[index]);
    Dqn_b32 is_chain_entry = *entry && (*entry)->next;

    while ((*entry) && (*entry)->hash != hash)
        entry = &((*entry)->next);

    if ((*entry) && (*entry)->hash == hash)
    {
        Dqn_MapEntry<T> *erase_entry = *entry;
        Dqn_MapEntry<T> *next        = erase_entry->next;
        (*entry)                      = next;

        Dqn__ZeroMemBytes(erase_entry, sizeof(*erase_entry), zero_mem);
        erase_entry->next = map->free_list;
        map->free_list    = erase_entry;

        if (is_chain_entry) map->chain_count--;
        else map->count--;
    }
}
#endif // DQN_WITH_MAP

#if defined(DQN_WITH_DSMAP)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_DSMap Template Implementation
// -------------------------------------------------------------------------------------------------
template <typename T>
Dqn_DSMap<T> Dqn_DSMapInit(Dqn_isize size)
{
    DQN_ASSERT_MSG(((size & (size - 1)) == 0), "Require non-zero power of 2 table size");
    Dqn_DSMap<T> result = {};
    result.slots       = Dqn_ArenaNewArray(&result.arena, Dqn_DSMapEntry<T>, size, Dqn_ZeroMem::Yes);
    if (result.slots) result.size = size;
    return result;
}

template <typename T>
void Dqn_DSMapFree(Dqn_DSMap<T> *map)
{
    Dqn_ArenaFree(&map->arena);
    *map = {};
}

template <typename T>
Dqn_DSMapEntry<T> *Dqn_DSMapFind(Dqn_DSMap<T> *map, Dqn_u64 hash)
{
    Dqn_DSMapEntry<T> *result = Dqn_DSMapFindOrAdd(map, hash, true /*find_only*/);
    return result;
}

template <typename T>
Dqn_DSMapEntry<T> *Dqn_DSMapFindOrAdd(Dqn_DSMap<T> *map, Dqn_u64 hash, Dqn_b32 find_only)
{
    if (!map->slots)
    {
        if (find_only)
            return nullptr;
        else
            *map = Dqn_DSMapInit<T>(DQN_DS_MAP_MIN_SIZE);
    }

    Dqn_isize          index  = hash % map->size;
    Dqn_DSMapEntry<T> *result = map->slots + index;
    while (result->occupied && result->hash != hash)
    {
        index  = (index + 1) % map->size;
        result = map->slots + index;
    }

    if (result->occupied)
    {
        DQN_ASSERT_MSG(result->hash == hash,
                       "We have a max load factor of 70%% so we should never get an occupied slot that doesn't match "
                       "the hash we were searching for");
    }
    else if (find_only)
    {
        result = nullptr;
    }
    else
    {
        result->hash     = hash;
        result->occupied = true;

        Dqn_f32 load_factor = ++map->count / DQN_CAST(Dqn_f32)map->size;
        if (load_factor >= 0.7f)
        {
            auto new_map = Dqn_DSMapInit<T>(map->size << 1);
            for (Dqn_isize map_index = 0; map_index < map->size; map_index++)
            {
                Dqn_DSMapEntry<T> *entry = map->slots + map_index;
                if (entry->occupied)
                {
                    Dqn_DSMapEntry<T> *new_entry = Dqn_DSMapAddCopy(&new_map, entry->hash, entry->value);
                    if (new_entry->hash == hash)
                        result = new_entry;
                }
            }

            Dqn_DSMapFree(map);
            *map = new_map;
        }
    }

    return result;
}

template <typename T>
Dqn_DSMapEntry<T> *Dqn_DSMapAdd(Dqn_DSMap<T> *map, Dqn_u64 hash, T &value)
{
    Dqn_DSMapEntry<T> *result = Dqn_DSMapFindOrAdd(map, hash, false /*find_only*/);
    if (result)
        result->value = value;

    return result;
}

template <typename T>
Dqn_DSMapEntry<T> *Dqn_DSMapAddCopy(Dqn_DSMap<T> *map, Dqn_u64 hash, T const &value)
{
    Dqn_DSMapEntry<T> *result = Dqn_DSMapFindOrAdd(map, hash, false /*find_only*/);
    if (result)
        result->value = value;

    return result;
}

template <typename T>
Dqn_DSMapEntry<T> *Dqn_DSMapGet(Dqn_DSMap<T> *map, Dqn_u64 hash)
{
    Dqn_DSMapEntry<T> *result = Dqn_DSMapFindOrAdd(map, hash, true /*find_only*/);
    return result;
}

template <typename T>
void Dqn_DSMapErase(Dqn_DSMap<T> *map, Dqn_u64 hash, Dqn_ZeroMem zero_mem)
{
    Dqn_isize          index  = hash % map->size;
    Dqn_DSMapEntry<T> *result = map->slots + index;
    if (!result || !result->occupied)
        return;

    Dqn_isize start_index = index;
    Dqn_isize probe_index = index;
    for (;;)
    {
        probe_index               = (probe_index + 1) % map->size;
        Dqn_DSMapEntry<T> *probe  = map->slots + probe_index;
        if (!probe->occupied) break;

        Dqn_isize desired_index = probe->hash % map->size;
        if (desired_index != probe_index)
        {
            map->slots[start_index] = map->slots[probe_index];
            start_index              = probe_index;
            DQN_ASSERT(map->slots[start_index].occupied);
            DQN_ASSERT(map->slots[probe_index].occupied);
        }
    }

    DQN_ASSERT(map->slots[start_index].occupied);
    map->slots[start_index].occupied = false;
    map->count -= 1;

    Dqn__ZeroMemBytes(map->slots + start_index,
                      sizeof(map->slots[start_index]),
                      zero_mem);
}
#endif // DQN_WITH_DSMAP

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_FixedString Template Implementation
// -------------------------------------------------------------------------------------------------
#if defined(DQN_WITH_FIXED_STRING)
template <Dqn_isize MAX_>
DQN_API Dqn_FixedString<MAX_> Dqn_FixedStringFmt(char const *fmt, ...)
{
    Dqn_FixedString<MAX_> result = {};
    va_list va;
    va_start(va, fmt);
    Dqn_FixedStringAppendFmtV(&result, fmt, va);
    va_end(va);
    return result;
}

template <Dqn_isize MAX_>
DQN_API Dqn_isize Dqn_FixedStringMax(Dqn_FixedString<MAX_> *)
{
    Dqn_isize result = MAX_;
    return result;
}

template <Dqn_isize MAX_>
DQN_API void Dqn_FixedStringClear(Dqn_FixedString<MAX_> *str) { *str = {}; }

template <Dqn_isize MAX_>
DQN_API Dqn_b32 Dqn_FixedStringAppendFmtV(Dqn_FixedString<MAX_> *str, char const *fmt, va_list va)
{
    va_list va2;
    va_copy(va2, va);
    Dqn_isize require = STB_SPRINTF_DECORATE(vsnprintf)(nullptr, 0, fmt, va) + 1;
    Dqn_isize space   = MAX_ - str->size;
    Dqn_b32 result    = require <= space;

    if (!result)
    {
        DQN_LOG_W("Insufficient space in string: require=%I64d, space=%I64d", require, space);
        return result;
    }

    str->size += STB_SPRINTF_DECORATE(vsnprintf)(str->str + str->size, static_cast<int>(space), fmt, va2);
    va_end(va2);
    return result;
}

template <Dqn_isize MAX_>
DQN_API Dqn_b32 Dqn_FixedStringAppendFmt(Dqn_FixedString<MAX_> *str, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Dqn_b32 result = Dqn_FixedStringAppendFmtV(str, fmt, va);
    va_end(va);
    return result;
}

template <Dqn_isize MAX_>
DQN_API Dqn_b32 Dqn_FixedStringAppend(Dqn_FixedString<MAX_> *str, char const *src, Dqn_isize size)
{
    if (size == -1) size = DQN_CAST(Dqn_isize)Dqn_CStringSize(src);
    Dqn_isize space = (MAX_ - 1 /*reserve byte for null terminator*/) - str->size;
    Dqn_b32 result = size <= space;

    if (!result)
    {
        DQN_LOG_W("Insufficient space in string: size=%I64d, space=%I64d", size, space);
        return result;
    }

    DQN_MEMCOPY(str->str + str->size, src, size);
    str->size += size;
    str->str[str->size] = 0;
    return result;
}

template <Dqn_isize MAX_>
DQN_API Dqn_b32 Dqn_FixedStringAppend(Dqn_FixedString<MAX_> *str, Dqn_String src)
{
    Dqn_b32 result = Dqn_FixedStringAppend(str, src.str, src.size);
    return result;
}

template <Dqn_isize MAX_>
DQN_API Dqn_String Dqn_FixedStringToString(Dqn_FixedString<MAX_> const *str)
{
    auto result = Dqn_StringInit(str->str, str->size);
    return result;
}
#endif // DQN_WITH_FIXED_STRING

template <typename T> void Dqn__EraseStableFromCArray(T *array, Dqn_isize size, Dqn_isize max, Dqn_isize index)
{
    DQN_ASSERT(index >= 0 && index < size);
    DQN_ASSERT(size <= max); (void)max;
    Dqn_isize next_index    = DQN_MIN(index + 1, size);
    Dqn_usize bytes_to_copy = (size - next_index) * sizeof(T);
    memmove(array + index, array + next_index, bytes_to_copy);
}

#if defined(DQN_WITH_FIXED_ARRAY)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_FixedArray Template Implementation
// -------------------------------------------------------------------------------------------------
DQN_FIXED_ARRAY_TEMPLATE
DQN_API DQN_FIXED_ARRAY_TEMPLATE_DECL Dqn_FixedArrayInit(T const *item, int num)
{
    DQN_FIXED_ARRAY_TEMPLATE_DECL result = {};
    Dqn_FixedArrayAdd(&result, item, num);
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_API Dqn_isize Dqn_FixedArrayMax(DQN_FIXED_ARRAY_TEMPLATE_DECL const *)
{
    Dqn_isize result = MAX_;
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_API Dqn_isize Dqn_FixedArrayGetIndex(DQN_FIXED_ARRAY_TEMPLATE_DECL const *a, T const *entry)
{
    DQN_ASSERT(entry >= a->begin() && entry <= a->end());
    Dqn_isize result = a->end() - entry;
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_API T *Dqn_FixedArrayAdd(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T const *items, Dqn_isize num)
{
    DQN_ASSERT(a->size + num <= MAX_);
    T *result = static_cast<T *>(DQN_MEMCOPY(a->data + a->size, items, sizeof(T) * num));
    a->size += num;
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_API T *Dqn_FixedArrayAdd(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T const &item)
{
    DQN_ASSERT(a->size < MAX_);
    a->data[a->size++] = item;
    return &a->data[a->size - 1];
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_API T *Dqn_FixedArrayMake(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize num)
{
    DQN_ASSERT(a->size + num <= MAX_);
    T *result = a->data + a->size;
    a->size += num;
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_API void Dqn_FixedArrayClear(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_ZeroMem zero_mem)
{
    a->size = 0;
    Dqn__ZeroMemBytes(a->data, sizeof(T) * MAX_, zero_mem);
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_API void Dqn_FixedArrayEraseStable(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize index)
{
    Dqn__EraseStableFromCArray<T>(a->data, a->size--, MAX_, index);
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_API void Dqn_FixedArrayEraseUnstable(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize index)
{
    DQN_ASSERT(index >= 0 && index < a->size);
    if (--a->size == 0) return;
    a->data[index] = a->data[a->size];
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_API void Dqn_FixedArrayPop(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize num, Dqn_ZeroMem zero_mem)
{
    DQN_ASSERT(a->size - num >= 0);
    a->size -= num;
    void *    begin = a->data + a->size;
    void *    end   = a->data + (a->size + num);
    Dqn_isize bytes = DQN_CAST(Dqn_isize) end - DQN_CAST(Dqn_isize) begin;
    Dqn__ZeroMemBytes(begin, bytes, zero_mem);
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_API T *Dqn_FixedArrayPeek(DQN_FIXED_ARRAY_TEMPLATE_DECL *a)
{
    T *result = (a->size == 0) ? nullptr : a->data + (a->size - 1);
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_API T Dqn_FixedArrayPeekCopy(DQN_FIXED_ARRAY_TEMPLATE_DECL const *a)
{
    DQN_ASSERT(a->size > 0);
    T const *result = a->data + (a->size - 1);
    return *result;
}

template <typename T, int MAX_, typename IsEqual>
DQN_API T *Dqn_FixedArrayFind(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, IsEqual IsEqualProc)
{
    for (T &entry : (*a))
    {
        if (IsEqualProc(entry))
            return &entry;
    }
    return nullptr;
}

// return: True if the entry was found, false if not- the entry is made using Dqn_FixedArrayMake() in this case
template <typename T, int MAX_, typename IsEqual>
DQN_API Dqn_b32 Dqn_FixedArrayFindElseMake(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T **entry, IsEqual IsEqualProc)
{
    Dqn_b32 result = true;
    T *search  = Dqn_FixedArrayFind(a, IsEqualProc);
    if (!search)
    {
        result = false;
        search = Dqn_FixedArrayMake(a, 1);
    }

    *entry = search;
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_API T *Dqn_FixedArrayFind(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T *find)
{
    for (T &entry : (*a))
    {
        if (*find == entry)
            return entry;
    }
    return nullptr;
}
#endif // DQN_WITH_FIXED_ARRAY

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Array Template Implementation
// -------------------------------------------------------------------------------------------------
template <typename T>
DQN_API Dqn_Array<T> Dqn_ArrayInitWithMemory(T *memory, Dqn_isize max, Dqn_isize size)
{
    Dqn_Array<T> result = {};
    result.data         = memory;
    result.size         = size;
    result.max          = max;
    return result;
}

template <typename T>
DQN_API Dqn_Array<T> Dqn_Array_InitWithArenaNoGrow(Dqn_Arena *arena, Dqn_isize max, Dqn_isize size, Dqn_ZeroMem zero_mem DQN_CALL_SITE_ARGS)
{
    Dqn_Array<T> result = {};
    if (max && arena)
    {
        auto *memory = DQN_CAST(T *)Dqn__ArenaAllocate(arena, sizeof(T) * max, alignof(T), zero_mem DQN_CALL_SITE_ARGS_INPUT);
        result = Dqn_ArrayInitWithMemory(memory, max, size);
    }

    return result;
}

template <typename T>
DQN_API bool Dqn_Array_Reserve(Dqn_Array<T> *a, Dqn_isize size DQN_CALL_SITE_ARGS)
{
    if (size <= a->size) return true;
    if (!a->arena) return false;

    T *new_ptr = DQN_CAST(T *)Dqn__ArenaAllocate(a->arena, sizeof(T) * size, alignof(T), Dqn_ZeroMem::Yes DQN_CALL_SITE_ARGS_INPUT);
    if (!new_ptr) return false;

    if (a->data)
    {
        // NOTE(dqn): Realloc, I don't like and don't want to support. Use virtual arrays
        DQN_MEMCOPY(new_ptr, a->data, a->size * sizeof(T));
    }

    a->data = new_ptr;
    a->max  = size;
    return true;
}

template <typename T>
DQN_API bool Dqn_Array_GrowIfNeeded(Dqn_Array<T> *a, Dqn_isize num_to_add DQN_CALL_SITE_ARGS)
{
    Dqn_isize new_size = a->size + num_to_add;
    bool result        = true;
    if (new_size > a->max)
    {
        Dqn_isize num_items = DQN_MAX(4, DQN_MAX(new_size, (a->max * 2)));
        result              = Dqn_Array_Reserve(a, num_items DQN_CALL_SITE_ARGS_INPUT);
    }

    return result;
}

template <typename T>
DQN_API T *Dqn_Array_AddArray(Dqn_Array<T> *a, T const *items, Dqn_isize num DQN_CALL_SITE_ARGS)
{
    if (!Dqn_Array_GrowIfNeeded(a, num DQN_CALL_SITE_ARGS_INPUT))
        return nullptr;
    T *result = static_cast<T *>(DQN_MEMCOPY(a->data + a->size, items, sizeof(T) * num));
    a->size += num;
    return result;
}

template <typename T>
DQN_API T *Dqn_Array_Add(Dqn_Array<T> *a, T const &item DQN_CALL_SITE_ARGS)
{
    if (!Dqn_Array_GrowIfNeeded(a, 1 DQN_CALL_SITE_ARGS_INPUT))
        return nullptr;
    a->data[a->size++] = item;
    return &a->data[a->size - 1];
}
template <typename T>
DQN_API T *Dqn_Array_Make(Dqn_Array<T> *a, Dqn_isize num DQN_CALL_SITE_ARGS)
{
    if (!Dqn_Array_GrowIfNeeded(a, num DQN_CALL_SITE_ARGS_INPUT))
        return nullptr;
    T *result = a->data + a->size;
    a->size += num;
    return result;
}

template <typename T>
DQN_API void Dqn_ArrayClear(Dqn_Array<T> *a, Dqn_ZeroMem zero_mem)
{
    a->size = 0;
    Dqn__ZeroMemBytes(a->data, sizeof(T) * a->max, zero_mem);
}

template <typename T>
DQN_API void Dqn_ArrayEraseStable(Dqn_Array<T> *a, Dqn_isize index)
{
    Dqn__EraseStableFromCArray<T>(a->data, a->size--, a->max, index);
}

template <typename T>
DQN_API void Dqn_ArrayEraseUnstable(Dqn_Array<T> *a, Dqn_isize index)
{
    DQN_ASSERT(index >= 0 && index < a->size);
    if (--a->size == 0) return;
    a->data[index] = a->data[a->size];
}

template <typename T>
DQN_API void Dqn_ArrayPop(Dqn_Array<T> *a, Dqn_isize num, Dqn_ZeroMem zero_mem)
{
    DQN_ASSERT(a->size - num >= 0);
    a->size -= num;

    void *begin     = a->data + a->size;
    void *end       = a->data + (a->size + num);
    Dqn_isize bytes = DQN_CAST(Dqn_isize) end - DQN_CAST(Dqn_isize) begin;
    Dqn__ZeroMemBytes(begin, bytes, zero_mem);
}

template <typename T>
DQN_API T *Dqn_ArrayPeek(Dqn_Array<T> *a)
{
    T *result = (a->size == 0) ? nullptr : a->data + (a->size - 1);
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_List Template Implementation
// -------------------------------------------------------------------------------------------------
template <typename T>
DQN_API Dqn_List<T> Dqn_ListInitWithArena(Dqn_Arena *arena, Dqn_isize chunk_size)
{
    Dqn_List<T> result = {};
    result.arena       = arena;
    result.chunk_size  = chunk_size;
    return result;
}

template <typename T>
DQN_API T *Dqn__ListMake(Dqn_List<T> *list, Dqn_isize count DQN_CALL_SITE_ARGS)
{
    if (list->chunk_size == 0)
        list->chunk_size = 128;

    if (!list->tail || (list->tail->count + count) > list->tail->size)
    {
        auto *tail = (Dqn_ListChunk<T> * )Dqn__ArenaAllocate(list->arena, sizeof(Dqn_ListChunk<T>), alignof(Dqn_ListChunk<T>), Dqn_ZeroMem::Yes DQN_CALL_SITE_ARGS_INPUT);
        if (!tail)
          return nullptr;

        Dqn_isize items = DQN_MAX(list->chunk_size, count);
        tail->data      = (T * )Dqn__ArenaAllocate(list->arena, sizeof(T) * items, alignof(T), Dqn_ZeroMem::Yes DQN_CALL_SITE_ARGS_INPUT);
        tail->size      = items;

        if (!tail->data)
            return nullptr;

        if (list->tail)
        {
            list->tail->next = tail;
            tail->prev       = list->tail;
        }

        list->tail = tail;

        if (!list->head)
            list->head = list->tail;
    }

    T *result = list->tail->data + list->tail->count;
    list->tail->count += count;
    list->count += count;
    return result;
}

template <typename T>
DQN_API Dqn_b32 Dqn_ListIterateFrom(Dqn_List<T> *list, Dqn_ListIterator<T> *iterator, Dqn_isize start_index)
{
    Dqn_b32 result = false;
    if (!list || !iterator || start_index < 0 || list->chunk_size <= 0)
        return result;

    if (!iterator->init)
    {
        *iterator = {};
        if (start_index == 0)
        {
            iterator->chunk = list->head;
        }
        else
        {
            Dqn_ListAt(list, start_index, &iterator->chunk);
            if (list->chunk_size > 0)
                iterator->chunk_data_index = start_index % list->chunk_size;
        }

        iterator->init = true;
    }

    if (iterator->chunk)
    {
        if (iterator->chunk_data_index >= iterator->chunk->count)
        {
            iterator->chunk            = iterator->chunk->next;
            iterator->chunk_data_index = 0;
        }

        if (iterator->chunk)
        {
            iterator->data = iterator->chunk->data + iterator->chunk_data_index++;
            result         = true;
        }
    }

    if (!iterator->chunk)
    {
        DQN_ASSERT(result == false);
    }
    return result;
}

template <typename T>
DQN_API T *Dqn_ListAt(Dqn_List<T> *list, Dqn_isize index, Dqn_ListChunk<T> **at_chunk)
{
    if (!list || !list->chunk_size || index < 0 || index >= list->count)
        return nullptr;

    Dqn_isize total_chunks       = list->count / (list->chunk_size + (list->chunk_size - 1));
    Dqn_isize desired_chunk      = index / list->chunk_size;
    Dqn_isize forward_scan_dist  = desired_chunk;
    Dqn_isize backward_scan_dist = total_chunks - desired_chunk;

    // NOTE: Linearly scan forwards/backwards to the chunk we need. We don't
    // have random access to chunks
    Dqn_isize current_chunk = 0;
    Dqn_ListChunk<T> **chunk = nullptr;
    if (forward_scan_dist <= backward_scan_dist)
    {
        for (chunk = &list->head;
             *chunk && current_chunk != desired_chunk;
             *chunk = (*chunk)->next, current_chunk++)
        {
        }
    }
    else
    {
        current_chunk = total_chunks;
        for (chunk = &list->tail;
             *chunk && current_chunk != desired_chunk;
             *chunk = (*chunk)->prev, current_chunk--)
        {
        }
    }

    T *result = nullptr;
    if (*chunk)
    {
        Dqn_isize relative_index = index % list->chunk_size;
        result                   = (*chunk)->data + relative_index;
        DQN_ASSERT(relative_index < (*chunk)->count);
    }

    if (result && at_chunk)
        *at_chunk = *chunk;

    return result;
}

#if defined(DQN_COMPILER_W32_MSVC)
    #pragma warning(pop)
#endif
#endif // DQN_H

// -------------------------------------------------------------------------------------------------
// NOTE: Implementation
// -------------------------------------------------------------------------------------------------
#if defined(DQN_IMPLEMENTATION)

#if defined(DQN_OS_WIN32)
    #pragma comment(lib, "bcrypt")
    #pragma comment(lib, "wininet")

    #if defined(DQN_NO_WIN32_MINIMAL_HEADER)
        #include <bcrypt.h>          // Dqn_OSSecureRNGBytes -> BCryptOpenAlgorithmProvider ... etc
        #include <shellscalingapi.h> // Dqn_WinMakeProcessDPIAware -> SetProcessDpiAwareProc
        #if defined(DQN_WITH_WIN_NET)
            #include <wininet.h> // Dqn_WinNet -> InternetConnect ... etc
        #endif // DQN_WITH_WIN_NET
    #else
        // Taken from Windows.h
        // ---------------------------------------------------------------------
        // Defines
        // ---------------------------------------------------------------------
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

        // NOTE: Free Type
        #define MEM_RELEASE 0x00008000

        // NOTE: Protect
        #define PAGE_READWRITE 0x04

        // NOTE: FindFirstFile
        #define INVALID_HANDLE_VALUE ((void *)(long *)-1)
        #define INVALID_FILE_ATTRIBUTES ((unsigned long)-1)
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

        #define CREATE_NEW          1
        #define CREATE_ALWAYS       2
        #define OPEN_EXISTING       3
        #define OPEN_ALWAYS         4
        #define TRUNCATE_EXISTING   5

        #define INVALID_FILE_SIZE ((unsigned long)0xFFFFFFFF)

        typedef enum PROCESS_DPI_AWARENESS
        {
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
            Dqn_u64 QuadPart;
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
            union {
                unsigned long dwOemId;          // Obsolete field...do not use
                struct {
                    Dqn_u16 wProcessorArchitecture;
                    Dqn_u16 wReserved;
                } DUMMYSTRUCTNAME;
            } DUMMYUNIONNAME;
            unsigned long dwPageSize;
            void *lpMinimumApplicationAddress;
            void *lpMaximumApplicationAddress;
            unsigned long *dwActiveProcessorMask;
            unsigned long dwNumberOfProcessors;
            unsigned long dwProcessorType;
            unsigned long dwAllocationGranularity;
            Dqn_u16 wProcessorLevel;
            Dqn_u16 wProcessorRevision;
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

        // ---------------------------------------------------------------------
        // Functions
        // ---------------------------------------------------------------------
        extern "C"
        {
        /*BOOL*/     int             CreateDirectoryW           (wchar_t const *lpPathName, SECURITY_ATTRIBUTES *lpSecurityAttributes);
        /*BOOL*/     int             RemoveDirectoryW           (wchar_t const *lpPathName);
        /*DWORD*/    unsigned long   GetCurrentDirectoryW       (unsigned long nBufferLength, wchar_t *lpBuffer);

        /*BOOL*/     int             FindNextFileW              (void *hFindFile, WIN32_FIND_DATAW *lpFindFileData);
        /*HANDLE*/   void           *FindFirstFileExW           (wchar_t const *lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, void *lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, void *lpSearchFilter, unsigned long dwAdditionalFlags);
        /*DWORD*/    unsigned long   GetFileAttributesExW       (wchar_t const *lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, WIN32_FILE_ATTRIBUTE_DATA *lpFileInformation);
        /*BOOL*/     int             GetFileSizeEx              (void *hFile, LARGE_INTEGER *lpFileSize);

        /*BOOL*/     int             MoveFileExW                (wchar_t const *lpExistingFileName, wchar_t const *lpNewFileName, unsigned long dwFlags);
        /*BOOL*/     int             CopyFileW                  (wchar_t const *lpExistingFileName, wchar_t const *lpNewFileName, int bFailIfExists);
        /*BOOL*/     int             DeleteFileW                (wchar_t const *lpExistingFileName);
        /*HANDLE*/   void           *CreateFileW                (wchar_t const *lpFileName, unsigned long dwDesiredAccess, unsigned long dwShareMode, SECURITY_ATTRIBUTES *lpSecurityAttributes, unsigned long dwCreationDisposition, unsigned long dwFlagsAndAttributes, void *hTemplateFile);
        /*BOOL*/     int             ReadFile                   (void *hFile, void *lpBuffer, unsigned long nNumberOfBytesToRead, unsigned long *lpNumberOfBytesRead, struct OVERLAPPED *lpOverlapped);
        /*BOOL*/     int             CloseHandle                (void *hObject);

        /*HMODULE*/  void           *LoadLibraryA               (char const *lpFileName);
        /*BOOL*/     int             FreeLibrary                (void *hModule);
        /*FARPROC*/  void           *GetProcAddress             (void *hModule, char const *lpProcName);

        /*DWORD*/    unsigned long   GetWindowModuleFileNameA   (void *hwnd, char *pszFileName, unsigned int cchFileNameMax);
        /*HMODULE*/  void           *GetModuleHandleA           (char const *lpModuleName);
        /*DWORD*/    unsigned long   GetModuleFileNameW         (void *hModule, wchar_t *lpFilename, unsigned long nSize);

        /*DWORD*/    unsigned long   WaitForSingleObject        (void *hHandle, unsigned long dwMilliseconds);

        /*BOOL*/     int             QueryPerformanceCounter    (LARGE_INTEGER *lpPerformanceCount);
        /*BOOL*/     int             QueryPerformanceFrequency  (LARGE_INTEGER *lpFrequency);

        /*HANDLE*/   void           *CreateThread               (SECURITY_ATTRIBUTES *lpThreadAttributes, size_t dwStackSize, unsigned long (*lpStartAddress)(void *), void *lpParameter, unsigned long dwCreationFlags, unsigned long *lpThreadId);
        /*HANDLE*/   void           *CreateSemaphoreA           (SECURITY_ATTRIBUTES *lpSecurityAttributes, long lInitialCount, long lMaxCount, char *lpName);
        /*BOOL*/     int             ReleaseSemaphore           (void *semaphore, long lReleaseCount, long *lpPreviousCount);
                     void            Sleep                      (unsigned long dwMilliseconds);

                     void           *VirtualAlloc               (void *lpAddress, size_t dwSize, unsigned long flAllocationType, unsigned long flProtect);
        /*BOOL*/     int             VirtualFree                (void *lpAddress, size_t dwSize, unsigned long dwFreeType);

                     void            GetSystemInfo              (SYSTEM_INFO *system_info);
                     void            GetSystemTime              (SYSTEMTIME  *lpSystemTime);
                     void            GetSystemTimeAsFileTime    (FILETIME    *lpFileTime);
                     void            GetLocalTime               (SYSTEMTIME  *lpSystemTime);

        /*DWORD*/    unsigned long   FormatMessageA             (unsigned long dwFlags, void *lpSource, unsigned long dwMessageId, unsigned long dwLanguageId, char *lpBuffer, unsigned long nSize, va_list *Arguments);
        /*DWORD*/    unsigned long   GetLastError               ();

                     int             MultiByteToWideChar        (unsigned int CodePage, unsigned long dwFlags, char const *lpMultiByteStr, int cbMultiByte, wchar_t *lpWideCharStr, int cchWideChar);
                     int             WideCharToMultiByte        (unsigned int CodePage, unsigned long dwFlags, wchar_t const *lpWideCharStr, int cchWideChar, char *lpMultiByteStr, int cbMultiByte, char const *lpDefaultChar, bool *lpUsedDefaultChar);

        /*NTSTATUS*/ long            BCryptOpenAlgorithmProvider(void *phAlgorithm, wchar_t const *pszAlgId, wchar_t const *pszImplementation, unsigned long dwFlags);
        /*NTSTATUS*/ long            BCryptGenRandom            (void *hAlgorithm, unsigned char *pbBuffer, unsigned long cbBuffer, unsigned long dwFlags);

        /*BOOLAPI*/  int             InternetCrackUrlA          (char const *lpszUrl, unsigned long dwUrlLength, unsigned long dwFlags, URL_COMPONENTSA *lpUrlComponents);
        /*HANDLE*/   void           *InternetOpenA              (char const *lpszAgent, unsigned long dwAccessType, char const *lpszProxy, char const *lpszProxyBypass, unsigned long dwFlags);
        /*HANDLE*/   void           *InternetConnectA           (void *hInternet, char const *lpszServerName, INTERNET_PORT nServerPort, char const *lpszUserName, char const *lpszPassword, unsigned long dwService, unsigned long dwFlags, unsigned long *dwContext);
        /*BOOLAPI*/  int             InternetSetOptionA         (void *hInternet, unsigned long dwOption, void *lpBuffer, unsigned long dwBufferLength);
        /*BOOLAPI*/  int             InternetReadFile           (void *hFile, void *lpBuffer, unsigned long dwNumberOfBytesToRead, unsigned long *lpdwNumberOfBytesRead);
        /*BOOLAPI*/  int             InternetCloseHandle        (void *hInternet);
        /*HANDLE*/   void           *HttpOpenRequestA           (void *hConnect, char const *lpszVerb, char const *lpszObjectName, char const *lpszVersion, char const *lpszReferrer, char const **lplpszAcceptTypes, unsigned long dwFlags, unsigned long *dwContext);
        /*BOOLAPI*/  int             HttpSendRequestA           (void *hRequest, char const *lpszHeaders, unsigned long dwHeadersLength, void *lpOptional, unsigned long dwOptionalLength);
        }
    #endif // !defined(DQN_NO_WIN32_MINIMAL_HEADER)
#elif defined(DQN_OS_UNIX)
    #include <errno.h>        // errno
    #include <fcntl.h>        // O_RDONLY ... etc
    #include <linux/fs.h>     // FICLONE
    #include <sys/ioctl.h>    // ioctl
    #include <sys/random.h>   // getrandom
    #include <sys/stat.h>     // stat
    #include <sys/sendfile.h> // sendfile
    #include <time.h>         // clock_gettime, nanosleep
    #include <unistd.h>       // access
#endif

Dqn_Lib dqn__lib;
DQN_API void Dqn__ZeroMemBytes(void *ptr, Dqn_usize count, Dqn_ZeroMem zero_mem)
{
    if (zero_mem == Dqn_ZeroMem::Yes)
    {
        DQN_MEMSET(ptr, 0, count);
    }
    else
    {
#if defined(DQN_MEMZERO_DEBUG_BYTE)
        DQN_MEMSET(ptr, DQN_MEMZERO_DEBUG_BYTE, count);
#endif
    }
}

// -------------------------------------------------------------------------------------------------
// NOTE: Intrinsics
// -------------------------------------------------------------------------------------------------
#if defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
#include <cpuid.h>
#endif

// -------------------------------------------------------------------------------------------------
// NOTE: CPUID
// -------------------------------------------------------------------------------------------------
Dqn_CPUIDRegisters Dqn_CPUID(int function_id)
{
    Dqn_CPUIDRegisters result = {};
#if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    __cpuid(DQN_CAST(int *)result.array, function_id);
#elif defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
    __get_cpuid(function_id, &result.array[0] /*eax*/, &result.array[1] /*ebx*/, &result.array[2] /*ecx*/ , &result.array[3] /*edx*/);
#else
    #error "Compiler not supported"
#endif
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_TicketMutex
// -------------------------------------------------------------------------------------------------
void Dqn_TicketMutexBegin(Dqn_TicketMutex *mutex)
{
    unsigned int ticket = Dqn_AtomicAddU32(&mutex->ticket, 1);
    Dqn_TicketMutexBeginTicket(mutex, ticket);
}

void Dqn_TicketMutexEnd(Dqn_TicketMutex *mutex)
{
    Dqn_AtomicAddU32(&mutex->serving, 1);
}

unsigned int Dqn_TicketMutexMakeTicket(Dqn_TicketMutex *mutex)
{
    unsigned int result = Dqn_AtomicAddU32(&mutex->ticket, 1);
    return result;
}

void Dqn_TicketMutexBeginTicket(const Dqn_TicketMutex *mutex, unsigned int ticket)
{
    DQN_ASSERT_MSG(mutex->serving <= ticket,
                   "Mutex skipped ticket? Was ticket generated by the correct mutex via MakeTicket? ticket = %u, "
                   "mutex->serving = %u",
                   ticket,
                   mutex->serving);
    while(ticket != mutex->serving)
    {
        // NOTE: Use spinlock intrinsic
        _mm_pause();
    }
}

Dqn_b32 Dqn_TicketMutexCanLock(const Dqn_TicketMutex *mutex, unsigned int ticket)
{
    Dqn_b32 result = (ticket == mutex->serving);
    return result;
}


// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Fmt Implementation
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_isize Dqn_FmtVLenNoNullTerminator(char const *fmt, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    Dqn_isize result = STB_SPRINTF_DECORATE(vsnprintf)(nullptr, 0, fmt, args_copy);
    va_end(args_copy);
    return result;
}

DQN_API Dqn_isize Dqn_FmtLenNoNullTerminator(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_isize result = STB_SPRINTF_DECORATE(vsnprintf)(nullptr, 0, fmt, args);
    va_end(args);
    return result;
}

DQN_API Dqn_isize Dqn_FmtVLen(char const *fmt, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    Dqn_isize result = STB_SPRINTF_DECORATE(vsnprintf)(nullptr, 0, fmt, args_copy) + 1;
    va_end(args_copy);
    return result;
}

DQN_API Dqn_isize Dqn_FmtLen(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_isize result = STB_SPRINTF_DECORATE(vsnprintf)(nullptr, 0, fmt, args) + 1;
    va_end(args);
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Log
// -------------------------------------------------------------------------------------------------
DQN_API void Dqn_LogVDefault(Dqn_LogType type,
                             void *      user_data,
                             char const *file,
                             Dqn_uint    file_len,
                             char const *func,
                             Dqn_uint    func_len,
                             Dqn_uint    line,
                             char const *fmt,
                             va_list     va)
{
    (void)user_data;

    // ---------------------------------------------------------------------------------------------
    // NOTE: Ensure log-file is opened for appending, ultra-lightweight spinlock mutex
    // ---------------------------------------------------------------------------------------------
    Dqn_TicketMutexBegin(&dqn__lib.log_file_mutex);
    if (!dqn__lib.log_no_output_file)
    {
        if (!dqn__lib.log_file)
        {
            Dqn_ThreadScratch scratch  = Dqn_ThreadGetScratch();
            Dqn_String        exe_dir  = Dqn_OSExecutableDirectory(scratch.arena);
            Dqn_String        log_file = Dqn_StringFmt(scratch.arena, "%.*s/dqn.log", DQN_STRING_FMT(exe_dir));
            fopen_s(DQN_CAST(FILE **)&dqn__lib.log_file, log_file.str, "a");
        }
    }
    Dqn_TicketMutexEnd(&dqn__lib.log_file_mutex);

    // ---------------------------------------------------------------------------------------------
    // NOTE: Extract file name from the path
    // ---------------------------------------------------------------------------------------------
    Dqn_isize file_name_len = 0;
    char const *file_name   = Dqn_CStringFileNameFromPath(file, file_len, &file_name_len);

    // -----------------------------------------------------------------------------------------
    // NOTE: Construct log
    // -----------------------------------------------------------------------------------------
    Dqn_ThreadScratch scratch = Dqn_ThreadGetScratch();
    Dqn_DateHMSTimeString const time = Dqn_DateHMSLocalTimeStringNow();
    Dqn_StringList string_list = {};
    Dqn_StringListAppendFmt(&string_list,
                             scratch.arena,
                             "[%.*s|%.*s|%s|%.*s|%05u|%.*s] ",
                             time.date_size,
                             time.date,
                             time.hms_size,
                             time.hms,
                             Dqn_LogTypeString[DQN_CAST(int) type],
                             DQN_CAST(int) file_name_len,
                             file_name,
                             line,
                             DQN_CAST(int) func_len,
                             func);

    Dqn_StringListAppendFmtV(&string_list, scratch.arena, fmt, va);
    Dqn_String log_line = Dqn_StringListBuild(&string_list, scratch.arena);

    // -----------------------------------------------------------------------------------------
    // NOTE: Print log to destinations
    // -----------------------------------------------------------------------------------------
    FILE *handles[] =
    {
        type == Dqn_LogType::Error ? stderr : stdout,
        DQN_CAST(FILE *)dqn__lib.log_file,
    };

    for (FILE *handle : handles)
    {
        if (handle)
            fprintf(handle, "%.*s\n", DQN_STRING_FMT(log_line));
    }
}

DQN_API void Dqn_LogV(Dqn_LogType type,
                      void *      user_data,
                      char const *file,
                      Dqn_uint    file_len,
                      char const *func,
                      Dqn_uint    func_len,
                      Dqn_uint    line,
                      char const *fmt,
                      va_list     va)
{
    Dqn_LogProc *logger = dqn__lib.LogCallback ? dqn__lib.LogCallback : Dqn_LogVDefault;
    logger(type, user_data, file, file_len, func, func_len, line, fmt, va);
}

DQN_API void Dqn_Log(Dqn_LogType type, void *user_data, char const *file, Dqn_uint file_len, char const *func, Dqn_uint func_len, Dqn_uint line, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Dqn_LogV(type, user_data, file, file_len, func, func_len, line, fmt, va);
    va_end(va);
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Align*
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_uintptr Dqn_AlignAddressEnsuringSpace(Dqn_uintptr address, Dqn_u8 alignment)
{
    Dqn_uintptr remainder       = address % alignment;
    Dqn_uintptr offset_to_align = alignment - remainder;
    Dqn_uintptr result          = address + offset_to_align;
    DQN_ASSERT(result % alignment == 0);
    DQN_ASSERT(result >= address);
    DQN_ASSERT(offset_to_align >= 1 && offset_to_align <= alignment);
    return result;
}

DQN_API Dqn_uintptr Dqn_AlignAddress(Dqn_uintptr address, Dqn_u8 alignment)
{
    Dqn_uintptr result = address;
    if (alignment > 0)
    {
        Dqn_uintptr remainder = result % alignment;
        if (remainder > 0)
        {
            Dqn_uintptr offset_to_align = alignment - remainder;
            result += offset_to_align;
        }
    }
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_AllocationTracer
// -------------------------------------------------------------------------------------------------
void Dqn_AllocationTracerAdd(Dqn_AllocationTracer *tracer, void *ptr, Dqn_usize size DQN_CALL_SITE_ARGS)
{
#if DQN_ALLOCATION_TRACING
    if (!tracer) return;
    Dqn_AllocationTrace trace = {};
    trace.ptr                 = ptr;
    trace.size                = size;
    trace.file                = file_;
    trace.func                = func_;
    trace.line                = line_;
    trace.msg                 = msg_;

    Dqn_TicketMutexBegin(&tracer->mutex);
#if 0 // TODO(dqn): We need to revisit this
    if (!trace->map.values)
    {
        trace->arena = Dqn_ArenaInitWithCRT();
        trace->map   = Dqn_MapInitWithArena(&trace->arena, 16192);
    }
#endif

    Dqn_b32 added = Dqn_MapAdd(&tracer->map, DQN_CAST(Dqn_u64) ptr, trace, Dqn_MapCollideRule::Chain);
    if (!added)
    {
        // Dqn_AllocationTrace *other = Dqn_MapGet(&tracer->table, DQN_CAST(Dqn_u64) ptr);
        DQN_ASSERT_MSG(added, "Hash table collision on %Ix", ptr);
    }
    Dqn_TicketMutexEnd(&tracer->mutex);
#else
    (void)tracer; (void)ptr; (void)size;
#endif
}

void Dqn_AllocationTracerRemove(Dqn_AllocationTracer *tracer, void *ptr)
{
#if DQN_ALLOCATION_TRACING
    if (!tracer) return;
    Dqn_TicketMutexBegin(&tracer->mutex);
    Dqn_AllocationTrace *trace = Dqn_MapGet(&tracer->map, DQN_CAST(Dqn_u64) ptr);
    DQN_ASSERT_MSG(trace->ptr == ptr, "(trace->ptr = %Ix, raw_ptr = %Ix", trace->ptr, ptr);

    Dqn_MapErase(&tracer->map, DQN_CAST(Dqn_u64) ptr);
    Dqn_TicketMutexEnd(&tracer->mutex);
#else
    (void)tracer; (void)ptr;
#endif
}

#if defined(DQN_WITH_CRT_ALLOCATOR)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_CRTAllocator
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_CRTAllocator Dqn_CRTAllocatorInitWithProcs(Dqn_CRTAllocatorMallocProc *malloc_proc, Dqn_CRTAllocatorReallocProc *realloc_proc, Dqn_CRTAllocatorFreeProc *free_proc)
{
    Dqn_CRTAllocator result = {};
    result.malloc           = malloc_proc;
    result.realloc          = realloc_proc;
    result.free             = free_proc;
    return result;
}

DQN_API void *Dqn__CRTAllocatorMalloc(Dqn_CRTAllocator *allocator, Dqn_usize size DQN_CALL_SITE_ARGS)
{
    void *result = allocator->malloc ? allocator->malloc(size) : DQN_MALLOC(size);
    if (result)
    {
        Dqn_AtomicAddU64(&allocator->malloc_bytes, size);
        Dqn_AtomicAddU64(&allocator->malloc_count, 1ULL);
        Dqn_AllocationTracerAdd(allocator->tracer, result, size DQN_CALL_SITE_ARGS_INPUT);
    }

    return result;
}

DQN_API void *Dqn__CRTAllocatorRealloc(Dqn_CRTAllocator *allocator, void *ptr, Dqn_usize size DQN_CALL_SITE_ARGS)
{
    void *result = allocator->realloc ? allocator->realloc(ptr, size) : DQN_REALLOC(ptr, size);
    if (result)
    {
        Dqn_AtomicAddU64(&allocator->realloc_bytes, size);
        Dqn_AtomicAddU64(&allocator->realloc_count, 1ULL);
        Dqn_AllocationTracerAdd(allocator->tracer, result, size DQN_CALL_SITE_ARGS_INPUT);
        if (result != ptr) Dqn_AllocationTracerRemove(allocator->tracer, ptr);
    }

    return result;
}

DQN_API void Dqn__CRTAllocatorFree(Dqn_CRTAllocator *allocator, void *ptr)
{
    if (ptr)
    {
        Dqn_AtomicAddU64(&allocator->free_count, 1ULL);
        Dqn_AllocationTracerRemove(allocator->tracer, ptr);
    }
    allocator->free ? allocator->free(ptr) : DQN_FREE(ptr);
}
#endif // DQN_WITH_CRT_ALLOCATOR

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_String
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_String Dqn_StringInit(char const *string, Dqn_isize size)
{
    Dqn_String result = {};
    result.str        = DQN_CAST(char *)string;
    result.size       = size;
    result.cap        = result.size;
    return result;
}

DQN_API Dqn_String Dqn_StringInitCString(char const *string)
{
    Dqn_String result = {};
    result.str        = DQN_CAST(char *)string;
    result.size       = Dqn_CStringSize(string);
    result.cap        = result.size;
    return result;
}


DQN_API Dqn_String Dqn_StringInitMemory(char *buf, Dqn_isize capacity)
{
    DQN_ASSERT(capacity > 0);
    Dqn_String result = {};
    result.str        = buf;
    result.size       = 0;
    result.cap        = capacity - 1; // Reserve 1 byte for null terminator
    return result;
}

DQN_API Dqn_b32 Dqn_StringIsValid(Dqn_String in)
{
    Dqn_b32 result = in.size >= 0 && in.str;
    return result;
}

DQN_API Dqn_String Dqn__StringFmt(Dqn_Arena *arena DQN_CALL_SITE_ARGS, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Dqn_String result = Dqn__StringFmtV(arena, fmt, va DQN_CALL_SITE_ARGS_INPUT);
    va_end(va);
    return result;
}

DQN_API Dqn_String Dqn__StringFmtV(Dqn_Arena *arena, char const *fmt, va_list va DQN_CALL_SITE_ARGS)
{
    Dqn_String result = {};
    va_list va2;
    va_copy(va2, va);
    result.size = STB_SPRINTF_DECORATE(vsnprintf)(nullptr, 0, fmt, va);
    result.str  = DQN_CAST(char *)Dqn__ArenaAllocate(arena, sizeof(char) * (result.size + 1), alignof(char), Dqn_ZeroMem::No DQN_CALL_SITE_ARGS_INPUT);
    if (result.str)
    {
        STB_SPRINTF_DECORATE(vsnprintf)(result.str, Dqn_SafeTruncateISizeToInt(result.size + 1), fmt, va2);
        result.str[result.size] = 0;
        result.cap              = result.size;
    }
    va_end(va2);
    return result;
}

DQN_API Dqn_String Dqn_String_Allocate(Dqn_Arena *arena, Dqn_isize size, Dqn_ZeroMem zero_mem DQN_CALL_SITE_ARGS)
{
    Dqn_String result = {};
    result.str        = DQN_CAST(char *)Dqn__ArenaAllocate(arena, size + 1, alignof(char), zero_mem DQN_CALL_SITE_ARGS_INPUT);
    result.cap        = size;
    return result;
}

DQN_API Dqn_String Dqn_String_CopyCString(char const *string, Dqn_isize size, Dqn_Arena *arena DQN_CALL_SITE_ARGS)
{
    Dqn_String result = {};
    if (size < 0 || !string || !arena)
    {
        DQN_INVALID_CODE_PATH;
        return result;
    }

    char *copy = DQN_CAST(char *)Dqn__ArenaAllocate(arena, size + 1, alignof(char), Dqn_ZeroMem::No DQN_CALL_SITE_ARGS_INPUT);
    DQN_MEMCOPY(copy, string, DQN_CAST(size_t)size);
    copy[size] = 0;

    result = Dqn_StringInit(copy, size);
    return result;
}

DQN_API Dqn_String Dqn_String_Copy(Dqn_String const src, Dqn_Arena *arena DQN_CALL_SITE_ARGS)
{
    Dqn_String result = Dqn_String_CopyCString(src.str, src.size, arena DQN_CALL_SITE_ARGS_INPUT);
    return result;
}

DQN_API Dqn_String Dqn_StringTrimWhitespaceAround(Dqn_String const src)
{
    Dqn_String result = {};
    result.str        = DQN_CAST(char *)Dqn_CStringTrimWhitespaceAround(src.str, src.size, &result.size);
    result.cap        = result.size;
    return result;
}

DQN_API Dqn_b32 operator==(Dqn_String const &lhs, Dqn_String const &rhs)
{
    Dqn_b32 result = false;
    if (lhs.size == rhs.size)
        result = (lhs.str == rhs.str) || (DQN_MEMCMP(lhs.str, rhs.str, lhs.size) == 0);
    return result;
}

DQN_API Dqn_b32 operator!=(Dqn_String const &lhs, Dqn_String const &rhs)
{
    Dqn_b32 result = !(lhs == rhs);
    return result;
}

DQN_API Dqn_b32 Dqn_StringAppendFmtV(Dqn_String *str, char const *fmt, va_list va)
{
    va_list va2;
    va_copy(va2, va);
    Dqn_isize require = STB_SPRINTF_DECORATE(vsnprintf)(nullptr, 0, fmt, va);
    Dqn_isize space   = str->cap - str->size;
    Dqn_b32 result    = require <= space;

    if (!result)
    {
        DQN_LOG_W("Insufficient space in string: require=%I64d, space=%I64d", require, space);
        return result;
    }

    str->size += STB_SPRINTF_DECORATE(vsnprintf)(str->str + str->size, DQN_CAST(int)(space + 1 /*null terminator*/), fmt, va2);
    va_end(va2);
    return result;
}

DQN_API Dqn_b32 Dqn_StringAppendFmt(Dqn_String *str, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Dqn_b32 result = Dqn_StringAppendFmtV(str, fmt, va);
    va_end(va);
    return result;
}

DQN_API Dqn_b32 Dqn_StringEq(Dqn_String const lhs, Dqn_String const rhs, Dqn_StringEqCase eq_case)
{
    Dqn_b32 result = false;
    if (lhs.size == rhs.size)
    {
        if (eq_case == Dqn_StringEqCase::Sensitive)
            result = (DQN_MEMCMP(lhs.str, rhs.str, DQN_CAST(size_t)lhs.size) == 0);
        else
        {
            result = true;
            for (Dqn_isize index = 0; index < lhs.size && result; index++)
                result = (Dqn_CharToLower(lhs.str[index]) == Dqn_CharToLower(rhs.str[index]));
        }
    }
    return result;
}

DQN_API Dqn_b32 Dqn_StringEqInsensitive(Dqn_String const lhs, Dqn_String const rhs)
{
    Dqn_b32 result = Dqn_StringEq(lhs, rhs, Dqn_StringEqCase::Insensitive);
    return result;
}

DQN_API Dqn_b32 Dqn_StringStartsWith(Dqn_String string, Dqn_String prefix, Dqn_StringEqCase eq_case)
{
    Dqn_b32 result = false;
    if (prefix.size > string.size)
        return result;

    Dqn_String substring = Dqn_String{string.str, prefix.size, prefix.size};
    result               = Dqn_StringEq(substring, prefix, eq_case);
    return result;
}

DQN_API Dqn_b32 Dqn_StringStartsWithInsensitive(Dqn_String string, Dqn_String prefix)
{
    Dqn_b32 result = Dqn_StringStartsWith(string, prefix, Dqn_StringEqCase::Insensitive);
    return result;
}

DQN_API Dqn_b32 Dqn_StringEndsWith(Dqn_String       string,
                                    Dqn_String       suffix,
                                    Dqn_StringEqCase eq_case)
{
    Dqn_b32 result = false;
    if (suffix.size > string.size)
        return result;

    Dqn_String substring = Dqn_String{string.str + string.size - suffix.size, suffix.size, suffix.size};
    result               = Dqn_StringEq(substring, suffix, eq_case);
    return result;
}

DQN_API Dqn_b32 Dqn_StringEndsWithInsensitive(Dqn_String string, Dqn_String suffix)
{
    Dqn_b32 result = Dqn_StringEndsWith(string, suffix, Dqn_StringEqCase::Insensitive);
    return result;
}

DQN_API Dqn_Array<Dqn_String> Dqn_StringSplit(Dqn_String src, Dqn_Arena *arena)
{
    enum StringSplitStage
    {
        StringSplitStage_Enumerate,
        StringSplitStage_Write,
        StringSplitStage_Count,
    };

    Dqn_Array<Dqn_String> result      = {};
    int                   split_count = 0;

    for (int stage = StringSplitStage_Enumerate;
         stage < StringSplitStage_Count;
         stage++)
    {
        char const *begin = src.str;
        char const *end   = src.str;

        if (stage == StringSplitStage_Write)
            result = Dqn_ArrayInitWithArenaNoGrow(arena, Dqn_String, split_count, 0, Dqn_ZeroMem::No);

        for (;;)
        {
            while (end[0] != '\r' && end[0] != '\n' && end[0] != 0)
                end++;

            if (end[0] == 0)
                break;

            auto split = Dqn_StringInit(begin, end - begin);
            begin      = end + 1;
            end        = begin;

            if (split.size == 0)
                continue;
            else
            {
                if (stage == StringSplitStage_Enumerate) split_count++;
                else result.data[result.size++] = split;
            }
        }
    }

    DQN_ASSERT(split_count == result.size);
    return result;
}

DQN_API Dqn_String Dqn_StringTrimPrefix(Dqn_String str, Dqn_String prefix, Dqn_StringEqCase eq_case)
{
    Dqn_String result = str;
    if (Dqn_StringStartsWith(str, prefix, eq_case))
    {
        result.str += prefix.size;
        result.size -= prefix.size;
        result.cap = result.size;
    }

    return result;
}

DQN_API Dqn_String Dqn_StringTrimSuffix(Dqn_String str, Dqn_String suffix, Dqn_StringEqCase eq_case)
{
    Dqn_String result = str;
    if (Dqn_StringEndsWith(str, suffix, eq_case))
    {
        result.size -= suffix.size;
        result.cap = result.size;
    }

    return result;
}

DQN_API Dqn_String Dqn_StringTrimByteOrderMark(Dqn_String src)
{
    // TODO(dqn): This is little endian
    auto UTF16BOM    = DQN_STRING("\xFF\xEF");
    auto UTF8BOM     = DQN_STRING("\xEF\xBB\xBF");
    Dqn_String result = Dqn_StringTrimPrefix(src, UTF16BOM);
    result            = Dqn_StringTrimPrefix(result, UTF8BOM);
    return result;
}

DQN_API Dqn_b32 Dqn_StringIsAllDigits(Dqn_String src)
{
    if (!Dqn_StringIsValid(src) || src.size == 0)
        return false;

    for (Dqn_isize ch_index = 0; ch_index < src.size; ch_index++)
    {
        if (!(src.str[ch_index] >= '0' && src.str[ch_index] <= '9'))
            return false;
    }

    return true;
}

DQN_API Dqn_b32 Dqn_StringIsAllHex(Dqn_String src)
{
    if (!Dqn_StringIsValid(src))
        return false;

    Dqn_String src_no_0x = Dqn_StringTrimPrefix(src, DQN_STRING("0x"), Dqn_StringEqCase::Insensitive);
    for (Dqn_isize ch_index = 0; ch_index < src_no_0x.size; ch_index++)
    {
        char ch = src_no_0x.str[ch_index];
        if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')))
            return false;
    }

    return true;
}

DQN_API Dqn_b32 Dqn_StringContainsChar(Dqn_String src, char ch)
{
    // TODO(dqn): This is easily simd-able for heavy workloads
    for (Dqn_isize ch_index = 0; ch_index < src.size; ch_index++)
    {
        if (src.str[ch_index] == ch)
            return true;
    }

    return false;
}

DQN_API void Dqn_StringRemove(Dqn_String *in, Dqn_isize begin, Dqn_isize size)
{
    if (!in)
        return;

    if (size > in->size)
        return;

    char *dest      = in->str + begin;
    char const *src = in->str + (begin + size);
    char const *end = in->str + in->size;
    DQN_MEMMOVE(dest, src, end - src);

    in->size -= size;
}

DQN_API Dqn_isize Dqn_StringFindOffset(Dqn_String src, Dqn_String find, Dqn_isize start_index, Dqn_StringEqCase eq_case)
{
    Dqn_isize result  = -1;
    if (!Dqn_StringIsValid(src) || !Dqn_StringIsValid(find))
        return -1;

    Dqn_isize src_end = src.size - find.size;
    for (Dqn_isize index = start_index; index <= src_end; index++)
    {
        Dqn_String check = Dqn_StringInit(src.str + index, find.size);
        if (Dqn_StringEq(check, find, eq_case))
        {
            result = index;
            break;
        }
    }

    return result;
}

DQN_API Dqn_String Dqn_StringFind(Dqn_String src, Dqn_String find, Dqn_isize start_index, Dqn_StringEqCase eq_case)
{
    Dqn_isize offset = Dqn_StringFindOffset(src, find, start_index, eq_case);
    Dqn_String result = src;
    if (offset == -1)
    {
        result.size = 0;
    }
    else
    {
        result.str += offset;
        result.size -= offset;
    }

    return result;
}

DQN_API Dqn_String Dqn_StringReplace(Dqn_String src,
                                     Dqn_String find,
                                     Dqn_String replace,
                                     Dqn_isize start_index,
                                     Dqn_Arena *arena,
                                     Dqn_Arena *temp_arena,
                                     Dqn_StringEqCase eq_case)
{
    auto temp_arena_scope       = Dqn_ArenaScope(temp_arena);
    Dqn_StringList string_list = {};
    Dqn_isize src_end          = src.size - find.size;
    Dqn_isize head             = start_index;

    for (Dqn_isize tail = head; tail <= src_end; tail++)
    {
        Dqn_String check = Dqn_StringInit(src.str + tail, find.size);
        if (!Dqn_StringEq(check, find, eq_case))
            continue;

        if (start_index > 0 && string_list.string_size == 0)
        {
            // User provided a hint in the string to start searching from, we
            // need to add the string up to the hint. We only do this if there's
            // a replacement action, otherwise we have a special case for no
            // replacements, where the entire string gets copied.
            Dqn_String string = Dqn_StringInit(src.str, head);
            Dqn_StringListAppendString(&string_list, temp_arena, string);
        }

        Dqn_String range = Dqn_StringInit(src.str + head, (tail - head));
        Dqn_StringListAppendString(&string_list, temp_arena, range);
        Dqn_StringListAppendString(&string_list, temp_arena, replace);
        head = tail + find.size;
        tail += find.size - 1; // NOTE: -1 since the for loop will post increment us past the end of the find string
    }

    Dqn_String result = {};
    if (string_list.string_size == 0)
    {
        // NOTE: No replacement possible, so we just do a full-copy
        result = Dqn_StringCopy(src, arena);
    }
    else
    {
        Dqn_String remainder = Dqn_StringInit(src.str + head, src.size - head);
        Dqn_StringListAppendString(&string_list, temp_arena, remainder);
        result = Dqn_StringListBuild(&string_list, arena);
    }

    return result;
}

DQN_API Dqn_String Dqn_StringReplaceInsensitive(Dqn_String src, Dqn_String find, Dqn_String replace, Dqn_isize start_index, Dqn_Arena *arena, Dqn_Arena *temp_arena)
{
    Dqn_String result = Dqn_StringReplace(src, find, replace, start_index, arena, temp_arena, Dqn_StringEqCase::Insensitive);
    return result;
}

DQN_API Dqn_String Dqn_StringFileNameFromPath(Dqn_String path)
{
    Dqn_String result = path;
    for (Dqn_isize i = (result.size - 1); i >= 0; --i)
    {
        if (result.str[i] == '\\' || result.str[i] == '/')
        {
            char const *end = result.str + result.size;
            result.str      = result.str + (i + 1);
            result.size     = DQN_CAST(Dqn_isize)(end - result.str);
            break;
        }
    }

    return result;
}

DQN_API Dqn_u64 Dqn_StringToU64(Dqn_String str)
{
    Dqn_u64 result = Dqn_CStringToU64(str.str, DQN_CAST(int)str.size);
    return result;
}

DQN_API Dqn_i64 Dqn_StringToI64(Dqn_String str)
{
    Dqn_i64 result = Dqn_CStringToI64(str.str, DQN_CAST(int)str.size);
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_StringList Implementation
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_StringListNode *Dqn_StringListMakeNode(Dqn_Arena *arena, Dqn_isize size)
{
    Dqn_StringListNode *result = Dqn_ArenaNew(arena, Dqn_StringListNode, Dqn_ZeroMem::Yes);
    if (size) result->string = Dqn_StringAllocate(arena, size, Dqn_ZeroMem::Yes);
    return result;
}

DQN_API void Dqn_StringListAddNode(Dqn_StringList *list, Dqn_StringListNode *node)
{
    if (list->curr)
    {
        list->curr->next = node;
        list->curr       = list->curr->next;
    }
    else
    {
        list->head = node;
        list->curr = node;
    }

    list->string_size += node->string.size;
}

DQN_API void Dqn_StringListAppendFmtV(Dqn_StringList *list, Dqn_Arena *arena, char const *fmt, va_list args)
{
    Dqn_StringListNode *node = Dqn_StringListMakeNode(arena, 0 /*size*/);
    node->string             = Dqn_StringFmtV(arena, fmt, args);
    Dqn_StringListAddNode(list, node);
}

DQN_API void Dqn_StringListAppendFmt(Dqn_StringList *list, Dqn_Arena *arena, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_StringListAppendFmtV(list, arena, fmt, args);
    va_end(args);
}

DQN_API void Dqn_StringListAppendStringCopy(Dqn_StringList *list, Dqn_Arena *arena, Dqn_String string)
{
    if (string.size <= 0)
        return;

    Dqn_StringListNode *node = Dqn_StringListMakeNode(arena, 0 /*size*/);
    node->string             = Dqn_StringCopy(string, arena);
    Dqn_StringListAddNode(list, node);
}

DQN_API void Dqn_StringListAppendString(Dqn_StringList *list, Dqn_Arena *arena, Dqn_String string)
{
    if (string.size <= 0)
        return;

    Dqn_StringListNode *node = Dqn_StringListMakeNode(arena, 0 /*size*/);
    node->string             = string;
    Dqn_StringListAddNode(list, node);
}

DQN_API Dqn_String Dqn_StringListBuild(Dqn_StringList const *list, Dqn_Arena *arena)
{
    Dqn_String result = Dqn_StringAllocate(arena, list->string_size, Dqn_ZeroMem::No);
    for (Dqn_StringListNode const *node = list->head; node; node = node->next)
    {
        DQN_MEMCOPY(result.str + result.size, node->string.str, node->string.size);
        result.size += node->string.size;
    }
    result.str[result.size] = 0;
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Arena
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_ArenaStatsString Dqn_ArenaStatsToString(Dqn_ArenaStats const *stats)
{
    // NOTE: We use a non-standard format string that is only usable via
    // stb sprintf that GCC warns about as an error. This pragma mutes that.
    #if defined(DQN_COMPILER_GCC)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wformat="
        #pragma GCC diagnostic ignored "-Wformat-extra-args"
    #elif defined(DQN_COMPILER_W32_CLANG)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wformat"
        #pragma GCC diagnostic ignored "-Wformat-invalid-specifier"
        #pragma GCC diagnostic ignored "-Wformat-extra-args"
    #endif

    Dqn_ArenaStatsString result = {};
    result.size = STB_SPRINTF_DECORATE(snprintf)(result.str,
                                                 DQN_CAST(int)Dqn_ArrayCountI(result.str),
                                                 "%_$$I64d/%_$$I64d (wasted %_$$I64d - %d blks)",
                                                 stats->bytes_used,
                                                 stats->bytes_allocated,
                                                 stats->bytes_wasted,
                                                 stats->block_count);

    #if defined(DQN_COMPILER_GCC)
        #pragma GCC diagnostic pop
    #elif defined(DQN_COMPILER_W32_CLANG)
        #pragma GCC diagnostic pop
    #endif

    return result;
}

DQN_API void *Dqn__ArenaCopy(Dqn_Arena *arena, void *src, Dqn_isize size, Dqn_u8 alignment DQN_CALL_SITE_ARGS)
{
    void *result = Dqn__ArenaAllocate(arena, size, alignment, Dqn_ZeroMem::No DQN_CALL_SITE_ARGS_INPUT);
    DQN_MEMCOPY(result, src, size);
    return result;
}

DQN_API void *Dqn__ArenaCopyNullTerminate(Dqn_Arena *arena, void *src, Dqn_isize size, Dqn_u8 alignment DQN_CALL_SITE_ARGS)
{
    void *result = Dqn__ArenaAllocate(arena, size + 1, alignment, Dqn_ZeroMem::Yes DQN_CALL_SITE_ARGS_INPUT);
    DQN_MEMCOPY(result, src, size);
    return result;
}

DQN_API Dqn_ArenaMemBlock *Dqn__ArenaAllocateBlock(Dqn_Arena *arena, Dqn_isize requested_size DQN_CALL_SITE_ARGS)
{
    Dqn_isize min_block_size = arena->min_block_size;
    if (min_block_size == 0) min_block_size = DQN_MEM_ARENA_DEFAULT_MIN_BLOCK_SIZE;

    Dqn_isize mem_block_size = DQN_MAX(min_block_size, requested_size);
    auto const allocate_size = DQN_CAST(Dqn_isize)(sizeof(*arena->curr_mem_block) + mem_block_size);

    Dqn_ArenaMemBlock *result = nullptr;
    switch (arena->mem_provider)
    {
        case Dqn_ArenaMemProvider::CRT:
        {
            result = DQN_CAST(Dqn_ArenaMemBlock *)DQN_MALLOC(allocate_size);
        }
        break;

        case Dqn_ArenaMemProvider::Virtual:
        {
            result = DQN_CAST(Dqn_ArenaMemBlock *)VirtualAlloc(nullptr, allocate_size, MEM_RESERVE, PAGE_READWRITE);
        }
        break;

        case Dqn_ArenaMemProvider::UserMemory:
        break;
    }

    if (!result) return result;

    *result        = {};
    result->size   = mem_block_size;
    result->memory = DQN_CAST(Dqn_u8 *)result + sizeof(*result);
    return result;
}

DQN_API void Dqn__ArenaFreeBlock(Dqn_Arena *arena, Dqn_ArenaMemBlock *block)
{
    if (!block)
        return;

    if (block->next)
        block->next->prev = block->prev;

    if (block->prev)
        block->prev->next = block->next;

    arena->current_stats.block_count--;
    arena->current_stats.bytes_allocated -= block->size;

    switch (arena->mem_provider)
    {
        case Dqn_ArenaMemProvider::CRT: DQN_FREE(block); break;
        case Dqn_ArenaMemProvider::Virtual: VirtualFree(block, block->size, MEM_RELEASE);
        case Dqn_ArenaMemProvider::UserMemory:
        break;
    }
}

DQN_API void Dqn__ArenaAttachBlock(Dqn_Arena *arena, Dqn_ArenaMemBlock *new_block)
{
    if (arena->top_mem_block)
    {
        DQN_ASSERT(arena->top_mem_block->next == nullptr);
        arena->top_mem_block->next = new_block;
        new_block->prev            = arena->top_mem_block;
        arena->top_mem_block       = new_block;
    }
    else
    {
        arena->top_mem_block  = new_block;
        arena->curr_mem_block = new_block;
    }

    arena->current_stats.block_count++;
    arena->highest_stats.block_count = DQN_MAX(arena->highest_stats.block_count, arena->current_stats.block_count);

    arena->current_stats.bytes_allocated += arena->curr_mem_block->size;
    arena->highest_stats.bytes_allocated = DQN_MAX(arena->highest_stats.bytes_allocated, arena->current_stats.bytes_allocated);
}

DQN_API Dqn_Arena Dqn_ArenaInitWithCRT(Dqn_isize size DQN_CALL_SITE_ARGS)
{
    Dqn_Arena result = {};
    result.mem_provider       = Dqn_ArenaMemProvider::CRT;
    if (size > 0)
    {
        DQN_ASSERT_MSG(size >= DQN_ISIZEOF(*result.curr_mem_block), "(%I64u >= %I64u) There needs to be enough space to encode the Dqn_ArenaMemBlock struct into the memory buffer", size, sizeof(*result.curr_mem_block));
        Dqn_ArenaMemBlock *mem_block = Dqn__ArenaAllocateBlock(&result, size DQN_CALL_SITE_ARGS_INPUT);
        Dqn__ArenaAttachBlock(&result, mem_block);
    }

    return result;
}

DQN_API Dqn_Arena Dqn_ArenaInitWithMemory(void *memory, Dqn_isize size)
{
    Dqn_Arena result = {};
    result.mem_provider       = Dqn_ArenaMemProvider::UserMemory;
    if (size > 0)
    {
        DQN_ASSERT_MSG(size >= DQN_ISIZEOF(*result.curr_mem_block), "(%I64u >= %I64u) There needs to be enough space to encode the Dqn_ArenaMemBlock struct into the memory buffer", size, sizeof(*result.curr_mem_block));
        auto *mem_block     = DQN_CAST(Dqn_ArenaMemBlock *) memory;
        *mem_block          = {};
        mem_block->memory   = DQN_CAST(Dqn_u8 *) memory + sizeof(*mem_block);
        mem_block->size     = size - DQN_CAST(Dqn_isize)sizeof(*mem_block);
        Dqn__ArenaAttachBlock(&result, mem_block);
    }
    return result;
}

DQN_API void Dqn_ArenaFree(Dqn_Arena *arena)
{
    for (Dqn_ArenaMemBlock *mem_block = arena->top_mem_block; mem_block;)
    {
        Dqn_ArenaMemBlock *block_to_free = mem_block;
        mem_block                        = block_to_free->prev;
        Dqn__ArenaFreeBlock(arena, block_to_free);
    }

    // NOTE: Copy
    Dqn_ArenaMemProvider mem_provider  = arena->mem_provider;
    Dqn_ArenaStats       highest_stats = arena->highest_stats;

    // NOTE: Reset and restore persistent information
    *arena               = {};
    arena->highest_stats = highest_stats;
    arena->mem_provider  = mem_provider;
}

DQN_API Dqn_b32 Dqn_ArenaReserve(Dqn_Arena *arena, Dqn_isize size DQN_CALL_SITE_ARGS)
{
    if (arena->top_mem_block)
    {
        Dqn_isize remaining_space = arena->top_mem_block->size - arena->top_mem_block->used;
        if (remaining_space >= size) return true;
    }

    Dqn_ArenaMemBlock *new_block = Dqn__ArenaAllocateBlock(arena, size DQN_CALL_SITE_ARGS_INPUT);
    if (!new_block) return false;
    Dqn__ArenaAttachBlock(arena, new_block);
    return true;
}

DQN_API void Dqn_ArenaResetUsage(Dqn_Arena *arena, Dqn_ZeroMem zero_mem)
{
    for (Dqn_ArenaMemBlock *block = arena->top_mem_block; block; block = block->prev)
    {
        if (!block->prev)
            arena->curr_mem_block = block;

        Dqn__ZeroMemBytes(block->memory, DQN_CAST(size_t)block->used, zero_mem);
        block->used = 0;
    }

    arena->current_stats.bytes_used   = 0;
    arena->current_stats.bytes_wasted = 0;
}

DQN_API Dqn_ArenaScopeData Dqn_ArenaBeginScope(Dqn_Arena *arena)
{
    Dqn_ArenaScopeData result = {};
    result.arena              = arena;
    result.curr_mem_block     = arena->curr_mem_block;
    result.top_mem_block      = arena->top_mem_block;

    result.curr_mem_block_used = (arena->curr_mem_block) ? arena->curr_mem_block->used : 0;
    result.current_stats       = arena->current_stats;
    return result;
}

DQN_API void Dqn_ArenaEndScope(Dqn_ArenaScopeData scope)
{
    // Revert the top and current memory block until we hit the one we were at
    // when the scope was started.
    Dqn_Arena *arena = scope.arena;
    while (arena->top_mem_block != scope.top_mem_block)
    {
        Dqn_ArenaMemBlock *block_to_free = arena->top_mem_block;
        if (arena->curr_mem_block == block_to_free)
            arena->curr_mem_block = block_to_free->prev;
        arena->top_mem_block = block_to_free->prev;
        Dqn__ArenaFreeBlock(arena, block_to_free);
    }

    // All the blocks between the top memory block and the current memory block
    // need to be zeroed
    for (Dqn_ArenaMemBlock *mem_block = arena->top_mem_block; mem_block != scope.curr_mem_block; mem_block = mem_block->prev)
        mem_block->used = 0;

    // Restore the current memory block's usage state and then restore the
    // stats. Note that we persist the highest used count as this is useful for
    // checking over/under-utilization of the arena throughout it's lifetime.
    if (arena->curr_mem_block)
        arena->curr_mem_block->used = scope.curr_mem_block_used;

    arena->current_stats = scope.current_stats;
}

Dqn_ArenaScope::Dqn_ArenaScope(Dqn_Arena *arena)
{
    this->arena = arena;
    this->region = Dqn_ArenaBeginScope(arena);
}

Dqn_ArenaScope::~Dqn_ArenaScope()
{
    Dqn_ArenaEndScope(this->region);
}

DQN_API void *Dqn__ArenaAllocate(Dqn_Arena *arena, Dqn_isize size, Dqn_u8 alignment, Dqn_ZeroMem zero_mem DQN_CALL_SITE_ARGS)
{
    // Check for sufficient space in the arena
    Dqn_isize allocation_size  = size + (alignment - 1);
    Dqn_b32 need_new_mem_block = true;
    for (Dqn_ArenaMemBlock *mem_block = arena->curr_mem_block; mem_block; mem_block = mem_block->next)
    {
        Dqn_b32 can_fit_in_block = (mem_block->used + allocation_size) <= mem_block->size;
        if (can_fit_in_block)
        {
            arena->curr_mem_block = mem_block;
            need_new_mem_block    = false;
            break;
        }
    }

    if (need_new_mem_block)
    {
        Dqn_ArenaMemBlock *new_block = Dqn__ArenaAllocateBlock(arena, allocation_size DQN_CALL_SITE_ARGS_INPUT);
        if (!new_block) return nullptr;

        Dqn__ArenaAttachBlock(arena, new_block);
        arena->curr_mem_block = arena->top_mem_block;
    }

    // Divvy out the pointer for the user
    Dqn_uintptr address = DQN_CAST(Dqn_uintptr) arena->curr_mem_block->memory + arena->curr_mem_block->used;
    void *result        = DQN_CAST(void *) Dqn_AlignAddress(address, alignment);

    // Prepare the pointer for use
    DQN_ASSERT(arena->curr_mem_block->used <= arena->curr_mem_block->size);
    Dqn__ZeroMemBytes(DQN_CAST(void *)address, allocation_size, zero_mem);
    Dqn_AllocationTracerAdd(arena->tracer, DQN_CAST(void *)address, allocation_size DQN_CALL_SITE_ARGS_INPUT);

    // Update the block and arena stats
    arena->curr_mem_block->used += allocation_size;
    arena->current_stats.bytes_used += allocation_size;
    arena->highest_stats.bytes_used = DQN_MAX(arena->highest_stats.bytes_used, arena->current_stats.bytes_used);
    return result;
}

DQN_API void Dqn_ArenaLogStats(Dqn_Arena const *arena)
{
    Dqn_ArenaStatsString highest = Dqn_ArenaStatsToString(&arena->highest_stats);
    Dqn_ArenaStatsString current = Dqn_ArenaStatsToString(&arena->current_stats);
    DQN_LOG_M("HIGH %.*s\nCURR %.*s\n", highest.size, highest.str, current.size, current.str);
}

#if defined(DQN_WITH_MATH)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_V2 Implementation
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_V2I Dqn_V2ToV2I(Dqn_V2 a)
{
    Dqn_V2I result = Dqn_V2I(DQN_CAST(Dqn_i32)a.x, DQN_CAST(Dqn_i32)a.y);
    return result;
}

DQN_API Dqn_V2 Dqn_V2Min(Dqn_V2 a, Dqn_V2 b)
{
    Dqn_V2 result = Dqn_V2(DQN_MIN(a.x, b.x), DQN_MIN(a.y, b.y));
    return result;
}

DQN_API Dqn_V2 Dqn_V2Max(Dqn_V2 a, Dqn_V2 b)
{
    Dqn_V2 result = Dqn_V2(DQN_MAX(a.x, b.x), DQN_MAX(a.y, b.y));
    return result;
}

DQN_API Dqn_V2 Dqn_V2Abs(Dqn_V2 a)
{
    Dqn_V2 result = Dqn_V2(DQN_ABS(a.x), DQN_ABS(a.y));
    return result;
}

DQN_API Dqn_f32 Dqn_V2Dot(Dqn_V2 a, Dqn_V2 b)
{
    Dqn_f32 result = (a.x * b.x) + (a.y * b.y);
    return result;
}

DQN_API Dqn_f32 Dqn_V2LengthSq(Dqn_V2 a, Dqn_V2 b)
{
    Dqn_f32 x_side = b.x - a.x;
    Dqn_f32 y_side = b.y - a.y;
    Dqn_f32 result = DQN_SQUARED(x_side) + DQN_SQUARED(y_side);
    return result;
}

DQN_API Dqn_V2 Dqn_V2Normalise(Dqn_V2 a)
{
    Dqn_f32 length_sq = DQN_SQUARED(a.x) + DQN_SQUARED(a.y);
    Dqn_f32 length    = DQN_SQRTF(length_sq);
    Dqn_V2 result     = a / length;
    return result;
}

DQN_API Dqn_V2 Dqn_V2Perpendicular(Dqn_V2 a)
{
    Dqn_V2 result = Dqn_V2(-a.y, a.x);
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_V3 Implementation
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_f32 Dqn_V3LengthSq(Dqn_V3 a)
{
    Dqn_f32 result = DQN_SQUARED(a.x) + DQN_SQUARED(a.y) + DQN_SQUARED(a.z);
    return result;
}

DQN_API Dqn_f32 Dqn_V3Length(Dqn_V3 a)
{
    Dqn_f32 length_sq = DQN_SQUARED(a.x) + DQN_SQUARED(a.y) + DQN_SQUARED(a.z);
    Dqn_f32 result    = DQN_SQRTF(length_sq);
    return result;
}

DQN_API Dqn_V3 Dqn_V3Normalise(Dqn_V3 a)
{
    Dqn_f32 length = Dqn_V3Length(a);
    Dqn_V3  result = a / length;
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_V4 Implementation
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_f32 Dqn_V4Dot(Dqn_V4 a, Dqn_V4 b)
{
    Dqn_f32 result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_M4 Implementation
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_M4 Dqn_M4Identity()
{
    Dqn_M4 result =
    {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4ScaleF(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z)
{
    Dqn_M4 result =
    {{
        {x, 0, 0, 0},
        {0, y, 0, 0},
        {0, 0, z, 0},
        {0, 0, 0, 1},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4Scale(Dqn_V3 xyz)
{
    Dqn_M4 result =
    {{
        {xyz.x, 0,     0,     0},
        {0,     xyz.y, 0,     0},
        {0,     0,     xyz.z, 0},
        {0,     0,     0,     1},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4TranslateF(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z)
{
    Dqn_M4 result =
    {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {x, y, z, 1},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4Translate(Dqn_V3 xyz)
{
    Dqn_M4 result =
    {{
        {1,     0,     0,     0},
        {0,     1,     0,     0},
        {0,     0,     1,     0},
        {xyz.x, xyz.y, xyz.z, 1},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4Transpose(Dqn_M4 mat)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
            result.columns[col][row] = mat.columns[row][col];
    return result;
}

DQN_API Dqn_M4 Dqn_M4Rotate(Dqn_V3 axis01, Dqn_f32 radians)
{
    DQN_ASSERT_MSG(DQN_ABS(Dqn_V3Length(axis01) - 1.f) <= 0.01f,
                   "Rotation axis must be normalised, length = %f",
                   Dqn_V3Length(axis01));

    Dqn_f32 sin           = DQN_SINF(radians);
    Dqn_f32 cos           = DQN_COSF(radians);
    Dqn_f32 one_minus_cos = 1.f - cos;

    Dqn_f32 x  = axis01.x;
    Dqn_f32 y  = axis01.y;
    Dqn_f32 z  = axis01.z;
    Dqn_f32 x2 = DQN_SQUARED(x);
    Dqn_f32 y2 = DQN_SQUARED(y);
    Dqn_f32 z2 = DQN_SQUARED(z);

    Dqn_M4 result =
    {{
        {cos + x2*one_minus_cos,    y*x*one_minus_cos + z*sin, z*x*one_minus_cos - y*sin, 0}, // Col 1
        {x*y*one_minus_cos - z*sin, cos + y2*one_minus_cos,    z*y*one_minus_cos + x*sin, 0}, // Col 2
        {x*z*one_minus_cos + y*sin, y*z*one_minus_cos - x*sin, cos + z2*one_minus_cos,    0}, // Col 3
        {0,                         0,                         0,                         1}, // Col 4
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4Orthographic(Dqn_f32 left, Dqn_f32 right, Dqn_f32 bottom, Dqn_f32 top, Dqn_f32 z_near, Dqn_f32 z_far)
{
    Dqn_M4 result =
    {{
         {2.f / (right - left),                     0.f,                                      0.f,                                          0.f},
         {0.f,                                      2.f / (top - bottom),                     0.f,                                          0.f},
         {0.f,                                      0.f,                                      -2.f / (z_far - z_near),                      0.f},
         {(-1.f * (right + left)) / (right - left), (-1.f * (top + bottom)) / (top - bottom), (-1.f * (z_far + z_near)) / (z_far - z_near), 1.f},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4Perspective(Dqn_f32 fov /*radians*/, Dqn_f32 aspect, Dqn_f32 z_near, Dqn_f32 z_far)
{
    Dqn_f32 tan_fov = DQN_TANF(fov / 2.f);
    Dqn_M4 result =
    {{
         {1.f / (aspect * tan_fov), 0.f,           0.f,                                     0.f},
         {0,                        1.f / tan_fov, 0.f,                                     0.f},
         {0.f,                      0.f,           (z_near + z_far) / (z_near - z_far),    -1.f},
         {0.f,                      0.f,           (2.f * z_near * z_far)/(z_near - z_far), 0.f},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4Add(Dqn_M4 lhs, Dqn_M4 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] + rhs.columns[col][it];
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4Sub(Dqn_M4 lhs, Dqn_M4 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] - rhs.columns[col][it];
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4Mul(Dqn_M4 lhs, Dqn_M4 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            Dqn_f32 sum = 0;
            for (int f32_it = 0; f32_it < 4; f32_it++)
                sum += lhs.columns[f32_it][row] * rhs.columns[col][f32_it];

            result.columns[col][row] = sum;
        }
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4Div(Dqn_M4 lhs, Dqn_M4 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] / rhs.columns[col][it];
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4AddF(Dqn_M4 lhs, Dqn_f32 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] + rhs;
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4SubF(Dqn_M4 lhs, Dqn_f32 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] - rhs;
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4MulF(Dqn_M4 lhs, Dqn_f32 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] * rhs;
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4DivF(Dqn_M4 lhs, Dqn_f32 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] / rhs;
    }
    return result;
}

#if defined(DQN_WITH_FIXED_STRING)
DQN_API Dqn_FixedString<256> Dqn_M4ColumnMajorString(Dqn_M4 mat)
{
    Dqn_FixedString<256> result = {};
    for (int row = 0; row < 4; row++)
    {
        for (int it = 0; it < 4; it++)
        {
            if (it == 0) Dqn_FixedStringAppend(&result, "|");
            Dqn_FixedStringAppendFmt(&result, "%.5f", mat.columns[it][row]);
            if (it != 3) Dqn_FixedStringAppend(&result, ", ");
            else         Dqn_FixedStringAppend(&result, "|\n");
        }
    }

    return result;
}
#endif

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Rect
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_Rect Dqn_RectInitFromPosAndSize(Dqn_V2 pos, Dqn_V2 size)
{
    Dqn_Rect result = {};
    result.min      = pos;
    if (size.x < 0) result.min.x -= size.x;
    if (size.y < 0) result.min.y -= size.y;
    result.max  = result.min + Dqn_V2Abs(size);
    return result;
}

DQN_API Dqn_V2 Dqn_RectCenter(Dqn_Rect rect)
{
    Dqn_V2 size   = rect.max - rect.min;
    Dqn_V2 result = rect.min + (size * 0.5f);
    return result;
}

DQN_API Dqn_b32 Dqn_RectContainsPoint(Dqn_Rect rect, Dqn_V2 p)
{
    Dqn_b32 result = (p.x >= rect.min.x && p.x <= rect.max.x && p.y >= rect.min.y && p.y <= rect.max.y);
    return result;
}

DQN_API Dqn_b32 Dqn_RectContainsRect(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_b32 result = (b.min >= a.min && b.max <= a.max);
    return result;
}

DQN_API Dqn_V2 Dqn_RectSize(Dqn_Rect rect)
{
    Dqn_V2 result = rect.max - rect.min;
    return result;
}

DQN_API Dqn_Rect Dqn_RectMove(Dqn_Rect src, Dqn_V2 move_amount)
{
    Dqn_Rect result = src;
    result.min += move_amount;
    result.max += move_amount;
    return result;
}

DQN_API Dqn_Rect Dqn_RectMoveTo(Dqn_Rect src, Dqn_V2 dest)
{
    Dqn_V2 move_amount = dest - src.min;
    Dqn_Rect result    = src;
    result.min += move_amount;
    result.max += move_amount;
    return result;
}

DQN_API Dqn_b32 Dqn_RectIntersects(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_b32 result = (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
                     (a.min.y <= b.max.y && a.max.y >= b.min.y);
    return result;
}

DQN_API Dqn_Rect Dqn_RectIntersection(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_Rect result  = {};
    if (Dqn_RectIntersects(a, b))
    {
        result.min.x = DQN_MAX(a.min.x, b.min.x);
        result.min.y = DQN_MAX(a.min.y, b.min.y);
        result.max.x = DQN_MIN(a.max.x, b.max.x);
        result.max.y = DQN_MIN(a.max.y, b.max.y);
    }

    return result;
}

DQN_API Dqn_Rect Dqn_RectUnion(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_Rect result  = {};
    result.min.x = DQN_MIN(a.min.x, b.min.x);
    result.min.y = DQN_MIN(a.min.y, b.min.y);
    result.max.x = DQN_MAX(a.max.x, b.max.x);
    result.max.y = DQN_MAX(a.max.y, b.max.y);
    return result;
}

DQN_API Dqn_Rect Dqn_RectFromRectI32(Dqn_RectI32 a)
{
    Dqn_Rect result = Dqn_Rect(a.min, a.max);
    return result;
}

DQN_API Dqn_V2I Dqn_RectI32_Size(Dqn_RectI32 rect)
{
    Dqn_V2I result = rect.max - rect.min;
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Math Utils
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_V2 Dqn_LerpV2(Dqn_V2 a, Dqn_f32 t, Dqn_V2 b)
{
    Dqn_V2 result = {};
    result.x  = a.x + ((b.x - a.x) * t);
    result.y  = a.y + ((b.y - a.y) * t);
    return result;
}

DQN_API Dqn_f32 Dqn_LerpF32(Dqn_f32 a, Dqn_f32 t, Dqn_f32 b)
{
    Dqn_f32 result = a + ((b - a) * t);
    return result;
}
#endif // DQN_WITH_MATH

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Bit
// -------------------------------------------------------------------------------------------------
DQN_API void Dqn_BitUnsetInplace(Dqn_u64 *flags, Dqn_u64 bitfield)
{
    *flags = (*flags & ~bitfield);
}

DQN_API void Dqn_BitSetInplace(Dqn_u64 *flags, Dqn_u64 bitfield)
{
    *flags = (*flags | bitfield);
}

DQN_API Dqn_b32 Dqn_BitIsSet(Dqn_u64 bits, Dqn_u64 bits_to_set)
{
    auto result = DQN_CAST(Dqn_b32)((bits & bits_to_set) == bits_to_set);
    return result;
}

DQN_API Dqn_b32 Dqn_BitIsNotSet(Dqn_u64 bits, Dqn_u64 bits_to_check)
{
    auto result = !Dqn_BitIsSet(bits, bits_to_check);
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Safe Arithmetic
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_i64 Dqn_SafeAddI64(Dqn_i64 a, Dqn_i64 b)
{
    DQN_ASSERT_MSG(a <= DQN_I64_MAX - b, "%I64d <= %I64d", a, DQN_I64_MAX - b);
    Dqn_i64 result = (a <= DQN_I64_MAX - b) ? (a + b) : DQN_I64_MAX;
    return result;
}

DQN_API Dqn_i64 Dqn_SafeMulI64(Dqn_i64 a, Dqn_i64 b)
{
    DQN_ASSERT_MSG(a <= DQN_I64_MAX / b , "%I64d <= %I64d", a, DQN_I64_MAX / b);
    Dqn_i64 result = (a <= DQN_I64_MAX / b) ? (a * b) : DQN_I64_MAX;
    return result;
}

DQN_API Dqn_u64 Dqn_SafeAddU64(Dqn_u64 a, Dqn_u64 b)
{
    DQN_ASSERT_MSG(a <= DQN_U64_MAX - b, "%I64u <= %I64u", a, DQN_U64_MAX - b);
    Dqn_u64 result = (a <= DQN_U64_MAX - b) ? (a + b) : DQN_U64_MAX;
    return result;
}

DQN_API Dqn_u64 Dqn_SafeSubU64(Dqn_u64 a, Dqn_u64 b)
{
    DQN_ASSERT_MSG(a >= b, "%I64u >= %I64u", a, b);
    Dqn_u64 result = (a >= b) ? (a - b) : 0;
    return result;
}

DQN_API Dqn_u32 Dqn_SafeSubU32(Dqn_u32 a, Dqn_u32 b)
{
    DQN_ASSERT_MSG(a >= b, "%I32u >= %I32u", a, b);
    Dqn_u32 result = (a >= b) ? (a - b) : 0;
    return result;
}

DQN_API Dqn_u64 Dqn_SafeMulU64(Dqn_u64 a, Dqn_u64 b)
{
    DQN_ASSERT_MSG(a <= DQN_U64_MAX / b , "%I64u <= %I64u", a, DQN_U64_MAX / b);
    Dqn_u64 result = (a <= DQN_U64_MAX / b) ? (a * b) : DQN_U64_MAX;
    return result;
}

DQN_API int Dqn_SafeTruncateISizeToInt(Dqn_isize val)
{
    DQN_ASSERT_MSG(val >= DQN_I32_MIN && val <= DQN_I32_MAX, "%zd >= %zd && %zd <= %zd", val, DQN_I32_MAX, val, DQN_I32_MAX);
    auto result = (val >= DQN_I32_MIN && val <= DQN_I32_MAX) ? DQN_CAST(int)val : 0;
    return result;
}

DQN_API Dqn_i32 Dqn_SafeTruncateISizeToI32(Dqn_isize val)
{
    DQN_ASSERT_MSG(val >= DQN_I32_MIN && val <= DQN_I32_MAX, "%zd >= %zd && %zd <= %zd", val, DQN_I32_MIN, val, DQN_I32_MAX);
    auto result = (val >= DQN_I32_MIN && val <= DQN_I32_MAX) ? DQN_CAST(Dqn_i32)val : 0;
    return result;
}

DQN_API Dqn_u32 Dqn_SafeTruncateUSizeToU32(Dqn_usize val)
{
    DQN_ASSERT_MSG(val <= DQN_U32_MAX, "%zu <= %zu", val, DQN_U32_MAX);
    auto result = (val <= DQN_U32_MAX) ? DQN_CAST(Dqn_u32)val : DQN_U32_MAX;
    return result;
}

DQN_API Dqn_i32 Dqn_SafeTruncateUSizeToI32(Dqn_usize val)
{
    DQN_ASSERT_MSG(val <= DQN_I32_MAX, "%zu <= %zu", val, DQN_I32_MAX);
    auto result = (val <= DQN_I32_MAX) ? DQN_CAST(int)val : DQN_I32_MAX;
    return result;
}

DQN_API int Dqn_SafeTruncateUSizeToInt(Dqn_usize val)
{
    DQN_ASSERT_MSG(val <= DQN_I32_MAX, "%zu <= %zu", val, DQN_I32_MAX);
    auto result = (val <= DQN_I32_MAX) ? DQN_CAST(int)val : DQN_I32_MAX;
    return result;
}

DQN_API Dqn_isize Dqn_SafeTruncateUSizeToISize(Dqn_usize val)
{
    DQN_ASSERT_MSG(val <= DQN_ISIZE_MAX, "%zu <= %zu", val, DQN_CAST(Dqn_usize)DQN_ISIZE_MAX);
    auto result = (val <= DQN_ISIZE_MAX) ? DQN_CAST(Dqn_isize)val : DQN_ISIZE_MAX;
    return result;
}

DQN_API Dqn_u32 Dqn_SafeTruncateU64ToU32(Dqn_u64 val)
{
    DQN_ASSERT_MSG(val <= DQN_U32_MAX, "%I64u <= %I64u", val, DQN_U32_MAX);
    auto result = (val <= DQN_U32_MAX) ? DQN_CAST(Dqn_u32)val : DQN_U32_MAX;
    return result;
}

DQN_API Dqn_u16 Dqn_SafeTruncateU64ToU16(Dqn_u64 val)
{
    DQN_ASSERT_MSG(val <= DQN_U16_MAX, "%I64u <= %I64u", val, DQN_U16_MAX);
    auto result = (val <= DQN_U16_MAX) ? DQN_CAST(Dqn_u16)val : DQN_U16_MAX;
    return result;
}

DQN_API Dqn_u8 Dqn_SafeTruncateU64ToU8(Dqn_u64 val)
{
    DQN_ASSERT_MSG(val <= DQN_U8_MAX, "%I64u <= %I64u", val, DQN_U8_MAX);
    auto result = (val <= DQN_U8_MAX) ? DQN_CAST(Dqn_u8)val : DQN_U8_MAX;
    return result;
}

DQN_API Dqn_i64 Dqn_SafeTruncateU64ToI64(Dqn_u64 val)
{
    DQN_ASSERT_MSG(val <= DQN_I64_MAX, "%I64u <= %I64d", val, DQN_I64_MAX);
    auto result = (val <= DQN_I64_MAX) ? DQN_CAST(Dqn_i64)val : DQN_I64_MAX;
    return result;
}


DQN_API Dqn_i32 Dqn_SafeTruncateU64ToI32(Dqn_u64 val)
{
    DQN_ASSERT_MSG(val <= DQN_I32_MAX, "%I64u <= %I64d", val, DQN_I32_MAX);
    auto result = (val <= DQN_I32_MAX) ? DQN_CAST(Dqn_i32)val : DQN_I32_MAX;
    return result;
}

DQN_API Dqn_i16 Dqn_SafeTruncateU64ToI16(Dqn_u64 val)
{
    DQN_ASSERT_MSG(val <= DQN_I16_MAX, "%I64u <= %I64d", val, DQN_I16_MAX);
    auto result = (val <= DQN_I16_MAX) ? DQN_CAST(Dqn_i16)val : DQN_I16_MAX;
    return result;
}

DQN_API Dqn_i8 Dqn_SafeTruncateU64ToI8(Dqn_u64 val)
{
    DQN_ASSERT_MSG(val <= DQN_I8_MAX, "%I64u <= %I64d", val, DQN_I8_MAX);
    auto result = (val <= DQN_I8_MAX) ? DQN_CAST(Dqn_i8)val : DQN_I8_MAX;
    return result;
}

DQN_API int Dqn_SafeTruncateU64ToInt(Dqn_u64 val)
{
    DQN_ASSERT_MSG(val <= DQN_I32_MAX, "%I64u <= %I64d", val, DQN_I32_MAX);
    auto result = (val <= DQN_I32_MAX) ? DQN_CAST(int)val : DQN_I32_MAX;
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Char
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_b32 Dqn_CharIsAlpha(char ch)
{
    Dqn_b32 result = (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
    return result;
}

DQN_API Dqn_b32 Dqn_CharIsDigit(char ch)
{
    Dqn_b32 result = (ch >= '0' && ch <= '9');
    return result;
}

DQN_API Dqn_b32 Dqn_CharIsAlphaNum(char ch)
{
    Dqn_b32 result = Dqn_CharIsAlpha(ch) || Dqn_CharIsDigit(ch);
    return result;
}

DQN_API Dqn_b32 Dqn_CharIsWhitespace(char ch)
{
    Dqn_b32 result = (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
    return result;
}

DQN_API Dqn_b32 Dqn_CharIsHex(char ch)
{
    Dqn_b32 result = ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') || (ch >= '0' && ch <= '9'));
    return result;
}

DQN_API Dqn_u8 Dqn_CharHexToU8(char ch)
{
    DQN_ASSERT_MSG(Dqn_CharIsHex(ch), "Hex character not valid '%c'", ch);

    Dqn_u8 result = 0;
    if (ch >= 'a' && ch <= 'f')
        result = ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F')
        result = ch - 'A' + 10;
    else
        result = ch - '0';
    return result;
}

char constexpr DQN_HEX_LUT[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
DQN_API char Dqn_CharToHex(char ch)
{
    char result = DQN_CAST(char)-1;
    if (ch <= 16) result = DQN_HEX_LUT[DQN_CAST(unsigned)ch];
    return result;
}

DQN_API char Dqn_CharToHexUnchecked(char ch)
{
    char result = DQN_HEX_LUT[DQN_CAST(unsigned)ch];
    return result;
}

DQN_API char Dqn_CharToLower(char ch)
{
    char result = ch;
    if (result >= 'A' && result <= 'Z')
        result += 'a' - 'A';
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_UTF
// -------------------------------------------------------------------------------------------------
DQN_API int Dqn_UTF8EncodeCodepoint(Dqn_u8 utf8[4], Dqn_u32 codepoint)
{
    // NOTE: Table from https://www.reedbeta.com/blog/programmers-intro-to-unicode/
    // ----------------------------------------+----------------------------+--------------------+
    // UTF-8 (binary)                          | Code point (binary)        | Range              |
    // ----------------------------------------+----------------------------+--------------------+
    // 0xxx'xxxx                               |                   xxx'xxxx | U+0000  - U+007F   |
    // 110x'xxxx 10yy'yyyy                     |              xxx'xxyy'yyyy | U+0080  - U+07FF   |
    // 1110'xxxx 10yy'yyyy 10zz'zzzz           |        xxxx'yyyy'yyzz'zzzz | U+0800  - U+FFFF   |
    // 1111'0xxx 10yy'yyyy 10zz'zzzz 10ww'wwww | x'xxyy'yyyy'zzzz'zzww'wwww | U+10000 - U+10FFFF |
    // ----------------------------------------+----------------------------+--------------------+

    if (codepoint <= 0b0111'1111)
    {
        utf8[0] = DQN_CAST(Dqn_u8)codepoint;
        return 1;
    }

    if (codepoint <= 0b0111'1111'1111)
    {
        utf8[0] = (0b1100'0000 | ((codepoint >> 6) & 0b01'1111)); // x
        utf8[1] = (0b1000'0000 | ((codepoint >> 0) & 0b11'1111)); // y
        return 2;
    }

    if (codepoint <= 0b1111'1111'1111'1111)
    {
        utf8[0] = (0b1110'0000 | ((codepoint >> 12) & 0b00'1111)); // x
        utf8[1] = (0b1000'0000 | ((codepoint >> 6)  & 0b11'1111)); // y
        utf8[2] = (0b1000'0000 | ((codepoint >> 0)  & 0b11'1111)); // z
        return 3;
    }

    if (codepoint <= 0b1'1111'1111'1111'1111'1111)
    {
        utf8[0] = (0b1111'0000 | ((codepoint >> 18) & 0b00'0111)); // x
        utf8[1] = (0b1000'0000 | ((codepoint >> 12) & 0b11'1111)); // y
        utf8[2] = (0b1000'0000 | ((codepoint >> 6)  & 0b11'1111)); // z
        utf8[3] = (0b1000'0000 | ((codepoint >> 0)  & 0b11'1111)); // w
        return 4;
    }

    return 0;
}

DQN_API int Dqn_UTF16EncodeCodepoint(Dqn_u16 utf16[2], Dqn_u32 codepoint)
{
    // NOTE: Table from https://www.reedbeta.com/blog/programmers-intro-to-unicode/
    // ----------------------------------------+------------------------------------+------------------+
    // UTF-16 (binary)                         | Code point (binary)                | Range            |
    // ----------------------------------------+------------------------------------+------------------+
    // xxxx'xxxx'xxxx'xxxx                     | xxxx'xxxx'xxxx'xxxx                | U+0000–U+FFFF    |
    // 1101'10xx'xxxx'xxxx 1101'11yy'yyyy'yyyy | xxxx'xxxx'xxyy'yyyy'yyyy + 0x10000 | U+10000–U+10FFFF |
    // ----------------------------------------+------------------------------------+------------------+

    if (codepoint <= 0b1111'1111'1111'1111)
    {
        utf16[0] = DQN_CAST(Dqn_u16)codepoint;
        return 1;
    }

    if (codepoint <= 0b1111'1111'1111'1111'1111)
    {
        Dqn_u32 surrogate_codepoint = codepoint + 0x10000;
        utf16[0] = 0b1101'1000'0000'0000 | ((surrogate_codepoint >> 10) & 0b11'1111'1111); // x
        utf16[1] = 0b1101'1100'0000'0000 | ((surrogate_codepoint >>  0) & 0b11'1111'1111); // y
        return 2;
    }

    return 0;
}

#if defined(DQN_WITH_HEX)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Hex
// -------------------------------------------------------------------------------------------------
DQN_API char const *Dqn_HexCStringTrimSpaceAnd0xPrefix(char const *hex, Dqn_isize size, Dqn_isize *real_size)
{
    Dqn_isize   trimmed_size = 0;
    char const *trimmed_hex  = Dqn_CStringTrimWhitespaceAround(hex, size, &trimmed_size);
    char const *result       = Dqn_CStringTrimPrefix(trimmed_hex,
                                                     trimmed_size,
                                                     "0x",
                                                     2 /*prefix_size*/,
                                                     &trimmed_size);
    if (real_size) *real_size = trimmed_size;
    return result;
}

DQN_API Dqn_String Dqn_HexStringTrimSpaceAnd0xPrefix(Dqn_String const string)
{
    Dqn_isize trimmed_size = 0;
    char const *trimmed    = Dqn_HexCStringTrimSpaceAnd0xPrefix(string.str, string.size, &trimmed_size);
    Dqn_String result      = Dqn_StringInit(trimmed, trimmed_size);
    return result;
}

DQN_API Dqn_u64 Dqn_HexCStringToU64(char const *hex, Dqn_isize size)
{
    Dqn_isize   trim_size = size;
    char const *trim_hex  = hex;
    if (trim_size >= 2)
    {
        if (trim_hex[0] == '0' && (trim_hex[1] == 'x' || trim_hex[1] == 'X'))
        {
            trim_size -= 2;
            trim_hex += 2;
        }
    }

    DQN_ASSERT(DQN_CAST(Dqn_usize)(trim_size * 4 / 8) /*maximum amount of bytes represented in the hex string*/ <= sizeof(Dqn_u64));

    Dqn_u64   result    = 0;
    Dqn_usize bits_written = 0;
    for (Dqn_isize hex_index = 0; hex_index < size; hex_index++, bits_written += 4)
    {
        char ch = trim_hex[hex_index];
        if (!Dqn_CharIsHex(ch)) break;
        Dqn_u8 val = Dqn_CharHexToU8(ch);
        Dqn_usize bit_shift = 60 - bits_written;
        result |= (DQN_CAST(Dqn_u64)val << bit_shift);
    }

    result >>= (64 - bits_written); // Shift the remainder digits to the end.
    return result;
}

DQN_API Dqn_u64 Dqn_HexStringToU64(Dqn_String hex)
{
    Dqn_u64 result = Dqn_HexCStringToU64(hex.str, hex.size);
    return result;
}

DQN_API void Dqn_HexBytesToHex(void const *src, int src_size, char *dest, int dest_size)
{
    (void)src_size; (void)dest_size;
    DQN_ASSERT(dest_size >= src_size * 2);

    unsigned char *src_u8 = DQN_CAST(unsigned char *)src;
    for (int src_index = 0, dest_index = 0;
         src_index < src_size;
         src_index += 1, dest_index += 2)
    {
        char byte            = src_u8[src_index];
        char hex01           = (byte >> 4) & 0b1111;
        char hex02           = (byte >> 0) & 0b1111;
        dest[dest_index + 0] = hex01 < 10 ? (hex01 + '0') : (hex01 - 10) + 'a';
        dest[dest_index + 1] = hex02 < 10 ? (hex02 + '0') : (hex02 - 10) + 'a';
    }
}

DQN_API void Dqn_HexToBytes(char const *hex, int hex_size, void *dest, int dest_size)
{
    (void)hex_size; (void)dest_size;
    DQN_ASSERT(dest_size >= DQN_CAST(int)((hex_size / 2.f) + .5f));

    unsigned char *dest_u8 = DQN_CAST(unsigned char *)dest;
    for (int hex_index = 0, dest_index = 0;
         hex_index < hex_size;
         hex_index += 2, dest_index += 1)
    {
        char hex01 = hex[hex_index];
        char hex02 = (hex_index + 1 < hex_size) ? hex[hex_index + 1] : 0;

        char bit4_01 =   (hex01 >= '0' && hex01 <= '9') ?  0 + (hex01 - '0')
                       : (hex01 >= 'a' && hex01 <= 'f') ? 10 + (hex01 - 'a')
                       : (hex01 >= 'A' && hex01 <= 'F') ? 10 + (hex01 - 'A')
                       : 0;

        char bit4_02 =   (hex02 >= '0' && hex02 <= '9') ?  0 + (hex02 - '0')
                       : (hex02 >= 'a' && hex02 <= 'f') ? 10 + (hex02 - 'a')
                       : (hex02 >= 'A' && hex02 <= 'F') ? 10 + (hex02 - 'A')
                       : 0;

        char byte = (bit4_01 << 4) | (bit4_02 << 0);
        dest_u8[dest_index] = byte;
    }
}

DQN_API char *Dqn_HexBytesToHexCStringArena(void const *bytes, Dqn_isize size, Dqn_Arena *arena)
{
    char *result = size > 0 ? Dqn_ArenaNewArray(arena, char, (size * 2) + 1 /*null terminate*/, Dqn_ZeroMem::No) : nullptr;
    if (result)
    {
        Dqn_HexBytesToHex(bytes, size, result, size * 2);
        result[size * 2] = 0;
    }

    return result;
}

DQN_API Dqn_String Dqn_HexBytesToHexStringArena(void const *bytes, Dqn_isize size, Dqn_Arena *arena)
{
    char *c_string = Dqn_HexBytesToHexCStringArena(bytes, size, arena);
    Dqn_String result = {};
    if (c_string)
        result = Dqn_StringInit(c_string, size * 2);
    return result;
}

DQN_API char *Dqn_HexU8ArrayToHexCStringArena(Dqn_Array<Dqn_u8> const bytes, Dqn_Arena *arena)
{
    char *result = Dqn_HexBytesToHexCStringArena(bytes.data, bytes.size, arena);
    return result;
}

DQN_API Dqn_String Dqn_HexU8ArrayToHexStringArena(Dqn_Array<Dqn_u8> const bytes, Dqn_Arena *arena)
{
    Dqn_String result = Dqn_HexBytesToHexStringArena(bytes.data, bytes.size, arena);
    return result;
}

DQN_API Dqn_u8 *Dqn_HexCStringToU8Bytes(char const *hex, Dqn_isize size, Dqn_isize *real_size, Dqn_Arena *arena)
{
    hex = Dqn_HexCStringTrimSpaceAnd0xPrefix(hex, size, &size);
    Dqn_isize binary_size = DQN_CAST(Dqn_isize)(size / 2.f + .5f);
    Dqn_u8 *result = Dqn_ArenaNewArray(arena, Dqn_u8, binary_size, Dqn_ZeroMem::No);
    if (result)
        Dqn_HexToBytes(hex, size, result, binary_size);

    if (real_size) *real_size = binary_size;
    return result;
}

DQN_API Dqn_Array<Dqn_u8> Dqn_HexCStringToU8Array(char const *hex, Dqn_isize size, Dqn_Arena *arena)
{
    Dqn_isize data_size = 0;
    auto *data          = DQN_CAST(Dqn_u8 *)Dqn_HexCStringToU8Bytes(hex, size, &data_size, arena);
    Dqn_Array<Dqn_u8> result = Dqn_ArrayInitWithMemory(data, data_size, data_size);
    return result;
}

DQN_API Dqn_Array<Dqn_u8> Dqn_HexStringToU8Array(Dqn_String const hex, Dqn_Arena *arena)
{
    Dqn_isize data_size      = 0;
    auto *data               = DQN_CAST(Dqn_u8 *) Dqn_HexCStringToU8Bytes(hex.str, hex.size, &data_size, arena);
    Dqn_Array<Dqn_u8> result = Dqn_ArrayInitWithMemory(data, data_size, data_size);
    return result;
}


#endif // DQN_WITH_HEX

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Str
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_b32 Dqn_CStringEquals(char const *a, char const *b, Dqn_isize a_size, Dqn_isize b_size)
{
    if (a_size == -1) a_size = DQN_CAST(Dqn_isize)Dqn_CStringSize(a);
    if (b_size == -1) b_size = DQN_CAST(Dqn_isize)Dqn_CStringSize(b);
    if (a_size != b_size) return false;
    return (DQN_MEMCMP(a, b, DQN_CAST(size_t)a_size) == 0);
}

DQN_API char const *Dqn_CStringFindMulti(char const *buf, char const *find_list[], Dqn_isize const *find_string_sizes, Dqn_isize find_size, Dqn_isize *match_index, Dqn_isize buf_size)
{
    char const *result = nullptr;
    if (find_size == 0) return result;
    if (buf_size < 0) buf_size = DQN_CAST(Dqn_isize)Dqn_CStringSize(buf);

    char const *buf_end = buf + buf_size;
    for (; buf != buf_end; ++buf)
    {
        Dqn_isize remaining = static_cast<Dqn_isize>(buf_end - buf);
        DQN_FOR_EACH(find_index, find_size)
        {
            char const *find   = find_list[find_index];
            Dqn_isize find_str_size = find_string_sizes[find_index];
            if (remaining < find_str_size) continue;

            if (strncmp(buf, find, DQN_CAST(size_t)find_str_size) == 0)
            {
                result       = buf;
                *match_index = find_index;
                return result;
            }
        }

    }
    return result;
}

DQN_API char const *Dqn_CStringFind(char const *buf, char const *find, Dqn_isize buf_size, Dqn_isize find_size, Dqn_b32 case_insensitive)
{
    if (find_size == 0) return nullptr;
    if (buf_size < 0) buf_size = DQN_CAST(Dqn_isize)Dqn_CStringSize(buf);
    if (find_size < 0) find_size = DQN_CAST(Dqn_isize)Dqn_CStringSize(find);

    char const *buf_end = buf + buf_size;
    char const *result = nullptr;
    for (; buf != buf_end; ++buf)
    {
        Dqn_isize remaining = static_cast<Dqn_isize>(buf_end - buf);
        if (remaining < find_size) break;

        Dqn_b32 matched = true;
        for (Dqn_isize index = 0; index < find_size; index++)
        {
            char lhs = buf[index];
            char rhs = find[index];

            if (case_insensitive)
            {
                lhs = Dqn_CharToLower(lhs);
                rhs = Dqn_CharToLower(rhs);
            }

            if (lhs != rhs)
            {
                matched = false;
                break;
            }
        }

        if (matched)
        {
            result = buf;
            break;
        }
    }
    return result;
}

DQN_API char const *Dqn_CStringFileNameFromPath(char const *path, Dqn_isize size, Dqn_isize *file_name_size)
{
    char const *result      = path;
    Dqn_isize   result_size = size <= -1 ? Dqn_CStringSize(path) : size;
    for (Dqn_isize i = (result_size - 1); i >= 0; --i)
    {
        if (result[i] == '\\' || result[i] == '/')
        {
            char const *file_end = result + result_size;
            result               = result + (i + 1);
            result_size          = DQN_CAST(Dqn_isize)(file_end - result);
            break;
        }
    }

    if (file_name_size) *file_name_size = result_size;
    return result;
}

DQN_API Dqn_isize Dqn_CStringSize(char const *src)
{
    Dqn_isize result = 0;
    while (src && src[0] != 0)
    {
        src++;
        result++;
    }

    return result;
}

DQN_API Dqn_b32 Dqn_CStringMatch(char const *src, char const *find, int find_size)
{
    if (find_size == -1) find_size = Dqn_SafeTruncateUSizeToInt(Dqn_CStringSize(find));
    Dqn_b32 result = (DQN_MEMCMP(src, find, DQN_CAST(size_t)find_size) == 0);
    return result;
}

DQN_API char const *Dqn_CStringSkipToChar(char const *src, char ch)
{
    char const *result = src;
    while (result && result[0] && result[0] != ch) ++result;
    return result;
}

DQN_API char const *Dqn_CStringSkipToNextAlphaNum(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_CharIsAlphaNum(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_CStringSkipToNextDigit(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_CharIsDigit(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_CStringSkipToNextChar(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_CharIsAlpha(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_CStringSkipToNextWord(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_CharIsWhitespace(result[0])) ++result;
    while (result && result[0] && Dqn_CharIsWhitespace(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_CStringSkipToNextWhitespace(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_CharIsWhitespace(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_CStringSkipWhitespace(char const *src)
{
    char const *result = src;
    while (result && result[0] && Dqn_CharIsWhitespace(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_CStringSkipToCharInPlace(char const **src, char ch)
{
    *src = Dqn_CStringSkipToChar(*src, ch);
    return *src;
}

DQN_API char const *Dqn_CStringSkipToNextAlphaNumInPlace(char const **src)
{
    *src = Dqn_CStringSkipToNextAlphaNum(*src);
    return *src;
}

DQN_API char const *Dqn_CStringSkipToNextCharInPlace(char const **src)
{
    *src = Dqn_CStringSkipToNextChar(*src);
    return *src;
}

DQN_API char const *Dqn_CStringSkipToNextWhitespaceInPlace(char const **src)
{
    *src = Dqn_CStringSkipToNextWhitespace(*src);
    return *src;
}

DQN_API char const *Dqn_CStringSkipToNextWordInPlace(char const **src)
{
    *src = Dqn_CStringSkipToNextWord(*src);
    return *src;
}

DQN_API char const *Dqn_CStringSkipWhitespaceInPlace(char const **src)
{
    *src = Dqn_CStringSkipWhitespace(*src);
    return *src;
}

DQN_API char const *Dqn_CStringTrimWhitespaceAround(char const *src, Dqn_isize size, Dqn_isize *new_size)
{
    char const *result = src;
    if (new_size) *new_size = 0;
    if (size <= 0) return result;

    char const *start = result;
    char const *end   = start + (size - 1);
    while (start <= end && Dqn_CharIsWhitespace(start[0])) start++;
    while (end > start && Dqn_CharIsWhitespace(end[0])) end--;

    result = start;
    if (new_size) *new_size = ((end - start) + 1);
    return result;
}

DQN_API char const *Dqn_CStringTrimPrefix(char const *src, Dqn_isize size, char const *prefix, Dqn_isize prefix_size, Dqn_isize *trimmed_size)
{
    if (size <= -1) size = Dqn_CStringSize(src);
    if (prefix_size <= -1) prefix_size = Dqn_CStringSize(prefix);
    char const *result = src;
    if (prefix_size > size)
        return result;

    if (DQN_MEMCMP(src, prefix, prefix_size) == 0)
    {
        result += prefix_size;
        if (trimmed_size) *trimmed_size = size - prefix_size;
    }

    return result;
}

DQN_API Dqn_b32 Dqn_CStringIsAllDigits(char const *src, Dqn_isize size)
{
    if (!src) return false;
    if (size <= -1) size = Dqn_CStringSize(src);
    for (Dqn_isize ch_index = 0; ch_index < size; ch_index++)
    {
        if (!(src[ch_index] >= '0' && src[ch_index] <= '9'))
            return false;
    }
    Dqn_b32 result = src && size > 0;
    return result;
}

DQN_API Dqn_u64 Dqn_CStringToU64(char const *buf, Dqn_isize size, char separator)
{
    Dqn_u64 result = 0;
    if (!buf)
        return result;

    if (size <= -1)
        size = Dqn_CStringSize(buf);

    for (Dqn_isize index = 0; index < size; ++index)
    {
        char ch = buf[index];
        if (index && ch == separator)
            continue;

        if (ch < '0' || ch > '9')
            break;

        result    = Dqn_SafeMulU64(result, 10);
        int digit = ch - '0';
        result    = Dqn_SafeAddU64(result, digit);
    }

    return result;
}

DQN_API Dqn_i64 Dqn_CStringToI64(char const *buf, Dqn_isize size, char separator)
{
    Dqn_i64 result = 0;
    if (!buf)
        return result;

    if (size <= -1)
        size = Dqn_CStringSize(buf);

    char const *buf_ptr = buf;
    Dqn_b32 negative    = (buf[0] == '-');
    if (negative)
    {
        ++buf_ptr;
        --size;
    }

    for (int buf_index = 0; buf_index < size; ++buf_index)
    {
        char ch = buf_ptr[buf_index];
        if (buf_index && ch == separator)
            continue;

        if (ch < '0' || ch > '9')
            break;

        result      = Dqn_SafeMulU64(result, 10);
        Dqn_i64 val = ch - '0';
        result      = Dqn_SafeAddI64(result, val);
    }

    if (negative)
        result *= -1;
    return result;
}

DQN_API Dqn_isize Dqn_LStringSize(wchar_t const *src)
{
    Dqn_isize result = 0;
    while (src && src[0] != 0)
    {
        src++;
        result++;
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_File
// -------------------------------------------------------------------------------------------------
DQN_API char *Dqn__FileRead(char const *file_path, Dqn_isize file_path_size, Dqn_isize *file_size, Dqn_Arena *arena DQN_CALL_SITE_ARGS)
{
#if defined(DQN_OS_WIN32)
    if (file_path_size <= 0)
        file_path_size = Dqn_CStringSize(file_path);

    Dqn_ThreadScratch scratch            = Dqn_ThreadGetScratch(arena);
    Dqn_String        file_path_string   = Dqn_StringInit(file_path, file_path_size);
    Dqn_StringW       file_path_string_w = Dqn_WinArenaUTF8ToWChar(file_path_string, scratch.arena);

    void *file_handle =
        CreateFileW(file_path_string_w.str, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        Dqn_WinDumpLastError("Failed to open file %.*s", file_path_size, file_path);
        return nullptr;
    }

    DQN_DEFER { CloseHandle(file_handle); };
    LARGE_INTEGER win_file_size;
    bool get_file_size_result = GetFileSizeEx(file_handle, &win_file_size);
    if (!get_file_size_result || win_file_size.QuadPart > (unsigned long)-1)
    {
        if (!get_file_size_result)
        {
            Dqn_WinDumpLastError("GetFileSizeEx");
        }
        else
        {
            DQN_LOG_E(
                "We don't support files larger than %$.2d yet, Win32 layer needs to use overlapped IO to chunk reading in "
                "the data");
        }

        return nullptr;
    }

    auto  arena_undo = Dqn_ArenaBeginScope(arena);
    auto *result     = DQN_CAST(char *) Dqn__ArenaAllocate(arena, win_file_size.QuadPart + 1, alignof(char), Dqn_ZeroMem::No DQN_CALL_SITE_ARGS_INPUT);

    // TODO(dqn): We need to chunk this and ensure that readfile read the bytes we wanted.
    unsigned long bytes_read = 0;
    if (ReadFile(file_handle, result, DQN_CAST(unsigned long)win_file_size.QuadPart, &bytes_read, nullptr /*overlapped*/) == 0)
    {
        Dqn_ArenaEndScope(arena_undo);
        Dqn_WinDumpLastError("ReadFile");
        return nullptr;
    }

    if (file_size) *file_size = win_file_size.QuadPart;
    return result;
#else
    Dqn_isize file_size_ = 0;
    if (!file_size)
        file_size = &file_size_;

    FILE *file_handle = fopen(file_path, "rb");
    if (!file_handle)
    {
        DQN_LOG_E("Failed to open file '%s' using fopen\n", file_path);
        return nullptr;
    }

    DQN_DEFER { fclose(file_handle); };
    fseek(file_handle, 0, SEEK_END);
    *file_size = ftell(file_handle);

    if (DQN_CAST(long)(*file_size) == -1L)
    {
        DQN_LOG_E("Failed to determine '%s' file size using ftell\n", file);
        return nullptr;
    }

    rewind(file_handle);
    auto  arena_undo = Dqn_ArenaBeginScope(arena);
    auto *result     = DQN_CAST(char *) Dqn__ArenaAllocate(arena, *file_size + 1, alignof(char), Dqn_ZeroMem::No DQN_CALL_SITE_ARGS_INPUT);
    if (!result)
    {
        DQN_LOG_M("Failed to allocate %td bytes to read file '%s'\n", *file_size + 1, file);
        return nullptr;
    }

    result[*file_size] = 0;
    if (fread(result, DQN_CAST(size_t)(*file_size), 1, file_handle) != 1)
    {
        Dqn_ArenaEndScope(arena_undo);
        DQN_LOG_E("Failed to read %td bytes into buffer from '%s'\n", *file_size, file);
        return nullptr;
    }

    return result;
#endif
}

DQN_API Dqn_String Dqn__FileArenaReadToString(char const *     file_path,
                                              Dqn_isize        file_path_size,
                                              Dqn_Arena *arena DQN_CALL_SITE_ARGS)
{
    Dqn_isize     file_size = 0;
    char *        string    = Dqn__FileRead(file_path, file_path_size, &file_size, arena DQN_CALL_SITE_ARGS_INPUT);
    Dqn_String    result    = Dqn_StringInit(string, file_size);
    return result;
}

DQN_API Dqn_b32 Dqn_FileWriteFile(char const *file_path, Dqn_isize file_path_size, char const *buffer, Dqn_isize buffer_size)
{
    // TODO(dqn): Use OS apis
    (void)file_path_size;

    FILE *file_handle = nullptr;
    fopen_s(&file_handle, file_path, "w+b");
    if (!file_handle)
    {
        DQN_LOG_E("Failed to open file '%s' using fopen\n", file_path);
        return false;
    }

    DQN_DEFER { fclose(file_handle); };
    Dqn_usize write_count = fwrite(buffer, buffer_size, 1 /*count*/, file_handle);

    if (write_count != 1)
    {
        DQN_LOG_E("Failed to write to file '%s' using fwrite\n", file_path);
        return false;
    }

    return true;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_File Implementation
// -------------------------------------------------------------------------------------------------
#if defined(DQN_OS_WIN32)
DQN_API Dqn_u64 Dqn__WinFileTimeToSeconds(FILETIME const *time)
{
    ULARGE_INTEGER time_large_int = {};
    time_large_int.u.LowPart      = time->dwLowDateTime;
    time_large_int.u.HighPart     = time->dwHighDateTime;
    Dqn_u64 result                = (time_large_int.QuadPart / 10000000ULL) - 11644473600ULL;
    return result;
}
#endif

// NOTE: Max size from MSDN, using \\? syntax, but the ? bit can be expanded
// even more so the max size is kind of not well defined.
#if defined(DQN_OS_WIN32) && !defined(DQN_OS_WIN32_MAX_PATH)
    #define DQN_OS_WIN32_MAX_PATH 32767 + 128 /*fudge*/
#endif

DQN_API Dqn_b32 Dqn_FileExists(Dqn_String path)
{
    Dqn_b32 result = false;
#if defined(DQN_OS_WIN32)
    wchar_t path_w[DQN_OS_WIN32_MAX_PATH];
    Dqn_WinUTF8ToWChar(path, path_w, DQN_CAST(int)Dqn_ArrayCountI(path_w));

    WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
    if (GetFileAttributesExW(path_w, GetFileExInfoStandard, &attrib_data))
    {
        result = (attrib_data.dwFileAttributes != INVALID_FILE_ATTRIBUTES) &&
                 !(attrib_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    }

#elif defined(DQN_OS_UNIX)
    struct stat stat_result;
    if (lstat(path.str, &stat_result) != -1)
        result = S_ISREG(stat_result.st_mode) || S_ISLNK(stat_result.st_mode);

#else
    #error Unimplemented

#endif

    return result;
}

DQN_API Dqn_b32 Dqn_FileDirExists(Dqn_String path)
{
    Dqn_b32 result = false;
#if defined(DQN_OS_WIN32)
    wchar_t path_w[DQN_OS_WIN32_MAX_PATH];
    Dqn_WinUTF8ToWChar(path, path_w, DQN_CAST(int)Dqn_ArrayCountInt(path_w));

    WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
    if (GetFileAttributesExW(path_w, GetFileExInfoStandard, &attrib_data))
    {
        result = (attrib_data.dwFileAttributes != INVALID_FILE_ATTRIBUTES) &&
                 (attrib_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    }

#elif defined(DQN_OS_UNIX)
    struct stat stat_result;
    if (lstat(path.str, &stat_result) != -1)
        result = S_ISDIR(stat_result.st_mode);
#else
    #error Unimplemented
#endif

    return result;
}

DQN_API Dqn_FileInfo Dqn_FileGetInfo(Dqn_String path)
{
    Dqn_FileInfo result = {};
#if defined(DQN_OS_WIN32)
    WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
    wchar_t path_w[DQN_OS_WIN32_MAX_PATH];
    Dqn_WinUTF8ToWChar(path, path_w, DQN_CAST(int)Dqn_ArrayCountInt(path_w));
    if (!GetFileAttributesExW(path_w, GetFileExInfoStandard, &attrib_data))
        return result;

    if (result)
    {
        result.create_time_in_s      = Dqn__WinFileTimeToSeconds(&attrib_data.ftCreationTime);
        result.last_access_time_in_s = Dqn__WinFileTimeToSeconds(&attrib_data.ftLastAccessTime);
        result.last_write_time_in_s  = Dqn__WinFileTimeToSeconds(&attrib_data.ftLastWriteTime);

        LARGE_INTEGER large_int = {};
        large_int.u.HighPart    = DQN_CAST(Dqn_i32)attrib_data.nFileSizeHigh;
        large_int.u.LowPart     = attrib_data.nFileSizeLow;
        result.size             = (Dqn_u64)large_int.QuadPart;
    }
#elif defined(DQN_OS_UNIX)
    struct stat file_stat;
    if (lstat(path.str, &file_stat) != -1)
    {
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

DQN_API Dqn_b32 Dqn_FileCopy(Dqn_String src, Dqn_String dest, Dqn_b32 overwrite)
{
    Dqn_b32 result = false;
#if defined(DQN_OS_WIN32)
    wchar_t src_w [DQN_OS_WIN32_MAX_PATH];
    wchar_t dest_w[DQN_OS_WIN32_MAX_PATH];
    Dqn_WinUTF8ToWChar(src, src_w, DQN_CAST(int)Dqn_ArrayCountInt(src_w));
    Dqn_WinUTF8ToWChar(dest, dest_w, DQN_CAST(int)Dqn_ArrayCountInt(dest_w));

    int fail_if_exists = overwrite == false;
    result = CopyFileW(src_w, dest_w, fail_if_exists) != 0;
    if (!result)
        Dqn_WinDumpLastError("Failed to copy from %.*s to %.*s", DQN_STRING_FMT(src), DQN_STRING_FMT(dest));

#elif defined(DQN_OS_UNIX)
    int src_fd  = open(src.str, O_RDONLY);
    int dest_fd = open(dest.str, O_WRONLY | O_CREAT | (overwrite ? O_TRUNC : 0));
    if (src_fd != -1 && dest_fd != -1)
    {
        // NOTE: Do a shallow copy-on-write
        // See: https://git.savannah.gnu.org/gitweb/?p=coreutils.git;a=blob;f=src/copy.c;h=159556d052158a6d3839b5b4ff69887471ff32da;hb=HEAD#l405
        // See: https://man7.org/linux/man-pages/man2/ioctl_ficlonerange.2.html
        int ioctl_result = ioctl(dest_fd, FICLONE, src_fd);
        if (ioctl_result == -1)
        {
            // If it fails, try a binary copy- and if that fails we give up.
            // Taken from: https://github.com/gingerBill/gb/blob/master/gb.h
            struct stat stat_existing;
            fstat(src_fd, &stat_existing);

            Dqn_isize bytes_written = sendfile64(dest_fd, src_fd, 0, stat_existing.st_size);
            result = (bytes_written == stat_existing.st_size);
        }
    }

    if (src_fd != -1) close(src_fd);
    if (dest_fd != -1) close(dest_fd);

#else
    #error Unimplemented
#endif

    return result;
}

DQN_API Dqn_b32 Dqn_FileMakeDir(Dqn_String path, Dqn_Arena *temp_arena)
{
    Dqn_b32 result           = true;
    int path_indexes_size    = 0;
    Dqn_u16 path_indexes[64] = {};

#if defined(DQN_OS_WIN32)
    (void)temp_arena;

    wchar_t src_w[DQN_OS_WIN32_MAX_PATH];
    int src_w_size = Dqn_WinUTF8ToWChar(path, src_w, DQN_CAST(int)Dqn_ArrayCountI(src_w));

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
    for (Dqn_i32 index = src_w_size - 1; index >= 0; index--)
    {
        Dqn_b32 first_char = index == (src_w_size - 1);
        wchar_t ch         = src_w[index];
        if (ch == '/' || ch == '\\' || first_char)
        {
            WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
            wchar_t temp = src_w[index];

            if (!first_char) src_w[index] = 0; // Temporarily null terminate it
            Dqn_b32 successful = GetFileAttributesExW(src_w, GetFileExInfoStandard, &attrib_data); // Check
            if (!first_char) src_w[index] = temp; // Undo null termination

            if (successful)
            {
                if (attrib_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    // NOTE: We found a directory, we can stop here and start
                    // building up all the directories that didn't exist up to
                    // this point.
                    break;
                }
                else
                {
                    // NOTE: There's something that exists in at this path, but
                    // it's not a directory. This request to make a directory is
                    // invalid.
                    return false;
                }
            }
            else
            {
                // NOTE: There's nothing that exists at this path, we can create
                // a directory here
                path_indexes[path_indexes_size++] = DQN_CAST(Dqn_u16)index;
            }
        }
    }

    for (Dqn_isize index = path_indexes_size - 1; index >= 0 && result; index--)
    {
        Dqn_u16 path_index = path_indexes[index];
        wchar_t temp = src_w[path_index];

        if (index != 0) src_w[path_index] = 0;
        result |= (CreateDirectoryW(src_w, nullptr) == 0);
        if (index != 0) src_w[path_index] = temp;
    }

#elif defined(DQN_OS_UNIX)
    auto scoped_arena = Dqn_ArenaScope(temp_arena);
    Dqn_String copy = Dqn_StringCopy(path, temp_arena);

    for (Dqn_i32 index = copy.size - 1; index >= 0; index--)
    {
        Dqn_b32 first_char = index == (copy.size - 1);
        wchar_t ch         = copy.str[index];
        if (ch == '/' || first_char)
        {
            char temp = copy.str[index];
            if (!first_char) copy.str[index] = 0; // Temporarily null terminate it
            Dqn_b32 is_file = Dqn_FileExists(copy);
            if (!first_char) copy.str[index] = temp; // Undo null termination

            if (is_file)
            {
                // NOTE: There's something that exists in at this path, but
                // it's not a directory. This request to make a directory is
                // invalid.
                return false;
            }
            else
            {
                if (Dqn_FileDirExists(copy))
                {
                    // NOTE: We found a directory, we can stop here and start
                    // building up all the directories that didn't exist up to
                    // this point.
                    break;
                }
                else
                {
                    // NOTE: There's nothing that exists at this path, we can
                    // create a directory here
                    path_indexes[path_indexes_size++] = DQN_CAST(Dqn_u16)index;
                }
            }
        }
    }

    for (Dqn_isize index = path_indexes_size - 1; index >= 0 && result; index--)
    {
        Dqn_u16 path_index = path_indexes[index];
        char temp = copy.str[path_index];

        if (index != 0) copy.str[path_index] = 0;
        result |= mkdir(copy.str, 0774) == 0;
        if (index != 0) copy.str[path_index] = temp;
    }

#else
    #error Unimplemented
#endif

    return result;
}

DQN_API Dqn_b32 Dqn_FileMove(Dqn_String src, Dqn_String dest, Dqn_b32 overwrite)
{
    Dqn_b32 result = false;

#if defined(DQN_OS_WIN32)
    wchar_t src_w [DQN_OS_WIN32_MAX_PATH];
    wchar_t dest_w[DQN_OS_WIN32_MAX_PATH];
    Dqn_WinUTF8ToWChar(src, src_w, DQN_CAST(int)Dqn_ArrayCountI(src_w));
    Dqn_WinUTF8ToWChar(dest, dest_w, DQN_CAST(int)Dqn_ArrayCountI(dest_w));

    unsigned long flags = MOVEFILE_COPY_ALLOWED;
    if (overwrite) flags |= MOVEFILE_REPLACE_EXISTING;

    result = MoveFileExW(src_w, dest_w, flags) != 0;
    if (!result) Dqn_WinDumpLastError("Failed to move from %.*s to %.*s", DQN_STRING_FMT(src), DQN_STRING_FMT(dest));

#elif defined(DQN_OS_UNIX)
    // See: https://github.com/gingerBill/gb/blob/master/gb.h
    bool file_moved = true;
    if (link(src.str, dest.str) == -1)
    {
        // NOTE: Link can fail if we're trying to link across different volumes
        // so we fall back to a binary directory.
        file_moved |= Dqn_FileCopy(src, dest, overwrite);
    }

    if (file_moved)
        result = (unlink(src.str) != -1); // Remove original file

#else
    #error Unimplemented

#endif

    return result;
}

DQN_API Dqn_b32 Dqn_FileDelete(Dqn_String path)
{
    Dqn_b32 result = false;
#if defined(DQN_OS_WIN32)
    wchar_t path_w [DQN_OS_WIN32_MAX_PATH];
    Dqn_WinUTF8ToWChar(path, path_w, DQN_CAST(int)Dqn_ArrayCountInt(path_w));
    result = DeleteFileW(path_w);
    if (!result)
        result = RemoveDirectoryW(path_w);

#elif defined(DQN_OS_UNIX)
    result = remove(path.str) == 0;

#else
    #error Unimplemented

#endif

    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Date
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_DateHMSTime Dqn_DateHMSLocalTimeNow()
{
    Dqn_DateHMSTime result = {};
#if defined(DQN_OS_WIN32)
    SYSTEMTIME sys_time;
    GetLocalTime(&sys_time);
    result.hour    = DQN_CAST(Dqn_i8)sys_time.wHour;
    result.minutes = DQN_CAST(Dqn_i8)sys_time.wMinute;
    result.seconds = DQN_CAST(Dqn_i8)sys_time.wSecond;

    result.day   = DQN_CAST(Dqn_i8)sys_time.wDay;
    result.month = DQN_CAST(Dqn_i8)sys_time.wMonth;
    result.year  = DQN_CAST(Dqn_i16)sys_time.wYear;
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

    result.day   = DQN_CAST(Dqn_i8)time.tm_mday;
    result.month = DQN_CAST(Dqn_i8)time.tm_mon + 1;
    result.year  = 1900 + DQN_CAST(Dqn_i16)time.tm_year;
#endif

    return result;
}

DQN_API Dqn_DateHMSTimeString Dqn_DateHMSLocalTimeStringNow(char date_separator, char hms_separator)
{
    Dqn_DateHMSTime const time = Dqn_DateHMSLocalTimeNow();

    Dqn_DateHMSTimeString result = {};
    result.hms_size              = DQN_CAST(Dqn_i8) STB_SPRINTF_DECORATE(snprintf)(result.hms,
                                                                      DQN_CAST(int) Dqn_ArrayCountI(result.hms),
                                                                      "%02d%c%02d%c%02d",
                                                                      time.hour,
                                                                      hms_separator,
                                                                      time.minutes,
                                                                      hms_separator,
                                                                      time.seconds);

    result.date_size = DQN_CAST(Dqn_i8) STB_SPRINTF_DECORATE(snprintf)(result.date,
                                                                       DQN_CAST(int) Dqn_ArrayCountI(result.date),
                                                                       "%d%c%02d%c%02d",
                                                                       time.year,
                                                                       date_separator,
                                                                       time.month,
                                                                       date_separator,
                                                                       time.day);

    return result;
}

DQN_API Dqn_u64 Dqn_DateEpochTime()
{
#if defined(DQN_OS_WIN32)
   const Dqn_i64 UNIX_TIME_START  = 0x019DB1DED53E8000; //January 1, 1970 (start of Unix epoch) in "ticks"
   const Dqn_i64 TICKS_PER_SECOND = 10'000'000; // Filetime returned is in intervals of 100 nanoseconds

   FILETIME file_time;
   GetSystemTimeAsFileTime(&file_time);

   LARGE_INTEGER date_time;
   date_time.LowPart  = file_time.dwLowDateTime;
   date_time.HighPart = file_time.dwHighDateTime;
   Dqn_u64 result     = (date_time.QuadPart - UNIX_TIME_START) / TICKS_PER_SECOND;

#elif defined(DQN_OS_UNIX)
    Dqn_u64 result = time(nullptr);
#else
    #error Unimplemented
#endif

    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: OS
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_b32 Dqn_OSSecureRNGBytes(void *buffer, Dqn_u32 size)
{
    if (!buffer || size < 0)
        return false;

    if (size == 0)
        return true;

#if defined(DQN_OS_WIN32)
    bool init = true;
    Dqn_TicketMutexBegin(&dqn__lib.win32_bcrypt_rng_mutex);
    if (!dqn__lib.win32_bcrypt_rng_handle)
    {
        wchar_t const BCRYPT_ALGORITHM[] = L"RNG";
        long /*NTSTATUS*/ init_status = BCryptOpenAlgorithmProvider(&dqn__lib.win32_bcrypt_rng_handle, BCRYPT_ALGORITHM, nullptr /*implementation*/, 0 /*flags*/);
        if (!dqn__lib.win32_bcrypt_rng_handle || init_status != 0)
        {
            DQN_LOG_E("Failed to initialise random number generator, error: %d", init_status);
            init = false;
        }
    }
    Dqn_TicketMutexEnd(&dqn__lib.win32_bcrypt_rng_mutex);

    if (!init)
        return false;

    long gen_status = BCryptGenRandom(dqn__lib.win32_bcrypt_rng_handle, DQN_CAST(unsigned char *)buffer, size, 0 /*flags*/);
    if (gen_status != 0)
    {
        DQN_LOG_E("Failed to generate random bytes: %d", gen_status);
        return false;
    }

#else
    DQN_ASSERT_MSG(size <= 32,
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

DQN_API Dqn_String Dqn_OSExecutableDirectory(Dqn_Arena *arena)
{
    Dqn_String result = {};

#if defined(DQN_OS_WIN32)
    char temp_mem[sizeof(wchar_t) * DQN_OS_WIN32_MAX_PATH + sizeof(Dqn_ArenaMemBlock)];
    Dqn_Arena   temp_arena = Dqn_ArenaInitWithMemory(temp_mem, Dqn_ArrayCountI(temp_mem));
    Dqn_StringW exe_dir_w  = Dqn_WinExecutableDirectoryW(&temp_arena);
    result                 = Dqn_WinArenaWCharToUTF8(exe_dir_w, arena);

#elif defined(DQN_OS_UNIX)

    int required_size_wo_null_terminator = 0;
    for (int try_size = 128;
         ;
         try_size *= 2)
    {
        auto scoped_arena = Dqn_ArenaScope(arena);
        char *try_buf     = Dqn_ArenaNewArray(arena, char, try_size, Dqn_ZeroMem::No);
        int bytes_written = readlink("/proc/self/exe", try_buf, try_size);
        if (bytes_written == -1)
        {
            // Failed, we're unable to determine the executable directory
            break;
        }
        else if (bytes_written == try_size)
        {
            // Try again, if returned size was equal- we may of prematurely
            // truncated according to the man pages
            continue;
        }
        else
        {
            // readlink will give us the path to the executable. Once we
            // determine the correct buffer size required to get the full file
            // path, we do some post-processing on said string and extract just
            // the directory.

            // TODO(dqn): It'd be nice if there's some way of keeping this
            // try_buf around, memcopy the byte and trash the try_buf from the
            // arena. Instead we just get the size and redo the call one last
            // time after this "calculate" step.
            DQN_ASSERT_MSG(bytes_written < try_size, "bytes_written can never be greater than the try size, function writes at most try_size");
            required_size_wo_null_terminator = bytes_written;

            for (int index_of_last_slash = bytes_written;
                 index_of_last_slash >= 0;
                 index_of_last_slash--)
            {
                if (try_buf[index_of_last_slash] == '/')
                {
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

    if (required_size_wo_null_terminator)
    {
        Dqn_ArenaScopeData scope = Dqn_ArenaBeginScope(arena);
        char *exe_path = Dqn_ArenaNewArray(arena, char, required_size_wo_null_terminator + 1, Dqn_ZeroMem::No);
        exe_path[required_size_wo_null_terminator] = 0;

        int bytes_written = readlink("/proc/self/exe", exe_path, required_size_wo_null_terminator);
        if (bytes_written == -1)
        {
            // Note that if read-link fails again can be because there's
            // a potential race condition here, our exe or directory could have
            // been deleted since the last call, so we need to be careful.
            Dqn_ArenaEndScope(scope);
        }
        else
        {
            result = Dqn_StringInit(exe_path, required_size_wo_null_terminator);
        }
    }

#else
    #error Unimplemented
#endif

    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Utilities
// -------------------------------------------------------------------------------------------------
DQN_API void Dqn_SleepMs(Dqn_uint milliseconds)
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

DQN_FILE_SCOPE void Dqn_PerfCounter_Init()
{
#if defined(DQN_OS_WIN32)
    if (dqn__lib.win32_qpc_frequency.QuadPart == 0)
        QueryPerformanceFrequency(&dqn__lib.win32_qpc_frequency);
#endif
}

DQN_API Dqn_f64 Dqn_PerfCounterS(Dqn_u64 begin, Dqn_u64 end)
{
    Dqn_PerfCounter_Init();
    Dqn_u64 ticks  = end - begin;
#if defined(DQN_OS_WIN32)
    Dqn_f64 result = ticks / DQN_CAST(Dqn_f64)dqn__lib.win32_qpc_frequency.QuadPart;
#else
    Dqn_f64 result = ticks / 1'000'000'000;
#endif
    return result;
}

DQN_API Dqn_f64 Dqn_PerfCounterMs(Dqn_u64 begin, Dqn_u64 end)
{
    Dqn_PerfCounter_Init();
    Dqn_u64 ticks  = end - begin;
#if defined(DQN_OS_WIN32)
    Dqn_f64 result = (ticks * 1'000) / DQN_CAST(Dqn_f64)dqn__lib.win32_qpc_frequency.QuadPart;
#else
    Dqn_f64 result = ticks / DQN_CAST(Dqn_f64)1'000'000;
#endif
    return result;
}

DQN_API Dqn_f64 Dqn_PerfCounterMicroS(Dqn_u64 begin, Dqn_u64 end)
{
    Dqn_PerfCounter_Init();
    Dqn_u64 ticks  = end - begin;
#if defined(DQN_OS_WIN32)
    Dqn_f64 result = (ticks * 1'000'000) / DQN_CAST(Dqn_f64)dqn__lib.win32_qpc_frequency.QuadPart;
#else
    Dqn_f64 result = ticks / DQN_CAST(Dqn_f64)1'000;
#endif
    return result;
}

DQN_API Dqn_f64 Dqn_PerfCounterNs(Dqn_u64 begin, Dqn_u64 end)
{
    Dqn_PerfCounter_Init();
    Dqn_u64 ticks  = end - begin;
#if defined(DQN_OS_WIN32)
    Dqn_f64 result = (ticks * 1'000'000'000) / DQN_CAST(Dqn_f64)dqn__lib.win32_qpc_frequency.QuadPart;
#else
    Dqn_f64 result = ticks;
#endif
    return result;
}

DQN_API Dqn_u64 Dqn_PerfCounterNow()
{
    Dqn_u64 result = 0;
#if defined(DQN_OS_WIN32)
    LARGE_INTEGER integer = {};
    int qpc_result = QueryPerformanceCounter(&integer);
    (void)qpc_result;
    DQN_ASSERT_MSG(qpc_result, "MSDN says this can only fail when running on a version older than Windows XP");
    result = integer.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    result = DQN_CAST(Dqn_u64)ts.tv_sec * 1'000'000'000 + DQN_CAST(Dqn_u64)ts.tv_nsec;
#endif

    return result;
}

DQN_API Dqn_Timer Dqn_TimerBegin()
{
    Dqn_Timer result = {};
    result.start     = Dqn_PerfCounterNow();
    return result;
}

DQN_API void Dqn_TimerEnd(Dqn_Timer *timer)
{
    timer->end = Dqn_PerfCounterNow();
}

DQN_API Dqn_f64 Dqn_TimerS(Dqn_Timer timer)
{
    Dqn_f64 result = Dqn_PerfCounterS(timer.start, timer.end);
    return result;
}

DQN_API Dqn_f64 Dqn_TimerMs(Dqn_Timer timer)
{
    Dqn_f64 result = Dqn_PerfCounterMs(timer.start, timer.end);
    return result;
}

DQN_API Dqn_f64 Dqn_TimerMicroS(Dqn_Timer timer)
{
    Dqn_f64 result = Dqn_PerfCounterMicroS(timer.start, timer.end);
    return result;
}

DQN_API Dqn_f64 Dqn_TimerNs(Dqn_Timer timer)
{
    Dqn_f64 result = Dqn_PerfCounterNs(timer.start, timer.end);
    return result;
}

DQN_API char *Dqn_U64ToStr(Dqn_u64 val, Dqn_U64Str *result, Dqn_b32 comma_sep)
{
    int buf_index            = (int)(Dqn_ArrayCount(result->buf) - 1);
    result->buf[buf_index--] = 0;

    if (val == 0)
    {
        result->buf[buf_index--] = '0';
        result->len              = 1;
    }
    else
    {
        for (int digit_count = 0; val > 0; result->len++, digit_count++)
        {
            if (comma_sep && (digit_count != 0) && (digit_count % 3 == 0))
            {
                result->buf[buf_index--] = ',';
                result->len++;
            }

            auto digit               = DQN_CAST(char)(val % 10);
            result->buf[buf_index--] = '0' + digit;
            val /= 10;
        }
    }

    result->str = result->buf + (buf_index + 1);
    return result->str;
}

DQN_API char *Dqn_U64ToTempStr(Dqn_u64 val, Dqn_b32 comma_sep)
{
    DQN_LOCAL_PERSIST Dqn_U64Str string;
    string = {};
    char *result = Dqn_U64ToStr(val, &string, comma_sep);
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Lib
// -------------------------------------------------------------------------------------------------
DQN_API void Dqn_LibDumpThreadContextArenaStats(Dqn_String file_path)
{
    (void)file_path;
#if defined(DQN_DEBUG_THREAD_CONTEXT)
    FILE *file = nullptr;
    fopen_s(&file, file_path.str, "a+b");
    if (file)
    {
        // ---------------------------------------------------------------------
        // Copy the stats from library book-keeping
        // ---------------------------------------------------------------------
        // NOTE: Extremely short critical section, copy the stats then do our
        // work on it.
        Dqn_ArenaStats current_stats[Dqn_ArrayCountI(dqn__lib.thread_context_arena_current_stats)];
        Dqn_ArenaStats highest_stats[Dqn_ArrayCountI(dqn__lib.thread_context_arena_highest_stats)];
        int stats_size = 0;

        Dqn_TicketMutexBegin(&dqn__lib.thread_context_mutex);
        stats_size = dqn__lib.thread_context_arena_stats_size;
        DQN_MEMCOPY(current_stats, dqn__lib.thread_context_arena_current_stats, sizeof(current_stats[0]) * stats_size);
        DQN_MEMCOPY(highest_stats, dqn__lib.thread_context_arena_highest_stats, sizeof(highest_stats[0]) * stats_size);
        Dqn_TicketMutexEnd(&dqn__lib.thread_context_mutex);

        // ---------------------------------------------------------------------
        // Print the cumulative stat
        // ---------------------------------------------------------------------
        Dqn_DateHMSTimeString now = Dqn_DateHMSLocalTimeStringNow();
        fprintf(file,
                "Time=%.*s %.*s | Thread Context Arenas | Count=%d\n",
                now.date_size, now.date,
                now.hms_size, now.hms,
                dqn__lib.thread_context_arena_stats_size);

        // Write the cumulative thread arena data
        {
            Dqn_ArenaStats cumulative_stats = {};
            Dqn_ArenaStats highest_cumulative_stats = {};
            for (Dqn_isize index = 0; index < stats_size; index++)
            {
                Dqn_ArenaStats const *current    = current_stats + index;
                Dqn_ArenaStats const *highest    = highest_stats + index;

                cumulative_stats.bytes_allocated += current->bytes_allocated;
                cumulative_stats.bytes_used      += current->bytes_used;
                cumulative_stats.bytes_wasted    += current->bytes_wasted;
                cumulative_stats.block_count     += current->block_count;

                highest_cumulative_stats.bytes_allocated = DQN_MAX(highest_cumulative_stats.bytes_allocated, highest->bytes_allocated);
                highest_cumulative_stats.bytes_used      = DQN_MAX(highest_cumulative_stats.bytes_used, highest->bytes_used);
                highest_cumulative_stats.bytes_wasted    = DQN_MAX(highest_cumulative_stats.bytes_wasted, highest->bytes_wasted);
                highest_cumulative_stats.block_count     = DQN_MAX(highest_cumulative_stats.block_count, highest->block_count);
            }

            Dqn_ArenaStatsString cumulative_stats_string         = Dqn_ArenaStatsToString(&cumulative_stats);
            Dqn_ArenaStatsString highest_cumulative_stats_string = Dqn_ArenaStatsToString(&highest_cumulative_stats);
            fprintf(file, "  [ALL] CURR %.*s\n", cumulative_stats_string.size, cumulative_stats_string.str);
            fprintf(file, "        HIGH %.*s\n", highest_cumulative_stats_string.size, highest_cumulative_stats_string.str);
        }

        // ---------------------------------------------------------------------
        // Print individual thread arena data
        // ---------------------------------------------------------------------
        for (Dqn_isize index = 0; index < stats_size; index++)
        {
            Dqn_ArenaStats const *current = current_stats + index;
            Dqn_ArenaStats const *highest = current_stats + index;

            Dqn_ArenaStatsString current_string = Dqn_ArenaStatsToString(current);
            Dqn_ArenaStatsString highest_string = Dqn_ArenaStatsToString(highest);

            fprintf(file, "  [%03d] CURR %.*s\n", DQN_CAST(int)index, current_string.size, current_string.str);
            fprintf(file, "         HIGH %.*s\n", highest_string.size, highest_string.str);
        }

        fclose(file);
        DQN_LOG_I("Dumped thread context arenas to %.*s", DQN_STRING_FMT(file_path));
    }
    else
    {
        DQN_LOG_E("Failed to dump thread context arenas to %.*s", DQN_STRING_FMT(file_path));
    }
#endif // #if defined(DQN_DEBUG_THREAD_CONTEXT)
}

DQN_API void Dqn_LibSetLogCallback(Dqn_LogProc *proc, void *user_data)
{
    dqn__lib.LogCallback   = proc;
    dqn__lib.log_user_data = user_data;
}

DQN_API void Dqn_LibSetLogFile(FILE *file)
{
    Dqn_TicketMutexBegin(&dqn__lib.log_file_mutex);
    dqn__lib.log_file           = file;
    dqn__lib.log_no_output_file = file ? false : true;
    Dqn_TicketMutexEnd(&dqn__lib.log_file_mutex);
}

// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_ThreadContext
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_ThreadContext *Dqn_ThreadGetContext()
{
    thread_local Dqn_ThreadContext result = {};
    if (!result.init)
    {
        result.init = true;
        for (Dqn_ThreadScratchData &scratch_data : result.scratch_data)
        {
            scratch_data.arena = Dqn_ArenaInitWithCRT(DQN_MEGABYTES(4));

#if defined(DQN_DEBUG_THREAD_CONTEXT)
            // NOTE: Allocate this arena a slot in the stats array that we use
            // to record allocation statistics for each thread's arena.
            Dqn_TicketMutexBegin(&dqn__lib.thread_context_mutex);
            scratch_data.arena_stats_index = dqn__lib.thread_context_arena_stats_size++;
            Dqn_TicketMutexEnd(&dqn__lib.thread_context_mutex);
            DQN_HARD_ASSERT(dqn__lib.thread_context_arena_stats_size < Dqn_ArrayCountI(dqn__lib.thread_context_arena_current_stats));
#endif
        }
    }

    return &result;
}

DQN_API Dqn_ThreadScratch Dqn_ThreadGetScratch(const Dqn_Arena *conflict_arena)
{
    Dqn_ThreadContext     *thread = Dqn_ThreadGetContext();
    Dqn_ThreadScratchData *result = nullptr;
    for (Dqn_ThreadScratchData &scratch_data : thread->scratch_data)
    {
        if (!conflict_arena || (&scratch_data.arena != conflict_arena))
        {
            result = &scratch_data;
            break;
        }
    }

    DQN_HARD_ASSERT(result);
    return Dqn_ThreadScratch(result);
}

#if defined(DQN_WITH_JSON_WRITER)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_JsonWriter
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_JsonWriter Dqn_JsonWriterInit(Dqn_Arena *arena, int spaces_per_indent)
{
    Dqn_JsonWriter result        = {};
    result.arena             = arena;
    result.spaces_per_indent = spaces_per_indent;
    return result;
}

DQN_API Dqn_String Dqn_JsonWriterBuild(Dqn_JsonWriter *writer, Dqn_Arena *arena)
{
    Dqn_String result = Dqn_StringListBuild(&writer->list, arena);
    return result;
}

DQN_API void Dqn_JsonWriter_DoIndent(Dqn_JsonWriter *writer)
{
    int spaces_per_indent = writer->spaces_per_indent ? writer->spaces_per_indent : 2;
    int spaces            = writer->indent_level * spaces_per_indent;
    if (spaces)
        Dqn_StringListAppendFmt(&writer->list, writer->arena, "%*s", spaces, "");
}

DQN_API void Dqn_JsonWriter_PreAddItem(Dqn_JsonWriter *writer)
{
    if (writer->parent_field_count_stack_size <= 0)
        return;

    Dqn_u16 *parent_field_count = &writer->parent_field_count_stack[writer->parent_field_count_stack_size - 1];
    if (*parent_field_count == 0)
    {
        // NOTE: First time we're adding an item to an object, we need to write
        // on a new line for nice formatting.
        Dqn_StringListAppendString(&writer->list, writer->arena, DQN_STRING("\n"));
    }
    else if (*parent_field_count > 0)
    {
        // NOTE: We have items in the object already and we're adding another
        // item so we need to add a comma on the previous item.
        Dqn_StringListAppendString(&writer->list, writer->arena, DQN_STRING(",\n"));
    }

    Dqn_JsonWriter_DoIndent(writer);
}

DQN_API void Dqn_JsonWriter_PostAddItem(Dqn_JsonWriter *writer)
{
    if (writer->parent_field_count_stack_size <= 0)
        return;

    Dqn_u16 *parent_field_count = &writer->parent_field_count_stack[writer->parent_field_count_stack_size - 1];
    (*parent_field_count)++;
}

DQN_API void Dqn_JsonWriter_BeginContainer(Dqn_JsonWriter *writer, Dqn_String name, bool array)
{
    Dqn_String container_ch = array ? DQN_STRING("[") : DQN_STRING("{");
    Dqn_JsonWriter_PreAddItem(writer);
    if (name.size)
        Dqn_StringListAppendFmt(&writer->list, writer->arena, "\"%.*s\": %.*s", DQN_STRING_FMT(name), DQN_STRING_FMT(container_ch));
    else
        Dqn_StringListAppendString(&writer->list, writer->arena, container_ch);
    Dqn_JsonWriter_PostAddItem(writer);

    writer->indent_level++;

    DQN_ASSERT(writer->parent_field_count_stack_size < DQN_ARRAY_COUNT(writer->parent_field_count_stack));
    Dqn_u16 *parent_field_count = &writer->parent_field_count_stack[writer->parent_field_count_stack_size++];
    *parent_field_count         = 0;

}

DQN_API void Dqn_JsonWriter_EndContainer(Dqn_JsonWriter *writer, Dqn_b32 array)
{
    Dqn_u16 *parent_field_count = &writer->parent_field_count_stack[writer->parent_field_count_stack_size - 1];
    if (*parent_field_count > 0)
    {
        // NOTE: End of object/array should start on a new line if the
        // array/object has atleast one field in it.
        Dqn_StringListAppendFmt(&writer->list, writer->arena, "\n");
    }

    writer->parent_field_count_stack_size--;
    DQN_ASSERT(writer->parent_field_count_stack_size >= 0);

    writer->indent_level--;
    DQN_ASSERT(writer->indent_level >= 0);

    Dqn_JsonWriter_DoIndent(writer);
    Dqn_StringListAppendString(&writer->list, writer->arena, array ? DQN_STRING("]") : DQN_STRING("}"));
}

DQN_API void Dqn_JsonWriterBeginNamedObject(Dqn_JsonWriter *writer, Dqn_String name)
{
    Dqn_JsonWriter_BeginContainer(writer, name, false /*array*/);
}

DQN_API void Dqn_JsonWriterBeginObject(Dqn_JsonWriter *writer)
{
    Dqn_JsonWriter_BeginContainer(writer, DQN_STRING(""), false /*array*/);
}

DQN_API void Dqn_JsonWriterEndObject(Dqn_JsonWriter *writer)
{
    Dqn_JsonWriter_EndContainer(writer, false /*array*/);
}

DQN_API void Dqn_JsonWriterBeginNamedArray(Dqn_JsonWriter *writer, Dqn_String name)
{
    Dqn_JsonWriter_BeginContainer(writer, name, true /*array*/);
}

DQN_API void Dqn_JsonWriterBeginArray(Dqn_JsonWriter *writer)
{
    Dqn_JsonWriter_BeginContainer(writer, DQN_STRING(""), false /*array*/);
}

DQN_API void Dqn_JsonWriterEndArray(Dqn_JsonWriter *writer)
{
    Dqn_JsonWriter_EndContainer(writer, true /*array*/);
}

DQN_API void Dqn_JsonWriterNamedString(Dqn_JsonWriter *writer, Dqn_String key, Dqn_String value)
{
    Dqn_JsonWriter_PreAddItem(writer);
    if (key.size)
        Dqn_StringListAppendFmt(&writer->list, writer->arena, "\"%.*s\": \"%.*s\"", DQN_STRING_FMT(key), DQN_STRING_FMT(value));
    else
        Dqn_StringListAppendFmt(&writer->list, writer->arena, "\"%.*s\"", DQN_STRING_FMT(value));
    Dqn_JsonWriter_PostAddItem(writer);
}

DQN_API void Dqn_JsonWriterString(Dqn_JsonWriter *writer, Dqn_String value)
{
    Dqn_JsonWriterNamedString(writer, DQN_STRING("") /*key*/, value);
}

DQN_API void Dqn_JsonWriterNamedU64(Dqn_JsonWriter *writer, Dqn_String key, Dqn_u64 value)
{
    Dqn_JsonWriter_PreAddItem(writer);
    if (key.size)
        Dqn_StringListAppendFmt(&writer->list, writer->arena, "\"%.*s\": %I64u", DQN_STRING_FMT(key), value);
    else
        Dqn_StringListAppendFmt(&writer->list, writer->arena, "%I64u", value);
    Dqn_JsonWriter_PostAddItem(writer);
}

DQN_API void Dqn_JsonWriterU64(Dqn_JsonWriter *writer, Dqn_u64 value)
{
    Dqn_JsonWriterNamedU64(writer, DQN_STRING("") /*key*/, value);
}

DQN_API void Dqn_JsonWriterNamedF64(Dqn_JsonWriter *writer, Dqn_String key, Dqn_f64 value, int decimal_places)
{
    Dqn_FixedString<8> const float_fmt =
        decimal_places > 0
            ? Dqn_FixedStringFmt<8>("%%.%df", decimal_places) // %.<decimal_places>f i.e. %.1f
            : Dqn_FixedStringFmt<8>("%%f");                   // %f

    Dqn_FixedString<32> const fmt_string =
        key.size ? Dqn_FixedStringFmt<32>(R"("%%.*s": %s)", float_fmt) :
                   Dqn_FixedStringFmt<32>(R"(%s)",        float_fmt);

    Dqn_JsonWriter_PreAddItem(writer);
    if (key.size)
        Dqn_StringListAppendFmt(&writer->list, writer->arena, fmt_string.str, DQN_STRING_FMT(key), value);
    else
        Dqn_StringListAppendFmt(&writer->list, writer->arena, fmt_string.str, value);
    Dqn_JsonWriter_PostAddItem(writer);
}

DQN_API void Dqn_JsonWriterF64(Dqn_JsonWriter *writer, Dqn_f64 value, int decimal_places)
{
    Dqn_JsonWriterNamedF64(writer, DQN_STRING("") /*key*/, value, decimal_places);
}
#endif // DQN_WITH_JSON_WRITER

#if defined(DQN_OS_WIN32)
// -------------------------------------------------------------------------------------------------
// NOTE: Dqn_Win Implementation
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_WinErrorMsg Dqn_WinLastError()
{
    Dqn_WinErrorMsg result;
    result.code   = GetLastError();
    result.str[0] = 0;

    unsigned long flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    void *module_to_get_errors_from = nullptr;

    if (result.code >= 12000 && result.code <= 12175) // WinINET Errors
    {
        flags |= FORMAT_MESSAGE_FROM_HMODULE;
        module_to_get_errors_from = GetModuleHandleA("wininet.dll");
    }

    result.size = FormatMessageA(flags,
                                 module_to_get_errors_from,                   // LPCVOID lpSource,
                                 result.code,                                 // unsigned long   dwMessageId,
                                 0,                                           // unsigned long   dwLanguageId,
                                 result.str,                                  // LPSTR   lpBuffer,
                                 DQN_CAST(unsigned long) Dqn_ArrayCountI(result.str), // unsigned long   nSize,
                                 nullptr                                      // va_list * Arguments);
    );

    return result;
}

DQN_API void Dqn_WinMakeProcessDPIAware()
{
    typedef bool SetProcessDpiAwareProc(void);
    typedef bool SetProcessDpiAwarenessProc(PROCESS_DPI_AWARENESS);
    typedef bool SetProcessDpiAwarenessContextProc(void * /*DPI_AWARENESS_CONTEXT*/);

    // NOTE(doyle): Taken from cmuratori/refterm snippet on DPI awareness. It
    // appears we can make this robust by just loading user32.dll and using
    // GetProcAddress on the DPI function. If it's not there, we're on an old
    // version of windows, so we can call an older version of the API.
    void *lib_handle = LoadLibraryA("user32.dll");
    if (auto *set_process_dpi_awareness_context = DQN_CAST(SetProcessDpiAwarenessContextProc *)GetProcAddress(DQN_CAST(HMODULE)lib_handle, "SetProcessDpiAwarenessContext"))
    {
        set_process_dpi_awareness_context(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }
    else if (auto *set_process_dpi_awareness = DQN_CAST(SetProcessDpiAwarenessProc *)GetProcAddress(DQN_CAST(HMODULE)lib_handle, "SetProcessDpiAwareness"))
    {
        set_process_dpi_awareness(PROCESS_PER_MONITOR_DPI_AWARE);
    }
    else if (auto *set_process_dpi_aware = DQN_CAST(SetProcessDpiAwareProc *)GetProcAddress(DQN_CAST(HMODULE)lib_handle, "SetProcessDpiAware"))
    {
        set_process_dpi_aware();
    }
}

DQN_API void Dqn__WinDumpLastError(Dqn_String file, Dqn_String function, Dqn_uint line, char const *fmt, ...)
{
    Dqn_WinErrorMsg msg = Dqn_WinLastError();

    Dqn_isize file_name_size = 0;
    char const *file_name = Dqn_CStringFileNameFromPath(file.str, file.size, &file_name_size);

    // TODO(dqn): Hmmm .. should this be a separate log or part of the above
    // macro. If so we need to make the logging macros more flexible.
    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        Dqn_LogV(Dqn_LogType::Error, dqn__lib.log_user_data, file_name, DQN_CAST(int)file_name_size, function.str, DQN_CAST(int)function.size, line, fmt, args);
        va_end(args);
    }

    if (msg.size)
        Dqn_Log(Dqn_LogType::Error, dqn__lib.log_user_data, file_name, DQN_CAST(int)file_name_size, function.str, DQN_CAST(int)function.size, line, "Error: %.*s", msg.size, msg.str);
    else
        Dqn_Log(Dqn_LogType::Error, dqn__lib.log_user_data, file_name, DQN_CAST(int)file_name_size, function.str, DQN_CAST(int)function.size, line, "FormatMessage error: %d. No error message for: %d", GetLastError(), msg.code);
}

DQN_API int Dqn_WinUTF8ToWCharSizeRequired(Dqn_String src)
{
    int result   = -1;
    int size_int = Dqn_SafeTruncateISizeToInt(src.size);
    if (!size_int)
        return result;

    int required = MultiByteToWideChar(CP_UTF8, 0 /*dwFlags*/, src.str, size_int, nullptr, 0);
    if (required == 0)
    {
        Dqn_WinErrorMsg error = Dqn_WinLastError();
        DQN_LOG_W("Failed to convert wide string '%.*s' to UTF8 string: %.*s",
                  DQN_STRING_FMT(src),
                  error.size, error.str);
        return result;
    }

    result = required;
    return result;
}

DQN_API int Dqn_WinUTF8ToWChar(Dqn_String src, wchar_t *dest, int dest_size)
{
    int result = 0;
    int size_int = Dqn_SafeTruncateISizeToInt(src.size);
    if (!size_int)
        return result;

    result = MultiByteToWideChar(CP_UTF8, 0 /*dwFlags*/, src.str, size_int, dest, dest_size);
    dest[result] = 0;
    return result;
}

DQN_API Dqn_StringW Dqn_WinArenaUTF8ToWChar(Dqn_String src, Dqn_Arena *arena)
{
    Dqn_StringW result = {};
    int required = Dqn_WinUTF8ToWCharSizeRequired(src);
    if (required == -1)
        return result;

    wchar_t *string = Dqn_ArenaNewArray(arena, wchar_t, required + 1, Dqn_ZeroMem::No);
    int string_size = Dqn_WinUTF8ToWChar(src, string, required);

    result = Dqn_StringW{string, string_size};
    return result;
}

DQN_API int Dqn_WinWCharToUTF8SizeRequired(Dqn_StringW src)
{
    int result = 0;
    int size_int = Dqn_SafeTruncateISizeToInt(src.size);
    if (!size_int)
        return result;

    result = WideCharToMultiByte(CP_UTF8, 0 /*dwFlags*/, src.str, size_int, nullptr, 0, nullptr, nullptr);
    if (result == 0)
    {
        Dqn_WinErrorMsg error = Dqn_WinLastError();
        DQN_LOG_W("Failed to convert wide string '%.*s' to UTF8 string: %.*s",
                  DQN_STRING_FMT(src),
                  error.size, error.str);
        return result;
    }

    return result;
}

DQN_API int Dqn_WinWCharToUTF8(Dqn_StringW src, char *dest, int dest_size)
{
    int result   = 0;
    int size_int = Dqn_SafeTruncateISizeToInt(src.size);
    if (!size_int)
        return result;

    result = WideCharToMultiByte(CP_UTF8, 0 /*dwFlags*/, src.str, size_int, dest, dest_size, nullptr, nullptr);
    dest[result] = 0;
    return result;
}

DQN_API Dqn_String Dqn_WinArenaWCharToUTF8(Dqn_StringW src, Dqn_Arena *arena)
{
    Dqn_String result = {};
    int required = Dqn_WinWCharToUTF8SizeRequired(src);
    if (required == -1)
        return result;

    char *string    = Dqn_ArenaNewArray(arena, char, required, Dqn_ZeroMem::No);
    int string_size = Dqn_WinWCharToUTF8(src, string, required);

    result = Dqn_String{string, string_size, string_size};
    return result;
}

DQN_API Dqn_StringW Dqn_WinExecutableDirectoryW(Dqn_Arena *arena)
{
    wchar_t buffer[DQN_OS_WIN32_MAX_PATH];
    int file_path_size = GetModuleFileNameW(nullptr /*module*/, buffer, DQN_CAST(unsigned long)Dqn_ArrayCountI(buffer));
    DQN_HARD_ASSERT_MSG(GetLastError() != ERROR_INSUFFICIENT_BUFFER, "How the hell?");

    int directory_size = file_path_size;
    for (int index = directory_size - 1; index >= 0; index--, directory_size--)
    {
        if (buffer[index] == '\\')
        {
            directory_size--;
            break;
        }
    }

    Dqn_StringW result = {};
    if (directory_size != 0)
    {
        auto *str = Dqn_ArenaNewArray(arena, wchar_t, directory_size + 1, Dqn_ZeroMem::No);
        if (str)
        {
            DQN_MEMCOPY(str, buffer, sizeof(wchar_t) * directory_size);
            str[directory_size] = 0;
            result = Dqn_StringW{str, directory_size};
        }
    }

    return result;
}

DQN_API Dqn_String Dqn_WinCurrentDir(Dqn_Arena *arena, Dqn_Arena *temp_arena, Dqn_String suffix)
{
    Dqn_StringW w_suffix = Dqn_WinArenaUTF8ToWChar(suffix, temp_arena);
    Dqn_StringW curr_dir = Dqn_WinCurrentDirW(temp_arena, w_suffix);
    Dqn_String result    = Dqn_WinArenaWCharToUTF8(curr_dir, arena);
    return result;
}

DQN_API Dqn_StringW Dqn_WinCurrentDirW(Dqn_Arena *arena, Dqn_StringW suffix)
{
    DQN_ASSERT(suffix.size >= 0);
    Dqn_StringW result = {};

    // NOTE: required_size is the size required *including* the null-terminator
    unsigned long         required_size  = GetCurrentDirectoryW(0, nullptr);
    unsigned long         desired_size   = required_size + DQN_CAST(unsigned long) suffix.size;
    Dqn_ArenaScopeData temp_state = Dqn_ArenaBeginScope(arena);

    wchar_t *w_path = Dqn_ArenaNewArray(arena, wchar_t, desired_size, Dqn_ZeroMem::No);
    if (!w_path)
        return result;

    unsigned long bytes_written_wo_null_terminator = GetCurrentDirectoryW(desired_size, w_path);
    if ((bytes_written_wo_null_terminator + 1) != required_size)
    {
        // TODO(dqn): Error
        Dqn_ArenaEndScope(temp_state); // Undo allocations
        return result;
    }

    if (suffix.size)
    {
        DQN_MEMCOPY(w_path + bytes_written_wo_null_terminator, suffix.str, sizeof(suffix.str[0]) * suffix.size);
        w_path[desired_size] = 0;
    }

    result = Dqn_StringW{w_path, desired_size - 1};
    return result;
}

DQN_API bool Dqn_WinFolderWIterate(Dqn_StringW path, Dqn_WinFolderIteratorW *it)
{
    WIN32_FIND_DATAW find_data = {};
    if (it->handle)
    {
        if (FindNextFileW(it->handle, &find_data) == 0)
            return false;
    }
    else
    {
        it->handle = FindFirstFileExW(path.str,              /*LPCWSTR lpFileName,*/
                                      FindExInfoStandard,    /*FINDEX_INFO_LEVELS fInfoLevelId,*/
                                      &find_data,            /*LPVOID lpFindFileData,*/
                                      FindExSearchNameMatch, /*FINDEX_SEARCH_OPS fSearchOp,*/
                                      nullptr,               /*LPVOID lpSearchFilter,*/
                                      FIND_FIRST_EX_LARGE_FETCH /*unsigned long dwAdditionalFlags)*/);

        if (it->handle == INVALID_HANDLE_VALUE)
            return false;
    }

    it->file_name_buf[0] = 0;
    it->file_name        = Dqn_StringW{it->file_name_buf, 0};

    do
    {
        if (find_data.cFileName[0] == '.' || (find_data.cFileName[0] == '.' && find_data.cFileName[1] == '.'))
            continue;

        if (find_data.dwFileAttributes & ~FILE_ATTRIBUTE_DIRECTORY)
        {
            it->file_name.size = Dqn_LStringSize(find_data.cFileName);
            DQN_ASSERT(it->file_name.size < (Dqn_ArrayCountI(it->file_name_buf) - 1));

            DQN_MEMCOPY(it->file_name.str, find_data.cFileName, it->file_name.size * sizeof(wchar_t));
            it->file_name_buf[it->file_name.size] = 0;

            break;
        }
    } while (FindNextFileW(it->handle, &find_data) != 0);

    return true;
}

DQN_API bool Dqn_WinFolderIterate(Dqn_String path, Dqn_WinFolderIterator *it)
{
    wchar_t path_w[DQN_OS_WIN32_MAX_PATH];
    path_w[0] = 0;
    int path_w_size = 0;

    Dqn_WinFolderIteratorW wide_it = {};
    if (it->handle)
        wide_it.handle = it->handle;
    else
    {
        path_w_size = Dqn_WinUTF8ToWChar(path, path_w, DQN_OS_WIN32_MAX_PATH);
        if (path_w[path_w_size - 2] != L'\\' &&
            path_w[path_w_size - 1] != L'*')
        {
            if ((path_w_size + 2) < (DQN_OS_WIN32_MAX_PATH - 1))
            {
                path_w[path_w_size++] = L'\\';
                path_w[path_w_size++] = L'*';
            }
        }
        else if (path_w[path_w_size - 1] == L'\\')
        {
            if ((path_w_size + 1) < (DQN_OS_WIN32_MAX_PATH - 1))
            {
                path_w[path_w_size++] = L'*';
            }
        }

        path_w[path_w_size++] = 0;
        DQN_ASSERT(path_w_size <= DQN_OS_WIN32_MAX_PATH);
    }

    bool result = Dqn_WinFolderWIterate(Dqn_StringW{path_w, path_w_size}, &wide_it);
    it->handle = wide_it.handle;
    if (result)
    {
        int size = Dqn_WinWCharToUTF8(wide_it.file_name, it->file_name_buf, DQN_CAST(int)Dqn_ArrayCountI(it->file_name_buf));
        it->file_name = Dqn_StringInit(it->file_name_buf, size);
    }

    return result;
}

#if defined(DQN_WITH_WIN_NET)
DQN_API Dqn_WinNetHandle Dqn_WinNetHandleInitCString(char const *url, int url_size)
{
    URL_COMPONENTSA components  = {};
    components.dwStructSize     = sizeof(components);
    components.dwHostNameLength = url_size;
    components.dwUrlPathLength  = url_size;

    // Seperate the URL into bits and bobs
    Dqn_WinNetHandle result = {};
    if (!InternetCrackUrlA(url, url_size, 0 /*flags*/, &components))
    {
        Dqn_WinDumpLastError("InternetCrackUrlA");
        return result;
    }

    if (url[url_size] != 0)
    {
        DQN_LOG_E("URL '%.*s' must be null-terminated", url_size, url);
        return result;
    }

    if (components.dwHostNameLength > (sizeof(result.host_name)/sizeof(result.host_name[0])) - 1)
    {
        // TODO(dqn): Error message for host being too long
        return result;
    }

    result.host_name_size = components.dwHostNameLength;
    DQN_MEMCOPY(result.host_name, components.lpszHostName, result.host_name_size);
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

    result.state = Dqn_WinNetHandleState::Initialised;
    return result;
}

DQN_API Dqn_WinNetHandle Dqn_WinNetHandleInitString(Dqn_String url)
{
    Dqn_WinNetHandle result = Dqn_WinNetHandleInitCString(url.str, DQN_CAST(int)url.size);
    return result;
}

DQN_API void Dqn_WinNetHandleFinish(Dqn_WinNetHandle *handle)
{
    if (!Dqn_WinNetHandleIsValid(handle))
        return;

    InternetCloseHandle(handle->internet_open_handle);
    InternetCloseHandle(handle->internet_connect_handle);
    InternetCloseHandle(handle->http_handle);
    handle->internet_open_handle    = nullptr;
    handle->internet_connect_handle = nullptr;
    handle->http_handle             = nullptr;
}

DQN_API bool Dqn_WinNetHandleIsValid(Dqn_WinNetHandle const *handle)
{
    bool result = handle->state >= Dqn_WinNetHandleState::Initialised;
    return result;
}

DQN_API void Dqn_WinNetHandleSetUserAgentCString(Dqn_WinNetHandle *handle, char const *user_agent, int user_agent_size)
{
    if (!Dqn_WinNetHandleIsValid(handle))
        return;

    InternetSetOptionA(handle->internet_open_handle, INTERNET_OPTION_USER_AGENT, (void *)user_agent, user_agent_size);
}

DQN_API bool Dqn_WinNetHandlePump(Dqn_WinNetHandle *handle, char const *http_verb, char *post_data, int post_data_size, char *dest, int dest_size, size_t *download_size)
{
    if (!Dqn_WinNetHandleIsValid(handle))
        return false;

    if (handle->state == Dqn_WinNetHandleState::Initialised)
    {
        DQN_ASSERT(handle->http_handle == nullptr);

        const char *ACCEPT_TYPES[] = {"text/html", nullptr};
        handle->http_handle        = HttpOpenRequestA(handle->internet_connect_handle,
                                                      http_verb,
                                                      handle->url,
                                                      nullptr /*http version*/,
                                                      nullptr /*referrer*/,
                                                      nullptr,
                                                      INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_SECURE,
                                                      0 /*context*/);

        if (HttpSendRequestA(handle->http_handle,
                             nullptr /*headers*/,
                             0 /*headers length*/,
                             post_data,
                             post_data_size))
        {
            handle->state = Dqn_WinNetHandleState::RequestGood;
            return true;
        }
        else
        {
            handle->state = Dqn_WinNetHandleState::RequestFailed;
            Dqn_WinDumpLastError("Failed to send request for: %.*s", handle->host_name_size, handle->host_name);
        }
    }

    if (handle->state == Dqn_WinNetHandleState::RequestFailed)
        return false;

    bool result = true;
    unsigned long bytes_read;
    if (InternetReadFile(handle->http_handle, dest, dest_size, &bytes_read))
    {
        if (bytes_read == 0)
            result = false;
        *download_size = bytes_read;
    }
    else
    {
        *download_size = 0;
        result = false;
    }

    if (!result)
    {
        // NOTE: If it's false here, we've finished downloading/the pumping the
        // handled finished. We can reset the handle state to allow the user to
        // re-use this handle by calling the function again with new post data.
        // IF they need to set a new URL/resource location then they need to
        // make a new handle for that otherwise they can re-use this handle to
        // hit that same end point.
        handle->state = Dqn_WinNetHandleState::Initialised;
        InternetCloseHandle(handle->http_handle);
        handle->http_handle = nullptr;
    }

    return result;
}

struct Dqn_WinNetChunk
{
    char             data[DQN_WIN_NET_HANDLE_DOWNLOAD_SIZE];
    size_t           size;
    Dqn_WinNetChunk *next;
};

DQN_API char *Dqn_WinNetHandlePumpToCString(Dqn_WinNetHandle *handle, char const *http_verb, char *post_data, int post_data_size, Dqn_Arena *arena, size_t *download_size)
{
    Dqn_ThreadScratch scratch     = Dqn_ThreadGetScratch(arena);
    size_t            total_size  = 0;
    Dqn_WinNetChunk * first_chunk = nullptr;
    for (Dqn_WinNetChunk *last_chunk = nullptr;;)
    {
        Dqn_WinNetChunk *chunk = Dqn_ArenaNew(scratch.arena, Dqn_WinNetChunk, Dqn_ZeroMem::Yes);
        bool pump_result       = Dqn_WinNetHandlePump(handle, http_verb, post_data, post_data_size, chunk->data, DQN_WIN_NET_HANDLE_DOWNLOAD_SIZE, &chunk->size);
        if (chunk->size)
        {
            total_size += chunk->size;
            if (first_chunk)
            {
                last_chunk->next = chunk;
                last_chunk       = chunk;
            }
            else
            {
                first_chunk = chunk;
                last_chunk  = chunk;
            }
        }

        if (!pump_result)
            break;
    }

    char *result     = Dqn_ArenaNewArray(arena, char, total_size + 1 /*null-terminator*/, Dqn_ZeroMem::No);
    char *result_ptr = result;
    for (Dqn_WinNetChunk *chunk = first_chunk; chunk; chunk = chunk->next)
    {
        DQN_MEMCOPY(result_ptr, chunk->data, chunk->size);
        result_ptr += chunk->size;
    }

    *download_size     = total_size;
    result[total_size] = 0;
    return result;
}

DQN_API Dqn_String Dqn_WinNetHandlePumpToString(Dqn_WinNetHandle *handle, char const *http_verb, char *post_data, int post_data_size, Dqn_Arena *arena)
{
    size_t     size     = 0;
    char *     download = Dqn_WinNetHandlePumpToCString(handle, http_verb, post_data, post_data_size, arena, &size);
    Dqn_String result   = Dqn_StringInit(download, size);
    return result;
}

DQN_API void Dqn_WinNetHandlePumpToCRTFile(Dqn_WinNetHandle *handle, char const *http_verb, char *post_data, int post_data_size, FILE *file)
{
    for (bool keep_pumping = true; keep_pumping;)
    {
        char buffer[DQN_WIN_NET_HANDLE_DOWNLOAD_SIZE];
        size_t buffer_size = 0;
        keep_pumping = Dqn_WinNetHandlePump(handle, http_verb, post_data, post_data_size, buffer, sizeof(buffer), &buffer_size);
        fprintf(file, "%.*s", (int)buffer_size, buffer);
    }
}

DQN_API char *Dqn_WinNetHandlePumpToMallocCString(Dqn_WinNetHandle *handle, char const *http_verb, char *post_data, int post_data_size, size_t *download_size)
{
    size_t            total_size  = 0;
    Dqn_WinNetChunk * first_chunk = nullptr;
    for (Dqn_WinNetChunk *last_chunk = nullptr;;)
    {
        Dqn_WinNetChunk *chunk = (Dqn_WinNetChunk *)calloc(1, sizeof(Dqn_WinNetChunk));
        bool pump_result       = Dqn_WinNetHandlePump(handle, http_verb, post_data, post_data_size, chunk->data, DQN_WIN_NET_HANDLE_DOWNLOAD_SIZE, &chunk->size);
        if (chunk->size)
        {
            total_size += chunk->size;
            if (first_chunk)
            {
                last_chunk->next = chunk;
                last_chunk       = chunk;
            }
            else
            {
                first_chunk = chunk;
                last_chunk  = chunk;
            }
        }

        if (!pump_result)
            break;
    }

    char *result     = (char *)malloc((total_size + 1) * sizeof(char));
    char *result_ptr = result;
    for (Dqn_WinNetChunk *chunk = first_chunk; chunk;)
    {
        DQN_MEMCOPY(result_ptr, chunk->data, chunk->size);
        result_ptr += chunk->size;

        Dqn_WinNetChunk *prev_chunk = chunk;
        chunk                       = chunk->next;
        free(prev_chunk);
    }

    *download_size     = total_size;
    result[total_size] = 0;
    return result;
}

DQN_API Dqn_String Dqn_WinNetHandlePumpToMallocString(Dqn_WinNetHandle *handle, char const *http_verb, char *post_data, int post_data_size)
{
    size_t     download_size = 0;
    char *     download      = Dqn_WinNetHandlePumpToMallocCString(handle, http_verb, post_data, post_data_size, &download_size);
    Dqn_String result        = Dqn_StringInit(download, download_size);
    return result;
}
#endif // DQN_WITH_WIN_NET
#endif // DQN_OS_WIN32

// -------------------------------------------------------------------------------------------------
// NOTE: Hashing - Dqn_FNV1A[32|64]
// -------------------------------------------------------------------------------------------------
//
// Default values recommended by: http://isthe.com/chongo/tech/comp/fnv/
//
DQN_API Dqn_u32 Dqn_FNV1A32Iterate(void const *bytes, Dqn_isize size, Dqn_u32 hash)
{
    auto buffer = DQN_CAST(Dqn_u8 const *)bytes;
    for (Dqn_isize i = 0; i < size; i++)
        hash = (buffer[i] ^ hash) * 16777619 /*FNV Prime*/;
    return hash;
}

DQN_API Dqn_u32 Dqn_FNV1A32Hash(void const *bytes, Dqn_isize size)
{
    Dqn_u32 result = Dqn_FNV1A32Iterate(bytes, size, DQN_FNV1A32_SEED);
    return result;
}

DQN_API Dqn_u64 Dqn_FNV1A64Iterate(void const *bytes, Dqn_isize size, Dqn_u64 hash)
{
    auto buffer = DQN_CAST(Dqn_u8 const *)bytes;
    for (Dqn_isize i = 0; i < size; i++)
        hash = (buffer[i] ^ hash) * 1099511628211 /*FNV Prime*/;
    return hash;
}

DQN_API Dqn_u64 Dqn_FNV1A64Hash(void const *bytes, Dqn_isize size)
{
    Dqn_u64 result = Dqn_FNV1A64Iterate(bytes, size, DQN_FNV1A64_SEED);
    return result;
}

// -------------------------------------------------------------------------------------------------
// NOTE: Hashing - Dqn_MurmurHash3
// -------------------------------------------------------------------------------------------------

#if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    #define DQN_MMH3_FORCE_INLINE __forceinline
    #define DQN_MMH3_ROTL32(x, y) _rotl(x, y)
    #define DQN_MMH3_ROTL64(x, y) _rotl64(x, y)
#else
    #define DQN_MMH3_FORCE_INLINE inline __attribute__((always_inline))
    #define DQN_MMH3_ROTL32(x, y) ((x) << (y)) | ((x) >> (32 - (y)))
    #define DQN_MMH3_ROTL64(x, y) ((x) << (y)) | ((x) >> (64 - (y)))
#endif

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

DQN_MMH3_FORCE_INLINE Dqn_u32 Dqn_MurmurHash3_GetBlock32(Dqn_u32 const *p, int i)
{
    return p[i];
}

DQN_MMH3_FORCE_INLINE Dqn_u64 Dqn_MurmurHash3_GetBlock64(Dqn_u64 const *p, int i)
{
    return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

DQN_MMH3_FORCE_INLINE Dqn_u32 Dqn_MurmurHash3_FMix32(Dqn_u32 h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

DQN_MMH3_FORCE_INLINE Dqn_u64 Dqn_MurmurHash3_FMix64(Dqn_u64 k)
{
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccd;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53;
    k ^= k >> 33;
    return k;
}

DQN_API Dqn_u32 Dqn_MurmurHash3x86_32(void const *key, int len, Dqn_u32 seed)
{
    const Dqn_u8 *data = (const Dqn_u8 *)key;
    const int nblocks   = len / 4;

    Dqn_u32 h1 = seed;

    const Dqn_u32 c1 = 0xcc9e2d51;
    const Dqn_u32 c2 = 0x1b873593;

    //----------
    // body

    const Dqn_u32 *blocks = (const Dqn_u32 *)(data + nblocks * 4);

    for (int i = -nblocks; i; i++)
    {
        Dqn_u32 k1 = Dqn_MurmurHash3_GetBlock32(blocks, i);

        k1 *= c1;
        k1 = DQN_MMH3_ROTL32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = DQN_MMH3_ROTL32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    //----------
    // tail

    const Dqn_u8 *tail = (const Dqn_u8 *)(data + nblocks * 4);

    Dqn_u32 k1 = 0;

    switch (len & 3)
    {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = DQN_MMH3_ROTL32(k1, 15);
            k1 *= c2;
            h1 ^= k1;
    };

    //----------
    // finalization

    h1 ^= len;

    h1 = Dqn_MurmurHash3_FMix32(h1);

    return h1;
}

DQN_API Dqn_MurmurHash3128 Dqn_MurmurHash3x64_128(void const *key, int len, Dqn_u32 seed)
{
    const Dqn_u8 *data = (const Dqn_u8 *)key;
    const int nblocks   = len / 16;

    Dqn_u64 h1 = seed;
    Dqn_u64 h2 = seed;

    const Dqn_u64 c1 = 0x87c37b91114253d5;
    const Dqn_u64 c2 = 0x4cf5ad432745937f;

    //----------
    // body

    const Dqn_u64 *blocks = (const Dqn_u64 *)(data);

    for (int i = 0; i < nblocks; i++)
    {
        Dqn_u64 k1 = Dqn_MurmurHash3_GetBlock64(blocks, i * 2 + 0);
        Dqn_u64 k2 = Dqn_MurmurHash3_GetBlock64(blocks, i * 2 + 1);

        k1 *= c1;
        k1 = DQN_MMH3_ROTL64(k1, 31);
        k1 *= c2;
        h1 ^= k1;

        h1 = DQN_MMH3_ROTL64(h1, 27);
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        k2 *= c2;
        k2 = DQN_MMH3_ROTL64(k2, 33);
        k2 *= c1;
        h2 ^= k2;

        h2 = DQN_MMH3_ROTL64(h2, 31);
        h2 += h1;
        h2 = h2 * 5 + 0x38495ab5;
    }

    //----------
    // tail

    const Dqn_u8 *tail = (const Dqn_u8 *)(data + nblocks * 16);

    Dqn_u64 k1 = 0;
    Dqn_u64 k2 = 0;

    switch (len & 15)
    {
        case 15:
            k2 ^= ((Dqn_u64)tail[14]) << 48;
        case 14:
            k2 ^= ((Dqn_u64)tail[13]) << 40;
        case 13:
            k2 ^= ((Dqn_u64)tail[12]) << 32;
        case 12:
            k2 ^= ((Dqn_u64)tail[11]) << 24;
        case 11:
            k2 ^= ((Dqn_u64)tail[10]) << 16;
        case 10:
            k2 ^= ((Dqn_u64)tail[9]) << 8;
        case 9:
            k2 ^= ((Dqn_u64)tail[8]) << 0;
            k2 *= c2;
            k2 = DQN_MMH3_ROTL64(k2, 33);
            k2 *= c1;
            h2 ^= k2;

        case 8:
            k1 ^= ((Dqn_u64)tail[7]) << 56;
        case 7:
            k1 ^= ((Dqn_u64)tail[6]) << 48;
        case 6:
            k1 ^= ((Dqn_u64)tail[5]) << 40;
        case 5:
            k1 ^= ((Dqn_u64)tail[4]) << 32;
        case 4:
            k1 ^= ((Dqn_u64)tail[3]) << 24;
        case 3:
            k1 ^= ((Dqn_u64)tail[2]) << 16;
        case 2:
            k1 ^= ((Dqn_u64)tail[1]) << 8;
        case 1:
            k1 ^= ((Dqn_u64)tail[0]) << 0;
            k1 *= c1;
            k1 = DQN_MMH3_ROTL64(k1, 31);
            k1 *= c2;
            h1 ^= k1;
    };

    //----------
    // finalization

    h1 ^= len;
    h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 = Dqn_MurmurHash3_FMix64(h1);
    h2 = Dqn_MurmurHash3_FMix64(h2);

    h1 += h2;
    h2 += h1;

    Dqn_MurmurHash3128 result = {};
    result.e[0]               = h1;
    result.e[1]               = h2;
    return result;
}

//-----------------------------------------------------------------------------
#endif // DQN_IMPLEMENTATION

#if !defined(DQN_STB_SPRINTF_HEADER_ONLY)
#define STB_SPRINTF_IMPLEMENTATION
#ifdef STB_SPRINTF_IMPLEMENTATION

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
#if defined(__ppc64__) || defined(__powerpc64__) || defined(__aarch64__) || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64) || defined(__s390x__)
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
static struct
{
   short temp; // force next field to be 2-byte aligned
   char pair[201];
} stbsp__digitpair =
{
  0,
   "00010203040506070809101112131415161718192021222324"
   "25262728293031323334353637383940414243444546474849"
   "50515253545556575859606162636465666768697071727374"
   "75767778798081828384858687888990919293949596979899"
};

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

static STBSP__ASAN stbsp__uint32 stbsp__strlen_limited(char const *s, stbsp__uint32 limit)
{
   char const * sn = s;

   // get up to 4-byte alignment
   for (;;) {
      if (((stbsp__uintptr)sn & 3) == 0)
         break;

      if (!limit || *sn == 0)
         return (stbsp__uint32)(sn - s);

      ++sn;
      --limit;
   }

   // scan over 4 bytes at a time to find terminating 0
   // this will intentionally scan up to 3 bytes past the end of buffers,
   // but becase it works 4B aligned, it will never cross page boundaries
   // (hence the STBSP__ASAN markup; the over-read here is intentional
   // and harmless)
   while (limit >= 4) {
      stbsp__uint32 v = *(stbsp__uint32 *)sn;
      // bit hack to find if there's a 0 byte in there
      if ((v - 0x01010101) & (~v) & 0x80808080UL)
         break;

      sn += 4;
      limit -= 4;
   }

   // handle the last few characters to find actual size
   while (limit && *sn) {
      ++sn;
      --limit;
   }

   return (stbsp__uint32)(sn - s);
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
            #ifdef STB_SPRINTF_NOUNALIGNED
                if(((stbsp__uintptr)bf) & 3) {
                    bf[0] = f[0];
                    bf[1] = f[1];
                    bf[2] = f[2];
                    bf[3] = f[3];
                } else
            #endif
            {
                *(stbsp__uint32 *)bf = v;
            }
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
         if (f[0] == 'h')
            ++f;  // QUARTERWIDTH
         break;
      // are we 64-bit (unix style)
      case 'l':
         fl |= ((sizeof(long) == 8) ? STBSP__INTMAX : 0);
         ++f;
         if (f[0] == 'l') {
            fl |= STBSP__INTMAX;
            ++f;
         }
         break;
      // are we 64-bit on intmax? (c99)
      case 'j':
         fl |= (sizeof(size_t) == 8) ? STBSP__INTMAX : 0;
         ++f;
         break;
      // are we 64-bit on size_t or ptrdiff_t? (c99)
      case 'z':
         fl |= (sizeof(ptrdiff_t) == 8) ? STBSP__INTMAX : 0;
         ++f;
         break;
      case 't':
         fl |= (sizeof(ptrdiff_t) == 8) ? STBSP__INTMAX : 0;
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
         // get the length, limited to desired precision
         // always limit to ~0u chars since our counts are 32b
         l = stbsp__strlen_limited(s, (pr >= 0) ? pr : ~0u);
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
         cs = 0;
         STBSP__NOTUSED(dp);
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
         // this is the insane action to get the pr to match %g semantics for %f
         if (dp > 0) {
            pr = (dp < (stbsp__int32)l) ? l - dp : 0;
         } else {
            pr = -dp + ((pr > (stbsp__int32)l) ? (stbsp__int32) l : pr);
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
               cs = 0;
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
                  *(stbsp__uint16 *)s = *(stbsp__uint16 *)&stbsp__digitpair.pair[(n % 100) * 2];
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
               *(stbsp__uint32 volatile *)bf = *(stbsp__uint32 volatile *)s;
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
   int length;
   char tmp[STB_SPRINTF_MIN];
} stbsp__context;

static char *stbsp__clamp_callback(const char *buf, void *user, int len)
{
   stbsp__context *c = (stbsp__context *)user;
   c->length += len;

   if (len > c->count)
      len = c->count;

   if (len) {
      if (buf != c->buf) {
         const char *s, *se;
         char *d;
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
      return c->tmp;
   return (c->count >= STB_SPRINTF_MIN) ? c->buf : c->tmp; // go direct into buffer if you can
}

static char * stbsp__count_clamp_callback( const char * buf, void * user, int len )
{
   stbsp__context * c = (stbsp__context*)user;
   (void) sizeof(buf);

   c->length += len;
   return c->tmp; // go direct into buffer if you can
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( vsnprintf )( char * buf, int count, char const * fmt, va_list va )
{
   stbsp__context c;

   if ( (count == 0) && !buf )
   {
      c.length = 0;

      STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__count_clamp_callback, &c, c.tmp, fmt, va );
   }
   else
   {
      int l;

      c.buf = buf;
      c.count = count;
      c.length = 0;

      STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__clamp_callback, &c, stbsp__clamp_callback(0,&c,0), fmt, va );

      // zero-terminate
      l = (int)( c.buf - buf );
      if ( l >= count ) // should never be greater, only equal (or less) than count
         l = count - 1;
      buf[l] = 0;
   }

   return c.length;
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

   return (stbsp__int32)((stbsp__uint64) b >> 63);
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
      ob = (stbsp__int64)xh;                \
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
   ng = (stbsp__int32)((stbsp__uint64) bits >> 63);
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
      if (((stbsp__uint64) bits << 1) == 0) // do zero
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
         *(stbsp__uint16 *)out = *(stbsp__uint16 *)&stbsp__digitpair.pair[(n % 100) * 2];
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

#endif // STB_SPRINTF_IMPLEMENTATION

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
#endif // DQN_STB_SPRINTF_HEADER_ONLY

#if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    #if !defined(DQN_CRT_SECURE_NO_WARNINGS_PREVIOUSLY_DEFINED)
        #undef _CRT_SECURE_NO_WARNINGS
    #endif
#endif
