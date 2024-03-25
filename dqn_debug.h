/*
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   $$$$$$$\  $$$$$$$$\ $$$$$$$\  $$\   $$\  $$$$$$\
//   $$  __$$\ $$  _____|$$  __$$\ $$ |  $$ |$$  __$$\
//   $$ |  $$ |$$ |      $$ |  $$ |$$ |  $$ |$$ /  \__|
//   $$ |  $$ |$$$$$\    $$$$$$$\ |$$ |  $$ |$$ |$$$$\
//   $$ |  $$ |$$  __|   $$  __$$\ $$ |  $$ |$$ |\_$$ |
//   $$ |  $$ |$$ |      $$ |  $$ |$$ |  $$ |$$ |  $$ |
//   $$$$$$$  |$$$$$$$$\ $$$$$$$  |\$$$$$$  |\$$$$$$  |
//   \_______/ \________|\_______/  \______/  \______/
//
//   dqn_debug.h -- Tools for debugging
//
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// [$ASAN] Dqn_Asan       -- Helpers to manually poison memory using ASAN
// [$STKT] Dqn_StackTrace -- Create stack traces
// [$DEBG] Dqn_Debug      -- Allocation leak tracking API
//
////////////////////////////////////////////////////////////////////////////////////////////////////
*/

// NOTE: [$ASAN] Dqn_Asan //////////////////////////////////////////////////////////////////////////
#if !defined(DQN_ASAN_POISON)
    #define DQN_ASAN_POISON 0
#endif

#if !defined(DQN_ASAN_VET_POISON)
    #define DQN_ASAN_VET_POISON 0
#endif

#define DQN_ASAN_POISON_ALIGNMENT 8

#if !defined(DQN_ASAN_POISON_GUARD_SIZE)
    #define DQN_ASAN_POISON_GUARD_SIZE 128
#endif
static_assert(Dqn_IsPowerOfTwoAligned(DQN_ASAN_POISON_GUARD_SIZE, DQN_ASAN_POISON_ALIGNMENT),
              "ASAN poison guard size must be a power-of-two and aligned to ASAN's alignment"
              "requirement (8 bytes)");

#if DQN_HAS_FEATURE(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
    #include <sanitizer/asan_interface.h>
#endif

// NOTE: [$STKT] Dqn_StackTrace ////////////////////////////////////////////////////////////////////
struct Dqn_StackTraceFrame
{
    uint64_t address;
    uint64_t line_number;
    Dqn_Str8 file_name;
    Dqn_Str8 function_name;
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

// NOTE: [$DEBG] Dqn_Debug /////////////////////////////////////////////////////////////////////////
enum Dqn_DebugAllocFlag
{
    Dqn_DebugAllocFlag_Freed         = 1 << 0,
    Dqn_DebugAllocFlag_LeakPermitted = 1 << 1,
};

struct Dqn_DebugAlloc
{
    void      *ptr;               // 8  Pointer to the allocation being tracked
    Dqn_usize  size;              // 16 Size of the allocation
    Dqn_usize  freed_size;        // 24 Store the size of the allocation when it is freed
    Dqn_Str8   stack_trace;       // 40 Stack trace at the point of allocation
    Dqn_Str8   freed_stack_trace; // 56 Stack trace of where the allocation was freed
    uint16_t   flags;             // 72 Bit flags from `Dqn_DebugAllocFlag`
};

static_assert(sizeof(Dqn_DebugAlloc) == 64 || sizeof(Dqn_DebugAlloc) == 32, // NOTE: 64 bit vs 32 bit pointers respectively
              "We aim to keep the allocation record as light as possible as "
              "memory tracking can get expensive. Enforce that there is no "
              "unexpected padding.");

// NOTE: [$ASAN] Dqn_Asan //////////////////////////////////////////////////////////////////////////
DQN_API void                           Dqn_ASAN_PoisonMemoryRegion     (void const volatile *ptr, Dqn_usize size);
DQN_API void                           Dqn_ASAN_UnpoisonMemoryRegion   (void const volatile *ptr, Dqn_usize size);

// NOTE: [$STKT] Dqn_StackTrace ////////////////////////////////////////////////////////////////////
DQN_API Dqn_StackTraceWalkResult       Dqn_StackTrace_Walk             (Dqn_Arena *arena, uint16_t limit);
DQN_API Dqn_Str8                       Dqn_StackTrace_WalkStr8CRT      (uint16_t limit, uint16_t skip);
DQN_API bool                           Dqn_StackTrace_WalkResultIterate(Dqn_StackTraceWalkResultIterator *it, Dqn_StackTraceWalkResult const *walk);
DQN_API Dqn_Str8                       Dqn_StackTrace_WalkResultStr8   (Dqn_Arena *arena, Dqn_StackTraceWalkResult const *walk, uint16_t skip);
DQN_API Dqn_Str8                       Dqn_StackTrace_WalkResultStr8CRT(Dqn_StackTraceWalkResult const *walk, uint16_t skip);
DQN_API Dqn_Slice<Dqn_StackTraceFrame> Dqn_StackTrace_GetFrames        (Dqn_Arena *arena, uint16_t limit);
DQN_API Dqn_StackTraceFrame            Dqn_StackTrace_RawFrameToFrame  (Dqn_Arena *arena, Dqn_StackTraceRawFrame raw_frame);
DQN_API void                           Dqn_StackTrace_Print            (uint16_t limit);
DQN_API void                           Dqn_StackTrace_ReloadSymbols    ();

// NOTE: [$DEBG] Dqn_Debug /////////////////////////////////////////////////////////////////////////
#if defined(DQN_LEAK_TRACKING)
DQN_API void                           Dqn_Debug_TrackAlloc            (void *ptr, Dqn_usize size, bool alloc_can_leak);
DQN_API void                           Dqn_Debug_TrackDealloc          (void *ptr);
DQN_API void                           Dqn_Debug_DumpLeaks             ();
#else
#define                                Dqn_Debug_TrackAlloc(ptr, size, alloc_can_leak) do { (void)ptr; (void)size; (void)alloc_can_leak; } while (0)
#define                                Dqn_Debug_TrackDealloc(ptr)                     do { (void)ptr;                                   } while (0)
#define                                Dqn_Debug_DumpLeaks()                           do {                                              } while (0)
#endif


