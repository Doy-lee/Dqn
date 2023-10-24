// NOTE: [$CSTR] Dqn_CStr8 ======================================================================
// @proc Dqn_CStr8_ArrayCount
//   @desc Calculate the size of a cstring literal/array at compile time
//   @param literal The cstring literal/array to calculate the size for
//   @return The size of the cstring not including the null-terminating byte

// @proc Dqn_CStr8_FSize, Dqn_CStr8_FVSize
//   Calculate the required size to format the given format cstring.
//   @param[in] fmt The format string to calculate the size for
//   @return The size required to format the string, not including the null
//   terminator.

// @proc Dqn_CStr8_Size
//   @desc Calculate the string length of the null-terminated string.
//   @param[in] a The string whose length is to be determined
//   @return The length of the string

DQN_API template <Dqn_usize N> constexpr Dqn_usize Dqn_CStr8_ArrayUCount(char const (&literal)[N]) { (void)literal; return N - 1; }
DQN_API template <Dqn_usize N> constexpr Dqn_usize Dqn_CStr8_ArrayICount(char const (&literal)[N]) { (void)literal; return N - 1; }
DQN_API                                  Dqn_usize Dqn_CStr8_FSize      (DQN_FMT_ATTRIB char const *fmt, ...);
DQN_API                                  Dqn_usize Dqn_CStr8_FVSize     (DQN_FMT_ATTRIB char const *fmt, va_list args);
DQN_API                                  Dqn_usize Dqn_CStr8_Size       (char const *a);
DQN_API                                  Dqn_usize Dqn_CStr16_Size      (wchar_t const *a);

// NOTE: [$STR8] Dqn_Str8 =======================================================================
// NOTE: API
// @proc Dqn_Str8_Init
//   @desc Initialise a string from a pointer and length
//   The string is invalid (i.e. Dqn_Str8_IsValid() returns false) if size is
//   negative or the string is null.

// @proc Dqn_Str8_InitCString
//   @desc Initialise a string from a cstring
//   The cstring must be null-terminated as its length is evaluated using
//   strlen. The string is invalid (i.e. Dqn_Str8_IsValid() returns false) if
//   size is negative or the string is null.

// @proc Dqn_Str8_InitF
//   @desc Create a string from a printf style format string
//   @param[in] allocator The allocator the string will be allocated from
//   @param[in] fmt The printf style format cstring

// @proc Dqn_Str8_InitFV
//   @desc Create a string from a printf style format string using a va_list
//   @param[in] arena The allocator the string will be allocated from
//   @param[in] fmt The printf style format cstring
//   @param[in] va The variable argument list
//
// @proc Dqn_Str8_IsValid
//   @desc Determine if the values of the given string are valid
//   A string is invalid if size is negative or the string is null.
//   @return True if the string is valid, false otherwise.

// @proc Dqn_Str8 Dqn_Str8_Slice
//   @desc Create a slice from a pre-existing string.
//   The requested slice is clamped to within the bounds of the original string.
//   @param[in] string The string to slice
//   @param[in] offset The starting byte to slice from
//   @param[in] size The size of the slice
//   @return The sliced string

// @proc Dqn_Str8_BinarySplit
//   @desc Split a string into the substring occuring prior and after the first
//   occurence of the `delimiter`. Neither strings include the `delimiter`.
//
//   @param[in] string The string to split
//   @param[in] string_size The size of the string
//   @param[in] delimiter The character to split the string on
//   @param[out] lhs_size The size of the left hand side of the split string
//   @param[out] rhs The right hand side of the split string
//   @param[out] rhs_size The size of the right hand side of the split string
//
//   @return The left hand side of the split string. The original pointer is
//   returned if the arguments were invalid.

// @proc Dqn_Str8_Split
//   @desc Split a string by the delimiting character.
//   This function can evaluate the number of splits required in the return value
//   by setting `splits` to null and `splits_count` to 0.
//   @param[in] string The source string to split
//   @param[in] delimiter The substring to split the string on
//   @param[out] splits (Optional) The destination array to write the splits to.
//   @param[in] splits_count The number of splits that can be written into the
//   `splits` array.
//   @return The number of splits in the `string`. If the return value is >=
//   'splits_count' then there are more splits in the string than can be written
//   to the `splits` array. The number of splits written is capped to the
//   capacity given by the caller, i.e. `splits_count`. This function should be
//   called again with a sufficiently sized array if all splits are desired.

// @proc Dqn_Str8_Segment
//   @desc Segment a string by inserting the `segment_char` every `segment_size`
//   characters in the string. For example, '123456789' split with
//   `segment_char` ' ' and `segment_size` '3' would yield, '123 456 789'.

// @proc Dqn_Str8_Allocate
//   @desc Create an empty string with the requested size
//   @param[in] allocator The allocator the string will be allocated from
//   @param[in] size The size in bytes of the string to allocate
//   @param[in] zero_mem Enum to indicate if the string's memory should be cleared

// @proc Dqn_Str8_CopyCString
//   @desc Create a copy of the given cstring
//   @param[in] allocator The allocator the string will be allocated from
//   @param[in] string The cstring to copy
//   @param[in] size The size of the cstring to copy. This cannot be <= 0
//   @return A copy of the string, invalid string if any argument was invalid.

// @proc Dqn_Str8_Copy
//   @desc Create a copy of the given string
//   @param[in] allocator The allocator the string will be allocated from
//   @param[in] string The string to copy
//   @return A copy of the string, invalid string if any argument was invalid.

// @proc Dqn_Str8_Eq, Dqn_Str8_EqInsensitive
//   @desc Compare a string for equality with or without case sensitivity.
//   @param[in] lhs The first string to compare equality with
//   @param[in] rhs The second string to compare equality with
//   @param[in] lhs The first string's size
//   @param[in] rhs The second string's size
//   @param[in] eq_case Set the comparison to be case sensitive or insensitive
//   @return True if the arguments are valid, non-null and the strings
//   are equal, false otherwise.

// @proc Dqn_Str8_StartsWith, Dqn_Str8_StartsWithInsensitive,
// Dqn_Str8_EndsWith, Dqn_Str8_EndswithInsensitive
//   @desc Check if a string starts/ends with the specified prefix
//   `EndsWithInsensitive` is case insensitive
//   @param[in] string The string to check for the prefix
//   @param[in] prefix The prefix to check against the string
//   @param[in] eq_case Set the comparison to be case sensitive or insensitive
//   @return True if the string is valid, non-null and has the specified prefix,
//   false otherwise.

// @proc Dqn_Str8_TrimPrefix, Dqn_Str8_TrimSuffix
//   @desc Remove the prefix/suffix respectively from the given `string.
//
//   @param[in] string The string to trim
//   @param[in] prefix The prefix to trim from the string
//   @param[in] suffix The suffix to trim from the string
//   @param[in] eq_case Set the comparison to be case sensitive or insensitive
//   @param[out] trimmed_string The size of the trimmed string
//
//   @return The trimmed string. The original input string is returned if
//   arguments are invalid or no trim was possible.

// @proc Dqn_Str8_TrimWhitespaceAround
//   @desc Trim whitespace from the prefix and suffix of the string
//
//   @param[in] string The string to trim
//   @param[in] string_size The size of the string
//   @param[out] trimmed_string The size of the trimmed string
//
//   @return The trimmed string. The original input string is returned if
//   arguments are invalid or no trim was possible.

// @proc Dqn_Str8_TrimByteOrderMark
//   @desc Trim UTF8, UTF16 BE/LE, UTF32 BE/LE byte order mark prefix in the string.
//
//   @param[in] string The string to trim
//   @param[in] string_size The size of the string
//   @param[out] trimmed_string The size of the trimmed string
//
//   @return The trimmed string. The original input string is returned if
//   arguments are invalid or no trim was possible.

// @proc Dqn_Str8_FileNameFromPath
//   @desc Get the file name from a file path. The file name is evaluated by
//   searching from the end of the string backwards to the first occurring path
//   separator '/' or '\'. If no path separator is found, the original string is
//   returned. This function preserves the file extension if there were any.
//
//   @param[in] path A file path on the disk
//   @param[in] size The size of the file path string, if size is '-1' the null
//   terminated string length is evaluated.
//   @param[out] file_name_size The size of the returned file name
//
//   @return The file name in the file path, if none is found, the original path
//   string is returned. Null pointer if arguments are null or invalid.

// @proc Dqn_Str8_ToI64, Dqn_Str8_ToU64
//   @desc Convert a number represented as a string to a signed 64 bit number.
//
//   The `separator` is an optional digit separator for example, if `separator`
//   is set to ',' then "1,234" will successfully be parsed to '1234'.
//
//   Real numbers are truncated. Both '-' and '+' prefixed strings are permitted,
//   i.e. "+1234" -> 1234 and "-1234" -> -1234. Strings must consist entirely of
//   digits, the seperator or the permitted prefixes as previously mentioned
//   otherwise this function will return false, i.e. "1234 dog" will cause the
//   function to return false, however, the output is greedily converted and
//   will be evaluated to "1234".
//
//   `ToU64` only   '+'        prefix is permitted
//   `ToI64` either '+' or '-' prefix is permitted
//
//   @param[in] separator The character used to separate the digits, if any. Set
//   this to 0, if no separators are permitted.

// @proc Dqn_Str8_Replace, Dqn_Str8_ReplaceInsensitive
//   @desc TODO(doyle): Write description

// @proc Dqn_Str8_Remove
//   @desc Remove the substring denoted by the begin index and the size from the string
//   string in-place using MEMMOVE to shift the string back.

// @proc Dqn_Str8_Find
//   @desc @param start_index Set an index within the string string to start the search
//   from, if not desired, set to 0
//   @return A string that points to the matching find, otherwise a 0 length string.

// @proc DQN_STR8
//   @desc Construct a UTF8 c-string literal into a Dqn_Str8 referencing a 
//   string stored in the data-segment. This string is read-only.

// @proc DQN_STR16
//   @desc Construct a UTF16 c-string literal into a Dqn_Str16 referencing a string
//   stored in the data-segment. This string is read-only.

// @proc DQN_STR_FMT
//   @desc Unpack a string into arguments for printing a string into a printf style
//   format string.

struct Dqn_Str8Link
{
    Dqn_Str8      string; // The string
    Dqn_Str8Link *next;   // The next string in the linked list
};

struct Dqn_Str16 /// A pointer and length style string that holds slices to UTF16 bytes.
{
    wchar_t   *data; // The UTF16 bytes of the string
    Dqn_usize  size; // The number of characters in the string

    #if defined(__cplusplus)
    wchar_t const *begin() const { return data; }        ///< Const begin iterator for range-for loops
    wchar_t const *end  () const { return data + size; } ///< Const end iterator for range-for loops
    wchar_t       *begin()       { return data; }        ///< Begin iterator for range-for loops
    wchar_t       *end  ()       { return data + size; } ///< End iterator for range-for loops
    #endif
};

struct Dqn_Str8BinarySplitResult
{
    Dqn_Str8 lhs;
    Dqn_Str8 rhs;
};

struct Dqn_Str8FindResult
{
    bool      found;                  // True if string was found. If false, the subsequent fields below are not set.
    Dqn_usize index;                  // The index in the buffer where the found string starts
    Dqn_Str8  match;                  // The matching string in the buffer that was searched
    Dqn_Str8  match_to_end_of_buffer; // The substring containing the found string to the end of the buffer
    Dqn_Str8  start_to_before_match;  // The substring from the start of the buffer up until the found string, not including it
};

// NOTE: Macros ====================================================================================
#define DQN_STR8(string)    Dqn_Str8{(char *)(string), sizeof(string) - 1}
#define DQN_STR16(string)   Dqn_Str16{(wchar_t *)(string), (sizeof(string)/sizeof(string[0])) - 1}
#define DQN_STR_FMT(string) (int)((string).size), (string).data

#if defined(__cplusplus)
#define Dqn_Str8_Init(data, size) (Dqn_Str8{(char *)(data), (Dqn_usize)(size)})
#else
#define Dqn_Str8_Init(data, size) (Dqn_Str8){(data), (size)}
#endif

// NOTE: API =======================================================================================
enum Dqn_Str8IsAll
{
    Dqn_Str8IsAll_Digits,
    Dqn_Str8IsAll_Hex,
};

enum Dqn_Str8EqCase
{
    Dqn_Str8EqCase_Sensitive,
    Dqn_Str8EqCase_Insensitive,
};

enum Dqn_Str8FindFlag
{
    Dqn_Str8FindFlag_Digit      = 1 << 0, // 0-9
    Dqn_Str8FindFlag_Whitespace = 1 << 1, // '\r', '\t', '\n', ' '
    Dqn_Str8FindFlag_Alphabet   = 1 << 2, // A-Z, a-z
    Dqn_Str8FindFlag_Plus       = 1 << 3, // +
    Dqn_Str8FindFlag_Minus      = 1 << 4, // -
    Dqn_Str8FindFlag_AlphaNum   = Dqn_Str8FindFlag_Alphabet | Dqn_Str8FindFlag_Digit,
};

struct Dqn_Str8SplitAllocResult
{
    Dqn_Str8 *data;
    Dqn_usize    size;
};

struct Dqn_Str8ToU64Result
{
    bool     success;
    uint64_t value;
};

struct Dqn_Str8ToI64Result
{
    bool    success;
    int64_t value;
};

DQN_API Dqn_Str8                  Dqn_Str8_InitCStr8              (char const *src);
#define                           Dqn_Str8_IsValid(string)        ((string).data)
DQN_API bool                      Dqn_Str8_IsAll                  (Dqn_Str8 string, Dqn_Str8IsAll is_all);

DQN_API Dqn_Str8                  Dqn_Str8_InitF                  (Dqn_Allocator allocator, DQN_FMT_ATTRIB char const *fmt, ...);
DQN_API Dqn_Str8                  Dqn_Str8_InitFV                 (Dqn_Allocator allocator, DQN_FMT_ATTRIB char const *fmt, va_list args);
DQN_API Dqn_Str8                  Dqn_Str8_Allocate               (Dqn_Allocator allocator, Dqn_usize size, Dqn_ZeroMem zero_mem);
DQN_API Dqn_Str8                  Dqn_Str8_CopyCString            (Dqn_Allocator allocator, char const *string, Dqn_usize size);
DQN_API Dqn_Str8                  Dqn_Str8_Copy                   (Dqn_Allocator allocator, Dqn_Str8 string);

DQN_API Dqn_Str8                  Dqn_Str8_Slice                  (Dqn_Str8 string, Dqn_usize offset, Dqn_usize size);
DQN_API Dqn_Str8                  Dqn_Str8_Advance                (Dqn_Str8 string, Dqn_usize amount);
DQN_API Dqn_Str8BinarySplitResult Dqn_Str8_BinarySplitArray       (Dqn_Str8 string, Dqn_Str8 const *find, Dqn_usize find_size);
DQN_API Dqn_Str8BinarySplitResult Dqn_Str8_BinarySplit            (Dqn_Str8 string, Dqn_Str8 find);
DQN_API Dqn_Str8BinarySplitResult Dqn_Str8_BinarySplitReverseArray(Dqn_Str8 string, Dqn_Str8 const *find, Dqn_usize find_size);
DQN_API Dqn_Str8BinarySplitResult Dqn_Str8_BinarySplitReverse     (Dqn_Str8 string, Dqn_Str8 find);
DQN_API Dqn_usize                 Dqn_Str8_Split                  (Dqn_Str8 string, Dqn_Str8 delimiter, Dqn_Str8 *splits, Dqn_usize splits_count);
DQN_API Dqn_Str8SplitAllocResult  Dqn_Str8_SplitAlloc             (Dqn_Allocator allocator, Dqn_Str8 string, Dqn_Str8 delimiter);

DQN_API Dqn_Str8FindResult        Dqn_Str8_FindFirstStringArray   (Dqn_Str8 string, Dqn_Str8 const *find, Dqn_usize find_size);
DQN_API Dqn_Str8FindResult        Dqn_Str8_FindFirstString        (Dqn_Str8 string, Dqn_Str8 find);
DQN_API Dqn_Str8FindResult        Dqn_Str8_FindFirst              (Dqn_Str8 string, uint32_t flags);
DQN_API Dqn_Str8                  Dqn_Str8_Segment                (Dqn_Allocator allocator, Dqn_Str8 src, Dqn_usize segment_size, char segment_char);

DQN_API bool                      Dqn_Str8_Eq                     (Dqn_Str8 lhs, Dqn_Str8 rhs, Dqn_Str8EqCase eq_case = Dqn_Str8EqCase_Sensitive);
DQN_API bool                      Dqn_Str8_EqInsensitive          (Dqn_Str8 lhs, Dqn_Str8 rhs);
DQN_API bool                      Dqn_Str8_StartsWith             (Dqn_Str8 string, Dqn_Str8 prefix, Dqn_Str8EqCase eq_case = Dqn_Str8EqCase_Sensitive);
DQN_API bool                      Dqn_Str8_StartsWithInsensitive  (Dqn_Str8 string, Dqn_Str8 prefix);
DQN_API bool                      Dqn_Str8_EndsWith               (Dqn_Str8 string, Dqn_Str8 prefix, Dqn_Str8EqCase eq_case = Dqn_Str8EqCase_Sensitive);
DQN_API bool                      Dqn_Str8_EndsWithInsensitive    (Dqn_Str8 string, Dqn_Str8 prefix);
DQN_API bool                      Dqn_Str8_HasChar                (Dqn_Str8 string, char ch);

DQN_API Dqn_Str8                  Dqn_Str8_TrimPrefix             (Dqn_Str8 string, Dqn_Str8 prefix, Dqn_Str8EqCase eq_case = Dqn_Str8EqCase_Sensitive);
DQN_API Dqn_Str8                  Dqn_Str8_TrimSuffix             (Dqn_Str8 string, Dqn_Str8 suffix, Dqn_Str8EqCase eq_case = Dqn_Str8EqCase_Sensitive);
DQN_API Dqn_Str8                  Dqn_Str8_TrimAround             (Dqn_Str8 string, Dqn_Str8 trim_string);
DQN_API Dqn_Str8                  Dqn_Str8_TrimWhitespaceAround   (Dqn_Str8 string);
DQN_API Dqn_Str8                  Dqn_Str8_TrimByteOrderMark      (Dqn_Str8 string);

DQN_API Dqn_Str8                  Dqn_Str8_FileNameFromPath       (Dqn_Str8 path);
DQN_API Dqn_Str8                  Dqn_Str8_FileNameNoExtension    (Dqn_Str8 path);
DQN_API Dqn_Str8                  Dqn_Str8_FilePathNoExtension    (Dqn_Str8 path);

DQN_API Dqn_Str8ToU64Result       Dqn_Str8_ToU64                  (Dqn_Str8 string, char separator);
DQN_API Dqn_Str8ToI64Result       Dqn_Str8_ToI64                  (Dqn_Str8 string, char separator);

DQN_API Dqn_Str8                  Dqn_Str8_Replace                (Dqn_Str8 string, Dqn_Str8 find, Dqn_Str8 replace, Dqn_usize start_index, Dqn_Allocator allocator, Dqn_Str8EqCase eq_case = Dqn_Str8EqCase_Sensitive);
DQN_API Dqn_Str8                  Dqn_Str8_ReplaceInsensitive     (Dqn_Str8 string, Dqn_Str8 find, Dqn_Str8 replace, Dqn_usize start_index, Dqn_Allocator allocator);
DQN_API void                      Dqn_Str8_Remove                 (Dqn_Str8 *string, Dqn_usize offset, Dqn_usize size);

#if defined(__cplusplus)
DQN_API bool                      operator==                      (Dqn_Str8 const &lhs, Dqn_Str8 const &rhs);
DQN_API bool                      operator!=                      (Dqn_Str8 const &lhs, Dqn_Str8 const &rhs);
#endif

#if !defined(DQN_NO_FSTR8)
// NOTE: [$FSTR] Dqn_FStr8 =========================================================================
// NOTE: API =======================================================================================
// @proc Dqn_FStr8_InitF
//   @desc Create a fixed string from the format string. The result string is
//   null-terminated.
//   @param fmt[in] Format string specifier to create the fixed string from
//   @return The created string, truncated if there was insufficient space

// @proc Dqn_FStr8_Max
//   @desc @param string[in] The string to query the maximum capacity of
//   @return Maximum capacity of the fixed string

// @proc Dqn_FStr8_Clear
//   @desc Reset the characters in the string
//   @param string[in] The string to clear

// @proc Dqn_FStr8_AppendFV
//   @desc Append a format string to the fixed string. On failure the string is
//   appended to but truncated ensuring null-termination.
//   @param string[in] The string to append to
//   @param fmt[in] Format string to append to the fixed string
//   @return True if append was successful, false otherwise.

// @proc Dqn_FStr8_AppendF
//   @desc @copydocs Dqn_FStr8_AppendF

// @proc Dqn_FStr8_AppendCStr8
//   @desc Append a cstring to the fixed string. On failure the string is
//   appended to but truncated ensuring null-termination.
//   @param string[in] The string to append to
//   @param value[in] Cstring to append to the fixed string
//   @param size[in] Size of the cstring
//   @return True if append was successful, false otherwise.

// @proc Dqn_FStr8_Append
//   @desc Append a string to the fixed string. On failure the string is
//   appended to but truncated ensuring null-termination.
//   @param string[in] The string to append to
//   @param value[in] String to append to the fixed string
//   determined before appending.
//   @return True if append was successful, false otherwise.

// @proc Dqn_FStr8_ToStr8
//   @desc Convert a fixed string to a string. The string holds a reference to the
//   fixed string and is invalidated once fixed string is deleted.
//   @param string[in] The fixed string to create a string from
//   @return String referencing the contents of `string`

// @proc Dqn_FStr8_Eq
//   @desc @see Dqn_Str8_Eq

// @proc Dqn_FStr8_EqStr8
//   @desc @see Dqn_Str8_Eq

// @proc Dqn_FStr8_EqInsensitive
//   @desc Compare a string for equality, case insensitive
//   @see Dqn_Str8_Eq

// @proc Dqn_FStr8_EqStr8Insensitive
//   @desc Compare a string for equality, case insensitive
//   @see Dqn_Str8_Eq

template <Dqn_usize N> struct Dqn_FStr8
{
    char      data[N+1];
    Dqn_usize size;

    bool operator==(Dqn_FStr8 const &other) const {
        if (size != other.size) return false;
        bool result = DQN_MEMCMP(data, other.data, size);
        return result;
    }

    bool operator!=(Dqn_FStr8 const &other) const { return !(*this == other); }

    char       *begin()       { return data; }
    char       *end  ()       { return data + size; }
    char const *begin() const { return data; }
    char const *end  () const { return data + size; }
};

template <Dqn_usize N>              Dqn_FStr8<N> Dqn_FStr8_InitF             (DQN_FMT_ATTRIB char const *fmt, ...);
template <Dqn_usize N>              Dqn_usize    Dqn_FStr8_Max               (Dqn_FStr8<N> const *string);
template <Dqn_usize N>              void         Dqn_FStr8_Clear             (Dqn_FStr8<N> *string);
template <Dqn_usize N>              bool         Dqn_FStr8_AppendFV          (Dqn_FStr8<N> *string, DQN_FMT_ATTRIB char const *fmt, va_list va);
template <Dqn_usize N>              bool         Dqn_FStr8_AppendF           (Dqn_FStr8<N> *string, DQN_FMT_ATTRIB char const *fmt, ...);
template <Dqn_usize N>              bool         Dqn_FStr8_AppendCStr8       (Dqn_FStr8<N> *string, char const *value, Dqn_usize size);
template <Dqn_usize N>              bool         Dqn_FStr8_Append            (Dqn_FStr8<N> *string, Dqn_Str8 value);
template <Dqn_usize N>              Dqn_Str8     Dqn_FStr8_ToStr8            (Dqn_FStr8<N> const *string);
template <Dqn_usize N>              bool         Dqn_FStr8_Eq                (Dqn_FStr8<N> const *lhs, Dqn_FStr8<N> const *rhs, Dqn_Str8EqCase eq_case);
template <Dqn_usize N>              bool         Dqn_FStr8_EqStr8            (Dqn_FStr8<N> const *lhs, Dqn_Str8 rhs, Dqn_Str8EqCase eq_case);
template <Dqn_usize N>              bool         Dqn_FStr8_EqInsensitive     (Dqn_FStr8<N> const *lhs, Dqn_FStr8<N> const *rhs);
template <Dqn_usize N>              bool         Dqn_FStr8_EqStr8Insensitive (Dqn_FStr8<N> const *lhs, Dqn_Str8 rhs);
template <Dqn_usize A, Dqn_usize B> bool         Dqn_FStr8_EqFStr8           (Dqn_FStr8<A> const *lhs, Dqn_FStr8<B> const *rhs, Dqn_Str8EqCase eq_case);
template <Dqn_usize A, Dqn_usize B> bool         Dqn_FStr8_EqFStr8Insensitive(Dqn_FStr8<A> const *lhs, Dqn_FStr8<B> const *rhs);
#endif // !defined(DQN_NO_FSTR8)

// NOTE: [$STRB] Dqn_Str8Builder ================================================================
// NOTE: API =======================================================================================
// @proc Dqn_Str8Builder_AppendRef, Dqn_Str8_AppendCopy,
// Dqn_Str8_AppendFV, Dqn_Str8_AppendF
//   @desc Append a string to the list of strings in the builder.
//
//   The string is appended to the builder as follows
//   - AppendRef: By reference
//   - AppendCopy: By copy using the builder's allocator to copy the string
//   - AppendFV, AppendF: Using a format string, allocated using the builder's
//     allocator
//
//   The string's data must persist whilst the string builder is being used.
//   @param builder The builder to append the string to
//   @param string The string to append to the builder
//   @return True if append was successful, false if parameters are invalid
//   or memory allocation failure.

// @proc Dqn_Str8Builder_Build
//   @desc Build the list of strings into the final composite string from the
//   string builder
//   @param builder The string builder to build the string from
//   @param allocator The allocator to use to build the string
//   @return The string if build was successful, empty string if parameters are
//   invalid or memory allocation failure.

struct Dqn_Str8Builder
{
    Dqn_Allocator allocator;   ///< Allocator to use to back the string list
    Dqn_Str8Link *head;        ///< First string in the linked list of strings
    Dqn_Str8Link *tail;        ///< Last string in the linked list of strings
    Dqn_usize     string_size; ///< The size in bytes necessary to construct the current string
    Dqn_usize     count;       ///< The number of links in the linked list of strings
};

DQN_API bool     Dqn_Str8Builder_AppendF   (Dqn_Str8Builder *builder, DQN_FMT_ATTRIB char const *fmt, ...);
DQN_API bool     Dqn_Str8Builder_AppendFV  (Dqn_Str8Builder *builder, DQN_FMT_ATTRIB char const *fmt, va_list args);
DQN_API bool     Dqn_Str8Builder_AppendRef (Dqn_Str8Builder *builder, Dqn_Str8 string);
DQN_API bool     Dqn_Str8Builder_AppendCopy(Dqn_Str8Builder *builder, Dqn_Str8 string);
DQN_API Dqn_Str8 Dqn_Str8Builder_Build     (Dqn_Str8Builder const *builder, Dqn_Allocator allocator);

// NOTE: [$CHAR] Dqn_Char ==========================================================================
DQN_API bool    Dqn_Char_IsAlphabet    (char ch);
DQN_API bool    Dqn_Char_IsDigit       (char ch);
DQN_API bool    Dqn_Char_IsAlphaNum    (char ch);
DQN_API bool    Dqn_Char_IsWhitespace  (char ch);
DQN_API bool    Dqn_Char_IsHex         (char ch);
DQN_API uint8_t Dqn_Char_HexToU8       (char ch);
DQN_API char    Dqn_Char_ToHex         (char ch);
DQN_API char    Dqn_Char_ToHexUnchecked(char ch);
DQN_API char    Dqn_Char_ToLower       (char ch);

// NOTE: [$UTFX] Dqn_UTF ===========================================================================
DQN_API int Dqn_UTF8_EncodeCodepoint(uint8_t utf8[4], uint32_t codepoint);
DQN_API int Dqn_UTF16_EncodeCodepoint(uint16_t utf16[2], uint32_t codepoint);

#if !defined(DQN_NO_FSTR8)
// NOTE: [$FSTR] Dqn_FStr8 ======================================================================
template <Dqn_usize N> Dqn_FStr8<N> Dqn_FStr8_InitF(DQN_FMT_ATTRIB char const *fmt, ...)
{
    Dqn_FStr8<N> result = {};
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        Dqn_FStr8_AppendFV(&result, fmt, args);
        va_end(args);
    }
    return result;
}

template <Dqn_usize N> Dqn_usize Dqn_FStr8_Max(Dqn_FStr8<N> const *)
{
    Dqn_usize result = N;
    return result;
}

template <Dqn_usize N> void Dqn_FStr8_Clear(Dqn_FStr8<N> *string)
{
    *string = {};
}

template <Dqn_usize N> bool Dqn_FStr8_AppendFV(Dqn_FStr8<N> *string, DQN_FMT_ATTRIB char const *fmt, va_list args)
{
    bool result = false;
    if (!string || !fmt)
        return result;

    Dqn_usize require = Dqn_CStr8_FVSize(fmt, args) + 1 /*null_terminate*/;
    Dqn_usize space   = (N + 1) - string->size;
    result            = require <= space;
    string->size += STB_SPRINTF_DECORATE(vsnprintf)(string->data + string->size, DQN_CAST(int)space, fmt, args);

    // NOTE: snprintf returns the required size of the format string
    // irrespective of if there's space or not.
    string->size = DQN_MIN(string->size, N);
    return result;
}

template <Dqn_usize N> bool Dqn_FStr8_AppendF(Dqn_FStr8<N> *string, DQN_FMT_ATTRIB char const *fmt, ...)
{
    bool result = false;
    if (!string || !fmt)
        return result;
    va_list args;
    va_start(args, fmt);
    result = Dqn_FStr8_AppendFV(string, fmt, args);
    va_end(args);
    return result;
}

template <Dqn_usize N> bool Dqn_FStr8_AppendCStr8(Dqn_FStr8<N> *string, char const *src, Dqn_usize size)
{
    DQN_ASSERT(string->size <= N);
    bool result = false;
    if (!string || !src || size == 0 || string->size >= N)
        return result;

    Dqn_usize space = N - string->size;
    result          = size <= space;
    DQN_MEMCPY(string->data + string->size, src, DQN_MIN(space, size));
    string->size = DQN_MIN(string->size + size, N);
    string->data[string->size] = 0;
    return result;
}

template <Dqn_usize N> bool Dqn_FStr8_Append(Dqn_FStr8<N> *string, Dqn_Str8 src)
{
    bool result = Dqn_FStr8_AppendCStr8(string, src.data, src.size);
    return result;
}

template <Dqn_usize N> Dqn_Str8 Dqn_FStr8_ToStr8(Dqn_FStr8<N> const *string)
{
    Dqn_Str8 result = {};
    if (!string || string->size <= 0)
        return result;

    result.data = DQN_CAST(char *)string->data;
    result.size = string->size;
    return result;
}

template <Dqn_usize N> bool Dqn_FStr8_Eq(Dqn_FStr8<N> const *lhs, Dqn_FStr8<N> const *rhs, Dqn_Str8EqCase eq_case)
{
    Dqn_Str8 lhs_s8 = Dqn_FStr8_ToStr8(lhs);
    Dqn_Str8 rhs_s8 = Dqn_FStr8_ToStr8(rhs);
    bool result = Dqn_Str8_Eq(lhs_s8, rhs_s8, eq_case);
    return result;
}

template <Dqn_usize N> bool Dqn_FStr8_EqStr8(Dqn_FStr8<N> const *lhs, Dqn_Str8 rhs, Dqn_Str8EqCase eq_case)
{
    Dqn_Str8 lhs_s8 = Dqn_FStr8_ToStr8(lhs);
    bool result = Dqn_Str8_Eq(lhs_s8, rhs, eq_case);
    return result;
}

template <Dqn_usize N> bool Dqn_FStr8_EqInsensitive(Dqn_FStr8<N> const *lhs, Dqn_FStr8<N> const *rhs)
{
    Dqn_Str8 lhs_s8 = Dqn_FStr8_ToStr8(lhs);
    Dqn_Str8 rhs_s8 = Dqn_FStr8_ToStr8(rhs);
    bool result     = Dqn_Str8_Eq(lhs_s8, rhs_s8, Dqn_Str8EqCase_Insensitive);
    return result;
}

template <Dqn_usize N> bool Dqn_FStr8_EqStr8Insensitive(Dqn_FStr8<N> const *lhs, Dqn_Str8 rhs)
{
    Dqn_Str8 lhs_s8 = Dqn_FStr8_ToStr8(lhs);
    bool result     = Dqn_Str8_Eq(lhs_s8, rhs, Dqn_Str8EqCase_Insensitive);
    return result;
}

template <Dqn_usize A, Dqn_usize B> bool Dqn_FStr8_EqFStr8(Dqn_FStr8<A> const *lhs, Dqn_FStr8<B> const *rhs, Dqn_Str8EqCase eq_case)
{
    Dqn_Str8 lhs_s8 = Dqn_FStr8_ToStr8(lhs);
    Dqn_Str8 rhs_s8 = Dqn_FStr8_ToStr8(rhs);
    bool result     = Dqn_Str8_Eq(lhs_s8, rhs_s8, eq_case);
    return result;
}

template <Dqn_usize A, Dqn_usize B> bool Dqn_FStr8_EqFStr8Insensitive(Dqn_FStr8<A> const *lhs, Dqn_FStr8<B> const *rhs)
{
    Dqn_Str8 lhs_s8 = Dqn_FStr8_ToStr8(lhs);
    Dqn_Str8 rhs_s8 = Dqn_FStr8_ToStr8(rhs);
    bool result = Dqn_Str8_Eq(lhs_s8, rhs_s8, Dqn_Str8EqCase_Insensitive);
    return result;
}
#endif // !defined(DQN_NO_FSTR8)
