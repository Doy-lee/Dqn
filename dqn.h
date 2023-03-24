/// @file dqn.h
#if !defined(DQN_H)
#define DQN_H
// ---------------------------------+-----------------------------+---------------------------------
// Table Of Contents                | Compile out with #define    | Description
// ---------------------------------+-----------------------------+---------------------------------
// [SECT-CFGM] Config macros        |                             | Compile time customisation of library
// [SECT-CMAC] Compiler macros      |                             | Macros for the compiler
// [SECT-INCL] Include files        |                             | Standard library Include dependencies
// [SECT-MACR] Macros               |                             | Define macros used in the library
// [SECT-TYPE] Typedefs             |                             | Typedefs used in the library
// [SECT-GSTR] Global Structs       |                             | Forward declare useful structs
// [SECT-W32H] Win32 minimal header | DQN_NO_WIN32_MINIMAL_HEADER | Minimal windows.h subset
// [SECT-INTR] Intrinsics           |                             | Atomics, cpuid, ticket mutex
// [SECT-CARR] Dqn_CArray           |                             | Compile time C-array size
// [SECT-STBS] stb_sprintf          |                             | Portable sprintf
// [SECT-CALL] Dqn_CallSite         |                             | Source code location/tracing
// [SECT-ALLO] Dqn_Allocator        |                             | Generic allocator interface
// [SECT-CSTR] Dqn_CString8         |                             | C-string helpers
// [SECT-STR8] Dqn_String8          |                             | Pointer and length strings
// [SECT-PRIN] Dqn_Print            |                             | Console printing
// [SECT-LLOG] Dqn_Log              |                             | Library logging
// [SECT-VMEM] Dqn_VMem             |                             | Virtual memory allocation
// [SECT-AREN] Dqn_Arena            |                             | Growing bump allocator
// [SECT-AMAP] Dqn_Map              | DQN_NO_MAP                  | Hashtable, collision=chain
// [SECT-DMAP] Dqn_DSMap            | DQN_NO_DSMAP                | Hashtable, 70% max load, PoT size, collision=linear, tombstoneless deletes
// [SECT-DLIB] Dqn_Library          |                             | Library run-time behaviour configuration
// [SECT-FSTR] Dqn_FString8         | DQN_NO_FSTRING8             | Fixed-size strings
// [SECT-STRB] Dqn_String8Builder   |                             |
// [SECT-FARR] Dqn_FArray           | DQN_NO_FARRAY               | Fixed-size arrays
// [SECT-LIST] Dqn_List             |                             | Chunked linked lists, append only
// [SECT-MATH] Math                 | DQN_NO_MATH                 | v2i, V2, V3, V4, Mat4, Rect, RectI32, Lerp
// [SECT-BITS] Dqn_Bit              |                             | Bitset manipulation
// [SECT-SAFE] Dqn_Safe             |                             | Safe arithmetic, casts, asserts
// [SECT-CHAR] Dqn_Char             |                             | Character ascii/digit.. helpers
// [SECT-UTFX] Dqn_UTF              |                             | Unicode helpers
// [SECT-BHEX] Dqn_Hex              | DQN_NO_HEX                  | Binary <-> hex helpers
// [SECT-DATE] Dqn_Date             |                             | Date-time helpers
// [SECT-WIND] Dqn_Win              |                             | Windows OS helpers
// [SECT-WINN] Dqn_WinNet           | DQN_NO_WINNET               | Windows internet download/query helpers
// [SECT-OSYS] Dqn_OS               |                             | Operating-system APIs
// [SECT-FSYS] Dqn_Fs               |                             | Filesystem helpers
// [SECT-MISC] Miscellaneous        |                             | General purpose helpers
// [SECT-TCTX] Dqn_ThreadContext    |                             | Per-thread data structure e.g. temp arenas
// [SECT-JSON] Dqn_JSONBuilder      | DQN_NO_JSON_BUILDER         | Construct json output
// [SECT-FNV1] Dqn_FNV1A            |                             | Hash(x) -> 32/64bit via FNV1a
// [SECT-MMUR] Dqn_MurmurHash3      |                             | Hash(x) -> 32/128bit via MurmurHash3
// ---------------------------------+-----------------------------+---------------------------------
//
// ---------------------------------+-----------------------------+---------------------------------
// [SECT-CFGM] Config macros        |                             | Compile time customisation of library
// -------------------------------------------------------------------------------------------------
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
// #define DQN_MEMSET_BYTE 0
//     Change the byte that DQN_MEMSET will clear memory with. By default this
//     is set to 0. Some of this library API accepts are clear memory parameter
//     to scrub memory after certain operations.
//
// #define DQN_LEAK_TRACKING
//     When defined to some allocating calls in the library will automatically
//     get passed in the file name, function name, line number and an optional
//     leak_msg.
//
// #define DQN_DEBUG_THREAD_CONTEXT
//     Define this macro to record allocation stats for arenas used in the
//     thread context. The thread context arena stats can be printed by using
//     Dqn_Library_DumpThreadContextArenaStat.
//
// ---------------------------------+-----------------------------+---------------------------------
// [SECT-CMAC] Compiler macros      |                             | Macros for the compiler
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

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-INCL] Include files        |                             | Standard library Include dependencies
// ---------------------------------+-----------------------------+---------------------------------
#include <stdarg.h> // va_list
#include <stdio.h>  // fprintf, FILE, stdout, stderr
#include <stdint.h> // [u]int_*, ...
#include <limits.h> // [U]INT_MAX, ...

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-MACR] Macros               |                             | Define macros used in the library
// ---------------------------------+-----------------------------+---------------------------------

// NOTE: Memory allocation dependencies
// -----------------------------------------------------------------------------
#if !defined(DQN_ALLOC)
    #define DQN_ALLOC(size) Dqn_VMem_Reserve(size, true /*commit*/)
#endif

#if !defined(DQN_DEALLOC)
    #define DQN_DEALLOC(ptr, size) Dqn_VMem_Release(ptr, size)
#endif

// NOTE: string.h dependencies
// -----------------------------------------------------------------------------
#if !defined(DQN_MEMCPY) || !defined(DQN_MEMSET) || !defined(DQN_MEMCMP) || !defined(DQN_MEMMOVE)
    #include <string.h>
    #if !defined(DQN_MEMCPY)
        #define DQN_MEMCPY(dest, src, count) memcpy(dest, src, count)
    #endif
    #if !defined(DQN_MEMSET)
        #define DQN_MEMSET(dest, value, count) memset(dest, value, count)
    #endif
    #if !defined(DQN_MEMCMP)
        #define DQN_MEMCMP(ptr1, ptr2, num) memcmp(ptr1, ptr2, num)
    #endif
    #if !defined(DQN_MEMMOVE)
        #define DQN_MEMMOVE(dest, src, num) memmove(dest, src, num)
    #endif
#endif

// NOTE: math.h dependencies
// -----------------------------------------------------------------------------
#if !defined(DQN_SQRTF) || !defined(DQN_SINF) || !defined(DQN_COSF) || !defined(DQN_TANF)
    #include <math.h>
    #define DQN_SQRTF(val) sqrtf(val)
    #if !defined(DQN_SINF)
        #define DQN_SINF(val) sinf(val)
    #endif
    #if !defined(DQN_COSF)
        #define DQN_COSF(val) cosf(val)
    #endif
    #if !defined(DQN_TANF)
        #define DQN_TANF(val) tanf(val)
    #endif
#endif

// NOTE: Math macros
// -----------------------------------------------------------------------------
#define DQN_PI 3.14159265359f

#define DQN_DEGREE_TO_RADIAN(degrees) ((degrees) * (DQN_PI / 180.0f))
#define DQN_RADIAN_TO_DEGREE(radians) ((radians) * (180.f * DQN_PI))

#define DQN_ABS(val) (((val) < 0) ? (-(val)) : (val))
#define DQN_MAX(a, b) ((a > b) ? (a) : (b))
#define DQN_MIN(a, b) ((a < b) ? (a) : (b))
#define DQN_CLAMP(val, lo, hi) DQN_MAX(DQN_MIN(val, hi), lo)
#define DQN_SQUARED(val) ((val) * (val))

// NOTE: Function/variable annotations
// -----------------------------------------------------------------------------
#if defined(DQN_STATIC_API)
    #define DQN_API static
#else
    #define DQN_API
#endif

#define DQN_LOCAL_PERSIST static
#define DQN_FILE_SCOPE static
#define DQN_CAST(val) (val)

#if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    #define DQN_FORCE_INLINE __forceinline
#else
    #define DQN_FORCE_INLINE inline __attribute__((always_inline))
#endif

// NOTE: Preprocessor token tricks
// -----------------------------------------------------------------------------
#define DQN_TOKEN_COMBINE2(x, y) x ## y
#define DQN_TOKEN_COMBINE(x, y) DQN_TOKEN_COMBINE2(x, y)
#define DQN_UNIQUE_NAME(prefix) DQN_TOKEN_COMBINE(prefix, __LINE__)

#define DQN_SWAP(a, b)   \
    do                   \
    {                    \
        auto temp = a;   \
        a        = b;    \
        b        = temp; \
    } while (0)

// NOTE: Compile time evaluation macros
// -----------------------------------------------------------------------------
#define DQN_ISIZEOF(val) DQN_CAST(ptrdiff_t)sizeof(val)
#define DQN_ARRAY_UCOUNT(array) (sizeof(array)/(sizeof((array)[0])))
#define DQN_ARRAY_ICOUNT(array) (Dqn_isize)DQN_ARRAY_UCOUNT(array)
#define DQN_CHAR_COUNT(string) (sizeof(string) - 1)

// NOTE: SI byte units
// -----------------------------------------------------------------------------
#define DQN_BYTES(val) (val)
#define DQN_KILOBYTES(val) (1024ULL * DQN_BYTES(val))
#define DQN_MEGABYTES(val) (1024ULL * DQN_KILOBYTES(val))
#define DQN_GIGABYTES(val) (1024ULL * DQN_MEGABYTES(val))

// NOTE: Duration to sections
// -----------------------------------------------------------------------------
#define DQN_SECONDS_TO_MS(val) ((val) * 1000.0f)
#define DQN_MINS_TO_S(val) ((val) * 60ULL)
#define DQN_HOURS_TO_S(val) (DQN_MINS_TO_S(val) * 60ULL)
#define DQN_DAYS_TO_S(val) (DQN_HOURS_TO_S(val) * 24ULL)
#define DQN_YEARS_TO_S(val) (DQN_DAYS_TO_S(val) * 365ULL)

// NOTE: Debug macros
// -----------------------------------------------------------------------------
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

#if !defined(DQN_MEMSET_BYTE)
    #define DQN_MEMSET_BYTE 0
#endif

// NOTE: Assert macros
// ------------------------------------------------------------------------------------------------
#define DQN_HARD_ASSERT(expr) DQN_HARD_ASSERT_MSG(expr, "")
#define DQN_HARD_ASSERT_MSG(expr, fmt, ...)                         \
    if (!(expr)) {                                                  \
        Dqn_Log_ErrorF("Hard assert triggered " #expr ". " fmt, ##__VA_ARGS__);  \
        DQN_DEBUG_BREAK;                                            \
    }

#if defined(__cplusplus__)
    #define DQN_ZERO_INIT {}
#else
    #define DQN_ZERO_INIT {0}
#endif

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

// NOTE: Defer macros
// ------------------------------------------------------------------------------------------------
#if 0
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
#endif

template <typename Procedure>
struct Dqn_Defer
{
    Procedure proc;
    Dqn_Defer(Procedure p) : proc(p) {}
    ~Dqn_Defer() { proc(); }
};

struct Dqn_DeferHelper
{
    template <typename Lambda>
    Dqn_Defer<Lambda> operator+(Lambda lambda) { return Dqn_Defer<Lambda>(lambda); };
};

#define DQN_DEFER const auto DQN_UNIQUE_NAME(defer_lambda_) = Dqn_DeferHelper() + [&]()

#define DQN_DEFER_LOOP(begin, end)                   \
    for (bool DQN_UNIQUE_NAME(once) = (begin, true); \
         DQN_UNIQUE_NAME(once);                      \
         end, DQN_UNIQUE_NAME(once) = false)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-TYPE] Typedefs             |                             | Typedefs used in the library
// ---------------------------------+-----------------------------+---------------------------------
typedef intptr_t Dqn_isize;
typedef uintptr_t Dqn_usize;
typedef intptr_t Dqn_isize;
typedef float Dqn_f32;
typedef double Dqn_f64;
typedef unsigned int Dqn_uint;
typedef int32_t Dqn_b32;

#define DQN_USIZE_MAX UINTPTR_MAX
#define DQN_ISIZE_MAX INTPTR_MAX
#define DQN_ISIZE_MIN INTPTR_MIN

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-GSTR] Global Structs       |                             | Forward declare useful structs
// ---------------------------------+-----------------------------+---------------------------------
struct Dqn_String8 ///< Pointer and length style UTF8 strings
{
    char      *data; ///< The UTF8 bytes of the string
    Dqn_isize  size; ///< The number of bytes in the string

    #if defined(__cplusplus)
    char const *begin() const { return data; }        ///< Const begin iterator for range-for loops
    char const *end  () const { return data + size; } ///< Const end iterator for range-for loops
    char       *begin()       { return data; }        ///< Begin iterator for range-for loops
    char       *end  ()       { return data + size; } ///< End iterator for range-for loops
    #endif
};

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-W32H] Win32 minimal header | DQN_NO_WIN32_MINIMAL_HEADER | Minimal windows.h subset
// ---------------------------------+-----------------------------+---------------------------------
#if defined(DQN_OS_WIN32)
    #if !defined(DQN_NO_WIN32_MINIMAL_HEADER) && !defined(_INC_WINDOWS)
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
    #endif // !defined(DQN_NO_WIN32_MINIMAL_HEADER) && !defined(_INC_WINDOWS)
#endif // !defined(DQN_OS_WIN32)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-INTR] Intrinsics           |                             | Atomics, cpuid, ticket mutex
// ---------------------------------+-----------------------------+---------------------------------
typedef enum Dqn_ZeroMem {
    Dqn_ZeroMem_No,  ///< Memory can be handed out without zero-ing it out
    Dqn_ZeroMem_Yes, ///< Memory should be zero-ed out before giving to the callee
} Dqn_ZeroMem;

// NOTE: Dqn_Atomic_Add/Exchange return the previous value store in the target
#if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    #include <intrin.h>
    #define Dqn_Atomic_AddU32(target, value) _InterlockedExchangeAdd(DQN_CAST(long volatile *)target, value)
    #define Dqn_Atomic_AddU64(target, value) _InterlockedExchangeAdd64(DQN_CAST(__int64 volatile *)target, value)
    #define Dqn_Atomic_SubU32(target, value) DQN_CAST(unsigned)Dqn_Atomic_AddU32(DQN_CAST(long volatile *)target, -DQN_CAST(long)value)
    #define Dqn_Atomic_SubU64(target, value) DQN_CAST(uint64_t)Dqn_Atomic_AddU64(target, -DQN_CAST(uint64_t)value)
    #define Dqn_Atomic_CompareExchange64(dest, desired_val, prev_val) _InterlockedCompareExchange64(DQN_CAST(__int64 volatile *)dest, desired_val, prev_val)
    #define Dqn_Atomic_CompareExchange32(dest, desired_val, prev_val) _InterlockedCompareExchange(DQN_CAST(long volatile *)dest, desired_val, prev_val)
    #define Dqn_CPUClockCycle() __rdtsc()
    #define Dqn_CompilerReadBarrierAndCPUReadFence _ReadBarrier(); _mm_lfence()
    #define Dqn_CompilerWriteBarrierAndCPUWriteFence _WriteBarrier(); _mm_sfence()
#elif defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
    #include <x86intrin.h>
    #define Dqn_Atomic_AddU32(target, value) __atomic_fetch_add(target, value, __ATOMIC_ACQ_REL)
    #define Dqn_Atomic_AddU64(target, value) __atomic_fetch_add(target, value, __ATOMIC_ACQ_REL)
    #define Dqn_Atomic_SubU32(target, value) __atomic_fetch_sub(target, value, __ATOMIC_ACQ_REL)
    #define Dqn_Atomic_SubU64(target, value) __atomic_fetch_sub(target, value, __ATOMIC_ACQ_REL)
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

/// Atomically set the value into the target using an atomic compare and swap.
/// @param[in,out] target The target pointer to set atomically
/// @param[in] value The value to set atomically into the target
/// @return The value that was last stored in the target
DQN_FORCE_INLINE uint64_t Dqn_Atomic_SetValue64(uint64_t volatile *target, uint64_t value)
{
    #if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    __int64 result;
    do {
        result = *target;
    } while (Dqn_Atomic_CompareExchange64(target, value, result) != result);
    return DQN_CAST(uint64_t)result;
    #elif defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
    uint64_t result = __sync_lock_test_and_set(target, value);
    return result;
    #else
    #error Unsupported compiler
    #endif
}

/// Atomically set the value into the target using an atomic compare and swap.
/// @param[in,out] target The target pointer to set atomically
/// @param[in] value The value to set atomically into the target
/// @return The value that was last stored in the target
DQN_FORCE_INLINE long Dqn_Atomic_SetValue32(long volatile *target, long value)
{
    #if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    long result;
    do {
        result = *target;
    } while (Dqn_Atomic_CompareExchange32(target, value, result) != result);
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
    Dqn_uint array[4]; ///< Values from 'CPUID' instruction for each register (EAX, EBX, ECX, EDX)
};

/// Execute 'CPUID' instruction to query the capabilities of the current CPU.
Dqn_CPUIDRegisters Dqn_CPUID(int function_id);

/// A mutex implemented using an atomic compare and swap on tickets handed out
/// for each critical section.
///
/// This mutex serves ticket in order and will block all other threads until the
/// tickets are returned in order. The thread with the oldest ticket that has
/// not been returned has right of way to execute, all other threads will be
/// blocked in an atomic compare and swap loop. block execution by going into an
/// atomic
///
/// When a thread is blocked by this mutex, a spinlock intrinsic `_mm_pause` is
/// used to yield the CPU and reduce spinlock on the thread. This mutex is not
/// ideal for long blocking operations. This mutex does not issue any syscalls
/// and relies entirely on atomic instructions.
struct Dqn_TicketMutex
{
    unsigned int volatile ticket;  ///< The next ticket to give out to the thread taking the mutex
    unsigned int volatile serving; ///< The ticket ID to block the mutex on until it is returned
};

/// Lock the mutex
void Dqn_TicketMutex_Begin(Dqn_TicketMutex *mutex);

/// Unlock the mutex
void Dqn_TicketMutex_End(Dqn_TicketMutex *mutex);

/// Allocate the next available ticket from the mutex for locking using
/// Dqn_TicketMutex_BeginTicket().
///
/// @param[in] mutex The mutex
///
/// @code
/// Dqn_TicketMutex mutex = {};
/// unsigned int ticket = Dqn_TicketMutex_MakeTicket(&mutex);
/// Dqn_TicketMutex_BeginTicket(&mutex, ticket); // Blocking call until we attain the lock
/// Dqn_TicketMutex_End(&mutex);
/// @endcode
Dqn_uint Dqn_TicketMutex_MakeTicket(Dqn_TicketMutex *mutex);

/// Lock the mutex using the given ticket if possible, otherwise block
/// waiting until the mutex can be locked.
void Dqn_TicketMutex_BeginTicket(Dqn_TicketMutex const *mutex, Dqn_uint ticket);

/// Determine if the mutex can be locked using the given ticket number.
bool Dqn_TicketMutex_CanLock(Dqn_TicketMutex const *mutex, Dqn_uint ticket);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-CARR] Dqn_CArray           |                             | Compile time C-array size
// ---------------------------------+-----------------------------+---------------------------------
/// Calculate the size of a C-array at compile time
/// @param array The C-array to calculate the size for
/// @return The size of the c-array buffer
template <typename T, Dqn_isize N> constexpr Dqn_usize Dqn_CArray_Count   (T const (&array)[N]) { (void)array; return N; }
template <typename T, Dqn_isize N> constexpr Dqn_isize Dqn_CArray_CountI  (T const (&array)[N]) { (void)array; return N; }
template <typename T, int N>       constexpr Dqn_isize Dqn_CArray_CountInt(T const (&array)[N]) { (void)array; return N; }

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-STBS] stb_sprintf          |                             | Portable sprintf
// ---------------------------------+-----------------------------+---------------------------------
/// @cond NO_DOXYYGEN
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
/// @endcond

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-CALL] Dqn_CallSite         |                             | Source code location/tracing
// ---------------------------------+-----------------------------+---------------------------------
typedef struct Dqn_CallSite {
    Dqn_String8  file;
    Dqn_String8  function;
    unsigned int line;
} Dqn_CalSite;

#define DQN_CALL_SITE Dqn_CallSite{DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__}

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-ALLO] Dqn_Allocator        |                             | Generic allocator interface
// ---------------------------------+-----------------------------+---------------------------------
#if defined(DQN_LEAK_TRACKING)
    #if defined(DQN_NO_DSMAP)
        #error "DSMap is required for allocation tracing"
    #endif
    #define DQN_LEAK_TRACE DQN_CALL_SITE,
    #define DQN_LEAK_TRACE_NO_COMMA DQN_CALL_SITE
    #define DQN_LEAK_TRACE_FUNCTION Dqn_CallSite leak_site_,
    #define DQN_LEAK_TRACE_FUNCTION_NO_COMMA Dqn_CallSite leak_site_
    #define DQN_LEAK_TRACE_ARG leak_site_,
    #define DQN_LEAK_TRACE_ARG_NO_COMMA leak_site_
    #define DQN_LEAK_TRACE_UNUSED (void)leak_site_;
#else
    #define DQN_LEAK_TRACE
    #define DQN_LEAK_TRACE_NO_COMMA
    #define DQN_LEAK_TRACE_FUNCTION
    #define DQN_LEAK_TRACE_FUNCTION_NO_COMMA
    #define DQN_LEAK_TRACE_ARG
    #define DQN_LEAK_TRACE_ARG_NO_COMMA
    #define DQN_LEAK_TRACE_UNUSED
#endif

typedef struct Dqn_LeakTrace {
    void         *ptr;             ///< The pointer we are tracking
    Dqn_isize     size;            ///< Size of the allocation
    Dqn_CallSite  call_site;       ///< Call site where the allocation was allocated
    bool          freed;           ///< True if this pointer has been freed
    Dqn_isize     freed_size;      ///< Size of the allocation that has been freed
    Dqn_CallSite  freed_call_site; ///< Call site where the allocation was freed
} Dqn_LeakTrace;

/// Allocate memory
/// @param size The number of bytes to allocate
/// @param zero_mem Flag to indicate if the allocated memory should be zero-ed out
/// @param user_context The user assigned pointer in the allocator
typedef void *Dqn_Allocator_AllocProc(DQN_LEAK_TRACE_FUNCTION size_t size, uint8_t align, Dqn_ZeroMem zero_mem, void *user_context);

/// Deallocate memory
/// @param ptr The pointer to deallocate memory for
/// @param size The number of bytes to deallocate
/// @param user_context The user assigned pointer in the allocator
typedef void Dqn_Allocator_DeallocProc(DQN_LEAK_TRACE_FUNCTION void *ptr, size_t size, void *user_context);

typedef struct Dqn_Allocator {
    void                      *user_context; ///< User assigned pointer that is passed into the allocator functions
    Dqn_Allocator_AllocProc   *alloc;        ///< Memory allocating routine
    Dqn_Allocator_DeallocProc *dealloc;      ///< Memory deallocating routine
} Dqn_Allocator;

/// Allocate bytes from the given allocator
/// @param[in] allocator The allocator to allocate bytes from
/// @param[in] size The amount of bytes to allocator
/// @param[in] zero_mem Flag to indicate if the allocated must be zero-ed out
#define Dqn_Allocator_Alloc(allocator, size, align, zero_mem)                  Dqn_Allocator_Alloc_(DQN_LEAK_TRACE allocator, size, align, zero_mem)
void *Dqn_Allocator_Alloc_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, size_t size, uint8_t align, Dqn_ZeroMem zero_mem);

/// Deallocate the memory from the pointer using the allocator
///
/// The pointer must originally have been allocated from the passed in
/// allocator. The size must also match the size that was originally allocated.
///
/// @param[in] allocator The allocator to allocate bytes from
/// @param[in] ptr The pointer to deallocate
/// @param[in] size The amount of bytes to deallocate.
#define Dqn_Allocator_Dealloc(allocator, ptr, size)                  Dqn_Allocator_Dealloc_(DQN_LEAK_TRACE allocator, ptr, size)
void Dqn_Allocator_Dealloc_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, void *ptr, size_t size);

#define Dqn_Allocator_NewArray(allocator, Type, count, zero_mem) (Type *)Dqn_Allocator_Alloc_(DQN_LEAK_TRACE allocator, sizeof(Type) * count, alignof(Type), zero_mem)
#define Dqn_Allocator_New(allocator, Type, zero_mem) (Type *)Dqn_Allocator_Alloc_(DQN_LEAK_TRACE allocator, sizeof(Type), alignof(Type), zero_mem)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-CSTR] Dqn_CString8         |                             | C-string helpers
// ---------------------------------+-----------------------------+---------------------------------
/// Calculate the size of a cstring literal/array at compile time
/// @param literal The cstring literal/array to calculate the size for
/// @return The size of the cstring not including the null-terminating byte
template <Dqn_isize N> constexpr Dqn_usize Dqn_CString8_ArrayCount(char const (&literal)[N]) { (void)literal; return N - 1; }

/// @copybrief Dqn_CString8_ArrayCount
template <Dqn_isize N> constexpr Dqn_isize Dqn_CString8_ArrayCountI(char const (&literal)[N]) { (void)literal; return N - 1; }

/// @copybrief Dqn_CString8_ArrayCount
template <int N>       constexpr Dqn_isize Dqn_CString8_ArrayCountInt(char const (&literal)[N]) { (void)literal; return N - 1; }

/// Calculate the required size to format the given format cstring.
/// @param[in] fmt The format string to calculate the size for
/// @return The size required to format the string, not including the null
/// terminator.
DQN_API Dqn_isize Dqn_CString8_FSize(char const *fmt, ...);

/// @copydoc Dqn_CString8_FSize
/// @param[in] args The variable argument list to use to format the string
DQN_API Dqn_isize Dqn_CString8_FVSize(char const *fmt, va_list args);

/// Split a string into the substring occuring prior and after the first
/// occurence of the `delimiter`.
///
/// @param[in] string The string to split
/// @param[in] string_size The size of the string
/// @param[in] delimiter The character to split the string on
/// @param[out] lhs_size The size of the left hand side of the split string
/// @param[out] rhs The right hand side of the split string
/// @param[out] rhs_size The size of the right hand side of the split string
///
/// @return The left hand side of the split string. The original pointer is
/// returned if the arguments were invalid.
DQN_API char const *Dqn_CString8_BinarySplit(char const *string, Dqn_isize string_size, char delimiter, Dqn_isize *lhs_size, char **rhs, Dqn_isize *rhs_size);

enum Dqn_CString8EqCase
{
    Dqn_CString8EqCase_Sensitive,
    Dqn_CString8EqCase_Insensitive,
};

/// Compare a string for equality with or without case sensitivity.
/// @param[in] lhs The first string to compare equality with
/// @param[in] rhs The second string to compare equality with
/// @param[in] lhs The first string's size
/// @param[in] rhs The second string's size
/// @param[in] eq_case Set the comparison to be case sensitive or insensitive
/// @return True if the arguments are valid, non-null and the strings
/// are equal, false otherwise.
DQN_API bool Dqn_CString8_Eq(char const *lhs, char const *rhs, Dqn_isize lhs_size = -1, Dqn_isize rhs_size = -1, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive);

/// Compare a string for equality, case insensitive
/// @see Dqn_CString8_Eq
DQN_API bool Dqn_CString8_EqInsensitive(char const *lhs, char const *rhs, Dqn_isize lhs_size = -1, Dqn_isize rhs_size = -1);

/// Check if a string starts with the specified prefix
/// @param[in] string The string to check for the prefix
/// @param[in] prefix The prefix to check against the string
/// @param[in] string_size The size of the string
/// @param[in] prefix_size The size of the prefix string
/// @param[in] eq_case Set the comparison to be case sensitive or insensitive
/// @return True if the string is valid, non-null and has the specified prefix,
/// false otherwise.
DQN_API bool Dqn_CString8_StartsWith(char const *string, char const *prefix, Dqn_isize string_size = -1, Dqn_isize prefix_size = -1, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive);

/// Check if a string starts with the specified prefix, case insensitive
/// @see Dqn_CString8_StartsWith
DQN_API bool Dqn_CString8_StartsWithInsensitive(char const *string, char const *prefix, Dqn_isize string_size = -1, Dqn_isize prefix_size = -1);

/// Check if a string ends with the specified suffix
/// @param[in] string The string to check for the suffix
/// @param[in] suffix The suffix to check against the string
/// @param[in] eq_case Set the comparison to be case sensitive or insensitive
/// @return True if the string has the specified suffix, false otherwise
DQN_API bool Dqn_CString8_EndsWith(char const *string, char const *suffix, Dqn_isize string_size = -1, Dqn_isize suffix_size = -1, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive);

/// Check if a string ends with the specified suffix, case insensitive
/// @param[in] string The string to check for the suffix
/// @param[in] suffix The suffix to check against the string
/// @return True if the string has the specified suffix, false otherwise
DQN_API bool Dqn_CString8_EndsWithInsensitive(char const *string, char const *suffix, Dqn_isize string_size = -1, Dqn_isize suffix_size = -1);

/// Remove the prefix from the given `string` if it starts with it by
/// offsetting the input string.
///
/// @param[in] string The string to trim
/// @param[in] prefix The prefix to trim from the string
/// @param[in] string_size The size of the string
/// @param[in] prefix_size The size of the prefix
/// @param[in] eq_case Set the comparison to be case sensitive or insensitive
/// @param[out] trimmed_string The size of the trimmed string
///
/// @return The trimmed string. The original input string is returned if
/// arguments are invalid or no trim was possible.
DQN_API char const *Dqn_CString8_TrimPrefix(char const *string, char const *prefix, Dqn_isize string_size = -1, Dqn_isize prefix_size = -1, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive, Dqn_isize *trimmed_size = nullptr);

/// Remove the prefix from the given `string` if it ends with it by
/// adjusting the input string size.
///
/// @param[in] string The string to trim
/// @param[in] suffix The suffix to trim from the string
/// @param[in] string_size The size of the string
/// @param[in] suffix_size The size of the suffix
/// @param[in] eq_case Set the comparison to be case sensitive or insensitive
/// @param[out] trimmed_string The size of the trimmed string
///
/// @return The trimmed string. The original input string is returned if
/// arguments are invalid or no trim was possible.
DQN_API char const *Dqn_CString8_TrimSuffix(char const *string, char const *suffix, Dqn_isize string_size = -1, Dqn_isize suffix_size = -1, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive, Dqn_isize *trimmed_size = nullptr);

/// Trim whitespace from the prefix and suffix of the string
///
/// @param[in] string The string to trim
/// @param[in] string_size The size of the string
/// @param[out] trimmed_string The size of the trimmed string
///
/// @return The trimmed string. The original input string is returned if
/// arguments are invalid or no trim was possible.
DQN_API char const *Dqn_CString8_TrimWhitespaceAround(char const *string, Dqn_isize string_size = -1, Dqn_isize *trimmed_size = nullptr);

/// Trim UTF8, UTF16 BE/LE, UTF32 BE/LE byte order mark prefix in the string.
///
/// @param[in] string The string to trim
/// @param[in] string_size The size of the string
/// @param[out] trimmed_string The size of the trimmed string
///
/// @return The trimmed string. The original input string is returned if
/// arguments are invalid or no trim was possible.
DQN_API char const *Dqn_CString8_TrimByteOrderMark(char const *string, Dqn_isize string_size = -1, Dqn_isize *trimmed_size = nullptr);

/// Get the file name from a file path.
///
/// The file name is evaluated by searching from the end of the string backwards
/// to the first occurring path separator '/' or '\'. If no path separator is
/// found, the original string is returned. This function preserves the file
/// extension if there were any.
///
/// @param[in] path A file path on the disk
/// @param[in] size The size of the file path string, if size is '-1' the null
/// terminated string length is evaluated.
/// @param[out] file_name_size The size of the returned file name
///
/// @return The file name in the file path, if none is found, the original path
/// string is returned. Null pointer if arguments are null or invalid.
DQN_API char const *Dqn_CString8_FileNameFromPath(char const *path, Dqn_isize size = -1, Dqn_isize *file_name_size = nullptr);

/// Convert a number represented as a string to a unsigned 64 bit number.
///
/// This function behaves similarly to @see Dqn_CString8_ToI64Checked with the
/// exception that only the '+' prefix is permitted.
DQN_API bool Dqn_CString8_ToU64Checked(char const *buf, Dqn_isize size, char separator, uint64_t *output);

/// Convert a number to an unsigned 64 bit number.
///
/// @copydetails Dqn_CString8_ToU64Checked
///
/// @return The parsed number. On failure, this function optimistically returns
/// the most that could be parsed from the string, i.e. "1234 dog" will return
/// "1234".
DQN_API uint64_t Dqn_CString8_ToU64(char const *buf, Dqn_isize size = -1, char separator = ',');

/// Convert a number represented as a string to a signed 64 bit number.
///
/// The `separator` is an optional digit separator for example, if `separator`
/// is set to ',' then "1,234" will successfully be parsed to '1234'.
///
/// Real numbers are truncated. Both '-' and '+' prefixed strings are permitted,
/// i.e. "+1234" -> 1234 and "-1234" -> -1234. Strings must consist entirely of
/// digits, the seperator or the permitted prefixes as previously mentioned,
/// otherwise this function will return false, i.e. "1234 dog" will cause the
/// function to return false, however, the output is greedily converted and will
/// be evaluated to "1234".
///
/// @param[in] buf The string to convert
/// @param[in] size The size of the string, pass '-1' to calculate the
/// null-terminated string length in the function.
/// @param[in] separator The character used to separate the digits, if any. Set
/// this to 0, if no separators are permitted.
/// @param[out] output The number to write the parsed value to
///
/// @return True if the string was parsed successfully, false otherwise, e.g.
/// non-permitted character found in string.
DQN_API bool Dqn_CString8_ToI64Checked(char const *buf, Dqn_isize size, char separator, int64_t *output);

/// Convert a number represented as a string to a signed 64 bit number.
///
/// This function behaves similarly to @see Dqn_CString8_ToI64Checked
///
/// @return The parsed number. On failure, this function optimistically returns
/// the most that could be parsed from the string, i.e. "1234 dog" will return
/// "1234".
DQN_API int64_t Dqn_CString8_ToI64(char const *buf, Dqn_isize size = -1, char separator = ',');

DQN_API char const *Dqn_CString8_FindMulti(char const *buf, char const *find_list[], Dqn_isize const *find_string_sizes, Dqn_isize find_size, Dqn_isize *match_index, Dqn_isize buf_size = -1);
DQN_API char const *Dqn_CString8_Find(char const *buf, char const *find, Dqn_isize buf_size = -1, Dqn_isize find_size = -1, bool case_insensitive = false);

/// Calculate the string length of the null-terminated string.
/// @param[in] a The string whose length is to be determined
/// @return The length of the string
DQN_API Dqn_isize Dqn_CString8_Size(char const *a);

DQN_API bool        Dqn_CString8_Match(char const *src, char const *find, int find_size);
DQN_API char const *Dqn_CString8_SkipToChar(char const *src, char ch);
DQN_API char const *Dqn_CString8_SkipToNextAlphaNum(char const *src);
DQN_API char const *Dqn_CString8_SkipToNextDigit(char const *src);
DQN_API char const *Dqn_CString8_SkipToNextChar(char const *src);
DQN_API char const *Dqn_CString8_SkipToNextWord(char const *src);
DQN_API char const *Dqn_CString8_SkipToNextWhitespace(char const *src);
DQN_API char const *Dqn_CString8_SkipWhitespace(char const *src);
DQN_API char const *Dqn_CString8_SkipToCharInPlace(char const **src, char ch);
DQN_API char const *Dqn_CString8_SkipToNextAlphaNumInPlace(char const **src);
DQN_API char const *Dqn_CString8_SkipToNextCharInPlace(char const **src);
DQN_API char const *Dqn_CString8_SkipToNextWhitespaceInPlace(char const **src);
DQN_API char const *Dqn_CString8_SkipToNextWordInPlace(char const **src);
DQN_API char const *Dqn_CString8_SkipWhitespaceInPlace(char const **src);
DQN_API bool        Dqn_CString8_IsAllDigits(char const *src, Dqn_isize size);

DQN_API Dqn_isize Dqn_CString16_Size(wchar_t const *a);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-STR8] Dqn_String8          |                             | Pointer and length strings
// ---------------------------------+-----------------------------+---------------------------------
/// Construct a UTF8 c-string literal into a Dqn_String8 referencing a string
/// stored in the data-segment. This string is read-only.
#define DQN_STRING8(string) Dqn_String8{(char *)(string), sizeof(string) - 1}

/// Construct a UTF16 c-string literal into a Dqn_String16 referencing a string
/// stored in the data-segment. This string is read-only.
#define DQN_STRING16(string) Dqn_String16{(wchar_t *)(string), (sizeof(string)/sizeof(string[0])) - 1}

/// Unpack a string into arguments for printing a string into a printf style
/// format string.
#define DQN_STRING_FMT(string) (int)((string).size), (string).data

struct Dqn_String8Link {
    Dqn_String8      string; ///< The string
    Dqn_String8Link *next;   ///< The next string in the linked list
};

struct Dqn_String16 /// A pointer and length style string that holds slices to UTF16 bytes.
{
    wchar_t   *data; ///< The UTF16 bytes of the string
    Dqn_isize  size; ///< The number of bytes in the string

    #if defined(__cplusplus)
    wchar_t const *begin() const { return data; }        ///< Const begin iterator for range-for loops
    wchar_t const *end  () const { return data + size; } ///< Const end iterator for range-for loops
    wchar_t       *begin()       { return data; }        ///< Begin iterator for range-for loops
    wchar_t       *end  ()       { return data + size; } ///< End iterator for range-for loops
    #endif
};

/// Initialise a string from a pointer and length
/// The string is invalid (i.e. Dqn_String8_IsValid() returns false) if size is
/// negative or the string is null.
DQN_API Dqn_String8 Dqn_String8_Init(char const *string, Dqn_isize size);

/// Initialise a string from a cstring
/// The cstring must be null-terminated as its length is evaluated using
/// strlen. The string is invalid (i.e. Dqn_String8_IsValid() returns false) if
/// size is negative or the string is null.
DQN_API Dqn_String8 Dqn_String8_InitCString(char const *string);

/// Create a string from a printf style format string
/// @param[in] allocator The allocator the string will be allocated from
/// @param[in] fmt The printf style format cstring
#define Dqn_String8_InitF(allocator, fmt, ...) Dqn_String8_InitF_(DQN_LEAK_TRACE allocator, fmt, ## __VA_ARGS__)
DQN_API Dqn_String8 Dqn_String8_InitF_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, char const *fmt, ...);

/// Create a string from a printf style format string using a va_list
/// @param[in] arena The allocator the string will be allocated from
/// @param[in] fmt The printf style format cstring
/// @param[in] va The variable argument list
#define Dqn_String8_InitFV(allocator, fmt, args) Dqn_String8_InitFV_(DQN_LEAK_TRACE allocator, fmt, args)
DQN_API Dqn_String8 Dqn_String8_InitFV_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, char const *fmt, va_list args);

/// Determine if the values of the given string are valid
/// A string is invalid if size is negative or the string is null.
/// @return True if the string is valid, false otherwise.
DQN_API bool Dqn_String8_IsValid(Dqn_String8 string);

/// Create a slice from a pre-existing string.
/// The requested slice is clamped to within the bounds of the original string.
/// @param[in] string The string to slice
/// @param[in] offset The starting byte to slice from
/// @param[in] size The size of the slice
/// @return The sliced string
DQN_API Dqn_String8 Dqn_String8_Slice(Dqn_String8 string, Dqn_isize offset, Dqn_isize size);

/// Create an empty string with the requested size
/// @param[in] allocator The allocator the string will be allocated from
/// @param[in] size The size in bytes of the string to allocate
/// @param[in] zero_mem Enum to indicate if the string's memory should be cleared
#define Dqn_String8_Allocate(allocator, size, zero_mem) Dqn_String8_Allocate_(DQN_LEAK_TRACE allocator, size, zero_mem)
DQN_API Dqn_String8 Dqn_String8_Allocate_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, Dqn_isize size, Dqn_ZeroMem zero_mem);

/// Create a copy of the given cstring
/// @param[in] allocator The allocator the string will be allocated from
/// @param[in] string The cstring to copy
/// @param[in] size The size of the cstring to copy. This cannot be <= 0
/// @return A copy of the string, invalid string if any argument was invalid.
#define Dqn_String8_CopyCString(allocator, string, size) Dqn_String8_CopyCString_(DQN_LEAK_TRACE allocator, string, size)
DQN_API Dqn_String8 Dqn_String8_CopyCString_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, char const *string, Dqn_isize size);

/// Create a copy of the given string
/// @param[in] allocator The allocator the string will be allocated from
/// @param[in] string The string to copy
/// @return A copy of the string, invalid string if any argument was invalid.
#define Dqn_String8_Copy(allocator, string) Dqn_String8_Copy_(DQN_LEAK_TRACE allocator, string)
DQN_API Dqn_String8 Dqn_String8_Copy_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, Dqn_String8 string);

/// @see Dqn_CString8_BinarySplit
DQN_API Dqn_String8 Dqn_String8_BinarySplit(Dqn_String8 string, char delimiter, Dqn_String8 *rhs);

/// @see Dqn_CString8_Eq
DQN_API bool Dqn_String8_Eq(Dqn_String8 lhs, Dqn_String8 rhs, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive);

/// @see Dqn_CString8_EqInsensitive
DQN_API bool Dqn_String8_EqInsensitive(Dqn_String8 lhs, Dqn_String8 rhs);

/// @see Dqn_CString8_StartsWith
DQN_API bool Dqn_String8_StartsWith(Dqn_String8 string, Dqn_String8 prefix, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive);

/// @see Dqn_CString8_StartsWithInsensitive
DQN_API bool Dqn_String8_StartsWithInsensitive(Dqn_String8 string, Dqn_String8 prefix);

/// @see Dqn_CString8_EndsWith
DQN_API bool Dqn_String8_EndsWith(Dqn_String8 string, Dqn_String8 prefix, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive);

/// @see Dqn_CString8_EndsWithInsensitive
DQN_API bool Dqn_String8_EndsWithInsensitive(Dqn_String8 string, Dqn_String8 prefix);

/// @see Dqn_CString8_TrimPrefix
DQN_API Dqn_String8 Dqn_String8_TrimPrefix(Dqn_String8 string, Dqn_String8 prefix, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive);

/// @see Dqn_CString8_TrimSuffix
DQN_API Dqn_String8 Dqn_String8_TrimSuffix(Dqn_String8 string, Dqn_String8 suffix, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive);

/// @see Dqn_CString8_TrimWhitespaceAround
DQN_API Dqn_String8 Dqn_String8_TrimWhitespaceAround(Dqn_String8 string);

/// @see Dqn_CString8_TrimByteOrderMark
DQN_API Dqn_String8 Dqn_String8_TrimByteOrderMark(Dqn_String8 string);

/// @see Dqn_CString8_FileNameFromPath
DQN_API Dqn_String8 Dqn_String8_FileNameFromPath(Dqn_String8 path);

/// @see Dqn_CString8_ToU64Checked
DQN_API bool Dqn_String8_ToU64Checked(Dqn_String8 string, char separator, uint64_t *output);

/// @see Dqn_CString8_ToU64
DQN_API uint64_t Dqn_String8_ToU64(Dqn_String8 string, char separator);

/// @see Dqn_CString8_ToI64Checked
DQN_API bool Dqn_String8_ToI64Checked(Dqn_String8 string, char separator, int64_t *output);

/// @see Dqn_CString8_ToI64
DQN_API int64_t Dqn_String8_ToI64(Dqn_String8 string, char separator);

/// Split a string by the delimiting character.
/// This function can evaluate the number of splits required in the return value
/// by setting `splits` to null and `splits_count` to 0.
/// @param[in] string The source string to split
/// @param[in] delimiter The substring to split the string on
/// @param[out] splits (Optional) The destination array to write the splits to.
/// @param[in] splits_count The number of splits that can be written into the
/// `splits` array.
/// @return The number of splits in the `string`. If the return value is >=
/// 'splits_count' then there are more splits in the string than can be written
/// to the `splits` array. The number of splits written is capped to the
/// capacity given by the caller, i.e. `splits_count`. This function should be
/// called again with a sufficiently sized array if all splits are desired.
DQN_API Dqn_isize   Dqn_String8_Split(Dqn_String8 string, Dqn_String8 delimiter, Dqn_String8 *splits, Dqn_isize splits_count);

DQN_API bool        Dqn_String8_IsAllDigits(Dqn_String8 string);
DQN_API bool        Dqn_String8_IsAllHex(Dqn_String8 string);
DQN_API bool        Dqn_String8_HasChar(Dqn_String8 string, char ch);

/// Remove the substring denoted by the begin index and the size from the string
/// string in-place using MEMMOVE to shift the string back.
DQN_API void        Dqn_String8_Remove(Dqn_String8 *string, Dqn_isize begin, Dqn_isize size);

/// @param[in] start_index Set an index within the string string to start the
/// search from, if not desired, set to 0
/// @return The index of the matching find, -1 if it is not found
DQN_API Dqn_isize   Dqn_String8_FindOffset(Dqn_String8 string, Dqn_String8 find, Dqn_isize start_index, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive);

/// @param start_index Set an index within the string string to start the search
/// from, if not desired, set to 0
/// @return A string that points to the matching find, otherwise a 0 length string.
DQN_API Dqn_String8 Dqn_String8_Find(Dqn_String8 string, Dqn_String8 find, Dqn_isize start_index, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive);

DQN_API Dqn_String8 Dqn_String8_Replace(Dqn_String8 string, Dqn_String8 find, Dqn_String8 replace, Dqn_isize start_index, Dqn_Allocator allocator, Dqn_CString8EqCase eq_case = Dqn_CString8EqCase_Sensitive);
DQN_API Dqn_String8 Dqn_String8_ReplaceInsensitive(Dqn_String8 string, Dqn_String8 find, Dqn_String8 replace, Dqn_isize start_index, Dqn_Allocator allocator);

#if defined(__cplusplus)
bool operator==(Dqn_String8 const &lhs, Dqn_String8 const &rhs);
bool operator!=(Dqn_String8 const &lhs, Dqn_String8 const &rhs);
#endif

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-PRIN] Dqn_Print            |                             | Console printing
// ---------------------------------+-----------------------------+---------------------------------
enum Dqn_PrintStd
{
    Dqn_PrintStd_Out,
    Dqn_PrintStd_Err,
};

DQN_API void Dqn_Print_Std(Dqn_PrintStd std_handle, Dqn_String8 string);
DQN_API void Dqn_Print_StdFV(Dqn_PrintStd std_handle, char const *fmt, va_list args);
DQN_API void Dqn_Print_StdF(Dqn_PrintStd std_handle, char const *fmt, ...);

DQN_API void Dqn_Print_StdLn(Dqn_PrintStd std_handle, Dqn_String8 string);
DQN_API void Dqn_Print_StdLnFV(Dqn_PrintStd std_handle, char const *fmt, va_list args);
DQN_API void Dqn_Print_StdLnF(Dqn_PrintStd std_handle, char const *fmt, ...);

#define Dqn_Print(string) Dqn_Print_Std(Dqn_PrintStd_Out, string)
#define Dqn_Print_FV(fmt, args) Dqn_Print_StdFV(Dqn_PrintStd_Out, fmt, args)
#define Dqn_Print_F(fmt, ...) Dqn_Print_StdF(Dqn_PrintStd_Out, fmt, ## __VA_ARGS__)

#define Dqn_Print_Ln(string) Dqn_Print_StdLn(Dqn_PrintStd_Out, string)
#define Dqn_Print_LnFV(fmt, args) Dqn_Print_StdLnFV(Dqn_PrintStd_Out, fmt, args)
#define Dqn_Print_LnF(fmt, ...) Dqn_Print_StdLnF(Dqn_PrintStd_Out, fmt, ## __VA_ARGS__)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-LLOG] Dqn_Log              |                             | Library logging
// ---------------------------------+-----------------------------+---------------------------------
enum Dqn_LogType
{
    Dqn_LogType_Error,
    Dqn_LogType_Info,
    Dqn_LogType_Warning,
    Dqn_LogType_Debug,
};

/// The logging procedure of the library. Users can override the default logging
/// function by setting the logging function pointer in Dqn_Library. This
/// function will be invoked every time a log is recorded using the following
/// functions.
///
/// @param[in] log_type This value is one of the Dqn_LogType values if the log
/// was generated from one of the default categories. -1 if the log is not from
/// one of the default categories.
typedef void Dqn_LogProc(Dqn_String8 type, int log_type, void *user_data, Dqn_CallSite call_site, char const *fmt, va_list va);

#define Dqn_Log_ErrorF(fmt, ...) Dqn_Log_TypeFCallSite(Dqn_LogType_Error, DQN_CALL_SITE, fmt, ## __VA_ARGS__)
#define Dqn_Log_DebugF(fmt, ...) Dqn_Log_TypeFCallSite(Dqn_LogType_Debug, DQN_CALL_SITE, fmt, ## __VA_ARGS__)
#define Dqn_Log_WarningF(fmt, ...) Dqn_Log_TypeFCallSite(Dqn_LogType_Warning, DQN_CALL_SITE, fmt, ## __VA_ARGS__)
#define Dqn_Log_InfoF(fmt, ...) Dqn_Log_TypeFCallSite(Dqn_LogType_Info, DQN_CALL_SITE, fmt, ## __VA_ARGS__)

#define Dqn_Log_ErrorFV(fmt, args) Dqn_Log_TypeFVCallSite(Dqn_LogType_Error, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_DebugFV(fmt, args) Dqn_Log_TypeFVCallSite(Dqn_LogType_Debug, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_WarningFV(fmt, args) Dqn_Log_TypeFVCallSite(Dqn_LogType_Warning, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_InfoFV(fmt, args) Dqn_Log_TypeFVCallSite(Dqn_LogType_Info, DQN_CALL_SITE, fmt, args)

#define Dqn_Log_FV(type, fmt, args) Dqn_Log_FVCallSite(type, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_F(type, fmt, ...) Dqn_Log_FVCallSite(type, fmt, ## __VA_ARGS__)

DQN_API void Dqn_Log_TypeFVCallSite(Dqn_LogType type, Dqn_CallSite call_site, char const *fmt, va_list va);
DQN_API void Dqn_Log_TypeFCallSite(Dqn_LogType type, Dqn_CallSite call_site, char const *fmt, ...);
DQN_API void Dqn_Log_FVCallSite(Dqn_String8 type, Dqn_CallSite call_site, char const *fmt, va_list va);
DQN_API void Dqn_Log_FCallSite(Dqn_String8 type, Dqn_CallSite call_site, char const *fmt, ...);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-VMEM] Dqn_VMem             |                             | Virtual memory allocation
// ---------------------------------+-----------------------------+---------------------------------
DQN_API void *Dqn_VMem_Reserve(Dqn_usize size, bool commit);
DQN_API bool Dqn_VMem_Commit(void *ptr, Dqn_usize size);
DQN_API void Dqn_VMem_Decommit(void *ptr, Dqn_usize size);
DQN_API void Dqn_VMem_Release(void *ptr, Dqn_usize size);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-AREN] Dqn_Arena            |                             | Growing bump allocator
// ---------------------------------+-----------------------------+---------------------------------
Dqn_isize const DQN_ARENA_MIN_BLOCK_SIZE = DQN_KILOBYTES(4);

struct Dqn_ArenaBlock
{
    void           *memory;    ///< The backing memory of the block
    Dqn_isize       size;      ///< The size of the block
    Dqn_isize       used;      ///< The number of bytes used up in the block. Always less than the committed amount.
    Dqn_isize       committed; ///< The number of physically backed bytes by the OS.
    Dqn_ArenaBlock *prev;      ///< The previous linked block
    Dqn_ArenaBlock *next;      ///< The next linked block
};

struct Dqn_ArenaStatString
{
    char     data[256];
    uint16_t size;
};

struct Dqn_ArenaStat
{
    Dqn_isize capacity;     ///< Total allocating capacity of the arena in bytes
    Dqn_isize used;         ///< Total amount of bytes used in the arena
    Dqn_isize wasted;       ///< Orphaned space in blocks due to allocations requiring more space than available in the active block
    uint32_t  blocks;       ///< Number of memory blocks in the arena
    Dqn_isize syscalls;     ///< Number of memory allocation syscalls into the OS

    Dqn_isize capacity_hwm; ///< High-water mark for 'capacity'
    Dqn_isize used_hwm;     ///< High-water mark for 'used'
    Dqn_isize wasted_hwm;   ///< High-water mark for 'wasted'
    uint32_t  blocks_hwm;   ///< High-water mark for 'blocks'
};

struct Dqn_Arena
{
    Dqn_isize       min_block_size;
    Dqn_ArenaBlock *curr;           ///< The current memory block of the arena
    Dqn_ArenaBlock *tail;           ///< The tail memory block of the arena
    Dqn_ArenaStat   stats;          ///< Current arena stats, reset when reset usage is invoked.
};

struct Dqn_ArenaTempMemory
{
    Dqn_Arena      *arena;     ///< The arena the scope is for
    Dqn_ArenaBlock *curr;      ///< The current block of the arena at the beginning of the scope
    Dqn_ArenaBlock *tail;      ///< The tail block of the arena at the beginning of the scope
    Dqn_isize       curr_used; ///< The current used amount of the current block
    Dqn_ArenaStat   stats;     ///< The stats of the arena at the beginning of the scope
};

// Automatically begin and end a temporary memory scope on object construction
// and destruction respectively.
#define Dqn_ArenaTempMemoryScope(arena) Dqn_ArenaTempMemoryScope_(DQN_LEAK_TRACE arena)
struct Dqn_ArenaTempMemoryScope_
{
    Dqn_ArenaTempMemoryScope_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena);
    ~Dqn_ArenaTempMemoryScope_();
    Dqn_ArenaTempMemory temp_memory;
    #if defined(DQN_LEAK_TRACKING)
    Dqn_CallSite        leak_site__;
    #endif
};

/// Grow the arena's capacity by the requested size
/// @param[in] size The size in bytes to expand the capacity of the arena
/// @param[in] commit The amount of bytes to request to be physically backed by
/// pages from the OS.
/// @return True if the arena was successfully grown, false otherwise.
#define Dqn_Arena_Grow(arena, size, commit) Dqn_Arena_Grow_(DQN_LEAK_TRACE arena, size, commit)
DQN_API bool Dqn_Arena_Grow_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, Dqn_isize size, Dqn_isize commit);

/// Allocate bytes from the given arena
/// @return A pointer to the allocated bytes. Null if the arena failed to
/// allocate memory
#define Dqn_Arena_Allocate(arena, size, align, zero_mem) Dqn_Arena_Allocate_(DQN_LEAK_TRACE arena, size, align, zero_mem)
DQN_API void *Dqn_Arena_Allocate_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, Dqn_isize size, uint8_t align, Dqn_ZeroMem zero_mem);

/// Allocate a single object from the arena. No constructor is called.
/// @return A pointer to the allocated object. Null if the arena failed to
/// allocate memory
#define Dqn_Arena_New(arena, Type, zero_mem) (Type *)Dqn_Arena_Allocate_(DQN_LEAK_TRACE arena, sizeof(Type), alignof(Type), zero_mem)

/// Allocate an array of objects from the arena.
/// @return A pointer to the allocated object. Null if the arena failed to
/// allocate memory
#define Dqn_Arena_NewArray(arena, Type, count, zero_mem) (Type *)Dqn_Arena_Allocate_(DQN_LEAK_TRACE arena, sizeof(Type) * count, alignof(Type), zero_mem)

/// Copy an object by allocating memory for the copy from the arena
/// @return A pointer to the allocated object. Null if the arena failed to
/// allocate memory
#define Dqn_Arena_Copy(arena, Type, src, count) (Type *)Dqn_Arena_Copy_(DQN_LEAK_TRACE arena, src, sizeof(*src) * count, alignof(Type))
DQN_API void *Dqn_Arena_Copy_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, void *src, Dqn_isize size, uint8_t alignment);

/// Copy an object by allocating memory for the copy from the arena. The object
/// has a null-terminating byte at the end of the stream.
/// @return A pointer to the allocated object. Null if the arena failed to
/// allocate memory
#define Dqn_Arena_CopyZ(arena, Type, src, count) (Type *)Dqn_Arena_CopyZ_(DQN_LEAK_TRACE arena, src, sizeof(*src) * count, alignof(Type))
DQN_API void *Dqn_Arena_CopyZ_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, void *src, Dqn_isize size, uint8_t alignment);

/// Set the arena's current block to the first block in the linked list of blocks
/// and mark all blocks free.
/// @param[in] zero_mem When yes, the memory is cleared using DQN_MEMSET with the
/// value of DQN_MEMSET_BYTE
DQN_API void Dqn_Arena_Reset(Dqn_Arena *arena, Dqn_ZeroMem zero_mem);

/// Free the arena returning all memory back to the OS
/// @param[in] zero_mem: When true, the memory is cleared using DQN_MEMSET with
/// the value of DQN_MEMSET_BYTE
#define Dqn_Arena_Free(arena, zero_mem) Dqn_Arena_Free_(DQN_LEAK_TRACE arena, zero_mem)
DQN_API void Dqn_Arena_Free_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, Dqn_ZeroMem zero_mem);

DQN_API Dqn_Allocator Dqn_Arena_Allocator(Dqn_Arena *arena);

/// Begin an allocation scope where all allocations between begin and end calls
/// will be reverted. Useful for short-lived or highly defined lifetime
/// allocations. An allocation scope is invalidated if the arena is freed
/// between the begin and end call.
DQN_API Dqn_ArenaTempMemory Dqn_Arena_BeginTempMemory(Dqn_Arena *arena);

/// End an allocation scope previously begun by calling begin scope.
#define Dqn_Arena_EndTempMemory(arena_temp_memory) Dqn_Arena_EndTempMemory_(DQN_LEAK_TRACE arena_temp_memory)
DQN_API void Dqn_Arena_EndTempMemory_(DQN_LEAK_TRACE_FUNCTION Dqn_ArenaTempMemory arena_temp_memory);

/// Dump the stats of the given arena to a string
/// @param[in] arena The arena to dump stats for
/// @return A stack allocated string containing the stats of the arena
DQN_API Dqn_ArenaStatString Dqn_Arena_StatString(Dqn_ArenaStat const *stat);

/// Dump the stats of the given arena to the memory log-stream.
/// @param[in] arena The arena to dump stats for
DQN_API void Dqn_Arena_LogStats(Dqn_Arena const *arena);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-AMAP] Dqn_Map              | DQN_NO_MAP                  | Hashtable, collision=chain
// ---------------------------------+-----------------------------+---------------------------------
#if !defined(DQN_NO_DSMAP)
template <typename T>
struct Dqn_MapEntry
{
    uint64_t      hash;
    T             value;
    Dqn_MapEntry *next;
};

/// A hash-table implementation that uses a linked-list allocated by an arena
/// for collisions.
template <typename T>
struct Dqn_Map
{
    Dqn_Arena         *arena;
    Dqn_MapEntry<T>  **slots;
    Dqn_isize          size;  ///< The number of slots

    // NOTE: Sum count and chain_count for total items in the list.
    Dqn_isize          count;       ///< The total number of top-level slots in the 'values' list occupied
    Dqn_isize          chain_count; ///< The total number of chained elements in 'values'
    Dqn_MapEntry<T>   *free_list;
};

enum struct Dqn_MapCollideRule
{
    Overwrite,
    Chain,
    Fail,
};

template <typename T> Dqn_Map<T> Dqn_Map_InitWithArena(Dqn_Arena *arena, Dqn_isize size = 0);

#define Dqn_Map_FindOrAdd(map, hash, rule) Dqn_Map_FindOrAdd_(DQN_LEAK_TRACE map, hash, rule)
template <typename T> Dqn_MapEntry<T> *Dqn_Map_FindOrAdd_(DQN_LEAK_TRACE_FUNCTION Dqn_Map<T> *map, uint64_t hash, Dqn_MapCollideRule rule);

#define Dqn_Map_Add(map, hash, value, rule) Dqn_Map_Add_(DQN_LEAK_TRACE map, hash, value, rule)
template <typename T> Dqn_MapEntry<T> *Dqn_Map_Add_(DQN_LEAK_TRACE_FUNCTION Dqn_Map<T> *map, uint64_t hash, T *value, Dqn_MapCollideRule rule);

#define Dqn_Map_AddCopy(map, hash, value, rule) Dqn_Map_AddCopy_(DQN_LEAK_TRACE map, hash, value, rule)
template <typename T> Dqn_MapEntry<T> *Dqn_Map_AddCopy_(DQN_LEAK_TRACE_FUNCTION Dqn_Map<T> *map, uint64_t hash, const T &value, Dqn_MapCollideRule rule);

template <typename T> Dqn_MapEntry<T> *Dqn_Map_Get(Dqn_Map<T> *map, uint64_t hash);

template <typename T> void Dqn_Map_Erase(Dqn_Map<T> *map, uint64_t hash, Dqn_ZeroMem zero_mem);
#endif // !defined DQN_NO_MAP

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-DMAP] Dqn_DSMap            | DQN_NO_DSMAP                | Hashtable, 70% max load, PoT size, collision=linear, tombstoneless deletes
// ---------------------------------+-----------------------------+---------------------------------
#if !defined(DQN_NO_DSMAP)

/// If a DSMap is initialized to zero and does *not* call the init function then
/// the table with be lazily initialised to this capacity on first write to the
/// hash table. This size must be a power of two.
#if !defined(DQN_DSMAP_MIN_SIZE)
    #define DQN_DSMAP_MIN_SIZE 4096
    static_assert((DQN_DSMAP_MIN_SIZE & (DQN_DSMAP_MIN_SIZE - 1)) == 0, "DSMap lazy initialisation size must be a power of two");
#endif

#define DQN_DS_MAP_API template <typename K, typename V> DQN_API
#define DQN_DS_MAP     Dqn_DSMap<K, V>

template <typename K, typename V>
struct Dqn_DSMapItem
{
    K                    key;
    V                    value;
    uint8_t              occupied;
    Dqn_DSMapItem<K, V> *next;
    Dqn_DSMapItem<K, V> *prev;
};

/// The hashing function prototype
using Dqn_DSMapHashProc = uint64_t(void *data, size_t size);

/// A hash table configured using the presets recommended by Demitri Spanos from
/// the Handmade Network,
/// - 70% max load
/// - power of two capacity
/// - linear probing on collision
/// - tombstoneless deletes (e.g. deletes slots get filled with linear probed
/// entries if there are any).
template <typename K, typename V>
struct Dqn_DSMap
{
    Dqn_Arena            arena;
    Dqn_DSMapItem<K, V> *slots;
    Dqn_isize            size;      ///< The number of slots
    Dqn_isize            count;     ///< The number of slots occupied in the list
    Dqn_DSMapItem<K, V> *link_head; ///< Linked list of items allocated in the map
    Dqn_DSMapItem<K, V> *link_tail; ///< Linked list of items allocated in the map
    Dqn_DSMapHashProc   *hash_function;
};

/// Initialise a hash table with a given size. The given size must be a power of
/// two. Initialisation is optional and the map can be zero-initialised in which
/// the hash table will default to the size specified in DQN_DSMAP_MIN_SIZE.
/// @param[in] size The number of slots in the hash-table. This value must be
/// a power of two.
DQN_DS_MAP_API
DQN_DS_MAP Dqn_DSMap_Init(Dqn_isize size);

DQN_DS_MAP_API
void Dqn_DSMap_Free(DQN_DS_MAP *map, Dqn_ZeroMem zero_mem);

DQN_DS_MAP_API
Dqn_DSMapItem<K, V> *Dqn_DSMap_Find(DQN_DS_MAP *map, K const &key);

enum Dqn_DSMapFindOnly    { Dqn_DSMapFindOnly_No,    Dqn_DSMapFindOnly_Yes };
enum Dqn_DSMapItemCreated { Dqn_DSMapItemCreated_No, Dqn_DSMapItemCreated_Yes };

DQN_DS_MAP_API
Dqn_DSMapItem<K, V> *Dqn_DSMap_FindOrAdd(DQN_DS_MAP *map, K const &key, Dqn_DSMapFindOnly find_only, Dqn_DSMapItemCreated *created);

DQN_DS_MAP_API
Dqn_DSMapItem<K, V> *Dqn_DSMap_Add(DQN_DS_MAP *map, K const &key, V const &value);

DQN_DS_MAP_API
Dqn_DSMapItem<K, V> *Dqn_DSMap_Make(DQN_DS_MAP *map, K const &key);

DQN_DS_MAP_API
Dqn_DSMapItem<K, V> *Dqn_DSMap_Get(DQN_DS_MAP *map, K const &key);

DQN_DS_MAP_API
void Dqn_DSMap_Erase(DQN_DS_MAP *map, K const &key, Dqn_ZeroMem zero_mem);
#endif // !defined(DQN_NO_DSMAP)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-DLIB] Dqn_Library          |                             | Library run-time behaviour configuration
// ---------------------------------+-----------------------------+---------------------------------
struct Dqn_Library
{
    Dqn_LogProc       *log_callback;   ///< Set this pointer to override the logging routine
    void *             log_user_data;
    bool               log_to_file;    ///< Output logs to file as well as standard out
    void *             log_file;       ///< TODO(dqn): Hmmm, how should we do this... ?
    Dqn_TicketMutex    log_file_mutex; ///< Is locked when instantiating the log_file for the first time

#if defined(DQN_LEAK_TRACKING)
    Dqn_TicketMutex          alloc_table_mutex;
    Dqn_DSMap<Dqn_LeakTrace> alloc_table;
#endif

#if defined(DQN_OS_WIN32)
    LARGE_INTEGER     win32_qpc_frequency;
    Dqn_TicketMutex   win32_bcrypt_rng_mutex;
    void             *win32_bcrypt_rng_handle;
#endif

#if defined(DQN_DEBUG_THREAD_CONTEXT)
    Dqn_TicketMutex thread_context_mutex;
    Dqn_ArenaStat   thread_context_arena_stats[256];
    uint8_t         thread_context_arena_stats_count;
#endif
} extern dqn_library;

/// Update the default logging function, all logging functions will run through
/// this callback
/// @param[in] proc The new logging function, set to nullptr to revert back to
/// the default logger.
/// @param[in] user_data A user defined parameter to pass to the callback
DQN_API void Dqn_Library_SetLogCallback(Dqn_LogProc *proc, void *user_data);

/// @param[in] file Pass in nullptr to turn off writing logs to disk, otherwise
/// point it to the FILE that you wish to write to.
DQN_API void Dqn_Library_SetLogFile(void *file);

/// Dump the per-thread arena statistics to the specified file
DQN_API void Dqn_Library_DumpThreadContextArenaStat(Dqn_String8 file_path);

#if defined(DQN_LEAK_TRACKING)
void Dqn_Library_LeakTraceAdd(Dqn_CallSite call_site, void *ptr, Dqn_usize size);
void Dqn_Library_LeakTraceMarkFree(Dqn_CallSite call_site, void *ptr);
#else
#define Dqn_Library_LeakTraceAdd(...)
#define Dqn_Library_LeakTraceMarkFree(...)
#endif

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-FSTR] Dqn_FString8         | DQN_NO_FSTRING8             | Fixed-size strings
// ---------------------------------+-----------------------------+---------------------------------
#if !defined(DQN_NO_FSTRING8)
#define DQN_FSTRING8_API template <Dqn_isize N> DQN_API
#define DQN_FSTRING8 Dqn_FString8<N>
DQN_FSTRING8_API struct Dqn_FString8
{
    char      data[N];
    Dqn_isize size;

    bool operator==(Dqn_FString8 const &other) const {
        if (size != other.size) return false;
        bool result = DQN_MEMCMP(data, other.data, size);
        return result;
    }

    bool operator!=(Dqn_FString8 const &other) const { return !(*this == other); }

    char       *begin()       { return data; }
    char       *end  ()       { return data + size; }
    char const *begin() const { return data; }
    char const *end  () const { return data + size; }
};

/// Create a fixed string from the format string. The result string is
/// null-terminated.
/// @param fmt[in] Format string specifier to create the fixed string from
/// @return The created string, truncated if there was insufficient space
DQN_FSTRING8_API DQN_FSTRING8 Dqn_FString8_InitF(char const *fmt, ...);

/// @param string[in] The string to query the maximum capacity of
/// @return Maximum capacity of the fixed string
DQN_FSTRING8_API Dqn_isize Dqn_FString8_Max(DQN_FSTRING8 *string);

/// Reset the characters in the string
/// @param string[in] The string to clear
DQN_FSTRING8_API void Dqn_FString8_Clear(DQN_FSTRING8 *string);

/// Append a format string to the fixed string. On failure the string is
/// appended to but truncated ensuring null-termination.
/// @param string[in] The string to append to
/// @param fmt[in] Format string to append to the fixed string
/// @return True if append was successful, false otherwise.
DQN_FSTRING8_API bool Dqn_FString8_AppendFV(DQN_FSTRING8 *string, char const *fmt, va_list va);

/// @copydocs Dqn_FString8_AppendF
DQN_FSTRING8_API bool Dqn_FString8_AppendF(DQN_FSTRING8 *string, char const *fmt, ...);

/// Append a cstring to the fixed string. On failure the string is
/// appended to but truncated ensuring null-termination.
/// @param string[in] The string to append to
/// @param value[in] Cstring to append to the fixed string
/// @param size[in] Size of the cstring, when <0, the cstring's length is
/// determined before appending.
/// @return True if append was successful, false otherwise.
DQN_FSTRING8_API bool Dqn_FString8_AppendCString8(DQN_FSTRING8 *string, char const *value, Dqn_isize size = -1);

/// Append a string to the fixed string. On failure the string is
/// appended to but truncated ensuring null-termination.
/// @param string[in] The string to append to
/// @param value[in] String to append to the fixed string
/// determined before appending.
/// @return True if append was successful, false otherwise.
DQN_FSTRING8_API bool Dqn_FString8_AppendString8(DQN_FSTRING8 *string, Dqn_String8 value);

/// Convert a fixed string to a string. The string holds a reference to the
/// fixed string and is invalidated once fixed string is deleted.
/// @param string[in] The fixed string to create a string from
/// @return String referencing the contents of `string`
DQN_FSTRING8_API Dqn_String8  Dqn_FString8_ToString8(DQN_FSTRING8 const *string);
#endif // !defined(DQN_NO_FSTRING8)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-STRB] Dqn_String8Builder   |                             |
// ---------------------------------+-----------------------------+---------------------------------
struct Dqn_String8Builder
{
    Dqn_Allocator    allocator;   ///< Allocator to use to back the string list
    Dqn_String8Link *head;        ///< First string in the linked list of strings
    Dqn_String8Link *tail;        ///< Last string in the linked list of strings
    ptrdiff_t        string_size; ///< The size in bytes necessary to construct the current string
    ptrdiff_t        count;       ///< The number of links in the linked list of strings
};

/// Append a string to the list of strings in the builder by reference.
/// The string's data must persist whilst the string builder is being used.
/// @param builder The builder to append the string to
/// @param string The string to append to the builder
/// @return True if append was successful, false if parameters are invalid
/// or memory allocation failure.
bool Dqn_String8Builder_AppendString8Ref(Dqn_String8Builder *builder, Dqn_String8 string);

/// Append a string to the list of strings in the builder by copy.
/// The string is copied using the builder's allocator before appending.
/// @param builder The builder to append the string to
/// @param string The string to append to the builder
/// @return True if append was successful, false if parameters are invalid
/// or memory allocation failure.
bool Dqn_String8Builder_AppendString8Copy(Dqn_String8Builder *builder, Dqn_String8 string);

/// @copydoc Dqn_String8Builder_AppendF
/// @param args The variable argument list to use in the format string
#define Dqn_String8Builder_AppendFV(builder, fmt, args) Dqn_String8Builder_AppendFV_(DQN_LEAK_TRACE builder, fmt, args)
bool Dqn_String8Builder_AppendFV_(DQN_LEAK_TRACE_FUNCTION Dqn_String8Builder *builder, char const *fmt, va_list args);

/// Append a printf-style format string to the list of strings in the builder.
/// @param builder The builder to append the string to
/// @param fmt The format string to use
/// @return True if append was successful, false if parameters are invalid
/// or memory allocation failure.
bool Dqn_String8Builder_AppendF(Dqn_String8Builder *builder, char const *fmt, ...);

/// Build the list of strings into the final composite string from the string
/// builder
/// @param builder The string builder to build the string from
/// @param allocator The allocator to use to build the string
/// @return The string if build was successful, empty string if parameters are
/// invalid or memory allocation failure.
Dqn_String8 Dqn_String8Builder_Build(Dqn_String8Builder const *builder, Dqn_Allocator allocator);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-FARR] Dqn_FArray           | DQN_NO_FARRAY               | Fixed-size arrays
// ---------------------------------+-----------------------------+---------------------------------
#if !defined(DQN_NO_FARRAY)
#define DQN_FARRAY_API template <typename T, Dqn_isize N> DQN_API
#define DQN_FARRAY Dqn_FArray<T, N>
DQN_FARRAY_API struct Dqn_FArray
{
    T         data[N]; ///< Storage of the fixed array
    Dqn_isize size;    ///< Current number of items in the array
    T       *begin()       { return data; }
    T       *end  ()       { return data + size; }
    T const *begin() const { return data; }
    T const *end  () const { return data + size; }
};

/// Initialise a fixed size array with the given items
/// @param items[in] Items to initialise the array with
/// @param count[in] Number of items to initialise the array with
/// @return Fixed size array with the items added to it
DQN_FARRAY_API DQN_FARRAY Dqn_FArray_Init(T const *items, Dqn_isize count);

/// @param array[in] The array to query the maximum capacity of
/// @return Maximum capacity of the fixed array
DQN_FARRAY_API Dqn_isize  Dqn_FArray_Max(DQN_FARRAY const *array);

/// Add the array of items into the array by copy
/// @param array[in] Array to copy items to
/// @param items[in] Array of items to copy
/// @param count[in] Number of items to copy
/// @return Pointer to the first item added, null pointer if arguments are
/// invalid or there is insufficient capacity.
DQN_FARRAY_API T *Dqn_FArray_AddArray(DQN_FARRAY *array, T const *items, Dqn_isize count);

/// Add the item into the array by copy
DQN_FARRAY_API T *Dqn_FArray_Add(DQN_FARRAY *a, T const &item);

/// Allocate the next `count` number of items from the array
/// @param array[in] The array to allocate from
/// @param count[in] Number of items to allocate
/// @return Pointer to the `count` items requested, null pointer if there's
/// insufficient space.
DQN_FARRAY_API T *Dqn_FArray_Make(DQN_FARRAY *array, Dqn_isize count);

/// Reset the number of items in the array
/// @param array[in] The array to clear
/// @param zero_mem[in] Flag to request the memory is zeroed-out or not
DQN_FARRAY_API void Dqn_FArray_Clear(DQN_FARRAY *array, Dqn_ZeroMem zero_mem);

/// Erase the item at index in the array, preserving the order of items in the
/// array
/// @param array[in] The array to erase from
/// @param index[in] Item index to erase from the array
/// @return True if erase was successful, false if parameters were invalid
DQN_FARRAY_API bool Dqn_FArray_EraseStable(DQN_FARRAY *array, Dqn_isize index);

/// Erase the item at index in the array
/// @param array[in] The array to erase from
/// @param index[in] Item index to erase from the array
/// @return True if erase was successful, false if parameters were invalid
DQN_FARRAY_API bool Dqn_FArray_EraseUnstable(DQN_FARRAY *array, Dqn_isize index);

/// Remove the last `count` elements from the array
/// @param array[in] The array to pop from
/// @param count[in] Number of elements to remove from the array
/// @return True if pop was successful, false if paramters were invalid
DQN_FARRAY_API bool Dqn_FArray_Pop(DQN_FARRAY *array, Dqn_isize count);

/// Return the last most element from the array
/// @param array[in] The array to peek
/// @return Last most element if successful, null pointer otherwise
DQN_FARRAY_API T *Dqn_FArray_Peek(DQN_FARRAY *array);

/// Return a copy of the last most element from the array. Asserts if the array
/// is empty.
/// @param array[in] The array to peek
/// @return Last most element
DQN_FARRAY_API T Dqn_FArray_PeekCopy(DQN_FARRAY const *array);

/// Find the specified item in the given array using the equality procedure
/// passed in.
/// @param array[in] Array to search
/// @param IsEqualProc[in] Lambda to a procedure that defines the equality of
/// the item to find in the array.
/// @param made[out] (Optional) Set to true if the item didn't exist and was
/// created successfully
/// @return Pointer to the item if found, null pointer otherwise
template <typename T, Dqn_isize N, typename IsEqualProc>
T *Dqn_FArray_Find(DQN_FARRAY *a, IsEqualProc IsEqual);

/// Find the specified item in the given array using the equality procedure
/// passed in.
/// @param array[in] The array to search
/// @param IsEqualProc[in] Lambda to a procedure that defines the equality of
/// the item to find in the array.
/// @param made[out] (Optional) Set to true if the item didn't exist and was
/// created successfully
/// @return Pointer to the item if found, null pointer otherwise
template <typename T, Dqn_isize N, typename IsEqualProc>
DQN_API T *Dqn_FArray_FindProcElseMake(DQN_FARRAY *array, IsEqualProc IsEqual, bool *made);

/// Find the specified item in the given array. The items must have an
/// operator== defined.
/// @param array[in] The array to search
/// @param find[in] Item to find in the array
/// @return Pointer to the item if found, null pointer otherwise
DQN_FARRAY_API T *Dqn_FArray_Find(DQN_FARRAY *array, T const *find);
#endif // !defined(DQN_NO_FARRAY)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-LIST] Dqn_List             |                             | Chunked linked lists, append only
// ---------------------------------+-----------------------------+---------------------------------
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
    Dqn_b32           init;             // (Internal): True if Dqn_List_Iterate has been called at-least once on this iterator
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

template <typename T> DQN_API
Dqn_List<T> Dqn_List_InitWithArena(Dqn_Arena *arena, Dqn_isize chunk_size = 128);

#define Dqn_List_Make(list, count) Dqn_List_Make_(DQN_LEAK_TRACE list, count)
template <typename T> DQN_API
T *Dqn_List_Make_(DQN_LEAK_TRACE_FUNCTION Dqn_List<T> *list, Dqn_isize count);

#define Dqn_List_Add(list, count) Dqn_List_Add_(DQN_LEAK_TRACE list, count)
template <typename T> DQN_API
T *Dqn_List_Add_(DQN_LEAK_TRACE_FUNCTION Dqn_List<T> *list, Dqn_isize count);

/// @param at_chunk[out] (Optional) The chunk that the index belongs to will 
/// be set in this parameter if given
/// @return The element, or null pointer if it is not a valid index.
template <typename T> DQN_API
T *Dqn_List_At(Dqn_List<T> *list, Dqn_isize index, Dqn_ListChunk<T> *at_chunk);

/// Produce an iterator for the data in the list
///
/// @param[in] start_index The index to start iterating from
///
/// @begincode
/// Dqn_List<int> list = {};
/// for (Dqn_ListIterator<int> it = {}; Dqn_List_Iterate(&list, &it, 0);)
/// {
///     int *item = it.data;
/// }
/// @endcode
///
template <typename T> DQN_API
bool Dqn_List_Iterate(Dqn_List<T> *list, Dqn_ListIterator<T> *it, Dqn_isize start_index);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-MATH] Math                 | DQN_NO_MATH                 | v2i, V2, V3, V4, Mat4, Rect, RectI32, Lerp
// ---------------------------------+-----------------------------+---------------------------------
#if !defined(DQN_NO_MATH)
struct Dqn_V2I
{
    int32_t x, y;

    Dqn_V2I() = default;
    Dqn_V2I(Dqn_f32 x_, Dqn_f32 y_): x((int32_t)x_), y((int32_t)y_) {}
    Dqn_V2I(int32_t x_, int32_t y_): x(x_), y(y_) {}
    Dqn_V2I(int32_t xy):             x(xy), y(xy) {}

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
    Dqn_V2I  operator* (int32_t other) const { Dqn_V2I result(x * other,   y * other);   return result; }
    Dqn_V2I  operator/ (Dqn_V2I other) const { Dqn_V2I result(x / other.x, y / other.y); return result; }
    Dqn_V2I  operator/ (Dqn_f32 other) const { Dqn_V2I result(x / other,   y / other);   return result; }
    Dqn_V2I  operator/ (int32_t other) const { Dqn_V2I result(x / other,   y / other);   return result; }
    Dqn_V2I &operator*=(Dqn_V2I other)       { *this = *this * other;                    return *this;  }
    Dqn_V2I &operator*=(Dqn_f32 other)       { *this = *this * other;                    return *this;  }
    Dqn_V2I &operator*=(int32_t other)       { *this = *this * other;                    return *this;  }
    Dqn_V2I &operator-=(Dqn_V2I other)       { *this = *this - other;                    return *this;  }
    Dqn_V2I &operator+=(Dqn_V2I other)       { *this = *this + other;                    return *this;  }
};

struct Dqn_V2
{
    Dqn_f32 x, y;

    Dqn_V2() = default;
    Dqn_V2(Dqn_f32 a)           : x(a),           y(a)           {}
    Dqn_V2(int32_t a)           : x((Dqn_f32)a),  y((Dqn_f32)a)  {}
    Dqn_V2(Dqn_f32 x, Dqn_f32 y): x(x),           y(y)           {}
    Dqn_V2(int32_t x, int32_t y): x((Dqn_f32)x),  y((Dqn_f32)y)  {}
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
    Dqn_V2  operator* (int32_t other) const { Dqn_V2 result(x * other,   y * other);   return result; }
    Dqn_V2  operator/ (Dqn_V2  other) const { Dqn_V2 result(x / other.x, y / other.y); return result; }
    Dqn_V2  operator/ (Dqn_f32 other) const { Dqn_V2 result(x / other,   y / other);   return result; }
    Dqn_V2  operator/ (int32_t other) const { Dqn_V2 result(x / other,   y / other);   return result; }
    Dqn_V2 &operator*=(Dqn_V2  other)       { *this = *this * other;               return *this;  }
    Dqn_V2 &operator*=(Dqn_f32 other)       { *this = *this * other;               return *this;  }
    Dqn_V2 &operator*=(int32_t other)       { *this = *this * other;               return *this;  }
    Dqn_V2 &operator/=(Dqn_V2  other)       { *this = *this / other;               return *this;  }
    Dqn_V2 &operator/=(Dqn_f32 other)       { *this = *this / other;               return *this;  }
    Dqn_V2 &operator/=(int32_t other)       { *this = *this / other;               return *this;  }
    Dqn_V2 &operator-=(Dqn_V2  other)       { *this = *this - other;               return *this;  }
    Dqn_V2 &operator+=(Dqn_V2  other)       { *this = *this + other;               return *this;  }
};

struct Dqn_V3
{
    Dqn_f32 x, y, z;

    Dqn_V3() = default;
    Dqn_V3(Dqn_f32 a)                      : x(a),                  y(a),                  z(a)                  {}
    Dqn_V3(int32_t a)                      : x(DQN_CAST(Dqn_f32)a), y(DQN_CAST(Dqn_f32)a), z(DQN_CAST(Dqn_f32)a) {}
    Dqn_V3(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z): x(x),                  y(y),                  z(z)                  {}
    Dqn_V3(int32_t x, int32_t y, Dqn_f32 z): x(DQN_CAST(Dqn_f32)x), y(DQN_CAST(Dqn_f32)y), z(DQN_CAST(Dqn_f32)z) {}
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
    Dqn_V3 operator*  (int32_t other) const { Dqn_V3 result(x * other,   y * other,   z * other);   return result; }
    Dqn_V3 operator/  (Dqn_V3  other) const { Dqn_V3 result(x / other.x, y / other.y, z / other.z); return result; }
    Dqn_V3 operator/  (Dqn_f32 other) const { Dqn_V3 result(x / other,   y / other,   z / other);   return result; }
    Dqn_V3 operator/  (int32_t other) const { Dqn_V3 result(x / other,   y / other,   z / other);   return result; }
    Dqn_V3 &operator*=(Dqn_V3  other)       { *this = *this * other; return *this;  }
    Dqn_V3 &operator*=(Dqn_f32 other)       { *this = *this * other; return *this;  }
    Dqn_V3 &operator*=(int32_t other)       { *this = *this * other; return *this;  }
    Dqn_V3 &operator/=(Dqn_V3  other)       { *this = *this / other; return *this;  }
    Dqn_V3 &operator/=(Dqn_f32 other)       { *this = *this / other; return *this;  }
    Dqn_V3 &operator/=(int32_t other)       { *this = *this / other; return *this;  }
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
  Dqn_V4(int32_t x, int32_t y, int32_t z, int32_t w): x(DQN_CAST(Dqn_f32)x), y(DQN_CAST(Dqn_f32)y), z(DQN_CAST(Dqn_f32)z), w(DQN_CAST(Dqn_f32)w) {}
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
  Dqn_V4  operator* (int32_t other) const { Dqn_V4 result(x * other,   y * other,   z * other,   w * other);   return result;  }
  Dqn_V4  operator/ (Dqn_f32 other) const { Dqn_V4 result(x / other,   y / other,   z / other,   w / other);   return result;  }
  Dqn_V4 &operator*=(Dqn_V4  other)       { *this = *this * other;                                             return *this;   }
  Dqn_V4 &operator*=(Dqn_f32 other)       { *this = *this * other;                                             return *this;   }
  Dqn_V4 &operator*=(int32_t other)       { *this = *this * other;                                             return *this;   }
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

DQN_API Dqn_M4  Dqn_M4_Identity();
DQN_API Dqn_M4  Dqn_M4_ScaleF(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z);
DQN_API Dqn_M4  Dqn_M4_Scale(Dqn_V3 xyz);
DQN_API Dqn_M4  Dqn_M4_TranslateF(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z);
DQN_API Dqn_M4  Dqn_M4_Translate(Dqn_V3 xyz);
DQN_API Dqn_M4  Dqn_M4_Transpose(Dqn_M4 mat);
DQN_API Dqn_M4  Dqn_M4_Rotate(Dqn_V3 axis, Dqn_f32 radians);
DQN_API Dqn_M4  Dqn_M4_Orthographic(Dqn_f32 left, Dqn_f32 right, Dqn_f32 bottom, Dqn_f32 top, Dqn_f32 z_near, Dqn_f32 z_far);
DQN_API Dqn_M4  Dqn_M4_Perspective(Dqn_f32 fov /*radians*/, Dqn_f32 aspect, Dqn_f32 z_near, Dqn_f32 z_far);
DQN_API Dqn_M4  Dqn_M4_Add(Dqn_M4 lhs, Dqn_M4 rhs);
DQN_API Dqn_M4  Dqn_M4_Sub(Dqn_M4 lhs, Dqn_M4 rhs);
DQN_API Dqn_M4  Dqn_M4_Mul(Dqn_M4 lhs, Dqn_M4 rhs);
DQN_API Dqn_M4  Dqn_M4_Div(Dqn_M4 lhs, Dqn_M4 rhs);
DQN_API Dqn_M4  Dqn_M4_AddF(Dqn_M4 lhs, Dqn_f32 rhs);
DQN_API Dqn_M4  Dqn_M4_SubF(Dqn_M4 lhs, Dqn_f32 rhs);
DQN_API Dqn_M4  Dqn_M4_MulF(Dqn_M4 lhs, Dqn_f32 rhs);
DQN_API Dqn_M4  Dqn_M4_DivF(Dqn_M4 lhs, Dqn_f32 rhs);

#if !defined(DQN_NO_FSTRING8)
DQN_API Dqn_FString8<256> Dqn_M4_ColumnMajorString(Dqn_M4 mat);
#endif

struct Dqn_Rect
{
    Dqn_V2 min, max;
    Dqn_Rect() = default;
    Dqn_Rect(Dqn_V2  min, Dqn_V2 max)  : min(min), max(max) {}
    Dqn_Rect(Dqn_V2I min, Dqn_V2I max) : min(min), max(max) {}
    Dqn_Rect(Dqn_f32 x, Dqn_f32 y, Dqn_f32 max_x, Dqn_f32 max_y) : min(x, y), max(max_x, max_y) {}
    bool operator==(Dqn_Rect other) const { return (min == other.min) && (max == other.max); }
};

struct Dqn_RectI32
{
    Dqn_V2I min, max;
    Dqn_RectI32() = default;
    Dqn_RectI32(Dqn_V2I min, Dqn_V2I max) : min(min), max(max) {}
};

DQN_API Dqn_Rect Dqn_Rect_InitFromPosAndSize(Dqn_V2 pos, Dqn_V2 size);
DQN_API Dqn_V2   Dqn_Rect_Center(Dqn_Rect rect);
DQN_API bool     Dqn_Rect_ContainsPoint(Dqn_Rect rect, Dqn_V2 p);
DQN_API bool     Dqn_Rect_ContainsRect(Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_V2   Dqn_Rect_Size(Dqn_Rect rect);
DQN_API Dqn_Rect Dqn_Rect_Move(Dqn_Rect src, Dqn_V2 move_amount);
DQN_API Dqn_Rect Dqn_Rect_MoveTo(Dqn_Rect src, Dqn_V2 dest);
DQN_API bool     Dqn_Rect_Intersects(Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_Rect Dqn_Rect_Intersection(Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_Rect Dqn_Rect_Union(Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_Rect Dqn_Rect_FromRectI32(Dqn_RectI32 a);
DQN_API Dqn_V2I  Dqn_RectI32_Size(Dqn_RectI32 rect);

DQN_API Dqn_V2  Dqn_Lerp_V2(Dqn_V2 a, Dqn_f32 t, Dqn_V2 b);
DQN_API Dqn_f32 Dqn_Lerp_F32(Dqn_f32 a, Dqn_f32 t, Dqn_f32 b);
#endif // !defined(DQN_NO_MATH)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-BITS] Dqn_Bit              |                             | Bitset manipulation
// ---------------------------------+-----------------------------+---------------------------------
DQN_API void    Dqn_Bit_UnsetInplace(uint32_t *flags, uint32_t bitfield);
DQN_API void    Dqn_Bit_SetInplace(uint32_t *flags, uint32_t bitfield);
DQN_API bool    Dqn_Bit_IsSet(uint32_t bits, uint32_t bits_to_set);
DQN_API bool    Dqn_Bit_IsNotSet(uint32_t bits, uint32_t bits_to_check);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-SAFE] Dqn_Safe             |                             | Safe arithmetic, casts, asserts
// ---------------------------------+-----------------------------+---------------------------------
#if defined(NDEBUG)
    #define Dqn_Safe_AssertF(expr, fmt, ...)
    #define Dqn_Safe_Assert(expr, fmt, ...)
#else
    #define Dqn_Safe_AssertF(expr, fmt, ...) \
        Dqn_Safe_AssertF_(expr, DQN_CALL_SITE, "Safe assert triggered " #expr ": " fmt, ## __VA_ARGS__)
    #define Dqn_Safe_Assert(expr) Dqn_Safe_AssertF(expr, "")
#endif

/// Assert the expression given in debug, whilst in release- assertion is
/// removed and the expression is evaluated and returned.
///
/// This function provides dual logic which allows handling of the condition
/// gracefully in release mode, but asserting in debug mode. This is an internal
/// function, prefer the @see Dqn_Safe_Assert macros.
///
/// @param assertion_expr[in] Expressin to assert on
/// @param fmt[in] Format string for providing a message on assertion
/// @return True if the expression evaluated to true, false otherwise.
DQN_API bool Dqn_Safe_AssertF_(bool assertion_expr, Dqn_CallSite call_site, char const *fmt, ...);

// NOTE: Dqn_Safe Arithmetic
// -----------------------------------------------------------------------------
/// Add 2 I64's together, safe asserting that the operation does not overflow.
/// @return The result of operation, INT64_MAX if it overflowed.
DQN_API int64_t  Dqn_Safe_AddI64(int64_t a,  int64_t b);

/// Multiply 2 I64's together, safe asserting that the operation does not
/// overflow.
/// @return The result of operation, IINT64_MAX if it overflowed.
DQN_API int64_t Dqn_Safe_MulI64(int64_t a,  int64_t b);

/// Add 2 U64's together, safe asserting that the operation does not overflow.
/// @return The result of operation, UINT64_MAX if it overflowed.
DQN_API uint64_t Dqn_Safe_AddU64(uint64_t a, uint64_t b);

/// Subtract 2 U64's together, safe asserting that the operation does not
/// overflow.
/// @return The result of operation, 0 if it overflowed.
DQN_API uint64_t Dqn_Safe_SubU64(uint64_t a, uint64_t b);

/// Multiple 2 U64's together, safe asserting that the operation does not
/// overflow.
/// @return The result of operation, UINT64_MAX if it overflowed.
DQN_API uint64_t Dqn_Safe_MulU64(uint64_t a, uint64_t b);

/// Subtract 2 U32's together, safe asserting that the operation does not
/// overflow.
/// @return The result of operation, 0 if it overflowed.
DQN_API uint32_t Dqn_Safe_SubU32(uint32_t a, uint32_t b);

// NOTE: Dqn_Safe_SaturateCastUSizeToI*
// -----------------------------------------------------------------------------
/// Truncate a usize to int clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT_MAX if the value would go out of the
/// valid range when casted.
DQN_API int Dqn_Safe_SaturateCastUSizeToInt(Dqn_usize val);

/// Truncate a usize to I8 clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT8_MAX if the value would go out of the
/// valid range when casted.
DQN_API int8_t Dqn_Safe_SaturateCastUSizeToI8(Dqn_usize val);

/// Truncate a usize to I16 clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT16_MAX if the value would go out of the
/// valid range when casted.
DQN_API int16_t Dqn_Safe_SaturateCastUSizeToI16(Dqn_usize val);

/// Truncate a usize to I32 clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT32_MAX if the value would go out of the
/// valid range when casted.
DQN_API int32_t Dqn_Safe_SaturateCastUSizeToI32(Dqn_usize val);

/// Truncate a usize to I64 clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT64_MAX if the value would go out of the
/// valid range when casted.
DQN_API int64_t Dqn_Safe_SaturateCastUSizeToI64(Dqn_usize val);

// NOTE: Dqn_Safe_SaturateCastU64ToU*
// -----------------------------------------------------------------------------
/// Truncate a u64 to uint clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or UINT_MAX if the value would go out of the
/// valid range when casted.
DQN_API unsigned int Dqn_Safe_SaturateCastU64ToUInt(uint64_t val);

/// Truncate a u64 to u8 clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or UINT8_MAX if the value would go out of the
/// valid range when casted.
DQN_API uint8_t Dqn_Safe_SaturateCastU64ToU8(uint64_t val);

/// Truncate a u64 to u16 clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or UINT16_MAX if the value would go out of the
/// valid range when casted.
DQN_API uint16_t Dqn_Safe_SaturateCastU64ToU16(uint64_t val);

/// Truncate a u64 to u32 clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or UINT32_MAX if the value would go out of the
/// valid range when casted.
DQN_API uint32_t Dqn_Safe_SaturateCastU64ToU32(uint64_t val);

// NOTE: Dqn_Safe_SaturateCastUSizeToU*
// -----------------------------------------------------------------------------
/// Truncate a usize to U8 clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or UINT8_MAX if the value would go out of the
/// valid range when casted.
DQN_API uint8_t Dqn_Safe_SaturateCastUSizeToU8(Dqn_usize val);

/// Truncate a usize to U16 clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or UINT16_MAX if the value would go out of the
/// valid range when casted.
DQN_API uint16_t Dqn_Safe_SaturateCastUSizeToU16(Dqn_usize val);

/// Truncate a usize to U32 clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or UINT32_MAX if the value would go out of the
/// valid range when casted.
DQN_API uint32_t Dqn_Safe_SaturateCastUSizeToU32(Dqn_usize val);

/// Truncate a usize to U64 clamping the result to the max value of the desired
/// data type. Safe asserts if clamping occurs.
/// @return The truncated value or UINT64_MAX if the value would go out of the
/// valid range when casted.
DQN_API uint64_t Dqn_Safe_SaturateCastUSizeToU64(Dqn_usize val);

// NOTE: Dqn_Safe_SaturateCastISizeToI*
// -----------------------------------------------------------------------------
/// Truncate an isize to int clamping the result to the min and max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT_MIN or INT_MAX if the value would go
/// out of the valid range when casted.
DQN_API int Dqn_Safe_SaturateCastISizeToInt(Dqn_isize val);

/// Truncate an isize to I8 clamping the result to the min and max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT8_MIN or INT8_MAX if the value would go
/// out of the valid range when casted.
DQN_API int8_t Dqn_Safe_SaturateCastISizeToI8(Dqn_isize val);

/// Truncate an isize to I16 clamping the result to the min and max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT16_MIN or INT16_MAX if the value would go
/// out of the valid range when casted.
DQN_API int16_t Dqn_Safe_SaturateCastISizeToI16(Dqn_isize val);

/// Truncate an isize to I32 clamping the result to the min and max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT32_MIN or INT32_MAX if the value would go
/// out of the valid range when casted.
DQN_API int32_t Dqn_Safe_SaturateCastISizeToI32(Dqn_isize val);

/// Truncate an isize to I64 clamping the result to the min and max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT64_MIN or INT64_MAX if the value would go
/// out of the valid range when casted.
DQN_API int64_t Dqn_Safe_SaturateCastISizeToI64(Dqn_isize val);

// NOTE: Dqn_Safe_SaturateCastISizeToU*
// -----------------------------------------------------------------------------
/// Truncate an isize to uint clamping the result to 0 and the max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or 0 or UINT_MAX if the value would go
/// out of the valid range when casted.
DQN_API unsigned int Dqn_Safe_SaturateCastISizeToUInt(Dqn_isize val);

/// Truncate an isize to U8 clamping the result to 0 and the max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or 0 or UINT8_MAX if the value would go
/// out of the valid range when casted.
DQN_API uint8_t Dqn_Safe_SaturateCastISizeToU8(Dqn_isize val);

/// Truncate an isize to U16 clamping the result to 0 and the max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or 0 or UINT16_MAX if the value would go
/// out of the valid range when casted.
DQN_API uint16_t Dqn_Safe_SaturateCastISizeToU16(Dqn_isize val);

/// Truncate an isize to U32 clamping the result to 0 and the max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or 0 or UINT32_MAX if the value would go
/// out of the valid range when casted.
DQN_API uint32_t Dqn_Safe_SaturateCastISizeToU32(Dqn_isize val);

/// Truncate an isize to U64 clamping the result to 0 and the max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or 0 or UINT64_MAX if the value would go
/// out of the valid range when casted.
DQN_API uint64_t Dqn_Safe_SaturateCastISizeToU64(Dqn_isize val);

// NOTE: Dqn_Safe_SaturateCastI64To*
// -----------------------------------------------------------------------------
/// Truncate an I64 to isize clamping the result to the min and max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or DQN_ISIZE_MIN or DQN_ISIZE_MAX if the value
/// would go out of the valid range when casted.
DQN_API Dqn_isize Dqn_Safe_SaturateCastI64ToISize(Dqn_isize val);

/// Truncate an I64 to I8 clamping the result to the min and max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT8_MIN or INT8_MAX if the value would go
/// out of the valid range when casted.
DQN_API int8_t Dqn_Safe_SaturateCastI64ToI8(int64_t val);

/// Truncate an I64 to I16 clamping the result to the min and max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT16_MIN or INT16_MAX if the value would go
/// out of the valid range when casted.
DQN_API int16_t Dqn_Safe_SaturateCastI64ToI16(int64_t val);

/// Truncate an I64 to I32 clamping the result to the min and max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT32_MIN or INT32_MAX if the value would go
/// out of the valid range when casted.
DQN_API int32_t Dqn_Safe_SaturateCastI64ToI32(int64_t val);

// NOTE: Dqn_Safe_SaturateCastIntTo*
// -----------------------------------------------------------------------------
/// Truncate an int to I8 clamping the result to the min and max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT8_MIN or INT8_MAX if the value
/// would go out of the valid range when casted.
DQN_API int8_t Dqn_Safe_SaturateCastIntToI8(int val);

/// Truncate an int to I16 clamping the result to the min and max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or INT16_MIN or INT16_MAX if the value
/// would go out of the valid range when casted.
DQN_API int16_t Dqn_Safe_SaturateCastIntToI16(int val);

/// Truncate an int to U8 clamping the result to 0 and the max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or 0 or UINT8_MAX if the value would go
/// out of the valid range when casted.
DQN_API uint8_t Dqn_Safe_SaturateCastIntToU8(int val);

/// Truncate an int to U16 clamping the result to 0 and the max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or 0 or UINT16_MAX if the value would go
/// out of the valid range when casted.
DQN_API uint16_t Dqn_Safe_SaturateCastIntToU16(int val);

/// Truncate an int to U32 clamping the result to 0 and the max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or 0 or UINT32_MAX if the value would go
/// out of the valid range when casted.
DQN_API uint32_t Dqn_Safe_SaturateCastIntToU32(int val);

/// Truncate an int to U64 clamping the result to 0 and the max value of the
/// desired data type. Safe asserts if clamping occurs.
/// @return The truncated value or 0 or UINT64_MAX if the value would go
/// out of the valid range when casted.
DQN_API uint64_t Dqn_Safe_SaturateCastIntToU64(int val);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-CHAR] Dqn_Char             |                             | Character ascii/digit.. helpers
// ---------------------------------+-----------------------------+---------------------------------
DQN_API bool    Dqn_Char_IsAlpha       (char ch);
DQN_API bool    Dqn_Char_IsDigit       (char ch);
DQN_API bool    Dqn_Char_IsAlphaNum    (char ch);
DQN_API bool    Dqn_Char_IsWhitespace  (char ch);
DQN_API bool    Dqn_Char_IsHex         (char ch);
DQN_API uint8_t Dqn_Char_HexToU8       (char ch);
DQN_API char    Dqn_Char_ToHex         (char ch);
DQN_API char    Dqn_Char_ToHexUnchecked(char ch);
DQN_API char    Dqn_Char_ToLower       (char ch);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-UTFX] Dqn_UTF              |                             | Unicode helpers
// ---------------------------------+-----------------------------+---------------------------------
DQN_API int Dqn_UTF8_EncodeCodepoint(uint8_t utf8[4], uint32_t codepoint);
DQN_API int Dqn_UTF16_EncodeCodepoint(uint16_t utf16[2], uint32_t codepoint);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-BHEX] Dqn_Hex              | DQN_NO_HEX                  | Binary <-> hex helpers
// ---------------------------------+-----------------------------+---------------------------------
#if !defined(DQN_NO_HEX)
DQN_API char const  *Dqn_Hex_TrimSpaceAnd0xPrefixCString8(char const *hex, Dqn_isize size, Dqn_isize *real_size);
DQN_API Dqn_String8  Dqn_Hex_TrimSpaceAnd0xPrefixString8(Dqn_String8 const string);

// NOTE: Byte to string conversions
// -----------------------------------------------------------------------------
struct Dqn_HexNumberString
{
    char   data[2 /*0x*/ + 16 /*hex*/ + 1 /*null-terminator*/];
    int8_t size;
};

enum Dqn_HexNumberStringFlags
{
    Dqn_HexNumberStringFlags_No0xPrefix   = 1 << 0, /// Remove the 0x prefix from the string
    Dqn_HexNumberStringFlags_UppercaseHex = 1 << 1, /// Use uppercase ascii characters for hex
};

/// Convert a 64 bit number to a hex string
///
/// @param[in] number Number to convert to hexadecimal representation
/// @param[in] flags Bit flags from Dqn_HexNumberStringFlags to customise the
/// output of the hexadecimal string.
///
/// @return The hexadecimal representation of the number. This string is always
/// null-terminated.
DQN_API Dqn_HexNumberString Dqn_Hex_U64ToNumberString(uint64_t number, uint32_t flags);

/// @copybrief Dqn_Hex_U64ToNumberString
///
/// @param[in] allocator The memory allocator to use for the memory of the
/// hexadecimal string.
/// @copyparams Dqn_Hex_U64ToNumberString
DQN_API Dqn_String8 Dqn_Hex_U64ToString8(Dqn_Allocator allocator, uint64_t number, uint32_t flags);

/// Convert a hexadecimal string a 64 bit value.
///
/// Asserts if the hex string is too big to be converted into a 64 bit number.
DQN_API uint64_t Dqn_Hex_CString8ToU64(char const *hex, Dqn_isize size);

/// @copydoc Dqn_Hex_String8ToU64
DQN_API uint64_t Dqn_Hex_String8ToU64(Dqn_String8 hex);

/// Convert a binary buffer into its hex representation into dest.
///
/// The dest buffer must be large enough to contain the hex representation, i.e.
/// atleast (src_size * 2).
///
/// @return True if the conversion into the dest buffer was successful, false
/// otherwise (e.g. invalid arguments).
DQN_API bool Dqn_Hex_BytesToCString8Buffer(void const *src, Dqn_isize src_size, char *dest, Dqn_isize dest_size);

/// Convert a series of bytes into a string
///
/// @return A null-terminated hex string, null pointer if allocation failed
DQN_API char *Dqn_Hex_BytesToCString8Arena(Dqn_Arena *arena, void const *bytes, Dqn_isize size);

/// @copydoc Dqn_Hex_BytesToCString8Arena
///
/// @return A hex string, the string is invalid if conversion failed.
DQN_API Dqn_String8 Dqn_Hex_BytesToString8Arena(Dqn_Arena *arena, void const *bytes, Dqn_isize size);

// NOTE: String to byte conversions
// -----------------------------------------------------------------------------
/// Convert a hex string into binary at `dest`.
///
/// The dest buffer must be large enough to contain the binary representation,
/// i.e. atleast ceil(hex_size / 2). This function will strip whitespace,
/// leading 0x/0X prefix from the string before conversion.
///
/// @param[in] hex The hexadecimal string to convert
/// @param[in] hex_size Size of the hex buffer. This function can handle an odd
/// size hex string i.e. "fff" produces 0xfff0.
/// @param[out] dest Buffer to write the bytes to
/// @param[out] dest_size Maximum number of bytes to write to dest
///
/// @return The number of bytes written to `dest_size`, this value will *never*
/// be greater than `dest_size`.
DQN_API Dqn_isize Dqn_Hex_CString8ToByteBuffer(char const *hex, Dqn_isize hex_size, void *dest, Dqn_isize dest_size);

/// String8 variant of @see Dqn_Hex_CString8ToByteBuffer
DQN_API Dqn_isize Dqn_Hex_String8ToByteBuffer(Dqn_String8 hex, void *dest, Dqn_isize dest_size);

/// Unchecked variant of @see Dqn_Hex_CString8ToByteBuffer
///
/// This function skips some string checks, it assumes the hex is a valid hex
/// stream and that the arguments are valid e.g. no trimming or 0x prefix
/// stripping is performed
DQN_API Dqn_isize Dqn_Hex_CString8ToByteBufferUnchecked(char const *hex, Dqn_isize hex_size, void *dest, Dqn_isize dest_size);

/// String8 variant of @see Dqn_Hex_CString8ToByteBufferUnchecked
DQN_API Dqn_isize Dqn_Hex_String8ToByteBufferUnchecked(Dqn_String8 hex, void *dest, Dqn_isize dest_size);

/// Dynamic allocating variant of @see Dqn_Hex_CString8ToByteBufferUnchecked
///
/// @param[in] arena The arena to allocate the bytes from
/// @param[in] hex Hex string to convert into bytes
/// @param[in] size Size of the hex string
/// @param[out] real_size The size of the buffer returned by the function
///
/// @return The byte representation of the hex string.
DQN_API char *Dqn_Hex_CString8ToBytesArena(Dqn_Arena *arena, char const *hex, Dqn_isize hex_size, Dqn_isize *real_size);

/// @copybrief Dqn_Hex_CString8ToBytesArena
///
/// @param[in] arena The arena to allocate the bytes from
/// @param[in] hex Hex string to convert into bytes
///
/// @return The byte representation of the hex string.
DQN_API Dqn_String8 Dqn_Hex_String8ToBytesArena(Dqn_Arena *arena, Dqn_String8 hex);
#endif // !defined(DQN_NO_HEX)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-DATE] Dqn_Date             |                             | Date-time helpers
// ---------------------------------+-----------------------------+---------------------------------
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

// @return The current time at the point of invocation
DQN_API Dqn_DateHMSTime       Dqn_Date_HMSLocalTimeNow();
DQN_API Dqn_DateHMSTimeString Dqn_Date_HMSLocalTimeStringNow(char date_separator = '-', char hms_separator = ':');
DQN_API Dqn_DateHMSTimeString Dqn_Date_HMSLocalTimeString(Dqn_DateHMSTime time, char date_separator = '-', char hms_separator = ':');

// return: The time elapsed since Unix epoch (1970-01-01T00:00:00Z) in seconds
DQN_API uint64_t Dqn_Date_EpochTime();

#if defined(DQN_OS_WIN32)
// ---------------------------------+-----------------------------+---------------------------------
// [SECT-WIND] Dqn_Win              |                             | Windows OS helpers
// ---------------------------------+-----------------------------+---------------------------------
struct Dqn_WinErrorMsg
{
    unsigned long code;
    char          data[DQN_KILOBYTES(64) - 1]; // Maximum error size
    unsigned long size;
};
DQN_API void Dqn_Win_LastErrorToBuffer(Dqn_WinErrorMsg *msg);
DQN_API Dqn_WinErrorMsg Dqn_Win_LastError();

/// Call once at application start-up to ensure that the application is DPI
/// aware on Windows and ensure that application UI is scaled up appropriately
/// for the monitor.
DQN_API void Dqn_Win_MakeProcessDPIAware();

// NOTE: Windows String8 To String16
// -----------------------------------------------------------------------------
/// Convert a UTF8 to UTF16 string.
///
/// The exact size buffer required for this function can be determined by
/// calling this function with the 'dest' set to null and 'dest_size' set to 0,
/// the return size is the size required for conversion not-including space for
/// the null-terminator. This function *always* null-terminates the input
/// buffer.
///
/// @return The number of u16's written/required for conversion. 0 if there was
/// a conversion error and can be queried using 'Dqn_Win_LastError'
DQN_API int          Dqn_Win_CString8ToCString16(const char *src, int src_size, wchar_t *dest, int dest_size);
DQN_API int          Dqn_Win_String8ToCString16(Dqn_String8 src, wchar_t *dest, int dest_size);
DQN_API Dqn_String16 Dqn_Win_String8ToString16Allocator(Dqn_String8 src, Dqn_Allocator allocator);

// NOTE: Windows String16 To String8
// -----------------------------------------------------------------------------
/// Convert a UTF16 to UTF8 string.
///
/// The exact size buffer required for this function can be determined by
/// calling this function with the 'dest' set to null and 'dest_size' set to 0,
/// the return size is the size required for conversion not-including space for
/// the null-terminator. This function *always* null-terminates the input
/// buffer.
///
/// @return The number of u8's written/required for conversion. 0 if there was
/// a conversion error and can be queried using 'Dqn_Win_LastError'
DQN_API int         Dqn_Win_CString16ToCString8(const wchar_t *src, int src_size, char *dest, int dest_size);
DQN_API Dqn_String8 Dqn_Win_CString16ToString8Allocator(const wchar_t *src, int src_size, Dqn_Allocator allocator);
DQN_API int         Dqn_Win_String16ToCString8(Dqn_String16 src, char *dest, int dest_size);
DQN_API Dqn_String8 Dqn_Win_String16ToString8Allocator(Dqn_String16 src, Dqn_Allocator allocator);

// NOTE: Windows Executable Directory
// -----------------------------------------------------------------------------
/// Evaluate the current executable's directory that is running when this
/// function is called.
/// @param[out] buffer The buffer to write the executable directory into. Set
/// this to null to calculate the required buffer size for the directory.
/// @param[in] size The size of the buffer given. Set this to 0 to calculate the
/// required buffer size for the directory.
/// @return The length of the executable directory string. If this return value
/// exceeds the capacity of the 'buffer', the 'buffer' is untouched.
DQN_API Dqn_isize    Dqn_Win_EXEDirW(wchar_t *buffer, Dqn_isize size);
DQN_API Dqn_String16 Dqn_Win_EXEDirWArena(Dqn_Arena *arena);

// @param[in] size (Optional) The size of the current directory string returned
// @param[in] suffix (Optional) A suffix to append to the current working directory
// @param[in] suffix_size (Optional) The size of the suffix to append
DQN_API Dqn_String8  Dqn_Win_WorkingDir(Dqn_Allocator allocator, Dqn_String8 suffix);
DQN_API Dqn_String16 Dqn_Win_WorkingDirW(Dqn_Allocator allocator, Dqn_String16 suffix);

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

DQN_API bool Dqn_Win_FolderIterate(Dqn_String8 path, Dqn_Win_FolderIterator *it);
DQN_API bool Dqn_Win_FolderWIterate(Dqn_String16 path, Dqn_Win_FolderIteratorW *it);

#if !defined(DQN_NO_WINNET)
// ---------------------------------+-----------------------------+---------------------------------
// [SECT-WINN] Dqn_WinNet           | DQN_NO_WINNET               | Windows internet download/query helpers
// ---------------------------------+-----------------------------+---------------------------------
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

// TODO(dqn): Useful options to expose in the handle
// https://docs.microsoft.com/en-us/windows/win32/wininet/option-flags
// INTERNET_OPTION_CONNECT_RETRIES -- default is 5 retries
// INTERNET_OPTION_CONNECT_TIMEOUT -- milliseconds
// INTERNET_OPTION_RECEIVE_TIMEOUT
// INTERNET_OPTION_SEND_TIMEOUT

// Setup a handle to the URL with the given HTTP verb.
DQN_API Dqn_WinNetHandle Dqn_Win_NetHandleInitCString(char const *url, int url_size);
DQN_API Dqn_WinNetHandle Dqn_Win_NetHandleInit(Dqn_String8 url);

// Setup a handle to the URL with the given HTTP verb.
//
// This function is the same as calling Dqn_Win_NetHandleInit() followed by
// Dqn_Win_NetHandleSetHTTPMethod().
//
// @param http_method The HTTP request type, e.g. "GET" or "POST" e.t.c
DQN_API Dqn_WinNetHandle Dqn_Win_NetHandleInitHTTPMethodCString(char const *url, int url_size, char const *http_method);
DQN_API Dqn_WinNetHandle Dqn_Win_NetHandleInitHTTPMethod(Dqn_String8 url, Dqn_String8 http_method);

DQN_API void             Dqn_Win_NetHandleClose(Dqn_WinNetHandle *handle);
DQN_API bool             Dqn_Win_NetHandleIsValid(Dqn_WinNetHandle const *handle);
DQN_API void             Dqn_Win_NetHandleSetUserAgentCString(Dqn_WinNetHandle *handle, char const *user_agent, int user_agent_size);

// Set the HTTP request method for the given handle. This function can be used
// on a pre-existing valid handle that has at the minimum been initialised.
DQN_API bool             Dqn_Win_NetHandleSetHTTPMethod(Dqn_WinNetHandle *handle, char const *method);

enum Dqn_WinNetHandleRequestHeaderFlag
{
    Dqn_WinNetHandleRequestHeaderFlag_Add,
    Dqn_WinNetHandleRequestHeaderFlag_AddIfNew,
    Dqn_WinNetHandleRequestHeaderFlag_Replace,
    Dqn_WinNetHandleRequestHeaderFlag_Count,
};

DQN_API bool Dqn_Win_NetHandleSetRequestHeaderCString8(Dqn_WinNetHandle *handle, char const *header, int header_size, uint32_t mode);
DQN_API bool Dqn_Win_NetHandleSetRequestHeaderString8(Dqn_WinNetHandle *handle, Dqn_String8 header, uint32_t mode);

struct Dqn_WinNetHandleResponse
{
    Dqn_String8  raw_headers;
    Dqn_String8 *headers;
    Dqn_isize    headers_size;

    // NOTE: Headers pulled from the 'raw_headers' for convenience
    uint64_t     content_length;
    Dqn_String8  content_type;
};
DQN_API Dqn_WinNetHandleResponse Dqn_Win_NetHandleSendRequest(Dqn_WinNetHandle *handle, Dqn_Allocator allocator, char const *post_data, unsigned long post_data_size);

DQN_API bool             Dqn_Win_NetHandlePump(Dqn_WinNetHandle *handle, char *dest, int dest_size, size_t *download_size);
DQN_API char *           Dqn_Win_NetHandlePumpCString8(Dqn_WinNetHandle *handle, Dqn_Arena *arena, size_t *download_size);
DQN_API Dqn_String8      Dqn_Win_NetHandlePumpString8(Dqn_WinNetHandle *handle, Dqn_Arena *arena);

DQN_API void             Dqn_Win_NetHandlePumpToCRTFile(Dqn_WinNetHandle *handle, FILE *file);
DQN_API char            *Dqn_Win_NetHandlePumpToAllocCString(Dqn_WinNetHandle *handle, size_t *download_size);
DQN_API Dqn_String8      Dqn_Win_NetHandlePumpToAllocString(Dqn_WinNetHandle *handle);
#endif // !defined(DQN_NO_WINNET)
#endif // defined(DQN_OS_WIN32)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-OSYS] Dqn_OS               | DQN_NO_WIN                  | Operating-system APIs
// ---------------------------------+-----------------------------+---------------------------------
/// Generate cryptographically secure bytes
DQN_API bool Dqn_OS_SecureRNGBytes(void *buffer, uint32_t size);

// return: The directory without the trailing '/' or ('\' for windows). Empty
//         string with a nullptr if it fails.
DQN_API Dqn_String8 Dqn_OS_EXEDir(Dqn_Allocator allocator);

DQN_API void Dqn_OS_SleepMs(Dqn_uint milliseconds);

DQN_API uint64_t Dqn_OS_PerfCounterNow   ();
DQN_API Dqn_f64  Dqn_OS_PerfCounterS     (uint64_t begin, uint64_t end);
DQN_API Dqn_f64  Dqn_OS_PerfCounterMs    (uint64_t begin, uint64_t end);
DQN_API Dqn_f64  Dqn_OS_PerfCounterMicroS(uint64_t begin, uint64_t end);
DQN_API Dqn_f64  Dqn_OS_PerfCounterNs    (uint64_t begin, uint64_t end);

/// Record time between two time-points using the OS's performance counter.
struct Dqn_OSTimer
{
    uint64_t start;
    uint64_t end;
};

DQN_API Dqn_OSTimer Dqn_OS_TimerBegin();
DQN_API void        Dqn_OS_TimerEnd    (Dqn_OSTimer *timer);
DQN_API Dqn_f64     Dqn_OS_TimerS      (Dqn_OSTimer timer);
DQN_API Dqn_f64     Dqn_OS_TimerMs     (Dqn_OSTimer timer);
DQN_API Dqn_f64     Dqn_OS_TimerMicroS (Dqn_OSTimer timer);
DQN_API Dqn_f64     Dqn_OS_TimerNs     (Dqn_OSTimer timer);

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
    DQN_ASSERT_MSG(timings_size_ < sizeof(timings_) / sizeof(timings_[0]),                              \
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

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-FSYS] Dqn_Fs               |                             | Filesystem helpers
// ---------------------------------+-----------------------------+---------------------------------
enum Dqn_FsInfoType
{
    Dqn_FsInfoType_Unknown,
    Dqn_FsInfoType_Directory,
    Dqn_FsInfoType_File,
};

struct Dqn_FsInfo
{
    Dqn_FsInfoType type;
    uint64_t create_time_in_s;
    uint64_t last_write_time_in_s;
    uint64_t last_access_time_in_s;
    uint64_t size;
};

// NOTE: File System API
// =============================================================================
// TODO(dqn): We should have a Dqn_String8 interface and a CString interface
DQN_API bool       Dqn_Fs_Exists(Dqn_String8 path);
DQN_API bool       Dqn_Fs_DirExists(Dqn_String8 path);
DQN_API Dqn_FsInfo Dqn_Fs_GetInfo(Dqn_String8 path);
DQN_API bool       Dqn_Fs_Copy(Dqn_String8 src, Dqn_String8 dest, bool overwrite);

DQN_API bool       Dqn_Fs_MakeDir(Dqn_String8 path);
DQN_API bool       Dqn_Fs_Move(Dqn_String8 src, Dqn_String8 dest, bool overwrite);

// TODO(dqn): This doesn't work on directories unless you delete the files
// in that directory first.
DQN_API bool       Dqn_Fs_Delete(Dqn_String8 path);

// NOTE: Read/Write Entire File API
// =============================================================================
// file_size: (Optional) The size of the file in bytes, the allocated buffer is (file_size + 1 [null terminator]) in bytes.
DQN_API bool       Dqn_Fs_WriteCString8(char const *file_path, Dqn_isize file_path_size, char const *buffer, Dqn_isize buffer_size);
DQN_API bool       Dqn_Fs_WriteString8(Dqn_String8 file_path, Dqn_String8 buffer);

/// Read a file at the specified path into memory.
/// @param[in] path Path to the file to read
/// @param[in] path_size The string size of the file path
/// @param[out] file_size (Optional) Pass a pointer to receive the number of bytes read
/// @param[in] allocator Allocator used to read the file to memory with
/// @return A cstring with the read file, null pointer on failure.
#define              Dqn_Fs_ReadCString8(path, path_size, file_size, allocator) Dqn_Fs_ReadCString8_(DQN_LEAK_TRACE path, path_size, file_size, allocator)
DQN_API char        *Dqn_Fs_ReadCString8_(DQN_LEAK_TRACE_FUNCTION char const *path, Dqn_isize path_size, Dqn_isize *file_size, Dqn_Allocator allocator);

/// Read a file at the specified path into memory.
/// @param[in] file_path Path to the file to read
/// @param[in] allocator Allocator used to read the file to memory with
/// @return A string with the read file, invalid string on failure.
#define              Dqn_Fs_ReadString8(path, allocator) Dqn_Fs_ReadString8_(DQN_LEAK_TRACE path, allocator)
DQN_API Dqn_String8  Dqn_Fs_ReadString8_(DQN_LEAK_TRACE_FUNCTION Dqn_String8 path, Dqn_Allocator allocator);

// NOTE: Read/Write File Stream API
// =============================================================================
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

DQN_API Dqn_FsFile Dqn_Fs_OpenFile(Dqn_String8 path, Dqn_FsFileOpen open_mode, uint32_t access);
DQN_API bool Dqn_Fs_WriteFile(Dqn_FsFile *file, char const *buffer, Dqn_isize size);
DQN_API void Dqn_Fs_CloseFile(Dqn_FsFile *file);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-MISC] Miscellaneous        |                             | General purpose helpers
// ---------------------------------+-----------------------------+---------------------------------
/// Write the format string to the buffer truncating with a trailing ".." if
/// there is insufficient space in the buffer followed by null-terminating the
/// buffer (uses stb_sprintf underneath).
/// @return The size of the string written to the buffer *not* including the
/// null-terminator.
DQN_API int Dqn_SNPrintF2DotsOnOverflow(char *buffer, int size, char const *fmt, ...);

struct Dqn_U64String
{
    char    data[27+1]; // NOTE(dqn): 27 is the maximum size of uint64_t including a separtor
    uint8_t size;
};

/// Convert a 64 bit unsigned value to its string representation.
/// @param[in] val Value to convert into a string
/// @param[in] separator The separator to insert every 3 digits. Set this to
/// 0 if no separator is desired.
DQN_API Dqn_U64String Dqn_U64ToString(uint64_t val, char separator);

// NOTE: Binary Search
// -----------------------------------------------------------------------------
template <typename T>
using Dqn_BinarySearchLessThanProc = bool(T const &lhs, T const &rhs);

template <typename T>
DQN_FORCE_INLINE bool Dqn_BinarySearch_DefaultLessThan(T const &lhs, T const &rhs);

enum Dqn_BinarySearchType
{
    Dqn_BinarySearchType_Match,
    Dqn_BinarySearchType_OnePastMatch,
};

template <typename T>
Dqn_isize
Dqn_BinarySearch(T const                        *array,
                 Dqn_isize                       array_size,
                 T const                        &find,
                 Dqn_BinarySearchType            type      = Dqn_BinarySearchType_Match,
                 Dqn_BinarySearchLessThanProc<T> less_than = Dqn_BinarySearch_DefaultLessThan);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-TCTX] Dqn_ThreadContext    |                             | Per-thread data structure e.g. temp arenas
// ---------------------------------+-----------------------------+---------------------------------
// Utility functions for building applications by providing an in-built thread
// context that gives the user access to a temporary arena automatically without
// explicit setup on the end application's side.

#if !defined(DQN_THREAD_CONTEXT_ARENAS)
    #define DQN_THREAD_CONTEXT_ARENAS 2
#endif

struct Dqn_ThreadContext
{
    Dqn_b32       init;

    /// Temp memory arena's for the calling thread
    Dqn_Arena     temp_arenas[DQN_THREAD_CONTEXT_ARENAS];

    /// Allocators that use the corresponding arena from the thread context.
    /// Provided for convenience when interfacing with allocator interfaces.
    Dqn_Allocator temp_allocators[DQN_THREAD_CONTEXT_ARENAS];

    #if defined(DQN_DEBUG_THREAD_CONTEXT)
    Dqn_ArenaStat temp_arenas_stat[DQN_THREAD_CONTEXT_ARENAS];
    #endif
};

struct Dqn_ThreadScratch
{
    Dqn_ThreadScratch(DQN_LEAK_TRACE_FUNCTION Dqn_ThreadContext *context, uint8_t context_index);
    ~Dqn_ThreadScratch();

    /// Index into the arena/allocator/stat array in the thread context
    /// specifying what arena was assigned.
    uint8_t              index;
    Dqn_Allocator        allocator;
    Dqn_Arena           *arena;
    Dqn_b32              destructed = false; /// Detect copies of the scratch
    Dqn_ArenaTempMemory  temp_memory;
    #if defined(DQN_LEAK_TRACKING)
    Dqn_CallSite         leak_site__;
    #endif
};

// Get the current thread's context- this contains all the metadata for managing
// the thread scratch data. In general you probably want Dqn_Thread_GetScratch()
// which ensures you get a usable scratch arena for temporary allocations
// without having to worry about selecting the right arena from the state.
#define Dqn_Thread_GetContext() Dqn_Thread_GetContext_(DQN_LEAK_TRACE_NO_COMMA)
DQN_API Dqn_ThreadContext *Dqn_Thread_GetContext_(DQN_LEAK_TRACE_FUNCTION_NO_COMMA);

/// Retrieve the per-thread temporary arena allocator that is reset on scope
/// exit.
///
/// The scratch arena must be deconflicted with any existing arenas in the
/// function to avoid trampling over each other's memory. Consider the situation
/// where the scratch arena is passed into the function. Inside the function, if
/// the same arena is reused then, if both arenas allocate, when the inner arena
/// is reset, this will undo the passed in arena's allocations in the function.
///
/// @param[in] conflict_arena A pointer to the arena currently being used in the
/// function
#define Dqn_Thread_GetScratch(conflict_arena) Dqn_Thread_GetScratch_(DQN_LEAK_TRACE conflict_arena)
DQN_API Dqn_ThreadScratch Dqn_Thread_GetScratch_(DQN_LEAK_TRACE_FUNCTION void const *conflict_arena);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-JSON] Dqn_JSONBuilder      | DQN_NO_JSON_BUILDER         | Construct json output
// ---------------------------------+-----------------------------+---------------------------------
#if !defined(DQN_NO_JSON_BUILDER)
// TODO(dqn): We need to write tests for this
struct Dqn_JSONBuilder
{
    uint16_t            parent_field_count_stack[32];
    int                 parent_field_count_stack_size;
    Dqn_String8Builder  builder;
    int                 indent_level;
    int                 spaces_per_indent;
};

#define Dqn_JSONBuilder_Object(writer)                  \
    DQN_DEFER_LOOP(Dqn_JSONBuilder_ObjectBegin(writer), \
                   Dqn_JSONBuilder_ObjectEnd(writer))

#define Dqn_JSONBuilder_ObjectNamed(writer, name)                  \
    DQN_DEFER_LOOP(Dqn_JSONBuilder_ObjectBeginNamed(writer, name), \
                   Dqn_JSONBuilder_ObjectEnd(writer))

#define Dqn_JSONBuilder_Array(writer)                  \
    DQN_DEFER_LOOP(Dqn_JSONBuilder_ArrayBegin(writer), \
                   Dqn_JSONBuilder_ArrayEnd(writer))

#define Dqn_JSONBuilder_ArrayNamed(writer, name)                  \
    DQN_DEFER_LOOP(Dqn_JSONBuilder_ArrayBeginNamed(writer, name), \
                   Dqn_JSONBuilder_ArrayEnd(writer))

DQN_API Dqn_JSONBuilder Dqn_JSONBuilder_Init(Dqn_Arena *arena, int spaces_per_indent);
DQN_API Dqn_String8     Dqn_JSONBuilder_Build(Dqn_JSONBuilder *builder, Dqn_Arena *arena);

DQN_API void            Dqn_JSONBuilder_ObjectBeginNamed(Dqn_JSONBuilder *builder, Dqn_String8 name);
DQN_API void            Dqn_JSONBuilder_ObjectBegin(Dqn_JSONBuilder *builder);
DQN_API void            Dqn_JSONBuilder_ObjectEnd(Dqn_JSONBuilder *builder);

DQN_API void            Dqn_JSONBuilder_ArrayBeginNamed(Dqn_JSONBuilder *builder, Dqn_String8 name);
DQN_API void            Dqn_JSONBuilder_ArrayBegin(Dqn_JSONBuilder *builder);
DQN_API void            Dqn_JSONBuilder_ArrayEnd(Dqn_JSONBuilder *builder);

DQN_API void            Dqn_JSONBuilder_StringNamed(Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value);
DQN_API void            Dqn_JSONBuilder_String(Dqn_JSONBuilder *builder, Dqn_String8 value);

DQN_API void            Dqn_JSONBuilder_LiteralNamed(Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value);
DQN_API void            Dqn_JSONBuilder_Literal(Dqn_JSONBuilder *builder, Dqn_String8 value);

DQN_API void            Dqn_JSONBuilder_U64Named(Dqn_JSONBuilder *builder, Dqn_String8 key, uint64_t value);
DQN_API void            Dqn_JSONBuilder_U64(Dqn_JSONBuilder *builder, uint64_t value);

DQN_API void            Dqn_JSONBuilder_I64Named(Dqn_JSONBuilder *builder, Dqn_String8 key, uint64_t value);
DQN_API void            Dqn_JSONBuilder_I64(Dqn_JSONBuilder *builder, uint64_t value);

/// @param[in] decimal_places When < 0 show the default number of decimal places
/// When >=0 show the specified amount of decimal places
DQN_API void            Dqn_JSONBuilder_F64Named(Dqn_JSONBuilder *builder, Dqn_String8 key, double value, int decimal_places);
DQN_API void            Dqn_JSONBuilder_F64(Dqn_JSONBuilder *builder, double value, int decimal_places);

DQN_API void            Dqn_JSONBuilder_BoolNamed(Dqn_JSONBuilder *builder, Dqn_String8 key, bool value);
DQN_API void            Dqn_JSONBuilder_Bool(Dqn_JSONBuilder *builder, bool value);
#endif // !defiend(DQN_NO_JSON_BUIDLER)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-FNV1] Dqn_FNV1A            |                             | Hash(x) -> 32/64bit via FNV1a
// ---------------------------------+-----------------------------+---------------------------------
#ifndef DQN_FNV1A32_SEED
    #define DQN_FNV1A32_SEED 2166136261U
#endif

#ifndef DQN_FNV1A64_SEED
    #define DQN_FNV1A64_SEED 14695981039346656037ULL
#endif

/// @begincode
/// char buffer1[128] = {random bytes};
/// char buffer2[128] = {random bytes};
/// uint64_t hash     = Dqn_FNV1A64_Hash(buffer1, sizeof(buffer1));
/// hash              = Dqn_FNV1A64_Iterate(buffer2, sizeof(buffer2), hash); // subsequent hashing
/// @endcode
DQN_API uint32_t Dqn_FNV1A32_Hash   (void const *bytes, Dqn_isize size);
DQN_API uint64_t Dqn_FNV1A64_Hash   (void const *bytes, Dqn_isize size);
DQN_API uint32_t Dqn_FNV1A32_Iterate(void const *bytes, Dqn_isize size, uint32_t hash);
DQN_API uint64_t Dqn_FNV1A64_Iterate(void const *bytes, Dqn_isize size, uint64_t hash);

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-MMUR] Dqn_MurmurHash3      |                             | Hash(x) -> 32/128bit via MurmurHash3
// ---------------------------------+-----------------------------+---------------------------------
/// MurmurHash3 was written by Austin Appleby, and is placed in the public
/// domain. The author (Austin Appleby) hereby disclaims copyright to this source
/// code.
///
/// Note - The x86 and x64 versions do _not_ produce the same results, as the
/// algorithms are optimized for their respective platforms. You can still
/// compile and run any of them on any platform, but your performance with the
/// non-native version will be less than optimal.
struct Dqn_MurmurHash3 { uint64_t e[2]; };

DQN_API uint32_t        Dqn_MurmurHash3_x86U32 (void const *key, int len, uint32_t seed);
DQN_API Dqn_MurmurHash3 Dqn_MurmurHash3_x64U128(void const *key, int len, uint32_t seed);
#define Dqn_MurmurHash3_x64U128AsU64(key, len, seed) (Dqn_MurmurHash3_x64U128(key, len, seed).e[0])

// NOTE: Dqn_Map Template Implementation
// -------------------------------------------------------------------------------------------------
#if !defined(DQN_NO_MAP)
template <typename T>
Dqn_Map<T> Dqn_Map_InitWithArena(Dqn_Arena *arena, Dqn_isize size)
{
    Dqn_Map<T> result = {};
    result.arena      = arena;

    Dqn_isize final_size          = size == 0 ? 4096 : size;
    result.slots                  = Dqn_Arena_NewArray(arena, Dqn_MapEntry<T> *, final_size, Dqn_ZeroMem_Yes);
    if (result.slots) result.size = final_size;
    return result;
}

template <typename T>
Dqn_MapEntry<T> *Dqn_Map_FindOrAdd_(DQN_LEAK_TRACE_FUNCTION Dqn_Map<T> *map, uint64_t hash, Dqn_MapCollideRule rule)
{
    Dqn_isize         index = hash % map->size;
    Dqn_MapEntry<T> *result = map->slots[index];

    if (result) {
        if (rule == Dqn_MapCollideRule::Chain) {
            while (result->hash != hash) {
                if (result->next)
                    result = result->next;
                else {
                    map->chain_count++;
                    result->next = (Dqn_MapEntry<T> *)Dqn_Arena_Allocate_(DQN_LEAK_TRACE_ARG map->arena, sizeof(Dqn_MapEntry<T>), alignof(Dqn_MapEntry<T>), Dqn_ZeroMem_Yes);
                    result       = result->next;
                    break;
                }
            }
        } else if (rule == Dqn_MapCollideRule::Fail) {
            result = nullptr;
        }
    } else {
        result = (Dqn_MapEntry<T> *)Dqn_Arena_Allocate_(DQN_LEAK_TRACE_ARG map->arena, sizeof(Dqn_MapEntry<T>), alignof(Dqn_MapEntry<T>), Dqn_ZeroMem_Yes);
        map->count++;
        map->slots[index] = result;
    }

    if (result)
        result->hash = hash;

    return result;
}

template <typename T>
Dqn_MapEntry<T> *Dqn_Map_Add_(DQN_LEAK_TRACE_FUNCTION Dqn_Map<T> *map, uint64_t hash, T &value, Dqn_MapCollideRule rule)
{
    Dqn_MapEntry<T> *result = Dqn_Map_FindOrAdd(DQN_LEAK_TRACE_ARG map, hash, rule);
    if (result)
        result->value = value;

    return result;
}

template <typename T>
Dqn_MapEntry<T> *Dqn_Map_AddCopy_(DQN_LEAK_TRACE_FUNCTION Dqn_Map<T> *map, uint64_t hash, T const &value, Dqn_MapCollideRule rule)
{
    Dqn_MapEntry<T> *result = Dqn_Map_FindOrAdd(DQN_LEAK_TRACE_ARG map, hash, rule);
    if (result)
        result->value = value;

    return result;
}

template <typename T>
Dqn_MapEntry<T> *Dqn_Map_Get(Dqn_Map<T> *map, uint64_t hash)
{
    Dqn_isize         index  = hash % map->size;
    Dqn_MapEntry<T> *result = nullptr;
    for (Dqn_MapEntry<T> *entry = map->slots[index];
         !result && entry;
         entry = entry->next)
    {
        result = (entry->hash == hash) ? entry : nullptr;
    }

    return result;
}

template <typename T>
void Dqn_Map_Erase(Dqn_Map<T> *map, uint64_t hash, Dqn_ZeroMem zero_mem)
{
    Dqn_isize         index = hash % map->size;
    Dqn_MapEntry<T> **entry = &(map->slots[index]);
    bool is_chain_entry = *entry && (*entry)->next;

    while ((*entry) && (*entry)->hash != hash)
        entry = &((*entry)->next);

    bool result = false;
    if ((*entry) && (*entry)->hash == hash) {
        result = true;
        Dqn_MapEntry<T> *erase_entry = *entry;
        Dqn_MapEntry<T> *next        = erase_entry->next;
        (*entry)                     = next;

        if (zero_mem == Dqn_ZeroMem_Yes)
            DQN_MEMSET(erase_entry, DQN_MEMSET_BYTE, sizeof(*erase_entry));

        erase_entry->next = map->free_list;
        map->free_list    = erase_entry;

        if (is_chain_entry) map->chain_count--;
        else map->count--;
    }
}
#endif // !defined(DQN_NO_MAP)

// NOTE: Dqn_DSMap Template Implementation
// -------------------------------------------------------------------------------------------------
#if !defined(DQN_NO_DSMAP)
DQN_DS_MAP_API
DQN_DS_MAP Dqn_DSMap_Init(Dqn_isize size)
{
    DQN_DS_MAP result = {};
    if (Dqn_Safe_AssertF(size > 0 && (size & (size - 1)) == 0, "Non-zero & power of 2 table size required")) {
        // TODO: We need to use placement new for C++ crap
        result.slots = (Dqn_DSMapItem<K, V> *)Dqn_Arena_Allocate_(DQN_LEAK_TRACE &result.arena,
                                                                  sizeof(Dqn_DSMapItem<K, V>) * size,
                                                                  alignof(Dqn_DSMapItem<K, V>),
                                                                  Dqn_ZeroMem_Yes);
        if (result.slots)
            result.size = size;
    }
    return result;
}

DQN_DS_MAP_API
void Dqn_DSMap_Free(DQN_DS_MAP *map, Dqn_ZeroMem zero_mem)
{
    Dqn_Arena_Free(&map->arena, zero_mem);
    *map = {};
}

DQN_DS_MAP_API
Dqn_DSMapItem<K, V> *Dqn_DSMap_Find(DQN_DS_MAP *map, uint64_t hash)
{
    Dqn_DSMapItem<K, V> *result = Dqn_DSMap_FindOrAdd(map, hash, true /*find_only*/);
    return result;
}

DQN_FILE_SCOPE uint64_t Dqn_DSMap_KeyHash_(Dqn_DSMapHashProc *hash_function, void const *key, int key_size)
{
    uint64_t result;
    if (hash_function)
        result = hash_function(&key, key_size);
    else
        result = Dqn_MurmurHash3_x64U128AsU64(&key, key_size, 0xc8f451ac/*seed*/);
    return result;
}

DQN_DS_MAP_API
Dqn_DSMapItem<K, V> *Dqn_DSMap_FindOrAdd(DQN_DS_MAP *map, K const &key, Dqn_DSMapFindOnly find_only, Dqn_DSMapItemCreated *created)
{
    if (created)
        *created = Dqn_DSMapItemCreated_No;

    if (!map)
        return nullptr;

    if (!map->slots) {
        if (find_only)
            return nullptr;
        else
            *map = Dqn_DSMap_Init<K, V>(DQN_DSMAP_MIN_SIZE);
    }

    // NOTE: Search for a vacant entry, linear probe if hash collision
    uint64_t hash               = Dqn_DSMap_KeyHash_(map->hash_function, &key, sizeof(key));
    Dqn_isize            index  = hash % map->size;
    Dqn_DSMapItem<K, V> *result = map->slots + index;
    while (result->occupied && result->key != key) {
        index  = (index + 1) % map->size;
        result = map->slots + index;
    }

    if (result->occupied) {
        DQN_ASSERT_MSG(result->key == key,
                       "We have a max load factor of 70%% so we should never get an occupied slot that doesn't match "
                       "the hash we were searching for");
    } else if (find_only == Dqn_DSMapFindOnly_Yes) {
        result = nullptr;
    } else {
        // NOTE: Fill the entry
        result->key      = key;
        result->occupied = true;

        if (created)
            *created = Dqn_DSMapItemCreated_Yes;

        // NOTE: Add the entry to the linked list of items (for iterability)
        if (map->link_tail) {
            map->link_tail->next = result;
            result->prev         = map->link_tail;
        } else {
            map->link_head = result;
        }
        map->link_tail = result;

        // NOTE: Grow the hashtable if we hit the loading limit
        Dqn_f32 load_factor = ++map->count / DQN_CAST(Dqn_f32)map->size;
        if (load_factor >= 0.7f) {

            auto new_map = Dqn_DSMap_Init<K, V>(map->size << 1);
            for (Dqn_DSMapItem<K, V> *entry = map->link_head; entry; entry = entry->next) {
                if (!entry->occupied)
                    continue;

                Dqn_DSMapItem<K, V> *new_entry = Dqn_DSMap_Add(&new_map, entry->key, entry->value);
                if (new_entry->key == key)
                    result = new_entry;
            }

            Dqn_DSMap_Free(map, Dqn_ZeroMem_No);
            *map = new_map;
        }
    }

    return result;
}

DQN_DS_MAP_API
Dqn_DSMapItem<K, V> *Dqn_DSMap_Add(DQN_DS_MAP *map, K const &key, V const &value)
{
    Dqn_DSMapItem<K, V> *result = Dqn_DSMap_FindOrAdd(map, key, Dqn_DSMapFindOnly_No, nullptr /*added*/);
    if (result)
        result->value = value;

    return result;
}

DQN_DS_MAP_API
Dqn_DSMapItem<K, V> *Dqn_DSMap_Make(DQN_DS_MAP *map, K const &key)
{
    Dqn_DSMapItem<K, V> *result = Dqn_DSMap_FindOrAdd(map, key, Dqn_DSMapFindOnly_No, nullptr /*added*/);
    return result;
}

DQN_DS_MAP_API
Dqn_DSMapItem<K, V> *Dqn_DSMap_Get(DQN_DS_MAP *map, K const &key)
{
    Dqn_DSMapItem<K, V> *result = Dqn_DSMap_FindOrAdd(map, key, Dqn_DSMapFindOnly_Yes, nullptr /*added*/);
    return result;
}

DQN_DS_MAP_API
void Dqn_DSMap_Erase(DQN_DS_MAP *map, K const &key, Dqn_ZeroMem zero_mem)
{
    if (!map)
        return;

    uint64_t hash               = Dqn_DSMap_KeyHash_(map, &key, sizeof(key));
    Dqn_isize            index  = hash % map->size;
    Dqn_DSMapItem<K, V> *result = map->slots + index;
    if (!result || !result->occupied)
        return;

    // NOTE: Unlink the item
    if (result->prev)
        result->prev->next = result->next;
    if (result->next)
        result->next->prev = result->prev;

    result->next = nullptr;
    result->prev = nullptr;

    // NOTE: Probe forward in the hash table and fill the gap with adjacent
    // entries that were linear-probed on the entry we just deleted
    Dqn_isize start_index = index;
    Dqn_isize probe_index = index;
    for (;;) {
        probe_index                = (probe_index + 1) % map->size;
        Dqn_DSMapItem<K, V> *probe = map->slots + probe_index;
        if (!probe->occupied)
            break;

        Dqn_isize desired_index = probe->hash % map->size;
        if (desired_index != probe_index) {
            map->slots[start_index] = map->slots[probe_index];
            start_index             = probe_index;
            DQN_ASSERT(map->slots[start_index].occupied);
            DQN_ASSERT(map->slots[probe_index].occupied);
        }
    }

    // NOTE: Delete the entry, note that if there were probes, this will end up
    // deleting the last item in the chain which is what we want because
    // essentially every entry gets shifted up the chain by 1 spot.
    DQN_ASSERT(map->slots[start_index].occupied);
    map->slots[start_index].occupied = false;
    map->count -= 1;

    if (zero_mem == Dqn_ZeroMem_Yes) {
        DQN_MEMSET(map->slots + start_index, DQN_MEMSET_BYTE, sizeof(map->slots[start_index]));
    }
}
#endif // !defined(DQN_NO_DSMAP)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-DLIB] Dqn_Library          |                             | Library run-time behaviour configuration
// ---------------------------------+-----------------------------+---------------------------------
DQN_API void Dqn_Library_SetLogCallback(Dqn_LogProc *proc, void *user_data)
{
    dqn_library.log_callback  = proc;
    dqn_library.log_user_data = user_data;
}

DQN_API void Dqn_Library_SetLogFile(FILE *file)
{
    Dqn_TicketMutex_Begin(&dqn_library.log_file_mutex);
    dqn_library.log_file    = file;
    dqn_library.log_to_file = file ? true : false;
    Dqn_TicketMutex_End(&dqn_library.log_file_mutex);
}

DQN_API void Dqn_Library_DumpThreadContextArenaStat(Dqn_String8 file_path)
{
    #if defined(DQN_DEBUG_THREAD_CONTEXT)
    // NOTE: Open a file to write the arena stats to
    FILE *file = nullptr;
    fopen_s(&file, file_path.data, "a+b");
    if (file) {
        Dqn_Log_ErrorF("Failed to dump thread context arenas [file=%.*s]", DQN_STRING_FMT(file_path));
        return;
    }

    // NOTE: Copy the stats from library book-keeping
    // NOTE: Extremely short critical section, copy the stats then do our
    // work on it.
    Dqn_ArenaStat stats[Dqn_CArray_CountI(dqn_library.thread_context_arena_stats)];
    int stats_size = 0;

    Dqn_TicketMutex_Begin(&dqn_library.thread_context_mutex);
    stats_size = dqn_library.thread_context_arena_stats_count;
    DQN_MEMCPY(stats, dqn_library.thread_context_arena_stats, sizeof(stats[0]) * stats_size);
    Dqn_TicketMutex_End(&dqn_library.thread_context_mutex);

    // NOTE: Print the cumulative stat
    Dqn_DateHMSTimeString now = Dqn_Date_HMSLocalTimeStringNow();
    fprintf(file,
            "Time=%.*s %.*s | Thread Context Arenas | Count=%d\n",
            now.date_size, now.date,
            now.hms_size, now.hms,
            dqn_library.thread_context_arena_stats_count);

    // NOTE: Write the cumulative thread arena data
    {
        Dqn_ArenaStat stat = {};
        for (Dqn_isize index = 0; index < stats_size; index++) {
            Dqn_ArenaStat const *current = stats + index;
            stat.capacity += current->capacity;
            stat.used     += current->used;
            stat.wasted   += current->wasted;
            stat.blocks   += current->blocks;

            stat.capacity_hwm = DQN_MAX(stat.capacity_hwm, current->capacity_hwm);
            stat.used_hwm     = DQN_MAX(stat.used_hwm, current->used_hwm);
            stat.wasted_hwm   = DQN_MAX(stat.wasted_hwm, current->wasted_hwm);
            stat.blocks_hwm   = DQN_MAX(stat.blocks_hwm, current->blocks_hwm);
        }

        Dqn_ArenaStatString stats_string = Dqn_Arena_StatString(&stat);
        fprintf(file, "  [ALL] CURR %.*s\n", stats_string.size, stats_string.data);
    }

    // NOTE: Print individual thread arena data
    for (Dqn_isize index = 0; index < stats_size; index++) {
        Dqn_ArenaStat const *current = stats + index;
        Dqn_ArenaStatString current_string = Dqn_Arena_StatString(current);
        fprintf(file, "  [%03d] CURR %.*s\n", DQN_CAST(int)index, current_string.size, current_string.data);
    }

    fclose(file);
    DQN_LOG_I("Dumped thread context arenas [file=%.*s]", DQN_STRING_FMT(file_path));
    #else
    (void)file_path;
    #endif // #if defined(DQN_DEBUG_THREAD_CONTEXT)
}

#if defined(DQN_LEAK_TRACKING)
DQN_API void Dqn_Library_LeakTraceAdd(Dqn_CallSite call_site, void *ptr, Dqn_usize size)
{
    if (!ptr)
        return;

    Dqn_TicketMutex_Begin(&dqn_library.alloc_table_mutex);
    Dqn_DSMapItemCreated created       = Dqn_DSMapItemCreated_No;
    Dqn_DSMapItem<Dqn_LeakTrace> *item = Dqn_DSMap_FindOrAdd(&dqn_library.alloc_table, (uintptr_t)ptr, Dqn_DSMapFindOnly_No, &created);

    // NOTE: If the entry was not added, we are reusing a pointer that has been freed.
    Dqn_LeakTrace *trace = &item->value;
    if (created == Dqn_DSMapItemCreated_No) {
        DQN_HARD_ASSERT_MSG(trace->freed, "This pointer is already in the leak tracker, however it"
                                          " has not been freed yet. Somehow this pointer has been"
                                          " given to the allocation table and has not being marked"
                                          " freed with an equivalent call to LeakTraceMarkFree()"
                                          " [ptr=%p, size=%_$$d, file=\"%.*s:%u\","
                                          " function=\"%.*s\"]",
                                          ptr,
                                          size,
                                          DQN_STRING_FMT(trace->call_site.file),
                                          trace->call_site.line,
                                          DQN_STRING_FMT(trace->call_site.function));
    }

    trace->ptr       = ptr;
    trace->size      = size;
    trace->call_site = call_site;
    Dqn_TicketMutex_End(&dqn_library.alloc_table_mutex);
}

DQN_API void Dqn_Library_LeakTraceMarkFree(Dqn_CallSite call_site, void *ptr)
{
    if (!ptr)
        return;

    Dqn_TicketMutex_Begin(&dqn_library.alloc_table_mutex);

    Dqn_DSMapItem<Dqn_LeakTrace> *item = Dqn_DSMap_Get(&dqn_library.alloc_table, DQN_CAST(uintptr_t)ptr);
    DQN_HARD_ASSERT_MSG(item, "Allocated pointer can not be removed as it does not exist in the"
                              " allocation table. When this memory was allocated, the pointer was"
                              " not added to the allocation table [ptr=%p]",
                              ptr);

    Dqn_LeakTrace *trace = &item->value;
    DQN_HARD_ASSERT_MSG(!trace->freed,
                        "Double free detected, pointer was previously allocated at [ptr=%p, %_$$d, file=\"%.*s:%u\", function=\"%.*s\"]",
                        ptr,
                        trace->size,
                        DQN_STRING_FMT(trace->call_site.file),
                        trace->call_site.line,
                        DQN_STRING_FMT(trace->call_site.function));

    trace->freed           = true;
    trace->freed_size      = trace->size;
    trace->freed_call_site = call_site;
    Dqn_TicketMutex_End(&dqn_library.alloc_table_mutex);
}
#endif /// defined(DQN_LEAK_TRACKING)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-FSTR] Dqn_FString8         | DQN_NO_FSTRING8             | Fixed-size strings
// ---------------------------------+-----------------------------+---------------------------------
#if !defined(DQN_NO_FSTRING8)
DQN_FSTRING8_API
DQN_FSTRING8 Dqn_FString8_InitF(char const *fmt, ...)
{
    DQN_FSTRING8 result = {};
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        Dqn_FString8_AppendFV(&result, fmt, args);
        va_end(args);
    }
    return result;
}

DQN_FSTRING8_API
Dqn_isize Dqn_FString8_Max(DQN_FSTRING8 *)
{
    Dqn_isize result = MAX_;
    return result;
}

DQN_FSTRING8_API
void Dqn_FString8_Clear(DQN_FSTRING8 *string)
{
    *str = {};
}

DQN_FSTRING8_API
bool Dqn_FString8_AppendFV(DQN_FSTRING8 *string, char const *fmt, va_list args)
{
    bool result = false;
    if (!string || !fmt)
        return result;

    DQN_HARD_ASSERT(string->size >= 0);
    Dqn_isize require = Dqn_CString8_FVSize(fmt, args) + 1 /*null_terminate*/;
    Dqn_isize space   = N - string->size;
    result            = require <= space;
    string->size += STB_SPRINTF_DECORATE(vsnprintf)(string->data + string->size, DQN_CAST(int)space, fmt, args);

    // NOTE: snprintf returns the required size of the format string
    // irrespective of if there's space or not, but, always null terminates so
    // the last byte is wasted.
    string->size = DQN_MIN(string->size, N - 1);
    return result;
}

DQN_FSTRING8_API
bool Dqn_FString8_AppendF(DQN_FSTRING8 *string, char const *fmt, ...)
{
    bool result = false;
    if (!string || !fmt)
        return result;

    DQN_HARD_ASSERT(string->size >= 0);

    va_list args;
    va_start(args, fmt);
    result = Dqn_FString8_AppendFV(string, fmt, args);
    va_end(args);
    return result;
}

DQN_FSTRING8_API
bool Dqn_FString8_AppendCString8(DQN_FSTRING8 *string, char const *src, Dqn_isize size)
{
    bool result = false;
    if (!string || !src || size == 0)
        return result;

    if (size <= -1) {
        size = DQN_CAST(Dqn_isize)Dqn_CString8_Size(src);
    }

    Dqn_isize space = (N - 1 /*reserve byte for null terminator*/) - string->size;
    result          = size <= space;
    DQN_MEMCPY(string->data + string->size, src, DQN_MIN(space, size));
    string->size = DQN_MIN(string->size + size, N);
    string->data[string->size] = 0;
    return result;
}

DQN_FSTRING8_API
bool Dqn_FString8_AppendString8(DQN_FSTRING8 *string, Dqn_String8 src)
{
    bool result = Dqn_FString8_Append(string, src.data, src.size);
    return result;
}

DQN_FSTRING8_API
Dqn_String8 Dqn_FString8_ToString8(DQN_FSTRING8 const *string)
{
    Dqn_String8 result = {};
    if (!string)
        return result;

    DQN_HARD_ASSERT(string->size >= 0);
    result.data = DQN_CAST(char *)string->data;
    result.size = string->size;
    return result;
}
#endif // !defined(DQN_NO_FSTRING8)

// NOTE: Dqn_FArray Template Implementation
// -------------------------------------------------------------------------------------------------
#if !defined(DQN_NO_FARRAY)
DQN_FARRAY_API
DQN_FARRAY Dqn_FArray_Init(T const *item, Dqn_isize count)
{
    DQN_FARRAY result = {};
    Dqn_FArray_AddArray(&result, item, count);
    return result;
}

DQN_FARRAY_API
Dqn_isize Dqn_FArray_Max(DQN_FARRAY const *)
{
    Dqn_isize result = N;
    return result;
}

DQN_FARRAY_API
T *Dqn_FArray_AddArray(DQN_FARRAY *array, T const *items, Dqn_isize count)
{
    T *result = nullptr;
    if (!array || !items || count <= 0)
        return result;

    DQN_ASSERT(array->size >= 0);
    if (Dqn_Safe_AssertF(array->size + count < N, "Array out of space")) {
        result = array->data + array->size;
        array->size += count;
        DQN_MEMCPY(result, items, sizeof(T) * count);
    }
    return result;
}

DQN_FARRAY_API
T *Dqn_FArray_Add(DQN_FARRAY *array, T const &item)
{
    T *result = nullptr;
    if (!array)
        return result;

    DQN_ASSERT(array->size >= 0);
    if (Dqn_Safe_AssertF(array->size < N, "Array out of space")) {
        array->data[array->size++] = item;
        result = array->data + (array->size - 1);
    }
    return result;
}

DQN_FARRAY_API
T *Dqn_FArray_Make(DQN_FARRAY *array, Dqn_isize count)
{
    T *result = nullptr;
    if (!array || count <= 0)
        return result;

    DQN_ASSERT(array->size >= 0);
    if (Dqn_Safe_AssertF(array->size + count <= N, "Array out of space")) {
        result = array->data + array->size;
        array->size += count;
    }
    return result;
}

DQN_FARRAY_API
void Dqn_FArray_Clear(DQN_FARRAY *array, Dqn_ZeroMem zero_mem)
{
    if (!array)
        return;

    DQN_ASSERT(array->size >= 0);
    array->size = 0;
    if (zero_mem == Dqn_ZeroMem_Yes)
        DQN_MEMSET(array->data, DQN_MEMSET_BYTE, sizeof(T));
}

DQN_FARRAY_API
bool Dqn_FArray_EraseStable(DQN_FARRAY *array, Dqn_isize index)
{
    if (!array)
        return false;

    DQN_ASSERT(array->size >= 0);
    if (index < 0 || index > array->size || array->size <= 0)
        return false;

    T *end  = array->data + array->size;
    T *dest = array->data + index;
    T *src  = DQN_MIN(dest + 1, end);
    DQN_MEMCPY(dest, src, DQN_CAST(size_t)(end - src));
    array->size--;
    return true;
}

DQN_FARRAY_API
bool Dqn_FArray_EraseUnstable(DQN_FARRAY *array, Dqn_isize index)
{
    if (!array)
        return false;

    DQN_ASSERT(array->size >= 0);
    if (!array || index < 0 || index > array->size || array->size <= 0)
        return false;

    array->data[index] = array->data[array->size - 1];
    array->size--;
    return true;
}

DQN_FARRAY_API
bool Dqn_FArray_Pop(DQN_FARRAY *array, Dqn_isize count)
{
    bool result = false;
    if (!array || array->size < count || count <= 0)
        return result;

    if (Dqn_Safe_Assert(array->size >= 0)) {
        array->size -= DQN_MIN(count, array->size);
        result = true;
    }
    return result;
}

DQN_FARRAY_API
T *Dqn_FArray_Peek(DQN_FARRAY *array)
{
    T *result = nullptr;
    if (!array)
        return result;

    if (Dqn_Safe_Assert(array->size >= 0)) {
        result = array->size ? nullptr : array->data + (array->size - 1);
    }
    return result;
}

DQN_FARRAY_API
T Dqn_FArray_PeekCopy(DQN_FARRAY const *array)
{
    T *ptr = Dqn_FArray_Peek(array);
    DQN_HARD_ASSERT(ptr);
    T result = *ptr;
    return result;
}

template <typename T, int N, typename IsEqualProc> DQN_API
T *Dqn_FArray_FindProc(DQN_FARRAY *array, IsEqualProc IsEqual)
{
    if (!array || !IsEqualProc)
        return nullptr;

    for (const T &entry : (*array)) {
        if (IsEqual(entry))
            return &entry;
    }
    return nullptr;
}

// return: True if the entry was found, false if not- the entry is made using Dqn_FArray_Make() in this case
template <typename T, Dqn_isize N, typename IsEqualProc>
DQN_API T *Dqn_FArray_FindProcElseMake(DQN_FARRAY *array, IsEqualProc IsEqual, bool *made)
{
    T *result = Dqn_FArray_FindProc(array, IsEqual);
    if (!result) {
        result = Dqn_FArray_Make(array, 1);
        if (result && made)
            *made = true;
    }
    return result;
}

DQN_FARRAY_API
T *Dqn_FArray_Find(DQN_FARRAY *array, T const *find)
{
    if (!array || !find)
        return nullptr;

    for (const T &entry : (*array)) {
        if (entry == *find)
            return &entry;
    }
    return nullptr;
}
#endif // !defined(DQN_NO_FARRAY)

// NOTE: Dqn_List Template Implementation
// -------------------------------------------------------------------------------------------------
template <typename T>
DQN_API Dqn_List<T> Dqn_List_InitWithArena(Dqn_Arena *arena, Dqn_isize chunk_size)
{
    Dqn_List<T> result = {};
    result.arena       = arena;
    result.chunk_size  = chunk_size;
    return result;
}

template <typename T>
DQN_API T *Dqn_List_Make_(DQN_LEAK_TRACE_FUNCTION Dqn_List<T> *list, Dqn_isize count)
{
    if (list->chunk_size == 0)
        list->chunk_size = 128;

    if (!list->tail || (list->tail->count + count) > list->tail->size)
    {
        auto *tail = (Dqn_ListChunk<T> * )Dqn_Arena_Allocate_(list->arena, sizeof(Dqn_ListChunk<T>), alignof(Dqn_ListChunk<T>), Dqn_ZeroMem_Yes DQN_LEAK_TRACE_ARG);
        if (!tail)
          return nullptr;

        Dqn_isize items = DQN_MAX(list->chunk_size, count);
        tail->data      = (T * )Dqn_Arena_Allocate_(list->arena, sizeof(T) * items, alignof(T), Dqn_ZeroMem_Yes DQN_LEAK_TRACE_ARG);
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
DQN_API T *Dqn_List_Add_(DQN_LEAK_TRACE_FUNCTION Dqn_List<T> *list, T const &value)
{
    T *result = Dqn_List_Make_(list, 1);
    *result = value;
    return result;
}

template <typename T>
DQN_API bool Dqn_List_Iterate(Dqn_List<T> *list, Dqn_ListIterator<T> *it, Dqn_isize start_index)
{
    bool result = false;
    if (!list || !it || start_index < 0 || list->chunk_size <= 0)
        return result;

    if (!it->init) {
        *it = {};
        if (start_index == 0) {
            it->chunk = list->head;
        } else {
            Dqn_List_At(list, start_index, &it->chunk);
            if (list->chunk_size > 0)
                it->chunk_data_index = start_index % list->chunk_size;
        }

        it->init = true;
    }

    if (it->chunk) {
        if (it->chunk_data_index >= it->chunk->count) {
            it->chunk            = it->chunk->next;
            it->chunk_data_index = 0;
        }

        if (it->chunk) {
            it->data = it->chunk->data + it->chunk_data_index++;
            result         = true;
        }
    }

    if (!it->chunk)
        DQN_ASSERT(result == false);
    return result;
}

template <typename T>
DQN_API T *Dqn_List_At(Dqn_List<T> *list, Dqn_isize index, Dqn_ListChunk<T> **at_chunk)
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

// NOTE: Dqn_BinarySearch Template Implementation
// -------------------------------------------------------------------------------------------------
template <typename T>
DQN_FORCE_INLINE bool Dqn_BinarySearch_DefaultLessThan(T const &lhs, T const &rhs)
{
    bool result = lhs < rhs;
    return result;
}

template <typename T>
Dqn_isize
Dqn_BinarySearch(T const                        *array,
                 Dqn_isize                       array_size,
                 T const                        &find,
                 Dqn_BinarySearchType            type,
                 Dqn_BinarySearchLessThanProc<T> less_than)
{
    Dqn_isize result = -1;
    Dqn_isize head   = 0;
    Dqn_isize tail   = array_size - 1;

    if (array && array_size >= 0) {
        while (result == -1 && head <= tail) {
            Dqn_isize mid  = (head + tail) / 2;
            T const &value = array[mid];
            if (less_than(find, value)) {
                tail = mid - 1;
            } else if (less_than(value, find)) {
                head = mid + 1;
            } else {
                result = mid;
            }
        }
    }

    if (type == Dqn_BinarySearchType_OnePastMatch) {
        result = (result == -1) ? tail + 1 : result + 1;
        result = DQN_MAX(result, 0);
    } else {
        DQN_ASSERT(type == Dqn_BinarySearchType_Match);
    }

    return result;
}

#if defined(DQN_COMPILER_W32_MSVC)
    #pragma warning(pop)
#endif
#endif // DQN_H

/// @cond NO_DOXYGEN
// NOTE: Implementation
// -------------------------------------------------------------------------------------------------
#if defined(DQN_IMPLEMENTATION)

#if defined(DQN_OS_WIN32)
// ---------------------------------+-----------------------------+---------------------------------
// [SECT-W32H] Win32 minimal header | DQN_NO_WIN32_MINIMAL_HEADER | Minimal windows.h subset
// ---------------------------------+-----------------------------+---------------------------------
    #pragma comment(lib, "bcrypt")
    #pragma comment(lib, "wininet")

    #if defined(DQN_NO_WIN32_MINIMAL_HEADER) || defined(_INC_WINDOWS)
        #include <bcrypt.h>          // Dqn_OS_SecureRNGBytes -> BCryptOpenAlgorithmProvider ... etc
        #include <shellscalingapi.h> // Dqn_Win_MakeProcessDPIAware -> SetProcessDpiAwareProc
        #if !defined(DQN_NO_WINNET)
            #include <wininet.h> // Dqn_Win_Net -> InternetConnect ... etc
        #endif // DQN_NO_WINNET
    #else
        // Taken from Windows.h
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
        #define MEM_DECOMMIT 0x00004000
        #define MEM_RELEASE 0x00008000

        // NOTE: Protect
        #define PAGE_READWRITE 0x04

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

        // Functions
        // ---------------------------------------------------------------------
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
        }
    #endif // !defined(DQN_NO_WIN32_MINIMAL_HEADER) && !defined(_INC_WINDOWS)
#elif defined(DQN_OS_UNIX)
    #include <errno.h>        // errno
    #include <fcntl.h>        // O_RDONLY ... etc
    #include <linux/fs.h>     // FICLONE
    #include <sys/ioctl.h>    // ioctl
    #include <sys/random.h>   // getrandom
    #include <sys/stat.h>     // stat
    #include <sys/sendfile.h> // sendfile
    #include <sys/mman.h>     // mmap
    #include <time.h>         // clock_gettime, nanosleep
    #include <unistd.h>       // access
#endif

Dqn_Library dqn_library;

// NOTE: Intrinsics
// -------------------------------------------------------------------------------------------------
#if defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
#include <cpuid.h>
#endif

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

// NOTE: Dqn_TicketMutex_
// -------------------------------------------------------------------------------------------------
DQN_API void Dqn_TicketMutex_Begin(Dqn_TicketMutex *mutex)
{
    unsigned int ticket = Dqn_Atomic_AddU32(&mutex->ticket, 1);
    Dqn_TicketMutex_BeginTicket(mutex, ticket);
}

DQN_API void Dqn_TicketMutex_End(Dqn_TicketMutex *mutex)
{
    Dqn_Atomic_AddU32(&mutex->serving, 1);
}

DQN_API Dqn_uint Dqn_TicketMutex_MakeTicket(Dqn_TicketMutex *mutex)
{
    Dqn_uint result = Dqn_Atomic_AddU32(&mutex->ticket, 1);
    return result;
}

DQN_API void Dqn_TicketMutex_BeginTicket(Dqn_TicketMutex const *mutex, Dqn_uint ticket)
{
    DQN_ASSERT_MSG(mutex->serving <= ticket,
                   "Mutex skipped ticket? Was ticket generated by the correct mutex via MakeTicket? ticket = %u, "
                   "mutex->serving = %u",
                   ticket,
                   mutex->serving);
    while (ticket != mutex->serving) {
        // NOTE: Use spinlock intrinsic
        _mm_pause();
    }
}

DQN_API bool Dqn_TicketMutex_CanLock(Dqn_TicketMutex const *mutex, Dqn_uint ticket)
{
    bool result = (ticket == mutex->serving);
    return result;
}

// NOTE: Dqn_Allocator
// -------------------------------------------------------------------------------------------------
DQN_API void *Dqn_Allocator_Alloc_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, size_t size, uint8_t align, Dqn_ZeroMem zero_mem)
{
    void *result = NULL;
    if (allocator.alloc) {
        result = allocator.alloc(DQN_LEAK_TRACE_ARG size, align, zero_mem, allocator.user_context);
    } else {
        result = DQN_ALLOC(size);
        Dqn_Library_LeakTraceAdd(DQN_LEAK_TRACE_ARG result, size);
    }
    return result;
}

DQN_API void Dqn_Allocator_Dealloc_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, void *ptr, size_t size)
{
    if (allocator.dealloc) {
        allocator.dealloc(DQN_LEAK_TRACE_ARG ptr, size, allocator.user_context);
    } else {
        DQN_DEALLOC(ptr, size);
        Dqn_Library_LeakTraceMarkFree(DQN_LEAK_TRACE_ARG ptr);
    }
}

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-LLOG] Dqn_Log              |                             | Library logging
// ---------------------------------+-----------------------------+---------------------------------
DQN_FILE_SCOPE void Dqn_Log_FVDefault_(Dqn_String8 type, int log_type, void *user_data, Dqn_CallSite call_site, char const *fmt, va_list args)
{
    (void)log_type;
    (void)user_data;

    // NOTE: Open log file for appending if requested
    // =========================================================================
    Dqn_TicketMutex_Begin(&dqn_library.log_file_mutex);
    if (dqn_library.log_to_file && !dqn_library.log_file) {
        Dqn_ThreadScratch scratch   = Dqn_Thread_GetScratch(nullptr);
        Dqn_String8        exe_dir  = Dqn_OS_EXEDir(scratch.allocator);
        Dqn_String8        log_file = Dqn_String8_InitF(scratch.allocator, "%.*s/dqn.log", DQN_STRING_FMT(exe_dir));
        fopen_s(DQN_CAST(FILE **)&dqn_library.log_file, log_file.data, "a");
    }
    Dqn_TicketMutex_End(&dqn_library.log_file_mutex);

    // NOTE: Generate the log header
    // =========================================================================
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String8 header        = {};
    {
        Dqn_String8 file_name            = Dqn_String8_FileNameFromPath(call_site.file);
        Dqn_DateHMSTimeString const time = Dqn_Date_HMSLocalTimeStringNow();
        header                           = Dqn_String8_InitF(scratch.allocator, "%.*s %.*s %.*s %.*s %05u %.*s: ",
                                                             time.date_size - 2,
                                                             time.date      + 2,
                                                             time.hms_size,
                                                             time.hms,
                                                             DQN_STRING_FMT(type),
                                                             DQN_STRING_FMT(file_name),
                                                             call_site.line,
                                                             DQN_STRING_FMT(call_site.function));

    }

    // NOTE: Header padding
    // =========================================================================
    Dqn_isize padding_size = 0;
    {
        DQN_LOCAL_PERSIST Dqn_isize max_header_length = 0;
        max_header_length                             = DQN_MAX(max_header_length, header.size);
        padding_size                                  = DQN_MAX(0, max_header_length - header.size);
    }

    // NOTE: Construct final log
    // =========================================================================
    Dqn_String8 msg = Dqn_String8_InitFV(scratch.allocator, fmt, args);

    Dqn_String8 log_line = Dqn_String8_Allocate(scratch.allocator, header.size + padding_size + msg.size, Dqn_ZeroMem_No);
    DQN_MEMCPY(log_line.data,                              header.data, header.size);
    DQN_MEMSET(log_line.data + header.size,                ' ',         padding_size);
    DQN_MEMCPY(log_line.data + header.size + padding_size, msg.data,    msg.size);

    // NOTE: Print log
    // =========================================================================
    Dqn_Print_StdLn(Dqn_PrintStd_Out, log_line);

    Dqn_TicketMutex_Begin(&dqn_library.log_file_mutex);
    if (dqn_library.log_to_file) {
        fprintf(DQN_CAST(FILE *)dqn_library.log_file, "%.*s\n", DQN_STRING_FMT(log_line));
    }
    Dqn_TicketMutex_End(&dqn_library.log_file_mutex);
}

DQN_API void Dqn_Log_FVCallSite(Dqn_String8 type, Dqn_CallSite call_site, char const *fmt, va_list args)
{
    Dqn_LogProc *logging_function = dqn_library.log_callback ? dqn_library.log_callback : Dqn_Log_FVDefault_;
    logging_function(type, -1 /*log_type*/, dqn_library.log_user_data, call_site, fmt, args);
}

DQN_API void Dqn_Log_FCallSite(Dqn_String8 type, Dqn_CallSite call_site, char const  *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Log_FVCallSite(type, call_site, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Log_TypeFVCallSite(Dqn_LogType type, Dqn_CallSite call_site, char const *fmt, va_list args)
{
    Dqn_String8 type_string = DQN_STRING8("DQN-BAD-LOG-TYPE");
    switch (type)
    {
        case Dqn_LogType_Error: type_string = DQN_STRING8("ERROR");
        case Dqn_LogType_Info: type_string = DQN_STRING8("INFO");
        case Dqn_LogType_Warning: type_string = DQN_STRING8("WARN");
        case Dqn_LogType_Debug: type_string = DQN_STRING8("DEBUG");
    }

    Dqn_LogProc *logging_function = dqn_library.log_callback ? dqn_library.log_callback : Dqn_Log_FVDefault_;
    logging_function(type_string, type /*log_type*/, dqn_library.log_user_data, call_site, fmt, args);
}

DQN_API void Dqn_Log_TypeFCallSite(Dqn_LogType type, Dqn_CallSite call_site, char const  *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Log_TypeFVCallSite(type, call_site, fmt, args);
    va_end(args);
}

// NOTE: Dqn_VMem_
// -------------------------------------------------------------------------------------------------
DQN_API void *Dqn_VMem_Reserve(Dqn_usize size, bool commit)
{
    #if defined(DQN_OS_WIN32)
    unsigned long flags = MEM_RESERVE | (commit ? MEM_COMMIT : 0);
    void *result = VirtualAlloc(nullptr, size, flags, PAGE_READWRITE);

    #elif defined(DQN_OS_UNIX)

    unsigned flags = PROT_NONE | (commit ? (PROT_READ | PROT_WRITE) : 0);
    void *result = mmap(nullptr, size, flags, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (result == MAP_FAILED)
        result = nullptr;

    #else
        #error "Missing implementation for Dqn_VMem_Reserve"
    #endif
    return result;
}

DQN_API bool Dqn_VMem_Commit(void *ptr, Dqn_usize size)
{
    #if defined(DQN_OS_WIN32)
    bool result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != nullptr;
    #elif defined(DQN_OS_UNIX)
    bool result = mprotect(ptr, size, PROT_READ|PROT_WRITE) == 0;
    #else
        #error "Missing implementation for Dqn_VMem_Commit"
    #endif
    return result;
}

DQN_API void Dqn_VMem_Decommit(void *ptr, Dqn_usize size)
{
    #if defined(DQN_OS_WIN32)
    VirtualFree(ptr, size, MEM_DECOMMIT);
    #elif defined(DQN_OS_UNIX)
    mprotect(ptr, size, PROT_NONE)
    madvise(ptr, size, MADV_FREE)
    #else
        #error "Missing implementation for Dqn_VMem_Decommit"
    #endif
}

DQN_API void Dqn_VMem_Release(void *ptr, Dqn_usize size)
{
    #if defined(DQN_OS_WIN32)
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
    #elif defined(DQN_OS_UNIX)
    munmap(ptr, size);
    #else
        #error "Missing implementation for Dqn_VMem_Release"
    #endif
}

// NOTE: Dqn_CString8
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_isize Dqn_CString8_FSize(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_isize result = STB_SPRINTF_DECORATE(vsnprintf)(nullptr, 0, fmt, args);
    va_end(args);
    return result;
}

DQN_API Dqn_isize Dqn_CString8_FVSize(char const *fmt, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    Dqn_isize result = STB_SPRINTF_DECORATE(vsnprintf)(nullptr, 0, fmt, args_copy);
    va_end(args_copy);
    return result;
}

static bool Dqn_CString8_Validate_(char const *lhs, Dqn_isize *lhs_size)
{
    if (lhs_size && *lhs_size <= -1)
        *lhs_size = Dqn_CString8_Size(lhs);

    if (!lhs || !lhs_size)
        return false;

    return true;
}

DQN_API char const *Dqn_CString8_BinarySplit(char const *string, Dqn_isize string_size, char delimiter, Dqn_isize *lhs_size, char **rhs, Dqn_isize *rhs_size)
{
    char const *result = string;
    if (lhs_size)
        *lhs_size = 0;
    if (rhs_size)
        *rhs_size = 0;
    if (rhs)
        *rhs = nullptr;

    if (!Dqn_CString8_Validate_(string, &string_size))
        return result;

    Dqn_isize offset = 0;
    while (offset < string_size && string[offset] != delimiter)
        offset++;

    // NOTE: LHS, the string before the delimiter
    result = string;
    if (lhs_size)
        *lhs_size = offset;

    // NOTE: RHS, the string after the delimiter
    if (rhs) {
        char const *end = string + string_size;
        *rhs      = DQN_CAST(char *)DQN_MIN((string + offset + 1), end);
        *rhs_size = end - (*rhs);
    }

    return result;
}

DQN_API bool Dqn_CString8_Eq(char const *lhs, char const *rhs, Dqn_isize lhs_size, Dqn_isize rhs_size, Dqn_CString8EqCase eq_case)
{
    if (!Dqn_CString8_Validate_(lhs, &lhs_size) || !Dqn_CString8_Validate_(rhs, &rhs_size)) {
        return false;
    }

    if (lhs_size != rhs_size)
        return false;

    bool result = lhs_size == rhs_size;
    if (result) {
        if (eq_case == Dqn_CString8EqCase_Sensitive) {
            result = (DQN_MEMCMP(lhs, rhs, DQN_CAST(size_t)lhs_size) == 0);
        } else {
            for (Dqn_isize index = 0; index < lhs_size && result; index++)
                result = (Dqn_Char_ToLower(lhs[index]) == Dqn_Char_ToLower(rhs[index]));
        }
    }

    return result;
}

DQN_API bool Dqn_CString8_EqInsensitive(char const *lhs, char const *rhs, Dqn_isize lhs_size, Dqn_isize rhs_size)
{
    bool result = Dqn_CString8_Eq(lhs, rhs, lhs_size, rhs_size, Dqn_CString8EqCase_Insensitive);
    return result;
}

DQN_API bool Dqn_CString8_StartsWith(char const *string, char const *prefix, Dqn_isize string_size, Dqn_isize prefix_size, Dqn_CString8EqCase eq_case)
{
    if (!Dqn_CString8_Validate_(string, &string_size) || !Dqn_CString8_Validate_(prefix, &prefix_size)) {
        return false;
    }

    bool result = false;
    if (string_size >= prefix_size)
        result = Dqn_CString8_Eq(string /*lhs*/, prefix /*rhs*/, prefix_size /*lhs_size*/, prefix_size /*rhs_size*/, eq_case);

    return result;
}

DQN_API bool Dqn_CString8_StartsWithInsensitive(char const *string, char const *prefix, Dqn_isize string_size, Dqn_isize prefix_size)
{
    bool result = Dqn_CString8_StartsWith(string, prefix, string_size, prefix_size, Dqn_CString8EqCase_Insensitive);
    return result;
}

DQN_API bool Dqn_CString8_EndsWith(char const *string, char const *suffix, Dqn_isize string_size, Dqn_isize suffix_size, Dqn_CString8EqCase eq_case)
{
    if (!Dqn_CString8_Validate_(string, &string_size) || !Dqn_CString8_Validate_(suffix, &suffix_size)) {
        return false;
    }

    bool result = false;
    if (string_size >= suffix_size) {
        char const *string_tail = (string + string_size) - suffix_size;
        result = Dqn_CString8_Eq(string_tail /*lhs*/, suffix /*rhs*/, suffix_size /*lhs_size*/, suffix_size /*rhs_size*/, eq_case);
    }

    return result;
}

DQN_API bool Dqn_CString8_EndsWithInsensitive(char const *string, char const *suffix, Dqn_isize string_size, Dqn_isize suffix_size)
{
    bool result = Dqn_CString8_EndsWith(string, suffix, string_size, suffix_size, Dqn_CString8EqCase_Insensitive);
    return result;
}

DQN_API char const *Dqn_CString8_TrimPrefix(char const *string, char const *prefix, Dqn_isize string_size, Dqn_isize prefix_size, Dqn_CString8EqCase eq_case, Dqn_isize *trimmed_size)
{
    char const *result = string;
    bool starts_with = Dqn_CString8_StartsWith(string, prefix, string_size, prefix_size, eq_case);

    if (starts_with) {
        result = string + prefix_size;
    }

    if (trimmed_size) {
        if (starts_with)
            *trimmed_size = string_size - prefix_size;
        else
            *trimmed_size = string_size;
    }

    return result;
}

DQN_API char const *Dqn_CString8_TrimSuffix(char const *string, char const *suffix, Dqn_isize string_size, Dqn_isize suffix_size, Dqn_CString8EqCase eq_case, Dqn_isize *trimmed_size)
{
    char const *result = string;
    bool ends_with = Dqn_CString8_EndsWith(string, suffix, string_size, suffix_size, eq_case);

    if (trimmed_size) {
        if (ends_with)
            *trimmed_size = string_size - suffix_size;
        else
            *trimmed_size = string_size;
    }

    return result;
}

DQN_API char const *Dqn_CString8_TrimWhitespaceAround(char const *string, Dqn_isize string_size, Dqn_isize *trimmed_size)
{
    char const *result = string;
    if (trimmed_size)
        *trimmed_size = 0;

    if (!Dqn_CString8_Validate_(string, &string_size)) {
        return result;
    }

    char const *start = result;
    char const *end   = start + string_size;

    while (start < end && Dqn_Char_IsWhitespace(start[0]))
        start++;

    while (end > start && Dqn_Char_IsWhitespace(end[-1]))
        end--;

    result                = start;
    Dqn_isize result_size = end - start;
    DQN_ASSERT(result_size >= 0);

    if (trimmed_size)
        *trimmed_size = result_size;

    return result;
}

DQN_API char const *Dqn_CString8_TrimByteOrderMark(char const *string, Dqn_isize string_size, Dqn_isize *trimmed_size)
{
    // TODO(dqn): This is little endian
    char const *result = string;
    if (trimmed_size)
        *trimmed_size = string_size;

    if (!Dqn_CString8_Validate_(string, &string_size))
        return result;

    char const UTF8_BOM[]     = "\xEF\xBB\xBF";
    char const UTF16_BOM_BE[] = "\xEF\xFF";
    char const UTF16_BOM_LE[] = "\xFF\xEF";
    char const UTF32_BOM_BE[] = "\x00\x00\xFE\xFF";
    char const UTF32_BOM_LE[] = "\xFF\xFE\x00\x00";

    Dqn_isize result_size = string_size;
    result = Dqn_CString8_TrimPrefix(result, UTF8_BOM,     result_size, DQN_CHAR_COUNT(UTF8_BOM),     Dqn_CString8EqCase_Sensitive, &result_size);
    result = Dqn_CString8_TrimPrefix(result, UTF16_BOM_BE, result_size, DQN_CHAR_COUNT(UTF16_BOM_BE), Dqn_CString8EqCase_Sensitive, &result_size);
    result = Dqn_CString8_TrimPrefix(result, UTF16_BOM_LE, result_size, DQN_CHAR_COUNT(UTF16_BOM_LE), Dqn_CString8EqCase_Sensitive, &result_size);
    result = Dqn_CString8_TrimPrefix(result, UTF32_BOM_BE, result_size, DQN_CHAR_COUNT(UTF32_BOM_BE), Dqn_CString8EqCase_Sensitive, &result_size);
    result = Dqn_CString8_TrimPrefix(result, UTF32_BOM_LE, result_size, DQN_CHAR_COUNT(UTF32_BOM_LE), Dqn_CString8EqCase_Sensitive, &result_size);

    if (trimmed_size)
        *trimmed_size = result_size;

    return result;
}

DQN_API char const *Dqn_CString8_FileNameFromPath(char const *path, Dqn_isize path_size, Dqn_isize *file_name_size)
{
    char const *result = path;
    if (file_name_size)
        *file_name_size = path_size;

    if (!Dqn_CString8_Validate_(path, &path_size))
        return result;

    Dqn_isize result_size = path_size;
    for (Dqn_isize i = (result_size - 1); i >= 0; --i) {
        if (result[i] == '\\' || result[i] == '/') {
            char const *file_end = result + result_size;
            result               = result + (i + 1);
            result_size          = file_end - result;
            break;
        }
    }

    if (file_name_size)
        *file_name_size = result_size;

    return result;
}

DQN_API bool Dqn_CString8_ToU64Checked(char const *buf, Dqn_isize size, char separator, uint64_t *output)
{
    // NOTE: Argument check
    if (!Dqn_CString8_Validate_(buf, &size)) {
        return false;
    }

    // NOTE: Sanitize input/output
    *output = 0;
    Dqn_isize start_index = Dqn_CString8_TrimWhitespaceAround(buf, size, &size /*new_size*/) - buf;
    DQN_ASSERT(start_index <= size);

    if (size == 0)
        return true;

    // NOTE: Handle prefix '+'
    if (!Dqn_Char_IsDigit(buf[start_index])) {
        if (buf[start_index] == '+') {
            start_index++;
        } else {
            return false;
        }
    }

    // NOTE: Convert the string number to the binary number
    for (Dqn_isize index = start_index; index < size; ++index) {
        char ch = buf[index];
        if (index) {
            if (separator != 0 && ch == separator)
                continue;
        }

        if (ch < '0' || ch > '9')
            return false;

        *output        = Dqn_Safe_MulU64(*output, 10);
        uint64_t digit = ch - '0';
        *output        = Dqn_Safe_AddU64(*output, digit);
    }

    return true;
}

DQN_API uint64_t Dqn_CString8_ToU64(char const *buf, Dqn_isize size, char separator)
{
    uint64_t result = 0;
    Dqn_CString8_ToU64Checked(buf, size, separator, &result);
    return result;
}

DQN_API bool Dqn_CString8_ToI64Checked(char const *buf, Dqn_isize size, char separator, int64_t *output)
{
    // NOTE: Sanitize input/output
    *output = 0;
    Dqn_isize start_index = Dqn_CString8_TrimWhitespaceAround(buf, size, &size /*new_size*/) - buf;
    DQN_ASSERT(start_index <= size);

    if (size == 0)
        return true;

    // NOTE: Handle prefix '-' or '+'
    bool negative = false;
    if (!Dqn_Char_IsDigit(buf[start_index])) {
        negative = (buf[start_index] == '-');
        if (negative || buf[start_index] == '+') {
            start_index++;
        } else {
            return false; // NOTE: Prefix not handled
        }
    }

    // NOTE: Convert the string number to the binary number
    for (Dqn_isize index = start_index; index < size; ++index) {
        char ch = buf[index];
        if (index) {
            if (separator != 0 && ch == separator)
                continue;
        }

        if (ch < '0' || ch > '9')
            return false;

        *output     = Dqn_Safe_MulI64(*output, 10);
        int64_t val = ch - '0';
        *output     = Dqn_Safe_AddI64(*output, val);
    }

    if (negative)
        *output *= -1;

    return true;
}

DQN_API int64_t Dqn_CString8_ToI64(char const *buf, Dqn_isize size, char separator)
{
    int64_t result = 0;
    Dqn_CString8_ToI64Checked(buf, size, separator, &result);
    return result;
}


DQN_API char const *Dqn_CString8_FindMulti(char const *buf, char const *find_list[], Dqn_isize const *find_string_sizes, Dqn_isize find_size, Dqn_isize *match_index, Dqn_isize buf_size)
{
    char const *result = nullptr;
    if (find_size == 0) return result;
    if (buf_size < 0) buf_size = DQN_CAST(Dqn_isize)Dqn_CString8_Size(buf);

    char const *buf_end = buf + buf_size;
    for (; buf != buf_end; ++buf) {
        Dqn_isize remaining = static_cast<Dqn_isize>(buf_end - buf);
        for (Dqn_isize find_index = 0; find_index < find_size; find_index++) {
            char const *find   = find_list[find_index];
            Dqn_isize find_str_size = find_string_sizes[find_index];
            if (remaining < find_str_size) continue;

            if (strncmp(buf, find, DQN_CAST(size_t)find_str_size) == 0) {
                result       = buf;
                *match_index = find_index;
                return result;
            }
        }

    }
    return result;
}

DQN_API char const *Dqn_CString8_Find(char const *buf, char const *find, Dqn_isize buf_size, Dqn_isize find_size, bool case_insensitive)
{
    if (find_size == 0) return nullptr;
    if (buf_size < 0) buf_size = DQN_CAST(Dqn_isize)Dqn_CString8_Size(buf);
    if (find_size < 0) find_size = DQN_CAST(Dqn_isize)Dqn_CString8_Size(find);

    char const *buf_end = buf + buf_size;
    char const *result = nullptr;
    for (; buf != buf_end; ++buf)
    {
        Dqn_isize remaining = static_cast<Dqn_isize>(buf_end - buf);
        if (remaining < find_size) break;

        bool matched = true;
        for (Dqn_isize index = 0; index < find_size; index++)
        {
            char lhs = buf[index];
            char rhs = find[index];

            if (case_insensitive)
            {
                lhs = Dqn_Char_ToLower(lhs);
                rhs = Dqn_Char_ToLower(rhs);
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

DQN_API Dqn_isize Dqn_CString8_Size(char const *src)
{
    Dqn_isize result = 0;
    while (src && src[0] != 0) {
        src++;
        result++;
    }

    return result;
}

DQN_API bool Dqn_CString8_Match(char const *src, char const *find, int find_size)
{
    if (find_size == -1) find_size = Dqn_Safe_SaturateCastUSizeToInt(Dqn_CString8_Size(find));
    bool result = (DQN_MEMCMP(src, find, DQN_CAST(size_t)find_size) == 0);
    return result;
}

DQN_API char const *Dqn_CString8_SkipToChar(char const *src, char ch)
{
    char const *result = src;
    while (result && result[0] && result[0] != ch) ++result;
    return result;
}

DQN_API char const *Dqn_CString8_SkipToNextAlphaNum(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_Char_IsAlphaNum(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_CString8_SkipToNextDigit(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_Char_IsDigit(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_CString8_SkipToNextChar(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_Char_IsAlpha(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_CString8_SkipToNextWord(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_Char_IsWhitespace(result[0])) ++result;
    while (result && result[0] && Dqn_Char_IsWhitespace(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_CString8_SkipToNextWhitespace(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_Char_IsWhitespace(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_CString8_SkipWhitespace(char const *src)
{
    char const *result = src;
    while (result && result[0] && Dqn_Char_IsWhitespace(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_CString8_SkipToCharInPlace(char const **src, char ch)
{
    *src = Dqn_CString8_SkipToChar(*src, ch);
    return *src;
}

DQN_API char const *Dqn_CString8_SkipToNextAlphaNumInPlace(char const **src)
{
    *src = Dqn_CString8_SkipToNextAlphaNum(*src);
    return *src;
}

DQN_API char const *Dqn_CString8_SkipToNextCharInPlace(char const **src)
{
    *src = Dqn_CString8_SkipToNextChar(*src);
    return *src;
}

DQN_API char const *Dqn_CString8_SkipToNextWhitespaceInPlace(char const **src)
{
    *src = Dqn_CString8_SkipToNextWhitespace(*src);
    return *src;
}

DQN_API char const *Dqn_CString8_SkipToNextWordInPlace(char const **src)
{
    *src = Dqn_CString8_SkipToNextWord(*src);
    return *src;
}

DQN_API char const *Dqn_CString8_SkipWhitespaceInPlace(char const **src)
{
    *src = Dqn_CString8_SkipWhitespace(*src);
    return *src;
}

DQN_API bool Dqn_CString8_IsAllDigits(char const *src, Dqn_isize size)
{
    if (!src) return false;
    if (size <= -1) size = Dqn_CString8_Size(src);
    for (Dqn_isize ch_index = 0; ch_index < size; ch_index++) {
        if (!(src[ch_index] >= '0' && src[ch_index] <= '9'))
            return false;
    }
    bool result = src && size > 0;
    return result;
}

DQN_API Dqn_isize Dqn_CString16_Size(wchar_t const *src)
{
    Dqn_isize result = 0;
    while (src && src[0] != 0) {
        src++;
        result++;
    }

    return result;
}


// NOTE: Dqn_String8
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_String8 Dqn_String8_Init(char const *string, Dqn_isize size)
{
    Dqn_String8 result = {};
    result.data = DQN_CAST(char *)string;
    result.size = size;
    return result;
}

DQN_API Dqn_String8 Dqn_String8_InitCString(char const *string)
{
    Dqn_isize size     = Dqn_CString8_Size(string);
    Dqn_String8 result = Dqn_String8_Init(string, size);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_InitF_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Dqn_String8 result = Dqn_String8_InitFV_(DQN_LEAK_TRACE_ARG allocator, fmt, va);
    va_end(va);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_InitFV_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, char const *fmt, va_list args)
{
    Dqn_String8 result = {};
    if (!fmt)
        return result;

    Dqn_isize size = Dqn_CString8_FVSize(fmt, args);
    if (size) {
        result = Dqn_String8_Allocate_(DQN_LEAK_TRACE_ARG allocator, size, Dqn_ZeroMem_No);
        if (Dqn_String8_IsValid(result))
            STB_SPRINTF_DECORATE(vsnprintf)(result.data, Dqn_Safe_SaturateCastISizeToInt(size + 1 /*null-terminator*/), fmt, args);
    }
    return result;
}

DQN_API bool Dqn_String8_IsValid(Dqn_String8 string)
{
    bool result = string.size >= 0 && string.data;
    return result;
}

DQN_API Dqn_String8 Dqn_String8_Slice(Dqn_String8 string, Dqn_isize offset, Dqn_isize size)
{
    Dqn_String8 result = Dqn_String8_Init(string.data, 0);
    if (!Dqn_String8_IsValid(string) || offset < 0 || size < 0)
        return result;

    Dqn_isize capped_offset = DQN_MIN(offset, string.size);
    Dqn_isize max_size      = string.size - capped_offset;
    Dqn_isize capped_size   = DQN_MIN(size, max_size);
    result                  = Dqn_String8_Init(string.data + capped_offset, capped_size);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_Allocate_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, Dqn_isize size, Dqn_ZeroMem zero_mem)
{
    Dqn_String8 result = {};
    result.data       = (char *)Dqn_Allocator_Alloc_(DQN_LEAK_TRACE_ARG allocator, size + 1, alignof(char), zero_mem);
    if (result.data) {
        result.size = size;
    }
    return result;
}

DQN_API Dqn_String8 Dqn_String8_CopyCString_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, char const *string, Dqn_isize size)
{
    Dqn_String8 result = {};
    if (!string || size <= 0)
        return result;

    result = Dqn_String8_Allocate_(DQN_LEAK_TRACE_ARG allocator, size, Dqn_ZeroMem_No);
    if (Dqn_String8_IsValid(result)) {
        DQN_MEMCPY(result.data, string, DQN_CAST(size_t)size);
        result.data[size] = 0;
    }
    return result;
}

DQN_API Dqn_String8 Dqn_String8_Copy_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, Dqn_String8 string)
{
    Dqn_String8 result = Dqn_String8_CopyCString_(DQN_LEAK_TRACE_ARG allocator, string.data, string.size);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_BinarySplit(Dqn_String8 string, char delimiter, Dqn_String8 *rhs)
{
    char **rhs_string   = rhs ? &rhs->data : nullptr;
    Dqn_isize *rhs_size = rhs ? &rhs->size : nullptr;
    Dqn_String8 result  = {};
    result.data         = DQN_CAST(char *)Dqn_CString8_BinarySplit(string.data, string.size, delimiter, &result.size, rhs_string, rhs_size);
    return result;
}

DQN_API bool Dqn_String8_Eq(Dqn_String8 lhs, Dqn_String8 rhs, Dqn_CString8EqCase eq_case)
{
    bool result = Dqn_CString8_Eq(lhs.data, rhs.data, lhs.size, rhs.size, eq_case);
    return result;
}

DQN_API bool Dqn_String8_EqInsensitive(Dqn_String8 lhs, Dqn_String8 rhs)
{
    bool result = Dqn_String8_Eq(lhs, rhs, Dqn_CString8EqCase_Insensitive);
    return result;
}

DQN_API bool Dqn_String8_StartsWith(Dqn_String8 string, Dqn_String8 prefix, Dqn_CString8EqCase eq_case)
{
    bool result = Dqn_CString8_StartsWith(string.data, prefix.data, string.size, prefix.size, eq_case);
    return result;
}

DQN_API bool Dqn_String8_StartsWithInsensitive(Dqn_String8 string, Dqn_String8 prefix)
{
    bool result = Dqn_String8_StartsWith(string, prefix, Dqn_CString8EqCase_Insensitive);
    return result;
}

DQN_API bool Dqn_String8_EndsWith(Dqn_String8 string, Dqn_String8 suffix, Dqn_CString8EqCase eq_case)
{
    bool result = Dqn_CString8_EndsWith(string.data, suffix.data, string.size, suffix.size, eq_case);
    return result;
}

DQN_API bool Dqn_String8_EndsWithInsensitive(Dqn_String8 string, Dqn_String8 suffix)
{
    bool result = Dqn_String8_EndsWith(string, suffix, Dqn_CString8EqCase_Insensitive);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_TrimPrefix(Dqn_String8 string, Dqn_String8 prefix, Dqn_CString8EqCase eq_case)
{
    Dqn_String8 result = {};
    result.data = DQN_CAST(char *)Dqn_CString8_TrimPrefix(string.data, prefix.data, string.size, prefix.size, eq_case, &result.size);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_TrimSuffix(Dqn_String8 string, Dqn_String8 suffix, Dqn_CString8EqCase eq_case)
{
    Dqn_String8 result = {};
    result.data = DQN_CAST(char *)Dqn_CString8_TrimSuffix(string.data, suffix.data, string.size, suffix.size, eq_case, &result.size);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_TrimWhitespaceAround(Dqn_String8 string)
{
    Dqn_String8 result = {};
    result.data = DQN_CAST(char *)Dqn_CString8_TrimWhitespaceAround(string.data, string.size, &result.size);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_TrimByteOrderMark(Dqn_String8 string)
{
    Dqn_String8 result = {};
    result.data = DQN_CAST(char *)Dqn_CString8_TrimByteOrderMark(string.data, string.size, &result.size);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_FileNameFromPath(Dqn_String8 path)
{
    Dqn_String8 result = {};
    result.data        = DQN_CAST(char *)Dqn_CString8_FileNameFromPath(path.data, path.size, &result.size);
    return result;
}

DQN_API bool Dqn_String8_ToU64Checked(Dqn_String8 string, char separator, uint64_t *output)
{
    bool result = Dqn_CString8_ToU64Checked(string.data, string.size, separator, output);
    return result;
}

DQN_API uint64_t Dqn_String8_ToU64(Dqn_String8 string, char separator)
{
    uint64_t result = Dqn_CString8_ToU64(string.data, string.size, separator);
    return result;
}

DQN_API bool Dqn_String8_ToI64Checked(Dqn_String8 string, char separator, int64_t *output)
{
    bool result = Dqn_CString8_ToI64Checked(string.data, string.size, separator, output);
    return result;
}

DQN_API int64_t Dqn_String8_ToI64(Dqn_String8 string, char separator)
{
    int64_t result = Dqn_CString8_ToI64(string.data, string.size, separator);
    return result;
}


DQN_API Dqn_isize Dqn_String8_Split(Dqn_String8 string, Dqn_String8 delimiter, Dqn_String8 *splits, Dqn_isize splits_count)
{
    Dqn_isize result = 0; // The number of splits in the actual string.
    if (!Dqn_String8_IsValid(string) || !Dqn_String8_IsValid(delimiter) || delimiter.size <= 0)
        return result;

    Dqn_isize splits_index = 0; // The number of splits written.
    Dqn_isize begin        = 0;
    for (Dqn_isize index = 0; index < string.size; ) {
        // NOTE: Check if we encountered the substring that is the delimiter
        Dqn_String8 check = Dqn_String8_Slice(string, index, delimiter.size);
        if (!Dqn_String8_Eq(check, delimiter)) {
            index++;
            continue;
        }

        // NOTE: Generate the split
        Dqn_String8 split = Dqn_String8_Init(string.data + begin, index - begin);
        if (splits && splits_index < splits_count && split.size)
            splits[splits_index++] = split;

        // NOTE: Advance the iterators
        result += (split.size > 0);
        index  += delimiter.size;
        begin   = index;

        // NOTE: Exit early if space in string is less than the delimiter size
        Dqn_isize space = string.size - index;
        if (space < delimiter.size)
            break;
    }

    return result;
}

DQN_API bool Dqn_String8_IsAllDigits(Dqn_String8 string)
{
    bool result = Dqn_String8_IsValid(string) && string.size > 0;
    for (Dqn_isize index = 0; result && index < string.size; index++)
        result = string.data[index] >= '0' && string.data[index] <= '9';
    return result;
}

DQN_API bool Dqn_String8_IsAllHex(Dqn_String8 string)
{
    Dqn_String8 trimmed = Dqn_String8_TrimPrefix(string, DQN_STRING8("0x"), Dqn_CString8EqCase_Insensitive);
    bool result        = Dqn_String8_IsValid(trimmed) && trimmed.size > 0;
    for (Dqn_isize index = 0; result && index < trimmed.size; index++) {
        char ch = trimmed.data[index];
        result  = (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
    }
    return result;
}

DQN_API bool Dqn_String8_HasChar(Dqn_String8 string, char ch)
{
    bool result = false;
    for (Dqn_isize index = 0; !result && index < string.size; index++) {
        result = string.data[index] == ch;
    }
    return result;
}

DQN_API void Dqn_String8_Remove(Dqn_String8 *string, Dqn_isize begin, Dqn_isize size)
{
    if (!string ||
        !Dqn_String8_IsValid(*string) ||
        (begin < 0 || begin >= string->size) ||
        (size <= 0 || size >= string->size))
    {
        return;
    }

    char *dest      = string->data + begin;
    char const *src = string->data + (begin + size);
    char const *end = string->data + string->size;
    DQN_MEMMOVE(dest, src, end - src);
    string->size -= size;
}

DQN_API Dqn_isize Dqn_String8_FindOffset(Dqn_String8 string, Dqn_String8 find, Dqn_isize start_index, Dqn_CString8EqCase eq_case)
{
    Dqn_isize result  = -1;
    if (!Dqn_String8_IsValid(string) || !Dqn_String8_IsValid(find) || start_index < 0)
        return -1;

    Dqn_isize max = string.size - find.size;
    for (Dqn_isize index = start_index; result == -1 && index <= max; index++) {
        Dqn_String8 check = Dqn_String8_Slice(string, index, find.size);
        if (Dqn_String8_Eq(check, find, eq_case))
            result = index;
    }

    return result;
}

DQN_API Dqn_String8 Dqn_String8_Find(Dqn_String8 string, Dqn_String8 find, Dqn_isize start_index, Dqn_CString8EqCase eq_case)
{
    Dqn_isize offset  = Dqn_String8_FindOffset(string, find, start_index, eq_case);
    Dqn_String8 result = Dqn_String8_Slice(string, offset == -1 ? 0 : offset, offset == -1 ? 0 : find.size);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_Replace(Dqn_String8 string,
                                       Dqn_String8 find,
                                       Dqn_String8 replace,
                                       Dqn_isize start_index,
                                       Dqn_Arena *arena,
                                       Dqn_Arena *temp_arena,
                                       Dqn_CString8EqCase eq_case)
{
    auto temp_arena_scope             = Dqn_ArenaTempMemoryScope(temp_arena);
    Dqn_String8Builder string_builder = {};
    string_builder.allocator          = Dqn_Arena_Allocator(temp_arena);
    Dqn_isize max                     = string.size - find.size;
    Dqn_isize head                    = start_index;

    for (Dqn_isize tail = head; tail <= max; tail++) {
        Dqn_String8 check = Dqn_String8_Slice(string, tail, find.size);
        if (!Dqn_String8_Eq(check, find, eq_case))
            continue;

        if (start_index > 0 && string_builder.string_size == 0) {
            // User provided a hint in the string to start searching from, we
            // need to add the string up to the hint. We only do this if there's
            // a replacement action, otherwise we have a special case for no
            // replacements, where the entire string gets copied.
            Dqn_String8 slice = Dqn_String8_Init(string.data, head);
            Dqn_String8Builder_AppendString8Ref(&string_builder, slice);
        }

        Dqn_String8 range = Dqn_String8_Slice(string, head, (tail - head));
        Dqn_String8Builder_AppendString8Ref(&string_builder, range);
        Dqn_String8Builder_AppendString8Ref(&string_builder, replace);
        head = tail + find.size;
        tail += find.size - 1; // NOTE: -1 since the for loop will post increment us past the end of the find string
    }

    Dqn_String8 result = {};
    if (string_builder.string_size == 0) {
        // NOTE: No replacement possible, so we just do a full-copy
        result = Dqn_String8_Copy(Dqn_Arena_Allocator(arena), string);
    } else {
        Dqn_String8 remainder = Dqn_String8_Init(string.data + head, string.size - head);
        Dqn_String8Builder_AppendString8Ref(&string_builder, remainder);
        result = Dqn_String8Builder_Build(&string_builder, Dqn_Arena_Allocator(arena));
    }

    return result;
}

DQN_API Dqn_String8 Dqn_String8_ReplaceInsensitive(Dqn_String8 string, Dqn_String8 find, Dqn_String8 replace, Dqn_isize start_index, Dqn_Arena *arena, Dqn_Arena *temp_arena)
{
    Dqn_String8 result = Dqn_String8_Replace(string, find, replace, start_index, arena, temp_arena, Dqn_CString8EqCase_Insensitive);
    return result;
}

#if defined(__cplusplus)
bool operator==(Dqn_String8 const &lhs, Dqn_String8 const &rhs)
{
    bool result = Dqn_String8_Eq(lhs, rhs, Dqn_CString8EqCase_Sensitive);
    return result;
}

bool operator!=(Dqn_String8 const &lhs, Dqn_String8 const &rhs)
{
    bool result = !(lhs == rhs);
    return result;
}
#endif

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-PRIN] Dqn_Print            |                             | Printing
// ---------------------------------+-----------------------------+---------------------------------
DQN_API void Dqn_Print_Std(Dqn_PrintStd std_handle, Dqn_String8 string)
{
    DQN_ASSERT(std_handle == Dqn_PrintStd_Out || std_handle == Dqn_PrintStd_Err);

    #if defined(DQN_OS_WIN32)
    // NOTE: Get the output handles from kernel
    // =========================================================================
    thread_local void *std_out_print_handle     = nullptr;
    thread_local void *std_err_print_handle     = nullptr;
    thread_local bool  std_out_print_to_console = false;
    thread_local bool  std_err_print_to_console = false;

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
    fprintf(std_handle == Dqn_PrintStd_Out ? stdout : stderr, "%.*s", DQN_STRING8_FMT(string));
    #endif
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

DQN_API void Dqn_Print_StdFV(Dqn_PrintStd std_handle, char const *fmt, va_list args)
{
    char buffer[STB_SPRINTF_MIN];
    stbsp_vsprintfcb(Dqn_Print_VSPrintfChunker_, DQN_CAST(void *)DQN_CAST(uintptr_t)std_handle, buffer, fmt, args);
}

DQN_API void Dqn_Print_StdF(Dqn_PrintStd std_handle, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdFV(std_handle, fmt, args);
    va_end(args);
}

DQN_API void Dqn_Print_StdLn(Dqn_PrintStd std_handle, Dqn_String8 string)
{
    Dqn_Print_Std(std_handle, string);
    Dqn_Print_Std(std_handle, DQN_STRING8("\n"));
}

DQN_API void Dqn_Print_StdLnFV(Dqn_PrintStd std_handle, char const *fmt, va_list args)
{
    Dqn_Print_StdFV(std_handle, fmt, args);
    Dqn_Print_Std(std_handle, DQN_STRING8("\n"));
}

DQN_API void Dqn_Print_StdLnF(Dqn_PrintStd std_handle, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Print_StdLnFV(std_handle, fmt, args);
    va_end(args);
}

// NOTE: Dqn_String8Builder Implementation
// -----------------------------------------------------------------------------
bool Dqn_String8Builder_AppendString8Ref(Dqn_String8Builder *builder, Dqn_String8 string)
{
    if (!builder || !string.data || string.size <= 0)
        return false;

    Dqn_String8Link *link = Dqn_Allocator_New(builder->allocator, Dqn_String8Link, Dqn_ZeroMem_No);
    if (!link)
        return false;

    link->string = string;
    link->next   = NULL;

    if (builder->head)
        builder->tail->next = link;
    else
        builder->head = link;

    builder->tail = link;
    builder->count++;
    builder->string_size += string.size;
    return true;
}

bool Dqn_String8Builder_AppendString8Copy(Dqn_String8Builder *builder, Dqn_String8 string)
{
    Dqn_String8 copy = Dqn_String8_Copy(builder->allocator, string);
    bool result      = Dqn_String8Builder_AppendString8Ref(builder, copy);
    return result;
}

bool Dqn_String8Builder_AppendFV_(DQN_LEAK_TRACE_FUNCTION Dqn_String8Builder *builder, char const *fmt, va_list args)
{
    Dqn_String8 string = Dqn_String8_InitFV(builder->allocator, fmt, args);
    if (string.size == 0)
        return true;

    bool result = Dqn_String8Builder_AppendString8Ref(builder, string);
    if (!result)
        Dqn_Allocator_Dealloc_(DQN_LEAK_TRACE_ARG builder->allocator, string.data, string.size + 1);
    return result;
}

bool Dqn_String8Builder_AppendF(Dqn_String8Builder *builder, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool result = Dqn_String8Builder_AppendFV(builder, fmt, args);
    va_end(args);
    return result;
}

Dqn_String8 Dqn_String8Builder_Build(Dqn_String8Builder const *builder, Dqn_Allocator allocator)
{
    Dqn_String8 result = DQN_ZERO_INIT;
    if (!builder || builder->string_size <= 0 || builder->count <= 0)
        return result;

    result.data = Dqn_Allocator_NewArray(allocator, char, builder->string_size + 1, Dqn_ZeroMem_No);
    if (!result.data)
        return result;

    for (Dqn_String8Link *link = builder->head; link; link = link->next) {
        DQN_MEMCPY(result.data + result.size, link->string.data, link->string.size);
        result.size += link->string.size;
    }

    result.data[result.size] = 0;
    DQN_ASSERT(result.size == builder->string_size);
    return result;
}

// NOTE: Dqn_Arena
// -------------------------------------------------------------------------------------------------
DQN_API bool Dqn_Arena_Grow_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, Dqn_isize size, Dqn_isize commit)
{
    // TODO: Use the call site arguments
    if (!arena || size <= 0 || commit < 0)
        return false;

    if (!Dqn_Safe_AssertF(commit <= size,
                          "Commit must be less than or equal to the size requested [size=%zd, commit=%zd]",
                          size, commit)) {
        commit = size;
    }

    // NOTE: If the commit amount is the same as the size, the caller has
    // requested all the memory is committed. We can save one sys-call by asking
    // the OS to reserve+commit in one call.
    bool commit_on_reserve     = size == commit;
    auto const allocation_size = DQN_CAST(Dqn_isize)(sizeof(*arena->curr) + size);
    auto *result               = DQN_CAST(Dqn_ArenaBlock *)Dqn_VMem_Reserve(allocation_size, commit_on_reserve);
    if (result) {
        // NOTE: If we didn't commit on reserve, commit the amount requested by
        // the user.
        if (!commit_on_reserve) {
            Dqn_VMem_Commit(result, sizeof(*result) + commit);
        }

        // NOTE: Sanity check memory is zero-ed out
        DQN_ASSERT(result->used == 0);
        DQN_ASSERT(result->next == nullptr);
        DQN_ASSERT(result->prev == nullptr);

        // NOTE: Setup the block
        result->size      = size;
        result->committed = commit;
        result->memory    = DQN_CAST(uint8_t *)result + sizeof(*result);

        // NOTE: Attach the block to the arena
        if (arena->tail) {
            arena->tail->next = result;
            result->prev      = arena->tail;
        } else {
            DQN_ASSERT(!arena->curr);
            arena->curr = result;
        }
        arena->tail = result;

        // NOTE: Update stats
        arena->stats.syscalls += (commit_on_reserve ? 1 : 2);
        arena->stats.blocks   += 1;
        arena->stats.capacity += arena->curr->size;

        arena->stats.blocks_hwm   = DQN_MAX(arena->stats.blocks_hwm,   arena->stats.blocks);
        arena->stats.capacity_hwm = DQN_MAX(arena->stats.capacity_hwm, arena->stats.capacity);

        Dqn_Library_LeakTraceAdd(DQN_LEAK_TRACE_ARG result, size);
    }

    return result;
}

DQN_API void *Dqn_Arena_Allocate_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, Dqn_isize size, uint8_t align, Dqn_ZeroMem zero_mem)
{
    DQN_ASSERT_MSG((align & (align - 1)) == 0, "Power of two alignment required");
    Dqn_isize allocation_size = size + (align - 1);
    while (!arena->curr || (arena->curr->used + allocation_size) > arena->curr->size) {
        if (arena->curr && arena->curr->next) {
            arena->curr = arena->curr->next;
        } else {
            Dqn_isize grow_size = DQN_MAX(DQN_MAX(allocation_size, arena->min_block_size), DQN_ARENA_MIN_BLOCK_SIZE);
            if (!Dqn_Arena_Grow_(DQN_LEAK_TRACE_ARG arena, grow_size /*size*/, grow_size /*commit*/)) {
                return nullptr;
            }
        }
    }

    // NOTE: Calculate an aligned allocation pointer
    Dqn_ArenaBlock *block        = arena->curr;
    uintptr_t const address      = DQN_CAST(uintptr_t)block->memory + block->used;
    Dqn_isize const align_offset = (align - (address & (align - 1))) & (align - 1);
    void *result                 = DQN_CAST(char *)(address + align_offset);

    DQN_ASSERT_MSG(block->committed >= block->used,
                   "Internal error: Committed size must always be greater than the used size [commit=%_$$zd, used=%_$$zd]",
                   block->committed, block->used);

    // NOTE: Ensure that the allocation is backed by physical pages from the OS
    Dqn_isize const commit_space = block->committed - block->used;
    if (commit_space < allocation_size) {
        Dqn_isize commit_size = allocation_size - commit_space;
        Dqn_VMem_Commit(DQN_CAST(char *)result + commit_space, commit_size);
        block->committed += commit_size;
    }

    if (zero_mem == Dqn_ZeroMem_Yes) {
        // NOTE: Newly committed pages are always 0-ed out, we only need to
        // memset the memory that was reused from the arena.
        DQN_MEMSET(DQN_CAST(char *)result - align_offset,
                   DQN_MEMSET_BYTE,
                   align_offset + commit_space);
    }

    // NOTE: Update arena
    arena->stats.used += allocation_size;
    arena->stats.used_hwm = DQN_MAX(arena->stats.used_hwm, arena->stats.used);
    block->used += allocation_size;

    DQN_ASSERT_MSG(size + align_offset <= allocation_size, "Internal error: Alignment size calculation error [size=%_$$zd, offset=%zd, allocation_size=%_$$zd]", size, align_offset, allocation_size);
    DQN_ASSERT_MSG(block->used <= block->committed, "Internal error: Committed size must be greater than used size [used=%_$$zd, committed=%_$$zd]", block->used, block->committed);
    DQN_ASSERT_MSG(block->committed <= block->size, "Internal error: Allocation exceeded block capacity [committed=%_$$zd, size=%_$$zd]", block->committed, block->size);
    DQN_ASSERT_MSG(((DQN_CAST(uintptr_t)result) & (align - 1)) == 0, "Internal error: Pointer alignment failed [address=%p, align=%x]", result, align);

    return result;
}

DQN_API void *Dqn_Arena_Copy_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, void *src, Dqn_isize size, uint8_t alignment)
{
    void *result = Dqn_Arena_Allocate_(DQN_LEAK_TRACE_ARG arena, size, alignment, Dqn_ZeroMem_No);
    DQN_MEMCPY(result, src, size);
    return result;
}

DQN_API void *Dqn_Arena_CopyZ_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, void *src, Dqn_isize size, uint8_t alignment)
{
    void *result = Dqn_Arena_Allocate_(DQN_LEAK_TRACE_ARG arena, size + 1, alignment, Dqn_ZeroMem_No);
    DQN_MEMCPY(result, src, size);
    (DQN_CAST(char *)result)[size] = 0;
    return result;
}

DQN_API void Dqn_Arena_Free_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, Dqn_ZeroMem zero_mem)
{
    if (!arena)
        return;

    while (arena->tail) {
        Dqn_ArenaBlock *tail = arena->tail;
        arena->tail          = tail->prev;
        if (zero_mem == Dqn_ZeroMem_Yes)
            DQN_MEMSET(tail->memory, DQN_MEMSET_BYTE, tail->committed);
        Dqn_VMem_Release(tail, sizeof(*tail) + tail->size);
        Dqn_Library_LeakTraceMarkFree(DQN_LEAK_TRACE_ARG tail);
    }

    arena->curr           = arena->tail = nullptr;
    arena->stats.capacity = 0;
    arena->stats.used     = 0;
    arena->stats.wasted   = 0;
    arena->stats.blocks   = 0;
}

DQN_API void Dqn_Arena_Reset(Dqn_Arena *arena, Dqn_ZeroMem zero_mem)
{
    if (!arena)
        return;

    // NOTE: Zero all the blocks until we reach the first block in the list
    for (Dqn_ArenaBlock *block = arena->tail; block; block = block->prev) {
        if (!block->prev)
            arena->curr = block;
        if (zero_mem == Dqn_ZeroMem_Yes)
            DQN_MEMSET(block->memory, DQN_MEMSET_BYTE, block->committed);
        block->used = 0;
    }

    arena->stats.used   = 0;
    arena->stats.wasted = 0;
}

DQN_FILE_SCOPE void *Dqn_Arena_AllocatorAlloc(DQN_LEAK_TRACE_FUNCTION size_t size, uint8_t align, Dqn_ZeroMem zero_mem, void *user_context)
{
    void *result = NULL;
    if (!user_context)
        return result;

    Dqn_Arena *arena = DQN_CAST(Dqn_Arena *)user_context;
    result           = Dqn_Arena_Allocate_(DQN_LEAK_TRACE_ARG arena, size, align, zero_mem);
    return result;
}

DQN_FILE_SCOPE void Dqn_Arena_AllocatorDealloc(DQN_LEAK_TRACE_FUNCTION void *ptr, size_t size, void *user_context)
{
    // NOTE: No-op, arenas batch allocate and batch deallocate. Call free on the
    // underlying arena, since we can't free individual pointers.
    DQN_LEAK_TRACE_UNUSED;
    (void)ptr;
    (void)size;
    (void)user_context;
}

DQN_API Dqn_Allocator Dqn_Arena_Allocator(Dqn_Arena *arena)
{
    Dqn_Allocator result = {};
    if (arena) {
        result.user_context = arena;
        result.alloc        = Dqn_Arena_AllocatorAlloc;
        result.dealloc      = Dqn_Arena_AllocatorDealloc;
    }
    return result;
}

DQN_API Dqn_ArenaTempMemory Dqn_Arena_BeginTempMemory(Dqn_Arena *arena)
{
    Dqn_ArenaTempMemory result = {};
    result.arena              = arena;
    result.curr               = arena->curr;
    result.tail               = arena->tail;
    result.curr_used          = (arena->curr) ? arena->curr->used : 0;
    result.stats              = arena->stats;
    return result;
}

DQN_API void Dqn_Arena_EndTempMemory_(DQN_LEAK_TRACE_FUNCTION Dqn_ArenaTempMemory scope)
{
    if (!scope.arena)
        return;

    // NOTE: Revert arena stats
    Dqn_Arena *arena      = scope.arena;
    arena->stats.capacity = scope.stats.capacity;
    arena->stats.used     = scope.stats.used;
    arena->stats.wasted   = scope.stats.wasted;
    arena->stats.blocks   = scope.stats.blocks;

    // NOTE: Revert the current block to the scope's current block
    arena->curr          = scope.curr;
    arena->curr->used    = scope.curr_used;

    // NOTE: Free the tail blocks until we reach the scope's tail block
    while (arena->tail != scope.tail) {
        Dqn_ArenaBlock *tail = arena->tail;
        arena->tail          = tail->prev;
        Dqn_VMem_Release(tail, sizeof(*tail) + tail->size);
        Dqn_Library_LeakTraceMarkFree(DQN_LEAK_TRACE_ARG tail);
    }

    // NOTE: Reset the usage of all the blocks between the tail and current block's
    if (arena->tail) {
        arena->tail->next = nullptr;
        for (Dqn_ArenaBlock *block = arena->tail; block && block != arena->curr; block = block->prev)
            block->used = 0;
    }
}

Dqn_ArenaTempMemoryScope_::Dqn_ArenaTempMemoryScope_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena)
{
    temp_memory = Dqn_Arena_BeginTempMemory(arena);
    #if defined(DQN_LEAK_TRACKING)
    leak_site__ = DQN_LEAK_TRACE_ARG_NO_COMMA;
    #endif
}

Dqn_ArenaTempMemoryScope_::~Dqn_ArenaTempMemoryScope_()
{
    #if defined(DQN_LEAK_TRACING)
    Dqn_Arena_EndTempMemory_(leak_site__, temp_memory);
    #else
    Dqn_Arena_EndTempMemory_(temp_memory);
    #endif
}

DQN_API Dqn_ArenaStatString Dqn_Arena_StatString(Dqn_ArenaStat const *stat)
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

    Dqn_ArenaStatString result = {};
    int size16 = STB_SPRINTF_DECORATE(snprintf)(result.data, DQN_ARRAY_ICOUNT(result.data),
                                                "ArenaStat{"
                                                    "used/hwm=%_$$zd/%_$$zd, "
                                                    "cap/hwm=%_$$zd/%_$$zd, "
                                                    "wasted/hwm=%_$$zd/%_$$zd, "
                                                    "blocks/hwm=%_$$u/%_$$u, "
                                                    "syscalls=%'zd"
                                                "}",
                                                stat->used,     stat->used_hwm,
                                                stat->capacity, stat->capacity_hwm,
                                                stat->wasted,   stat->wasted_hwm,
                                                stat->blocks,   stat->blocks_hwm,
                                                stat->syscalls);
    result.size = Dqn_Safe_SaturateCastIntToU16(size16);

    #if defined(DQN_COMPILER_GCC)
        #pragma GCC diagnostic pop
    #elif defined(DQN_COMPILER_W32_CLANG)
        #pragma GCC diagnostic pop
    #endif

    return result;
}

DQN_API void Dqn_Arena_LogStats(Dqn_Arena const *arena)
{
    Dqn_ArenaStatString string = Dqn_Arena_StatString(&arena->stats);
    Dqn_Log_InfoF("%.*s\n", DQN_STRING_FMT(string));
}

#if !defined(DQN_NO_MATH)
// ---------------------------------+-----------------------------+---------------------------------
// [SECT-MATH] Math                 | DQN_NO_MATH                 | v2i, V2, V3, V4, Mat4, Rect, RectI32, Lerp
// ---------------------------------+-----------------------------+---------------------------------
DQN_API Dqn_V2I Dqn_V2ToV2I(Dqn_V2 a)
{
    Dqn_V2I result = Dqn_V2I(DQN_CAST(int32_t)a.x, DQN_CAST(int32_t)a.y);
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

// NOTE: Dqn_V4 Implementation
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_f32 Dqn_V4Dot(Dqn_V4 a, Dqn_V4 b)
{
    Dqn_f32 result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
    return result;
}

// NOTE: Dqn_M4 Implementation
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_M4 Dqn_M4_Identity()
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

DQN_API Dqn_M4 Dqn_M4_ScaleF(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z)
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

DQN_API Dqn_M4 Dqn_M4_Scale(Dqn_V3 xyz)
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

DQN_API Dqn_M4 Dqn_M4_TranslateF(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z)
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

DQN_API Dqn_M4 Dqn_M4_Translate(Dqn_V3 xyz)
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

DQN_API Dqn_M4 Dqn_M4_Transpose(Dqn_M4 mat)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
            result.columns[col][row] = mat.columns[row][col];
    return result;
}

DQN_API Dqn_M4 Dqn_M4_Rotate(Dqn_V3 axis01, Dqn_f32 radians)
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

DQN_API Dqn_M4 Dqn_M4_Orthographic(Dqn_f32 left, Dqn_f32 right, Dqn_f32 bottom, Dqn_f32 top, Dqn_f32 z_near, Dqn_f32 z_far)
{
    // NOTE: Here is the matrix in column major for readability. Below it's
    // transposed due to how you have to declare column major matrices in C/C++.
    //
    // m = [2/r-l, 0,      0,     -1*(r+l)/(r-l)]
    //     [0,     2/t-b,  0,      1*(t+b)/(t-b)]
    //     [0,     0,     -2/f-n, -1*(f+n)/(f-n)]
    //     [0,     0,      0,      1            ]

    Dqn_M4 result =
    {{
         {2.f / (right - left),                     0.f,                                      0.f,                                          0.f},
         {0.f,                                      2.f / (top - bottom),                     0.f,                                          0.f},
         {0.f,                                      0.f,                                      -2.f / (z_far - z_near),                      0.f},
         {(-1.f * (right + left)) / (right - left), (-1.f * (top + bottom)) / (top - bottom), (-1.f * (z_far + z_near)) / (z_far - z_near), 1.f},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4_Perspective(Dqn_f32 fov /*radians*/, Dqn_f32 aspect, Dqn_f32 z_near, Dqn_f32 z_far)
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

DQN_API Dqn_M4 Dqn_M4_Add(Dqn_M4 lhs, Dqn_M4 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] + rhs.columns[col][it];
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_Sub(Dqn_M4 lhs, Dqn_M4 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] - rhs.columns[col][it];
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_Mul(Dqn_M4 lhs, Dqn_M4 rhs)
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

DQN_API Dqn_M4 Dqn_M4_Div(Dqn_M4 lhs, Dqn_M4 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] / rhs.columns[col][it];
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_AddF(Dqn_M4 lhs, Dqn_f32 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] + rhs;
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_SubF(Dqn_M4 lhs, Dqn_f32 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] - rhs;
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_MulF(Dqn_M4 lhs, Dqn_f32 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] * rhs;
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_DivF(Dqn_M4 lhs, Dqn_f32 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] / rhs;
    }
    return result;
}

#if !defined(DQN_NO_FSTRING8)
DQN_API Dqn_FString8<256> Dqn_M4_ColumnMajorString(Dqn_M4 mat)
{
    Dqn_FString8<256> result = {};
    for (int row = 0; row < 4; row++)
    {
        for (int it = 0; it < 4; it++)
        {
            if (it == 0) Dqn_FString8_AppendCString8(&result, "|");
            Dqn_FString8_AppendF(&result, "%.5f", mat.columns[it][row]);
            if (it != 3) Dqn_FString8_AppendCString8(&result, ", ");
            else         Dqn_FString8_AppendCString8(&result, "|\n");
        }
    }

    return result;
}
#endif

// NOTE: Dqn_Rect
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_Rect Dqn_Rect_InitFromPosAndSize(Dqn_V2 pos, Dqn_V2 size)
{
    Dqn_Rect result = {};
    result.min      = pos;
    if (size.x < 0) result.min.x -= size.x;
    if (size.y < 0) result.min.y -= size.y;
    result.max  = result.min + Dqn_V2Abs(size);
    return result;
}

DQN_API Dqn_V2 Dqn_Rect_Center(Dqn_Rect rect)
{
    Dqn_V2 size   = rect.max - rect.min;
    Dqn_V2 result = rect.min + (size * 0.5f);
    return result;
}

DQN_API bool Dqn_Rect_ContainsPoint(Dqn_Rect rect, Dqn_V2 p)
{
    bool result = (p.x >= rect.min.x && p.x <= rect.max.x && p.y >= rect.min.y && p.y <= rect.max.y);
    return result;
}

DQN_API bool Dqn_Rect_ContainsRect(Dqn_Rect a, Dqn_Rect b)
{
    bool result = (b.min >= a.min && b.max <= a.max);
    return result;
}

DQN_API Dqn_V2 Dqn_Rect_Size(Dqn_Rect rect)
{
    Dqn_V2 result = rect.max - rect.min;
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_Move(Dqn_Rect src, Dqn_V2 move_amount)
{
    Dqn_Rect result = src;
    result.min += move_amount;
    result.max += move_amount;
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_MoveTo(Dqn_Rect src, Dqn_V2 dest)
{
    Dqn_V2 move_amount = dest - src.min;
    Dqn_Rect result    = src;
    result.min += move_amount;
    result.max += move_amount;
    return result;
}

DQN_API bool Dqn_Rect_Intersects(Dqn_Rect a, Dqn_Rect b)
{
    bool result = (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
                     (a.min.y <= b.max.y && a.max.y >= b.min.y);
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_Intersection(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_Rect result  = {};
    if (Dqn_Rect_Intersects(a, b))
    {
        result.min.x = DQN_MAX(a.min.x, b.min.x);
        result.min.y = DQN_MAX(a.min.y, b.min.y);
        result.max.x = DQN_MIN(a.max.x, b.max.x);
        result.max.y = DQN_MIN(a.max.y, b.max.y);
    }

    return result;
}

DQN_API Dqn_Rect Dqn_Rect_Union(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_Rect result  = {};
    result.min.x = DQN_MIN(a.min.x, b.min.x);
    result.min.y = DQN_MIN(a.min.y, b.min.y);
    result.max.x = DQN_MAX(a.max.x, b.max.x);
    result.max.y = DQN_MAX(a.max.y, b.max.y);
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_FromRectI32(Dqn_RectI32 a)
{
    Dqn_Rect result = Dqn_Rect(a.min, a.max);
    return result;
}

DQN_API Dqn_V2I Dqn_RectI32_Size(Dqn_RectI32 rect)
{
    Dqn_V2I result = rect.max - rect.min;
    return result;
}

// NOTE: Math Utils
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_V2 Dqn_Lerp_V2(Dqn_V2 a, Dqn_f32 t, Dqn_V2 b)
{
    Dqn_V2 result = {};
    result.x  = a.x + ((b.x - a.x) * t);
    result.y  = a.y + ((b.y - a.y) * t);
    return result;
}

DQN_API Dqn_f32 Dqn_Lerp_F32(Dqn_f32 a, Dqn_f32 t, Dqn_f32 b)
{
    Dqn_f32 result = a + ((b - a) * t);
    return result;
}
#endif // !defined(DQN_NO_MATH)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-BITS] Dqn_Bit              |                             | Bitset manipulation
// ---------------------------------+-----------------------------+---------------------------------
DQN_API void Dqn_Bit_UnsetInplace(uint64_t *flags, uint64_t bitfield)
{
    *flags = (*flags & ~bitfield);
}

DQN_API void Dqn_Bit_SetInplace(uint64_t *flags, uint64_t bitfield)
{
    *flags = (*flags | bitfield);
}

DQN_API bool Dqn_Bit_IsSet(uint64_t bits, uint64_t bits_to_set)
{
    auto result = DQN_CAST(bool)((bits & bits_to_set) == bits_to_set);
    return result;
}

DQN_API bool Dqn_Bit_IsNotSet(uint64_t bits, uint64_t bits_to_check)
{
    auto result = !Dqn_Bit_IsSet(bits, bits_to_check);
    return result;
}

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-SAFE] Dqn_Safe             |                             | Safe arithmetic, casts, asserts
// ---------------------------------+-----------------------------+---------------------------------
DQN_API bool Dqn_Safe_AssertF_(bool assertion_expr, Dqn_CallSite call_site, char const *fmt, ...)
{
    bool result = assertion_expr;
    if (!result) {
        va_list args;
        va_start(args, fmt);
        Dqn_Log_TypeFVCallSite(Dqn_LogType_Error, call_site, fmt, args);
        va_end(args);
        DQN_DEBUG_BREAK;
    }
    return result;
}

DQN_API int64_t Dqn_Safe_AddI64(int64_t a, int64_t b)
{
    int64_t result = Dqn_Safe_AssertF(a <= INT64_MAX - b, "a=%zd, b=%zd", a, b) ? (a + b) : INT64_MAX;
    return result;
}

DQN_API int64_t Dqn_Safe_MulI64(int64_t a, int64_t b)
{
    int64_t result = Dqn_Safe_AssertF(a <= INT64_MAX / b, "a=%zd, b=%zd", a, b) ? (a * b) : INT64_MAX;
    return result;
}

DQN_API uint64_t Dqn_Safe_AddU64(uint64_t a, uint64_t b)
{
    uint64_t result = Dqn_Safe_AssertF(a <= UINT64_MAX - b, "a=%zu, b=%zu", a, b) ? (a + b) : UINT64_MAX;
    return result;
}

DQN_API uint64_t Dqn_Safe_SubU64(uint64_t a, uint64_t b)
{
    uint64_t result = Dqn_Safe_AssertF(a >= b, "a=%zu, b=%zu", a, b) ? (a - b) : 0;
    return result;
}

DQN_API uint64_t Dqn_Safe_MulU64(uint64_t a, uint64_t b)
{
    uint64_t result = Dqn_Safe_AssertF(a <= UINT64_MAX / b, "a=%zu, b=%zu", a, b) ? (a * b) : UINT64_MAX;
    return result;
}

DQN_API uint32_t Dqn_Safe_SubU32(uint32_t a, uint32_t b)
{
    uint32_t result = Dqn_Safe_AssertF(a >= b, "a=%u, b=%u", a, b) ? (a - b) : 0;
    return result;
}

// NOTE: Dqn_Safe_SaturateCastUSizeToI*
// -----------------------------------------------------------------------------
// INT*_MAX literals will be promoted to the type of uintmax_t as uintmax_t is
// the highest possible rank (unsigned > signed).
DQN_API int Dqn_Safe_SaturateCastUSizeToInt(Dqn_usize val)
{
    int result = Dqn_Safe_Assert(DQN_CAST(uintmax_t)val <= INT_MAX) ? DQN_CAST(int)val : INT_MAX;
    return result;
}

DQN_API int8_t Dqn_Safe_SaturateCastUSizeToI8(Dqn_usize val)
{
    int8_t result = Dqn_Safe_Assert(DQN_CAST(uintmax_t)val <= INT8_MAX) ? DQN_CAST(int8_t)val : INT8_MAX;
    return result;
}

DQN_API int16_t Dqn_Safe_SaturateCastUSizeToI16(Dqn_usize val)
{
    int16_t result = Dqn_Safe_Assert(DQN_CAST(uintmax_t)val <= INT16_MAX) ? DQN_CAST(int16_t)val : INT16_MAX;
    return result;
}

DQN_API int32_t Dqn_Safe_SaturateCastUSizeToI32(Dqn_usize val)
{
    int32_t result = Dqn_Safe_Assert(DQN_CAST(uintmax_t)val <= INT32_MAX) ? DQN_CAST(int32_t)val : INT32_MAX;
    return result;
}

DQN_API int64_t Dqn_Safe_SaturateCastUSizeToI64(Dqn_usize val)
{
    int64_t result = Dqn_Safe_Assert(DQN_CAST(uintmax_t)val <= INT64_MAX) ? DQN_CAST(int64_t)val : INT64_MAX;
    return result;
}

// NOTE: Dqn_Safe_SaturateCastUSizeToU*
// -----------------------------------------------------------------------------
// Both operands are unsigned and the lowest rank operand will be promoted to
// match the highest rank operand.
DQN_API uint8_t Dqn_Safe_SaturateCastUSizeToU8(Dqn_usize val)
{
    uint8_t result = Dqn_Safe_Assert(val <= UINT8_MAX) ? DQN_CAST(uint8_t)val : UINT8_MAX;
    return result;
}

DQN_API uint16_t Dqn_Safe_SaturateCastUSizeToU16(Dqn_usize val)
{
    uint16_t result = Dqn_Safe_Assert(val <= UINT16_MAX) ? DQN_CAST(uint16_t)val : UINT16_MAX;
    return result;
}

DQN_API uint32_t Dqn_Safe_SaturateCastUSizeToU32(Dqn_usize val)
{
    uint32_t result = Dqn_Safe_Assert(val <= UINT32_MAX) ? DQN_CAST(uint32_t)val : UINT32_MAX;
    return result;
}

DQN_API uint64_t Dqn_Safe_SaturateCastUSizeToU64(Dqn_usize val)
{
    uint64_t result = Dqn_Safe_Assert(val <= UINT64_MAX) ? DQN_CAST(uint64_t)val : UINT64_MAX;
    return result;
}

// NOTE: Dqn_Safe_SaturateCastU64ToU*
// -----------------------------------------------------------------------------
// Both operands are unsigned and the lowest rank operand will be promoted to
// match the highest rank operand.
DQN_API unsigned int Dqn_Safe_SaturateCastU64ToUInt(uint64_t val)
{
    unsigned int result = Dqn_Safe_Assert(val <= UINT8_MAX) ? DQN_CAST(unsigned int)val : UINT_MAX;
    return result;
}

DQN_API uint8_t Dqn_Safe_SaturateCastU64ToU8(uint64_t val)
{
    uint8_t result = Dqn_Safe_Assert(val <= UINT8_MAX) ? DQN_CAST(uint8_t)val : UINT8_MAX;
    return result;
}

DQN_API uint16_t Dqn_Safe_SaturateCastU64ToU16(uint64_t val)
{
    uint16_t result = Dqn_Safe_Assert(val <= UINT16_MAX) ? DQN_CAST(uint16_t)val : UINT16_MAX;
    return result;
}

DQN_API uint32_t Dqn_Safe_SaturateCastU64ToU32(uint64_t val)
{
    uint32_t result = Dqn_Safe_Assert(val <= UINT32_MAX) ? DQN_CAST(uint32_t)val : UINT32_MAX;
    return result;
}


// NOTE: Dqn_Safe_SaturateCastISizeToI*
// -----------------------------------------------------------------------------
// Both operands are signed so the lowest rank operand will be promoted to
// match the highest rank operand.
DQN_API int Dqn_Safe_SaturateCastISizeToInt(Dqn_isize val)
{
    Dqn_Safe_Assert(val >= INT_MIN && val <= INT_MAX);
    int result = DQN_CAST(int)DQN_CLAMP(val, INT_MIN, INT_MAX);
    return result;
}

DQN_API int8_t Dqn_Safe_SaturateCastISizeToI8(Dqn_isize val)
{
    Dqn_Safe_Assert(val >= INT8_MIN && val <= INT8_MAX);
    int8_t result = DQN_CAST(int8_t)DQN_CLAMP(val, INT8_MIN, INT8_MAX);
    return result;
}

DQN_API int16_t Dqn_Safe_SaturateCastISizeToI16(Dqn_isize val)
{
    Dqn_Safe_Assert(val >= INT16_MIN && val <= INT16_MAX);
    int16_t result = DQN_CAST(int16_t)DQN_CLAMP(val, INT16_MIN, INT16_MAX);
    return result;
}

DQN_API int32_t Dqn_Safe_SaturateCastISizeToI32(Dqn_isize val)
{
    Dqn_Safe_Assert(val >= INT32_MIN && val <= INT32_MAX);
    int32_t result = DQN_CAST(int32_t)DQN_CLAMP(val, INT32_MIN, INT32_MAX);
    return result;
}

DQN_API int64_t Dqn_Safe_SaturateCastISizeToI64(Dqn_isize val)
{
    Dqn_Safe_Assert(val >= INT64_MIN && val <= INT64_MAX);
    int64_t result = DQN_CAST(int64_t)DQN_CLAMP(val, INT64_MIN, INT64_MAX);
    return result;
}

// NOTE: Dqn_Safe_SaturateCastISizeToU*
// -----------------------------------------------------------------------------
// If the value is a negative integer, we clamp to 0. Otherwise, we know that
// the value is >=0, we can upcast safely to bounds check against the maximum
// allowed value.
DQN_API unsigned int Dqn_Safe_SaturateCastISizeToUInt(Dqn_isize val)
{
    unsigned int result = 0;
    if (Dqn_Safe_Assert(val >= DQN_CAST(Dqn_isize)0)) {
        if (Dqn_Safe_Assert(DQN_CAST(uintmax_t)val <= UINT_MAX))
            result = DQN_CAST(unsigned int)val;
        else
            result = UINT_MAX;
    }
    return result;
}

DQN_API uint8_t Dqn_Safe_SaturateCastISizeToU8(Dqn_isize val)
{
    uint8_t result = 0;
    if (Dqn_Safe_Assert(val >= DQN_CAST(Dqn_isize)0)) {
        if (Dqn_Safe_Assert(DQN_CAST(uintmax_t)val <= UINT8_MAX))
            result = DQN_CAST(uint8_t)val;
        else
            result = UINT8_MAX;
    }
    return result;
}

DQN_API uint16_t Dqn_Safe_SaturateCastISizeToU16(Dqn_isize val)
{
    uint16_t result = 0;
    if (Dqn_Safe_Assert(val >= DQN_CAST(Dqn_isize)0)) {
        if (Dqn_Safe_Assert(DQN_CAST(uintmax_t)val <= UINT16_MAX))
            result = DQN_CAST(uint16_t)val;
        else
            result = UINT16_MAX;
    }
    return result;
}

DQN_API uint32_t Dqn_Safe_SaturateCastISizeToU32(Dqn_isize val)
{
    uint32_t result = 0;
    if (Dqn_Safe_Assert(val >= DQN_CAST(Dqn_isize)0)) {
        if (Dqn_Safe_Assert(DQN_CAST(uintmax_t)val <= UINT32_MAX))
            result = DQN_CAST(uint32_t)val;
        else
            result = UINT32_MAX;
    }
    return result;
}

DQN_API uint64_t Dqn_Safe_SaturateCastISizeToU64(Dqn_isize val)
{
    uint64_t result = 0;
    if (Dqn_Safe_Assert(val >= DQN_CAST(Dqn_isize)0)) {
        if (Dqn_Safe_Assert(DQN_CAST(uintmax_t)val <= UINT64_MAX))
            result = DQN_CAST(uint64_t)val;
        else
            result = UINT64_MAX;
    }
    return result;
}

// NOTE: Dqn_Safe_SaturateCastI64To*
// -----------------------------------------------------------------------------
// Both operands are signed so the lowest rank operand will be promoted to
// match the highest rank operand.
DQN_API Dqn_isize Dqn_Safe_SaturateCastI64ToISize(int64_t val)
{
    Dqn_Safe_Assert(val >= DQN_ISIZE_MIN && val <= DQN_ISIZE_MAX);
    Dqn_isize result = DQN_CAST(int64_t)DQN_CLAMP(val, DQN_ISIZE_MIN, DQN_ISIZE_MAX);
    return result;
}

DQN_API int8_t Dqn_Safe_SaturateCastI64ToI8(int64_t val)
{
    Dqn_Safe_Assert(val >= INT8_MIN && val <= INT8_MAX);
    int8_t result = DQN_CAST(int8_t)DQN_CLAMP(val, INT8_MIN, INT8_MAX);
    return result;
}

DQN_API int16_t Dqn_Safe_SaturateCastI64ToI16(int64_t val)
{
    Dqn_Safe_Assert(val >= INT16_MIN && val <= INT16_MAX);
    int16_t result = DQN_CAST(int16_t)DQN_CLAMP(val, INT16_MIN, INT16_MAX);
    return result;
}

DQN_API int32_t Dqn_Safe_SaturateCastI64ToI32(int64_t val)
{
    Dqn_Safe_Assert(val >= INT32_MIN && val <= INT32_MAX);
    int32_t result = DQN_CAST(int32_t)DQN_CLAMP(val, INT32_MIN, INT32_MAX);
    return result;
}

// NOTE: Dqn_Safe_SaturateCastIntTo*
// -----------------------------------------------------------------------------
DQN_API int8_t Dqn_Safe_SaturateCastIntToI8(int val)
{
    Dqn_Safe_Assert(val >= INT8_MIN && val <= INT8_MAX);
    int8_t result = DQN_CAST(int8_t)DQN_CLAMP(val, INT8_MIN, INT8_MAX);
    return result;
}

DQN_API int16_t Dqn_Safe_SaturateCastIntToI16(int val)
{
    Dqn_Safe_Assert(val >= INT16_MIN && val <= INT16_MAX);
    int16_t result = DQN_CAST(int16_t)DQN_CLAMP(val, INT16_MIN, INT16_MAX);
    return result;
}

DQN_API uint8_t Dqn_Safe_SaturateCastIntToU8(int val)
{
    uint8_t result = 0;
    if (Dqn_Safe_Assert(val >= DQN_CAST(Dqn_isize)0)) {
        if (Dqn_Safe_Assert(DQN_CAST(uintmax_t)val <= UINT8_MAX))
            result = DQN_CAST(uint8_t)val;
        else
            result = UINT8_MAX;
    }
    return result;
}

DQN_API uint16_t Dqn_Safe_SaturateCastIntToU16(int val)
{
    uint16_t result = 0;
    if (Dqn_Safe_Assert(val >= DQN_CAST(Dqn_isize)0)) {
        if (Dqn_Safe_Assert(DQN_CAST(uintmax_t)val <= UINT16_MAX))
            result = DQN_CAST(uint16_t)val;
        else
            result = UINT16_MAX;
    }
    return result;
}

DQN_API uint32_t Dqn_Safe_SaturateCastIntToU32(int val)
{
    static_assert(sizeof(val) <= sizeof(uint32_t), "Sanity check to allow simplifying of casting");
    uint32_t result = 0;
    if (Dqn_Safe_Assert(val >= 0))
        result = DQN_CAST(uint32_t)val;
    return result;
}

DQN_API uint64_t Dqn_Safe_SaturateCastIntToU64(int val)
{
    static_assert(sizeof(val) <= sizeof(uint64_t), "Sanity check to allow simplifying of casting");
    uint64_t result = 0;
    if (Dqn_Safe_Assert(val >= 0))
        result = DQN_CAST(uint64_t)val;
    return result;
}

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-CHAR] Dqn_Char             |                             | Character ascii/digit.. helpers
// ---------------------------------+-----------------------------+---------------------------------
DQN_API bool Dqn_Char_IsAlpha(char ch)
{
    bool result = (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
    return result;
}

DQN_API bool Dqn_Char_IsDigit(char ch)
{
    bool result = (ch >= '0' && ch <= '9');
    return result;
}

DQN_API bool Dqn_Char_IsAlphaNum(char ch)
{
    bool result = Dqn_Char_IsAlpha(ch) || Dqn_Char_IsDigit(ch);
    return result;
}

DQN_API bool Dqn_Char_IsWhitespace(char ch)
{
    bool result = (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
    return result;
}

DQN_API bool Dqn_Char_IsHex(char ch)
{
    bool result = ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') || (ch >= '0' && ch <= '9'));
    return result;
}

DQN_API uint8_t Dqn_Char_HexToU8(char ch)
{
    DQN_ASSERT_MSG(Dqn_Char_IsHex(ch), "Hex character not valid '%c'", ch);

    uint8_t result = 0;
    if (ch >= 'a' && ch <= 'f')
        result = ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F')
        result = ch - 'A' + 10;
    else
        result = ch - '0';
    return result;
}

char constexpr DQN_HEX_LUT[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
DQN_API char Dqn_Char_ToHex(char ch)
{
    char result = DQN_CAST(char)-1;
    if (ch <= 16) result = DQN_HEX_LUT[DQN_CAST(unsigned)ch];
    return result;
}

DQN_API char Dqn_Char_ToHexUnchecked(char ch)
{
    char result = DQN_HEX_LUT[DQN_CAST(unsigned)ch];
    return result;
}

DQN_API char Dqn_Char_ToLower(char ch)
{
    char result = ch;
    if (result >= 'A' && result <= 'Z')
        result += 'a' - 'A';
    return result;
}

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-UTFX] Dqn_UTF              |                             | Unicode helpers
// ---------------------------------+-----------------------------+---------------------------------
DQN_API int Dqn_UTF8_EncodeCodepoint(uint8_t utf8[4], uint32_t codepoint)
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
        utf8[0] = DQN_CAST(uint8_t)codepoint;
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

DQN_API int Dqn_UTF16_EncodeCodepoint(uint16_t utf16[2], uint32_t codepoint)
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
        utf16[0] = DQN_CAST(uint16_t)codepoint;
        return 1;
    }

    if (codepoint <= 0b1111'1111'1111'1111'1111)
    {
        uint32_t surrogate_codepoint = codepoint + 0x10000;
        utf16[0] = 0b1101'1000'0000'0000 | ((surrogate_codepoint >> 10) & 0b11'1111'1111); // x
        utf16[1] = 0b1101'1100'0000'0000 | ((surrogate_codepoint >>  0) & 0b11'1111'1111); // y
        return 2;
    }

    return 0;
}

#if !defined(DQN_NO_HEX)
// ---------------------------------+-----------------------------+---------------------------------
// [SECT-BHEX] Dqn_Hex              | DQN_NO_HEX                  | Binary <-> hex helpers
// ---------------------------------+-----------------------------+---------------------------------
DQN_API char const *Dqn_Hex_TrimSpaceAnd0xPrefixCString8(char const *hex, Dqn_isize size, Dqn_isize *real_size)
{
    Dqn_isize   trim_size = 0;
    char const *result    = Dqn_CString8_TrimWhitespaceAround(hex, size, &trim_size);
    if (trim_size >= 2) {
        if (result[0] == '0' && (result[1] == 'x' || result[1] == 'X')) {
            trim_size -= 2;
            result += 2;
        }
    }

    if (real_size)
        *real_size = trim_size;
    return result;
}

DQN_API Dqn_String8 Dqn_Hex_TrimSpaceAnd0xPrefixString8(Dqn_String8 const string)
{
    Dqn_isize trimmed_size = 0;
    char const *trimmed    = Dqn_Hex_TrimSpaceAnd0xPrefixCString8(string.data, string.size, &trimmed_size);
    Dqn_String8 result     = Dqn_String8_Init(trimmed, trimmed_size);
    return result;
}

DQN_API Dqn_HexNumberString Dqn_Hex_U64ToNumberString(uint64_t number, uint32_t flags)
{
    Dqn_String8 prefix = {};
    if (!(flags & Dqn_HexNumberStringFlags_No0xPrefix))
        prefix = DQN_STRING8("0x");

    Dqn_HexNumberString result = {};
    DQN_MEMCPY(result.data, prefix.data, prefix.size);
    result.size += DQN_CAST(int8_t)prefix.size;

    char const *fmt = (flags & Dqn_HexNumberStringFlags_UppercaseHex) ? "%I64X" : "%I64x";
    int size        = STB_SPRINTF_DECORATE(snprintf)(result.data + result.size, DQN_ARRAY_UCOUNT(result.data) - result.size, fmt, number);
    result.size    += Dqn_Safe_SaturateCastIntToI8(size);
    DQN_ASSERT(result.size < DQN_ARRAY_UCOUNT(result.data));

    // NOTE: snprintf returns the required size of the format string
    // irrespective of if there's space or not, but, always null terminates so
    // the last byte is wasted.
    result.size = DQN_MIN(result.size, DQN_ARRAY_UCOUNT(result.data) - 1);
    return result;
}

DQN_API Dqn_String8 Dqn_Hex_U64ToString8(Dqn_Allocator allocator, uint64_t number, uint32_t flags)
{
    Dqn_String8 prefix = {};
    if (!(flags & Dqn_HexNumberStringFlags_No0xPrefix))
        prefix = DQN_STRING8("0x");

    char const *fmt         = (flags & Dqn_HexNumberStringFlags_UppercaseHex) ? "%I64X" : "%I64x";
    Dqn_isize required_size = Dqn_CString8_FSize(fmt, number) + prefix.size;
    Dqn_String8 result      = Dqn_String8_Allocate(allocator, required_size, Dqn_ZeroMem_No);

    if (Dqn_String8_IsValid(result)) {
        DQN_MEMCPY(result.data, prefix.data, prefix.size);
        int space = DQN_CAST(int)DQN_MAX((result.size - prefix.size) + 1, 0); /*null-terminator*/
        STB_SPRINTF_DECORATE(snprintf)(result.data + prefix.size, space, fmt, number);
    }
    return result;
}

DQN_API uint64_t Dqn_Hex_CString8ToU64(char const *hex, Dqn_isize size)
{
    Dqn_isize   trim_size = size;
    char const *trim_hex  = hex;
    if (trim_size >= 2) {
        if (trim_hex[0] == '0' && (trim_hex[1] == 'x' || trim_hex[1] == 'X')) {
            trim_size -= 2;
            trim_hex += 2;
        }
    }

    DQN_ASSERT(DQN_CAST(Dqn_usize)(trim_size * 4 / 8) /*maximum amount of bytes represented in the hex string*/ <= sizeof(uint64_t));

    uint64_t   result      = 0;
    Dqn_usize bits_written = 0;
    Dqn_isize max_size     = DQN_MIN(size, 8 /*bytes*/ * 2 /*hex chars per byte*/);
    for (Dqn_isize hex_index = 0; hex_index < max_size; hex_index++, bits_written += 4) {
        char ch = trim_hex[hex_index];
        if (!Dqn_Char_IsHex(ch))
            break;
        uint8_t val = Dqn_Char_HexToU8(ch);
        Dqn_usize bit_shift = 60 - bits_written;
        result |= (DQN_CAST(uint64_t)val << bit_shift);
    }

    result >>= (64 - bits_written); // Shift the remainder digits to the end.
    return result;
}

DQN_API uint64_t Dqn_Hex_String8ToU64(Dqn_String8 hex)
{
    uint64_t result = Dqn_Hex_CString8ToU64(hex.data, hex.size);
    return result;
}

DQN_API bool Dqn_Hex_BytesToCString8Buffer(void const *src, Dqn_isize src_size, char *dest, Dqn_isize dest_size)
{
    if (!src || !dest || src_size < 0)
        return false;

    if (!Dqn_Safe_Assert(dest_size >= src_size * 2))
        return false;

    char const *HEX             = "0123456789abcdef";
    unsigned char const *src_u8 = DQN_CAST(unsigned char const *)src;
    for (Dqn_isize src_index = 0, dest_index = 0; src_index < src_size; src_index++) {
        char byte          = src_u8[src_index];
        char hex01         = (byte >> 4) & 0b1111;
        char hex02         = (byte >> 0) & 0b1111;
        dest[dest_index++] = HEX[(int)hex01];
        dest[dest_index++] = HEX[(int)hex02];
    }

    return true;
}

DQN_API char *Dqn_Hex_BytesToCString8Arena(Dqn_Arena *arena, void const *bytes, Dqn_isize size)
{
    char *result = size > 0 ? Dqn_Arena_NewArray(arena, char, (size * 2) + 1 /*null terminate*/, Dqn_ZeroMem_No) : nullptr;
    if (result) {
        bool converted = Dqn_Hex_BytesToCString8Buffer(bytes, size, result, size * 2);
        DQN_ASSERT(converted);
        result[size * 2] = 0;
    }
    return result;
}

DQN_API Dqn_String8 Dqn_Hex_BytesToString8Arena(Dqn_Arena *arena, void const *bytes, Dqn_isize size)
{
    Dqn_String8 result = {};
    result.data        = Dqn_Hex_BytesToCString8Arena(arena, bytes, size);
    if (result.data)
        result.size = size * 2;
    return result;
}

DQN_API Dqn_isize Dqn_Hex_CString8ToByteBuffer(char const *hex, Dqn_isize hex_size, void *dest, Dqn_isize dest_size)
{
    Dqn_isize result = 0;
    if (!hex || hex_size <= 0)
        return result;

    Dqn_isize trim_size  = 0;
    char const *trim_hex = Dqn_Hex_TrimSpaceAnd0xPrefixCString8(hex,
                                                                hex_size,
                                                                &trim_size);
    if (dest_size < (trim_size / 2))
        return result;

    result = Dqn_Hex_CString8ToByteBufferUnchecked(trim_hex,
                                                   trim_size,
                                                   dest,
                                                   dest_size);
    return result;
}

DQN_API Dqn_isize Dqn_Hex_CString8ToByteBufferUnchecked(char const *hex, Dqn_isize hex_size, void *dest, Dqn_isize dest_size)
{
    Dqn_isize result       = 0;
    unsigned char *dest_u8 = DQN_CAST(unsigned char *)dest;

    for (Dqn_isize hex_index = 0;
         hex_index < hex_size;
         hex_index += 2, result += 1)
    {
        char hex01   = hex[hex_index];
        char hex02   = (hex_index + 1 < hex_size) ? hex[hex_index + 1] : 0;

        char bit4_01 =   (hex01 >= '0' && hex01 <= '9') ?  0 + (hex01 - '0')
                       : (hex01 >= 'a' && hex01 <= 'f') ? 10 + (hex01 - 'a')
                       : (hex01 >= 'A' && hex01 <= 'F') ? 10 + (hex01 - 'A')
                       : 0;

        char bit4_02 =   (hex02 >= '0' && hex02 <= '9') ?  0 + (hex02 - '0')
                       : (hex02 >= 'a' && hex02 <= 'f') ? 10 + (hex02 - 'a')
                       : (hex02 >= 'A' && hex02 <= 'F') ? 10 + (hex02 - 'A')
                       : 0;

        char byte       = (bit4_01 << 4) | (bit4_02 << 0);
        dest_u8[result] = byte;
    }

    DQN_ASSERT(result <= dest_size);
    return result;
}

DQN_API Dqn_isize Dqn_Hex_String8ToByteBufferUnchecked(Dqn_String8 hex, void *dest, Dqn_isize dest_size)
{
    Dqn_isize result = Dqn_Hex_CString8ToByteBufferUnchecked(hex.data, hex.size, dest, dest_size);
    return result;
}

DQN_API Dqn_isize Dqn_Hex_String8ToByteBuffer(Dqn_String8 hex, void *dest, Dqn_isize dest_size)
{
    Dqn_isize result = Dqn_Hex_CString8ToByteBuffer(hex.data, hex.size, dest, dest_size);
    return result;
}

DQN_API char *Dqn_Hex_CString8ToBytesArena(Dqn_Arena *arena, char const *hex, Dqn_isize size, Dqn_isize *real_size)
{
    char *result = nullptr;
    if (!arena || !hex || size <= 0)
        return result;

    Dqn_isize trim_size  = 0;
    char const *trim_hex = Dqn_Hex_TrimSpaceAnd0xPrefixCString8(hex,
                                                                size,
                                                                &trim_size);

    Dqn_isize binary_size = trim_size / 2;
    result                = Dqn_Arena_NewArray(arena, char, binary_size, Dqn_ZeroMem_No);
    if (result) {
        Dqn_isize convert_size = Dqn_Hex_CString8ToByteBufferUnchecked(trim_hex, trim_size, result, binary_size);
        if (real_size)
            *real_size = convert_size;
    }
    return result;
}

DQN_API Dqn_String8 Dqn_Hex_String8ToBytesArena(Dqn_Arena *arena, Dqn_String8 hex)
{
    Dqn_String8 result = {};
    result.data        = Dqn_Hex_CString8ToBytesArena(arena, hex.data, hex.size, &result.size);
    return result;
}
#endif // !defined(DQN_NO_HEX)

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-DATE] Dqn_Date             |                             | Date-time helpers
// ---------------------------------+-----------------------------+---------------------------------
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
                                                                      DQN_CAST(int) Dqn_CArray_CountI(result.hms),
                                                                      "%02d%c%02d%c%02d",
                                                                      time.hour,
                                                                      hms_separator,
                                                                      time.minutes,
                                                                      hms_separator,
                                                                      time.seconds);

    result.date_size = DQN_CAST(uint8_t) STB_SPRINTF_DECORATE(snprintf)(result.date,
                                                                       DQN_CAST(int) Dqn_CArray_CountI(result.date),
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

// NOTE: Max size from MSDN, using \\? syntax, but the ? bit can be expanded
// even more so the max size is kind of not well defined.
#if defined(DQN_OS_WIN32) && !defined(DQN_OS_WIN32_MAX_PATH)
    #define DQN_OS_WIN32_MAX_PATH 32767 + 128 /*fudge*/
#endif

#if defined(DQN_OS_WIN32)
// ---------------------------------+-----------------------------+---------------------------------
// [SECT-WIND] Dqn_Win              |                             | Windows OS helpers
// ---------------------------------+-----------------------------+---------------------------------
DQN_API void Dqn_Win_LastErrorToBuffer(Dqn_WinErrorMsg *msg)
{
    msg->code    = GetLastError();
    msg->data[0] = 0;

    unsigned long flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    void *module_to_get_errors_from = nullptr;

    if (msg->code >= 12000 && msg->code <= 12175) { // WinINET Errors
        flags |= FORMAT_MESSAGE_FROM_HMODULE;
        module_to_get_errors_from = GetModuleHandleA("wininet.dll");
    }

    msg->size = FormatMessageA(flags,
                                 module_to_get_errors_from,   // LPCVOID lpSource,
                                 msg->code,                   // unsigned long   dwMessageId,
                                 0,                           // unsigned long   dwLanguageId,
                                 msg->data,                   // LPSTR   lpBuffer,
                                 DQN_ARRAY_ICOUNT(msg->data), // unsigned long   nSize,
                                 nullptr);                    // va_list * Arguments

    if (msg->size >= 2 &&
        (msg->data[msg->size - 2] == '\r' && msg->data[msg->size - 1] == '\n')) {
        msg->size -= 2;
    }
}

DQN_API Dqn_WinErrorMsg Dqn_Win_LastError()
{
    Dqn_WinErrorMsg result = {};
    Dqn_Win_LastErrorToBuffer(&result);
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
    if (auto *set_process_dpi_awareness_context = DQN_CAST(SetProcessDpiAwarenessContextProc *)GetProcAddress(DQN_CAST(HMODULE)lib_handle, "SetProcessDpiAwarenessContext")) {
        set_process_dpi_awareness_context(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    } else if (auto *set_process_dpi_awareness = DQN_CAST(SetProcessDpiAwarenessProc *)GetProcAddress(DQN_CAST(HMODULE)lib_handle, "SetProcessDpiAwareness")) {
        set_process_dpi_awareness(PROCESS_PER_MONITOR_DPI_AWARE);
    } else if (auto *set_process_dpi_aware = DQN_CAST(SetProcessDpiAwareProc *)GetProcAddress(DQN_CAST(HMODULE)lib_handle, "SetProcessDpiAware")) {
        set_process_dpi_aware();
    }
}

DQN_API void Dqn_Win_DumpLastError_(Dqn_CallSite call_site, char const *fmt, ...)
{
    // TODO(dqn): Hmmm .. should this be a separate log or part of the above
    // macro. If so we need to make the logging macros more flexible.
    Dqn_WinErrorMsg msg = Dqn_Win_LastError();
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        Dqn_Log_TypeFVCallSite(Dqn_LogType_Error, call_site, fmt, args);
        va_end(args);
    }

    if (msg.size)
        Dqn_Log_TypeFCallSite(Dqn_LogType_Error, call_site, "Last Windows error [msg=%.*s]", DQN_STRING_FMT(msg));
    else
        Dqn_Log_TypeFCallSite(Dqn_LogType_Error, call_site, "Failed to dump last error, no error message found [format_message_error=%d, msg_error=%d]", GetLastError(), msg.code);
}

// NOTE: Windows UTF8 to String16
// -----------------------------------------------------------------------------
DQN_API int Dqn_Win_CString8ToCString16(const char *src, int src_size, wchar_t *dest, int dest_size)
{
    int result = MultiByteToWideChar(CP_UTF8, 0 /*dwFlags*/, src, src_size, dest, dest_size);
    if (result && dest && dest_size > 0)
        dest[DQN_MIN(result, dest_size - 1)] = 0; // Null-terminate the buffer
    return result;
}

DQN_API int Dqn_Win_String8ToCString16(Dqn_String8 src, wchar_t *dest, int dest_size)
{
    int result   = 0;
    int src_size = Dqn_Safe_SaturateCastISizeToInt(src.size);
    if (src_size)
        result = Dqn_Win_CString8ToCString16(src.data, src_size, dest, dest_size);
    return result;
}

DQN_API Dqn_String16 Dqn_Win_String8ToString16Allocator(Dqn_String8 src, Dqn_Allocator allocator)
{
    Dqn_String16 result = {};
    int required = Dqn_Win_String8ToCString16(src, nullptr, 0);
    if (required != 0) {
        result.data = Dqn_Allocator_NewArray(allocator, wchar_t, required + 1, Dqn_ZeroMem_No);
        if (result.data) {
            result.size = Dqn_Win_String8ToCString16(src, result.data, required + 1);
            DQN_ASSERT(result.size == required);
        }
    }
    return result;
}

// NOTE: Windows String16 To UTF8
// -----------------------------------------------------------------------------
DQN_API int Dqn_Win_CString16ToCString8(const wchar_t* src, int src_size, char *dest, int dest_size)
{
    int result = WideCharToMultiByte(CP_UTF8, 0 /*dwFlags*/, src, src_size, dest, dest_size, nullptr, nullptr);
    if (result && dest && dest_size > 0)
        dest[DQN_MIN(result, dest_size - 1)] = 0;
    return result;
}

DQN_API Dqn_String8 Dqn_Win_CString16ToString8Allocator(const wchar_t* src, int src_size, Dqn_Allocator allocator)
{
    Dqn_String8 result = {};
    int required = Dqn_Win_CString16ToCString8(src, src_size, nullptr, 0);
    if (required != 0) {
        // NOTE: String allocate ensures there's one extra byte for
        // null-termination already so no-need to +1 the required size
        result = Dqn_String8_Allocate(allocator, required, Dqn_ZeroMem_No);
        if (Dqn_String8_IsValid(result)) {
            int next_required = Dqn_Win_CString16ToCString8(src, src_size, result.data, required + 1);
            DQN_ASSERT(required == next_required);
        }
    }
    return result;
}

DQN_API int Dqn_Win_String16ToCString8(Dqn_String16 src, char *dest, int dest_size)
{
    int result = 0;
    int src_size = Dqn_Safe_SaturateCastISizeToInt(src.size);
    if (src_size) {
        result = Dqn_Win_CString16ToCString8(src.data, src_size, dest, dest_size);
    }
    return result;
}

DQN_API Dqn_String8 Dqn_Win_String16ToString8Allocator(Dqn_String16 src, Dqn_Allocator allocator)
{
    Dqn_String8 result = {};
    int src_size = Dqn_Safe_SaturateCastISizeToInt(src.size);
    if (src_size) {
        result = Dqn_Win_CString16ToString8Allocator(src.data, src_size, allocator);
    }
    return result;
}

// NOTE: Windows Executable Directory
// -----------------------------------------------------------------------------
DQN_API Dqn_isize Dqn_Win_EXEDirW(wchar_t *buffer, Dqn_isize size)
{
    wchar_t module_path[DQN_OS_WIN32_MAX_PATH];
    int module_size = GetModuleFileNameW(nullptr /*module*/, module_path, DQN_ARRAY_UCOUNT(module_path));
    DQN_HARD_ASSERT_MSG(GetLastError() != ERROR_INSUFFICIENT_BUFFER, "How the hell?");

    Dqn_isize result = 0;
    for (int index = module_size - 1; !result && index >= 0; index--)
        result = module_path[index] == '\\' ? index : 0;

    if (!buffer || size < result) {
        return result;
    }

    DQN_MEMCPY(buffer, module_path, sizeof(wchar_t) * result);
    return result;
}

DQN_API Dqn_String16 Dqn_Win_EXEDirWArena(Dqn_Arena *arena)
{
    wchar_t dir[DQN_OS_WIN32_MAX_PATH];
    Dqn_isize dir_size = Dqn_Win_EXEDirW(dir, DQN_ARRAY_ICOUNT(dir));
    DQN_HARD_ASSERT_MSG(dir_size <= DQN_ARRAY_ICOUNT(dir), "How the hell?");

    Dqn_String16 result = {};
    if (dir_size > 0) {
        result.data = Dqn_Arena_CopyZ(arena, wchar_t, dir, dir_size);
        if (result.data) {
            result.size = dir_size;
        }
    }
    return result;
}

DQN_API Dqn_String8 Dqn_Win_WorkingDir(Dqn_Allocator allocator, Dqn_String8 suffix)
{
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(allocator.user_context);
    Dqn_String16 suffix16     = Dqn_Win_String8ToString16Allocator(suffix, Dqn_Arena_Allocator(scratch.arena));
    Dqn_String16 dir16        = Dqn_Win_WorkingDirW(Dqn_Arena_Allocator(scratch.arena), suffix16);
    Dqn_String8 result        = Dqn_Win_String16ToString8Allocator(dir16, allocator);
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

    result = Dqn_String16{w_path, DQN_CAST(Dqn_isize)(desired_size - 1)};
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
        DQN_ASSERT(it->file_name.size < (Dqn_CArray_CountI(it->file_name_buf) - 1));
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

    wchar_t path16[DQN_OS_WIN32_MAX_PATH + 1];
    path16[0] = 0;
    int path16_size = 0;

    Dqn_Win_FolderIteratorW wide_it = {};
    if (it->handle) {
        wide_it.handle = it->handle;
    } else {
        bool needs_asterisks = Dqn_String8_EndsWith(path, DQN_STRING8("\\")) ||
                               Dqn_String8_EndsWith(path, DQN_STRING8("/"));
        bool has_glob        = Dqn_String8_EndsWith(path, DQN_STRING8("\\*")) ||
                               Dqn_String8_EndsWith(path, DQN_STRING8("/*"));

        path16_size = Dqn_Win_String8ToCString16(path, path16, DQN_ARRAY_UCOUNT(path16));
        if (path16_size <= 0) // Conversion error
            return false;

        if (!has_glob) {
            // NOTE: We are missing the glob for enumerating the files, we will
            // add those characters in this branch, so overwrite the null
            // character, add the glob and re-null terminate the buffer.
            int space = DQN_OS_WIN32_MAX_PATH - path16_size;
            if (needs_asterisks) {
                if (space < 2)
                    return false;
                path16[path16_size++] = L'*';
            } else {
                if (space < 3)
                    return false;
                path16[path16_size++] = L'\\';
                path16[path16_size++] = L'*';
            }
            path16[path16_size++] = 0;
        }
        DQN_ASSERT(path16_size <= DQN_OS_WIN32_MAX_PATH);
    }

    bool result = Dqn_Win_FolderWIterate(Dqn_String16{path16, path16_size}, &wide_it);
    it->handle = wide_it.handle;
    if (result) {
        int size = Dqn_Win_String16ToCString8(wide_it.file_name, it->file_name_buf, DQN_ARRAY_ICOUNT(it->file_name_buf));
        it->file_name = Dqn_String8_Init(it->file_name_buf, size);
    }

    return result;
}

#if !defined(DQN_NO_WIN_NET)
// ---------------------------------+-----------------------------+---------------------------------
// [SECT-WINN] Dqn_WinNet           | DQN_NO_WINNET               | Windows internet download/query helpers
// ---------------------------------+-----------------------------+---------------------------------
DQN_API Dqn_WinNetHandle Dqn_Win_NetHandleInitCString(char const *url, int url_size)
{
    URL_COMPONENTSA components  = {};
    components.dwStructSize     = sizeof(components);
    components.dwHostNameLength = url_size;
    components.dwUrlPathLength  = url_size;

    // Seperate the URL into bits and bobs
    Dqn_WinNetHandle result = {};
    if (!InternetCrackUrlA(url, url_size, 0 /*flags*/, &components)) {
        Dqn_Log_ErrorF("InternetCrackUrlA failed [reason=%.*s]", DQN_STRING_FMT(Dqn_Win_LastError()));
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

    if (!Dqn_Safe_Assert(handle->http_handle))
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
        Dqn_Log_ErrorF("Failed to send request to %.*s [reason=%.*s]",
                       handle->host_name_size,
                       handle->host_name,
                       DQN_STRING_FMT(Dqn_Win_LastError()));
        return result;
    }

    handle->state = Dqn_WinNetHandleState_RequestGood;
    unsigned long buffer_size = 0;
    int query_result          = HttpQueryInfoA(handle->http_handle, HTTP_QUERY_RAW_HEADERS_CRLF, nullptr, &buffer_size, nullptr);
    if (!Dqn_Safe_Assert(query_result != ERROR_INSUFFICIENT_BUFFER))
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
    Dqn_isize splits_required = Dqn_String8_Split(result.raw_headers, delimiter, nullptr, 0);
    result.headers            = Dqn_Allocator_NewArray(allocator, Dqn_String8, splits_required, Dqn_ZeroMem_No);
    result.headers_size       = Dqn_String8_Split(result.raw_headers, delimiter, result.headers, splits_required);

    bool found_content_type   = false;
    bool found_content_length = false;
    for (Dqn_isize header_index = 0; header_index < result.headers_size; header_index++) {
        Dqn_String8 header = result.headers[header_index];
        Dqn_String8 value  = {};
        Dqn_String8 key    = Dqn_String8_BinarySplit(header, ':', &value);

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
                result.content_length = Dqn_String8_ToU64(value, 0 /*separator*/);
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
    Dqn_ThreadScratch scratch   = Dqn_Thread_GetScratch(arena);
    size_t total_size             = 0;
    Dqn_Win_NetChunk *first_chunk = nullptr;
    for (Dqn_Win_NetChunk *last_chunk = nullptr;;)
    {
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
    for (Dqn_Win_NetChunk *chunk = first_chunk; chunk; chunk = chunk->next)
    {
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
    for (bool keep_pumping = true; keep_pumping;)
    {
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
        auto *chunk      = DQN_CAST(Dqn_Win_NetChunk *)Dqn_VMem_Reserve(sizeof(Dqn_Win_NetChunk), true /*commit*/);
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

    auto *result     = DQN_CAST(char *)Dqn_VMem_Reserve(total_size * sizeof(char), true /*commit*/);
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

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-OSYS] Dqn_OS               | DQN_NO_WIN                  | Operating-system APIs
// ---------------------------------+-----------------------------+---------------------------------
DQN_API bool Dqn_OS_SecureRNGBytes(void *buffer, uint32_t size)
{
    if (!buffer || size < 0)
        return false;

    if (size == 0)
        return true;

#if defined(DQN_OS_WIN32)
    bool init = true;
    Dqn_TicketMutex_Begin(&dqn_library.win32_bcrypt_rng_mutex);
    if (!dqn_library.win32_bcrypt_rng_handle)
    {
        wchar_t const BCRYPT_ALGORITHM[] = L"RNG";
        long /*NTSTATUS*/ init_status = BCryptOpenAlgorithmProvider(&dqn_library.win32_bcrypt_rng_handle, BCRYPT_ALGORITHM, nullptr /*implementation*/, 0 /*flags*/);
        if (!dqn_library.win32_bcrypt_rng_handle || init_status != 0)
        {
            Dqn_Log_ErrorF("Failed to initialise random number generator, error: %d", init_status);
            init = false;
        }
    }
    Dqn_TicketMutex_End(&dqn_library.win32_bcrypt_rng_mutex);

    if (!init)
        return false;

    long gen_status = BCryptGenRandom(dqn_library.win32_bcrypt_rng_handle, DQN_CAST(unsigned char *)buffer, size, 0 /*flags*/);
    if (gen_status != 0)
    {
        Dqn_Log_ErrorF("Failed to generate random bytes: %d", gen_status);
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

DQN_API Dqn_String8 Dqn_OS_EXEDir(Dqn_Allocator allocator)
{
    Dqn_String8 result = {};

#if defined(DQN_OS_WIN32)
    wchar_t exe_dir[DQN_OS_WIN32_MAX_PATH];
    Dqn_isize exe_dir_size = Dqn_Win_EXEDirW(exe_dir, DQN_ARRAY_ICOUNT(exe_dir));
    result = Dqn_Win_CString16ToString8Allocator(exe_dir, DQN_CAST(int)exe_dir_size, allocator);

#elif defined(DQN_OS_UNIX)

    int required_size_wo_null_terminator = 0;
    for (int try_size = 128;
         ;
         try_size *= 2)
    {
        auto scoped_arena = Dqn_ArenaTempMemoryScope(arena);
        char *try_buf     = Dqn_Arena_NewArray(arena, char, try_size, Dqn_ZeroMem_No);
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
        Dqn_ArenaTempMemory scope = Dqn_Arena_BeginTempMemory(arena);
        char *exe_path = Dqn_Arena_NewArray(arena, char, required_size_wo_null_terminator + 1, Dqn_ZeroMem_No);
        exe_path[required_size_wo_null_terminator] = 0;

        int bytes_written = readlink("/proc/self/exe", exe_path, required_size_wo_null_terminator);
        if (bytes_written == -1)
        {
            // Note that if read-link fails again can be because there's
            // a potential race condition here, our exe or directory could have
            // been deleted since the last call, so we need to be careful.
            Dqn_Arena_EndTempMemory(scope);
        }
        else
        {
            result = Dqn_String8_Init(exe_path, required_size_wo_null_terminator);
        }
    }

#else
    #error Unimplemented
#endif

    return result;
}

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
#if defined(DQN_OS_WIN32)
    if (dqn_library.win32_qpc_frequency.QuadPart == 0)
        QueryPerformanceFrequency(&dqn_library.win32_qpc_frequency);
#endif
}

DQN_API Dqn_f64 Dqn_OS_PerfCounterS(uint64_t begin, uint64_t end)
{
    Dqn_OS_PerfCounter_Init();
    uint64_t ticks  = end - begin;
#if defined(DQN_OS_WIN32)
    Dqn_f64 result = ticks / DQN_CAST(Dqn_f64)dqn_library.win32_qpc_frequency.QuadPart;
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
    Dqn_f64 result = (ticks * 1'000) / DQN_CAST(Dqn_f64)dqn_library.win32_qpc_frequency.QuadPart;
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
    Dqn_f64 result = (ticks * 1'000'000) / DQN_CAST(Dqn_f64)dqn_library.win32_qpc_frequency.QuadPart;
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
    Dqn_f64 result = (ticks * 1'000'000'000) / DQN_CAST(Dqn_f64)dqn_library.win32_qpc_frequency.QuadPart;
#else
    Dqn_f64 result = ticks;
#endif
    return result;
}

DQN_API uint64_t Dqn_OS_PerfCounterNow()
{
    uint64_t result = 0;
#if defined(DQN_OS_WIN32)
    LARGE_INTEGER integer = {};
    int qpc_result = QueryPerformanceCounter(&integer);
    (void)qpc_result;
    DQN_ASSERT_MSG(qpc_result, "MSDN says this can only fail when running on a version older than Windows XP");
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

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-FSYS] Dqn_Fs               |                             | Filesystem helpers
// ---------------------------------+-----------------------------+---------------------------------
#if defined(DQN_OS_WIN32)
DQN_API uint64_t Dqn__WinFileTimeToSeconds(FILETIME const *time)
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
#if defined(DQN_OS_WIN32)
    wchar_t path16[DQN_OS_WIN32_MAX_PATH];
    int path16_size = Dqn_Win_String8ToCString16(path, path16, DQN_ARRAY_ICOUNT(path16));
    if (path16_size) {
        WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
        if (GetFileAttributesExW(path16, GetFileExInfoStandard, &attrib_data)) {
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
#if defined(DQN_OS_WIN32)
    wchar_t path16[DQN_OS_WIN32_MAX_PATH];
    int path16_size = Dqn_Win_String8ToCString16(path, path16, DQN_ARRAY_ICOUNT(path16));
    if (path16_size) {
        WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
        if (GetFileAttributesExW(path16, GetFileExInfoStandard, &attrib_data)) {
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
    #if defined(DQN_OS_WIN32)
    WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
    wchar_t path16[DQN_OS_WIN32_MAX_PATH];
    Dqn_Win_String8ToCString16(path, path16, DQN_ARRAY_ICOUNT(path16));
    if (!GetFileAttributesExW(path16, GetFileExInfoStandard, &attrib_data))
        return result;

    result.create_time_in_s      = Dqn__WinFileTimeToSeconds(&attrib_data.ftCreationTime);
    result.last_access_time_in_s = Dqn__WinFileTimeToSeconds(&attrib_data.ftLastAccessTime);
    result.last_write_time_in_s  = Dqn__WinFileTimeToSeconds(&attrib_data.ftLastWriteTime);

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
    Dqn_String16      src16   = Dqn_Win_String8ToString16Allocator(src, Dqn_Arena_Allocator(scratch.arena));
    Dqn_String16      dest16  = Dqn_Win_String8ToString16Allocator(dest, Dqn_Arena_Allocator(scratch.arena));

    int fail_if_exists = overwrite == false;
    result = CopyFileW(src16.data, dest16.data, fail_if_exists) != 0;

    if (!result) {
        Dqn_WinErrorMsg error = Dqn_Win_LastError();
        Dqn_Log_ErrorF("Copy file failed [src=\"%.*s\", dest=\"%.*s\", reason=\"%.*s\"]",
                       DQN_STRING_FMT(src),
                       DQN_STRING_FMT(dest),
                       DQN_STRING_FMT(error));
    }

#elif defined(DQN_OS_UNIX)
    int src_fd  = open(src.data, O_RDONLY);
    int dest_fd = open(dest.data, O_WRONLY | O_CREAT | (overwrite ? O_TRUNC : 0));

    if (src_fd != -1 && dest_fd != -1) {
        struct stat stat_existing;
        fstat(src_fd, &stat_existing);
        Dqn_isize bytes_written = sendfile64(dest_fd, src_fd, 0, stat_existing.st_size);
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
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    bool result               = true;
    int path_indexes_size     = 0;
    uint16_t path_indexes[64] = {};

#if defined(DQN_OS_WIN32)
    Dqn_String16 path16 = Dqn_Win_String8ToString16Allocator(path, Dqn_Arena_Allocator(scratch.arena));

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
    for (Dqn_isize index = path16.size - 1; index >= 0; index--) {
        bool first_char = index == (path16.size - 1);
        wchar_t ch      = path16.data[index];
        if (ch == '/' || ch == '\\' || first_char) {
            WIN32_FILE_ATTRIBUTE_DATA attrib_data = {};
            wchar_t temp = path16.data[index];

            if (!first_char) path16.data[index] = 0; // Temporarily null terminate it
            bool successful = GetFileAttributesExW(path16.data, GetFileExInfoStandard, &attrib_data); // Check
            if (!first_char) path16.data[index] = temp; // Undo null termination

            if (successful) {
                if (attrib_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    // NOTE: We found a directory, we can stop here and start
                    // building up all the directories that didn't exist up to
                    // this point.
                    break;
                } else {
                    // NOTE: There's something that exists in at this path, but
                    // it's not a directory. This request to make a directory is
                    // invalid.
                    return false;
                }
            } else {
                // NOTE: There's nothing that exists at this path, we can create
                // a directory here
                path_indexes[path_indexes_size++] = DQN_CAST(uint16_t)index;
            }
        }
    }

    for (Dqn_isize index = path_indexes_size - 1; index >= 0 && result; index--) {
        uint16_t path_index = path_indexes[index];
        wchar_t temp = path16.data[path_index];

        if (index != 0) path16.data[path_index] = 0;
        result |= (CreateDirectoryW(path16.data, nullptr) == 0);
        if (index != 0) path16.data[path_index] = temp;
    }

#elif defined(DQN_OS_UNIX)
    Dqn_String8 copy = Dqn_String8_Copy(scratch.arena, path);
    for (int32_t index = copy.size - 1; index >= 0; index--) {
        bool first_char = index == (copy.size - 1);
        wchar_t ch         = copy.data[index];
        if (ch == '/' || first_char) {
            char temp = copy.data[index];
            if (!first_char) copy.data[index] = 0; // Temporarily null terminate it
            bool is_file = Dqn_Fs_Exists(copy);
            if (!first_char) copy.data[index] = temp; // Undo null termination

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

    for (Dqn_isize index = path_indexes_size - 1; index >= 0 && result; index--) {
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
    Dqn_String16      src16   = Dqn_Win_String8ToString16Allocator(src, Dqn_Arena_Allocator(scratch.arena));
    Dqn_String16      dest16  = Dqn_Win_String8ToString16Allocator(dest, Dqn_Arena_Allocator(scratch.arena));

    unsigned long flags = MOVEFILE_COPY_ALLOWED;
    if (overwrite) {
        flags |= MOVEFILE_REPLACE_EXISTING;
    }

    result = MoveFileExW(src16.data, dest16.data, flags) != 0;
    if (!result) {
        Dqn_Log_ErrorF("Failed to move file [from=%.*s, to=%.*s, reason=%.*s]",
                  DQN_STRING_FMT(src),
                  DQN_STRING_FMT(dest),
                  DQN_STRING_FMT(Dqn_Win_LastError()));
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
#if defined(DQN_OS_WIN32)
    wchar_t path16[DQN_OS_WIN32_MAX_PATH];
    int path16_size = Dqn_Win_String8ToCString16(path, path16, DQN_ARRAY_ICOUNT(path16));
    if (path16_size) {
        result = DeleteFileW(path16);
        if (!result)
            result = RemoveDirectoryW(path16);
    }
#elif defined(DQN_OS_UNIX)
    result = remove(path.data) == 0;
#else
    #error Unimplemented
#endif

    return result;
}

// NOTE: Read/Write Entire File API
// =============================================================================
DQN_API char *Dqn_Fs_ReadCString8_(DQN_LEAK_TRACE_FUNCTION char const *path, Dqn_isize path_size, Dqn_isize *file_size, Dqn_Allocator allocator)
{
    char *result = nullptr;
    if (!path)
        return result;

    if (path_size <= 0)
        path_size = Dqn_CString8_Size(path);

    (void)allocator;
    (void)file_size;

#if defined(DQN_OS_WIN32)
    // NOTE: Convert to UTF16
    // -------------------------------------------------------------------------
    Dqn_ThreadScratch scratch   = Dqn_Thread_GetScratch(allocator.user_context);
    Dqn_String8       path8     = Dqn_String8_Init(path, path_size);
    Dqn_String16      path16    = Dqn_Win_String8ToString16Allocator(path8, scratch.allocator);
    Dqn_WinErrorMsg   error_msg = {};

    // NOTE: Get the file handle
    // -------------------------------------------------------------------------
    void *file_handle = CreateFileW(/*LPCWSTR               lpFileName*/            path16.data,
                                    /*DWORD                 dwDesiredAccess*/       GENERIC_READ,
                                    /*DWORD                 dwShareMode*/           0,
                                    /*LPSECURITY_ATTRIBUTES lpSecurityAttributes*/  nullptr,
                                    /*DWORD                 dwCreationDisposition*/ OPEN_EXISTING,
                                    /*DWORD                 dwFlagsAndAttributes*/  FILE_ATTRIBUTE_READONLY,
                                    /*HANDLE                hTemplateFile*/         nullptr);
    if (file_handle == INVALID_HANDLE_VALUE) {
        Dqn_Win_LastErrorToBuffer(&error_msg);
        Dqn_Log_ErrorF("Failed to open file for reading [file=%.*s, reason=%.*s]", DQN_STRING_FMT(path8), DQN_STRING_FMT(error_msg));
        return nullptr;
    }
    DQN_DEFER { CloseHandle(file_handle); };

    // NOTE: Query the file size
    // -------------------------------------------------------------------------
    LARGE_INTEGER win_file_size;
    if (!GetFileSizeEx(file_handle, &win_file_size)) {
        Dqn_Win_LastErrorToBuffer(&error_msg);
        Dqn_Log_ErrorF("Failed to query file size [file=%.*s, reason=%.*s]", DQN_STRING_FMT(path8), DQN_STRING_FMT(error_msg));
        return nullptr;
    }

    unsigned long const bytes_desired = DQN_CAST(unsigned long)win_file_size.QuadPart;
    if (!Dqn_Safe_AssertF(bytes_desired == win_file_size.QuadPart,
                          "Current implementation doesn't support >4GiB, implement Win32 overlapped IO")) {
        return nullptr;
    }

    // NOTE: Read the file from disk
    // -------------------------------------------------------------------------
    result = DQN_CAST(char *)Dqn_Allocator_Alloc_(DQN_LEAK_TRACE_ARG allocator,
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
        Dqn_Win_LastErrorToBuffer(&error_msg);
        Dqn_Log_ErrorF("'ReadFile' failed to load file to memory [file=%.*s, reason=%.*s]", DQN_STRING_FMT(path8), DQN_STRING_FMT(error_msg));
        return nullptr;
    }

    if (bytes_read != bytes_desired) {
        Dqn_Win_LastErrorToBuffer(&error_msg);
        Dqn_Allocator_Dealloc(allocator, result, bytes_desired);
        Dqn_Log_ErrorF("'ReadFile' failed to read all bytes into file [file=%.*s, bytes_desired=%u, bytes_read=%u, reason=%.*s]",
                  DQN_STRING_FMT(path8),
                  bytes_desired,
                  bytes_read,
                  DQN_STRING_FMT(error_msg));
        return nullptr;
    }

    if (file_size) {
        *file_size = Dqn_Safe_SaturateCastI64ToISize(bytes_read);
    }
#else
    Dqn_isize file_size_ = 0;
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
    result = DQN_CAST(char *)Dqn_Allocator_Alloc(DQN_LEAK_TRACE_ARG allocator,
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

DQN_API Dqn_String8 Dqn_Fs_ReadString8_(DQN_LEAK_TRACE_FUNCTION Dqn_String8 path, Dqn_Allocator allocator)
{
    Dqn_isize   file_size = 0;
    char *      string    = Dqn_Fs_ReadCString8_(DQN_LEAK_TRACE_ARG path.data, path.size, &file_size, allocator);
    Dqn_String8 result    = Dqn_String8_Init(string, file_size);
    return result;
}

DQN_API bool Dqn_Fs_WriteCString8(char const *path, Dqn_isize path_size, char const *buffer, Dqn_isize buffer_size)
{
    bool result = false;
    if (!path || !buffer || buffer_size <= 0)
        return result;

#if defined(DQN_OS_WIN32)
    if (path_size <= 0)
        path_size = Dqn_CString8_Size(path);

    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String8       path8   = Dqn_String8_Init(path, path_size);
    Dqn_String16      path16  = Dqn_Win_String8ToString16Allocator(path8, scratch.allocator);

    void *file_handle = CreateFileW(/*LPCWSTR               lpFileName*/            path16.data,
                                    /*DWORD                 dwDesiredAccess*/       GENERIC_WRITE,
                                    /*DWORD                 dwShareMode*/           0,
                                    /*LPSECURITY_ATTRIBUTES lpSecurityAttributes*/  nullptr,
                                    /*DWORD                 dwCreationDisposition*/ CREATE_ALWAYS,
                                    /*DWORD                 dwFlagsAndAttributes*/  FILE_ATTRIBUTE_NORMAL,
                                    /*HANDLE                hTemplateFile*/         nullptr);

    if (file_handle == INVALID_HANDLE_VALUE) {
        Dqn_Log_ErrorF("Failed to open file for writing [file=%.*s, reason=%.*s]", DQN_STRING_FMT(path8), DQN_STRING_FMT(Dqn_Win_LastError()));
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

DQN_API bool Dqn_Fs_WriteString8(Dqn_String8 file_path, Dqn_String8 buffer)
{
    bool result = Dqn_Fs_WriteCString8(file_path.data, file_path.size, buffer.data, buffer.size);
    return result;
}

// NOTE: Read/Write File Stream API
// =============================================================================
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
        DQN_ASSERT_MSG(
            (access & ~Dqn_FsFileAccess_AppendOnly) == 0,
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
    Dqn_String16      path16  = Dqn_Win_String8ToString16Allocator(path, scratch.allocator);
    void *handle              = CreateFileW(/*LPCWSTR               lpFileName*/            path16.data,
                                            /*DWORD                 dwDesiredAccess*/       access_mode,
                                            /*DWORD                 dwShareMode*/           0,
                                            /*LPSECURITY_ATTRIBUTES lpSecurityAttributes*/  nullptr,
                                            /*DWORD                 dwCreationDisposition*/ create_flag,
                                            /*DWORD                 dwFlagsAndAttributes*/  FILE_ATTRIBUTE_NORMAL,
                                            /*HANDLE                hTemplateFile*/         nullptr);

    if (handle == INVALID_HANDLE_VALUE) {
        Dqn_WinErrorMsg msg = Dqn_Win_LastError();
        result.error_size =
            DQN_CAST(uint16_t) Dqn_SNPrintF2DotsOnOverflow(result.error,
                                                           DQN_ARRAY_UCOUNT(result.error),
                                                           "Open file failed: %.*s for \"%.*s\"",
                                                           DQN_STRING_FMT(msg),
                                                           DQN_STRING_FMT(path));
        return result;
    }
    #else
    if (access & Dqn_FsFileAccess_Execute) {
        result.error_size = DQN_CAST(uint16_t) Dqn_SNPrintF2DotsOnOverflow(
            result.error,
            DQN_ARRAY_UCOUNT(result.error),
            "Open file failed: execute access not supported for \"%.*s\"",
            DQN_STRING8_FMT(path));
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
                DQN_STRING8_FMT(path));
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
            DQN_STRING8_FMT(path));
        return result;
    }
    #endif
    result.handle = handle;
    return result;
}

DQN_API bool Dqn_Fs_WriteFile(Dqn_FsFile *file, char const *buffer, Dqn_isize size)
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
        Dqn_WinErrorMsg msg = Dqn_Win_LastError();
        file->error_size =
            DQN_CAST(uint16_t) Dqn_SNPrintF2DotsOnOverflow(file->error,
                                                           DQN_ARRAY_UCOUNT(file->error),
                                                           "Write file failed: %.*s for %.*s",
                                                           DQN_STRING_FMT(msg));
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

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-MISC] Miscellaneous        |                             | General purpose helpers
// ---------------------------------+-----------------------------+---------------------------------
DQN_API int Dqn_SNPrintF2DotsOnOverflow(char *buffer, int size, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int size_required = STB_SPRINTF_DECORATE(vsnprintf)(buffer, size, fmt, args);
    int result        = DQN_MAX(DQN_MIN(size_required, size - 1), 0);
    if (result == size - 1) {
        buffer[size - 2] = '.';
        buffer[size - 3] = '.';
    }
    va_end(args);
    return result;
}

DQN_API Dqn_U64String Dqn_U64ToString(uint64_t val, char separator)
{
    Dqn_U64String result = {};
    if (val == 0) {
        result.data[result.size++] = '0';
    } else {
        // NOTE: The number is written in reverse because we form the string by
        // dividing by 10, so we write it in, then reverse it out after all is
        // done.
        Dqn_U64String temp = {};
        for (size_t digit_count = 0; val > 0; digit_count++) {
            if (separator && (digit_count != 0) && (digit_count % 3 == 0)) {
                temp.data[temp.size++] = separator;
            }

            auto digit             = DQN_CAST(char)(val % 10);
            temp.data[temp.size++] = '0' + digit;
            val /= 10;
        }

        // NOTE: Reverse the string
        for (size_t temp_index = temp.size - 1; temp_index < temp.size; temp_index--) {
            char ch = temp.data[temp_index];
            result.data[result.size++] = ch;
        }
    }

    return result;
}

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-TCTX] Dqn_ThreadContext    |                             | Per-thread data structure e.g. temp arenas
// ---------------------------------+-----------------------------+---------------------------------
Dqn_ThreadScratch::Dqn_ThreadScratch(DQN_LEAK_TRACE_FUNCTION Dqn_ThreadContext *context, uint8_t context_index)
{
    index       = context_index;
    allocator   = context->temp_allocators[index];
    arena       = &context->temp_arenas[index];
    temp_memory = Dqn_Arena_BeginTempMemory(arena);
    #if defined(DQN_LEAK_TRACKING)
    leak_site__ = DQN_LEAK_TRACE_ARG_NO_COMMA;
    #endif
}

Dqn_ThreadScratch::~Dqn_ThreadScratch()
{
    #if defined(DQN_DEBUG_THREAD_CONTEXT)
    temp_arenas_stat[index] = arena->stats;
    #endif
    DQN_ASSERT(destructed == false);
    #if defined(DQN_LEAK_TRACING)
    Dqn_Arena_EndTempMemory_(DQN_LEAK_TRACE_ARG_NO_COMMA temp_memory);
    #else
    Dqn_Arena_EndTempMemory_(temp_memory);
    #endif
    destructed = true;
}

DQN_API Dqn_ThreadContext *Dqn_Thread_GetContext_(DQN_LEAK_TRACE_FUNCTION_NO_COMMA)
{
    thread_local Dqn_ThreadContext result = {};
    if (!result.init) {
        result.init = true;
        for (uint8_t index = 0; index < DQN_THREAD_CONTEXT_ARENAS; index++) {
            Dqn_Arena *arena              = result.temp_arenas + index;
            result.temp_allocators[index] = Dqn_Arena_Allocator(arena);
            Dqn_Arena_Grow_(DQN_LEAK_TRACE_ARG arena, DQN_MEGABYTES(4), DQN_MEGABYTES(4));
        }
    }
    return &result;
}

// TODO: Is there a way to handle conflict arenas without the user needing to
// manually pass it in?
DQN_API Dqn_ThreadScratch Dqn_Thread_GetScratch_(DQN_LEAK_TRACE_FUNCTION void const *conflict_arena)
{
    static_assert(DQN_THREAD_CONTEXT_ARENAS < (uint8_t)-1, "We use UINT8_MAX as a sentinel value");
    Dqn_ThreadContext *context = Dqn_Thread_GetContext_(DQN_LEAK_TRACE_ARG_NO_COMMA);
    uint8_t context_index      = (uint8_t)-1;
    for (uint8_t index = 0; index < DQN_THREAD_CONTEXT_ARENAS; index++) {
        Dqn_Arena *arena = context->temp_arenas + index;
        if (!conflict_arena || arena != conflict_arena) {
            context_index = index;
            break;
        }
    }

    DQN_ASSERT(context_index != (uint8_t)-1);
    return Dqn_ThreadScratch(context, context_index);
}

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-JSON] Dqn_JSONBuilder      | DQN_NO_JSON_BUILDER         | Construct json output
// ---------------------------------+-----------------------------+---------------------------------
#if !defined(DQN_NO_JSON_BUILDER)
Dqn_JSONBuilder Dqn_JSONBuilder_Init(Dqn_Arena *arena, int spaces_per_indent)
{
    Dqn_JSONBuilder result   = {};
    result.spaces_per_indent = spaces_per_indent;
    result.builder.allocator = Dqn_Arena_Allocator(arena);
    return result;
}

Dqn_String8 Dqn_JSONBuilder_Build(Dqn_JSONBuilder *builder, Dqn_Arena *arena)
{
    Dqn_String8 result = Dqn_String8Builder_Build(&builder->builder, Dqn_Arena_Allocator(arena));
    return result;
}

void Dqn_JSONBuilder_AppendFV_(Dqn_JSONBuilder *builder, char const *fmt, va_list args)
{
    if (!builder || !fmt)
        return;

    if (builder->builder.allocator.alloc)
        Dqn_String8Builder_AppendFV(&builder->builder, fmt, args);
    else
        vfprintf(stdout, fmt, args);
}

void Dqn_JSONBuilder_AppendF_(Dqn_JSONBuilder *builder, char const *fmt, ...)
{
    if (!builder || !fmt)
        return;

    va_list args;
    va_start(args, fmt);
    Dqn_JSONBuilder_AppendFV_(builder, fmt, args);
    va_end(args);
}

void Dqn_JSONBuilder_AppendString8Ref_(Dqn_JSONBuilder *builder, Dqn_String8 string)
{
    if (builder->builder.allocator.alloc)
        Dqn_String8Builder_AppendString8Ref(&builder->builder, string);
    else
        fprintf(stdout, "%.*s", DQN_STRING_FMT(string));
}

void Dqn_JSONBuilder_DoIndent_(Dqn_JSONBuilder *builder)
{
    int spaces_per_indent = builder->spaces_per_indent ? builder->spaces_per_indent : 2;
    int spaces            = builder->indent_level * spaces_per_indent;
    if (spaces)
        Dqn_JSONBuilder_AppendF_(builder, "%*c", spaces, ' ');
}

void Dqn_JSONBuilder_PreAddItem_(Dqn_JSONBuilder *builder)
{
    if (builder->parent_field_count_stack_size <= 0)
        return;

    uint16_t *parent_field_count = &builder->parent_field_count_stack[builder->parent_field_count_stack_size - 1];
    if (*parent_field_count == 0) {
        // NOTE: First time we're adding an item to an object, we need to write
        // on a new line for nice formatting.
        Dqn_JSONBuilder_AppendString8Ref_(builder, DQN_STRING8("\n"));
    } else if (*parent_field_count > 0) {
        // NOTE: We have items in the object already and we're adding another
        // item so we need to add a comma on the previous item.
        Dqn_JSONBuilder_AppendString8Ref_(builder, DQN_STRING8(",\n"));
    }

    Dqn_JSONBuilder_DoIndent_(builder);
}

void Dqn_JSONBuilder_PostAddItem_(Dqn_JSONBuilder *builder)
{
    if (builder->parent_field_count_stack_size <= 0)
        return;

    uint16_t *parent_field_count = &builder->parent_field_count_stack[builder->parent_field_count_stack_size - 1];
    (*parent_field_count)++;
}

void Dqn_JSONBuilder_BeginContainer_(Dqn_JSONBuilder *builder, Dqn_String8 name, bool array)
{
    Dqn_String8 container_ch = array ? DQN_STRING8("[") : DQN_STRING8("{");
    Dqn_JSONBuilder_PreAddItem_(builder);
    if (name.size)
        Dqn_JSONBuilder_AppendF_(builder, "\"%.*s\": %.*s", DQN_STRING_FMT(name), DQN_STRING_FMT(container_ch));
    else
        Dqn_JSONBuilder_AppendString8Ref_(builder, container_ch);
    Dqn_JSONBuilder_PostAddItem_(builder);

    builder->indent_level++;

    DQN_ASSERT(builder->parent_field_count_stack_size < DQN_ARRAY_UCOUNT(builder->parent_field_count_stack));
    uint16_t *parent_field_count = &builder->parent_field_count_stack[builder->parent_field_count_stack_size++];
    *parent_field_count          = 0;
}

void Dqn_JSONBuilder_EndContainer_(Dqn_JSONBuilder *builder, bool array)
{
    uint16_t *parent_field_count = &builder->parent_field_count_stack[builder->parent_field_count_stack_size - 1];
    if (*parent_field_count > 0) {
        // NOTE: End of object/array should start on a new line if the
        // array/object has atleast one field in it.
        Dqn_JSONBuilder_AppendString8Ref_(builder, DQN_STRING8("\n"));
    }

    builder->parent_field_count_stack_size--;
    DQN_ASSERT(builder->parent_field_count_stack_size >= 0);

    builder->indent_level--;
    DQN_ASSERT(builder->indent_level >= 0);

    Dqn_JSONBuilder_DoIndent_(builder);
    Dqn_JSONBuilder_AppendString8Ref_(builder, array ? DQN_STRING8("]") : DQN_STRING8("}"));

    // NOTE: Root object got closed, what ever that we write next has to start
    // on a new line.
    if (builder->parent_field_count_stack_size == 0) {
        Dqn_JSONBuilder_AppendString8Ref_(builder, DQN_STRING8("\n"));
    }
}

void Dqn_JSONBuilder_ObjectBeginNamed(Dqn_JSONBuilder *builder, Dqn_String8 name)
{
    Dqn_JSONBuilder_BeginContainer_(builder, name, false /*array*/);
}

void Dqn_JSONBuilder_ObjectBegin(Dqn_JSONBuilder *builder)
{
    Dqn_JSONBuilder_BeginContainer_(builder, DQN_STRING8(""), false /*array*/);
}

void Dqn_JSONBuilder_ObjectEnd(Dqn_JSONBuilder *builder)
{
    Dqn_JSONBuilder_EndContainer_(builder, false /*array*/);
}

void Dqn_JSONBuilder_ArrayBeginNamed(Dqn_JSONBuilder *builder, Dqn_String8 name)
{
    Dqn_JSONBuilder_BeginContainer_(builder, name, true /*array*/);
}

void Dqn_JSONBuilder_ArrayBegin(Dqn_JSONBuilder *builder)
{
    Dqn_JSONBuilder_BeginContainer_(builder, DQN_STRING8(""), false /*array*/);
}

void Dqn_JSONBuilder_ArrayEnd(Dqn_JSONBuilder *builder)
{
    Dqn_JSONBuilder_EndContainer_(builder, true /*array*/);
}

void Dqn_JSONBuilder_StringNamed(Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value)
{
    Dqn_JSONBuilder_PreAddItem_(builder);
    if (key.size)
        Dqn_JSONBuilder_AppendF_(builder, "\"%.*s\": \"%.*s\"", DQN_STRING_FMT(key), DQN_STRING_FMT(value));
    else
        Dqn_JSONBuilder_AppendF_(builder, "\"%.*s\"", DQN_STRING_FMT(value));
    Dqn_JSONBuilder_PostAddItem_(builder);
}

void Dqn_JSONBuilder_String(Dqn_JSONBuilder *builder, Dqn_String8 value)
{
    Dqn_JSONBuilder_StringNamed(builder, DQN_STRING8("") /*key*/, value);
}

void Dqn_JSONBuilder_LiteralNamed(Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value)
{
    Dqn_JSONBuilder_PreAddItem_(builder);
    if (key.size)
        Dqn_JSONBuilder_AppendF_(builder, "\"%.*s\": %.*s", DQN_STRING_FMT(key), DQN_STRING_FMT(value));
    else
        Dqn_JSONBuilder_AppendF_(builder, "%.*s", DQN_STRING_FMT(value));
    Dqn_JSONBuilder_PostAddItem_(builder);
}

void Dqn_JSONBuilder_Literal(Dqn_JSONBuilder *builder, Dqn_String8 value)
{
    Dqn_JSONBuilder_LiteralNamed(builder, DQN_STRING8("") /*key*/, value);
}

void Dqn_JSONBuilder_U64Named(Dqn_JSONBuilder *builder, Dqn_String8 key, uint64_t value)
{
    Dqn_JSONBuilder_PreAddItem_(builder);
    if (key.size)
        Dqn_JSONBuilder_AppendF_(builder, "\"%.*s\": %I64u", DQN_STRING_FMT(key), value);
    else
        Dqn_JSONBuilder_AppendF_(builder, "%I64u", value);
    Dqn_JSONBuilder_PostAddItem_(builder);
}

void Dqn_JSONBuilder_U64(Dqn_JSONBuilder *builder, uint64_t value)
{
    Dqn_JSONBuilder_U64Named(builder, DQN_STRING8("") /*key*/, value);
}

void Dqn_JSONBuilder_I64Named(Dqn_JSONBuilder *builder, Dqn_String8 key, uint64_t value)
{
    Dqn_JSONBuilder_PreAddItem_(builder);
    if (key.size)
        Dqn_JSONBuilder_AppendF_(builder, "\"%.*s\": %I64d", DQN_STRING_FMT(key), value);
    else
        Dqn_JSONBuilder_AppendF_(builder, "%I64d", value);
    Dqn_JSONBuilder_PostAddItem_(builder);
}

void Dqn_JSONBuilder_I64(Dqn_JSONBuilder *builder, uint64_t value)
{
    Dqn_JSONBuilder_I64Named(builder, DQN_STRING8("") /*key*/, value);
}

void Dqn_JSONBuilder_F64Named(Dqn_JSONBuilder *builder, Dqn_String8 key, double value, int decimal_places)
{
    if (!builder)
        return;

    if (decimal_places >= 16)
        decimal_places = 16;

    // NOTE: Generate the format string for the float, depending on how many
    // decimals places it wants.
    char float_fmt[16];
    if (decimal_places > 0) {
        // NOTE: Emit the format string "%.<decimal_places>f" i.e. %.1f
        STB_SPRINTF_DECORATE(snprintf)(float_fmt, sizeof(float_fmt), "%%.%df", decimal_places);
    } else {
        // NOTE: Emit the format string "%f"
        STB_SPRINTF_DECORATE(snprintf)(float_fmt, sizeof(float_fmt), "%%f");
    }

    char fmt[32];
    if (key.size)
        STB_SPRINTF_DECORATE(snprintf)(fmt, sizeof(fmt), "\"%%.*s\": %s", float_fmt);
    else
        STB_SPRINTF_DECORATE(snprintf)(fmt, sizeof(fmt), "%s", float_fmt);

    Dqn_JSONBuilder_PreAddItem_(builder);
    if (key.size)
        Dqn_JSONBuilder_AppendF_(builder, fmt, DQN_STRING_FMT(key), value);
    else
        Dqn_JSONBuilder_AppendF_(builder, fmt, value);
    Dqn_JSONBuilder_PostAddItem_(builder);
}

void Dqn_JSONBuilder_F64(Dqn_JSONBuilder *builder, double value, int decimal_places)
{
    Dqn_JSONBuilder_F64Named(builder, DQN_STRING8("") /*key*/, value, decimal_places);
}

void Dqn_JSONBuilder_BoolNamed(Dqn_JSONBuilder *builder, Dqn_String8 key, bool value)
{
    Dqn_JSONBuilder_PreAddItem_(builder);
    Dqn_String8 value_string = value ? DQN_STRING8("true") : DQN_STRING8("false");
    if (key.size)
        Dqn_JSONBuilder_AppendF_(builder, "\"%.*s\": %.*s", DQN_STRING_FMT(key), DQN_STRING_FMT(value_string));
    else
        Dqn_JSONBuilder_AppendF_(builder, "%Boold", value);
    Dqn_JSONBuilder_PostAddItem_(builder);
}

void Dqn_JSONBuilder_Bool(Dqn_JSONBuilder *builder, bool value)
{
    Dqn_JSONBuilder_BoolNamed(builder, DQN_STRING8("") /*key*/, value);
}
#endif // !defined(DQN_NO_JSON_BUILDER)


// ---------------------------------+-----------------------------+---------------------------------
// [SECT-FNV1] Dqn_FNV1A            |                             | Hash(x) -> 32/64bit via FNV1a
// ---------------------------------+-----------------------------+---------------------------------
// Default values recommended by: http://isthe.com/chongo/tech/comp/fnv/
DQN_API uint32_t Dqn_FNV1A32_Iterate(void const *bytes, Dqn_isize size, uint32_t hash)
{
    auto buffer = DQN_CAST(uint8_t const *)bytes;
    for (Dqn_isize i = 0; i < size; i++)
        hash = (buffer[i] ^ hash) * 16777619 /*FNV Prime*/;
    return hash;
}

DQN_API uint32_t Dqn_FNV1A32_Hash(void const *bytes, Dqn_isize size)
{
    uint32_t result = Dqn_FNV1A32_Iterate(bytes, size, DQN_FNV1A32_SEED);
    return result;
}

DQN_API uint64_t Dqn_FNV1A64_Iterate(void const *bytes, Dqn_isize size, uint64_t hash)
{
    auto buffer = DQN_CAST(uint8_t const *)bytes;
    for (Dqn_isize i = 0; i < size; i++)
        hash = (buffer[i] ^ hash) * 1099511628211 /*FNV Prime*/;
    return hash;
}

DQN_API uint64_t Dqn_FNV1A64_Hash(void const *bytes, Dqn_isize size)
{
    uint64_t result = Dqn_FNV1A64_Iterate(bytes, size, DQN_FNV1A64_SEED);
    return result;
}

// ---------------------------------+-----------------------------+---------------------------------
// [SECT-MMUR] Dqn_MurmurHash3      |                             | Hash(x) -> 32/128bit via MurmurHash3
// ---------------------------------+-----------------------------+---------------------------------
#if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    #define DQN_MMH3_ROTL32(x, y) _rotl(x, y)
    #define DQN_MMH3_ROTL64(x, y) _rotl64(x, y)
#else
    #define DQN_MMH3_ROTL32(x, y) ((x) << (y)) | ((x) >> (32 - (y)))
    #define DQN_MMH3_ROTL64(x, y) ((x) << (y)) | ((x) >> (64 - (y)))
#endif

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

DQN_FORCE_INLINE uint32_t Dqn_MurmurHash3_GetBlock32(uint32_t const *p, int i)
{
    return p[i];
}

DQN_FORCE_INLINE uint64_t Dqn_MurmurHash3_GetBlock64(uint64_t const *p, int i)
{
    return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

DQN_FORCE_INLINE uint32_t Dqn_MurmurHash3_FMix32(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

DQN_FORCE_INLINE uint64_t Dqn_MurmurHash3_FMix64(uint64_t k)
{
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccd;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53;
    k ^= k >> 33;
    return k;
}

DQN_API uint32_t Dqn_MurmurHash3_x86U32(void const *key, int len, uint32_t seed)
{
    const uint8_t *data = (const uint8_t *)key;
    const int nblocks   = len / 4;

    uint32_t h1 = seed;

    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    //----------
    // body

    const uint32_t *blocks = (const uint32_t *)(data + nblocks * 4);

    for (int i = -nblocks; i; i++)
    {
        uint32_t k1 = Dqn_MurmurHash3_GetBlock32(blocks, i);

        k1 *= c1;
        k1 = DQN_MMH3_ROTL32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = DQN_MMH3_ROTL32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    //----------
    // tail

    const uint8_t *tail = (const uint8_t *)(data + nblocks * 4);

    uint32_t k1 = 0;

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

DQN_API Dqn_MurmurHash3 Dqn_MurmurHash3_x64U128(void const *key, int len, uint32_t seed)
{
    const uint8_t *data = (const uint8_t *)key;
    const int nblocks   = len / 16;

    uint64_t h1 = seed;
    uint64_t h2 = seed;

    const uint64_t c1 = 0x87c37b91114253d5;
    const uint64_t c2 = 0x4cf5ad432745937f;

    //----------
    // body

    const uint64_t *blocks = (const uint64_t *)(data);

    for (int i = 0; i < nblocks; i++)
    {
        uint64_t k1 = Dqn_MurmurHash3_GetBlock64(blocks, i * 2 + 0);
        uint64_t k2 = Dqn_MurmurHash3_GetBlock64(blocks, i * 2 + 1);

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

    const uint8_t *tail = (const uint8_t *)(data + nblocks * 16);

    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch (len & 15)
    {
        case 15:
            k2 ^= ((uint64_t)tail[14]) << 48;
        case 14:
            k2 ^= ((uint64_t)tail[13]) << 40;
        case 13:
            k2 ^= ((uint64_t)tail[12]) << 32;
        case 12:
            k2 ^= ((uint64_t)tail[11]) << 24;
        case 11:
            k2 ^= ((uint64_t)tail[10]) << 16;
        case 10:
            k2 ^= ((uint64_t)tail[9]) << 8;
        case 9:
            k2 ^= ((uint64_t)tail[8]) << 0;
            k2 *= c2;
            k2 = DQN_MMH3_ROTL64(k2, 33);
            k2 *= c1;
            h2 ^= k2;

        case 8:
            k1 ^= ((uint64_t)tail[7]) << 56;
        case 7:
            k1 ^= ((uint64_t)tail[6]) << 48;
        case 6:
            k1 ^= ((uint64_t)tail[5]) << 40;
        case 5:
            k1 ^= ((uint64_t)tail[4]) << 32;
        case 4:
            k1 ^= ((uint64_t)tail[3]) << 24;
        case 3:
            k1 ^= ((uint64_t)tail[2]) << 16;
        case 2:
            k1 ^= ((uint64_t)tail[1]) << 8;
        case 1:
            k1 ^= ((uint64_t)tail[0]) << 0;
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

    Dqn_MurmurHash3 result = {};
    result.e[0]            = h1;
    result.e[1]            = h2;
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
/// @endcond
