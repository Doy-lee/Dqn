// NOTE: Preprocessor Token Tricks =================================================================
#define DQN_STRINGIFY(x) #x
#define DQN_TOKEN_COMBINE2(x, y) x ## y
#define DQN_TOKEN_COMBINE(x, y) DQN_TOKEN_COMBINE2(x, y)

// NOTE: [$CMAC] Compiler macros ===================================================================
// NOTE: Warning! Order is important here, clang-cl on Windows defines _MSC_VER
#if defined(_MSC_VER)
    #if defined(__clang__)
        #define DQN_COMPILER_CLANG_CL
        #define DQN_COMPILER_CLANG
    #else
        #define DQN_COMPILER_MSVC
    #endif
#elif defined(__clang__)
    #define DQN_COMPILER_CLANG
#elif defined(__GNUC__)
    #define DQN_COMPILER_GCC
#endif

// NOTE: Declare struct literals that work in both C and C++ because the syntax 
// is different between languages.
#if 0
    struct Foo { int a; }
    struct Foo foo = DQN_LITERAL(Foo){32}; // Works on both C and C++
#endif

#if defined(__cplusplus)
    #define DQN_LITERAL(T) T
#else
    #define DQN_LITERAL(T) (T)
#endif

#if defined(__cplusplus)
    #define DQN_THREAD_LOCAL thread_local
#else
    #define DQN_THREAD_LOCAL _Thread_local
#endif

#if defined(_WIN32)
    #define DQN_OS_WIN32
#elif defined(__linux__)
    #define DQN_OS_UNIX
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
    #define DQN_PLATFORM_ARM64
#elif defined(__EMSCRIPTEN__)
    #define DQN_PLATFORM_EMSCRIPTEN
#endif

#if defined(DQN_COMPILER_MSVC) || defined(DQN_COMPILER_CLANG_CL)
    #if defined(_CRT_SECURE_NO_WARNINGS)
        #define DQN_CRT_SECURE_NO_WARNINGS_PREVIOUSLY_DEFINED
    #else
        #define _CRT_SECURE_NO_WARNINGS
    #endif
#endif

#if defined(DQN_COMPILER_MSVC)
    #define DQN_FMT_ATTRIB _Printf_format_string_
    #define DQN_MSVC_WARNING_PUSH __pragma(warning(push))
    #define DQN_MSVC_WARNING_DISABLE(...) __pragma(warning(disable: ##__VA_ARGS__))
    #define DQN_MSVC_WARNING_POP __pragma(warning(pop))
#else
    #define DQN_FMT_ATTRIB
    #define DQN_MSVC_WARNING_PUSH
    #define DQN_MSVC_WARNING_DISABLE(...)
    #define DQN_MSVC_WARNING_POP
#endif

#if defined(DQN_COMPILER_CLANG) || defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG_CL)
    #define DQN_GCC_WARNING_PUSH _Pragma("GCC diagnostic push")
    #define DQN_GCC_WARNING_DISABLE_HELPER_0(x) #x
    #define DQN_GCC_WARNING_DISABLE_HELPER_1(y) DQN_GCC_WARNING_DISABLE_HELPER_0(GCC diagnostic ignored #y)
    #define DQN_GCC_WARNING_DISABLE(warning) _Pragma(DQN_GCC_WARNING_DISABLE_HELPER_1(warning))
    #define DQN_GCC_WARNING_POP _Pragma("GCC diagnostic pop")
#else
    #define DQN_GCC_WARNING_PUSH
    #define DQN_GCC_WARNING_DISABLE(...)
    #define DQN_GCC_WARNING_POP
#endif

// NOTE: [$MACR] Macros ============================================================================
#define DQN_FOR_UINDEX(index, size) for (Dqn_usize index = 0; index < size; index++)
#define DQN_FOR_IINDEX(index, size) for (Dqn_isize index = 0; index < size; index++)

#define Dqn_AlignUpPowerOfTwo(value, pot) (((uintptr_t)(value) + ((uintptr_t)(pot) - 1)) & ~((uintptr_t)(pot) - 1))
#define Dqn_AlignDownPowerOfTwo(value, pot) ((uintptr_t)(value) & ((uintptr_t)(pot) - 1))
#define Dqn_IsPowerOfTwo(value) ((((uintptr_t)(value)) & (((uintptr_t)(value)) - 1)) == 0)
#define Dqn_IsPowerOfTwoAligned(value, pot) ((((uintptr_t)value) & (((uintptr_t)pot) - 1)) == 0)

// NOTE: Alloc Macros ==============================================================================
#if !defined(DQN_ALLOC)
    #if defined(DQN_PLATFORM_EMSCRIPTEN)
        #define DQN_ALLOC(size) malloc(size)
    #else
        #define DQN_ALLOC(size) Dqn_VMem_Reserve(size, Dqn_VMemCommit_Yes, Dqn_VMemPage_ReadWrite)
    #endif
#endif

#if !defined(DQN_DEALLOC)
    #if defined(DQN_PLATFORM_EMSCRIPTEN)
        #define DQN_DEALLOC(ptr, size) free(ptr)
    #else
        #define DQN_DEALLOC(ptr, size) Dqn_VMem_Release(ptr, size)
    #endif
#endif

// NOTE: String.h Dependencies =====================================================================
#if !defined(DQN_MEMCPY) || !defined(DQN_MEMSET) || !defined(DQN_MEMCMP) || !defined(DQN_MEMMOVE)
    #include <string.h>
    #if !defined(DQN_MEMCPY)
        #define DQN_MEMCPY(dest, src, count) memcpy(dest, src, count)
    #endif
    #if !defined(DQN_MEMSET)
        #define DQN_MEMSET(dest, value, count) memset(dest, value, count)
    #endif
    #if !defined(DQN_MEMCMP)
        #define DQN_MEMCMP(lhs, rhs, count) memcmp(lhs, rhs, count)
    #endif
    #if !defined(DQN_MEMMOVE)
        #define DQN_MEMMOVE(dest, src, count) memmove(dest, src, count)
    #endif
#endif

// NOTE: Math.h Dependencies =======================================================================
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

#if !defined(DQN_OS_WIN32)
#include <stdlib.h> // exit()
#endif

// NOTE: Math Macros ===============================================================================
#define DQN_PI 3.14159265359f

#define DQN_DEGREE_TO_RADIAN(degrees) ((degrees) * (DQN_PI / 180.0f))
#define DQN_RADIAN_TO_DEGREE(radians) ((radians) * (180.f * DQN_PI))

#define DQN_ABS(val) (((val) < 0) ? (-(val)) : (val))
#define DQN_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define DQN_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define DQN_CLAMP(val, lo, hi) DQN_MAX(DQN_MIN(val, hi), lo)
#define DQN_SQUARED(val) ((val) * (val))

#define DQN_SWAP(a, b)   \
    do                   \
    {                    \
        auto temp = a;   \
        a        = b;    \
        b        = temp; \
    } while (0)

// NOTE: Function/Variable Annotations =============================================================
#if defined(DQN_STATIC_API)
    #define DQN_API static
#else
    #define DQN_API
#endif

#define DQN_LOCAL_PERSIST static
#define DQN_FILE_SCOPE static
#define DQN_CAST(val) (val)

#if defined(DQN_COMPILER_MSVC) || defined(DQN_COMPILER_CLANG_CL)
    #define DQN_FORCE_INLINE __forceinline
#else
    #define DQN_FORCE_INLINE inline __attribute__((always_inline))
#endif

// NOTE: Size Macros ===============================================================================
#define DQN_ISIZEOF(val) DQN_CAST(ptrdiff_t)sizeof(val)
#define DQN_ARRAY_UCOUNT(array) (sizeof(array)/(sizeof((array)[0])))
#define DQN_ARRAY_ICOUNT(array) (Dqn_isize)DQN_ARRAY_UCOUNT(array)
#define DQN_CHAR_COUNT(string) (sizeof(string) - 1)

// NOTE: SI Byte Macros ============================================================================
#define DQN_BYTES(val) (val)
#define DQN_KILOBYTES(val) (1024ULL * DQN_BYTES(val))
#define DQN_MEGABYTES(val) (1024ULL * DQN_KILOBYTES(val))
#define DQN_GIGABYTES(val) (1024ULL * DQN_MEGABYTES(val))

// NOTE: Time Macros ===============================================================================
#define DQN_SECONDS_TO_MS(val) ((val) * 1000)
#define DQN_MINS_TO_S(val) ((val) * 60ULL)
#define DQN_HOURS_TO_S(val) (DQN_MINS_TO_S(val) * 60ULL)
#define DQN_DAYS_TO_S(val) (DQN_HOURS_TO_S(val) * 24ULL)
#define DQN_YEARS_TO_S(val) (DQN_DAYS_TO_S(val) * 365ULL)

// NOTE: Debug Break ===============================================================================
#if !defined(DQN_DEBUG_BREAK)
    #if defined(NDEBUG)
        #define DQN_DEBUG_BREAK
    #else
        #if defined(DQN_COMPILER_MSVC) || defined(DQN_COMPILER_CLANG_CL)
            #define DQN_DEBUG_BREAK __debugbreak()
        #elif defined(DQN_COMPILER_CLANG)
            #define DQN_DEBUG_BREAK __builtin_debugtrap()
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

// NOTE: Assert Macros =============================================================================
#define DQN_HARD_ASSERT(expr) DQN_HARD_ASSERTF(expr, "")
#define DQN_HARD_ASSERTF(expr, fmt, ...)                                           \
    if (!(expr)) {                                                                 \
        Dqn_Log_ErrorF("Hard assert triggered [" #expr "]. " fmt, ##__VA_ARGS__);  \
        Dqn_StackTrace_Print(128 /*limit*/);                                       \
        DQN_DEBUG_BREAK;                                                           \
    }

#if defined(DQN_NO_ASSERT)
    #define DQN_ASSERTF(...)
    #define DQN_ASSERT(...)
#else
    #define DQN_ASSERT(expr) DQN_ASSERTF(expr, "")
    #define DQN_ASSERTF(expr, fmt, ...)                                           \
        if (!(expr)) {                                                            \
            Dqn_Log_ErrorF("Assert triggered [" #expr "]. " fmt, ##__VA_ARGS__);  \
            Dqn_StackTrace_Print(128 /*limit*/);                                  \
            DQN_DEBUG_BREAK;                                                      \
        }
#endif

#define DQN_INVALID_CODE_PATHF(fmt, ...) DQN_ASSERTF(0, fmt, ##__VA_ARGS__)
#define DQN_INVALID_CODE_PATH DQN_INVALID_CODE_PATHF("Invalid code path triggered")

// NOTE: Check macro ===============================================================================
// Check the expression trapping in debug, whilst in release- trapping is
// removed and the expression is evaluated as if it were a normal 'if' branch.
//
// This allows handling of the condition gracefully when compiled out but traps 
// to notify the developer in builds when it's compiled in.
#if 0
    bool flag = true;
    if (DQN_CHECKF(flag, "Flag was false!")) {
        // This branch will execute!
    } else {
        // Prints "Flag was false!"
    }
#endif
#define DQN_CHECK(expr) DQN_CHECKF(expr, "")

#if defined(DQN_NO_CHECK_BREAK)
    #define DQN_CHECKF(expr, fmt, ...) \
        ((expr) ? true : (Dqn_Log_TypeFCallSite(Dqn_LogType_Warning, DQN_CALL_SITE, fmt, ## __VA_ARGS__), false))
#else
    #define DQN_CHECKF(expr, fmt, ...) \
        ((expr) ? true : (Dqn_Log_TypeFCallSite(Dqn_LogType_Error, DQN_CALL_SITE, fmt, ## __VA_ARGS__), DQN_DEBUG_BREAK, false))
#endif

// NOTE: Zero initialisation macro =================================================================
#if defined(__cplusplus)
    #define DQN_ZERO_INIT {}
#else
    #define DQN_ZERO_INIT {0}
#endif

// NOTE: Defer Macro ===============================================================================
#if defined(__cplusplus)
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

#define DQN_UNIQUE_NAME(prefix) DQN_TOKEN_COMBINE(prefix, __LINE__)
#define DQN_DEFER const auto DQN_UNIQUE_NAME(defer_lambda_) = Dqn_DeferHelper() + [&]()
#endif // defined(__cplusplus)

#define DQN_DEFER_LOOP(begin, end)                   \
    for (bool DQN_UNIQUE_NAME(once) = (begin, true); \
         DQN_UNIQUE_NAME(once);                      \
         end, DQN_UNIQUE_NAME(once) = false)

// NOTE: [$TYPE] Types =============================================================================
typedef intptr_t     Dqn_isize;
typedef uintptr_t    Dqn_usize;
typedef intptr_t     Dqn_isize;
typedef float        Dqn_f32;
typedef double       Dqn_f64;
typedef unsigned int Dqn_uint;
typedef int32_t      Dqn_b32;

#define DQN_F32_MAX   3.402823466e+38F
#define DQN_F32_MIN   1.175494351e-38F
#define DQN_F64_MAX   1.7976931348623158e+308
#define DQN_F64_MIN   2.2250738585072014e-308
#define DQN_USIZE_MAX UINTPTR_MAX
#define DQN_ISIZE_MAX INTPTR_MAX
#define DQN_ISIZE_MIN INTPTR_MIN

enum Dqn_ZeroMem
{
    Dqn_ZeroMem_No,  // Memory can be handed out without zero-ing it out
    Dqn_ZeroMem_Yes, // Memory should be zero-ed out before giving to the callee
};

struct Dqn_Str8
{
    char      *data; // The bytes of the string
    Dqn_usize  size; // The number of bytes in the string

    char const *begin() const { return data; }
    char const *end  () const { return data + size; }
    char       *begin()       { return data; }
    char       *end  ()       { return data + size; }
};

#if !defined(DQN_NO_SLICE)
template <typename T> struct Dqn_Slice // A pointer and length container of data
{
    T         *data;
    Dqn_usize  size;

    T       *begin()       { return data; }
    T       *end  ()       { return data + size; }
    T const *begin() const { return data; }
    T const *end  () const { return data + size; }
};
#endif

// NOTE: [$CALL] Dqn_CallSite ======================================================================
struct Dqn_CallSite
{
    Dqn_Str8     file;
    Dqn_Str8     function;
    unsigned int line;
};
#define DQN_CALL_SITE Dqn_CallSite{DQN_STR8(__FILE__), DQN_STR8(__func__), __LINE__}

// NOTE: [$INTR] Intrinsics ========================================================================
// Platform agnostic functions for CPU level instructions like atomics, barriers
// and timestamp counters.
//
// NOTE: API
// @proc Dqn_Atomic_SetValue64, Dqn_Atomic_SetValue32
//   @desc Atomically set the value into the target using an atomic compare and 
//   swap.
//   @param[in,out] target The target pointer to set atomically
//   @param[in] value The value to set atomically into the target
//   @return The value that was last stored in the target

// @proc Dqn_CPUID
//   Execute 'CPUID' instruction to query the capabilities of the current CPU.

// NOTE: Dqn_Atomic_Add/Exchange return the previous value store in the target
#if defined(DQN_COMPILER_MSVC) || defined(DQN_COMPILER_CLANG_CL)
    #include <intrin.h>
    #define Dqn_Atomic_CompareExchange64(dest, desired_val, prev_val) _InterlockedCompareExchange64((__int64 volatile *)dest, desired_val, prev_val)
    #define Dqn_Atomic_CompareExchange32(dest, desired_val, prev_val) _InterlockedCompareExchange((long volatile *)dest, desired_val, prev_val)
    #define Dqn_Atomic_AddU32(target, value)                          _InterlockedExchangeAdd((long volatile *)target, value)
    #define Dqn_Atomic_AddU64(target, value)                          _InterlockedExchangeAdd64((__int64 volatile *)target, value)
    #define Dqn_Atomic_SubU32(target, value)                          Dqn_Atomic_AddU32(DQN_CAST(long volatile *)target, (long)-value)
    #define Dqn_Atomic_SubU64(target, value)                          Dqn_Atomic_AddU64(target, (uint64_t)-value)
    #define Dqn_CPU_TSC()                                             __rdtsc()
    #define Dqn_CompilerReadBarrierAndCPUReadFence                    _ReadBarrier(); _mm_lfence()
    #define Dqn_CompilerWriteBarrierAndCPUWriteFence                  _WriteBarrier(); _mm_sfence()
#elif defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
    #if defined(__ANDROID__)
    #elif defined(DQN_PLATFORM_EMSCRIPTEN)
        #include <emmintrin.h>
    #else
        #include <x86intrin.h>
    #endif

    #define Dqn_Atomic_AddU32(target, value) __atomic_fetch_add(target, value, __ATOMIC_ACQ_REL)
    #define Dqn_Atomic_AddU64(target, value) __atomic_fetch_add(target, value, __ATOMIC_ACQ_REL)
    #define Dqn_Atomic_SubU32(target, value) __atomic_fetch_sub(target, value, __ATOMIC_ACQ_REL)
    #define Dqn_Atomic_SubU64(target, value) __atomic_fetch_sub(target, value, __ATOMIC_ACQ_REL)
    #if defined(DQN_COMPILER_GCC)
        #define Dqn_CPU_TSC() __rdtsc()
    #else
        #define Dqn_CPU_TSC() __builtin_readcyclecounter()
    #endif
    #define Dqn_CompilerReadBarrierAndCPUReadFence   asm volatile("lfence" ::: "memory")
    #define Dqn_CompilerWriteBarrierAndCPUWriteFence asm volatile("sfence" ::: "memory")
#else
    #error "Compiler not supported"
#endif

DQN_FORCE_INLINE uint64_t Dqn_Atomic_SetValue64(uint64_t volatile *target, uint64_t value)
{
    #if defined(DQN_COMPILER_MSVC) || defined(DQN_COMPILER_CLANG_CL)
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

DQN_FORCE_INLINE long Dqn_Atomic_SetValue32(long volatile *target, long value)
{
    #if defined(DQN_COMPILER_MSVC) || defined(DQN_COMPILER_CLANG_CL)
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

#if !defined(DQN_PLATFORM_ARM64)
struct Dqn_CPUIDRegisters
{
    Dqn_uint array[4]; ///< Values from 'CPUID' instruction for each register (EAX, EBX, ECX, EDX)
};

Dqn_CPUIDRegisters Dqn_CPUID(int function_id);
#endif // DQN_PLATFORM_ARM64

// NOTE: [$TMUT] Dqn_TicketMutex ===================================================================
//
// A mutex implemented using an atomic compare and swap on tickets handed out
// for each critical section.
//
// This mutex serves ticket in order and will block all other threads until the
// tickets are returned in order. The thread with the oldest ticket that has
// not been returned has right of way to execute, all other threads will be
// blocked in an atomic compare and swap loop. block execution by going into an
// atomic
//
// When a thread is blocked by this mutex, a spinlock intrinsic `_mm_pause` is
// used to yield the CPU and reduce spinlock on the thread. This mutex is not
// ideal for long blocking operations. This mutex does not issue any syscalls
// and relies entirely on atomic instructions.
//
// NOTE: API
//
// @proc Dqn_TicketMutex_Begin, End
//   @desc Lock and unlock the mutex respectively

// @proc Dqn_TicketMutex_MakeTicket
//   @desc Allocate the next available ticket from the mutex for locking using
//   Dqn_TicketMutex_BeginTicket().
//   @param[in] mutex The mutex
#if 0
    Dqn_TicketMutex mutex = {};
    unsigned int ticket   = Dqn_TicketMutex_MakeTicket(&mutex);
    Dqn_TicketMutex_BeginTicket(&mutex, ticket); // Blocking call until we attain the lock
    Dqn_TicketMutex_End(&mutex);
#endif

// @proc Dqn_TicketMutex_BeginTicket
//   @desc Lock the mutex using the given ticket if possible, otherwise block
//   waiting until the mutex can be locked.

// @proc Dqn_TicketMutex_CanLock
//   @desc Determine if the mutex can be locked using the given ticket number

struct Dqn_TicketMutex
{
    unsigned int volatile ticket;  // The next ticket to give out to the thread taking the mutex
    unsigned int volatile serving; // The ticket ID to block the mutex on until it is returned
};

void     Dqn_TicketMutex_Begin      (Dqn_TicketMutex *mutex);
void     Dqn_TicketMutex_End        (Dqn_TicketMutex *mutex);
Dqn_uint Dqn_TicketMutex_MakeTicket (Dqn_TicketMutex *mutex);
void     Dqn_TicketMutex_BeginTicket(Dqn_TicketMutex const *mutex, Dqn_uint ticket);
bool     Dqn_TicketMutex_CanLock    (Dqn_TicketMutex const *mutex, Dqn_uint ticket);

// NOTE: [$ALLO] Dqn_Allocator =====================================================================
typedef void *Dqn_Allocator_AllocProc(size_t size, uint8_t align, Dqn_ZeroMem zero_mem, void *user_context);
typedef void  Dqn_Allocator_DeallocProc(void *ptr, size_t size, void *user_context);

struct Dqn_Allocator
{
    void                      *user_context; // User assigned pointer that is passed into the allocator functions
    Dqn_Allocator_AllocProc   *alloc;        // Memory allocating routine
    Dqn_Allocator_DeallocProc *dealloc;      // Memory deallocating routine
};

// NOTE: Macros ====================================================================================
#define Dqn_Allocator_NewArray(allocator, Type, count, zero_mem) (Type *)Dqn_Allocator_Alloc(allocator, sizeof(Type) * count, alignof(Type), zero_mem)
#define Dqn_Allocator_New(allocator, Type, zero_mem) (Type *)Dqn_Allocator_Alloc(allocator, sizeof(Type), alignof(Type), zero_mem)

// NOTE: API =======================================================================================
void   *Dqn_Allocator_Alloc  (Dqn_Allocator allocator, size_t size, uint8_t align, Dqn_ZeroMem zero_mem);
void    Dqn_Allocator_Dealloc(Dqn_Allocator allocator, void *ptr, size_t size);

// NOTE: [$PRIN] Dqn_Print =========================================================================
enum Dqn_PrintStd
{
    Dqn_PrintStd_Out,
    Dqn_PrintStd_Err,
};

enum Dqn_PrintBold
{
    Dqn_PrintBold_No,
    Dqn_PrintBold_Yes,
};

struct Dqn_PrintStyle
{
    Dqn_PrintBold bold;
    bool          colour;
    uint8_t       r, g, b;
};

enum Dqn_PrintESCColour
{
    Dqn_PrintESCColour_Fg,
    Dqn_PrintESCColour_Bg,
};

// NOTE: Print Style ===============================================================================
DQN_API Dqn_PrintStyle Dqn_Print_StyleColour       (uint8_t r, uint8_t g, uint8_t b, Dqn_PrintBold bold);
DQN_API Dqn_PrintStyle Dqn_Print_StyleColourU32    (uint32_t rgb, Dqn_PrintBold bold);
DQN_API Dqn_PrintStyle Dqn_Print_StyleBold         ();

// NOTE: Print Standard Out ========================================================================
#define                Dqn_Print(string)                        Dqn_Print_Std(Dqn_PrintStd_Out, string)
#define                Dqn_Print_F(fmt, ...)                    Dqn_Print_StdF(Dqn_PrintStd_Out, fmt, ## __VA_ARGS__)
#define                Dqn_Print_FV(fmt, args)                  Dqn_Print_StdFV(Dqn_PrintStd_Out, fmt, args)

#define                Dqn_Print_Style(style, string)           Dqn_Print_StdStyle(Dqn_PrintStd_Out, style, string)
#define                Dqn_Print_FStyle(style, fmt, ...)        Dqn_Print_StdFStyle(Dqn_PrintStd_Out, style, fmt, ## __VA_ARGS__)
#define                Dqn_Print_FVStyle(style, fmt, args, ...) Dqn_Print_StdFVStyle(Dqn_PrintStd_Out, style, fmt, args)

#define                Dqn_Print_Ln(string)                     Dqn_Print_StdLn(Dqn_PrintStd_Out, string)
#define                Dqn_Print_LnF(fmt, ...)                  Dqn_Print_StdLnF(Dqn_PrintStd_Out, fmt, ## __VA_ARGS__)
#define                Dqn_Print_LnFV(fmt, args)                Dqn_Print_StdLnFV(Dqn_PrintStd_Out, fmt, args)

#define                Dqn_Print_LnStyle(style, string)         Dqn_Print_StdLnStyle(Dqn_PrintStd_Out, style, string);
#define                Dqn_Print_LnFStyle(style, fmt, ...)      Dqn_Print_StdLnFStyle(Dqn_PrintStd_Out, style, fmt, ## __VA_ARGS__);
#define                Dqn_Print_LnFVStyle(style, fmt, args)    Dqn_Print_StdLnFVStyle(Dqn_PrintStd_Out, style, fmt, args);

#define                Dqn_Print_Err(string)                       Dqn_Print_Std(Dqn_PrintStd_Err, string)
#define                Dqn_Print_ErrF(fmt, ...)                    Dqn_Print_StdF(Dqn_PrintStd_Err, fmt, ## __VA_ARGS__)
#define                Dqn_Print_ErrFV(fmt, args)                  Dqn_Print_StdFV(Dqn_PrintStd_Err, fmt, args)

#define                Dqn_Print_ErrStyle(style, string)           Dqn_Print_StdStyle(Dqn_PrintStd_Err, style, string)
#define                Dqn_Print_ErrFStyle(style, fmt, ...)        Dqn_Print_StdFStyle(Dqn_PrintStd_Err, style, fmt, ## __VA_ARGS__)
#define                Dqn_Print_ErrFVStyle(style, fmt, args, ...) Dqn_Print_StdFVStyle(Dqn_PrintStd_Err, style, fmt, args)

#define                Dqn_Print_ErrLn(string)                     Dqn_Print_StdLn(Dqn_PrintStd_Err, string)
#define                Dqn_Print_ErrLnF(fmt, ...)                  Dqn_Print_StdLnF(Dqn_PrintStd_Err, fmt, ## __VA_ARGS__)
#define                Dqn_Print_ErrLnFV(fmt, args)                Dqn_Print_StdLnFV(Dqn_PrintStd_Err, fmt, args)

#define                Dqn_Print_ErrLnStyle(style, string)         Dqn_Print_StdLnStyle(Dqn_PrintStd_Err, style, string);
#define                Dqn_Print_ErrLnFStyle(style, fmt, ...)      Dqn_Print_StdLnFStyle(Dqn_PrintStd_Err, style, fmt, ## __VA_ARGS__);
#define                Dqn_Print_ErrLnFVStyle(style, fmt, args)    Dqn_Print_StdLnFVStyle(Dqn_PrintStd_Err, style, fmt, args);


// NOTE: Print =====================================================================================
DQN_API void           Dqn_Print_Std               (Dqn_PrintStd std_handle, Dqn_Str8 string);
DQN_API void           Dqn_Print_StdF              (Dqn_PrintStd std_handle, DQN_FMT_ATTRIB char const *fmt, ...);
DQN_API void           Dqn_Print_StdFV             (Dqn_PrintStd std_handle, DQN_FMT_ATTRIB char const *fmt, va_list args);

DQN_API void           Dqn_Print_StdStyle          (Dqn_PrintStd std_handle, Dqn_PrintStyle style, Dqn_Str8 string);
DQN_API void           Dqn_Print_StdFStyle         (Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_ATTRIB char const *fmt, ...);
DQN_API void           Dqn_Print_StdFVStyle        (Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_ATTRIB char const *fmt, va_list args);

DQN_API void           Dqn_Print_StdLn             (Dqn_PrintStd std_handle, Dqn_Str8 string);
DQN_API void           Dqn_Print_StdLnF            (Dqn_PrintStd std_handle, DQN_FMT_ATTRIB char const *fmt, ...);
DQN_API void           Dqn_Print_StdLnFV           (Dqn_PrintStd std_handle, DQN_FMT_ATTRIB char const *fmt, va_list args);

DQN_API void           Dqn_Print_StdLnStyle        (Dqn_PrintStd std_handle, Dqn_PrintStyle style, Dqn_Str8 string);
DQN_API void           Dqn_Print_StdLnFStyle       (Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_ATTRIB char const *fmt, ...);
DQN_API void           Dqn_Print_StdLnFVStyle      (Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_ATTRIB char const *fmt, va_list args);

// NOTE: ANSI Formatting Codes =====================================================================
Dqn_Str8               Dqn_Print_ESCColourStr8   (Dqn_PrintESCColour colour, uint8_t r, uint8_t g, uint8_t b);
Dqn_Str8               Dqn_Print_ESCColourU32Str8(Dqn_PrintESCColour colour, uint32_t value);

#define                Dqn_Print_ESCColourFgStr8(r, g, b)  Dqn_Print_ESCColourStr8(Dqn_PrintESCColour_Fg, r, g, b)
#define                Dqn_Print_ESCColourBgStr8(r, g, b)  Dqn_Print_ESCColourStr8(Dqn_PrintESCColour_Bg, r, g, b)
#define                Dqn_Print_ESCColourFg(r, g, b)      Dqn_Print_ESCColourStr8(Dqn_PrintESCColour_Fg, r, g, b).data
#define                Dqn_Print_ESCColourBg(r, g, b)      Dqn_Print_ESCColourStr8(Dqn_PrintESCColour_Bg, r, g, b).data

#define                Dqn_Print_ESCColourFgU32Str8(value) Dqn_Print_ESCColourU32Str8(Dqn_PrintESCColour_Fg, value)
#define                Dqn_Print_ESCColourBgU32Str8(value) Dqn_Print_ESCColourU32Str8(Dqn_PrintESCColour_Bg, value)
#define                Dqn_Print_ESCColourFgU32(value)     Dqn_Print_ESCColourU32Str8(Dqn_PrintESCColour_Fg, value).data
#define                Dqn_Print_ESCColourBgU32(value)     Dqn_Print_ESCColourU32Str8(Dqn_PrintESCColour_Bg, value).data

#define                Dqn_Print_ESCReset                  "\x1b[0m"
#define                Dqn_Print_ESCBold                   "\x1b[1m"
#define                Dqn_Print_ESCResetStr8              DQN_STR8(Dqn_Print_ESCReset)
#define                Dqn_Print_ESCBoldStr8               DQN_STR8(Dqn_Print_ESCBold)

// NOTE: [$LLOG] Dqn_Log  ==========================================================================
// NOTE: API
// @proc Dqn_LogProc
//   @desc The logging procedure of the library. Users can override the default
//   logging function by setting the logging function pointer in Dqn_Library.
//   This function will be invoked every time a log is recorded using the
//   following functions.
//
//   @param[in] log_type This value is one of the Dqn_LogType values if the log
//   was generated from one of the default categories. -1 if the log is not from
//   one of the default categories.

enum Dqn_LogType
{
    Dqn_LogType_Debug,
    Dqn_LogType_Info,
    Dqn_LogType_Warning,
    Dqn_LogType_Error,
    Dqn_LogType_Count,
};

/// RGBA
#define Dqn_LogTypeColourU32_Info    0x00'87'ff'ff // Blue
#define Dqn_LogTypeColourU32_Warning 0xff'ff'00'ff // Yellow
#define Dqn_LogTypeColourU32_Error   0xff'00'00'ff // Red

typedef void Dqn_LogProc(Dqn_Str8 type, int log_type, void *user_data, Dqn_CallSite call_site, DQN_FMT_ATTRIB char const *fmt, va_list va);

#define Dqn_Log_DebugF(fmt, ...)        Dqn_Log_TypeFCallSite(Dqn_LogType_Debug, DQN_CALL_SITE, fmt, ## __VA_ARGS__)
#define Dqn_Log_InfoF(fmt, ...)         Dqn_Log_TypeFCallSite(Dqn_LogType_Info, DQN_CALL_SITE, fmt, ## __VA_ARGS__)
#define Dqn_Log_WarningF(fmt, ...)      Dqn_Log_TypeFCallSite(Dqn_LogType_Warning, DQN_CALL_SITE, fmt, ## __VA_ARGS__)
#define Dqn_Log_ErrorF(fmt, ...)        Dqn_Log_TypeFCallSite(Dqn_LogType_Error, DQN_CALL_SITE, fmt, ## __VA_ARGS__)

#define Dqn_Log_DebugFV(fmt, args)      Dqn_Log_TypeFVCallSite(Dqn_LogType_Debug, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_InfoFV(fmt, args)       Dqn_Log_TypeFVCallSite(Dqn_LogType_Info, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_WarningFV(fmt, args)    Dqn_Log_TypeFVCallSite(Dqn_LogType_Warning, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_ErrorFV(fmt, args)      Dqn_Log_TypeFVCallSite(Dqn_LogType_Error, DQN_CALL_SITE, fmt, args)

#define Dqn_Log_TypeFV(type, fmt, args) Dqn_Log_TypeFVCallSite(type, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_TypeF(type, fmt, ...)   Dqn_Log_TypeFCallSite(type, DQN_CALL_SITE, fmt, ## __VA_ARGS__)

#define Dqn_Log_FV(type, fmt, args)     Dqn_Log_FVCallSite(type, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_F(type, fmt, ...)       Dqn_Log_FCallSite(type, DQN_CALL_SITE, fmt, ## __VA_ARGS__)

DQN_API Dqn_Str8 Dqn_Log_MakeStr8      (Dqn_Allocator allocator, bool colour, Dqn_Str8 type, int log_type, Dqn_CallSite call_site, DQN_FMT_ATTRIB char const *fmt, va_list args);
DQN_API void     Dqn_Log_TypeFVCallSite(Dqn_LogType type, Dqn_CallSite call_site, DQN_FMT_ATTRIB char const *fmt, va_list va);
DQN_API void     Dqn_Log_TypeFCallSite (Dqn_LogType type, Dqn_CallSite call_site, DQN_FMT_ATTRIB char const *fmt, ...);
DQN_API void     Dqn_Log_FVCallSite    (Dqn_Str8 type, Dqn_CallSite call_site, DQN_FMT_ATTRIB char const *fmt, va_list va);
DQN_API void     Dqn_Log_FCallSite     (Dqn_Str8 type, Dqn_CallSite call_site, DQN_FMT_ATTRIB char const *fmt, ...);
