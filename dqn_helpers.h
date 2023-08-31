#if !defined(DQN_NO_JSON_BUILDER)
// NOTE: [$JSON] Dqn_JSONBuilder ===================================================================
// Basic helper class to construct JSON output to a string
// TODO(dqn): We need to write tests for this
//
// NOTE: API =======================================================================================
// @proc Dqn_JSONBuilder_Build
//   @desc Convert the internal JSON buffer in the builder into a string.
//   @param[in] arena The allocator to use to build the string

// @proc Dqn_JSONBuilder_KeyValue, Dqn_JSONBuilder_KeyValueF
//   @desc Add a JSON key value pair untyped. The value is emitted directly 
//   without checking the contents of value.
//
//   All other functions internally call into this function which is the main
//   workhorse of the builder.

// @proc Dqn_JSON_Builder_ObjectEnd
//   @desc End a JSON object in the builder, generates internally a '}' string

// @proc Dqn_JSON_Builder_ArrayEnd
//   @desc End a JSON array in the builder, generates internally a ']' string

// @proc Dqn_JSONBuilder_LiteralNamed
//   @desc Add a named JSON key-value object whose value is directly written to
//   the following '"<key>": <value>' (e.g. useful for emitting the 'null'
//   value)

// @proc Dqn_JSONBuilder_U64Named,  Dqn_JSONBuilder_U64,
//       Dqn_JSONBuilder_I64Named,  Dqn_JSONBuilder_I64,
//       Dqn_JSONBuilder_F64Named,  Dqn_JSONBuilder_F64,
//       Dqn_JSONBuilder_BoolNamed, Dqn_JSONBuilder_Bool,
//   @desc Add the named JSON data type as a key-value object. Generates 
//   internally the string '"<key>": <value>'

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
    Dqn_String8Builder  string_builder;    // (Internal)
    int                 indent_level;      // (Internal)
    int                 spaces_per_indent; // The number of spaces per indent level
    Dqn_JSONBuilderItem last_item;
};

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


DQN_API Dqn_JSONBuilder Dqn_JSONBuilder_Init            (Dqn_Allocator allocator, int spaces_per_indent);
DQN_API Dqn_String8     Dqn_JSONBuilder_Build           (Dqn_JSONBuilder const *builder, Dqn_Allocator allocator);
DQN_API void            Dqn_JSONBuilder_KeyValue        (Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value);
DQN_API void            Dqn_JSONBuilder_KeyValueF       (Dqn_JSONBuilder *builder, Dqn_String8 key, char const *value_fmt, ...);
DQN_API void            Dqn_JSONBuilder_ObjectBeginNamed(Dqn_JSONBuilder *builder, Dqn_String8 name);
DQN_API void            Dqn_JSONBuilder_ObjectEnd       (Dqn_JSONBuilder *builder);
DQN_API void            Dqn_JSONBuilder_ArrayBeginNamed (Dqn_JSONBuilder *builder, Dqn_String8 name);
DQN_API void            Dqn_JSONBuilder_ArrayEnd        (Dqn_JSONBuilder *builder);
DQN_API void            Dqn_JSONBuilder_StringNamed     (Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value);
DQN_API void            Dqn_JSONBuilder_LiteralNamed    (Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value);
DQN_API void            Dqn_JSONBuilder_U64Named        (Dqn_JSONBuilder *builder, Dqn_String8 key, uint64_t value);
DQN_API void            Dqn_JSONBuilder_I64Named        (Dqn_JSONBuilder *builder, Dqn_String8 key, int64_t value);
DQN_API void            Dqn_JSONBuilder_F64Named        (Dqn_JSONBuilder *builder, Dqn_String8 key, double value, int decimal_places);
DQN_API void            Dqn_JSONBuilder_BoolNamed       (Dqn_JSONBuilder *builder, Dqn_String8 key, bool value);

#define                 Dqn_JSONBuilder_ObjectBegin(builder) Dqn_JSONBuilder_ObjectBeginNamed(builder, DQN_STRING8(""))
#define                 Dqn_JSONBuilder_ArrayBegin(builder) Dqn_JSONBuilder_ArrayBeginNamed(builder, DQN_STRING8(""))
#define                 Dqn_JSONBuilder_String(builder, value) Dqn_JSONBuilder_StringNamed(builder, DQN_STRING8(""), value)
#define                 Dqn_JSONBuilder_Literal(builder, value) Dqn_JSONBuilder_LiteralNamed(builder, DQN_STRING8(""), value)
#define                 Dqn_JSONBuilder_U64(builder, value) Dqn_JSONBuilder_U64Named(builder, DQN_STRING8(""), value)
#define                 Dqn_JSONBuilder_I64(builder, value) Dqn_JSONBuilder_I64Named(builder, DQN_STRING8(""), value)
#define                 Dqn_JSONBuilder_F64(builder, value) Dqn_JSONBuilder_F64Named(builder, DQN_STRING8(""), value)
#define                 Dqn_JSONBuilder_Bool(builder, value) Dqn_JSONBuilder_BoolNamed(builder, DQN_STRING8(""), value)
#endif // !defined(DQN_NO_JSON_BUIDLER)

#if !defined(DQN_NO_BIN)
// NOTE: [$BHEX] Dqn_Bin ===========================================================================
// NOTE: API =======================================================================================
// @proc Dqn_Bin_U64ToHexU64String
//   @desc Convert a 64 bit number to a hex string
//   @param[in] number Number to convert to hexadecimal representation
//   @param[in] flags Bit flags from Dqn_BinHexU64StringFlags to customise the
//   output of the hexadecimal string.
//   @return The hexadecimal representation of the number. This string is always
//   null-terminated.

// @proc Dqn_Bin_U64ToHex
//   @copybrief Dqn_Bin_U64ToHexU64String

//   @param[in] allocator The memory allocator to use for the memory of the
//   hexadecimal string.
//   @copyparams Dqn_Bin_U64ToHexU64String

// @proc Dqn_Bin_HexBufferToU64
//   @desc Convert a hexadecimal string a 64 bit value.
//   Asserts if the hex string is too big to be converted into a 64 bit number.

// @proc Dqn_Bin_HexToU64
//   @copydoc Dqn_Bin_HexToU64

// @proc Dqn_Bin_BytesToHexBuffer
//   @desc Convert a binary buffer into its hex representation into dest.
//
//   The dest buffer must be large enough to contain the hex representation, i.e.
//   atleast (src_size * 2).
//
//   @return True if the conversion into the dest buffer was successful, false
//   otherwise (e.g. invalid arguments).

// @proc Dqn_Bin_BytesToHexBufferArena
//   @desc Convert a series of bytes into a string
//   @return A null-terminated hex string, null pointer if allocation failed

// @proc Dqn_Bin_BytesToHexArena
//   @copydoc Dqn_Bin_BytesToHexBufferArena
//   @return A hex string, the string is invalid if conversion failed.

// @proc Dqn_Bin_HexBufferToBytes
//   @desc Convert a hex string into binary at `dest`.
//
//   The dest buffer must be large enough to contain the binary representation,
//   i.e. atleast ceil(hex_size / 2). This function will strip whitespace,
//   leading 0x/0X prefix from the string before conversion.
//
//   @param[in] hex The hexadecimal string to convert
//   @param[in] hex_size Size of the hex buffer. This function can handle an odd
//   size hex string i.e. "fff" produces 0xfff0.
//   @param[out] dest Buffer to write the bytes to
//   @param[out] dest_size Maximum number of bytes to write to dest
//
//   @return The number of bytes written to `dest_size`, this value will *never*
//   be greater than `dest_size`.

// @proc Dqn_Bin_HexToBytes
//   @desc String8 variant of @see Dqn_Bin_HexBufferToBytes

// @proc Dqn_Bin_StringHexBufferToBytesUnchecked
//   @desc Unchecked variant of @see Dqn_Bin_HexBufferToBytes
//
//   This function skips some string checks, it assumes the hex is a valid hex
//   stream and that the arguments are valid e.g. no trimming or 0x prefix
//   stripping is performed

// @proc Dqn_Bin_String
//   @desc String8 variant of @see Dqn_Bin_HexBufferToBytesUnchecked

// @proc Dqn_Bin_HexBufferToBytesArena
//   Dynamic allocating variant of @see Dqn_Bin_HexBufferToBytesUnchecked
//
//   @param[in] arena The arena to allocate the bytes from
//   @param[in] hex Hex string to convert into bytes
//   @param[in] size Size of the hex string
//   @param[out] real_size The size of the buffer returned by the function
//
//   @return The byte representation of the hex string.

// @proc Dqn_Bin_HexToBytesArena
//   @copybrief Dqn_Bin_HexBufferToBytesArena
//
//   @param[in] arena The arena to allocate the bytes from
//   @param[in] hex Hex string to convert into bytes
//
//   @return The byte representation of the hex string.

struct Dqn_BinHexU64String
{
    char    data[2 /*0x*/ + 16 /*hex*/ + 1 /*null-terminator*/];
    uint8_t size;
};

enum Dqn_BinHexU64StringFlags
{
    Dqn_BinHexU64StringFlags_No0xPrefix   = 1 << 0, /// Remove the 0x prefix from the string
    Dqn_BinHexU64StringFlags_UppercaseHex = 1 << 1, /// Use uppercase ascii characters for hex
};

DQN_API char const *         Dqn_Bin_HexBufferTrim0x          (char const *hex, Dqn_usize size, Dqn_usize *real_size);
DQN_API Dqn_String8          Dqn_Bin_HexTrim0x                (Dqn_String8 string);

DQN_API Dqn_BinHexU64String  Dqn_Bin_U64ToHexU64String        (uint64_t number, uint32_t flags);
DQN_API Dqn_String8          Dqn_Bin_U64ToHex                 (Dqn_Allocator allocator, uint64_t number, uint32_t flags);

DQN_API uint64_t             Dqn_Bin_HexBufferToU64           (char const *hex, Dqn_usize size);
DQN_API uint64_t             Dqn_Bin_HexToU64                 (Dqn_String8 hex);

DQN_API Dqn_String8          Dqn_Bin_BytesToHexArena          (Dqn_Arena *arena, void const *src, Dqn_usize size);
DQN_API char *               Dqn_Bin_BytesToHexBufferArena    (Dqn_Arena *arena, void const *src, Dqn_usize size);
DQN_API bool                 Dqn_Bin_BytesToHexBuffer         (void const *src, Dqn_usize src_size, char *dest, Dqn_usize dest_size);

DQN_API Dqn_usize            Dqn_Bin_HexBufferToBytesUnchecked(char const *hex, Dqn_usize hex_size, void *dest, Dqn_usize dest_size);
DQN_API Dqn_usize            Dqn_Bin_HexBufferToBytes         (char const *hex, Dqn_usize hex_size, void *dest, Dqn_usize dest_size);
DQN_API char *               Dqn_Bin_HexBufferToBytesArena    (Dqn_Arena *arena, char const *hex, Dqn_usize hex_size, Dqn_usize *real_size);

DQN_API Dqn_usize            Dqn_Bin_HexToBytesUnchecked      (Dqn_String8 hex, void *dest, Dqn_usize dest_size);
DQN_API Dqn_usize            Dqn_Bin_HexToBytes               (Dqn_String8 hex, void *dest, Dqn_usize dest_size);
DQN_API Dqn_String8          Dqn_Bin_HexToBytesArena          (Dqn_Arena *arena, Dqn_String8 hex);
#endif // !defined(DQN_NO_BIN)

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

// NOTE: [$MISC] Misc ==============================================================================
// NOTE: API =======================================================================================
// @proc Dqn_SNPrintFDotTruncate
//   @desc Write the format string to the buffer truncating with a trailing ".."
//   if there is insufficient space in the buffer followed by null-terminating
//   the buffer (uses stb_sprintf underneath).
//   @return The size of the string written to the buffer *not* including the
//   null-terminator.
//
// @proc Dqn_U64ToString
//   @desc Convert a 64 bit unsigned value to its string representation.
//   @param[in] val Value to convert into a string
//   @param[in] separator The separator to insert every 3 digits. Set this to
//   0 if no separator is desired.

struct Dqn_U64String
{
    char    data[27+1]; // NOTE(dqn): 27 is the maximum size of uint64_t including a separtor
    uint8_t size;
};

DQN_API int           Dqn_SNPrintFDotTruncate(char *buffer, int size, char const *fmt, ...);
DQN_API Dqn_U64String Dqn_U64ToString        (uint64_t val, char separator);

#if !defined(DQN_NO_PROFILER)
// NOTE: [$PROF] Dqn_Profiler ======================================================================
// A profiler based off Casey Muratori's Computer Enhance course, Performance
// Aware Programming. This profiler measures function elapsed time using the
// CPU's time stamp counter (e.g. rdtsc) providing a rough cycle count
// that can be converted into a duration.
//
// This profiler uses a double buffer scheme for storing profiling markers.
// After an application's typical update/frame cycle you can swap the profiler's
// buffer whereby the front buffer contains the previous frames profiling
// metrics and the back buffer will be populated with the new frame's profiling
// metrics.
#if 0
    uint64_t tsc_per_seconds = Dqn_EstimateTSCPerSecond(/*duration_ms_to_gauge_tsc_frequency*/ 100);
    for (;;) { // Main update loop
        enum Zone { Zone_MainLoop, Zone_Count };
        Dqn_ProfilerZone profiler_zone_main_update = Dqn_Profiler_BeginZone(Zone_MainLoop);
        Dqn_ProfilerAnchor *anchors = Dqn_ProfilerAnchorBuffer(Dqn_ProfilerAnchorBuffer_Front);
        for (size_t index = 0; index < Zone_Count; index++) {
            Dqn_ProfilerAnchor *anchor = anchors + index;
            printf("%.*s[%u] %zu cycles (%.1fms)\n",
                   DQN_STRING_FMT(anchor->anchor_index),
                   anchor->hit_count,
                   anchor->tsc_inclusive,
                   anchor->tsc_inclusive * tsc_per_seconds * 1000.f);
        }
        Dqn_Profiler_EndZone(&profiler_zone_main_update);
        Dqn_Profiler_SwapAnchorBuffer(Zone_Count); // Should occur after all profiling zones are ended!
    }
#endif

// @proc Dqn_Profiler_AnchorBuffer
//   Retrieve the requested buffer from the profiler for writing/reading
//   profiling metrics.
//
//   @param buffer Enum that lets you specify which buffer to grab from the
//   profiler. The front buffer contains the previous frame's profiling metrics 
//   and the back buffer is where the profiler is current writing to.
//
//   For end user intents and purposes, you likely only need to read the front
//   buffer which contain the metrics that you can visualise regarding the most
//   profiling metrics recorded.

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
    uint64_t    tsc_inclusive;
    uint64_t    tsc_exclusive;
    uint16_t    hit_count;
    Dqn_String8 name;
};

struct Dqn_ProfilerZone
{
    uint16_t    anchor_index;
    uint64_t    begin_tsc;
    uint16_t    parent_zone;
    uint64_t    elapsed_tsc_at_zone_start;
};

#if defined(__cplusplus)
struct Dqn_ProfilerZoneScope
{
    Dqn_ProfilerZoneScope(Dqn_String8 name, uint16_t anchor_index);
    ~Dqn_ProfilerZoneScope();
    Dqn_ProfilerZone zone;
};
#define Dqn_Profiler_ZoneScope(name) auto DQN_UNIQUE_NAME(profile_zone_) = Dqn_ProfilerZoneScope(DQN_STRING8(name), __COUNTER__ + 1)
#define Dqn_Profiler_ZoneScopeWithIndex(name, anchor_index) auto DQN_UNIQUE_NAME(profile_zone_) = Dqn_ProfilerZoneScope(DQN_STRING8(name), anchor_index)
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

// NOTE: Macros ====================================================================================
#define             Dqn_Profiler_BeginZone(name) Dqn_Profiler_BeginZoneWithIndex(DQN_STRING8(name), __COUNTER__ + 1)

// NOTE: API =======================================================================================
Dqn_ProfilerZone    Dqn_Profiler_BeginZoneWithIndex(Dqn_String8 name, uint16_t anchor_index);
void                Dqn_Profiler_EndZone           (Dqn_ProfilerZone zone);
Dqn_ProfilerAnchor *Dqn_Profiler_AnchorBuffer      (Dqn_ProfilerAnchorBuffer buffer);
void                Dqn_Profiler_SwapAnchorBuffer  (uint32_t anchor_count);
void                Dqn_Profiler_Dump              (uint64_t tsc_per_second);
#endif // !defined(DQN_NO_PROFILER)

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

// @proc Dqn_Library_SetLogFile
//   @param[in] file Pass in nullptr to turn off writing logs to disk, otherwise
//   point it to the FILE that you wish to write to.

// @proc Dqn_Library_DumpThreadContextArenaStat
//   @desc Dump the per-thread arena statistics to the specified file

struct Dqn_Library
{
    bool                     lib_init;       // True if the library has been initialised via `Dqn_Library_Init`
    Dqn_TicketMutex          lib_mutex;
    Dqn_String8              exe_dir;        // The directory of the current executable

    Dqn_Arena                arena;
    Dqn_ArenaCatalog         arena_catalog;

    Dqn_LogProc             *log_callback;   // Set this pointer to override the logging routine
    void *                   log_user_data;
    bool                     log_to_file;    // Output logs to file as well as standard out
    Dqn_FsFile               log_file;       // TODO(dqn): Hmmm, how should we do this... ?
    Dqn_TicketMutex          log_file_mutex; // Is locked when instantiating the log_file for the first time
    bool                     log_no_colour;  // Disable colours in the logging output

    // NOTE: Leak Tracing ==========================================================================

    bool                     allocs_are_allowed_to_leak;
    bool                     alloc_tracking_disabled;
    #if defined(DQN_LEAK_TRACING)
    #if defined(DQN_NO_DSMAP)
        #error "DSMap is required for allocation tracing"
    #endif

    // TODO(doyle): @robust Setting some of these flags is not multi-thread safe
    uint16_t                   stack_trace_offset_to_our_call_stack;
    Dqn_TicketMutex            alloc_table_mutex;
    Dqn_DSMap<Dqn_AllocRecord> alloc_table;
    #endif

    // NOTE: OS ====================================================================================

    #if defined(DQN_OS_WIN32)
    LARGE_INTEGER            win32_qpc_frequency;
    Dqn_TicketMutex          win32_bcrypt_rng_mutex;
    void                    *win32_bcrypt_rng_handle;
    #endif

    uint32_t                 os_page_size;
    uint32_t                 os_alloc_granularity;

    // NOTE: Profiler ==============================================================================

    #if !defined(DQN_NO_PROFILER)
    Dqn_Profiler *profiler;
    Dqn_Profiler  profiler_default_instance;
    #endif
} extern *g_dqn_library;

// NOTE: API =======================================================================================
DQN_API Dqn_Library *Dqn_Library_Init                      ();
DQN_API void         Dqn_Library_SetPointer                (Dqn_Library *library);
#if !defined(DQN_NO_PROFILER)
DQN_API void         Dqn_Library_SetProfiler               (Dqn_Profiler *profiler);
#endif
DQN_API void         Dqn_Library_SetLogCallback            (Dqn_LogProc *proc, void *user_data);
DQN_API void         Dqn_Library_DumpThreadContextArenaStat(Dqn_String8 file_path);

