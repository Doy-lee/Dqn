// NOTE: Table Of Contents =========================================================================
// Index                    | Disable #define | Description
// =================================================================================================
// [$ALLO] Dqn_Allocator    |                 | Generic allocator interface
// [$VMEM] Dqn_VMem         |                 | Virtual memory allocation
// [$AREN] Dqn_Arena        |                 | Growing bump allocator
// [$ACAT] Dqn_ArenaCatalog |                 | Collate, create & manage arenas in a catalog
// =================================================================================================

// NOTE: [$ALLO] Dqn_Allocator =====================================================================
#if defined(DQN_LEAK_TRACING)
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

struct Dqn_LeakTrace
{
    void         *ptr;             // The pointer we are tracking
    Dqn_usize     size;            // Size of the allocation
    Dqn_CallSite  call_site;       // Call site where the allocation was allocated
    bool          freed;           // True if this pointer has been freed
    Dqn_usize     freed_size;      // Size of the allocation that has been freed
    Dqn_CallSite  freed_call_site; // Call site where the allocation was freed
};

typedef void *Dqn_Allocator_AllocProc(DQN_LEAK_TRACE_FUNCTION size_t size, uint8_t align, Dqn_ZeroMem zero_mem, void *user_context);
typedef void  Dqn_Allocator_DeallocProc(DQN_LEAK_TRACE_FUNCTION void *ptr, size_t size, void *user_context);

struct Dqn_Allocator
{
    void                      *user_context; // User assigned pointer that is passed into the allocator functions
    Dqn_Allocator_AllocProc   *alloc;        // Memory allocating routine
    Dqn_Allocator_DeallocProc *dealloc;      // Memory deallocating routine
};

// NOTE: Macros ==================================================================================
#define Dqn_Allocator_Alloc(allocator, size, align, zero_mem) Dqn_Allocator_Alloc_(DQN_LEAK_TRACE allocator, size, align, zero_mem)
#define Dqn_Allocator_Dealloc(allocator, ptr, size) Dqn_Allocator_Dealloc_(DQN_LEAK_TRACE allocator, ptr, size)
#define Dqn_Allocator_NewArray(allocator, Type, count, zero_mem) (Type *)Dqn_Allocator_Alloc_(DQN_LEAK_TRACE allocator, sizeof(Type) * count, alignof(Type), zero_mem)
#define Dqn_Allocator_New(allocator, Type, zero_mem) (Type *)Dqn_Allocator_Alloc_(DQN_LEAK_TRACE allocator, sizeof(Type), alignof(Type), zero_mem)

// NOTE: Internal ==================================================================================
void *Dqn_Allocator_Alloc_  (DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, size_t size, uint8_t align, Dqn_ZeroMem zero_mem);
void  Dqn_Allocator_Dealloc_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, void *ptr, size_t size);

// NOTE: [$VMEM] Dqn_VMem ==========================================================================
enum Dqn_VMemCommit
{
    Dqn_VMemCommit_No,
    Dqn_VMemCommit_Yes,
};

enum Dqn_VMemPage
{
    // Exception on read/write with a page. This flag overrides the read/write 
    // access.
    Dqn_VMemPage_NoAccess  = 1 << 0,

    // Only read permitted on the page.
    Dqn_VMemPage_Read      = 1 << 1,

    // Only write permitted on the page. On Windows this is not supported and
    // will be promoted to read+write permissions.
    Dqn_VMemPage_Write     = 1 << 2,

    Dqn_VMemPage_ReadWrite = Dqn_VMemPage_Read | Dqn_VMemPage_Write,

    // Modifier used in conjunction with previous flags. Raises exception on
    // first access to the page, then, the underlying protection flags are
    // active. This is supported on Windows, on other OS's using this flag will
    // set the OS equivalent of Dqn_VMemPage_NoAccess.
    Dqn_VMemPage_Guard     = 1 << 3,
};

#if !defined(DQN_VMEM_PAGE_GRANULARITY)
    #define DQN_VMEM_PAGE_GRANULARITY DQN_KILOBYTES(4)
#endif

#if !defined(DQN_VMEM_RESERVE_GRANULARITY)
    #define DQN_VMEM_RESERVE_GRANULARITY DQN_KILOBYTES(64)
#endif

#if !defined(DQN_VMEM_COMMIT_GRANULARITY)
    #define DQN_VMEM_COMMIT_GRANULARITY DQN_VMEM_PAGE_GRANULARITY
#endif

static_assert(Dqn_IsPowerOfTwo(DQN_VMEM_RESERVE_GRANULARITY),
              "This library assumes that the memory allocation routines from the OS has PoT allocation granularity");
static_assert(Dqn_IsPowerOfTwo(DQN_VMEM_COMMIT_GRANULARITY),
              "This library assumes that the memory allocation routines from the OS has PoT allocation granularity");
static_assert(DQN_VMEM_COMMIT_GRANULARITY < DQN_VMEM_RESERVE_GRANULARITY,
              "Minimum commit size must be lower than the reserve size to avoid OOB math on pointers in this library");

DQN_API void *Dqn_VMem_Reserve (Dqn_usize size, Dqn_VMemCommit commit, uint32_t page_flags);
DQN_API bool  Dqn_VMem_Commit  (void *ptr, Dqn_usize size, uint32_t page_flags);
DQN_API void  Dqn_VMem_Decommit(void *ptr, Dqn_usize size);
DQN_API void  Dqn_VMem_Release (void *ptr, Dqn_usize size);
DQN_API int   Dqn_VMem_Protect (void *ptr, Dqn_usize size, uint32_t page_flags);

// NOTE: [$AREN] Dqn_Arena =========================================================================
// A bump-allocator that can grow dynamically by chaining blocks of memory
// together. The arena's memory is backed by virtual memory allowing the
// allocator to reserve and commit physical pages as memory is given from
// the block of memory.
//
// Arena's allow grouping of multiple allocations into one lifetime that is
// bound to the arena. Allocation involves a simple 'bump' of the pointer in the
// memory block. Freeing involves resetting the pointer to the start of the
// block and/or releasing the single pointer to the entire block of memory.
//
// This allocator reserves memory blocks at a 64k granularity as per the minimum
// granularity reserve size of VirtualAlloc on Windows. Memory is commit at
// a 4k granularity for similar reasons. On 64 bit platforms you have access
// to 48 bits of address space for applications, this is 256TB of address space
// you can reserve. The typical usage for this style of arena is to reserve
// as much space as you possibly need, ever, for the lifetime of the arena (e.g.
// 64GB) since the arena only commits as much as needed.
//
// NOTE: API
// @proc Dqn_Arena_Grow
//   @desc Grow the arena's capacity by allocating a block of memory with the
//   requested size. The requested size is rounded up to the nearest 64k
//   boundary as that is the minimum reserve granularity (atleast on Windows)
//   for virtual memory.
//   @param size[in] The size in bytes to expand the capacity of the arena
//   @param commit[in] The amount of bytes to request to be physically backed by
//   pages from the OS.
//   @param flags[in] Bit flags from 'Dqn_ArenaBlockFlags', set to 0 if none
//   @return The block of memory that

// @proc Dqn_Arena_Allocate, Dqn_Arena_New, Dqn_Arena_NewArray,
// Dqn_Arena_NewArrayWithBlock,
//   @desc Allocate byte/objects
//   `Allocate`          allocates bytes
//   `New`               allocates an object
//   `NewArray`          allocates an array of objects
//   `NewArrayWithBlock` allocates an array of objects from the given memory 'block'
//   @return A pointer to the allocated bytes/object. Null pointer on failure

// @proc Dqn_Arena_Copy, Dqn_Arena_CopyZ
//   @desc Allocate a copy of an object's bytes. The 'Z' variant adds
//   a null-terminating byte at the end of the stream.
//   @return A pointer to the allocated object. Null pointer on failure.

// @proc Dqn_Arena_Reset
//   @desc Set the arena's current block to the first block in the linked list
//   of blocks and mark all blocks free.
//   @param[in] zero_mem When yes, the memory is cleared using DQN_MEMSET with the
//   value of DQN_MEMSET_BYTE

// @proc Dqn_Arena_Free
//   @desc Free the arena returning all memory back to the OS
//   @param[in] zero_mem: When true, the memory is cleared using DQN_MEMSET with
//   the value of DQN_MEMSET_BYTE

// @proc Dqn_Arena_BeginTempMemory
//   @desc Begin an allocation scope where all allocations between begin and end
//   calls will be reverted. Useful for short-lived or highly defined lifetime
//   allocations. An allocation scope is invalidated if the arena is freed
//   between the begin and end call.

// @proc Dqn_Arena_EndTempMemory
//   @desc End an allocation scope previously begun by calling begin scope.

// @proc Dqn_Arena_StatString
//   @desc Dump the stats of the given arena to a string
//   @param[in] arena The arena to dump stats for
//   @return A stack allocated string containing the stats of the arena

// @proc Dqn_Arena_LogStats
//   @desc Dump the stats of the given arena to the memory log-stream.
//   @param[in] arena The arena to dump stats for

enum Dqn_ArenaBlockFlags
{
    Dqn_ArenaBlockFlags_Private = 1 << 0, ///< Private blocks can only allocate its memory when used in the 'FromBlock' API variants
};

struct Dqn_ArenaStat
{
    Dqn_usize capacity;     // Total allocating capacity of the arena in bytes
    Dqn_usize used;         // Total amount of bytes used in the arena
    Dqn_usize wasted;       // Orphaned space in blocks due to allocations requiring more space than available in the active block
    uint32_t  blocks;       // Number of memory blocks in the arena
    Dqn_usize syscalls;     // Number of memory allocation syscalls into the OS

    Dqn_usize capacity_hwm; // High-water mark for 'capacity'
    Dqn_usize used_hwm;     // High-water mark for 'used'
    Dqn_usize wasted_hwm;   // High-water mark for 'wasted'
    uint32_t  blocks_hwm;   // High-water mark for 'blocks'
};

struct Dqn_ArenaBlock
{
    struct Dqn_Arena *arena;   // Arena that owns this block
    void             *memory;  // Backing memory of the block
    Dqn_usize         size;    // Size of the block
    Dqn_usize         used;    // Number of bytes used up in the block. Always less than the commit amount.
    Dqn_usize         hwm_used;// High-water mark for 'used' bytes in this block
    Dqn_usize         commit;  // Number of bytes in the block physically backed by pages
    Dqn_ArenaBlock   *prev;    // Previous linked block
    Dqn_ArenaBlock   *next;    // Next linked block
    uint8_t           flags;   // Bit field for 'Dqn_ArenaBlockFlags'
};

struct Dqn_ArenaStatString
{
    char     data[256];
    uint16_t size;
};

struct Dqn_Arena
{
    bool            use_after_free_guard;
    uint32_t        temp_memory_count;
    Dqn_String8     label; // Optional label to describe the arena
    Dqn_ArenaBlock *head;  // Active block the arena is allocating from
    Dqn_ArenaBlock *curr;  // Active block the arena is allocating from
    Dqn_ArenaBlock *tail;  // Last block in the linked list of blocks
    Dqn_ArenaStat   stats; // Current arena stats, reset when reset usage is invoked.
};

struct Dqn_ArenaTempMemory
{
    Dqn_Arena      *arena;     // Arena the scope is for
    Dqn_ArenaBlock *head;      // Head block of the arena at the beginning of the scope
    Dqn_ArenaBlock *curr;      // Current block of the arena at the beginning of the scope
    Dqn_ArenaBlock *tail;      // Tail block of the arena at the beginning of the scope
    Dqn_usize       curr_used; // Current used amount of the current block
    Dqn_ArenaStat   stats;     // Stats of the arena at the beginning of the scope
};

// Automatically begin and end a temporary memory scope on object construction
// and destruction respectively.
#define DQN_ARENA_TEMP_MEMORY_SCOPE(arena) Dqn_ArenaTempMemoryScope_ DQN_UNIQUE_NAME(temp_memory_) = Dqn_ArenaTempMemoryScope_(DQN_LEAK_TRACE arena)
#define Dqn_ArenaTempMemoryScope(arena) Dqn_ArenaTempMemoryScope_(DQN_LEAK_TRACE arena)
struct Dqn_ArenaTempMemoryScope_
{
    Dqn_ArenaTempMemoryScope_(DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena);
    ~Dqn_ArenaTempMemoryScope_();
    Dqn_ArenaTempMemory temp_memory;
    #if defined(DQN_LEAK_TRACING)
    Dqn_CallSite        leak_site__;
    #endif
};

enum Dqn_ArenaCommit
{
    // Commit the pages to ensure the block has the requested commit amount.
    // No-op if the block has sufficient commit space already.
    Dqn_ArenaCommit_EnsureSpace,
    Dqn_ArenaCommit_GetNewPages, // Grow the block by the requested commit amount
};

// NOTE: Allocation ================================================================================
#define                     Dqn_Arena_Grow(arena, size, commit, flags)                Dqn_Arena_Grow_(DQN_LEAK_TRACE arena, size, commit, flags)
#define                     Dqn_Arena_Allocate(arena, size, align, zero_mem)          Dqn_Arena_Allocate_(DQN_LEAK_TRACE arena, size, align, zero_mem)
#define                     Dqn_Arena_New(arena, Type, zero_mem)                      (Type *)Dqn_Arena_Allocate_(DQN_LEAK_TRACE arena, sizeof(Type), alignof(Type), zero_mem)
#define                     Dqn_Arena_NewArray(arena, Type, count, zero_mem)          (Type *)Dqn_Arena_Allocate_(DQN_LEAK_TRACE arena, sizeof(Type) * count, alignof(Type), zero_mem)
#define                     Dqn_Arena_NewArrayWithBlock(block, Type, count, zero_mem) (Type *)Dqn_Arena_AllocateFromBlock(block, sizeof(Type) * count, alignof(Type), zero_mem)
#define                     Dqn_Arena_Copy(arena, Type, src, count)                   (Type *)Dqn_Arena_Copy_(DQN_LEAK_TRACE arena, src, sizeof(*src) * count, alignof(Type))
#define                     Dqn_Arena_CopyZ(arena, Type, src, count)                  (Type *)Dqn_Arena_CopyZ_(DQN_LEAK_TRACE arena, src, sizeof(*src) * count, alignof(Type))
#define                     Dqn_Arena_Free(arena, zero_mem)                           Dqn_Arena_Free_(DQN_LEAK_TRACE arena, zero_mem)

DQN_API void                Dqn_Arena_CommitFromBlock  (Dqn_ArenaBlock *block, Dqn_usize size, Dqn_ArenaCommit commit);
DQN_API void *              Dqn_Arena_AllocateFromBlock(Dqn_ArenaBlock *block, Dqn_usize size, uint8_t align, Dqn_ZeroMem zero_mem);
DQN_API Dqn_Allocator       Dqn_Arena_Allocator        (Dqn_Arena *arena);
DQN_API void                Dqn_Arena_Reset            (Dqn_Arena *arena, Dqn_ZeroMem zero_mem);

// NOTE: Temp Memory ===============================================================================
DQN_API Dqn_ArenaTempMemory Dqn_Arena_BeginTempMemory  (Dqn_Arena *arena);
#define                     Dqn_Arena_EndTempMemory(arena_temp_memory)                Dqn_Arena_EndTempMemory_(DQN_LEAK_TRACE arena_temp_memory)

// NOTE: Arena Stats ===============================================================================
DQN_API Dqn_ArenaStatString Dqn_Arena_StatString       (Dqn_ArenaStat const *stat);
DQN_API void                Dqn_Arena_LogStats         (Dqn_Arena const *arena);

// NOTE: Internal ==================================================================================
DQN_API Dqn_ArenaBlock *    Dqn_Arena_Grow_            (DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, Dqn_usize size, Dqn_usize commit, uint8_t flags);
DQN_API void *              Dqn_Arena_Allocate_        (DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, Dqn_usize size, uint8_t align, Dqn_ZeroMem zero_mem);
DQN_API void *              Dqn_Arena_Copy_            (DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, void *src, Dqn_usize size, uint8_t alignment);
DQN_API void                Dqn_Arena_Free_            (DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, Dqn_ZeroMem zero_mem);
DQN_API void *              Dqn_Arena_CopyZ_           (DQN_LEAK_TRACE_FUNCTION Dqn_Arena *arena, void *src, Dqn_usize size, uint8_t alignment);
DQN_API void                Dqn_Arena_EndTempMemory_   (DQN_LEAK_TRACE_FUNCTION Dqn_ArenaTempMemory arena_temp_memory);

// NOTE: [$ACAT] Dqn_ArenaCatalog ==================================================================
struct Dqn_ArenaCatalogItem
{
    Dqn_Arena            *arena;
    Dqn_ArenaCatalogItem *next;
    Dqn_ArenaCatalogItem *prev;
};

struct Dqn_ArenaCatalog
{
    Dqn_TicketMutex       ticket_mutex; // Mutex for adding to the linked list of arenas
    Dqn_Arena            *arena;
    Dqn_ArenaCatalogItem  sentinel;
    uint16_t              arena_count;
};

DQN_API void       Dqn_ArenaCatalog_Init   (Dqn_ArenaCatalog *catalog, Dqn_Arena *arena);
DQN_API void       Dqn_ArenaCatalog_Add    (Dqn_ArenaCatalog *catalog, Dqn_Arena *arena);
DQN_API Dqn_Arena *Dqn_ArenaCatalog_Alloc  (Dqn_ArenaCatalog *catalog, Dqn_usize byte_size, Dqn_usize commit);
DQN_API Dqn_Arena *Dqn_ArenaCatalog_AllocFV(Dqn_ArenaCatalog *catalog, Dqn_usize byte_size, Dqn_usize commit, char const *fmt, va_list args);
DQN_API Dqn_Arena *Dqn_ArenaCatalog_AllocF (Dqn_ArenaCatalog *catalog, Dqn_usize byte_size, Dqn_usize commit, char const *fmt, ...);
