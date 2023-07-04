// =================================================================================================
// [$CSTR] Dqn_CString8         |                             | C-string helpers
// =================================================================================================
// @proc Dqn_CString8_ArrayCount
//   @desc Calculate the size of a cstring literal/array at compile time
//   @param literal The cstring literal/array to calculate the size for
//   @return The size of the cstring not including the null-terminating byte
//
// @proc Dqn_CString8_FSize, Dqn_CString8_FVSize
//   Calculate the required size to format the given format cstring.
//   @param[in] fmt The format string to calculate the size for
//   @return The size required to format the string, not including the null
//   terminator.
//
// @proc Dqn_CString8_Size
//   @desc Calculate the string length of the null-terminated string.
//   @param[in] a The string whose length is to be determined
//   @return The length of the string

DQN_API template <Dqn_usize N> constexpr Dqn_usize Dqn_CString8_ArrayUCount(char const (&literal)[N]) { (void)literal; return N - 1; }
DQN_API template <Dqn_usize N> constexpr Dqn_usize Dqn_CString8_ArrayICount(char const (&literal)[N]) { (void)literal; return N - 1; }
DQN_API                                  Dqn_usize Dqn_CString8_FSize      (char const *fmt, ...);
DQN_API                                  Dqn_usize Dqn_CString8_FVSize     (char const *fmt, va_list args);
DQN_API                                  Dqn_usize Dqn_CString8_Size       (char const *a);
DQN_API                                  Dqn_usize Dqn_CString16_Size      (wchar_t const *a);

// =================================================================================================
// [$STR8] Dqn_String8          |                             | Pointer and length strings
// =================================================================================================
//
// NOTE: API

// @proc Dqn_String8_Init
//   @desc Initialise a string from a pointer and length
//   The string is invalid (i.e. Dqn_String8_IsValid() returns false) if size is
//   negative or the string is null.

// @proc Dqn_String8_InitCString
//   @desc Initialise a string from a cstring
//   The cstring must be null-terminated as its length is evaluated using
//   strlen. The string is invalid (i.e. Dqn_String8_IsValid() returns false) if
//   size is negative or the string is null.

// @proc Dqn_String8_InitF
//   @desc Create a string from a printf style format string
//   @param[in] allocator The allocator the string will be allocated from
//   @param[in] fmt The printf style format cstring

// @proc Dqn_String8_InitFV
//   @desc Create a string from a printf style format string using a va_list
//   @param[in] arena The allocator the string will be allocated from
//   @param[in] fmt The printf style format cstring
//   @param[in] va The variable argument list
//
// @proc Dqn_String8_IsValid
//   @desc Determine if the values of the given string are valid
//   A string is invalid if size is negative or the string is null.
//   @return True if the string is valid, false otherwise.

// @proc Dqn_String8 Dqn_String8_Slice
//   @desc Create a slice from a pre-existing string.
//   The requested slice is clamped to within the bounds of the original string.
//   @param[in] string The string to slice
//   @param[in] offset The starting byte to slice from
//   @param[in] size The size of the slice
//   @return The sliced string

// @proc Dqn_String8_BinarySplit
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

// @proc Dqn_String8_Split
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

// @proc Dqn_String8_Segment
//   @desc Segment a string by inserting the `segment_char` every `segment_size`
//   characters in the string. For example, '123456789' split with
//   `segment_char` ' ' and `segment_size` '3' would yield, '123 456 789'.

// @proc Dqn_String8_Allocate
//   @desc Create an empty string with the requested size
//   @param[in] allocator The allocator the string will be allocated from
//   @param[in] size The size in bytes of the string to allocate
//   @param[in] zero_mem Enum to indicate if the string's memory should be cleared

// @proc Dqn_String8_CopyCString
//   @desc Create a copy of the given cstring
//   @param[in] allocator The allocator the string will be allocated from
//   @param[in] string The cstring to copy
//   @param[in] size The size of the cstring to copy. This cannot be <= 0
//   @return A copy of the string, invalid string if any argument was invalid.

// @proc Dqn_String8_Copy
//   @desc Create a copy of the given string
//   @param[in] allocator The allocator the string will be allocated from
//   @param[in] string The string to copy
//   @return A copy of the string, invalid string if any argument was invalid.

// @proc Dqn_String8_Eq, Dqn_String8_EqInsensitive
//   @desc Compare a string for equality with or without case sensitivity.
//   @param[in] lhs The first string to compare equality with
//   @param[in] rhs The second string to compare equality with
//   @param[in] lhs The first string's size
//   @param[in] rhs The second string's size
//   @param[in] eq_case Set the comparison to be case sensitive or insensitive
//   @return True if the arguments are valid, non-null and the strings
//   are equal, false otherwise.

// @proc Dqn_String8_StartsWith, Dqn_String8_StartsWithInsensitive,
// Dqn_String8_EndsWith, Dqn_String8_EndswithInsensitive
//   @desc Check if a string starts/ends with the specified prefix
//   `EndsWithInsensitive` is case insensitive
//   @param[in] string The string to check for the prefix
//   @param[in] prefix The prefix to check against the string
//   @param[in] eq_case Set the comparison to be case sensitive or insensitive
//   @return True if the string is valid, non-null and has the specified prefix,
//   false otherwise.

// @proc Dqn_String8_TrimPrefix, Dqn_String8_TrimSuffix
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

// @proc Dqn_String8_TrimWhitespaceAround
//   @desc Trim whitespace from the prefix and suffix of the string
//
//   @param[in] string The string to trim
//   @param[in] string_size The size of the string
//   @param[out] trimmed_string The size of the trimmed string
//
//   @return The trimmed string. The original input string is returned if
//   arguments are invalid or no trim was possible.

// @proc Dqn_String8_TrimByteOrderMark
//   @desc Trim UTF8, UTF16 BE/LE, UTF32 BE/LE byte order mark prefix in the string.
//
//   @param[in] string The string to trim
//   @param[in] string_size The size of the string
//   @param[out] trimmed_string The size of the trimmed string
//
//   @return The trimmed string. The original input string is returned if
//   arguments are invalid or no trim was possible.

// @proc Dqn_String8_FileNameFromPath
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

// @proc Dqn_String8_ToI64Checked, Dqn_String8_ToI64,
//  Dqn_String8_ToU64Checked, Dqn_String8_ToU64
//   @desc Convert a number represented as a string to a signed 64 bit number.
//
//   The `separator` is an optional digit separator for example, if `separator`
//   is set to ',' then "1,234" will successfully be parsed to '1234'.
//
//   Real numbers are truncated. Both '-' and '+' prefixed strings are permitted,
//   i.e. "+1234" -> 1234 and "-1234" -> -1234. Strings must consist entirely of
//   digits, the seperator or the permitted prefixes as previously mentioned
//   otherwise this function will return false, i.e. "1234 dog" will cause the
//   function to return false, however, the output is greedily converted and will
//   be evaluated to "1234".
//
//   `ToU64Checked` and `ToU64` only '+' prefix is permitted
//   `ToI64Checked` and `ToI64` both '+' and '-' prefix is permitted
//
//   @param[in] buf The string to convert
//   @param[in] size The size of the string, pass '-1' to calculate the
//   null-terminated string length in the function.
//   @param[in] separator The character used to separate the digits, if any. Set
//   this to 0, if no separators are permitted.
//   @param[out] output The number to write the parsed value to
//
//   @return The checked variants return false if there were invalid characters
//   in the string true otherwise.
//   The non-checked variant returns the number that could optimistically be
//   parsed from the string e.g. "1234 dog" will return 1234.

// @proc Dqn_String8_Replace, Dqn_String8_ReplaceInsensitive
//   @desc TODO(doyle): Write description

// @proc Dqn_String8_Remove
//   @desc Remove the substring denoted by the begin index and the size from the string
//   string in-place using MEMMOVE to shift the string back.

// @proc Dqn_String8_Find
//   @desc @param start_index Set an index within the string string to start the search
//   from, if not desired, set to 0
//   @return A string that points to the matching find, otherwise a 0 length string.

// @proc DQN_STRING8
//   @desc Construct a UTF8 c-string literal into a Dqn_String8 referencing a 
//   string stored in the data-segment. This string is read-only.

// @proc DQN_STRING16
//   @desc Construct a UTF16 c-string literal into a Dqn_String16 referencing a string
//   stored in the data-segment. This string is read-only.

// @proc DQN_STRING_FMT
//   @desc Unpack a string into arguments for printing a string into a printf style
//   format string.

struct Dqn_String8Link
{
    Dqn_String8      string; ///< The string
    Dqn_String8Link *next;   ///< The next string in the linked list
};

struct Dqn_String16 /// A pointer and length style string that holds slices to UTF16 bytes.
{
    wchar_t   *data; ///< The UTF16 bytes of the string
    Dqn_usize  size; ///< The number of characters in the string

    #if defined(__cplusplus)
    wchar_t const *begin() const { return data; }        ///< Const begin iterator for range-for loops
    wchar_t const *end  () const { return data + size; } ///< Const end iterator for range-for loops
    wchar_t       *begin()       { return data; }        ///< Begin iterator for range-for loops
    wchar_t       *end  ()       { return data + size; } ///< End iterator for range-for loops
    #endif
};

struct Dqn_String8BinarySplitResult
{
    Dqn_String8 lhs;
    Dqn_String8 rhs;
};

struct Dqn_String8FindResult
{
    bool        found;
    Dqn_usize   index;
    Dqn_String8 match;
    Dqn_String8 match_to_end_of_buffer;
};

// NOTE: Macros ====================================================================================
#define DQN_STRING8(string)                              Dqn_String8{(char *)(string), sizeof(string) - 1}
#define DQN_STRING16(string)                             Dqn_String16{(wchar_t *)(string), (sizeof(string)/sizeof(string[0])) - 1}
#define DQN_STRING_FMT(string)                           (int)((string).size), (string).data

#if defined(__cplusplus)
#define Dqn_String8_Init(data, size)                     (Dqn_String8{(char *)(data), (Dqn_usize)(size)})
#else
#define Dqn_String8_Init(data, size)                     (Dqn_String8){(data), (size)}
#endif

#define Dqn_String8_InitF(allocator, fmt, ...)           Dqn_String8_InitF_(DQN_LEAK_TRACE allocator, fmt, ## __VA_ARGS__)
#define Dqn_String8_InitFV(allocator, fmt, args)         Dqn_String8_InitFV_(DQN_LEAK_TRACE allocator, fmt, args)
#define Dqn_String8_Allocate(allocator, size, zero_mem)  Dqn_String8_Allocate_(DQN_LEAK_TRACE allocator, size, zero_mem)
#define Dqn_String8_CopyCString(allocator, string, size) Dqn_String8_CopyCString_(DQN_LEAK_TRACE allocator, string, size)
#define Dqn_String8_Copy(allocator, string)              Dqn_String8_Copy_(DQN_LEAK_TRACE allocator, string)

// NOTE: API =======================================================================================
enum Dqn_String8IsAll
{
    Dqn_String8IsAll_Digits,
    Dqn_String8IsAll_Hex,
};

enum Dqn_String8EqCase
{
    Dqn_String8EqCase_Sensitive,
    Dqn_String8EqCase_Insensitive,
};

enum Dqn_String8FindFlag
{
    Dqn_String8FindFlag_Digit      = 1 << 0, // 0-9
    Dqn_String8FindFlag_Whitespace = 1 << 1, // '\r', '\t', '\n', ' '
    Dqn_String8FindFlag_Alphabet   = 1 << 2, // A-Z, a-z
    Dqn_String8FindFlag_Plus       = 1 << 3, // +
    Dqn_String8FindFlag_Minus      = 1 << 4, // -
    Dqn_String8FindFlag_AlphaNum   = Dqn_String8FindFlag_Alphabet | Dqn_String8FindFlag_Digit,
};

DQN_API Dqn_String8                  Dqn_String8_InitCString8          (char const *src);
DQN_API bool                         Dqn_String8_IsValid               (Dqn_String8 string);
DQN_API bool                         Dqn_String8_IsAll                 (Dqn_String8 string, Dqn_String8IsAll is_all);

DQN_API Dqn_String8                  Dqn_String8_Slice                 (Dqn_String8 string, Dqn_usize offset, Dqn_usize size);
DQN_API Dqn_String8BinarySplitResult Dqn_String8_BinarySplitArray      (Dqn_String8 string, Dqn_String8 const *find, Dqn_usize find_size);
DQN_API Dqn_String8BinarySplitResult Dqn_String8_BinarySplit           (Dqn_String8 string, Dqn_String8 find);
DQN_API Dqn_usize                    Dqn_String8_Split                 (Dqn_String8 string, Dqn_String8 delimiter, Dqn_String8 *splits, Dqn_usize splits_count);
DQN_API Dqn_String8FindResult        Dqn_String8_FindFirstStringArray  (Dqn_String8 string, Dqn_String8 const *find, Dqn_usize find_size);
DQN_API Dqn_String8FindResult        Dqn_String8_FindFirstString       (Dqn_String8 string, Dqn_String8 find);
DQN_API Dqn_String8FindResult        Dqn_String8_FindFirst             (Dqn_String8 string, uint32_t flags);
DQN_API Dqn_String8                  Dqn_String8_Segment               (Dqn_Allocator allocator, Dqn_String8 src, Dqn_usize segment_size, char segment_char);

DQN_API bool                         Dqn_String8_Eq                    (Dqn_String8 lhs, Dqn_String8 rhs, Dqn_String8EqCase eq_case = Dqn_String8EqCase_Sensitive);
DQN_API bool                         Dqn_String8_EqInsensitive         (Dqn_String8 lhs, Dqn_String8 rhs);
DQN_API bool                         Dqn_String8_StartsWith            (Dqn_String8 string, Dqn_String8 prefix, Dqn_String8EqCase eq_case = Dqn_String8EqCase_Sensitive);
DQN_API bool                         Dqn_String8_StartsWithInsensitive (Dqn_String8 string, Dqn_String8 prefix);
DQN_API bool                         Dqn_String8_EndsWith              (Dqn_String8 string, Dqn_String8 prefix, Dqn_String8EqCase eq_case = Dqn_String8EqCase_Sensitive);
DQN_API bool                         Dqn_String8_EndsWithInsensitive   (Dqn_String8 string, Dqn_String8 prefix);
DQN_API bool                         Dqn_String8_HasChar               (Dqn_String8 string, char ch);

DQN_API Dqn_String8                  Dqn_String8_TrimPrefix            (Dqn_String8 string, Dqn_String8 prefix, Dqn_String8EqCase eq_case = Dqn_String8EqCase_Sensitive);
DQN_API Dqn_String8                  Dqn_String8_TrimSuffix            (Dqn_String8 string, Dqn_String8 suffix, Dqn_String8EqCase eq_case = Dqn_String8EqCase_Sensitive);
DQN_API Dqn_String8                  Dqn_String8_TrimWhitespaceAround  (Dqn_String8 string);
DQN_API Dqn_String8                  Dqn_String8_TrimByteOrderMark     (Dqn_String8 string);

DQN_API Dqn_String8                  Dqn_String8_FileNameFromPath      (Dqn_String8 path);

DQN_API bool                         Dqn_String8_ToU64Checked          (Dqn_String8 string, char separator, uint64_t *output);
DQN_API uint64_t                     Dqn_String8_ToU64                 (Dqn_String8 string, char separator);
DQN_API bool                         Dqn_String8_ToI64Checked          (Dqn_String8 string, char separator, int64_t *output);
DQN_API int64_t                      Dqn_String8_ToI64                 (Dqn_String8 string, char separator);

DQN_API Dqn_String8                  Dqn_String8_Replace               (Dqn_String8 string, Dqn_String8 find, Dqn_String8 replace, Dqn_usize start_index, Dqn_Allocator allocator, Dqn_String8EqCase eq_case = Dqn_String8EqCase_Sensitive);
DQN_API Dqn_String8                  Dqn_String8_ReplaceInsensitive    (Dqn_String8 string, Dqn_String8 find, Dqn_String8 replace, Dqn_usize start_index, Dqn_Allocator allocator);
DQN_API void                         Dqn_String8_Remove                (Dqn_String8 *string, Dqn_usize offset, Dqn_usize size);

#if defined(__cplusplus)
DQN_API bool                         operator==                        (Dqn_String8 const &lhs, Dqn_String8 const &rhs);
DQN_API bool                         operator!=                        (Dqn_String8 const &lhs, Dqn_String8 const &rhs);
#endif

// NOTE: Internal ==================================================================================
DQN_API Dqn_String8 Dqn_String8_InitF_               (DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, char const *fmt, ...);
DQN_API Dqn_String8 Dqn_String8_InitFV_              (DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, char const *fmt, va_list args);
DQN_API Dqn_String8 Dqn_String8_Allocate_            (DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, Dqn_usize size, Dqn_ZeroMem zero_mem);
DQN_API Dqn_String8 Dqn_String8_CopyCString_         (DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, char const *string, Dqn_usize size);
DQN_API Dqn_String8 Dqn_String8_Copy_                (DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, Dqn_String8 string);

#if !defined(DQN_NO_FSTRING8)
// =================================================================================================
// [$FSTR] Dqn_FString8         | DQN_NO_FSTRING8             | Fixed-size strings
// =================================================================================================
//
// NOTE: API
//
// @proc Dqn_FString8_InitF
//   @desc Create a fixed string from the format string. The result string is
//   null-terminated.
//   @param fmt[in] Format string specifier to create the fixed string from
//   @return The created string, truncated if there was insufficient space
//
// @proc Dqn_FString8_Max
//   @desc @param string[in] The string to query the maximum capacity of
//   @return Maximum capacity of the fixed string
//
// @proc Dqn_FString8_Clear
//   @desc Reset the characters in the string
//   @param string[in] The string to clear
//
// @proc Dqn_FString8_AppendFV
//   @desc Append a format string to the fixed string. On failure the string is
//   appended to but truncated ensuring null-termination.
//   @param string[in] The string to append to
//   @param fmt[in] Format string to append to the fixed string
//   @return True if append was successful, false otherwise.
//
// @proc Dqn_FString8_AppendF
//   @desc @copydocs Dqn_FString8_AppendF
//
// @proc Dqn_FString8_AppendCString8
//   @desc Append a cstring to the fixed string. On failure the string is
//   appended to but truncated ensuring null-termination.
//   @param string[in] The string to append to
//   @param value[in] Cstring to append to the fixed string
//   @param size[in] Size of the cstring
//   @return True if append was successful, false otherwise.
//
// @proc Dqn_FString8_Append
//   @desc Append a string to the fixed string. On failure the string is
//   appended to but truncated ensuring null-termination.
//   @param string[in] The string to append to
//   @param value[in] String to append to the fixed string
//   determined before appending.
//   @return True if append was successful, false otherwise.
//
// @proc Dqn_FString8_ToString8
//   @desc Convert a fixed string to a string. The string holds a reference to the
//   fixed string and is invalidated once fixed string is deleted.
//   @param string[in] The fixed string to create a string from
//   @return String referencing the contents of `string`
//
// @proc Dqn_FString8_Eq
//   @desc @see Dqn_String8_Eq
//
// @proc Dqn_FString8_EqString8
//   @desc @see Dqn_String8_Eq
//
// @proc Dqn_FString8_EqInsensitive
//   @desc Compare a string for equality, case insensitive
//   @see Dqn_String8_Eq
//
// @proc Dqn_FString8_EqString8Insensitive
//   @desc Compare a string for equality, case insensitive
//   @see Dqn_String8_Eq

template <Dqn_usize N> struct Dqn_FString8
{
    char      data[N+1];
    Dqn_usize size;

    bool operator==(Dqn_FString8 const &other) const {
        if (size != other.size) return false;
        bool result = DQN_MEMCMP(data, other.data, size);
        return result;
    }

    bool operator!=(Dqn_FString8 const &other) const { return !(*this == other); }

    char       *begin()       { return data; }
    char       *end  ()       { return data + size; }
    char const *begin() const { return data; }
    char const *end  () const { return data + size; }
};

template <Dqn_usize N>              Dqn_FString8<N> Dqn_FString8_InitF                (char const *fmt, ...);
template <Dqn_usize N>              Dqn_usize       Dqn_FString8_Max                  (Dqn_FString8<N> const *string);
template <Dqn_usize N>              void            Dqn_FString8_Clear                (Dqn_FString8<N> *string);
template <Dqn_usize N>              bool            Dqn_FString8_AppendFV             (Dqn_FString8<N> *string, char const *fmt, va_list va);
template <Dqn_usize N>              bool            Dqn_FString8_AppendF              (Dqn_FString8<N> *string, char const *fmt, ...);
template <Dqn_usize N>              bool            Dqn_FString8_AppendCString8       (Dqn_FString8<N> *string, char const *value, Dqn_usize size);
template <Dqn_usize N>              bool            Dqn_FString8_Append               (Dqn_FString8<N> *string, Dqn_String8 value);
template <Dqn_usize N>              Dqn_String8     Dqn_FString8_ToString8            (Dqn_FString8<N> const *string);
template <Dqn_usize N>              bool            Dqn_FString8_Eq                   (Dqn_FString8<N> const *lhs, Dqn_FString8<N> const *rhs, Dqn_String8EqCase eq_case);
template <Dqn_usize N>              bool            Dqn_FString8_EqString8            (Dqn_FString8<N> const *lhs, Dqn_String8 rhs, Dqn_String8EqCase eq_case);
template <Dqn_usize N>              bool            Dqn_FString8_EqInsensitive        (Dqn_FString8<N> const *lhs, Dqn_FString8<N> const *rhs);
template <Dqn_usize N>              bool            Dqn_FString8_EqString8Insensitive (Dqn_FString8<N> const *lhs, Dqn_String8 rhs);
template <Dqn_usize A, Dqn_usize B> bool            Dqn_FString8_EqFString8           (Dqn_FString8<A> const *lhs, Dqn_FString8<B> const *rhs, Dqn_String8EqCase eq_case);
template <Dqn_usize A, Dqn_usize B> bool            Dqn_FString8_EqFString8Insensitive(Dqn_FString8<A> const *lhs, Dqn_FString8<B> const *rhs);
#endif // !defined(DQN_NO_FSTRING8)

// =================================================================================================
// [$STRB] Dqn_String8Builder   |                             |
// =================================================================================================
struct Dqn_String8Builder
{
    Dqn_Allocator    allocator;   ///< Allocator to use to back the string list
    Dqn_String8Link *head;        ///< First string in the linked list of strings
    Dqn_String8Link *tail;        ///< Last string in the linked list of strings
    Dqn_usize        string_size; ///< The size in bytes necessary to construct the current string
    Dqn_usize        count;       ///< The number of links in the linked list of strings
};

/// Append a string to the list of strings in the builder by reference.
/// The string's data must persist whilst the string builder is being used.
/// @param builder The builder to append the string to
/// @param string The string to append to the builder
/// @return True if append was successful, false if parameters are invalid
/// or memory allocation failure.
bool Dqn_String8Builder_AppendRef(Dqn_String8Builder *builder, Dqn_String8 string);

/// Append a string to the list of strings in the builder by copy.
/// The string is copied using the builder's allocator before appending.
/// @param builder The builder to append the string to
/// @param string The string to append to the builder
/// @return True if append was successful, false if parameters are invalid
/// or memory allocation failure.
bool Dqn_String8Builder_AppendCopy(Dqn_String8Builder *builder, Dqn_String8 string);

/// @copydoc Dqn_String8Builder_AppendF
/// @param args The variable argument list to use in the format string
#define Dqn_String8Builder_AppendFV(builder, fmt, args) Dqn_String8Builder_AppendFV_(DQN_LEAK_TRACE builder, fmt, args)
bool Dqn_String8Builder_AppendFV_(DQN_LEAK_TRACE_FUNCTION Dqn_String8Builder *builder, char const *fmt, va_list args);

/// Append a printf-style format string to the list of strings in the builder.
/// @param builder The builder to append the string to
/// @param fmt The format string to use
/// @return True if append was successful, false if parameters are invalid
/// or memory allocation failure.
bool Dqn_String8Builder_AppendF(Dqn_String8Builder *builder, char const *fmt, ...);

/// Build the list of strings into the final composite string from the string
/// builder
/// @param builder The string builder to build the string from
/// @param allocator The allocator to use to build the string
/// @return The string if build was successful, empty string if parameters are
/// invalid or memory allocation failure.
Dqn_String8 Dqn_String8Builder_Build(Dqn_String8Builder const *builder, Dqn_Allocator allocator);

// =================================================================================================
// [$JSON] Dqn_JSONBuilder      | DQN_NO_JSON_BUILDER         | Construct json output
// =================================================================================================
#if !defined(DQN_NO_JSON_BUILDER)
// TODO(dqn): We need to write tests for this
enum Dqn_JSONBuilderItem {
    Dqn_JSONBuilderItem_Empty,
    Dqn_JSONBuilderItem_OpenContainer,
    Dqn_JSONBuilderItem_CloseContainer,
    Dqn_JSONBuilderItem_KeyValue,
};

/// Basic helper class to construct JSON string output
struct Dqn_JSONBuilder {
    bool                use_stdout;        ///< When set, ignore the string builder and dump immediately to stdout
    Dqn_String8Builder  string_builder;    ///< (Internal)
    int                 indent_level;      ///< (Internal)
    int                 spaces_per_indent; ///< The number of spaces per indent level
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


/// Initialise a JSON builder
Dqn_JSONBuilder Dqn_JSONBuilder_Init(Dqn_Allocator allocator, int spaces_per_indent);

/// Convert the internal JSON buffer in the builder into a string.
///
/// @param[in] arena The allocator to use to build the string
Dqn_String8 Dqn_JSONBuilder_Build(Dqn_JSONBuilder const *builder, Dqn_Allocator allocator);

/// Add a JSON key value pair untyped. The value is emitted directly without
/// checking the contents of value.
///
/// All other functions internally call into this function which is the main
/// workhorse of the builder.
void Dqn_JSONBuilder_KeyValue(Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value);

void Dqn_JSONBuilder_KeyValueF(Dqn_JSONBuilder *builder, Dqn_String8 key, char const *value_fmt, ...);

/// Begin a named JSON object for writing into in the builder
///
/// Generates internally a string like '"<name>": {'
void    Dqn_JSONBuilder_ObjectBeginNamed(Dqn_JSONBuilder *builder, Dqn_String8 name);
#define Dqn_JSONBuilder_ObjectBegin(builder) Dqn_JSONBuilder_ObjectBeginNamed(builder, DQN_STRING8(""))

/// End a JSON object for writing into in the builder
///
/// Generates internally a string like '}'
void  Dqn_JSONBuilder_ObjectEnd(Dqn_JSONBuilder *builder);

/// Begin a named JSON array for writing into in the builder
///
/// Generates internally a string like '"<name>": ['
void    Dqn_JSONBuilder_ArrayBeginNamed(Dqn_JSONBuilder *builder, Dqn_String8 name);
#define Dqn_JSONBuilder_ArrayBegin(builder) Dqn_JSONBuilder_ArrayBeginNamed(builder, DQN_STRING8(""))

/// Begin a named JSON array for writing into in the builder
///
/// Generates internally a string like ']'
void Dqn_JSONBuilder_ArrayEnd(Dqn_JSONBuilder *builder);

/// Add a named JSON string key-value object
///
/// Generates internally a string like '"<key>": "<value>"'
void    Dqn_JSONBuilder_StringNamed(Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value);
#define Dqn_JSONBuilder_String(builder, value) Dqn_JSONBuilder_StringNamed(builder, DQN_STRING8(""), value)

/// Add a named JSON key-value object whose value is directly written
///
/// Generates internally a string like '"<key>": <value>' (e.g. useful for
/// emitting the 'null' value)
void    Dqn_JSONBuilder_LiteralNamed(Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value);
#define Dqn_JSONBuilder_Literal(builder, value) Dqn_JSONBuilder_LiteralNamed(builder, DQN_STRING8(""), value)

/// Add a named JSON u64 key-value object
///
/// Generates internally a string like '"<key>": <value>'
void    Dqn_JSONBuilder_U64Named(Dqn_JSONBuilder *builder, Dqn_String8 key, uint64_t value);
#define Dqn_JSONBuilder_U64(builder, value) Dqn_JSONBuilder_U64Named(builder, DQN_STRING8(""), value)

/// Add a JSON i64 key-value object
///
/// Generates internally a string like '"<key>": <value>'
void    Dqn_JSONBuilder_I64Named(Dqn_JSONBuilder *builder, Dqn_String8 key, int64_t value);
#define Dqn_JSONBuilder_I64(builder, value) Dqn_JSONBuilder_I64Named(builder, DQN_STRING8(""), value)

/// Add a JSON f64 key-value object
///
/// Generates internally a string like '"<key>": <value>'
/// @param[in] decimal_places The number of decimal places to preserve. Maximum 16
void    Dqn_JSONBuilder_F64Named(Dqn_JSONBuilder *builder, Dqn_String8 key, double value, int decimal_places);
#define Dqn_JSONBuilder_F64(builder, value) Dqn_JSONBuilder_F64Named(builder, DQN_STRING8(""), value)

/// Add a JSON bool key-value object
///
/// Generates internally a string like '"<key>": <value>'
void    Dqn_JSONBuilder_BoolNamed(Dqn_JSONBuilder *builder, Dqn_String8 key, bool value);
#define Dqn_JSONBuilder_Bool(builder, value) Dqn_JSONBuilder_BoolNamed(builder, DQN_STRING8(""), value)
#endif // !defined(DQN_NO_JSON_BUIDLER)


// =================================================================================================
// [$CHAR] Dqn_Char             |                             | Character ascii/digit.. helpers
// =================================================================================================
DQN_API bool    Dqn_Char_IsAlphabet    (char ch);
DQN_API bool    Dqn_Char_IsDigit       (char ch);
DQN_API bool    Dqn_Char_IsAlphaNum    (char ch);
DQN_API bool    Dqn_Char_IsWhitespace  (char ch);
DQN_API bool    Dqn_Char_IsHex         (char ch);
DQN_API uint8_t Dqn_Char_HexToU8       (char ch);
DQN_API char    Dqn_Char_ToHex         (char ch);
DQN_API char    Dqn_Char_ToHexUnchecked(char ch);
DQN_API char    Dqn_Char_ToLower       (char ch);

// =================================================================================================
// [$UTFX] Dqn_UTF              |                             | Unicode helpers
// =================================================================================================
DQN_API int Dqn_UTF8_EncodeCodepoint(uint8_t utf8[4], uint32_t codepoint);
DQN_API int Dqn_UTF16_EncodeCodepoint(uint16_t utf16[2], uint32_t codepoint);

#if !defined(DQN_NO_HEX)
// =================================================================================================
// [$BHEX] Dqn_Bin              | DQN_NO_HEX                  | Binary <-> hex helpers
// =================================================================================================
//
// NOTE: API
//
// @proc Dqn_Bin_U64ToHexU64String
//   @desc Convert a 64 bit number to a hex string
//   @param[in] number Number to convert to hexadecimal representation
//   @param[in] flags Bit flags from Dqn_BinHexU64StringFlags to customise the
//   output of the hexadecimal string.
//   @return The hexadecimal representation of the number. This string is always
//   null-terminated.
//
// @proc Dqn_Bin_U64ToHex
//   @copybrief Dqn_Bin_U64ToHexU64String
//
//   @param[in] allocator The memory allocator to use for the memory of the
//   hexadecimal string.
//   @copyparams Dqn_Bin_U64ToHexU64String
//
// @proc Dqn_Bin_HexBufferToU64
//   @desc Convert a hexadecimal string a 64 bit value.
//   Asserts if the hex string is too big to be converted into a 64 bit number.
//
// @proc Dqn_Bin_HexToU64
//   @copydoc Dqn_Bin_HexToU64
//
// @proc Dqn_Bin_BytesToHexBuffer
//   @desc Convert a binary buffer into its hex representation into dest.
//
//   The dest buffer must be large enough to contain the hex representation, i.e.
//   atleast (src_size * 2).
//
//   @return True if the conversion into the dest buffer was successful, false
//   otherwise (e.g. invalid arguments).
//
// @proc Dqn_Bin_BytesToHexBufferArena
//   @desc Convert a series of bytes into a string
//   @return A null-terminated hex string, null pointer if allocation failed
//
// @proc Dqn_Bin_BytesToHexArena
//   @copydoc Dqn_Bin_BytesToHexBufferArena
//   @return A hex string, the string is invalid if conversion failed.
//
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
//
// @proc Dqn_Bin_HexToBytes
//   @desc String8 variant of @see Dqn_Bin_HexBufferToBytes
//
// @proc Dqn_Bin_StringHexBufferToBytesUnchecked
//   @desc Unchecked variant of @see Dqn_Bin_HexBufferToBytes
//
//   This function skips some string checks, it assumes the hex is a valid hex
//   stream and that the arguments are valid e.g. no trimming or 0x prefix
//   stripping is performed
//
// @proc Dqn_Bin_String
//   @desc String8 variant of @see Dqn_Bin_HexBufferToBytesUnchecked
//
// @proc Dqn_Bin_HexBufferToBytesArena
//   Dynamic allocating variant of @see Dqn_Bin_HexBufferToBytesUnchecked
//
//   @param[in] arena The arena to allocate the bytes from
//   @param[in] hex Hex string to convert into bytes
//   @param[in] size Size of the hex string
//   @param[out] real_size The size of the buffer returned by the function
//
//   @return The byte representation of the hex string.
//
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
#endif // !defined(DQN_NO_HEX)

/// Write the format string to the buffer truncating with a trailing ".." if
/// there is insufficient space in the buffer followed by null-terminating the
/// buffer (uses stb_sprintf underneath).
/// @return The size of the string written to the buffer *not* including the
/// null-terminator.
DQN_API int Dqn_SNPrintF2DotsOnOverflow(char *buffer, int size, char const *fmt, ...);

struct Dqn_U64String
{
    char    data[27+1]; // NOTE(dqn): 27 is the maximum size of uint64_t including a separtor
    uint8_t size;
};

/// Convert a 64 bit unsigned value to its string representation.
/// @param[in] val Value to convert into a string
/// @param[in] separator The separator to insert every 3 digits. Set this to
/// 0 if no separator is desired.
DQN_API Dqn_U64String Dqn_U64ToString(uint64_t val, char separator);

// =================================================================================================
// [$STBS] stb_sprintf          |                             | Portable sprintf
// =================================================================================================
/// @cond NO_DOXYYGEN
// stb_sprintf - v1.10 - public domain snprintf() implementation
// originally by Jeff Roberts / RAD Game Tools, 2015/10/20
// http://github.com/nothings/stb
//
// allowed types:  sc uidBboXx p AaGgEef n
// lengths      :  hh h ll j z t I64 I32 I
//
// Contributors:
//    Fabian "ryg" Giesen (reformatting)
//    github:aganm (attribute format)
//
// Contributors (bugfixes):
//    github:d26435
//    github:trex78
//    github:account-login
//    Jari Komppa (SI suffixes)
//    Rohit Nirmal
//    Marcin Wojdyr
//    Leonard Ritter
//    Stefano Zanotti
//    Adam Allison
//    Arvid Gerstmann
//    Markus Kolb
//
// LICENSE:
//
//   See end of file for license information.

#ifndef STB_SPRINTF_H_INCLUDE
#define STB_SPRINTF_H_INCLUDE

/*
Single file sprintf replacement.

Originally written by Jeff Roberts at RAD Game Tools - 2015/10/20.
Hereby placed in public domain.

This is a full sprintf replacement that supports everything that
the C runtime sprintfs support, including float/double, 64-bit integers,
hex floats, field parameters (%*.*d stuff), length reads backs, etc.

Why would you need this if sprintf already exists?  Well, first off,
it's *much* faster (see below). It's also much smaller than the CRT
versions code-space-wise. We've also added some simple improvements
that are super handy (commas in thousands, callbacks at buffer full,
for example). Finally, the format strings for MSVC and GCC differ
for 64-bit integers (among other small things), so this lets you use
the same format strings in cross platform code.

It uses the standard single file trick of being both the header file
and the source itself. If you just include it normally, you just get
the header file function definitions. To get the code, you include
it from a C or C++ file and define STB_SPRINTF_IMPLEMENTATION first.

It only uses va_args macros from the C runtime to do it's work. It
does cast doubles to S64s and shifts and divides U64s, which does
drag in CRT code on most platforms.

It compiles to roughly 8K with float support, and 4K without.
As a comparison, when using MSVC static libs, calling sprintf drags
in 16K.

API:
====
int stbsp_sprintf( char * buf, char const * fmt, ... )
int stbsp_snprintf( char * buf, int count, char const * fmt, ... )
  Convert an arg list into a buffer.  stbsp_snprintf always returns
  a zero-terminated string (unlike regular snprintf).

int stbsp_vsprintf( char * buf, char const * fmt, va_list va )
int stbsp_vsnprintf( char * buf, int count, char const * fmt, va_list va )
  Convert a va_list arg list into a buffer.  stbsp_vsnprintf always returns
  a zero-terminated string (unlike regular snprintf).

int stbsp_vsprintfcb( STBSP_SPRINTFCB * callback, void * user, char * buf, char const * fmt, va_list va )
    typedef char * STBSP_SPRINTFCB( char const * buf, void * user, int len );
  Convert into a buffer, calling back every STB_SPRINTF_MIN chars.
  Your callback can then copy the chars out, print them or whatever.
  This function is actually the workhorse for everything else.
  The buffer you pass in must hold at least STB_SPRINTF_MIN characters.
    // you return the next buffer to use or 0 to stop converting

void stbsp_set_separators( char comma, char period )
  Set the comma and period characters to use.

FLOATS/DOUBLES:
===============
This code uses a internal float->ascii conversion method that uses
doubles with error correction (double-doubles, for ~105 bits of
precision).  This conversion is round-trip perfect - that is, an atof
of the values output here will give you the bit-exact double back.

One difference is that our insignificant digits will be different than
with MSVC or GCC (but they don't match each other either).  We also
don't attempt to find the minimum length matching float (pre-MSVC15
doesn't either).

If you don't need float or doubles at all, define STB_SPRINTF_NOFLOAT
and you'll save 4K of code space.

64-BIT INTS:
============
This library also supports 64-bit integers and you can use MSVC style or
GCC style indicators (%I64d or %lld).  It supports the C99 specifiers
for size_t and ptr_diff_t (%jd %zd) as well.

EXTRAS:
=======
Like some GCCs, for integers and floats, you can use a ' (single quote)
specifier and commas will be inserted on the thousands: "%'d" on 12345
would print 12,345.

For integers and floats, you can use a "$" specifier and the number
will be converted to float and then divided to get kilo, mega, giga or
tera and then printed, so "%$d" 1000 is "1.0 k", "%$.2d" 2536000 is
"2.53 M", etc. For byte values, use two $:s, like "%$$d" to turn
2536000 to "2.42 Mi". If you prefer JEDEC suffixes to SI ones, use three
$:s: "%$$$d" -> "2.42 M". To remove the space between the number and the
suffix, add "_" specifier: "%_$d" -> "2.53M".

In addition to octal and hexadecimal conversions, you can print
integers in binary: "%b" for 256 would print 100.

PERFORMANCE vs MSVC 2008 32-/64-bit (GCC is even slower than MSVC):
===================================================================
"%d" across all 32-bit ints (4.8x/4.0x faster than 32-/64-bit MSVC)
"%24d" across all 32-bit ints (4.5x/4.2x faster)
"%x" across all 32-bit ints (4.5x/3.8x faster)
"%08x" across all 32-bit ints (4.3x/3.8x faster)
"%f" across e-10 to e+10 floats (7.3x/6.0x faster)
"%e" across e-10 to e+10 floats (8.1x/6.0x faster)
"%g" across e-10 to e+10 floats (10.0x/7.1x faster)
"%f" for values near e-300 (7.9x/6.5x faster)
"%f" for values near e+300 (10.0x/9.1x faster)
"%e" for values near e-300 (10.1x/7.0x faster)
"%e" for values near e+300 (9.2x/6.0x faster)
"%.320f" for values near e-300 (12.6x/11.2x faster)
"%a" for random values (8.6x/4.3x faster)
"%I64d" for 64-bits with 32-bit values (4.8x/3.4x faster)
"%I64d" for 64-bits > 32-bit values (4.9x/5.5x faster)
"%s%s%s" for 64 char strings (7.1x/7.3x faster)
"...512 char string..." ( 35.0x/32.5x faster!)
*/

#if defined(__clang__)
 #if defined(__has_feature) && defined(__has_attribute)
  #if __has_feature(address_sanitizer)
   #if __has_attribute(__no_sanitize__)
    #define STBSP__ASAN __attribute__((__no_sanitize__("address")))
   #elif __has_attribute(__no_sanitize_address__)
    #define STBSP__ASAN __attribute__((__no_sanitize_address__))
   #elif __has_attribute(__no_address_safety_analysis__)
    #define STBSP__ASAN __attribute__((__no_address_safety_analysis__))
   #endif
  #endif
 #endif
#elif defined(__GNUC__) && (__GNUC__ >= 5 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
 #if defined(__SANITIZE_ADDRESS__) && __SANITIZE_ADDRESS__
  #define STBSP__ASAN __attribute__((__no_sanitize_address__))
 #endif
#endif

#ifndef STBSP__ASAN
#define STBSP__ASAN
#endif

#ifdef STB_SPRINTF_STATIC
#define STBSP__PUBLICDEC static
#define STBSP__PUBLICDEF static STBSP__ASAN
#else
#ifdef __cplusplus
#define STBSP__PUBLICDEC extern "C"
#define STBSP__PUBLICDEF extern "C" STBSP__ASAN
#else
#define STBSP__PUBLICDEC extern
#define STBSP__PUBLICDEF STBSP__ASAN
#endif
#endif

#if defined(__has_attribute)
 #if __has_attribute(format)
   #define STBSP__ATTRIBUTE_FORMAT(fmt,va) __attribute__((format(printf,fmt,va)))
 #endif
#endif

#ifndef STBSP__ATTRIBUTE_FORMAT
#define STBSP__ATTRIBUTE_FORMAT(fmt,va)
#endif

#ifdef _MSC_VER
#define STBSP__NOTUSED(v)  (void)(v)
#else
#define STBSP__NOTUSED(v)  (void)sizeof(v)
#endif

#include <stdarg.h> // for va_arg(), va_list()
#include <stddef.h> // size_t, ptrdiff_t

#ifndef STB_SPRINTF_MIN
#define STB_SPRINTF_MIN 512 // how many characters per callback
#endif
typedef char *STBSP_SPRINTFCB(const char *buf, void *user, int len);

#ifndef STB_SPRINTF_DECORATE
#define STB_SPRINTF_DECORATE(name) stbsp_##name // define this before including if you want to change the names
#endif

STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(vsprintf)(char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(vsnprintf)(char *buf, int count, char const *fmt, va_list va);
STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(sprintf)(char *buf, char const *fmt, ...) STBSP__ATTRIBUTE_FORMAT(2,3);
STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...) STBSP__ATTRIBUTE_FORMAT(3,4);

STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEC void STB_SPRINTF_DECORATE(set_separators)(char comma, char period);
#endif // STB_SPRINTF_H_INCLUDE
/// @endcond

#if !defined(DQN_NO_FSTRING8)
// =================================================================================================
// [$FSTR] Dqn_FString8         | DQN_NO_FSTRING8             | Fixed-size strings
// =================================================================================================
template <Dqn_usize N> Dqn_FString8<N> Dqn_FString8_InitF(char const *fmt, ...)
{
    Dqn_FString8<N> result = {};
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        Dqn_FString8_AppendFV(&result, fmt, args);
        va_end(args);
    }
    return result;
}

template <Dqn_usize N> Dqn_usize Dqn_FString8_Max(Dqn_FString8<N> const *)
{
    Dqn_usize result = N;
    return result;
}

template <Dqn_usize N> void Dqn_FString8_Clear(Dqn_FString8<N> *string)
{
    *string = {};
}

template <Dqn_usize N> bool Dqn_FString8_AppendFV(Dqn_FString8<N> *string, char const *fmt, va_list args)
{
    bool result = false;
    if (!string || !fmt)
        return result;

    DQN_HARD_ASSERT(string->size >= 0);
    Dqn_usize require = Dqn_CString8_FVSize(fmt, args) + 1 /*null_terminate*/;
    Dqn_usize space   = (N + 1) - string->size;
    result            = require <= space;
    string->size += STB_SPRINTF_DECORATE(vsnprintf)(string->data + string->size, DQN_CAST(int)space, fmt, args);

    // NOTE: snprintf returns the required size of the format string
    // irrespective of if there's space or not.
    string->size = DQN_MIN(string->size, N);
    return result;
}

template <Dqn_usize N> bool Dqn_FString8_AppendF(Dqn_FString8<N> *string, char const *fmt, ...)
{
    bool result = false;
    if (!string || !fmt)
        return result;

    DQN_HARD_ASSERT(string->size >= 0);

    va_list args;
    va_start(args, fmt);
    result = Dqn_FString8_AppendFV(string, fmt, args);
    va_end(args);
    return result;
}

template <Dqn_usize N> bool Dqn_FString8_AppendCString8(Dqn_FString8<N> *string, char const *src, Dqn_usize size)
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

template <Dqn_usize N> bool Dqn_FString8_Append(Dqn_FString8<N> *string, Dqn_String8 src)
{
    bool result = Dqn_FString8_AppendCString8(string, src.data, src.size);
    return result;
}

template <Dqn_usize N> Dqn_String8 Dqn_FString8_ToString8(Dqn_FString8<N> const *string)
{
    Dqn_String8 result = {};
    if (!string || string->size <= 0)
        return result;

    result.data = DQN_CAST(char *)string->data;
    result.size = string->size;
    return result;
}

template <Dqn_usize N> bool Dqn_FString8_Eq(Dqn_FString8<N> const *lhs, Dqn_FString8<N> const *rhs, Dqn_String8EqCase eq_case)
{
    Dqn_String8 lhs_s8 = Dqn_FString8_ToString8(lhs);
    Dqn_String8 rhs_s8 = Dqn_FString8_ToString8(rhs);
    bool result = Dqn_String8_Eq(lhs_s8, rhs_s8, eq_case);
    return result;
}

template <Dqn_usize N> bool Dqn_FString8_EqString8(Dqn_FString8<N> const *lhs, Dqn_String8 rhs, Dqn_String8EqCase eq_case)
{
    Dqn_String8 lhs_s8 = Dqn_FString8_ToString8(lhs);
    bool result = Dqn_String8_Eq(lhs_s8, rhs, eq_case);
    return result;
}

template <Dqn_usize N> bool Dqn_FString8_EqInsensitive(Dqn_FString8<N> const *lhs, Dqn_FString8<N> const *rhs)
{
    Dqn_String8 lhs_s8 = Dqn_FString8_ToString8(lhs);
    Dqn_String8 rhs_s8 = Dqn_FString8_ToString8(rhs);
    bool result        = Dqn_String8_Eq(lhs_s8, rhs_s8, Dqn_String8EqCase_Insensitive);
    return result;
}

template <Dqn_usize N> bool Dqn_FString8_EqString8Insensitive(Dqn_FString8<N> const *lhs, Dqn_String8 rhs)
{
    Dqn_String8 lhs_s8 = Dqn_FString8_ToString8(lhs);
    bool result        = Dqn_String8_Eq(lhs_s8, rhs, Dqn_String8EqCase_Insensitive);
    return result;
}

template <Dqn_usize A, Dqn_usize B> bool Dqn_FString8_EqFString8(Dqn_FString8<A> const *lhs, Dqn_FString8<B> const *rhs, Dqn_String8EqCase eq_case)
{
    Dqn_String8 lhs_s8 = Dqn_FString8_ToString8(lhs);
    Dqn_String8 rhs_s8 = Dqn_FString8_ToString8(rhs);
    bool result        = Dqn_String8_Eq(lhs_s8, rhs_s8, eq_case);
    return result;
}

template <Dqn_usize A, Dqn_usize B> bool Dqn_FString8_EqFString8Insensitive(Dqn_FString8<A> const *lhs, Dqn_FString8<B> const *rhs)
{
    Dqn_String8 lhs_s8 = Dqn_FString8_ToString8(lhs);
    Dqn_String8 rhs_s8 = Dqn_FString8_ToString8(rhs);
    bool result = Dqn_String8_Eq(lhs_s8, rhs_s8, Dqn_String8EqCase_Insensitive);
    return result;
}
#endif // !defined(DQN_NO_FSTRING8)
