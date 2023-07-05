// NOTE: Table Of Contents =========================================================================
// Index                     | Disable #define | Description
// =================================================================================================
// [$DLIB] Dqn_Library       |                 | Library run-time behaviour configuration
// [$BITS] Dqn_Bit           |                 | Bitset manipulation
// [$SAFE] Dqn_Safe          |                 | Safe arithmetic, casts, asserts
// [$TCTX] Dqn_ThreadContext |                 | Per-thread data structure e.g. temp arenas
// [$BSEA] Dqn_BinarySearch  |                 | Binary search
// =================================================================================================

// NOTE: [$DLIB] Dqn_Library =======================================================================
// Book-keeping data for the library and allow customisation of certain features
// provided.
//
// NOTE: API =======================================================================================
// @proc Dqn_Library_SetLogCallback
//   @desc Update the default logging function, all logging functions will run through
//   this callback
//   @param[in] proc The new logging function, set to nullptr to revert back to
//   the default logger.
//   @param[in] user_data A user defined parameter to pass to the callback
//
// @proc Dqn_Library_SetLogFile
//   @param[in] file Pass in nullptr to turn off writing logs to disk, otherwise
//   point it to the FILE that you wish to write to.
//
// @proc Dqn_Library_DumpThreadContextArenaStat
//   @desc Dump the per-thread arena statistics to the specified file

struct Dqn_Library
{
    bool                     lib_init;
    Dqn_TicketMutex          lib_mutex;

    Dqn_LogProc             *log_callback;   ///< Set this pointer to override the logging routine
    void *                   log_user_data;
    bool                     log_to_file;    ///< Output logs to file as well as standard out
    void *                   log_file;       ///< TODO(dqn): Hmmm, how should we do this... ?
    Dqn_TicketMutex          log_file_mutex; ///< Is locked when instantiating the log_file for the first time
    bool                     log_no_colour;  ///< Disable colours in the logging output

    /// The backup arena to use if no arena is passed into Dqn_Library_Init
    Dqn_Arena                arena_catalog_backup_arena;
    Dqn_ArenaCatalog         arena_catalog;

    #if defined(DQN_LEAK_TRACING)
    Dqn_TicketMutex          alloc_table_mutex;
    Dqn_DSMap<Dqn_LeakTrace> alloc_table;
    #endif

    #if defined(DQN_OS_WIN32)
    LARGE_INTEGER            win32_qpc_frequency;
    Dqn_TicketMutex          win32_bcrypt_rng_mutex;
    void                    *win32_bcrypt_rng_handle;
    #endif

    #if defined(DQN_DEBUG_THREAD_CONTEXT)
    Dqn_TicketMutex          thread_context_mutex;
    Dqn_ArenaStat            thread_context_arena_stats[256];
    uint8_t                  thread_context_arena_stats_count;
    #endif
} extern dqn_library;

// NOTE: Properties ================================================================================
DQN_API Dqn_Library *Dqn_Library_Init                      (Dqn_Arena *arena);
DQN_API void         Dqn_Library_SetLogCallback            (Dqn_LogProc *proc, void *user_data);
DQN_API void         Dqn_Library_SetLogFile                (void *file);
DQN_API void         Dqn_Library_DumpThreadContextArenaStat(Dqn_String8 file_path);

// NOTE: Leak Trace ================================================================================
#if defined(DQN_LEAK_TRACING)
DQN_API void Dqn_Library_LeakTraceAdd              (Dqn_CallSite call_site, void *ptr, Dqn_usize size);
DQN_API void Dqn_Library_LeakTraceMarkFree         (Dqn_CallSite call_site, void *ptr);
#else
#define      Dqn_Library_LeakTraceAdd(...)
#define      Dqn_Library_LeakTraceMarkFree(...)
#endif

// NOTE: [$BITS] Dqn_Bit ===========================================================================
DQN_API void Dqn_Bit_UnsetInplace(uint32_t *flags, uint32_t bitfield);
DQN_API void Dqn_Bit_SetInplace  (uint32_t *flags, uint32_t bitfield);
DQN_API bool Dqn_Bit_IsSet       (uint32_t bits, uint32_t bits_to_set);
DQN_API bool Dqn_Bit_IsNotSet    (uint32_t bits, uint32_t bits_to_check);

// NOTE: [$SAFE] Dqn_Safe ==========================================================================
// @proc Dqn_Safe_AddI64, Dqn_Safe_MulI64
//   @desc Add/multiply 2 I64's together, safe asserting that the operation does 
//   not overflow.
//   @return The result of operation, INT64_MAX if it overflowed.

// @proc Dqn_Safe_AddU64, Dqn_Safe_MulU64
//   @desc Add/multiply 2 U64's together, safe asserting that the operation does 
//   not overflow.
//   @return The result of operation, UINT64_MAX if it overflowed.

// @proc Dqn_Safe_SubU64, Dqn_Safe_SubU32
//   @desc Subtract 2xU64 or 2xU32's together, safe asserting that the operation
//   does not overflow.
//   @return The result of operation, 0 if it overflowed.

// @proc Dqn_Safe_SaturateCastUSizeToInt,
//       Dqn_Safe_SaturateCastUSizeToI8,
//       Dqn_Safe_SaturateCastUSizeToI16,
//       Dqn_Safe_SaturateCastUSizeToI32,
//       Dqn_Safe_SaturateCastUSizeToI64
//
//       Dqn_Safe_SaturateCastU64ToUInt,
//       Dqn_Safe_SaturateCastU64ToU8,
//       Dqn_Safe_SaturateCastU64ToU16,
//       Dqn_Safe_SaturateCastU64ToU32,
//
//       Dqn_Safe_SaturateCastUSizeToU8,
//       Dqn_Safe_SaturateCastUSizeToU16,
//       Dqn_Safe_SaturateCastUSizeToU32,
//       Dqn_Safe_SaturateCastUSizeToU64
//
//       Dqn_Safe_SaturateCastISizeToInt,
//       Dqn_Safe_SaturateCastISizeToI8,
//       Dqn_Safe_SaturateCastISizeToI16,
//       Dqn_Safe_SaturateCastISizeToI32,
//       Dqn_Safe_SaturateCastISizeToI64,
//
//       int Dqn_Safe_SaturateCastISizeToUInt,
//       Dqn_Safe_SaturateCastISizeToU8,
//       Dqn_Safe_SaturateCastISizeToU16,
//       Dqn_Safe_SaturateCastISizeToU32,
//       Dqn_Safe_SaturateCastISizeToU64,
//
//       Dqn_Safe_SaturateCastI64ToISize,
//       Dqn_Safe_SaturateCastI64ToI8,
//       Dqn_Safe_SaturateCastI64ToI16,
//       Dqn_Safe_SaturateCastI64ToI32,
//
//       Dqn_Safe_SaturateCastIntToI8,
//       Dqn_Safe_SaturateCastIntToU8,
//       Dqn_Safe_SaturateCastIntToU16,
//       Dqn_Safe_SaturateCastIntToU32,
//       Dqn_Safe_SaturateCastIntToU64,
//
//   @desc Truncate the lhs operand to the right clamping the result to the max
//   value of the desired data type. Safe asserts if clamping occurs.
//
//   The following sentinel values are returned when saturated,
//   USize -> Int:  INT_MAX
//   USize -> I8:   INT8_MAX
//   USize -> I16:  INT16_MAX
//   USize -> I32:  INT32_MAX
//   USize -> I64:  INT64_MAX
//
//   U64   -> UInt: UINT_MAX
//   U64   -> U8:   UINT8_MAX
//   U64   -> U16:  UINT16_MAX
//   U64   -> U32:  UINT32_MAX
//
//   USize -> U8:   UINT8_MAX
//   USize -> U16:  UINT16_MAX
//   USize -> U32:  UINT32_MAX
//   USize -> U64:  UINT64_MAX
//
//   ISize -> Int:  INT_MIN   or INT_MAX
//   ISize -> I8:   INT8_MIN  or INT8_MAX
//   ISize -> I16:  INT16_MIN or INT16_MAX
//   ISize -> I32:  INT32_MIN or INT32_MAX
//   ISize -> I64:  INT64_MIN or INT64_MAX
//
//   ISize -> UInt: 0 or UINT_MAX
//   ISize -> U8:   0 or UINT8_MAX
//   ISize -> U16:  0 or UINT16_MAX
//   ISize -> U32:  0 or UINT32_MAX
//   ISize -> U64:  0 or UINT64_MAX
//
//   I64 -> ISize:  DQN_ISIZE_MIN or DQN_ISIZE_MAX
//   I64 -> I8:     INT8_MIN      or INT8_MAX
//   I64 -> I16:    INT16_MIN     or INT16_MAX
//   I64 -> I32:    INT32_MIN     or INT32_MAX
//
//   Int -> I8:     INT8_MIN  or INT8_MAX
//   Int -> I16:    INT16_MIN or INT16_MAX
//   Int -> U8:     0         or UINT8_MAX
//   Int -> U16:    0         or UINT16_MAX
//   Int -> U32:    0         or UINT32_MAX
//   Int -> U64:    0         or UINT64_MAX

DQN_API int64_t      Dqn_Safe_AddI64                 (int64_t a,  int64_t b);
DQN_API int64_t      Dqn_Safe_MulI64                 (int64_t a,  int64_t b);

DQN_API uint64_t     Dqn_Safe_AddU64                 (uint64_t a, uint64_t b);
DQN_API uint64_t     Dqn_Safe_MulU64                 (uint64_t a, uint64_t b);

DQN_API uint64_t     Dqn_Safe_SubU64                 (uint64_t a, uint64_t b);
DQN_API uint32_t     Dqn_Safe_SubU32                 (uint32_t a, uint32_t b);

DQN_API int          Dqn_Safe_SaturateCastUSizeToInt (Dqn_usize val);
DQN_API int8_t       Dqn_Safe_SaturateCastUSizeToI8  (Dqn_usize val);
DQN_API int16_t      Dqn_Safe_SaturateCastUSizeToI16 (Dqn_usize val);
DQN_API int32_t      Dqn_Safe_SaturateCastUSizeToI32 (Dqn_usize val);
DQN_API int64_t      Dqn_Safe_SaturateCastUSizeToI64 (Dqn_usize val);

DQN_API unsigned int Dqn_Safe_SaturateCastU64ToUInt  (uint64_t val);
DQN_API uint8_t      Dqn_Safe_SaturateCastU64ToU8    (uint64_t val);
DQN_API uint16_t     Dqn_Safe_SaturateCastU64ToU16   (uint64_t val);
DQN_API uint32_t     Dqn_Safe_SaturateCastU64ToU32   (uint64_t val);

DQN_API uint8_t      Dqn_Safe_SaturateCastUSizeToU8  (Dqn_usize val);
DQN_API uint16_t     Dqn_Safe_SaturateCastUSizeToU16 (Dqn_usize val);
DQN_API uint32_t     Dqn_Safe_SaturateCastUSizeToU32 (Dqn_usize val);
DQN_API uint64_t     Dqn_Safe_SaturateCastUSizeToU64 (Dqn_usize val);

DQN_API int          Dqn_Safe_SaturateCastISizeToInt (Dqn_isize val);
DQN_API int8_t       Dqn_Safe_SaturateCastISizeToI8  (Dqn_isize val);
DQN_API int16_t      Dqn_Safe_SaturateCastISizeToI16 (Dqn_isize val);
DQN_API int32_t      Dqn_Safe_SaturateCastISizeToI32 (Dqn_isize val);
DQN_API int64_t      Dqn_Safe_SaturateCastISizeToI64 (Dqn_isize val);

DQN_API unsigned int Dqn_Safe_SaturateCastISizeToUInt(Dqn_isize val);
DQN_API uint8_t      Dqn_Safe_SaturateCastISizeToU8  (Dqn_isize val);
DQN_API uint16_t     Dqn_Safe_SaturateCastISizeToU16 (Dqn_isize val);
DQN_API uint32_t     Dqn_Safe_SaturateCastISizeToU32 (Dqn_isize val);
DQN_API uint64_t     Dqn_Safe_SaturateCastISizeToU64 (Dqn_isize val);

DQN_API Dqn_isize    Dqn_Safe_SaturateCastI64ToISize (int64_t val);
DQN_API int8_t       Dqn_Safe_SaturateCastI64ToI8    (int64_t val);
DQN_API int16_t      Dqn_Safe_SaturateCastI64ToI16   (int64_t val);
DQN_API int32_t      Dqn_Safe_SaturateCastI64ToI32   (int64_t val);

DQN_API int8_t       Dqn_Safe_SaturateCastIntToI8    (int val);
DQN_API int16_t      Dqn_Safe_SaturateCastIntToI16   (int val);
DQN_API uint8_t      Dqn_Safe_SaturateCastIntToU8    (int val);
DQN_API uint16_t     Dqn_Safe_SaturateCastIntToU16   (int val);
DQN_API uint32_t     Dqn_Safe_SaturateCastIntToU32   (int val);
DQN_API uint64_t     Dqn_Safe_SaturateCastIntToU64   (int val);

// NOTE: [$TCTX] Dqn_ThreadContext =================================================================
// Each thread is assigned in their thread-local storage (TLS) scratch and
// permanent arena allocators. These can be used for allocations with a lifetime
// scoped to the lexical scope or for storing data permanently using the arena
// paradigm.
//
// TLS in this implementation is implemented using the `thread_local` C/C++
// keyword.
//
// NOTE: API
//
// @proc Dqn_Thread_GetContext
//   @desc Get the current thread's context- this contains all the metadata for managing
//   the thread scratch data. In general you probably want Dqn_Thread_GetScratch()
//   which ensures you get a usable scratch arena for temporary allocations
//   without having to worry about selecting the right arena from the state.
//
// @proc Dqn_Thread_GetScratch
//   @desc Retrieve the per-thread temporary arena allocator that is reset on scope
//   exit.
//
//   The scratch arena must be deconflicted with any existing arenas in the
//   function to avoid trampling over each other's memory. Consider the situation
//   where the scratch arena is passed into the function. Inside the function, if
//   the same arena is reused then, if both arenas allocate, when the inner arena
//   is reset, this will undo the passed in arena's allocations in the function.
//
//   @param[in] conflict_arena A pointer to the arena currently being used in the
//   function

#if !defined(DQN_THREAD_CONTEXT_ARENAS)
    #define DQN_THREAD_CONTEXT_ARENAS 2
#endif

struct Dqn_ThreadContext
{
    Dqn_b32        init;

    Dqn_Arena     *arena;     ///< Per thread arena
    Dqn_Allocator  allocator; ///< Allocator that uses the arena

    /// Temp memory arena's for the calling thread
    Dqn_Arena     *temp_arenas[DQN_THREAD_CONTEXT_ARENAS];

    /// Allocators that use the corresponding arena from the thread context.
    /// Provided for convenience when interfacing with allocator interfaces.
    Dqn_Allocator  temp_allocators[DQN_THREAD_CONTEXT_ARENAS];

    #if defined(DQN_DEBUG_THREAD_CONTEXT)
    Dqn_ArenaStat  temp_arenas_stat[DQN_THREAD_CONTEXT_ARENAS];
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
    #if defined(DQN_LEAK_TRACING)
    Dqn_CallSite         leak_site__;
    #endif
};

// NOTE: Context ===================================================================================
#define                    Dqn_Thread_GetContext()                  Dqn_Thread_GetContext_(DQN_LEAK_TRACE_NO_COMMA)
#define                    Dqn_Thread_GetScratch(conflict_arena)    Dqn_Thread_GetScratch_(DQN_LEAK_TRACE conflict_arena)
DQN_API uint32_t           Dqn_Thread_GetID();

// NOTE: Internal ==================================================================================
DQN_API Dqn_ThreadContext *Dqn_Thread_GetContext_(DQN_LEAK_TRACE_FUNCTION_NO_COMMA);
DQN_API Dqn_ThreadScratch  Dqn_Thread_GetScratch_(DQN_LEAK_TRACE_FUNCTION void const *conflict_arena);

// NOTE: [$BSEA] Dqn_BinarySearch ==================================================================
template <typename T>
using Dqn_BinarySearchLessThanProc = bool(T const &lhs, T const &rhs);

template <typename T>
DQN_FORCE_INLINE bool Dqn_BinarySearch_DefaultLessThan(T const &lhs, T const &rhs);

enum Dqn_BinarySearchType
{
    /// Index of the match. If no match is found, found is set to false and the
    /// index is set to 0
    Dqn_BinarySearchType_Match,

    /// Index after the match. If no match is found, found is set to false and
    /// the index is set to one past the closest match.
    Dqn_BinarySearchType_OnePastMatch,
};

struct Dqn_BinarySearchResult
{
    bool      found;
    Dqn_usize index;
};

template <typename T>
Dqn_BinarySearchResult
Dqn_BinarySearch(T const                        *array,
                 Dqn_usize                       array_size,
                 T const                        &find,
                 Dqn_BinarySearchType            type      = Dqn_BinarySearchType_Match,
                 Dqn_BinarySearchLessThanProc<T> less_than = Dqn_BinarySearch_DefaultLessThan);


template <typename T> DQN_FORCE_INLINE bool Dqn_BinarySearch_DefaultLessThan(T const &lhs, T const &rhs)
{
    bool result = lhs < rhs;
    return result;
}

template <typename T>
Dqn_BinarySearchResult
Dqn_BinarySearch(T const                        *array,
                 Dqn_usize                       array_size,
                 T const                        &find,
                 Dqn_BinarySearchType            type,
                 Dqn_BinarySearchLessThanProc<T> less_than)
{
    Dqn_BinarySearchResult result = {};
    Dqn_usize head                = 0;
    Dqn_usize tail                = array_size - 1;
    if (array && array_size > 0) {
        while (!result.found && head <= tail) {
            Dqn_usize mid  = (head + tail) / 2;
            T const &value = array[mid];
            if (less_than(find, value)) {
                tail = mid - 1;
                if (mid == 0)
                    break;
            } else if (less_than(value, find)) {
                head = mid + 1;
            } else {
                result.found = true;
                result.index = mid;
            }
        }
    }

    if (type == Dqn_BinarySearchType_OnePastMatch)
        result.index = result.found ? result.index + 1 : tail + 1;
    else
        DQN_ASSERT(type == Dqn_BinarySearchType_Match);

    return result;
}

