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
    // This flag must only be used in Dqn_VMem_Protect
    Dqn_VMemPage_Guard     = 1 << 3,

    // If leak tracing is enabled, this flag will allow the allocation recorded
    // from the reserve call to be leaked, e.g. not printed when leaks are
    // dumped to the console.
    Dqn_VMemPage_AllocRecordLeakPermitted = 1 << 2,
};

DQN_API void *Dqn_VMem_Reserve (Dqn_usize size, Dqn_VMemCommit commit, uint32_t page_flags);
DQN_API bool  Dqn_VMem_Commit  (void *ptr, Dqn_usize size, uint32_t page_flags);
DQN_API void  Dqn_VMem_Decommit(void *ptr, Dqn_usize size);
DQN_API void  Dqn_VMem_Release (void *ptr, Dqn_usize size);
DQN_API int   Dqn_VMem_Protect (void *ptr, Dqn_usize size, uint32_t page_flags);

// NOTE: [$MEMB] Dqn_MemBlock ======================================================================
// Encapsulates allocation of objects from a raw block of memory by bumping a
// a pointer in the block. Some examples include our memory arenas are
// implemented as light wrappers over chained memory blocks and our arrays
// backed by virtual memory take memory blocks.
//
// One pattern we take advantage of under this design is that our virtual arrays
// can ask an arena for a memory block and sub-allocate its contiguous items
// from it. Since the arena created the memory block, the array's lifetime is
// bound to the arena which could also be managing a bunch of other allocations
// with the same lifetime.
//
// This provides an advantage over creating a specific arena for that array that
// is configured not to grow or chain (in order to adhere to the contiguous
// layout requirement) thus limiting the arena to that 1 specific usecase.
//
// NOTE: API =======================================================================================
// @proc Dqn_MemBlockSizeRequiredResult
//   @desc Calculate various size metrics about how many bytes it'd take to
//   allocate a pointer from the given block. The size of the allocation is
//   treated as one object and the padding and page-guards are applied
//   accordingly to the one object
//
//   If you are trying to determine how many bytes are required for `N` distinct
//   objects then you must multiple the result of this function by `N` to
//   account for the per-item page-guard paddding.
//
//   @param `block` Pass in the block you wish to allocate from to calculate
//   size metrics for. You may pass in `null` to calculate how many bytes are 
//   needed to `Dqn_MemBlock_Init` a fresh block capable of allocating the size 
//   requested.
//
//   @param `flags` The `Dqn_MemBlockFlag`s to apply in the calculation. Various
//   features may influence the sizes required for allocating the requested
//   amount of bytes. If `block` is passed in, the flags will be OR'ed together 
//   to determine the flags to account for.

enum Dqn_MemBlockFlag
{
    Dqn_MemBlockFlag_Nil                      = 0,
    Dqn_MemBlockFlag_ArenaPrivate             = 1 << 0,

    // Enforce that adjacent allocations from this block are contiguous in
    // memory (as long as the alignment used between allocations are
    /// consistent).
    //
    // This flag is currently only used when ASAN memory poison-ing is enabled
    // via `DQN_ASAN_POISON`. In this mode all allocations are sandwiched with a
    // page's worth of poison-ed memory breaking the contiguous requirement of
    // arrays. Passing this flag will stop the block from padding pointers with
    // poison.
    Dqn_MemBlockFlag_AllocsAreContiguous      = 1 << 1,

    // If leak tracing is enabled, this flag will allow the allocation recorded
    // from the reserve call to be leaked, e.g. not printed when leaks are
    // dumped to the console.
    Dqn_MemBlockFlag_AllocRecordLeakPermitted = 1 << 2,
    Dqn_MemBlockFlag_All                      = Dqn_MemBlockFlag_ArenaPrivate |
                                                Dqn_MemBlockFlag_AllocsAreContiguous |
                                                Dqn_MemBlockFlag_AllocRecordLeakPermitted,
};

struct Dqn_MemBlock
{
    void         *data;
    Dqn_usize     used;
    Dqn_usize     size;
    Dqn_usize     commit;
    Dqn_MemBlock *next;
    Dqn_MemBlock *prev;
    uint8_t       flags;
};

struct Dqn_MemBlockSizeRequiredResult
{
    // Offset from the block's data pointer that the allocation will start at
    // If `block` was null then this is always set to 0.
    Dqn_usize data_offset;

    // How many bytes will be allocated for the amount requested by the user.
    // This is usually the same as the number requested except when ASAN
    // poison-ing is enabled. In that case, the pointer will be padded at the
    // end with a page's worth of poison-ed memory.
    Dqn_usize alloc_size;

    // How many bytes of space is needed in a block for allocating the requested
    // pointer. This may differ from the allocation size depending on additional
    // alignment requirements *and* whether or not ASAN poison-ing is required.
    Dqn_usize block_size;
};

DQN_API Dqn_usize                      Dqn_MemBlock_MetadataSize(uint8_t flags);
DQN_API Dqn_MemBlockSizeRequiredResult Dqn_MemBlock_SizeRequired(Dqn_MemBlock const *block, Dqn_usize size, uint8_t alignment, uint32_t flags);
DQN_API Dqn_MemBlock *                 Dqn_MemBlock_Init        (Dqn_usize reserve, Dqn_usize commit, uint32_t flags);
DQN_API void         *                 Dqn_MemBlock_Alloc       (Dqn_MemBlock *block, Dqn_usize size, uint8_t alignment, Dqn_ZeroMem zero_mem);
DQN_API void                           Dqn_MemBlock_Free        (Dqn_MemBlock *block);
DQN_API void                           Dqn_MemBlock_Pop         (Dqn_MemBlock *block, Dqn_usize size);
DQN_API void                           Dqn_MemBlock_PopTo       (Dqn_MemBlock *block, Dqn_usize to);

#define               Dqn_MemBlock_New(block, Type, zero_mem)             (Type *)Dqn_MemBlock_Alloc(block, sizeof(Type),         alignof(Type), zero_mem)
#define               Dqn_MemBlock_NewArray(block, Type, count, zero_mem) (Type *)Dqn_MemBlock_Alloc(block, sizeof(Type) * count, alignof(Type), zero_mem)

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

// @proc Dqn_Arena_Alloc, Dqn_Arena_New, Dqn_Arena_NewArray,
// Dqn_Arena_NewArrayWithBlock,
//   @desc Alloc byte/objects
//   `Alloc`             allocates bytes
//   `New`               allocates an object
//   `NewArray`          allocates an array of objects
//   `NewArrayWithBlock` allocates an array of objects from the given memory 'block'
//   @return A pointer to the allocated bytes/object. Null pointer on failure

// @proc Dqn_Arena_Copy, Dqn_Arena_CopyZ
//   @desc Alloc a copy of an object's bytes. The 'Z' variant adds
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
    bool            allocs_are_allowed_to_leak;
    Dqn_String8     label; // Optional label to describe the arena
    Dqn_MemBlock   *head;  // Active block the arena is allocating from
    Dqn_MemBlock   *curr;  // Active block the arena is allocating from
    Dqn_MemBlock   *tail;  // Last block in the linked list of blocks
    uint64_t        blocks;
};

struct Dqn_ArenaTempMemory
{
    Dqn_Arena    *arena;     // Arena the scope is for
    Dqn_MemBlock *head;      // Head block of the arena at the beginning of the scope
    Dqn_MemBlock *curr;      // Current block of the arena at the beginning of the scope
    Dqn_MemBlock *tail;      // Tail block of the arena at the beginning of the scope
    Dqn_usize     blocks;
    Dqn_usize     curr_used; // Current used amount of the current block
};

// Automatically begin and end a temporary memory scope on object construction
// and destruction respectively.
#define Dqn_Arena_TempMemoryScope(arena) Dqn_ArenaTempMemoryScope DQN_UNIQUE_NAME(temp_memory_) = Dqn_ArenaTempMemoryScope(arena)
struct Dqn_ArenaTempMemoryScope
{
    Dqn_ArenaTempMemoryScope(Dqn_Arena *arena);
    ~Dqn_ArenaTempMemoryScope();
    Dqn_ArenaTempMemory temp_memory;
    bool                cancel = false;
};

enum Dqn_ArenaCommit
{
    // Commit the pages to ensure the block has the requested commit amount.
    // No-op if the block has sufficient commit space already.
    Dqn_ArenaCommit_EnsureSpace,
    Dqn_ArenaCommit_GetNewPages, // Grow the block by the requested commit amount
};

// NOTE: Allocation ================================================================================
#define                     Dqn_Arena_New(arena, Type, zero_mem)             (Type *)Dqn_Arena_Alloc(arena, sizeof(Type), alignof(Type), zero_mem)
#define                     Dqn_Arena_NewCopy(arena, Type, src)              (Type *)Dqn_Arena_Copy(arena, (src), sizeof(*src), alignof(Type))
#define                     Dqn_Arena_NewCopyZ(arena, Type, src)             (Type *)Dqn_Arena_Copy(arena, (src), sizeof(*src), alignof(Type))
#define                     Dqn_Arena_NewArray(arena, Type, count, zero_mem) (Type *)Dqn_Arena_Alloc(arena, sizeof(Type) * (count), alignof(Type), zero_mem)
#define                     Dqn_Arena_NewArrayCopy(arena, Type, src, count)  (Type *)Dqn_Arena_Copy(arena, (src), sizeof(*src) * (count), alignof(Type))
#define                     Dqn_Arena_NewArrayCopyZ(arena, Type, src, count) (Type *)Dqn_Arena_CopyZ(arena, (src), sizeof(*src) * (count), alignof(Type))

DQN_API Dqn_Allocator       Dqn_Arena_Allocator      (Dqn_Arena *arena);
DQN_API Dqn_MemBlock *      Dqn_Arena_Grow           (Dqn_Arena *arena, Dqn_usize size, Dqn_usize commit, uint8_t flags);
DQN_API void *              Dqn_Arena_Alloc          (Dqn_Arena *arena, Dqn_usize size, uint8_t align, Dqn_ZeroMem zero_mem);
DQN_API void *              Dqn_Arena_Copy           (Dqn_Arena *arena, void *src, Dqn_usize size, uint8_t alignment);
DQN_API void *              Dqn_Arena_CopyZ          (Dqn_Arena *arena, void *src, Dqn_usize size, uint8_t alignment);
DQN_API void                Dqn_Arena_Free           (Dqn_Arena *arena, Dqn_ZeroMem zero_mem);

// NOTE: Temp Memory ===============================================================================
DQN_API Dqn_ArenaTempMemory Dqn_Arena_BeginTempMemory(Dqn_Arena *arena);
DQN_API void                Dqn_Arena_EndTempMemory  (Dqn_ArenaTempMemory temp_memory, bool cancel);

// NOTE: Arena Stats ===============================================================================
DQN_API Dqn_ArenaStatString Dqn_Arena_StatString     (Dqn_ArenaStat const *stat);
DQN_API void                Dqn_Arena_LogStats       (Dqn_Arena const *arena);

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
