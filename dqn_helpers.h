/*
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   $$\   $$\ $$$$$$$$\ $$\       $$$$$$$\  $$$$$$$$\ $$$$$$$\   $$$$$$\
//   $$ |  $$ |$$  _____|$$ |      $$  __$$\ $$  _____|$$  __$$\ $$  __$$\
//   $$ |  $$ |$$ |      $$ |      $$ |  $$ |$$ |      $$ |  $$ |$$ /  \__|
//   $$$$$$$$ |$$$$$\    $$ |      $$$$$$$  |$$$$$\    $$$$$$$  |\$$$$$$\
//   $$  __$$ |$$  __|   $$ |      $$  ____/ $$  __|   $$  __$$<  \____$$\
//   $$ |  $$ |$$ |      $$ |      $$ |      $$ |      $$ |  $$ |$$\   $$ |
//   $$ |  $$ |$$$$$$$$\ $$$$$$$$\ $$ |      $$$$$$$$\ $$ |  $$ |\$$$$$$  |
//   \__|  \__|\________|\________|\__|      \________|\__|  \__| \______/
//
//   dqn_helpers.h -- Helper functions/data structures
//
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// [$PCG3] Dqn_PCG32        --                  -- RNG from the PCG family
// [$JSON] Dqn_JSONBuilder  -- DQN_JSON_BUILDER -- Construct json output
// [$BHEX] Dqn_Bin          -- DQN_BIN          -- Binary <-> hex helpers
// [$BSEA] Dqn_BinarySearch --                  -- Binary search
// [$BITS] Dqn_Bit          --                  -- Bitset manipulation
// [$SAFE] Dqn_Safe         --                  -- Safe arithmetic, casts, asserts
// [$MISC] Misc             --                  -- Uncategorised helper functions
// [$DLIB] Dqn_Library      --                  -- Globally shared runtime data for this library
// [$PROF] Dqn_Profiler     -- DQN_PROFILER     -- Profiler that measures using a timestamp counter
//
////////////////////////////////////////////////////////////////////////////////////////////////////
*/

// NOTE: [$PCGX] Dqn_PCG32 /////////////////////////////////////////////////////////////////////////
struct Dqn_PCG32 { uint64_t state; };

#if !defined(DQN_NO_JSON_BUILDER)
// NOTE: [$JSON] Dqn_JSONBuilder ///////////////////////////////////////////////////////////////////
enum Dqn_JSONBuilderItem
{
    Dqn_JSONBuilderItem_Empty,
    Dqn_JSONBuilderItem_OpenContainer,
    Dqn_JSONBuilderItem_CloseContainer,
    Dqn_JSONBuilderItem_KeyValue,
};

struct Dqn_JSONBuilder
{
    bool                use_stdout;        // When set, ignore the string builder and dump immediately to stdout
    Dqn_Str8Builder     string_builder;    // (Internal)
    int                 indent_level;      // (Internal)
    int                 spaces_per_indent; // The number of spaces per indent level
    Dqn_JSONBuilderItem last_item;
};
#endif // !defined(DQN_NO_JSON_BUIDLER)

#if !defined(DQN_NO_BIN)
// NOTE: [$BHEX] Dqn_Bin ///////////////////////////////////////////////////////////////////////////
struct Dqn_BinHexU64Str8
{
    char    data[2 /*0x*/ + 16 /*hex*/ + 1 /*null-terminator*/];
    uint8_t size;
};

enum Dqn_BinHexU64Str8Flags
{
    Dqn_BinHexU64Str8Flags_No0xPrefix   = 1 << 0, /// Remove the 0x prefix from the string
    Dqn_BinHexU64Str8Flags_UppercaseHex = 1 << 1, /// Use uppercase ascii characters for hex
};
#endif // !defined(DQN_NO_BIN)

// NOTE: [$BSEA] Dqn_BinarySearch //////////////////////////////////////////////////////////////////
template <typename T>
using Dqn_BinarySearchLessThanProc = bool(T const &lhs, T const &rhs);

template <typename T>
bool Dqn_BinarySearch_DefaultLessThan(T const &lhs, T const &rhs);

enum Dqn_BinarySearchType
{
    // Index of the match. If no match is found, found is set to false and the
    // index is set to the index where the match should be inserted/exist, if
    // it were in the array (in practice this turns out to be the first or
    // [last index + 1] of the array).
    Dqn_BinarySearchType_Match,

    // Index of the first element in the array that is `element >= find`. If no such
    // item is found or the array is empty, then, the index is set to the array
    // size and found is set to `false`.
    //
    // For example:
    //   int array[] = {0, 1, 2, 3, 4, 5};
    //   Dqn_BinarySearchResult result = Dqn_BinarySearch(array, DQN_ARRAY_UCOUNT(array), 4, Dqn_BinarySearchType_LowerBound);
    //   printf("%zu\n", result.index); // Prints index '4'

    Dqn_BinarySearchType_LowerBound,

    // Index of the first element in the array that is `element > find`. If no such
    // item is found or the array is empty, then, the index is set to the array
    // size and found is set to `false`.
    //
    // For example:
    //   int array[] = {0, 1, 2, 3, 4, 5};
    //   Dqn_BinarySearchResult result = Dqn_BinarySearch(array, DQN_ARRAY_UCOUNT(array), 4, Dqn_BinarySearchType_UpperBound);
    //   printf("%zu\n", result.index); // Prints index '5'

    Dqn_BinarySearchType_UpperBound,
};

struct Dqn_BinarySearchResult
{
    bool      found;
    Dqn_usize index;
};

// NOTE: [$MISC] Misc //////////////////////////////////////////////////////////////////////////////
struct Dqn_U64Str8
{
    char    data[27+1]; // NOTE(dqn): 27 is the maximum size of uint64_t including a separtor
    uint8_t size;
};

enum Dqn_U64ByteSizeType
{
    Dqn_U64ByteSizeType_B,
    Dqn_U64ByteSizeType_KiB,
    Dqn_U64ByteSizeType_MiB,
    Dqn_U64ByteSizeType_GiB,
    Dqn_U64ByteSizeType_TiB,
    Dqn_U64ByteSizeType_Count,
    Dqn_U64ByteSizeType_Auto,
};

struct Dqn_U64ByteSize
{
    Dqn_U64ByteSizeType type;
    Dqn_Str8            suffix; // "KiB", "MiB", "GiB" .. e.t.c
    Dqn_f64             bytes;
};

enum Dqn_U64AgeUnit
{
    Dqn_U64AgeUnit_Sec  = 1 << 0,
    Dqn_U64AgeUnit_Min  = 1 << 1,
    Dqn_U64AgeUnit_Hr   = 1 << 2,
    Dqn_U64AgeUnit_Day  = 1 << 3,
    Dqn_U64AgeUnit_Week = 1 << 4,
    Dqn_U64AgeUnit_Year = 1 << 5,
    Dqn_U64AgeUnit_HMS  = Dqn_U64AgeUnit_Sec | Dqn_U64AgeUnit_Min | Dqn_U64AgeUnit_Hr,
    Dqn_U64AgeUnit_All  = Dqn_U64AgeUnit_HMS | Dqn_U64AgeUnit_Day | Dqn_U64AgeUnit_Week | Dqn_U64AgeUnit_Year,
};

#if !defined(DQN_NO_PROFILER)
// NOTE: [$PROF] Dqn_Profiler //////////////////////////////////////////////////////////////////////
#if !defined(DQN_PROFILER_ANCHOR_BUFFER_SIZE)
    #define DQN_PROFILER_ANCHOR_BUFFER_SIZE 128
#endif

struct Dqn_ProfilerAnchor
{
    // Inclusive refers to the time spent to complete the function call
    // including all children functions.
    //
    // Exclusive refers to the time spent in the function, not including any
    // time spent in children functions that we call that are also being
    // profiled. If we recursively call into ourselves, the time we spent in
    // our function is accumulated.
    uint64_t tsc_inclusive;
    uint64_t tsc_exclusive;
    uint16_t hit_count;
    Dqn_Str8 name;
};

struct Dqn_ProfilerZone
{
    uint16_t anchor_index;
    uint64_t begin_tsc;
    uint16_t parent_zone;
    uint64_t elapsed_tsc_at_zone_start;
};

#if defined(__cplusplus)
struct Dqn_ProfilerZoneScope
{
    Dqn_ProfilerZoneScope(Dqn_Str8 name, uint16_t anchor_index);
    ~Dqn_ProfilerZoneScope();
    Dqn_ProfilerZone zone;
};
#define Dqn_Profiler_ZoneScope(name) auto DQN_UNIQUE_NAME(profile_zone_) = Dqn_ProfilerZoneScope(DQN_STR8(name), __COUNTER__ + 1)
#define Dqn_Profiler_ZoneScopeWithIndex(name, anchor_index) auto DQN_UNIQUE_NAME(profile_zone_) = Dqn_ProfilerZoneScope(DQN_STR8(name), anchor_index)
#endif

enum Dqn_ProfilerAnchorBuffer
{
    Dqn_ProfilerAnchorBuffer_Back,
    Dqn_ProfilerAnchorBuffer_Front,
};

struct Dqn_Profiler
{
    Dqn_ProfilerAnchor anchors[2][DQN_PROFILER_ANCHOR_BUFFER_SIZE];
    uint8_t            active_anchor_buffer;
    uint16_t           parent_zone;
};
#endif // !defined(DQN_NO_PROFILER)

// NOTE: [$JOBQ] Dqn_JobQueue ///////////////////////////////////////////////////////////////////////
typedef void (Dqn_JobQueueFunc)(void *user_context);
struct Dqn_Job
{
    bool              add_to_completion_queue;
    Dqn_Arena        *arena;
    Dqn_JobQueueFunc *func;
    uint32_t          user_tag;
    void             *user_context;
};

struct Dqn_JobQueueSPMC
{
    Dqn_OSMutex       mutex;
    Dqn_OSSemaphore   thread_wait_for_job_semaphore;
    Dqn_OSSemaphore   wait_for_completion_semaphore;
    uint32_t          threads_waiting_for_completion;

    Dqn_Job           jobs[64];
    Dqn_b32           quit;
    uint32_t          quit_exit_code;
    uint32_t volatile read_index;
    uint32_t volatile finish_index;
    uint32_t volatile write_index;

    Dqn_OSSemaphore   complete_queue_write_semaphore;
    Dqn_Job           complete_queue[64];
    uint32_t volatile complete_read_index;
    uint32_t volatile complete_write_index;
};

// NOTE: [$DLIB] Dqn_Library ///////////////////////////////////////////////////////////////////////
// Book-keeping data for the library and allow customisation of certain features
// provided.
struct Dqn_Library
{
    bool                       lib_init;                 // True if the library has been initialised via `Dqn_Library_Init`
    Dqn_TicketMutex            lib_mutex;
    Dqn_Str8                   exe_dir;                  // The directory of the current executable
    Dqn_Arena                  arena;
    Dqn_ChunkPool              pool;                     // Uses 'arena' for malloc-like allocations
    Dqn_ArenaCatalog           arena_catalog;
    bool                       slow_verification_checks; // Enable expensive library verification checks
    // NOTE: Logging ///////////////////////////////////////////////////////////////////////////////
    Dqn_LogProc *              log_callback;             // Set this pointer to override the logging routine
    void *                     log_user_data;
    bool                       log_to_file;              // Output logs to file as well as standard out
    Dqn_OSFile                 log_file;                 // TODO(dqn): Hmmm, how should we do this... ?
    Dqn_TicketMutex            log_file_mutex;           // Is locked when instantiating the log_file for the first time
    bool                       log_no_colour;            // Disable colours in the logging output
    // NOTE: Leak Tracing //////////////////////////////////////////////////////////////////////////
    #if defined(DQN_LEAK_TRACKING)
    Dqn_DSMap<Dqn_DebugAlloc>  alloc_table;
    Dqn_TicketMutex            alloc_table_mutex;
    Dqn_Arena                  alloc_table_arena;
    #endif
    // NOTE: Win32 /////////////////////////////////////////////////////////////////////////////////
    #if defined(DQN_OS_WIN32)
    LARGE_INTEGER              win32_qpc_frequency;
    Dqn_TicketMutex            win32_bcrypt_rng_mutex;
    void *                     win32_bcrypt_rng_handle;
    #endif
    bool                       win32_sym_initialised;
    // NOTE: OS ////////////////////////////////////////////////////////////////////////////////////
    uint32_t                   os_page_size;
    uint32_t                   os_alloc_granularity;
    // NOTE: Profiler //////////////////////////////////////////////////////////////////////////////
    #if !defined(DQN_NO_PROFILER)
    Dqn_Profiler *profiler;
    Dqn_Profiler  profiler_default_instance;
    #endif
} extern *g_dqn_library;

enum Dqn_LibraryOnInit
{
    Dqn_LibraryOnInit_Nil,
    Dqn_LibraryOnInit_LogFeatures,
};

// NOTE: [$PCGX] Dqn_PCG32 /////////////////////////////////////////////////////////////////////////
DQN_API Dqn_PCG32            Dqn_PCG32_Init                          (uint64_t seed);
DQN_API uint32_t             Dqn_PCG32_Next                          (Dqn_PCG32 *rng);
DQN_API uint64_t             Dqn_PCG32_Next64                        (Dqn_PCG32 *rng);
DQN_API uint32_t             Dqn_PCG32_Range                         (Dqn_PCG32 *rng, uint32_t low, uint32_t high);
DQN_API Dqn_f32              Dqn_PCG32_NextF32                       (Dqn_PCG32 *rng);
DQN_API Dqn_f64              Dqn_PCG32_NextF64                       (Dqn_PCG32 *rng);
DQN_API void                 Dqn_PCG32_Advance                       (Dqn_PCG32 *rng, uint64_t delta);

#if !defined(DQN_NO_JSON_BUILDER)
// NOTE: [$JSON] Dqn_JSONBuilder ///////////////////////////////////////////////////////////////////
#define Dqn_JSONBuilder_Object(builder)                  \
    DQN_DEFER_LOOP(Dqn_JSONBuilder_ObjectBegin(builder), \
                   Dqn_JSONBuilder_ObjectEnd(builder))

#define Dqn_JSONBuilder_ObjectNamed(builder, name)                  \
    DQN_DEFER_LOOP(Dqn_JSONBuilder_ObjectBeginNamed(builder, name), \
                   Dqn_JSONBuilder_ObjectEnd(builder))

#define Dqn_JSONBuilder_Array(builder)                  \
    DQN_DEFER_LOOP(Dqn_JSONBuilder_ArrayBegin(builder), \
                   Dqn_JSONBuilder_ArrayEnd(builder))

#define Dqn_JSONBuilder_ArrayNamed(builder, name)                  \
    DQN_DEFER_LOOP(Dqn_JSONBuilder_ArrayBeginNamed(builder, name), \
                   Dqn_JSONBuilder_ArrayEnd(builder))


DQN_API Dqn_JSONBuilder      Dqn_JSONBuilder_Init                    (Dqn_Arena *arena, int spaces_per_indent);
DQN_API Dqn_Str8             Dqn_JSONBuilder_Build                   (Dqn_JSONBuilder const *builder, Dqn_Arena *arena);
DQN_API void                 Dqn_JSONBuilder_KeyValue                (Dqn_JSONBuilder *builder, Dqn_Str8 key, Dqn_Str8 value);
DQN_API void                 Dqn_JSONBuilder_KeyValueF               (Dqn_JSONBuilder *builder, Dqn_Str8 key, char const *value_fmt, ...);
DQN_API void                 Dqn_JSONBuilder_ObjectBeginNamed        (Dqn_JSONBuilder *builder, Dqn_Str8 name);
DQN_API void                 Dqn_JSONBuilder_ObjectEnd               (Dqn_JSONBuilder *builder);
DQN_API void                 Dqn_JSONBuilder_ArrayBeginNamed         (Dqn_JSONBuilder *builder, Dqn_Str8 name);
DQN_API void                 Dqn_JSONBuilder_ArrayEnd                (Dqn_JSONBuilder *builder);
DQN_API void                 Dqn_JSONBuilder_Str8Named               (Dqn_JSONBuilder *builder, Dqn_Str8 key, Dqn_Str8 value);
DQN_API void                 Dqn_JSONBuilder_LiteralNamed            (Dqn_JSONBuilder *builder, Dqn_Str8 key, Dqn_Str8 value);
DQN_API void                 Dqn_JSONBuilder_U64Named                (Dqn_JSONBuilder *builder, Dqn_Str8 key, uint64_t value);
DQN_API void                 Dqn_JSONBuilder_I64Named                (Dqn_JSONBuilder *builder, Dqn_Str8 key, int64_t value);
DQN_API void                 Dqn_JSONBuilder_F64Named                (Dqn_JSONBuilder *builder, Dqn_Str8 key, double value, int decimal_places);
DQN_API void                 Dqn_JSONBuilder_BoolNamed               (Dqn_JSONBuilder *builder, Dqn_Str8 key, bool value);

#define                      Dqn_JSONBuilder_ObjectBegin(builder)    Dqn_JSONBuilder_ObjectBeginNamed(builder, DQN_STR8(""))
#define                      Dqn_JSONBuilder_ArrayBegin(builder)     Dqn_JSONBuilder_ArrayBeginNamed(builder, DQN_STR8(""))
#define                      Dqn_JSONBuilder_Str8(builder, value)    Dqn_JSONBuilder_Str8Named(builder, DQN_STR8(""), value)
#define                      Dqn_JSONBuilder_Literal(builder, value) Dqn_JSONBuilder_LiteralNamed(builder, DQN_STR8(""), value)
#define                      Dqn_JSONBuilder_U64(builder, value)     Dqn_JSONBuilder_U64Named(builder, DQN_STR8(""), value)
#define                      Dqn_JSONBuilder_I64(builder, value)     Dqn_JSONBuilder_I64Named(builder, DQN_STR8(""), value)
#define                      Dqn_JSONBuilder_F64(builder, value)     Dqn_JSONBuilder_F64Named(builder, DQN_STR8(""), value)
#define                      Dqn_JSONBuilder_Bool(builder, value)    Dqn_JSONBuilder_BoolNamed(builder, DQN_STR8(""), value)
#endif // !defined(DQN_NO_JSON_BUILDER)
#if !defined(DQN_NO_BIN)
// NOTE: [$BHEX] Dqn_Bin ///////////////////////////////////////////////////////////////////////////
// TODO(doyle): I'm not happy with this API. Its ugly and feels complicated
// because I designed it as a C-API first (e.g. ptr + length) vs just accepting
// that Dqn_Str8/Dqn_Slice is a superior API to design for first.
DQN_API char const *         Dqn_Bin_HexBufferTrim0x                 (char const *hex, Dqn_usize size, Dqn_usize *real_size);
DQN_API Dqn_Str8             Dqn_Bin_HexTrim0x                       (Dqn_Str8 string);

DQN_API Dqn_BinHexU64Str8    Dqn_Bin_U64ToHexU64Str8                 (uint64_t number, uint32_t flags);
DQN_API Dqn_Str8             Dqn_Bin_U64ToHex                        (Dqn_Arena *arena, uint64_t number, uint32_t flags);

DQN_API uint64_t             Dqn_Bin_HexToU64                        (Dqn_Str8 hex);
DQN_API uint64_t             Dqn_Bin_HexPtrToU64                     (char const *hex, Dqn_usize size);

DQN_API Dqn_Str8             Dqn_Bin_BytesToHexArena                 (Dqn_Arena *arena, void const *src, Dqn_usize size);
DQN_API char *               Dqn_Bin_BytesToHexBufferArena           (Dqn_Arena *arena, void const *src, Dqn_usize size);
DQN_API bool                 Dqn_Bin_BytesToHexBuffer                (void const *src, Dqn_usize src_size, char *dest, Dqn_usize dest_size);

DQN_API Dqn_usize            Dqn_Bin_HexBufferToBytesUnchecked       (char const *hex, Dqn_usize hex_size, void *dest, Dqn_usize dest_size);
DQN_API Dqn_usize            Dqn_Bin_HexBufferToBytes                (char const *hex, Dqn_usize hex_size, void *dest, Dqn_usize dest_size);
DQN_API char *               Dqn_Bin_HexBufferToBytesArena           (Dqn_Arena *arena, char const *hex, Dqn_usize hex_size, Dqn_usize *real_size);

DQN_API Dqn_usize            Dqn_Bin_HexToBytesUnchecked             (Dqn_Str8 hex, void *dest, Dqn_usize dest_size);
DQN_API Dqn_usize            Dqn_Bin_HexToBytes                      (Dqn_Str8 hex, void *dest, Dqn_usize dest_size);
DQN_API Dqn_Str8             Dqn_Bin_HexToBytesArena                 (Dqn_Arena *arena, Dqn_Str8 hex);
#endif // !defined(DQN_NO_BIN)

// NOTE: [$BSEA] Dqn_BinarySearch //////////////////////////////////////////////////////////////////
template <typename T> bool                   Dqn_BinarySearch_DefaultLessThan(T const &lhs, T const &rhs);
template <typename T> Dqn_BinarySearchResult Dqn_BinarySearch                (T const *array,
                                                                              Dqn_usize array_size,
                                                                              T const &find,
                                                                              Dqn_BinarySearchType type = Dqn_BinarySearchType_Match,
                                                                              Dqn_BinarySearchLessThanProc<T> less_than = Dqn_BinarySearch_DefaultLessThan);

// NOTE: [$BITS] Dqn_Bit ///////////////////////////////////////////////////////////////////////////
DQN_API void                Dqn_Bit_UnsetInplace                     (Dqn_usize *flags, Dqn_usize bitfield);
DQN_API void                Dqn_Bit_SetInplace                       (Dqn_usize *flags, Dqn_usize bitfield);
DQN_API bool                Dqn_Bit_IsSet                            (Dqn_usize bits, Dqn_usize bits_to_set);
DQN_API bool                Dqn_Bit_IsNotSet                         (Dqn_usize bits, Dqn_usize bits_to_check);

// NOTE: [$SAFE] Dqn_Safe //////////////////////////////////////////////////////////////////////////
DQN_API int64_t             Dqn_Safe_AddI64                          (int64_t a,  int64_t b);
DQN_API int64_t             Dqn_Safe_MulI64                          (int64_t a,  int64_t b);

DQN_API uint64_t            Dqn_Safe_AddU64                          (uint64_t a, uint64_t b);
DQN_API uint64_t            Dqn_Safe_MulU64                          (uint64_t a, uint64_t b);

DQN_API uint64_t            Dqn_Safe_SubU64                          (uint64_t a, uint64_t b);
DQN_API uint32_t            Dqn_Safe_SubU32                          (uint32_t a, uint32_t b);

DQN_API int                 Dqn_Safe_SaturateCastUSizeToInt          (Dqn_usize val);
DQN_API int8_t              Dqn_Safe_SaturateCastUSizeToI8           (Dqn_usize val);
DQN_API int16_t             Dqn_Safe_SaturateCastUSizeToI16          (Dqn_usize val);
DQN_API int32_t             Dqn_Safe_SaturateCastUSizeToI32          (Dqn_usize val);
DQN_API int64_t             Dqn_Safe_SaturateCastUSizeToI64          (Dqn_usize val);

DQN_API int                 Dqn_Safe_SaturateCastU64ToInt            (uint64_t val);
DQN_API int64_t             Dqn_Safe_SaturateCastU64ToI64            (uint64_t val);
DQN_API unsigned int        Dqn_Safe_SaturateCastU64ToUInt           (uint64_t val);
DQN_API uint8_t             Dqn_Safe_SaturateCastU64ToU8             (uint64_t val);
DQN_API uint16_t            Dqn_Safe_SaturateCastU64ToU16            (uint64_t val);
DQN_API uint32_t            Dqn_Safe_SaturateCastU64ToU32            (uint64_t val);

DQN_API uint8_t             Dqn_Safe_SaturateCastUSizeToU8           (Dqn_usize val);
DQN_API uint16_t            Dqn_Safe_SaturateCastUSizeToU16          (Dqn_usize val);
DQN_API uint32_t            Dqn_Safe_SaturateCastUSizeToU32          (Dqn_usize val);
DQN_API uint64_t            Dqn_Safe_SaturateCastUSizeToU64          (Dqn_usize val);

DQN_API int                 Dqn_Safe_SaturateCastISizeToInt          (Dqn_isize val);
DQN_API int8_t              Dqn_Safe_SaturateCastISizeToI8           (Dqn_isize val);
DQN_API int16_t             Dqn_Safe_SaturateCastISizeToI16          (Dqn_isize val);
DQN_API int32_t             Dqn_Safe_SaturateCastISizeToI32          (Dqn_isize val);
DQN_API int64_t             Dqn_Safe_SaturateCastISizeToI64          (Dqn_isize val);

DQN_API unsigned int        Dqn_Safe_SaturateCastISizeToUInt         (Dqn_isize val);
DQN_API uint8_t             Dqn_Safe_SaturateCastISizeToU8           (Dqn_isize val);
DQN_API uint16_t            Dqn_Safe_SaturateCastISizeToU16          (Dqn_isize val);
DQN_API uint32_t            Dqn_Safe_SaturateCastISizeToU32          (Dqn_isize val);
DQN_API uint64_t            Dqn_Safe_SaturateCastISizeToU64          (Dqn_isize val);

DQN_API Dqn_isize           Dqn_Safe_SaturateCastI64ToISize          (int64_t val);
DQN_API int8_t              Dqn_Safe_SaturateCastI64ToI8             (int64_t val);
DQN_API int16_t             Dqn_Safe_SaturateCastI64ToI16            (int64_t val);
DQN_API int32_t             Dqn_Safe_SaturateCastI64ToI32            (int64_t val);

DQN_API unsigned int        Dqn_Safe_SaturateCastI64ToUInt           (int64_t val);
DQN_API Dqn_isize           Dqn_Safe_SaturateCastI64ToUSize          (int64_t val);
DQN_API uint8_t             Dqn_Safe_SaturateCastI64ToU8             (int64_t val);
DQN_API uint16_t            Dqn_Safe_SaturateCastI64ToU16            (int64_t val);
DQN_API uint32_t            Dqn_Safe_SaturateCastI64ToU32            (int64_t val);
DQN_API uint64_t            Dqn_Safe_SaturateCastI64ToU64            (int64_t val);

DQN_API int8_t              Dqn_Safe_SaturateCastIntToI8             (int val);
DQN_API int16_t             Dqn_Safe_SaturateCastIntToI16            (int val);
DQN_API uint8_t             Dqn_Safe_SaturateCastIntToU8             (int val);
DQN_API uint16_t            Dqn_Safe_SaturateCastIntToU16            (int val);
DQN_API uint32_t            Dqn_Safe_SaturateCastIntToU32            (int val);
DQN_API uint64_t            Dqn_Safe_SaturateCastIntToU64            (int val);

// NOTE: [$MISC] Misc //////////////////////////////////////////////////////////////////////////////
DQN_API int                 Dqn_FmtBuffer3DotTruncate                (char *buffer, int size, DQN_FMT_ATTRIB char const *fmt, ...);
DQN_API Dqn_U64Str8         Dqn_U64ToStr8                            (uint64_t val, char separator);
DQN_API Dqn_U64ByteSize     Dqn_U64ToByteSize                        (uint64_t bytes, Dqn_U64ByteSizeType type);
DQN_API Dqn_Str8            Dqn_U64ToByteSizeStr8                    (Dqn_Arena *arena, uint64_t bytes, Dqn_U64ByteSizeType desired_type);
DQN_API Dqn_Str8            Dqn_U64ByteSizeTypeString                (Dqn_U64ByteSizeType type);
DQN_API Dqn_Str8            Dqn_U64ToAge                             (Dqn_Arena *arena, uint64_t age_s, Dqn_usize type);

// NOTE: [$PROF] Dqn_Profiler //////////////////////////////////////////////////////////////////////
#define                     Dqn_Profiler_BeginZone(name)             Dqn_Profiler_BeginZoneWithIndex(DQN_STR8(name), __COUNTER__ + 1)
DQN_API Dqn_ProfilerZone    Dqn_Profiler_BeginZoneWithIndex          (Dqn_Str8 name, uint16_t anchor_index);
DQN_API void                Dqn_Profiler_EndZone                     (Dqn_ProfilerZone zone);
DQN_API Dqn_ProfilerAnchor *Dqn_Profiler_AnchorBuffer                (Dqn_ProfilerAnchorBuffer buffer);
DQN_API void                Dqn_Profiler_SwapAnchorBuffer            ();
DQN_API void                Dqn_Profiler_Dump                        (uint64_t tsc_per_second);

// NOTE: [$JOBQ] Dqn_JobQueue ///////////////////////////////////////////////////////////////////////
DQN_API Dqn_JobQueueSPMC    Dqn_OS_JobQueueSPMCInit                  ();
DQN_API bool                Dqn_OS_JobQueueSPMCAddArray              (Dqn_JobQueueSPMC *queue, Dqn_Job *jobs, uint32_t count);
DQN_API bool                Dqn_OS_JobQueueSPMCAdd                   (Dqn_JobQueueSPMC *queue, Dqn_Job job);
DQN_API void                Dqn_OS_JobQueueSPMCWaitForCompletion     (Dqn_JobQueueSPMC *queue);
DQN_API int32_t             Dqn_OS_JobQueueSPMCThread                (Dqn_OSThread *thread);
DQN_API Dqn_usize           Dqn_OS_JobQueueSPMCGetFinishedJobs       (Dqn_JobQueueSPMC *queue, Dqn_Job *jobs, Dqn_usize jobs_size);

// NOTE: Dqn_Library ///////////////////////////////////////////////////////////////////////////////
DQN_API Dqn_Library *       Dqn_Library_Init                         (Dqn_LibraryOnInit on_init);
DQN_API void                Dqn_Library_SetPointer                   (Dqn_Library *library);
#if !defined(DQN_NO_PROFILER)
DQN_API void                Dqn_Library_SetProfiler                  (Dqn_Profiler *profiler);
#endif
DQN_API void                Dqn_Library_SetLogCallback               (Dqn_LogProc *proc, void *user_data);
DQN_API void                Dqn_Library_DumpThreadContextArenaStat   (Dqn_Str8 file_path);
DQN_API Dqn_Arena *         Dqn_Library_AllocArenaF                  (Dqn_usize reserve, Dqn_usize commit, uint8_t arena_flags, char const *fmt, ...);

// NOTE: [$BSEA] Dqn_BinarySearch //////////////////////////////////////////////////////////////////
template <typename T>
bool Dqn_BinarySearch_DefaultLessThan(T const &lhs, T const &rhs)
{
    bool result = lhs < rhs;
    return result;
}

template <typename T>
Dqn_BinarySearchResult Dqn_BinarySearch(T const                         *array,
                                        Dqn_usize                        array_size,
                                        T const                         &find,
                                        Dqn_BinarySearchType             type,
                                        Dqn_BinarySearchLessThanProc<T>  less_than)
{
    Dqn_BinarySearchResult result = {};
    if (!array || array_size <= 0)
        return result;

    T const *end   = array + array_size;
    T const *first = array;
    T const *last  = end;
    while (first != last) {
        Dqn_usize count = last - first;
        T const *it     = first + (count / 2);

        bool advance_first = false;
        if (type == Dqn_BinarySearchType_UpperBound)
            advance_first = !less_than(find, it[0]);
        else
            advance_first = less_than(it[0], find);

        if (advance_first)
            first = it + 1;
        else
            last  = it;
    }

    switch (type) {
        case Dqn_BinarySearchType_Match: {
            result.found = first != end && !less_than(find, *first);
        } break;

        case Dqn_BinarySearchType_LowerBound: /*FALLTHRU*/
        case Dqn_BinarySearchType_UpperBound: {
            result.found = first != end;
        } break;
    }

    result.index = first - array;
    return result;
}

