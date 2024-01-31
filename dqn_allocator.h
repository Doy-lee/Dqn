////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    $$$$$$\  $$\       $$\       $$$$$$\   $$$$$$\   $$$$$$\ $$$$$$$$\  $$$$$$\  $$$$$$$\
//   $$  __$$\ $$ |      $$ |     $$  __$$\ $$  __$$\ $$  __$$\\__$$  __|$$  __$$\ $$  __$$\
//   $$ /  $$ |$$ |      $$ |     $$ /  $$ |$$ /  \__|$$ /  $$ |  $$ |   $$ /  $$ |$$ |  $$ |
//   $$$$$$$$ |$$ |      $$ |     $$ |  $$ |$$ |      $$$$$$$$ |  $$ |   $$ |  $$ |$$$$$$$  |
//   $$  __$$ |$$ |      $$ |     $$ |  $$ |$$ |      $$  __$$ |  $$ |   $$ |  $$ |$$  __$$<
//   $$ |  $$ |$$ |      $$ |     $$ |  $$ |$$ |  $$\ $$ |  $$ |  $$ |   $$ |  $$ |$$ |  $$ |
//   $$ |  $$ |$$$$$$$$\ $$$$$$$$\ $$$$$$  |\$$$$$$  |$$ |  $$ |  $$ |    $$$$$$  |$$ |  $$ |
//   \__|  \__|\________|\________|\______/  \______/ \__|  \__|  \__|    \______/ \__|  \__|
//
//   dqn_allocator.h -- Custom memory allocators
//
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// [$AREN] Dqn_Arena        -- Growing bump allocator
// [$CHUN] Dqn_ChunkPool    -- Allocates reusable, free-able memory in PoT chunks
// [$ACAT] Dqn_ArenaCatalog -- Collate, create & manage arenas in a catalog
//
// NOTE: [$AREN] Dqn_Arena /////////////////////////////////////////////////////////////////////////
#if !defined(DQN_ARENA_RESERVE_SIZE)
    #define DQN_ARENA_RESERVE_SIZE DQN_MEGABYTES(64)
#endif
#if !defined(DQN_ARENA_COMMIT_SIZE)
    #define DQN_ARENA_COMMIT_SIZE DQN_KILOBYTES(64)
#endif

struct Dqn_ArenaBlock
{
    Dqn_ArenaBlock *prev;
    uint64_t        used;
    uint64_t        commit;
    uint64_t        reserve;
    uint64_t        reserve_sum;
};

enum Dqn_ArenaFlag
{
    Dqn_ArenaFlag_Nil          = 0,
    Dqn_ArenaFlag_NoGrow       = 1 << 0,
    Dqn_ArenaFlag_NoPoison     = 1 << 1,
    Dqn_ArenaFlag_NoAllocTrack = 1 << 2,
    Dqn_ArenaFlag_AllocCanLeak = 1 << 3,
};

struct Dqn_Arena
{
    Dqn_ArenaBlock *curr;
    uint8_t         flags;
};

struct Dqn_ArenaTempMem
{
    Dqn_Arena *arena;
    uint64_t   used_sum;
};

struct Dqn_ArenaTempMemScope
{
    Dqn_ArenaTempMemScope(Dqn_Arena *arena);
    ~Dqn_ArenaTempMemScope();
    Dqn_ArenaTempMem mem;
};

Dqn_usize const DQN_ARENA_HEADER_SIZE = Dqn_AlignUpPowerOfTwo(sizeof(Dqn_Arena), 64);

// NOTE: [$CHUN] Dqn_ChunkPool /////////////////////////////////////////////////////////////////////
#if !defined(DQN_CHUNK_POOL_DEFAULT_ALIGN)
    #define DQN_CHUNK_POOL_DEFAULT_ALIGN 16
#endif

struct Dqn_ChunkPoolSlot
{
    void              *data;
    Dqn_ChunkPoolSlot *next;
};

enum Dqn_ChunkPoolSlotSize
{
    Dqn_ChunkPoolSlotSize_32B,
    Dqn_ChunkPoolSlotSize_64B,
    Dqn_ChunkPoolSlotSize_128B,
    Dqn_ChunkPoolSlotSize_256B,
    Dqn_ChunkPoolSlotSize_512B,
    Dqn_ChunkPoolSlotSize_1KiB,
    Dqn_ChunkPoolSlotSize_2KiB,
    Dqn_ChunkPoolSlotSize_4KiB,
    Dqn_ChunkPoolSlotSize_8KiB,
    Dqn_ChunkPoolSlotSize_16KiB,
    Dqn_ChunkPoolSlotSize_32KiB,
    Dqn_ChunkPoolSlotSize_64KiB,
    Dqn_ChunkPoolSlotSize_128KiB,
    Dqn_ChunkPoolSlotSize_256KiB,
    Dqn_ChunkPoolSlotSize_512KiB,
    Dqn_ChunkPoolSlotSize_1MiB,
    Dqn_ChunkPoolSlotSize_2MiB,
    Dqn_ChunkPoolSlotSize_4MiB,
    Dqn_ChunkPoolSlotSize_8MiB,
    Dqn_ChunkPoolSlotSize_16MiB,
    Dqn_ChunkPoolSlotSize_32MiB,
    Dqn_ChunkPoolSlotSize_64MiB,
    Dqn_ChunkPoolSlotSize_128MiB,
    Dqn_ChunkPoolSlotSize_256MiB,
    Dqn_ChunkPoolSlotSize_512MiB,
    Dqn_ChunkPoolSlotSize_1GiB,
    Dqn_ChunkPoolSlotSize_2GiB,
    Dqn_ChunkPoolSlotSize_4GiB,
    Dqn_ChunkPoolSlotSize_8GiB,
    Dqn_ChunkPoolSlotSize_16GiB,
    Dqn_ChunkPoolSlotSize_32GiB,
    Dqn_ChunkPoolSlotSize_Count,
};

struct Dqn_ChunkPool
{
    Dqn_Arena         *arena;
    Dqn_ChunkPoolSlot *slots[Dqn_ChunkPoolSlotSize_Count];
    uint8_t            align;
};

// NOTE: [$ACAT] Dqn_ArenaCatalog //////////////////////////////////////////////////////////////////
struct Dqn_ArenaCatalogItem
{
    Dqn_Arena            *arena;
    Dqn_Str8              label;
    Dqn_ArenaCatalogItem *next;
    Dqn_ArenaCatalogItem *prev;
};

struct Dqn_ArenaCatalog
{
    Dqn_TicketMutex       ticket_mutex; // Mutex for adding to the linked list of arenas
    struct Dqn_ChunkPool *pool;
    Dqn_ArenaCatalogItem  sentinel;
    uint16_t              arena_count;
};

// NOTE: [$AREN] Dqn_Arena /////////////////////////////////////////////////////////////////////////
DQN_API Dqn_Arena             Dqn_Arena_InitSize             (uint64_t reserve, uint64_t commit, uint8_t flags);
DQN_API void                  Dqn_Arena_Deinit               (Dqn_Arena *arena);
DQN_API bool                  Dqn_Arena_Commit               (Dqn_Arena *arena, uint64_t size);
DQN_API bool                  Dqn_Arena_CommitTo             (Dqn_Arena *arena, uint64_t pos);
DQN_API void *                Dqn_Arena_Alloc                (Dqn_Arena *arena, uint64_t size, uint8_t align, Dqn_ZeroMem zero_mem);
DQN_API void *                Dqn_Arena_AllocContiguous      (Dqn_Arena *arena, uint64_t size, uint8_t align, Dqn_ZeroMem zero_mem);
DQN_API void *                Dqn_Arena_Copy                 (Dqn_Arena *arena, void const *data, uint64_t size, uint8_t align);
DQN_API void                  Dqn_Arena_PopTo                (Dqn_Arena *arena, uint64_t init_used);
DQN_API void                  Dqn_Arena_Pop                  (Dqn_Arena *arena, uint64_t amount);
DQN_API void                  Dqn_Arena_Clear                (Dqn_Arena *arena);
DQN_API Dqn_ArenaTempMem      Dqn_Arena_TempMemBegin         (Dqn_Arena *arena);
DQN_API void                  Dqn_Arena_TempMemEnd           (Dqn_ArenaTempMem mem);
#define                       Dqn_Arena_New(arena, T, zero_mem)             (T *)Dqn_Arena_Alloc(arena,        sizeof(T),              alignof(T), zero_mem)
#define                       Dqn_Arena_NewArray(arena, T, count, zero_mem) (T *)Dqn_Arena_Alloc(arena,        sizeof(T)    * (count), alignof(T), zero_mem)
#define                       Dqn_Arena_NewCopy(arena, T, src)              (T *)Dqn_Arena_Copy (arena, (src), sizeof(*src),           alignof(T))
#define                       Dqn_Arena_NewArrayCopy(arena, T, src, count)  (T *)Dqn_Arena_Copy (arena, (src), sizeof(*src) * (count), alignof(T))

// NOTE: [$CHUN] Dqn_ChunkPool /////////////////////////////////////////////////////////////////////
#define                       Dqn_ChunkPool_New(pool, T)     (T *)Dqn_ChunkPool_Alloc(pool, sizeof(T))
DQN_API Dqn_ChunkPool         Dqn_ChunkPool_Init             (Dqn_Arena *arena, uint8_t align);
DQN_API bool                  Dqn_ChunkPool_IsValid          (Dqn_ChunkPool const *pool);
DQN_API void *                Dqn_ChunkPool_Alloc            (Dqn_ChunkPool *pool, Dqn_usize size);
DQN_API Dqn_Str8              Dqn_ChunkPool_AllocStr8FV      (Dqn_ChunkPool *pool, DQN_FMT_ATTRIB char const *fmt, va_list args);
DQN_API Dqn_Str8              Dqn_ChunkPool_AllocStr8F       (Dqn_ChunkPool *pool, DQN_FMT_ATTRIB char const *fmt, ...);
DQN_API Dqn_Str8              Dqn_ChunkPool_AllocStr8Copy    (Dqn_ChunkPool *pool, Dqn_Str8 string);
DQN_API void                  Dqn_ChunkPool_Dealloc          (Dqn_ChunkPool *pool, void *ptr);

// NOTE: [$ACAT] Dqn_ArenaCatalog //////////////////////////////////////////////////////////////////
DQN_API void                  Dqn_ArenaCatalog_Init          (Dqn_ArenaCatalog *catalog, Dqn_ChunkPool *pool);
DQN_API Dqn_ArenaCatalogItem *Dqn_ArenaCatalog_Find          (Dqn_ArenaCatalog *catalog, Dqn_Str8 label);
DQN_API void                  Dqn_ArenaCatalog_AddLabelRef   (Dqn_ArenaCatalog *catalog, Dqn_Arena *arena, Dqn_Str8 label);
DQN_API void                  Dqn_ArenaCatalog_AddLabelCopy  (Dqn_ArenaCatalog *catalog, Dqn_Arena *arena, Dqn_Str8 label);
DQN_API void                  Dqn_ArenaCatalog_AddF          (Dqn_ArenaCatalog *catalog, Dqn_Arena *arena, DQN_FMT_ATTRIB char const *fmt, ...);
DQN_API void                  Dqn_ArenaCatalog_AddFV         (Dqn_ArenaCatalog *catalog, Dqn_Arena *arena, DQN_FMT_ATTRIB char const *fmt, va_list args);
DQN_API Dqn_Arena *           Dqn_ArenaCatalog_AllocLabelRef (Dqn_ArenaCatalog *catalog, Dqn_usize reserve, Dqn_usize commit, uint8_t arena_flags, Dqn_Str8 label);
DQN_API Dqn_Arena *           Dqn_ArenaCatalog_AllocLabelCopy(Dqn_ArenaCatalog *catalog, Dqn_usize reserve, Dqn_usize commit, uint8_t arena_flags, Dqn_Str8 label);
DQN_API Dqn_Arena *           Dqn_ArenaCatalog_AllocFV       (Dqn_ArenaCatalog *catalog, Dqn_usize reserve, Dqn_usize commit, uint8_t arena_flags, DQN_FMT_ATTRIB char const *fmt, va_list args);
DQN_API Dqn_Arena *           Dqn_ArenaCatalog_AllocF        (Dqn_ArenaCatalog *catalog, Dqn_usize reserve, Dqn_usize commit, uint8_t arena_flags, DQN_FMT_ATTRIB char const *fmt, ...);
