#if !defined(DQN_NO_VARRAY)
// NOTE: [$VARR] Dqn_VArray ========================================================================
// An array that is backed by virtual memory by reserving addressing space and
// comitting pages as items are allocated in the array. This array never
// reallocs, instead you should reserve the upper bound of the memory you will
// possibly ever need (e.g. 16GB) and let the array commit physical pages on
// demand. On 64 bit operating systems you are given 48 bits of addressable
// space giving you 256 TB of reservable memory. This gives you practically
// an unlimited array capacity that avoids reallocs and only consumes memory
// that is actually occupied by the array.
//
// Each page that is committed into the array will be at page/allocation
// granularity which are always cache aligned. This array essentially retains
// all the benefits of normal arrays,
//
// - contiguous memory
// - O(1) random access
// - O(N) iterate
//
// In addition to no realloc on expansion or shrinking.
//
// TODO(doyle): Add an API for shrinking the array by decomitting pages back to 
// the OS.
//
// NOTE: API
// @proc Dqn_VArray_InitByteSize, Dqn_VArray_Init
//   @desc Initialise an array with the requested byte size or item capacity
//   respectively. The returned array may have a higher capacity than the
//   requested amount since requested memory from the OS may have a certain
//   alignment requirement (e.g. on Windows reserve/commit are 64k/4k aligned).

// @proc Dqn_VArray_IsValid
//   @desc Verify if the array has been initialised

// @proc Dqn_VArray_Make, Dqn_VArray_Add
//   @desc Allocate items into the array
//     'Make' creates the `count` number of requested items
//     'Add' adds the array of items into the array
//   @return The array of items allocated. Null pointer if the array is invalid
//   or the array has insufficient space for the requested items.

// @proc Dqn_VArray_EraseRange
//   @desc Erase the next `count` items at `begin_index` in the array. `count`
//   can be positive or negative which dictates the if we erase forward from the
//   `begin_index` or in reverse.
//
//   This operation will invalidate all pointers to the array!
//
//   @param erase The erase method, stable erase will shift all elements after
//   the erase ranged into the range. Unstable erase will copy the tail elements
//   into the range to delete.

// @proc Dqn_VArray_Clear
//   @desc Set the size of the array to 0

// @proc Dqn_VArray_Reserve
//   @desc Ensure that the requested number of items are backed by physical
//   pages from the OS. Calling this pre-emptively will minimise syscalls into
//   the kernel to request memory. The requested items will be rounded up to the
//   in bytes to the allocation granularity of OS allocation APIs hence the
//   reserved space may be greater than the requested amount (e.g. this is 4k
//   on Windows).

template <typename T> struct Dqn_VArray
{
    Dqn_MemBlock *block; // Block of memory from the allocator for this array
    T            *data;  // Pointer to the start of the array items in the block of memory
    Dqn_usize     size;  // Number of items currently in the array
    Dqn_usize     max;   // Maximum number of items this array can store

    T       *begin()       { return data; }
    T       *end  ()       { return data + size; }
    T const *begin() const { return data; }
    T const *end  () const { return data + size; }
};

enum Dqn_VArrayErase
{
    Dqn_VArrayErase_Unstable,
    Dqn_VArrayErase_Stable,
};

// NOTE: Setup =====================================================================================
DQN_API template <typename T> Dqn_VArray<T> Dqn_VArray_InitByteSize(Dqn_Arena *arena, Dqn_usize byte_size);
DQN_API template <typename T> Dqn_VArray<T> Dqn_VArray_Init        (Dqn_Arena *arena, Dqn_usize max);
DQN_API template <typename T> bool          Dqn_VArray_IsValid     (Dqn_VArray<T> const *array);
DQN_API template <typename T> void          Dqn_VArray_Reserve     (Dqn_VArray<T> *array, Dqn_usize count);

// NOTE: Insert ====================================================================================
DQN_API template <typename T> T *           Dqn_VArray_Make        (Dqn_VArray<T> *array, Dqn_usize count, Dqn_ZeroMem zero_mem);
DQN_API template <typename T> T *           Dqn_VArray_Add         (Dqn_VArray<T> *array, T const *items, Dqn_usize count);

// NOTE: Modify ====================================================================================
DQN_API template <typename T> void          Dqn_VArray_EraseRange  (Dqn_VArray<T> *array, Dqn_usize begin_index, Dqn_isize count, Dqn_VArrayErase erase);
DQN_API template <typename T> void          Dqn_VArray_Clear       (Dqn_VArray<T> *array);
#endif // !defined(DQN_NO_VARRAY)

#if !defined(DQN_NO_DSMAP)
// NOTE: [$DMAP] Dqn_DSMap =========================================================================
// A hash table configured using the presets recommended by Demitri Spanos
// from the Handmade Network (HMN),
//
// - power of two capacity
// - grow by 2x on load >= 75%
// - open-addressing with linear probing
// - separate large values (esp. variable length values) into a separate table
// - use a well-known hash function: MurmurHash3 (or xxhash, city, spooky ...)
// - chain-repair on delete (rehash items in the probe chain after delete)
// - shrink by 1/2 on load < 25% (suggested by Martins Mmozeiko of HMN)
//
// Source: discord.com/channels/239737791225790464/600063880533770251/941835678424129597
//
// This hash-table stores slots (values) separate from the hash mapping. Hashes
// are mapped to slots using the hash-to-slot array which is an array of slot
// indexes. This array intentionally only stores indexes to maximise usage
// of the cache line. Linear probing on collision will only cost a couple of
// cycles to fetch from L1 cache the next slot index to attempt.
//
// The slots array stores values contiguously, non-sorted allowing iteration of
// the map. On element erase, the last element is swapped into the deleted
// element causing the non-sorted property of this table.
//
// The 0th slot (DQN_DS_MAP_SENTINEL_SLOT) in the slots array is reserved for a
// sentinel value, e.g. all zeros value. After map initialisation the 'occupied'
// value of the array will be set to 1 to exclude the sentinel from the
// capacity of the table. Skip the first value if you are iterating the hash
// table!
//
// This hash-table accept either a U64 or a buffer (ptr + len) as the key. In
// practice this covers a majority of use cases (with string, buffer and number
// keys). It also allows us to minimise our C++ templates to only require 1
// variable which is the Value part of the hash-table simplifying interface
// complexity and cruft brought by C++.
//
// Keys are value-copied into the hash-table. If the key uses a pointer to a
// buffer, this buffer must be valid throughout the lifetime of the hash table!
//
// NOTE: API
//
// - All maps must be created by calling `DSMap_Init()` with the desired size
// and the memory allocated for the table can be freed by called
// `DSMap_Deinit()`.
//
// - Functions that return a pointer or boolean will always return null or false
// if the passed in map is invalid e.g. `DSMap_IsValid()` returns false.

// @proc Dqn_DSMap_Init
//   @param size[in] The number of slots in the table. This size must be a
//   power-of-two or otherwise an assert will be triggered.
//   @return The hash table. On memory allocation failure, the table will be
//   zero initialised whereby calling Dqn_DSMap_IsValid() will return false.

// @proc Dqn_DSMap_Deinit
//   @desc Free the memory allocated by the table

// @proc Dqn_DSMap_IsValid
//   @desc Verify that the table is in a valid state (e.g. initialised 
//   correctly).

// @proc Dqn_DSMap_Hash
//   @desc Hash the input key using the custom hash function if it's set on the
//   map, otherwise uses the default hashing function (32bit Murmur3).
//
// @proc Dqn_DSMap_HashToSlotIndex
//   @desc Calculate the index into the map's `slots` array from the given hash.

// @proc Dqn_DSMap_FindSlot, Dqn_DSMap_Find
//   @desc Find the slot in the map's `slots` array corresponding to the given
//   key and hash. If the map does not contain the key, a null pointer is
//   returned.
//
//   `Find`     returns the value.
//   `FindSlot` returns the map's hash table slot.

// @proc Dqn_DSMap_MakeSlot, Dqn_DSMap_Make, Dqn_DSMap_Set, Dqn_DSMap_SetSlot
//   @desc Same as `DSMap_Find*` except if the key does not exist in the table,
//   a hash-table slot will be made.
//
//   `Make`     assigns the key       to the table and returns the hash table slot's value.
//   `Set`      assigns the key-value to the table and returns the hash table slot's value.
//   `MakeSlot` assigns the key       to the table and returns the hash table slot.
//   `SetSlot`  assigns the key-value to the table and returns the hash table slot.
//
//   If by adding the key-value pair to the table puts the table over 75% load,
//   the table will be grown to 2x the current the size before insertion
//   completes.
//
//   @param found[out] Pass a pointer to a bool. The bool will be set to true
//   if the item already existed in the map before, or false if the item was
//   just created by this call.

// @proc Dqn_DSMap_Resize
//   @desc Resize the table and move all elements to the new map.
//   the elements currently set in the
//   @param size[in] New size of the table, must be a power of two.
//   @return False if memory allocation fails, or the requested size is smaller
//   than the current number of elements in the map to resize. True otherwise.

// @proc Dqn_DSMap_Erase
//   @desc Remove the key-value pair from the table. If by erasing the key-value
//   pair from the table puts the table under 25% load, the table will be shrunk
//   by 1/2 the current size after erasing. The table will not shrink below the
//   initial size that the table was initialised as.

// @proc Dqn_DSMap_KeyCStringLit, Dqn_DSMap_KeyU64, Dqn_DSMap_KeyBuffer,
// Dqn_DSMap_KeyString8 Dqn_DSMap_KeyString8Copy
//   @desc Create a hash-table key given
//
//   `KeyCStringLit`  a cstring literal
//   `KeyU64`         a u64
//   `KeyBuffer`      a (ptr+len) slice of bytes
//   `KeyString8`     a Dqn_String8 string
//   `KeyString8Copy` a Dqn_String8 string that is copied first using the allocator
//
//   If the key points to an array of bytes, the lifetime of those bytes *must*
//   remain valid throughout the lifetime of the map as the pointers are value
//   copied into the hash table!

// @proc Dqn_DSMap_KeyU64NoHash
//   @desc Create a hash-table key given the uint64. This u64 is *not* hashed to
//   map values into the table. This is useful if you already have a source of
//   data that is already sufficiently uniformly distributed already (e.g.
//   using 8 bytes taken from a SHA256 hash as the key).
//
//   This value will be used directly but truncated to 32 bits as the table uses
//   32 bit hashes for mapping keys to values.

enum Dqn_DSMapKeyType
{
    Dqn_DSMapKeyType_Invalid,
    Dqn_DSMapKeyType_U64,       // Use a U64 key that is `hash(u64) % size` to map into the table
    Dqn_DSMapKeyType_U64NoHash, // Use a U64 key that is `u64 % size` to map into the table
    Dqn_DSMapKeyType_Buffer,    // Use a buffer key that is `hash(buffer) % size` to map into the table
};

struct Dqn_DSMapKey
{
    Dqn_DSMapKeyType type;
    uint32_t hash;
    union Payload {
        struct Buffer {
            void const *data;
            uint32_t    size;
        } buffer;
        uint64_t u64;
    } payload;
};

template <typename T> struct Dqn_DSMapSlot
{
    Dqn_DSMapKey key;   ///< Hash table lookup key
    T            value; ///< Hash table value
};

using Dqn_DSMapHashFunction = uint32_t(Dqn_DSMapKey key, uint32_t seed);
template <typename T> struct Dqn_DSMap
{
    uint32_t              *hash_to_slot;  // Mapping from hash to a index in the slots array
    Dqn_DSMapSlot<T>      *slots;         // Values of the array stored contiguously, non-sorted order
    uint32_t               size;          // Total capacity of the map and is a power of two
    uint32_t               occupied;      // Number of slots used in the hash table
    Dqn_Allocator          allocator;     // Backing allocator for the hash table
    uint32_t               initial_size;  // Initial map size, map cannot shrink on erase below this size
    Dqn_DSMapHashFunction *hash_function; // Custom hashing function to use if field is set
    uint32_t               hash_seed;     // Seed for the hashing function, when 0, DQN_DS_MAP_DEFAULT_HASH_SEED is used
};

// NOTE: Setup =====================================================================================
DQN_API template <typename T> Dqn_DSMap<T>      Dqn_DSMap_Init           (uint32_t size);
DQN_API template <typename T> void              Dqn_DSMap_Deinit         (Dqn_DSMap<T> *map);
DQN_API template <typename T> bool              Dqn_DSMap_IsValid        (Dqn_DSMap<T> const *map);

// NOTE: Hash ======================================================================================
DQN_API template <typename T> uint32_t          Dqn_DSMap_Hash           (Dqn_DSMap<T> const *map, Dqn_DSMapKey key);
DQN_API template <typename T> uint32_t          Dqn_DSMap_HashToSlotIndex(Dqn_DSMap<T> const *map, Dqn_DSMapKey key);

// NOTE: Insert ====================================================================================
DQN_API template <typename T> Dqn_DSMapSlot<T> *Dqn_DSMap_FindSlot       (Dqn_DSMap<T> const *map, Dqn_DSMapKey key);
DQN_API template <typename T> Dqn_DSMapSlot<T> *Dqn_DSMap_MakeSlot       (Dqn_DSMap<T> *map, Dqn_DSMapKey key, bool *found);
DQN_API template <typename T> Dqn_DSMapSlot<T> *Dqn_DSMap_SetSlot        (Dqn_DSMap<T> *map, Dqn_DSMapKey key, T const &value, bool *found);
DQN_API template <typename T> T *               Dqn_DSMap_Find           (Dqn_DSMap<T> const *map, Dqn_DSMapKey key);
DQN_API template <typename T> T *               Dqn_DSMap_Make           (Dqn_DSMap<T> *map, Dqn_DSMapKey key, bool *found);
DQN_API template <typename T> T *               Dqn_DSMap_Set            (Dqn_DSMap<T> *map, Dqn_DSMapKey key, T const &value, bool *found);
DQN_API template <typename T> bool              Dqn_DSMap_Resize         (Dqn_DSMap<T> *map, uint32_t size);
DQN_API template <typename T> bool              Dqn_DSMap_Erase          (Dqn_DSMap<T> *map, Dqn_DSMapKey key);

// NOTE: Table Keys ================================================================================
DQN_API template <typename T> Dqn_DSMapKey      Dqn_DSMap_KeyBuffer      (Dqn_DSMap<T> const *map, void const *data, uint32_t size);
DQN_API template <typename T> Dqn_DSMapKey      Dqn_DSMap_KeyU64         (Dqn_DSMap<T> const *map, uint64_t u64);
DQN_API template <typename T> Dqn_DSMapKey      Dqn_DSMap_KeyString8     (Dqn_DSMap<T> const *map, Dqn_String8 string);
DQN_API template <typename T> Dqn_DSMapKey      Dqn_DSMap_KeyString8Copy (Dqn_DSMap<T> const *map, Dqn_Allocator allocator, Dqn_String8 string);
#define                                         Dqn_DSMap_KeyCStringLit(map, string) Dqn_DSMap_KeyBuffer(map, string, sizeof((string))/sizeof((string)[0]) - 1)
DQN_API Dqn_DSMapKey                            Dqn_DSMap_KeyU64NoHash   (uint64_t u64);
DQN_API bool                                    Dqn_DSMap_KeyEquals      (Dqn_DSMapKey lhs, Dqn_DSMapKey rhs);
DQN_API bool                                    operator==               (Dqn_DSMapKey lhs, Dqn_DSMapKey rhs);
#endif // !defined(DQN_NO_DSMAP)

#if !defined(DQN_NO_FARRAY)
// NOTE: [$FARR] Dqn_FArray ========================================================================
template <typename T, Dqn_usize N> struct Dqn_FArray
{
    T         data[N]; // Pointer to the start of the array items in the block of memory
    Dqn_usize size;    // Number of items currently in the array

    T       *begin()       { return data; }
    T       *end  ()       { return data + size; }
    T const *begin() const { return data; }
    T const *end  () const { return data + size; }
};

enum Dqn_FArrayErase
{
    Dqn_FArrayErase_Unstable,
    Dqn_FArrayErase_Stable,
};

// NOTE: Setup =====================================================================================
DQN_API template <typename T, Dqn_usize N> Dqn_FArray<T, N> Dqn_FArray_Init      (T const *array, Dqn_usize count);
DQN_API template <typename T, Dqn_usize N> bool             Dqn_FArray_IsValid   (Dqn_FArray<T, N> const *array);

// NOTE: Insert ====================================================================================
DQN_API template <typename T, Dqn_usize N> T *              Dqn_FArray_Make      (Dqn_FArray<T, N> *array, Dqn_usize count, Dqn_ZeroMem zero_mem);
DQN_API template <typename T, Dqn_usize N> T *              Dqn_FArray_Add       (Dqn_FArray<T, N> *array, T const *items, Dqn_usize count);

// NOTE: Modify ====================================================================================
DQN_API template <typename T, Dqn_usize N> void             Dqn_FArray_EraseRange(Dqn_FArray<T, N> *array, Dqn_usize begin_index, Dqn_isize count, Dqn_FArrayErase erase);
DQN_API template <typename T, Dqn_usize N> void             Dqn_FArray_Clear     (Dqn_FArray<T, N> *array);
#endif // !defined(DQN_NO_FARRAY)

#if !defined(DQN_NO_LIST)
// NOTE: [$LIST] Dqn_List ==========================================================================
//
// NOTE: API
// @proc Dqn_List_At
//   @param at_chunk[out] (Optional) The chunk that the index belongs to will 
//   be set in this parameter if given
//   @return The element, or null pointer if it is not a valid index.

// @proc Dqn_List_Iterate
//   @desc Produce an iterator for the data in the list
//
//   @param[in] start_index The index to start iterating from
//
#if 0
    Dqn_List<int> list = {};
    for (Dqn_ListIterator<int> it = {}; Dqn_List_Iterate(&list, &it, 0);)
    {
        int *item = it.data;
    }
#endif

template <typename T> struct Dqn_ListChunk
{
    T                *data;
    Dqn_usize         size;
    Dqn_usize         count;
    Dqn_ListChunk<T> *next;
    Dqn_ListChunk<T> *prev;
};

template <typename T> struct Dqn_ListIterator
{
    Dqn_b32           init;             // True if Dqn_List_Iterate has been called at-least once on this iterator
    Dqn_ListChunk<T> *chunk;            // The chunk that the iterator is reading from
    Dqn_usize         chunk_data_index; // The index in the chunk the iterator is referencing
    T                *data;             // Pointer to the data the iterator is referencing. Nullptr if invalid.
};

template <typename T> struct Dqn_List
{
    Dqn_Arena        *arena;
    Dqn_usize         count;      // Cumulative count of all items made across all list chunks
    Dqn_usize         chunk_size; // When new ListChunk's are required, the minimum 'data' entries to allocate for that node.
    Dqn_ListChunk<T> *head;
    Dqn_ListChunk<T> *tail;
};

// NOTE: API =======================================================================================
template <typename T> Dqn_List<T> Dqn_List_Init   (Dqn_Arena *arena, Dqn_usize chunk_size);
template <typename T> T *         Dqn_List_At     (Dqn_List<T> *list, Dqn_usize index, Dqn_ListChunk<T> *at_chunk);
template <typename T> bool        Dqn_List_Iterate(Dqn_List<T> *list, Dqn_ListIterator<T> *it, Dqn_usize start_index);

template <typename T> T *         Dqn_List_Make   (Dqn_List<T> *list, Dqn_usize count);
template <typename T> T *         Dqn_List_Add    (Dqn_List<T> *list, T const &value);
#endif // !defined(DQN_NO_LIST)

#if !defined(DQN_NO_VARRAY)
// NOTE: [$VARR] Dqn_VArray ========================================================================
DQN_API template <typename T> Dqn_VArray<T> Dqn_VArray_InitByteSize(Dqn_Arena *arena, Dqn_usize byte_size)
{
    Dqn_VArray<T> result = {};
    result.block         = Dqn_Arena_Grow(arena, byte_size, 0 /*commit*/, Dqn_MemBlockFlag_ArenaPrivate | Dqn_MemBlockFlag_AllocsAreContiguous);
    result.data          = DQN_CAST(T *)Dqn_MemBlock_Alloc(result.block, /*size*/ 0, alignof(T), Dqn_ZeroMem_No);
    result.max           = (result.block->size - result.block->used) / sizeof(T);
    return result;
}

DQN_API template <typename T> Dqn_VArray<T> Dqn_VArray_Init(Dqn_Arena *arena, Dqn_usize max)
{
    Dqn_VArray<T> result = Dqn_VArray_InitByteSize<T>(arena, max * sizeof(T));
    DQN_ASSERT(result.max >= max);
    return result;
}

DQN_API template <typename T> bool Dqn_VArray_IsValid(Dqn_VArray<T> const *array)
{
    bool result = array && array->data && array->size <= array->max && array->block;
    return result;
}

DQN_API template <typename T> T *Dqn_VArray_Make(Dqn_VArray<T> *array, Dqn_usize count, Dqn_ZeroMem zero_mem)
{
    if (!Dqn_VArray_IsValid(array))
        return nullptr;

    if (!DQN_CHECKF((array->size + count) < array->max, "Array is out of virtual memory"))
        return nullptr;

    // TODO: Use placement new? Why doesn't this work?
    T *result = Dqn_MemBlock_NewArray(array->block, T, count, zero_mem);
    if (result)
        array->size += count;
    return result;
}

DQN_API template <typename T> T *Dqn_VArray_Add(Dqn_VArray<T> *array, T const *items, Dqn_usize count)
{
    T *result = Dqn_VArray_Make(array, count, Dqn_ZeroMem_No);
    if (result)
        DQN_MEMCPY(result, items, count * sizeof(T));
    return result;
}

DQN_API template <typename T> void Dqn_VArray_EraseRange(Dqn_VArray<T> *array, Dqn_usize begin_index, Dqn_isize count, Dqn_VArrayErase erase)
{
    if (!Dqn_VArray_IsValid(array) || array->size == 0 || count == 0)
        return;

    // NOTE: Caculate the end index of the erase range
    Dqn_isize abs_count = DQN_ABS(count);
    Dqn_usize end_index = 0;
    if (count < 0) {
        end_index = begin_index - (abs_count - 1);
        if (end_index > begin_index)
            end_index = 0;
    } else {
        end_index = begin_index + (abs_count - 1);
        if (end_index < begin_index)
            end_index = array->size - 1;
    }

    // NOTE: Ensure begin_index < one_past_end_index
    if (end_index < begin_index) {
        Dqn_usize tmp = begin_index;
        begin_index   = end_index;
        end_index     = tmp;
    }

    // NOTE: Ensure indexes are within valid bounds
    begin_index = DQN_MIN(begin_index, array->size);
    end_index   = DQN_MIN(end_index,   array->size - 1);

    // NOTE: Erase the items in the range [begin_index, one_past_end_index)
    Dqn_usize one_past_end_index = end_index + 1;
    Dqn_usize erase_count        = one_past_end_index - begin_index;
    if (erase_count) {
        T *end  = array->data + array->size;
        T *dest = array->data + begin_index;
        if (erase == Dqn_VArrayErase_Stable) {
            T *src = dest + erase_count;
            DQN_MEMMOVE(dest, src, (end - src) * sizeof(T));
        } else {
            T *src = end - erase_count;
            DQN_MEMCPY(dest, src, (end - src) * sizeof(T));
        }
        array->size -= erase_count;
        Dqn_MemBlock_Pop(array->block, erase_count * sizeof(T));
    }
}

DQN_API template <typename T> void Dqn_VArray_Clear(Dqn_VArray<T> *array)
{
    if (array)
        array->size = 0;
}

DQN_API template <typename T> void Dqn_VArray_Reserve(Dqn_VArray<T> *array, Dqn_usize count)
{
    if (!Dqn_VArray_IsValid(array) || count == 0)
        return;

    Dqn_Arena_CommitFromBlock(array->block, count * sizeof(T), Dqn_ArenaCommit_EnsureSpace);
}
#endif // !defined(DQN_NO_VARRAY)

#if !defined(DQN_NO_DSMAP)
// NOTE: [$DMAP] Dqn_DSMap =========================================================================
uint32_t const DQN_DS_MAP_DEFAULT_HASH_SEED = 0x8a1ced49;
uint32_t const DQN_DS_MAP_SENTINEL_SLOT = 0;

template <typename T>
Dqn_DSMap<T> Dqn_DSMap_Init(uint32_t size)
{
    Dqn_DSMap<T> result = {};
    if (DQN_CHECKF((size & (size - 1)) == 0, "Power-of-two size required")) {
        result.hash_to_slot = Dqn_Allocator_NewArray(result.allocator, uint32_t, size, Dqn_ZeroMem_Yes);
        if (result.hash_to_slot) {
            result.slots = Dqn_Allocator_NewArray(result.allocator, Dqn_DSMapSlot<T>, size, Dqn_ZeroMem_Yes);
            if (result.slots) {
                result.occupied     = 1; // For sentinel
                result.size         = size;
                result.initial_size = size;
            } else {
                Dqn_Allocator_Dealloc(result.allocator, result.hash_to_slot, size * sizeof(*result.hash_to_slot));
            }
        }
    }
    return result;
}

template <typename T>
void Dqn_DSMap_Deinit(Dqn_DSMap<T> *map)
{
    if (!map)
        return;
    if (map->slots)
        Dqn_Allocator_Dealloc(map->allocator, map->slots, sizeof(*map->slots) * map->size);
    if (map->hash_to_slot)
        Dqn_Allocator_Dealloc(map->allocator, map->hash_to_slot, sizeof(*map->hash_to_slot) * map->size);
    *map = {};
}

template <typename T>
bool Dqn_DSMap_IsValid(Dqn_DSMap<T> const *map)
{
    bool result = map &&
                  map->hash_to_slot &&                  // Hash to slot mapping array must be allocated
                  map->slots &&                         // Slots array must be allocated
                  (map->size & (map->size - 1)) == 0 && // Must be power of two size
                  map->occupied >= 1;                   // DQN_DS_MAP_SENTINEL_SLOT takes up one slot
    return result;
}

template <typename T>
uint32_t Dqn_DSMap_Hash(Dqn_DSMap<T> const *map, Dqn_DSMapKey key)
{
    uint32_t result = 0;
    if (!map)
        return result;

    if (key.type == Dqn_DSMapKeyType_U64NoHash) {
        result = DQN_CAST(uint32_t)key.payload.u64;
        return result;
    }

    uint32_t seed = map->hash_seed ? map->hash_seed : DQN_DS_MAP_DEFAULT_HASH_SEED;
    if (map->hash_function) {
        map->hash_function(key, seed);
    } else {
        // NOTE: Courtesy of Demetri Spanos (which this hash table was inspired
        // from), the following is a hashing function snippet provided for
        // reliable, quick and simple quality hashing functions for hash table
        // use.
        // Source: https://github.com/demetri/scribbles/blob/c475464756c104c91bab83ed4e14badefef12ab5/hashing/ub_aware_hash_functions.c

        char const *key_ptr = nullptr;
        uint32_t len        = 0;
        uint32_t h          = seed;
        switch (key.type) {
            case Dqn_DSMapKeyType_U64NoHash: DQN_INVALID_CODE_PATH; /*FALLTHRU*/
            case Dqn_DSMapKeyType_Invalid: break;

            case Dqn_DSMapKeyType_Buffer:
                key_ptr = DQN_CAST(char const *)key.payload.buffer.data;
                len     = key.payload.buffer.size;
                break;

            case Dqn_DSMapKeyType_U64:
                key_ptr = DQN_CAST(char const *)&key.payload.u64;
                len     = sizeof(key.payload.u64);
                break;
        }

        // Murmur3 32-bit without UB unaligned accesses
        // uint32_t mur3_32_no_UB(const void *key, int len, uint32_t h)

        // main body, work on 32-bit blocks at a time
        for (uint32_t i = 0; i < len / 4; i++) {
            uint32_t k;
            memcpy(&k, &key_ptr[i * 4], sizeof(k));

            k *= 0xcc9e2d51;
            k = ((k << 15) | (k >> 17)) * 0x1b873593;
            h = (((h ^ k) << 13) | ((h ^ k) >> 19)) * 5 + 0xe6546b64;
        }

        // load/mix up to 3 remaining tail bytes into a tail block
        uint32_t t    = 0;
        uint8_t *tail = ((uint8_t *)key_ptr) + 4 * (len / 4);
        switch (len & 3) {
            case 3: t ^= tail[2] << 16;
            case 2: t ^= tail[1] << 8;
            case 1: {
                t ^= tail[0] << 0;
                h ^= ((0xcc9e2d51 * t << 15) | (0xcc9e2d51 * t >> 17)) * 0x1b873593;
            }
        }

        // finalization mix, including key length
        h      = ((h ^ len) ^ ((h ^ len) >> 16)) * 0x85ebca6b;
        h      = (h ^ (h >> 13)) * 0xc2b2ae35;
        result = h ^ (h >> 16);
    }
    return result;
}

template <typename T>
uint32_t Dqn_DSMap_HashToSlotIndex(Dqn_DSMap<T> const *map, Dqn_DSMapKey key)
{
    uint32_t result = DQN_DS_MAP_SENTINEL_SLOT;
    if (!Dqn_DSMap_IsValid(map))
        return result;

    result = key.hash & (map->size - 1);
    for (;;) {
        if (map->hash_to_slot[result] == DQN_DS_MAP_SENTINEL_SLOT)
            return result;

        Dqn_DSMapSlot<T> *slot = map->slots + map->hash_to_slot[result];
        if (slot->key.type == Dqn_DSMapKeyType_Invalid || (slot->key.hash == key.hash && slot->key == key))
            return result;

        result = (result + 1) & (map->size - 1);
    }
}

template <typename T>
Dqn_DSMapSlot<T> *Dqn_DSMap_FindSlot(Dqn_DSMap<T> const *map, Dqn_DSMapKey key)
{
    Dqn_DSMapSlot<T> const *result = nullptr;
    if (Dqn_DSMap_IsValid(map)) {
        uint32_t index = Dqn_DSMap_HashToSlotIndex(map, key);
        if (map->hash_to_slot[index] != DQN_DS_MAP_SENTINEL_SLOT)
            result = map->slots + map->hash_to_slot[index];
    }
    return DQN_CAST(Dqn_DSMapSlot<T> *)result;
}

template <typename T>
Dqn_DSMapSlot<T> *Dqn_DSMap_MakeSlot(Dqn_DSMap<T> *map, Dqn_DSMapKey key, bool *found)
{
    Dqn_DSMapSlot<T> *result = nullptr;
    if (Dqn_DSMap_IsValid(map)) {
        uint32_t index = Dqn_DSMap_HashToSlotIndex(map, key);
        if (map->hash_to_slot[index] == DQN_DS_MAP_SENTINEL_SLOT) {
            // NOTE: Create the slot
            map->hash_to_slot[index] = map->occupied++;

            // NOTE: Check if resize is required
            bool map_is_75pct_full = (map->occupied * 4) > (map->size * 3);
            if (map_is_75pct_full) {
                if (!Dqn_DSMap_Resize(map, map->size * 2))
                    return result;
                result = Dqn_DSMap_MakeSlot(map, key, nullptr /*found*/);
            } else {
                result = map->slots + map->hash_to_slot[index];
            }

            // NOTE: Update the slot
            result->key = key;
            if (found)
                *found = false;
        } else {
            result = map->slots + map->hash_to_slot[index];
            if (found)
                *found = true;
        }
    }
    return result;
}

template <typename T>
Dqn_DSMapSlot<T> *Dqn_DSMap_SetSlot(Dqn_DSMap<T> *map, Dqn_DSMapKey key, T const &value, bool *found)
{
    Dqn_DSMapSlot<T> *result = nullptr;
    if (!Dqn_DSMap_IsValid(map))
        return result;

    result        = Dqn_DSMap_MakeSlot(map, key, found);
    result->value = value;
    return result;
}

template <typename T>
T *Dqn_DSMap_Find(Dqn_DSMap<T> const *map, Dqn_DSMapKey key)
{
    Dqn_DSMapSlot<T> const *slot = Dqn_DSMap_FindSlot(map, key);
    T const *result              = slot ? &slot->value : nullptr;
    return DQN_CAST(T *)result;
}

template <typename T>
T *Dqn_DSMap_Make(Dqn_DSMap<T> *map, Dqn_DSMapKey key, bool *found)
{
    Dqn_DSMapSlot<T> *slot = Dqn_DSMap_MakeSlot(map, key, found);
    T *result              = &slot->value;
    return result;
}

template <typename T>
T *Dqn_DSMap_Set(Dqn_DSMap<T> *map, Dqn_DSMapKey key, T const &value, bool *found)
{
    Dqn_DSMapSlot<T> *result = Dqn_DSMap_SetSlot(map, key, value, found);
    return &result->value;
}

template <typename T>
bool Dqn_DSMap_Resize(Dqn_DSMap<T> *map, uint32_t size)
{
    if (!Dqn_DSMap_IsValid(map) || size < map->occupied || size < map->initial_size)
        return false;

    Dqn_DSMap<T> new_map = Dqn_DSMap_Init<T>(size);
    if (!Dqn_DSMap_IsValid(&new_map))
        return false;

    new_map.initial_size = map->initial_size;
    for (uint32_t old_index = 1 /*Sentinel*/; old_index < map->occupied; old_index++) {
        Dqn_DSMapSlot<T> *old_slot = map->slots + old_index;
        if (old_slot->key.type != Dqn_DSMapKeyType_Invalid) {
            Dqn_DSMap_Set(&new_map, old_slot->key, old_slot->value, nullptr /*found*/);
        }
    }

    DQN_MEMCPY(new_map.slots, map->slots, sizeof(*map->slots) * map->occupied);
    Dqn_DSMap_Deinit(map);
    *map = new_map;
    return true;
}


template <typename T>
bool Dqn_DSMap_Erase(Dqn_DSMap<T> *map, Dqn_DSMapKey key)
{
    if (!Dqn_DSMap_IsValid(map))
        return false;

    uint32_t index      = Dqn_DSMap_HashToSlotIndex(map, key);
    uint32_t slot_index = map->hash_to_slot[index];
    if (slot_index == DQN_DS_MAP_SENTINEL_SLOT)
        return false;

    // NOTE: Mark the slot as unoccupied
    map->hash_to_slot[index] = DQN_DS_MAP_SENTINEL_SLOT;
    map->slots[slot_index]   = {}; // TODO: Optional?

    if (map->occupied > 1 /*Sentinel*/) {
        // NOTE: Repair the hash chain, e.g. rehash all the items after the removed
        // element and reposition them if necessary.
        for (uint32_t probe_index = index;;) {
            probe_index = (probe_index + 1) & (map->size - 1);
            if (map->hash_to_slot[probe_index] == DQN_DS_MAP_SENTINEL_SLOT)
                break;

            Dqn_DSMapSlot<T> *probe = map->slots + map->hash_to_slot[probe_index];
            uint32_t new_index      = probe->key.hash & (map->size - 1);
            if (index <= probe_index) {
                if (index < new_index && new_index <= probe_index)
                    continue;
            } else {
                if (index < new_index || new_index <= probe_index)
                    continue;
            }

            map->hash_to_slot[index]       = map->hash_to_slot[probe_index];
            map->hash_to_slot[probe_index] = DQN_DS_MAP_SENTINEL_SLOT;
            index                          = probe_index;
            DQN_ASSERT(Dqn_DSMap_FindSlot(map, probe->key) == probe);
        }

        // NOTE: We have erased a slot from the hash table, this leaves a gap
        // in our contiguous array. After repairing the chain, the hash mapping
        // is correct.
        // We will now fill in the vacant spot that we erased using the last 
        // element in the slot list.
        if (map->occupied >= 3 /*Ignoring sentinel, at least 2 other elements to unstable erase*/) {
            // NOTE: Copy in last slot to the erase slot
            Dqn_DSMapSlot<T> *last_slot = map->slots + map->occupied - 1;
            map->slots[slot_index]      = *last_slot;

            // NOTE: Update the hash-to-slot mapping for the value that was copied in
            uint32_t hash_to_slot_index           = Dqn_DSMap_HashToSlotIndex(map, last_slot->key);
            map->hash_to_slot[hash_to_slot_index] = slot_index;
            *last_slot = {}; // TODO: Optional?
        }
    }

    map->occupied--;
    bool map_is_below_25pct_full = (map->occupied * 4) < (map->size * 1);
    if (map_is_below_25pct_full && (map->size / 2) >= map->initial_size)
        Dqn_DSMap_Resize(map, map->size / 2);

    return true;
}

template <typename T>
DQN_API Dqn_DSMapKey Dqn_DSMap_KeyBuffer(Dqn_DSMap<T> const *map, void const *data, uint32_t size)
{
    Dqn_DSMapKey result        = {};
    result.type                = Dqn_DSMapKeyType_Buffer;
    result.payload.buffer.data = data;
    result.payload.buffer.size = size;
    result.hash                = Dqn_DSMap_Hash(map, result);
    return result;
}

template <typename T>
DQN_API Dqn_DSMapKey Dqn_DSMap_KeyU64(Dqn_DSMap<T> const *map, uint64_t u64)
{
    Dqn_DSMapKey result = {};
    result.type         = Dqn_DSMapKeyType_U64;
    result.payload.u64  = u64;
    result.hash         = Dqn_DSMap_Hash(map, result);
    return result;
}

template <typename T>
DQN_API Dqn_DSMapKey Dqn_DSMap_KeyString8(Dqn_DSMap<T> const *map, Dqn_String8 string)
{
    DQN_ASSERT(string.size > 0 && string.size <= UINT32_MAX);
    Dqn_DSMapKey result        = {};
    result.type                = Dqn_DSMapKeyType_Buffer;
    result.payload.buffer.data = string.data;
    result.payload.buffer.size = DQN_CAST(uint32_t)string.size;
    result.hash                = Dqn_DSMap_Hash(map, result);
    return result;
}

template <typename T>
DQN_API Dqn_DSMapKey Dqn_DSMap_KeyString8Copy(Dqn_DSMap<T> const *map, Dqn_Allocator allocator, Dqn_String8 string)
{
    Dqn_String8 copy    = Dqn_String8_Copy(allocator, string);
    Dqn_DSMapKey result = Dqn_DSMap_KeyString8(map, copy);
    return result;
}
#endif // !defined(DQN_NO_DSMAP)

#if !defined(DQN_NO_FARRAY)
// NOTE: [$FARR] Dqn_FArray ========================================================================
DQN_API template <typename T, Dqn_usize N> Dqn_FArray<T, N> Dqn_FArray_Init(T const *array, Dqn_usize count)
{
    Dqn_FArray<T, N> result = {};
    bool added              = Dqn_FArray_Add(&result, array, count);
    DQN_ASSERT(added);
    return result;
}
DQN_API template <typename T, Dqn_usize N> bool Dqn_FArray_IsValid(Dqn_FArray<T, N> const *array)
{
    bool result = array && array->size <= DQN_ARRAY_UCOUNT(array->data);
    return result;
}

DQN_API template <typename T, Dqn_usize N> T *Dqn_FArray_Make(Dqn_FArray<T, N> *array, Dqn_usize count, Dqn_ZeroMem zero_mem)
{
    if (!Dqn_FArray_IsValid(array))
        return nullptr;

    if (!DQN_CHECKF((array->size + count) < DQN_ARRAY_UCOUNT(array->data), "Array is out of memory"))
        return nullptr;

    // TODO: Use placement new? Why doesn't this work?
    T *result    = array->data + array->size;
    array->size += count;
    if (zero_mem == Dqn_ZeroMem_Yes)
        DQN_MEMSET(result, DQN_MEMSET_BYTE, sizeof(*result) * count);
    return result;
}

DQN_API template <typename T, Dqn_usize N> T *Dqn_FArray_Add(Dqn_FArray<T, N> *array, T const *items, Dqn_usize count)
{
    T *result = Dqn_FArray_Make(array, count, Dqn_ZeroMem_No);
    if (result)
        DQN_MEMCPY(result, items, count * sizeof(T));
    return result;
}

DQN_API template <typename T, Dqn_usize N> void Dqn_FArray_EraseRange(Dqn_FArray<T, N> *array, Dqn_usize begin_index, Dqn_isize count, Dqn_FArrayErase erase)
{
    if (!Dqn_FArray_IsValid(array) || array->size == 0 || count == 0)
        return;

    // NOTE: Caculate the end index of the erase range
    Dqn_isize abs_count = DQN_ABS(count);
    Dqn_usize end_index = 0;
    if (count < 0) {
        end_index = begin_index - (abs_count - 1);
        if (end_index > begin_index)
            end_index = 0;
    } else {
        end_index = begin_index + (abs_count - 1);
        if (end_index < begin_index)
            end_index = array->size - 1;
    }

    // NOTE: Ensure begin_index < one_past_end_index
    if (end_index < begin_index) {
        Dqn_usize tmp = begin_index;
        begin_index   = end_index;
        end_index     = tmp;
    }

    // NOTE: Ensure indexes are within valid bounds
    begin_index = DQN_MIN(begin_index, array->size);
    end_index   = DQN_MIN(end_index,   array->size - 1);

    // NOTE: Erase the items in the range [begin_index, one_past_end_index)
    Dqn_usize one_past_end_index = end_index + 1;
    Dqn_usize erase_count        = one_past_end_index - begin_index;
    if (erase_count) {
        T *end  = array->data + array->size;
        T *dest = array->data + begin_index;
        if (erase == Dqn_FArrayErase_Stable) {
            T *src = dest + erase_count;
            DQN_MEMMOVE(dest, src, (end - src) * sizeof(T));
        } else {
            T *src = end - erase_count;
            DQN_MEMCPY(dest, src, (end - src) * sizeof(T));
        }
        array->size -= erase_count;
    }
}

DQN_API template <typename T, Dqn_usize N> void Dqn_FArray_Clear(Dqn_FArray<T, N> *array)
{
    if (array)
        array->size = 0;
}
#endif // !defined(DQN_NO_FARRAY)

#if !defined(DQN_NO_LIST)
// NOTE: [$LIST] Dqn_List ==========================================================================
template <typename T> DQN_API Dqn_List<T> Dqn_List_Init(Dqn_Arena *arena, Dqn_usize chunk_size)
{
    Dqn_List<T> result = {};
    result.arena       = arena;
    result.chunk_size  = chunk_size;
    return result;
}

template <typename T> DQN_API T *Dqn_List_Make(Dqn_List<T> *list, Dqn_usize count)
{
    if (list->chunk_size == 0)
        list->chunk_size = 128;
    if (!list->tail || (list->tail->count + count) > list->tail->size) {
        auto *tail = Dqn_Arena_New(list->arena, Dqn_ListChunk<T>, Dqn_ZeroMem_Yes);
        if (!tail)
          return nullptr;

        Dqn_usize items = DQN_MAX(list->chunk_size, count);
        tail->data      = Dqn_Arena_NewArray(list->arena, T, items, Dqn_ZeroMem_Yes);
        tail->size      = items;

        if (!tail->data)
            return nullptr;

        if (list->tail) {
            list->tail->next = tail;
            tail->prev       = list->tail;
        }

        list->tail = tail;

        if (!list->head)
            list->head = list->tail;
    }

    T *result = list->tail->data + list->tail->count;
    list->tail->count += count;
    list->count       += count;
    return result;
}

template <typename T> DQN_API T *Dqn_List_Add(Dqn_List<T> *list, T const &value)
{
    T *result = Dqn_List_Make(list, 1);
    *result   = value;
    return result;
}

template <typename T> DQN_API bool Dqn_List_Iterate(Dqn_List<T> *list, Dqn_ListIterator<T> *it, Dqn_usize start_index)
{
    bool result = false;
    if (!list || !it || list->chunk_size <= 0)
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

template <typename T> DQN_API T *Dqn_List_At(Dqn_List<T> *list, Dqn_usize index, Dqn_ListChunk<T> **at_chunk)
{
    if (!list || !list->chunk_size || index >= list->count)
        return nullptr;

    Dqn_usize total_chunks       = list->count / (list->chunk_size + (list->chunk_size - 1));
    Dqn_usize desired_chunk      = index / list->chunk_size;
    Dqn_usize forward_scan_dist  = desired_chunk;
    Dqn_usize backward_scan_dist = total_chunks - desired_chunk;

    // NOTE: Linearly scan forwards/backwards to the chunk we need. We don't
    // have random access to chunks
    Dqn_usize current_chunk = 0;
    Dqn_ListChunk<T> **chunk = nullptr;
    if (forward_scan_dist <= backward_scan_dist) {
        for (chunk = &list->head; *chunk && current_chunk != desired_chunk; *chunk = (*chunk)->next, current_chunk++) {
        }
    } else {
        current_chunk = total_chunks;
        for (chunk = &list->tail; *chunk && current_chunk != desired_chunk; *chunk = (*chunk)->prev, current_chunk--) {
        }
    }

    T *result = nullptr;
    if (*chunk) {
        Dqn_usize relative_index = index % list->chunk_size;
        result                   = (*chunk)->data + relative_index;
        DQN_ASSERT(relative_index < (*chunk)->count);
    }

    if (result && at_chunk)
        *at_chunk = *chunk;

    return result;
}
#endif // !defined(DQN_NO_LIST)
