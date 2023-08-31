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

DQN_API void Dqn_ASAN_PoisonMemoryRegion(void const volatile *ptr, Dqn_usize size);
DQN_API void Dqn_ASAN_UnpoisonMemoryRegion(void const volatile *ptr, Dqn_usize size);

// NOTE: [$STKT] Dqn_StackTrace ====================================================================
// Create a stack trace at the calling site that these functions are invoked
// from.
//
// NOTE: API =======================================================================================
//   @proc Dqn_StackTrace_Walk
//   @desc This functions generates the stack trace as a series of U64's that
//   represent the base address of the functions on the call-stack at the point
//   of execution. These functions are stored in order from the current
//   executing function first and the most ancestor function last in the walk.

//   @proc Dqn_StackTrace_WalkResultIterate
//   @desc Create an iterator to iterate the walk result and produce
//   `Dqn_StackTraceRawFrame` from each level of the call-stack. These frames
//   can then be converted into `Dqn_StackTraceFrame` which is a human readable
//   representation of the frame.
#if 0
    Dqn_ThreadScratch scratch     = Dqn_Thread_GetScratch(nullptr);
    Dqn_StackTraceWalkResult walk = Dqn_StackTrace_Walk(scratch.arena, 128 /*limit*/);
    for (Dqn_StackTraceWalkResultIterator it = {}; Dqn_StackTrace_WalkResultIterate(&it, &walk); ) {
        Dqn_StackTraceFrame frame = Dqn_StackTrace_RawFrameToFrame(scratch.arena, it.raw_frame);
        Dqn_Print_LnF("%.*s(%I64u): %.*s", DQN_STRING_FMT(frame.file_name), frame.line_number, DQN_STRING_FMT(frame.function_name));
    }
#endif

//   @proc Dqn_StackTrace_GetFrames
//   @desc Create a stack trace at the point of execution and return the frames
//   converted into its human readable format.

//   @proc Dqn_StackTrace_RawFrameToFrame
//   @desc Convert a raw frame from a stack trace walk into the human readable
//   format (e.g. with function names, line number and file name).

struct Dqn_StackTraceFrame
{
    uint64_t    address;
    uint64_t    line_number;
    Dqn_String8 file_name;
    Dqn_String8 function_name;
};

struct Dqn_StackTraceRawFrame
{
    void     *process;
    uint64_t  base_addr;
};

struct Dqn_StackTraceWalkResult
{
    void     *process;   // [Internal] Windows handle to the process
    uint64_t *base_addr; // The addresses of the functions in the stack trace
    uint16_t  size;      // The number of `base_addr`'s stored from the walk
};

struct Dqn_StackTraceWalkResultIterator
{
    Dqn_StackTraceRawFrame raw_frame;
    uint16_t               index;
};

DQN_API Dqn_StackTraceWalkResult       Dqn_StackTrace_Walk             (Dqn_Arena *arena, uint16_t limit);
DQN_API bool                           Dqn_StackTrace_WalkResultIterate(Dqn_StackTraceWalkResultIterator *it, Dqn_StackTraceWalkResult *walk);
DQN_API Dqn_Slice<Dqn_StackTraceFrame> Dqn_StackTrace_GetFrames        (Dqn_Arena *arena, uint16_t limit);
DQN_API void                           Dqn_StackTrace_Print            (uint16_t limit);
DQN_API Dqn_StackTraceFrame            Dqn_StackTrace_RawFrameToFrame  (Dqn_Arena *arena, Dqn_StackTraceRawFrame raw_frame);

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
