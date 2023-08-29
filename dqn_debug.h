// NOTE: Debug Macros ==============================================================================
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

#if !defined(DQN_DUMP_STACK_TRACE)
    #define DQN_DUMP_STACK_TRACE                                                                  \
        do {                                                                                      \
            Dqn_String8 stack_trace_       = Dqn_String8_InitCString8(b_stacktrace_get_string()); \
            Dqn_String8 clean_stack_trace_ = Dqn_Debug_CleanStackTrace(stack_trace_);             \
            Dqn_Print_StdF(Dqn_PrintStd_Err, "%.*s", DQN_STRING_FMT(clean_stack_trace_));         \
            free(stack_trace_.data);                                                              \
        } while (0)
#endif

#if !defined(DQN_ASAN_POISON)
    #define DQN_ASAN_POISON 0
#endif

#if !defined(DQN_ASAN_POISON_VET)
    #define DQN_ASAN_POISON_VET 0
#endif

#define DQN_ASAN_POISON_ALIGNMENT 8
#if !defined(DQN_ASAN_POISON_GUARD_SIZE)
    #define DQN_ASAN_POISON_GUARD_SIZE 128
#endif
static_assert(Dqn_IsPowerOfTwoAligned(DQN_ASAN_POISON_GUARD_SIZE, DQN_ASAN_POISON_ALIGNMENT),
              "ASAN poison guard size must be a power-of-two and aligned to ASAN's alignment"
              "requirement (8 bytes)");

// NOTE: MSVC does not support the feature detection macro for instance so we
// compile it out
#if !defined(__has_feature)
    #define __has_feature(x) 0
#endif

// NOTE: [$ASAN] Dqn_Asan ==========================================================================
#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
    #include <sanitizer/asan_interface.h>
#endif

void Dqn_ASAN_PoisonMemoryRegion(void const volatile *ptr, Dqn_usize size);
void Dqn_ASAN_UnpoisonMemoryRegion(void const volatile *ptr, Dqn_usize size);

// NOTE: [$CALL] Dqn_CallSite ======================================================================
struct Dqn_CallSite
{
    Dqn_String8  file;
    Dqn_String8  function;
    unsigned int line;
};
#define DQN_CALL_SITE Dqn_CallSite{DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__}

// NOTE: [$DEBG] Dqn_Debug =========================================================================
enum Dqn_AllocRecordFlag
{
    Dqn_AllocRecordFlag_Freed         = 1 << 0,
    Dqn_AllocRecordFlag_LeakPermitted = 1 << 1,
};

struct Dqn_AllocRecord
{
    void         *ptr;                    // Pointer to the allocation being tracked
    Dqn_usize     size;                   // Size of the allocation
    Dqn_usize     freed_size;             // Store the size of the allocation when it is freed
    char         *stack_trace;            // Stack trace at the point of allocation
    char         *freed_stack_trace;      // Stack trace of where the allocation was freed
    uint16_t      stack_trace_size;       // Size of the `stack_trace`
    uint16_t      freed_stack_trace_size; // Size of `freed_stack_trace`
    uint16_t      flags;                  // Bit flags from `Dqn_AllocRecordFlag`
    char          padding[2];
};
static_assert(sizeof(Dqn_AllocRecord) == 48,
              "We aim to keep the allocation record as light as possible as "
              "memory tracking can get expensive. Enforce that there is no "
              "unexpected padding.");

DQN_API Dqn_String8 Dqn_Debug_CleanStackTrace(Dqn_String8 stack_trace);

#if defined(DQN_LEAK_TRACING)
#define Dqn_Debug_TrackAlloc(ptr, size, leak_permitted) Dqn_Debug_TrackAlloc_  (Dqn_String8_InitCString8(b_stacktrace_get_string()), ptr, size, leak_permitted)
#define Dqn_Debug_TrackDealloc(ptr)                     Dqn_Debug_TrackDealloc_(Dqn_String8_InitCString8(b_stacktrace_get_string()), ptr)

DQN_API void Dqn_Debug_TrackAlloc_(Dqn_String8 stack_trace, void *ptr, Dqn_usize size, bool leak_permitted);
DQN_API void Dqn_Debug_TrackDealloc_(Dqn_String8 stack_trace, void *ptr);
DQN_API void Dqn_Debug_DumpLeaks();
#else
#define      Dqn_Debug_TrackAlloc(...)
#define      Dqn_Debug_TrackDealloc(...)
#define      Dqn_Debug_DumpLeaks(...)
#endif

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

typedef void Dqn_LogProc(Dqn_String8 type, int log_type, void *user_data, Dqn_CallSite call_site, char const *fmt, va_list va);

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

DQN_API Dqn_String8 Dqn_Log_MakeString    (Dqn_Allocator allocator, bool colour, Dqn_String8 type, int log_type, Dqn_CallSite call_site, char const *fmt, va_list args);
DQN_API void        Dqn_Log_TypeFVCallSite(Dqn_LogType type, Dqn_CallSite call_site, char const *fmt, va_list va);
DQN_API void        Dqn_Log_TypeFCallSite (Dqn_LogType type, Dqn_CallSite call_site, char const *fmt, ...);
DQN_API void        Dqn_Log_FVCallSite    (Dqn_String8 type, Dqn_CallSite call_site, char const *fmt, va_list va);
DQN_API void        Dqn_Log_FCallSite     (Dqn_String8 type, Dqn_CallSite call_site, char const *fmt, ...);
