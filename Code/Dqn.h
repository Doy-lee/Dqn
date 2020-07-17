// -------------------------------------------------------------------------------------------------
//
// Preprocessor Config
//
// -------------------------------------------------------------------------------------------------
// #define DQN_IMPLEMENTATION In one and only one C++ file to enable the header file
// #define DQN_NO_ASSERT      Disable assertions
// #define DQN_STATIC_API     Apply static to all function definitions and disable external linkage to other TU's.
//
// #define DQN_ALLOCATOR_DEFAULT_TO_NULL
//
// If defined, 0 initialising an allocator uses the null allocator (i.e. crashes
// when allocating). It forces the user to specify explicitly which allocator to
// use, for example.
//
// Dqn_Allocator allocator = {};
// allocator.type = Dqn_AllocatorType::Heap;
//
// or
//
// Dqn_Allocator allocator = Dqn_AllocatorHeap()
//
// Otherwise if not defined, Dqn_Allocator allocator = {}; will by default use
// malloc, realloc, free
//
// -------------------------------------------------------------------------------------------------
//
// Library Config
//
// -------------------------------------------------------------------------------------------------
// Dqn library logs errors and outputs through Dqn_Log(...). This is
// customisable by updating the function ptr the logging routine is called
// through. Currently it is set in the global variable `dqn_log`, i.e.
//
// Dqn_LogProc *dqn_log = Dqn_Log;
//
// Set dqn_log to your own routine to override the default library logging
// routine.

#if !defined(DQN_H)
#define DQN_H

// -------------------------------------------------------------------------------------------------
//
// Compiler
//
// -------------------------------------------------------------------------------------------------
#if defined(_MSC_VER)
    #define DQN_COMPILER_MSVC
#elif defined(__clang__)
    #define DQN_COMPILER_CLANG
#elif defined(__GNUC__)
    #define DQN_COMPILER_GCC
#endif

#if defined(DQN_COMPILER_MSVC)
    #if defined(_CRT_SECURE_NO_WARNINGS)
        #define DQN_CRT_SECURE_NO_WARNINGS_PREVIOUSLY_DEFINED
    #else
        #define _CRT_SECURE_NO_WARNINGS
    #endif
#endif

#include <float.h> // FLT_MAX
#include <stdint.h> // uint/int typedefs
#include <stdarg.h> // va_list
#include <limits.h> // INT_MIN/MAX etc
#include <stddef.h> // ptrdiff_t

// -------------------------------------------------------------------------------------------------
//
// NOTE: Overridable Macros
//
// -------------------------------------------------------------------------------------------------
#if !defined(DQN_CALLOC)
    #include <stdlib.h>
    #define DQN_CALLOC(count, size) calloc(count, size)
#endif

#if !defined(DQN_MALLOC)
    #include <stdlib.h>
    #define DQN_MALLOC(size) malloc(size)
#endif

#if !defined(DQN_FREE)
    #include <stdlib.h>
    #define DQN_FREE(ptr) free(ptr)
#endif

#if !defined(DQN_MEMCOPY)
    #include <string.h>
    #define DQN_MEMCOPY(dest, src, count) memcpy(dest, src, count)
#endif

#if !defined(DQN_MEMSET)
    #include <string.h>
    #define DQN_MEMSET(dest, value, count) memset(dest, value, count)
#endif

#if !defined(DQN_MEMCMP)
    #include <string.h>
    #define DQN_MEMCMP(ptr1, ptr2, num) memcmp(ptr1, ptr2, num)
#endif

#if !defined(DQN_SQRTF)
    #include <math.h>
    #define DQN_SQRTF(val) sqrtf(val)
#endif

#if !defined(DQN_STRLEN)
    #include <string.h>
    #define DQN_STRLEN(str) strlen(str)
#endif

// -------------------------------------------------------------------------------------------------
//
// Utility Macros
//
// -------------------------------------------------------------------------------------------------
#define DQN_CAST(val) (val)
#define DQN_ABS(val) (((val) < 0) ? (-(val)) : (val))
#define DQN_SQUARED(val) ((val) * (val))
#define DQN_MIN(a, b) ((a < b) ? (a) : (b))
#define DQN_MAX(a, b) ((a > b) ? (a) : (b))
#define DQN_SWAP(a, b)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        auto tmp = a;                                                                                                  \
        a        = b;                                                                                                  \
        b        = tmp;                                                                                                \
    } while (0)

#define DQN_LEN_AND_STR(string)                    Dqn_CharCount(str), string
#define DQN_STR_AND_LEN(string)                    string, Dqn_CharCount(string)
#define DQN_STR_AND_LEN_I(string)                  string, (int)Dqn_CharCount(string)
#define DQN_FOR_EACH(i, limit)                     for (Dqn_isize i = 0; i < (Dqn_isize)(limit); ++i)
#define DQN_FOR_EACH_REVERSE(i, limit)             for (Dqn_isize i = (Dqn_isize)(limit-1); i >= 0; --i)
#define DQN_FOR_EACH_ITERATOR(it_name, array, num) for (auto it_name = array; it_name != (array + num); it_name++)

#define DQN_BYTES(val)     (val)
#define DQN_KILOBYTES(val) (1024ULL * DQN_BYTES(val))
#define DQN_MEGABYTES(val) (1024ULL * DQN_KILOBYTES(val))
#define DQN_GIGABYTES(val) (1024ULL * DQN_MEGABYTES(val))

#define DQN_MINS_TO_S(val)  ((val) * 60ULL)
#define DQN_HOURS_TO_S(val) (DQN_MINS_TO_S(val) * 60ULL)
#define DQN_DAYS_TO_S(val)  (DQN_HOURS_TO_S(val) * 24ULL)
#define DQN_YEARS_TO_S(val)  (DQN_DAYS_TO_S(val) * 365ULL)
#define DQN_ISIZEOF(val) DQN_CAST(ptrdiff_t)sizeof(val)

#ifdef DQN_COMPILER_MSVC
    #define DQN_DEBUG_BREAK __debugbreak()
#elif defined(DQN_COMPILER_CLANG) || defined(DQN_COMPILER_GCC)
    #include <signal.h>
    #define DQN_DEBUG_BREAK raise(SIGTRAP)
#endif

#define DQN_INVALID_CODE_PATH 0
#define DQN_SECONDS_TO_MS(val) ((val) * 1000.0f)
#define DQN_MATH_PI 3.14159265359f
#define DQN_DEGREE_TO_RADIAN(val) (val) * (DQN_MATH_PI / 180.0f)

#if defined(DQN_STATIC_API)
    #define DQN_API static
#else
    #define DQN_API
#endif
#define DQN_LOCAL_PERSIST static

// -------------------------------------------------------------------------------------------------
//
// Assert Macro
//
// ------------------------------------------------------------------------------------------------
#if defined(DQN_NO_ASSERT)
    #define DQN_ASSERT(expr)
    #define DQN_ASSERT_MSG(expr, fmt, ...)
    #define DQN_ASSERT_IF(expr) if ((expr))
    #define DQN_ASSERT_MSG_IF(expr, fmt, ...) if ((expr))
#else
    #define DQN_ASSERT(expr) DQN_ASSERT_MSG(expr, "")
    #define DQN_ASSERT_MSG(expr, fmt, ...)                                                                                 \
        if (!(expr))                                                                                                       \
        {                                                                                                                  \
            DQN_LOG_E("Assert: [" #expr "] " fmt, ##__VA_ARGS__);                                                          \
            DQN_DEBUG_BREAK;                                                                                               \
        }

    #define DQN_ASSERT_IF(expr) DQN_ASSERT_MSG_IF(expr, "")
    #define DQN_ASSERT_MSG_IF(expr, fmt, ...)                                                                              \
        DQN_ASSERT_MSG(!(expr), fmt, ## __VA_ARGS__) \
        if (0)
#endif

// ------------------------------------------------------------------------------------------------
//
// Typedefs
//
// ------------------------------------------------------------------------------------------------
using Dqn_uintptr = uintptr_t;
using Dqn_usize   = size_t;
using Dqn_isize   = ptrdiff_t;
using Dqn_f64     = double;
using Dqn_f32     = float;
using Dqn_i64     = int64_t;
using Dqn_i32     = int32_t;
using Dqn_i16     = int16_t;
using Dqn_i8      = int8_t;
using Dqn_uchar   = unsigned char;
using Dqn_uint    = unsigned int;
using Dqn_u64     = uint64_t;
using Dqn_u32     = uint32_t;
using Dqn_u16     = uint16_t;
using Dqn_u8      = uint8_t;
using Dqn_b32     = int32_t;

// ------------------------------------------------------------------------------------------------
//
// Constants
//
// ------------------------------------------------------------------------------------------------
const Dqn_i32 DQN_I32_MAX     = INT32_MAX;
const Dqn_u32 DQN_U32_MAX     = UINT32_MAX;
const Dqn_f32 DQN_F32_MAX     = FLT_MAX;
const Dqn_isize DQN_ISIZE_MAX = PTRDIFF_MAX;
const Dqn_usize DQN_USIZE_MAX = SIZE_MAX;

// ------------------------------------------------------------------------------------------------
//
// Compile Time Utilities
//
// ------------------------------------------------------------------------------------------------
template <typename T, Dqn_isize N>
constexpr Dqn_usize Dqn_ArrayCount(T const (&)[N])
{
    return N;
}

template <typename T, Dqn_isize N>
constexpr Dqn_isize Dqn_ArrayCountI(T const (&)[N])
{
    return N;
}

template <Dqn_isize N>
constexpr Dqn_usize Dqn_CharCount(char const (&)[N])
{
    return N - 1;
}

template <Dqn_isize N>
constexpr Dqn_isize Dqn_CharCountI(char const (&)[N])
{
    return N - 1;
}

// ------------------------------------------------------------------------------------------------
//
// Defer Macro
//
// ------------------------------------------------------------------------------------------------
template <typename Procedure>
struct DqnDefer
{
    Procedure proc;
    DqnDefer(Procedure p) : proc(p) {}
    ~DqnDefer()                     { proc(); }
};

struct DqnDeferHelper
{
    template <typename Lambda>
    DqnDefer<Lambda> operator+(Lambda lambda) { return DqnDefer<Lambda>(lambda); };
};

#define DQN_TOKEN_COMBINE2(x, y) x ## y
#define DQN_TOKEN_COMBINE(x, y) DQN_TOKEN_COMBINE2(x, y)
#define DQN_UNIQUE_NAME(prefix) DQN_TOKEN_COMBINE(prefix, __LINE__)
#define DQN_DEFER const auto DQN_UNIQUE_NAME(defer_lambda_) = DqnDeferHelper() + [&]()

// ------------------------------------------------------------------------------------------------
//
// Utility Enums
//
// ------------------------------------------------------------------------------------------------
enum struct Dqn_ZeroMem
{
    No,
    Yes
};

// -------------------------------------------------------------------------------------------------
//
// stb_sprintf
//
// -------------------------------------------------------------------------------------------------
// stb_sprintf - v1.05 - public domain snprintf() implementation
// originally by Jeff Roberts / RAD Game Tools, 2015/10/20
// http://github.com/nothings/stb
//
// allowed types:  sc uidBboXx p AaGgEef n
// lengths      :  h ll j z t I64 I32 I
//
// Contributors:
//    Fabian "ryg" Giesen (reformatting)
//
// Contributors (bugfixes):
//    github:d26435
//    github:trex78
//    Jari Komppa (SI suffixes)
//    Rohit Nirmal
//    Marcin Wojdyr
//    Leonard Ritter
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

#if defined(__has_feature)
   #if __has_feature(address_sanitizer)
      #define STBI__ASAN __attribute__((no_sanitize("address")))
   #endif
#endif
#ifndef STBI__ASAN
#define STBI__ASAN
#endif

#ifdef STB_SPRINTF_STATIC
#define STBSP__PUBLICDEC static
#define STBSP__PUBLICDEF static STBI__ASAN
#else
#ifdef __cplusplus
#define STBSP__PUBLICDEC extern "C"
#define STBSP__PUBLICDEF extern "C" STBI__ASAN
#else
#define STBSP__PUBLICDEC extern
#define STBSP__PUBLICDEF STBI__ASAN
#endif
#endif

#include <stdarg.h> // for va_list()

#ifndef STB_SPRINTF_MIN
#define STB_SPRINTF_MIN 512 // how many characters per callback
#endif
typedef char *STBSP_SPRINTFCB(char *buf, void *user, int len);

#ifndef STB_SPRINTF_DECORATE
#define STB_SPRINTF_DECORATE(name) stbsp_##name // define this before including if you want to change the names
#endif

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintf)(char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsnprintf)(char *buf, int count, char const *fmt, va_list va);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(sprintf)(char *buf, char const *fmt, ...);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...);

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEF void STB_SPRINTF_DECORATE(set_separators)(char comma, char period);
#endif // STB_SPRINTF_H_INCLUDE

// -------------------------------------------------------------------------------------------------
//
// NOTE: Logging
//
// -------------------------------------------------------------------------------------------------
#define X_MACRO                                                                                                        \
    X_ENTRY(Debug, "DBG")                                                                                              \
    X_ENTRY(Error, "ERR")                                                                                              \
    X_ENTRY(Warning, "WRN")                                                                                            \
    X_ENTRY(Info, "INF")                                                                                               \
    X_ENTRY(Memory, "MEM")

#define X_ENTRY(enum_val, string) enum_val,
enum struct Dqn_LogType { X_MACRO };
#undef X_ENTRY

#define X_ENTRY(enum_val, string) string,
char const *Dqn_LogTypeString[] = { X_MACRO };
#undef X_ENTRY
#undef X_MACRO

#define DQN_LOG_E(fmt, ...) dqn_log(Dqn_LogType::Error,   DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_D(fmt, ...) dqn_log(Dqn_LogType::Debug,   DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_W(fmt, ...) dqn_log(Dqn_LogType::Warning, DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_I(fmt, ...) dqn_log(Dqn_LogType::Info,    DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_M(fmt, ...) dqn_log(Dqn_LogType::Memory,  DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG(log_type, fmt, ...) dqn_log(log_type,     DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)

DQN_API void Dqn_LogV   (Dqn_LogType type, char const *file, Dqn_usize file_len, char const *func, Dqn_usize func_len, Dqn_usize line, char const *fmt, va_list va);
DQN_API void Dqn_Log    (Dqn_LogType type, char const *file, Dqn_usize file_len, char const *func, Dqn_usize func_len, Dqn_usize line, char const *fmt, ...);
typedef void Dqn_LogProc(Dqn_LogType type, char const *file, Dqn_usize file_len, char const *func, Dqn_usize func_len, Dqn_usize line, char const *fmt, ...);
extern Dqn_LogProc *dqn_log;

// -------------------------------------------------------------------------------------------------
//
// Dqn_Align
//
// -------------------------------------------------------------------------------------------------
// NOTE: Even if pointer is aligned, align it again, ensuring there's at minimum
// 1 byte and at most alignment bytes of space between the aligned pointer and
// raw pointer. We do this to keep metadata exactly 1 byte behind the aligned
// pointer.
DQN_API Dqn_uintptr Dqn_AlignAddressEnsuringSpace(Dqn_uintptr address, Dqn_u8 alignment);
DQN_API Dqn_uintptr Dqn_AlignAddress             (Dqn_uintptr address, Dqn_u8 alignment);

// -------------------------------------------------------------------------------------------------
//
// Dqn_PointerMetadata
//
// -------------------------------------------------------------------------------------------------
struct Dqn_PointerMetadata
{
    Dqn_u8 alignment;
    Dqn_u8 offset; // Subtract offset from aligned ptr to return to the allocation ptr
};

DQN_API Dqn_isize           Dqn_PointerMetadata_SizeRequired(Dqn_isize size, Dqn_u8 alignment);
DQN_API char               *Dqn_PointerMetadata_Init(void *ptr, Dqn_u8 alignment);
DQN_API Dqn_PointerMetadata Dqn_PointerMetadata_Get(void *ptr);
DQN_API char               *Dqn_PointerMetadata_GetRawPointer(void *ptr);

// -------------------------------------------------------------------------------------------------
//
// NOTE: Math
//
// -------------------------------------------------------------------------------------------------
union Dqn_V2I
{
  struct { Dqn_i32 x, y; };
  struct { Dqn_i32 w, h; };
  struct { Dqn_i32 min, max; };
  Dqn_i32 e[2];

  Dqn_V2I() = default;
  constexpr Dqn_V2I(Dqn_f32 x_, Dqn_f32 y_): x((Dqn_i32)x_), y((Dqn_i32)y_) {}
  constexpr Dqn_V2I(Dqn_i32 x_, Dqn_i32 y_): x(x_), y(y_) {}
  constexpr Dqn_V2I(Dqn_i32 xy):             x(xy), y(xy) {}

  constexpr bool     operator!=(Dqn_V2I other) const { return !(*this == other);                }
  constexpr bool     operator==(Dqn_V2I other) const { return (x == other.x) && (y == other.y); }
  constexpr bool     operator>=(Dqn_V2I other) const { return (x >= other.x) && (y >= other.y); }
  constexpr bool     operator<=(Dqn_V2I other) const { return (x <= other.x) && (y <= other.y); }
  constexpr bool     operator< (Dqn_V2I other) const { return (x <  other.x) && (y <  other.y); }
  constexpr bool     operator> (Dqn_V2I other) const { return (x >  other.x) && (y >  other.y); }
  constexpr Dqn_V2I  operator- (Dqn_V2I other) const { Dqn_V2I result(x - other.x, y - other.y); return result; }
  constexpr Dqn_V2I  operator+ (Dqn_V2I other) const { Dqn_V2I result(x + other.x, y + other.y); return result; }
  constexpr Dqn_V2I  operator* (Dqn_V2I other) const { Dqn_V2I result(x * other.x, y * other.y); return result; }
  constexpr Dqn_V2I  operator* (Dqn_f32 other) const { Dqn_V2I result(x * other,   y * other);   return result; }
  constexpr Dqn_V2I  operator* (Dqn_i32 other) const { Dqn_V2I result(x * other,   y * other);   return result; }
  constexpr Dqn_V2I  operator/ (Dqn_V2I other) const { Dqn_V2I result(x / other.x, y / other.y); return result; }
  constexpr Dqn_V2I  operator/ (Dqn_f32 other) const { Dqn_V2I result(x / other,   y / other);   return result; }
  constexpr Dqn_V2I  operator/ (Dqn_i32 other) const { Dqn_V2I result(x / other,   y / other);   return result; }
  constexpr Dqn_V2I &operator*=(Dqn_V2I other)       { *this = *this * other;                    return *this;  }
  constexpr Dqn_V2I &operator*=(Dqn_f32 other)       { *this = *this * other;                    return *this;  }
  constexpr Dqn_V2I &operator*=(Dqn_i32 other)       { *this = *this * other;                    return *this;  }
  constexpr Dqn_V2I &operator-=(Dqn_V2I other)       { *this = *this - other;                    return *this;  }
  constexpr Dqn_V2I &operator+=(Dqn_V2I other)       { *this = *this + other;                    return *this;  }
};

union Dqn_V2
{
  struct { Dqn_f32 x, y; };
  struct { Dqn_f32 w, h; };
  struct { Dqn_f32 min, max; };
  Dqn_f32 e[2];

  Dqn_V2() = default;
  constexpr Dqn_V2(Dqn_f32 a)         : x(a),        y(a)        {}
  constexpr Dqn_V2(Dqn_i32 a)         : x((Dqn_f32)a),   y((Dqn_f32)a)   {}
  constexpr Dqn_V2(Dqn_f32 x_, Dqn_f32 y_): x(x_),       y(y_)       {}
  constexpr Dqn_V2(Dqn_i32 x_, Dqn_i32 y_): x((Dqn_f32)x_),  y((Dqn_f32)y_)  {}
  constexpr Dqn_V2(Dqn_V2I a)         : x((Dqn_f32)a.x), y((Dqn_f32)a.y) {}

  constexpr bool    operator!=(Dqn_V2  other) const { return !(*this == other);                }
  constexpr bool    operator==(Dqn_V2  other) const { return (x == other.x) && (y == other.y); }
  constexpr bool    operator>=(Dqn_V2  other) const { return (x >= other.x) && (y >= other.y); }
  constexpr bool    operator<=(Dqn_V2  other) const { return (x <= other.x) && (y <= other.y); }
  constexpr bool    operator< (Dqn_V2  other) const { return (x <  other.x) && (y <  other.y); }
  constexpr bool    operator> (Dqn_V2  other) const { return (x >  other.x) && (y >  other.y); }
  constexpr Dqn_V2  operator- (Dqn_V2  other) const { Dqn_V2 result(x - other.x, y - other.y); return result; }
  constexpr Dqn_V2  operator+ (Dqn_V2  other) const { Dqn_V2 result(x + other.x, y + other.y); return result; }
  constexpr Dqn_V2  operator* (Dqn_V2  other) const { Dqn_V2 result(x * other.x, y * other.y); return result; }
  constexpr Dqn_V2  operator* (Dqn_f32 other) const { Dqn_V2 result(x * other,   y * other);   return result; }
  constexpr Dqn_V2  operator* (Dqn_i32 other) const { Dqn_V2 result(x * other,   y * other);   return result; }
  constexpr Dqn_V2  operator/ (Dqn_V2  other) const { Dqn_V2 result(x / other.x, y / other.y); return result; }
  constexpr Dqn_V2  operator/ (Dqn_f32 other) const { Dqn_V2 result(x / other,   y / other);   return result; }
  constexpr Dqn_V2  operator/ (Dqn_i32 other) const { Dqn_V2 result(x / other,   y / other);   return result; }
  constexpr Dqn_V2 &operator*=(Dqn_V2  other)       { *this = *this * other;               return *this;  }
  constexpr Dqn_V2 &operator*=(Dqn_f32 other)       { *this = *this * other;               return *this;  }
  constexpr Dqn_V2 &operator*=(Dqn_i32 other)       { *this = *this * other;               return *this;  }
  constexpr Dqn_V2 &operator/=(Dqn_V2  other)       { *this = *this / other;               return *this;  }
  constexpr Dqn_V2 &operator/=(Dqn_f32 other)       { *this = *this / other;               return *this;  }
  constexpr Dqn_V2 &operator/=(Dqn_i32 other)       { *this = *this / other;               return *this;  }
  constexpr Dqn_V2 &operator-=(Dqn_V2  other)       { *this = *this - other;               return *this;  }
  constexpr Dqn_V2 &operator+=(Dqn_V2  other)       { *this = *this + other;               return *this;  }
};

DQN_API Dqn_V2I Dqn_V2_ToV2I        (Dqn_V2 a);
DQN_API Dqn_V2  Dqn_V2_Min          (Dqn_V2 a, Dqn_V2 b);
DQN_API Dqn_V2  Dqn_V2_Max          (Dqn_V2 a, Dqn_V2 b);
DQN_API Dqn_V2  Dqn_V2_Abs          (Dqn_V2 a);
DQN_API Dqn_f32 Dqn_V2_Dot          (Dqn_V2 a, Dqn_V2 b);
DQN_API Dqn_f32 Dqn_V2_LengthSq     (Dqn_V2 a, Dqn_V2 b);
DQN_API Dqn_V2  Dqn_V2_Normalise    (Dqn_V2 a);
DQN_API Dqn_V2  Dqn_V2_Perpendicular(Dqn_V2 a);

union Dqn_V3
{
  struct { Dqn_f32 x, y, z; };
  struct { Dqn_f32 r, g, b; };
  Dqn_V2  xy;
  Dqn_f32 e[3];

  Dqn_V3() = default;
  constexpr Dqn_V3(Dqn_f32 a)                 : x(a),        y(a),       z(a)        {}
  constexpr Dqn_V3(Dqn_i32 a)                 : x((Dqn_f32)a),   y((Dqn_f32)a),  z((Dqn_f32)a)   {}
  constexpr Dqn_V3(Dqn_f32 x_, Dqn_f32 y_, Dqn_f32 z_): x(x_),       y(y_),      z(z_)       {}
  constexpr Dqn_V3(Dqn_i32 x_, Dqn_i32 y_, Dqn_f32 z_): x((Dqn_f32)x_),  y((Dqn_f32)y_), z((Dqn_f32)z_)  {}
  constexpr Dqn_V3(Dqn_V2  xy, Dqn_f32 z_)        : x(xy.x),     y(xy.y),    z(z_)       {}

  constexpr bool   operator!= (Dqn_V3  other) const { return !(*this == other);                }
  constexpr bool   operator== (Dqn_V3  other) const { return (x == other.x) && (y == other.y) && (z == other.z); }
  constexpr bool   operator>= (Dqn_V3  other) const { return (x >= other.x) && (y >= other.y) && (z >= other.z); }
  constexpr bool   operator<= (Dqn_V3  other) const { return (x <= other.x) && (y <= other.y) && (z <= other.z); }
  constexpr bool   operator<  (Dqn_V3  other) const { return (x <  other.x) && (y <  other.y) && (z <  other.z); }
  constexpr bool   operator>  (Dqn_V3  other) const { return (x >  other.x) && (y >  other.y) && (z >  other.z); }
  constexpr Dqn_V3 operator-  (Dqn_V3  other) const { Dqn_V3 result(x - other.x, y - other.y, z - other.z); return result; }
  constexpr Dqn_V3 operator+  (Dqn_V3  other) const { Dqn_V3 result(x + other.x, y + other.y, z + other.z); return result; }
  constexpr Dqn_V3 operator*  (Dqn_V3  other) const { Dqn_V3 result(x * other.x, y * other.y, z * other.z); return result; }
  constexpr Dqn_V3 operator*  (Dqn_f32 other) const { Dqn_V3 result(x * other,   y * other,   z * other);   return result; }
  constexpr Dqn_V3 operator*  (Dqn_i32 other) const { Dqn_V3 result(x * other,   y * other,   z * other);   return result; }
  constexpr Dqn_V3 operator/  (Dqn_V3  other) const { Dqn_V3 result(x / other.x, y / other.y, z / other.z); return result; }
  constexpr Dqn_V3 operator/  (Dqn_f32 other) const { Dqn_V3 result(x / other,   y / other,   z / other);   return result; }
  constexpr Dqn_V3 operator/  (Dqn_i32 other) const { Dqn_V3 result(x / other,   y / other,   z / other);   return result; }
  constexpr Dqn_V3 &operator*=(Dqn_V3  other)       { *this = *this * other;               return *this;  }
  constexpr Dqn_V3 &operator*=(Dqn_f32 other)       { *this = *this * other;               return *this;  }
  constexpr Dqn_V3 &operator*=(Dqn_i32 other)       { *this = *this * other;               return *this;  }
  constexpr Dqn_V3 &operator/=(Dqn_V3  other)       { *this = *this / other;               return *this;  }
  constexpr Dqn_V3 &operator/=(Dqn_f32 other)       { *this = *this / other;               return *this;  }
  constexpr Dqn_V3 &operator/=(Dqn_i32 other)       { *this = *this / other;               return *this;  }
  constexpr Dqn_V3 &operator-=(Dqn_V3  other)       { *this = *this - other;               return *this;  }
  constexpr Dqn_V3 &operator+=(Dqn_V3  other)       { *this = *this + other;               return *this;  }
};

union Dqn_V4
{
  struct { Dqn_f32 x, y, z, w; };
  struct { Dqn_f32 r, g, b, a; };
  Dqn_V3 rgb;
  Dqn_f32 e[4];

  Dqn_V4() = default;
  constexpr Dqn_V4(Dqn_f32 xyzw)                      : x(xyzw),    y(xyzw),    z(xyzw),    w(xyzw)    {}
  constexpr Dqn_V4(Dqn_f32 x_, Dqn_f32 y_, Dqn_f32 z_, Dqn_f32 w_): x(x_),      y(y_),      z(z_),      w(w_)      {}
  constexpr Dqn_V4(Dqn_i32 x_, Dqn_i32 y_, Dqn_i32 z_, Dqn_i32 w_): x((Dqn_f32)x_), y((Dqn_f32)y_), z((Dqn_f32)z_), w((Dqn_f32)w_) {}
  constexpr Dqn_V4(Dqn_V3 xyz, Dqn_f32 w_)                : x(xyz.x),  y(xyz.y),   z(xyz.z),   w(w_)       {}

  constexpr bool    operator!=(Dqn_V4  other) const { return !(*this == other);            }
  constexpr bool    operator==(Dqn_V4  other) const { return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w); }
  constexpr bool    operator>=(Dqn_V4  other) const { return (x >= other.x) && (y >= other.y) && (z >= other.z) && (w >= other.w); }
  constexpr bool    operator<=(Dqn_V4  other) const { return (x <= other.x) && (y <= other.y) && (z <= other.z) && (w <= other.w); }
  constexpr bool    operator< (Dqn_V4  other) const { return (x <  other.x) && (y <  other.y) && (z <  other.z) && (w <  other.w); }
  constexpr bool    operator> (Dqn_V4  other) const { return (x >  other.x) && (y >  other.y) && (z >  other.z) && (w >  other.w); }
  constexpr Dqn_V4  operator- (Dqn_V4  other) const { Dqn_V4 result(x - other.x, y - other.y, z - other.z, w - other.w); return result;  }
  constexpr Dqn_V4  operator+ (Dqn_V4  other) const { Dqn_V4 result(x + other.x, y + other.y, z + other.z, w + other.w); return result;  }
  constexpr Dqn_V4  operator* (Dqn_V4  other) const { Dqn_V4 result(x * other.x, y * other.y, z * other.z, w * other.w); return result;  }
  constexpr Dqn_V4  operator* (Dqn_f32 other) const { Dqn_V4 result(x * other,   y * other,   z * other,   w * other);   return result;  }
  constexpr Dqn_V4  operator* (Dqn_i32 other) const { Dqn_V4 result(x * other,   y * other,   z * other,   w * other);   return result;  }
  constexpr Dqn_V4  operator/ (Dqn_f32 other) const { Dqn_V4 result(x / other,   y / other,   z / other,   w / other);   return result;  }
  constexpr Dqn_V4 &operator*=(Dqn_V4  other)       { *this = *this * other;                                             return *this;   }
  constexpr Dqn_V4 &operator*=(Dqn_f32 other)       { *this = *this * other;                                             return *this;   }
  constexpr Dqn_V4 &operator*=(Dqn_i32 other)       { *this = *this * other;                                             return *this;   }
  constexpr Dqn_V4 &operator-=(Dqn_V4  other)       { *this = *this - other;                                             return *this;   }
  constexpr Dqn_V4 &operator+=(Dqn_V4  other)       { *this = *this + other;                                             return *this;   }
};

DQN_API Dqn_f32 Dqn_V4_Dot(Dqn_V4 const *a, Dqn_V4 const *b);

struct Dqn_Rect
{
    Dqn_V2 min, max;
    Dqn_Rect() = default;
    Dqn_Rect(Dqn_V2  min, Dqn_V2 max)  : min(min), max(max) {}
    Dqn_Rect(Dqn_V2I min, Dqn_V2I max) : min(min), max(max) {}
    Dqn_Rect(Dqn_f32 x, Dqn_f32 y, Dqn_f32 max_x, Dqn_f32 max_y) : min(x, y), max(max_x, max_y) {}
    constexpr Dqn_b32 operator==(Dqn_Rect other) const { return (min == other.min) && (max == other.max); }
};

struct Dqn_RectI32
{
    Dqn_V2I min, max;
    Dqn_RectI32() = default;
    Dqn_RectI32(Dqn_V2I min, Dqn_V2I max) : min(min), max(max) {}
};

DQN_API Dqn_Rect Dqn_Rect_InitFromPosAndSize(Dqn_V2 pos, Dqn_V2 size);
DQN_API Dqn_V2   Dqn_Rect_Center            (Dqn_Rect rect);
DQN_API Dqn_b32  Dqn_Rect_ContainsPoint     (Dqn_Rect rect, Dqn_V2 p);
DQN_API Dqn_b32  Dqn_Rect_ContainsRect      (Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_V2   Dqn_Rect_Size              (Dqn_Rect rect);
DQN_API Dqn_Rect Dqn_Rect_Move              (Dqn_Rect src, Dqn_V2 move_amount);
DQN_API Dqn_b32  Dqn_Rect_Intersects        (Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_Rect Dqn_Rect_Intersection      (Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_Rect Dqn_Rect_Union             (Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_Rect Dqn_Rect_FromRectI32       (Dqn_RectI32 a);
DQN_API Dqn_V2I  Dqn_RectI32_Size           (Dqn_RectI32 rect);

union Dqn_Mat4
{
    Dqn_f32 e[16];
    Dqn_V4  row[4];
    Dqn_f32 row_major[4][4];
    Dqn_f32 operator[](Dqn_usize i) const { return e[i]; }
};

DQN_API Dqn_Mat4 Dqn_Mat4_Identity   ();
DQN_API Dqn_Mat4 Dqn_Mat4_Scale3f    (Dqn_f32 x, Dqn_f32 y, Dqn_f32 z);
DQN_API Dqn_Mat4 Dqn_Mat4_ScaleV3    (Dqn_V3 vec);
DQN_API Dqn_Mat4 Dqn_Mat4_Translate3f(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z);
DQN_API Dqn_Mat4 Dqn_Mat4_TranslateV3(Dqn_V3 vec);
DQN_API Dqn_Mat4 operator*           (Dqn_Mat4 const &a, Dqn_Mat4 const &b);
DQN_API Dqn_V4   operator*           (Dqn_Mat4 const &mat, Dqn_V4 const &vec);

// -------------------------------------------------------------------------------------------------
//
// NOTE: Math Utils
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_V2  Dqn_LerpV2 (Dqn_V2 a, Dqn_f32 t, Dqn_V2 b);
DQN_API Dqn_f32 Dqn_LerpF32(Dqn_f32 a, Dqn_f32 t, Dqn_f32 b);

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Allocator
//
// -------------------------------------------------------------------------------------------------
enum struct Dqn_Allocator_Type
{
#if defined(DQN_ALLOCATOR_DEFAULT_TO_NULL)
    Null,
    Heap,   // Malloc, free
#else
    Heap,   // Malloc, free
    Null,
#endif

    XHeap,  // Malloc free, crash on failure
    Arena,
    Custom,
};

#define DQN_ALLOCATOR_ALLOCATE_PROC(name) void *name(Dqn_isize size, Dqn_u8 alignment, void *user_context)
#define DQN_ALLOCATOR_FREE_PROC(name) void name(void *ptr, void *user_context)
typedef DQN_ALLOCATOR_ALLOCATE_PROC(Dqn_Allocator_AllocateProc);
typedef DQN_ALLOCATOR_FREE_PROC(Dqn_Allocator_FreeProc);
struct Dqn_Allocator
{
    Dqn_Allocator_Type type;
    union
    {
        void                      *user;
        struct Dqn_ArenaAllocator *arena;
    } context;

    Dqn_isize bytes_allocated;
    Dqn_isize total_bytes_allocated;

    Dqn_isize allocations;
    Dqn_isize total_allocations;

    // NOTE: Only required if type == Dqn_Allocator_Type::Custom
    Dqn_Allocator_AllocateProc *allocate;
    Dqn_Allocator_FreeProc     *free;
};

DQN_API Dqn_Allocator Dqn_Allocator_InitWithNull ();
DQN_API Dqn_Allocator Dqn_Allocator_InitWithHeap ();
DQN_API Dqn_Allocator Dqn_Allocator_InitWithXHeap();
DQN_API Dqn_Allocator Dqn_Allocator_InitWithArena(Dqn_ArenaAllocator *arena);
DQN_API void         *Dqn_Allocator_Allocate     (Dqn_Allocator *allocator, Dqn_isize size, Dqn_u8 alignment, Dqn_ZeroMem zero_mem = Dqn_ZeroMem::Yes);
DQN_API void          Dqn_Allocator_Free         (Dqn_Allocator *allocator, void *ptr);

template <typename T>
DQN_API T *
Dqn_Allocator_AllocateType(Dqn_Allocator *allocator, Dqn_isize num, Dqn_ZeroMem zero_mem = Dqn_ZeroMem::Yes)
{
    auto *result = DQN_CAST(T *)Dqn_Allocator_Allocate(allocator, sizeof(T) * num, alignof(T), zero_mem);
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_ArenaAllocator
//
// -------------------------------------------------------------------------------------------------
struct Dqn_ArenaAllocatorBlock
{
    void         *memory;
    Dqn_isize     size;
    Dqn_isize     used;
    Dqn_ArenaAllocatorBlock *prev;
    Dqn_ArenaAllocatorBlock *next;
};

Dqn_usize const DQN_MEM_ARENA_DEFAULT_MIN_BLOCK_SIZE = DQN_KILOBYTES(4);
struct Dqn_ArenaAllocator
{
    // NOTE: Configuration (fill once after "Zero Initialisation {}")
    Dqn_isize     min_block_size;
    Dqn_Allocator allocator;

    // NOTE: Read Only
    Dqn_ArenaAllocatorBlock *curr_mem_block;
    Dqn_ArenaAllocatorBlock *top_mem_block;
    Dqn_isize                highest_used_mark;
    int                      total_allocated_mem_blocks;
    Dqn_isize                usage_before_last_reset;
    Dqn_isize                wastage_before_last_reset;
};

struct Dqn_ArenaAllocatorRegion
{
    Dqn_ArenaAllocator *arena;
    Dqn_ArenaAllocatorBlock *curr_mem_block;
    Dqn_isize                curr_mem_block_used;
    Dqn_ArenaAllocatorBlock *top_mem_block;
};

struct Dqn_ArenaAllocatorScopedRegion
{
    Dqn_ArenaAllocatorScopedRegion(Dqn_ArenaAllocator *arena);
    ~Dqn_ArenaAllocatorScopedRegion();
    Dqn_ArenaAllocatorRegion region;
};

DQN_API Dqn_ArenaAllocator             Dqn_ArenaAllocator_InitWithAllocator(Dqn_Allocator allocator, Dqn_isize size = 0);
DQN_API Dqn_ArenaAllocator             Dqn_ArenaAllocator_InitWithMemory   (void *memory, Dqn_isize size);
DQN_API void                          *Dqn_ArenaAllocator_Allocate         (Dqn_ArenaAllocator *arena, Dqn_isize size, Dqn_u8 alignment, Dqn_ZeroMem zero_mem = Dqn_ZeroMem::Yes);
DQN_API void                           Dqn_ArenaAllocator_Free             (Dqn_ArenaAllocator *arena);
DQN_API Dqn_b32                        Dqn_ArenaAllocator_Reserve          (Dqn_ArenaAllocator *arena, Dqn_isize size);
DQN_API void                           Dqn_ArenaAllocator_ResetUsage       (Dqn_ArenaAllocator *arena, Dqn_ZeroMem zero_mem);
DQN_API Dqn_ArenaAllocatorRegion       Dqn_ArenaAllocator_BeginRegion      (Dqn_ArenaAllocator *arena);
DQN_API void                           Dqn_ArenaAllocator_EndRegion        (Dqn_ArenaAllocatorRegion region);
DQN_API Dqn_ArenaAllocatorScopedRegion Dqn_ArenaAllocator_MakeScopedRegion (Dqn_ArenaAllocator *arena);

template <typename T>
T *Dqn_ArenaAllocator_AllocateType(Dqn_ArenaAllocator *arena, Dqn_isize num = 1, Dqn_ZeroMem zero_mem = Dqn_ZeroMem::Yes)
{
    auto *result = DQN_CAST(T *)Dqn_ArenaAllocator_Allocate(arena, sizeof(T) * num, alignof(T), zero_mem);
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Bit
//
// -------------------------------------------------------------------------------------------------
DQN_API void    Dqn_Bit_UnsetInplace(Dqn_u32 *flags, Dqn_u32 bitfield);
DQN_API void    Dqn_Bit_SetInplace(Dqn_u32 *flags, Dqn_u32 bitfield);
DQN_API Dqn_b32 Dqn_Bit_IsSet(Dqn_u32 bits, Dqn_u32 bits_to_set);
DQN_API Dqn_b32 Dqn_Bit_IsNotSet(Dqn_u32 bits, Dqn_u32 bits_to_check);

// -------------------------------------------------------------------------------------------------
//
// NOTE: Safe Arithmetic
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_i64   Dqn_Safe_AddI64              (Dqn_i64 a, Dqn_i64 b);
DQN_API Dqn_i64   Dqn_Safe_MulI64              (Dqn_i64 a, Dqn_i64 b);
DQN_API Dqn_u64   Dqn_Safe_AddU64              (Dqn_u64 a, Dqn_u64 b);
DQN_API Dqn_u64   Dqn_Safe_SubU64              (Dqn_u64 a, Dqn_u64 b);
DQN_API Dqn_u32   Dqn_Safe_SubU32              (Dqn_u32 a, Dqn_u32 b);
DQN_API Dqn_u64   Dqn_Safe_MulU64              (Dqn_u64 a, Dqn_u64 b);
DQN_API int       Dqn_Safe_TruncateISizeToInt  (Dqn_isize val);
DQN_API Dqn_i32   Dqn_Safe_TruncateISizeToI32  (Dqn_isize val);
DQN_API Dqn_i8    Dqn_Safe_TruncateISizeToI8   (Dqn_isize val);
DQN_API Dqn_u32   Dqn_Safe_TruncateUSizeToU32  (Dqn_u64 val);
DQN_API int       Dqn_Safe_TruncateUSizeToI32  (Dqn_usize val);
DQN_API int       Dqn_Safe_TruncateUSizeToInt  (Dqn_usize val);
DQN_API Dqn_isize Dqn_Safe_TruncateUSizeToISize(Dqn_usize val);

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Char
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_b32 Dqn_Char_IsAlpha     (char ch);
DQN_API Dqn_b32 Dqn_Char_IsDigit     (char ch);
DQN_API Dqn_b32 Dqn_Char_IsAlphaNum  (char ch);
DQN_API Dqn_b32 Dqn_Char_IsWhitespace(char ch);
DQN_API char    Dqn_Char_ToLower     (char ch);

// -------------------------------------------------------------------------------------------------
//
// NOTE: String
//
// -------------------------------------------------------------------------------------------------
#define DQN_STRING_LITERAL(string) {string, Dqn_CharCountI(string)}
struct Dqn_String
{
    union {
        // NOTE: To appease GCC, Clang can't assign C string literal to char *
        //       Only UB if you try modify a string originally declared const
        char const *str_;
        char *str;
    };

    Dqn_isize   size;
    char const *begin() const { return str; }
    char const *end  () const { return str + size; }
    char       *begin()       { return str; }
    char       *end  ()       { return str + size; }
};

DQN_API Dqn_b32    Dqn_String_Compare               (Dqn_String const lhs, Dqn_String const rhs);
DQN_API Dqn_b32    Dqn_String_CompareCaseInsensitive(Dqn_String const lhs, Dqn_String const rhs);

// allocator: (Optional) When null, the string is allocated with DQN_MALLOC, result should be freed with DQN_FREE.
DQN_API Dqn_String Dqn_String_Copy                  (Dqn_String const src, Dqn_Allocator *allocator);
DQN_API Dqn_String Dqn_String_TrimWhitespaceAround  (Dqn_String src);
DQN_API Dqn_b32    operator==                       (Dqn_String const &lhs, Dqn_String const &rhs);

// return: The allocated string. When allocation fails, str returned is nullptr, size is set to the length required NOT INCLUDING the null terminator.
//         i.e. the required buffer length for generating the string is (result.size + 1).
DQN_API Dqn_String Dqn_String_FmtV                  (Dqn_Allocator *allocator, char const *fmt, va_list va);
DQN_API Dqn_String Dqn_String_FmtF                  (Dqn_Allocator *allocator, char const *fmt, ...);

// Free a string allocated with `Dqn_String_Copy`, `Dqn_String_FmtV` `Dqn_String_FmtF`
// allocator: The same allocator specified when `Dqn_String_Copy` was called.
DQN_API void       Dqn_String_Free                  (Dqn_String *string, Dqn_Allocator *allocator);

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Str
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_b32     Dqn_Str_Equals                     (char const *a, char const *b, Dqn_isize a_len = -1, Dqn_isize b_len = -1);
DQN_API char const *Dqn_Str_FindMulti                  (char const *buf, char const *find_list[], Dqn_isize const *find_string_lens, Dqn_isize find_len, Dqn_isize *match_index, Dqn_isize buf_len = -1);
DQN_API char const *Dqn_Str_Find                       (char const *buf, char const *find, Dqn_isize buf_len = -1, Dqn_isize find_len = -1);
DQN_API Dqn_b32     Dqn_Str_Match                      (char const *src, char const *find, int find_len);
DQN_API char const *Dqn_Str_SkipToChar                 (char const *src, char ch);
DQN_API char const *Dqn_Str_SkipToNextAlphaNum         (char const *src);
DQN_API char const *Dqn_Str_SkipToNextDigit            (char const *src);
DQN_API char const *Dqn_Str_SkipToNextChar             (char const *src);
DQN_API char const *Dqn_Str_SkipToNextWord             (char const *src);
DQN_API char const *Dqn_Str_SkipToNextWhitespace       (char const *src);
DQN_API char const *Dqn_Str_SkipWhitespace             (char const *src);
DQN_API char const *Dqn_Str_SkipToCharInPlace          (char const **src, char ch);
DQN_API char const *Dqn_Str_SkipToNextAlphaNumInPlace  (char const **src);
DQN_API char const *Dqn_Str_SkipToNextCharInPlace      (char const **src);
DQN_API char const *Dqn_Str_SkipToNextWhitespaceInPlace(char const **src);
DQN_API char const *Dqn_Str_SkipToNextWordInPlace      (char const **src);
DQN_API char const *Dqn_Str_SkipWhitespaceInPlace      (char const **src);
DQN_API Dqn_u64     Dqn_Str_ToU64                      (char const *buf, int len = -1);
DQN_API Dqn_i64     Dqn_Str_ToI64                      (char const *buf, int len = -1);

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_File
//
// -------------------------------------------------------------------------------------------------
// file_size: (Optional) The size of the file in bytes, the allocated buffer is (file_size + 1 [null terminator]) in bytes.
// allocator: (Optional) When null, the buffer is allocated with DQN_MALLOC, result should be freed with DQN_FREE.
// return: nullptr if allocation failed.
DQN_API char   *Dqn_File_ReadEntireFile (char const *file, Dqn_isize *file_size, Dqn_Allocator *allocator);
DQN_API Dqn_b32 Dqn_File_WriteEntireFile(char const *file, char const *buffer, Dqn_isize buffer_size);

// -------------------------------------------------------------------------------------------------
//
// NOTE: Utiltiies
//
// -------------------------------------------------------------------------------------------------
struct Dqn_U64Str
{
    // Points to the start of the str in the buffer, not necessarily buf since
    // we write into the buffer in reverse
    char *str;
    char  buf[27]; // NOTE(doyle): 27 is the maximum size of Dqn_u64 including commas
    int   len;
};

DQN_API char *Dqn_EpochTimeToDate(Dqn_i64 timestamp, char *buf, Dqn_isize buf_len);
DQN_API char *Dqn_U64ToStr       (Dqn_u64 val, Dqn_U64Str *result, Dqn_b32 comma_sep);

// -------------------------------------------------------------------------------------------------
//
// NOTE: String Builder
//
// -------------------------------------------------------------------------------------------------
struct Dqn_StringBuilderBlock
{
    char                   *mem;
    Dqn_isize               size;
    Dqn_isize               used;
    Dqn_StringBuilderBlock *next;
};

Dqn_isize constexpr DQN_STRING_BUILDER_MIN_BLOCK_SIZE = DQN_KILOBYTES(4);
template <Dqn_isize N = DQN_KILOBYTES(16)>
struct Dqn_StringBuilder
{
    Dqn_Allocator           allocator;
    char                    fixed_mem[N];
    Dqn_StringBuilderBlock  fixed_mem_block;
    Dqn_StringBuilderBlock *last_mem_block;
};

template <Dqn_isize N>
void Dqn_StringBuilder__LazyInitialise(Dqn_StringBuilder<N> *builder)
{
    builder->fixed_mem_block.mem  = builder->fixed_mem;
    builder->fixed_mem_block.size = Dqn_ArrayCount(builder->fixed_mem);
    builder->fixed_mem_block.used = 0;
    builder->fixed_mem_block.next = nullptr;
    builder->last_mem_block       = &builder->fixed_mem_block;
}

// size_required: The length of the string not including the null terminator.
template <Dqn_isize N>
DQN_API char *Dqn_StringBuilder_AllocateWriteBuffer(Dqn_StringBuilder<N> *builder, Dqn_isize size_required)
{
    if (!builder->fixed_mem_block.mem)
    {
        DQN_ASSERT(!builder->last_mem_block);
        Dqn_StringBuilder__LazyInitialise(builder);
    }

    Dqn_StringBuilderBlock *block = builder->last_mem_block;
    Dqn_b32 new_block_needed = (block->size - block->used) < size_required;
    if (new_block_needed)
    {
        Dqn_isize allocation_size = DQN_MAX(size_required, DQN_STRING_BUILDER_MIN_BLOCK_SIZE);
        block                     = Dqn_Allocator_AllocateType<Dqn_StringBuilderBlock>(&builder->allocator, 1);
        if (!block) return nullptr;

        *block                        = {};
        block->mem                    = DQN_CAST(char *)Dqn_Allocator_Allocate(&builder->allocator, allocation_size, alignof(char), Dqn_ZeroMem::No);
        block->size                   = allocation_size;
        builder->last_mem_block->next = block;
        builder->last_mem_block       = builder->last_mem_block->next;
    }

    char *result = block->mem + block->used;
    block->used += size_required;
    return result;
}

// The necessary length to build the string, it returns the length not including the null-terminator
template <Dqn_isize N> Dqn_isize Dqn_StringBuilder_BuildLength(Dqn_StringBuilder<N> const *builder)
{
    Dqn_isize result = 0;
    for (Dqn_StringBuilderBlock const *block = &builder->fixed_mem_block;
         block;
         block = block->next)
    {
        result += block->used;
    }

    return result;
}

template <Dqn_isize N>
void Dqn_StringBuilder_BuildToDest(Dqn_StringBuilder<N> const *builder, char *dest, Dqn_usize dest_size)
{
    if (!dest) return;
    if (dest_size == 1) { dest[0] = 0; return; }

    char *ptr                 = dest;
    char const *end           = dest + dest_size;
    Dqn_isize remaining_space = end - ptr;
    for (Dqn_StringBuilderBlock const *block = &builder->fixed_mem_block;
         block && remaining_space > 0;
         block = block->next, remaining_space = end - ptr)
    {
        Dqn_isize num_bytes     = block->used;
        Dqn_isize bytes_to_copy = DQN_MIN(num_bytes, remaining_space);
        DQN_MEMCOPY(ptr, block->mem, bytes_to_copy);
        ptr += bytes_to_copy;
    }

    if (remaining_space > 0) ptr[ 0] = 0; // Append null terminator
    else                     ptr[-1] = 0; // Oops ran out of space. Terminate the output prematurely.
}

template <Dqn_isize N>
char *Dqn_StringBuilder_Build(Dqn_StringBuilder<N> *builder, Dqn_Allocator *allocator, Dqn_isize *len = nullptr)
{
    Dqn_isize len_ = 0;
    if (!len) len = &len_;
    *len = Dqn_StringBuilder_BuildLength(builder);
    auto *result  = DQN_CAST(char *)Dqn_Allocator_Allocate(allocator, *len + 1, alignof(char), Dqn_ZeroMem::No);
    Dqn_StringBuilder_BuildToDest(builder, result, *len + 1);
    return result;
}

template <Dqn_isize N>
Dqn_String Dqn_StringBuilder_BuildString(Dqn_StringBuilder<N> *builder, Dqn_Allocator *allocator)
{
    Dqn_String result = {};
    result.str        = Dqn_StringBuilder_Build(builder, allocator, &result.size);
    return result;
}

template <Dqn_isize N>
void Dqn_StringBuilder_AppendFmtV(Dqn_StringBuilder<N> *builder, char const *fmt, va_list va)
{
    if (!fmt) return;
    va_list va2;
    va_copy(va2, va);
    Dqn_isize len     = stbsp_vsnprintf(nullptr, 0, fmt, va);
    char *buf         = Dqn_StringBuilder_AllocateWriteBuffer(builder, len + 1);
    stbsp_vsnprintf(buf, static_cast<int>(len + 1), fmt, va2);
    va_end(va2);

    DQN_ASSERT(builder->last_mem_block->used >= 0);
    builder->last_mem_block->used--; // stbsp_vsnprintf null terminates, back out the null-terminator from the mem block
}

template <Dqn_isize N>
void Dqn_StringBuilder_AppendFmtF(Dqn_StringBuilder<N> *builder, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Dqn_StringBuilder_AppendFmtV(builder, fmt, va);
    va_end(va);
}

template <Dqn_isize N>
void Dqn_StringBuilder_Append(Dqn_StringBuilder<N> *builder, char const *str, Dqn_isize len = -1)
{
    if (!str) return;
    if (len == -1) len = DQN_CAST(Dqn_isize)strlen(str);
    if (len == 0) return;
    char *buf = Dqn_StringBuilder_AllocateWriteBuffer(builder, len);
    DQN_MEMCOPY(buf, str, len);
}

template <Dqn_isize N>
void Dqn_StringBuilder_AppendString(Dqn_StringBuilder<N> *builder, Dqn_String const string)
{
    if (!string.str || string.len == 0) return;
    char *buf = Dqn_StringBuilder_AllocateWriteBuffer(builder, string.len);
    DQN_MEMCOPY(buf, string.str, string.len);
}

template <Dqn_isize N>
void Dqn_StringBuilder_AppendChar(Dqn_StringBuilder<N> *builder, char ch)
{
    char *buf = Dqn_StringBuilder_AllocateWriteBuffer(builder, 1);
    *buf++    = ch;
}

template <Dqn_isize N>
void Dqn_StringBuilder_Free(Dqn_StringBuilder<N> *builder)
{
    for (Dqn_StringBuilderBlock *block = builder->fixed_mem_block.next;
         block;
         block = block->next)
    {
        Dqn_StringBuilderBlock *block_to_free = block;
        Dqn_Allocator_Free(&builder->allocator, block_to_free->mem);
        Dqn_Allocator_Free(&builder->allocator, block_to_free);
    }
    Dqn_StringBuilder__LazyInitialise(builder);
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Slices
//
// -------------------------------------------------------------------------------------------------
template <typename T>
struct Dqn_Slice
{
    T        *data;
    Dqn_isize size;

    T const &operator[] (Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i < size, "%d >= 0 && %d < %d", i, size); return  data[i]; }
    T       &operator[] (Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i < size, "%d >= 0 && %d < %d", i, size); return  data[i]; }
    T const *begin      ()            const { return data; }
    T const *end        ()            const { return data + size; }
    T       *begin      ()                  { return data; }
    T       *end        ()                  { return data + size; }
    T const *operator+  (Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i < size, "%d >= 0 && %d < %d", i, size); return data + i; }
    T       *operator+  (Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i < size, "%d >= 0 && %d < %d", i, size); return data + i; }
};

template <typename T>
Dqn_b32 operator==(Dqn_Slice<T> const &lhs, Dqn_Slice<T> const &rhs)
{
    Dqn_b32 result = lhs.size == rhs.size && lhs.data == rhs.data;
    return result;
}

template <typename T, Dqn_isize N>
inline Dqn_Slice<T> Dqn_Slice_InitWithArray(T (&array)[N])
{
    Dqn_Slice<T> result = {};
    result.size         = N;
    result.data         = array;
    return result;
}

template <typename T> inline Dqn_Slice<T> Dqn_Slice_Allocate(Dqn_Allocator *allocator, Dqn_isize size, Dqn_ZeroMem zero_mem = Dqn_ZeroMem::Yes)
{
    Dqn_Slice<T> result = {};
    result.size         = size;
    result.data         = DQN_CAST(T *) Dqn_Allocator_Allocate(allocator, (sizeof(T) * size), alignof(T), zero_mem);
    return result;
}

template <typename T> inline Dqn_Slice<T> Dqn_Slice_CopyNullTerminated(Dqn_Allocator *allocator, T const *src, Dqn_isize size)
{
    Dqn_Slice<T> result = {};
    result.size         = size;
    result.data = DQN_CAST(T *) Dqn_Allocator_Allocate(allocator, (sizeof(T) * size) + 1, alignof(T), Dqn_ZeroMem::No);
    DQN_MEMCOPY(result.data, src, size * sizeof(T));
    result.buf[size] = 0;
    return result;
}

template <typename T> inline Dqn_Slice<T> Dqn_Slice_CopyNullTerminated(Dqn_Allocator *allocator, Dqn_Slice<T> const src)
{
    Dqn_Slice<T> result = Dqn_Slice_CopyNullTerminated(allocator, src.data, src.size);
    return result;
}

template <typename T> inline Dqn_Slice<T> Dqn_Slice_Copy(Dqn_Allocator *allocator, T const *src, Dqn_isize size)
{
    Dqn_Slice<T> result = {};
    result.size         = size;
    result.data = DQN_CAST(T *) Dqn_Allocator_Allocate(allocator, sizeof(T) * size, alignof(T), Dqn_ZeroMem::No);
    DQN_MEMCOPY(result.dat, src, size * sizeof(T));
    return result;
}

template <typename T> inline Dqn_Slice<T> Dqn_Slice_Copy(Dqn_Allocator *allocator, Dqn_Slice<T> const src)
{
    Dqn_Slice<T> result = Dqn_Slice_Copy(allocator, src.data, src.size);
    return result;
}

template <typename T> inline bool Dqn_Slice_Equals(Dqn_Slice<T> const a, Dqn_Slice<T> const b)
{
    bool result = false;
    if (a.size != b.size) return result;
    result = (memcmp(a.data, b.data, a.size) == 0);
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_FixedArray
//
// -------------------------------------------------------------------------------------------------

template <typename T> void Dqn__EraseStableFromCArray(T *array, Dqn_isize size, Dqn_isize max, Dqn_isize index)
{
    DQN_ASSERT(index >= 0 && index < size);
    DQN_ASSERT(size <= max); (void)max;
    Dqn_isize next_index    = DQN_MIN(index + 1, size);
    Dqn_usize bytes_to_copy = (size - next_index) * sizeof(T);
    memmove(array + index, array + next_index, bytes_to_copy);
}

#define DQN_FIXED_ARRAY_TEMPLATE template <typename T, int MAX_>
#define DQN_FIXED_ARRAY_TEMPLATE_DECL Dqn_FixedArray<T, MAX_>
DQN_FIXED_ARRAY_TEMPLATE struct Dqn_FixedArray
{
    T               data[MAX_];
    Dqn_isize       size;

    T       &operator[] (Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i <= size, "%jd >= 0 && %jd < %jd", i, size); return  data[i]; }
    T       *begin      ()                  { return data; }
    T       *end        ()                  { return data + size; }
    T       *operator+  (Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i <= size, "%jd >= 0 && %jd < %jd", i, size); return data + i; }

    T const &operator[] (Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i <= size, "%jd >= 0 && %jd < %jd", i, i, size); return  data[i]; }
    T const *begin      ()            const { return data; }
    T const *end        ()            const { return data + size; }
    T const *operator+  (Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i <= size, "%jd >= 0 && %jd < %jd", i, size); return data + i; }
};

DQN_FIXED_ARRAY_TEMPLATE
Dqn_isize Dqn_FixedArray_Max(DQN_FIXED_ARRAY_TEMPLATE_DECL const *)
{
    Dqn_isize result = MAX_;
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
DQN_FIXED_ARRAY_TEMPLATE_DECL Dqn_FixedArray_Init(T const *item, int num)
{
    DQN_FIXED_ARRAY_TEMPLATE_DECL result = {};
    Dqn_FixedArray_Add(&result, item, num);
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
T * Dqn_FixedArray_Add(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T const *items, Dqn_isize num)
{
    DQN_ASSERT(a->size + num <= MAX_);
    T *result = static_cast<T *>(DQN_MEMCOPY(a->data + a->size, items, sizeof(T) * num));
    a->size += num;
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
T * Dqn_FixedArray_Add(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T const item)
{
    DQN_ASSERT(a->size < MAX_);
    a->data[a->size++] = item;
    return &a->data[a->size - 1];
}

DQN_FIXED_ARRAY_TEMPLATE
T * Dqn_FixedArray_Make(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize num)
{
    DQN_ASSERT(a->size + num <= MAX_);
    T *result = a->data + a->size;
    a->size += num;
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
void Dqn_FixedArray_Clear(DQN_FIXED_ARRAY_TEMPLATE_DECL *a)
{
    a->size = 0;
}

DQN_FIXED_ARRAY_TEMPLATE
void Dqn_FixedArray_EraseStable(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize index)
{
    Dqn__EraseStableFromCArray<T>(a->data, a->size--, MAX_, index);
}

DQN_FIXED_ARRAY_TEMPLATE
void Dqn_FixedArray_EraseUnstable(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize index)
{
    DQN_ASSERT(index >= 0 && index < a->size);
    if (--a->size == 0) return;
    a->data[index] = a->data[a->size];
}


DQN_FIXED_ARRAY_TEMPLATE
void Dqn_FixedArray_Pop(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize num = 1)
{
    DQN_ASSERT(a->size - num >= 0);
    a->size -= num;
}

DQN_FIXED_ARRAY_TEMPLATE
T * Dqn_FixedArray_Peek(DQN_FIXED_ARRAY_TEMPLATE_DECL *a)
{
    T *result = (a->size == 0) ? nullptr : a->data + (a->size - 1);
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
T Dqn_FixedArray_PeekCopy(DQN_FIXED_ARRAY_TEMPLATE_DECL const *a)
{
    DQN_ASSERT(a->size > 0);
    T const *result = a->data + (a->size - 1);
    return *result;
}

DQN_FIXED_ARRAY_TEMPLATE
Dqn_isize Dqn_FixedArray_GetIndex(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T const *entry)
{
    Dqn_isize result = a->end() - entry;
    return result;
}

template <typename T, int MAX_, typename EqualityProc>
T * Dqn_FixedArray_Find(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, EqualityProc IsEqual)
{
    for (T &entry : (*a))
    {
        if (IsEqual(entry))
            return &entry;
    }
    return nullptr;
}

// return: True if the entry was found, false if not- the entry is made using Dqn_FixedArray_Make() in this case
template <typename T, int MAX_, typename EqualityProc>
Dqn_b32 Dqn_FixedArray_FindElseMake(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T **entry, EqualityProc IsEqual)
{
    Dqn_b32 result = true;
    T *search  = Dqn_FixedArray_Find(a, IsEqual);
    if (!search)
    {
        result = false;
        search = Dqn_FixedArray_Make(a, 1);
    }

    *entry = search;
    return result;
}

DQN_FIXED_ARRAY_TEMPLATE
T *Dqn_FixedArray_Find(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T *find)
{
    for (T &entry : (*a))
    {
        if (*find == entry)
            return entry;
    }
    return nullptr;
}

DQN_FIXED_ARRAY_TEMPLATE
Dqn_Slice<T> Dqn_FixedArray_Slice(DQN_FIXED_ARRAY_TEMPLATE_DECL *a)
{
    Dqn_Slice<T> result = {a->data, a->size};
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Array
//
// -------------------------------------------------------------------------------------------------
template <typename T> struct Dqn_Array
{
    Dqn_Allocator allocator;
    T            *data;
    Dqn_isize     size;
    Dqn_isize     max;

    T const    operator[](Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i < size, "%d >= 0 && %d < %d", i, size); return  data[i]; }
    T          operator[](Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i < size, "%d >= 0 && %d < %d", i, size); return  data[i]; }
    T const   *begin    ()             const { return data; }
    T const   *end      ()             const { return data + size; }
    T         *begin    ()                   { return data; }
    T         *end      ()                   { return data + size; }
    T const   *operator+(Dqn_isize i)  const { DQN_ASSERT_MSG(i >= 0 && i < size, "%d >= 0 && %d < %d", i, size); return data + i; }
    T         *operator+(Dqn_isize i)        { DQN_ASSERT_MSG(i >= 0 && i < size, "%d >= 0 && %d < %d", i, size); return data + i; }
};

template <typename T> Dqn_Array<T> Dqn_Array_InitWithMemory(T *memory, Dqn_isize max, Dqn_isize size = 0)
{
    Dqn_Array<T> result = {};
    result.allocator    = Dqn_Allocator_InitWithNull();
    result.data         = memory;
    result.size         = size;
    result.max          = max;
    return result;
}

template <typename T> Dqn_Array<T> Dqn_Array_InitWithAllocatorNoGrow(Dqn_Allocator *allocator, Dqn_isize max, Dqn_isize size = 0, Dqn_ZeroMem zero_mem = Dqn_ZeroMem::Yes)
{
    T *memory           = DQN_CAST(T *)Dqn_Allocator_Allocate(allocator, sizeof(T) * max, alignof(T), zero_mem);
    Dqn_Array<T> result = Dqn_Array_InitWithMemory(memory, max, size);
    return result;
}


template <typename T> bool Dqn_Array_Reserve(Dqn_Array<T> *a, Dqn_isize size)
{
    if (size <= a->size) return true;
    T *new_ptr = DQN_CAST(T *)Dqn_Allocator_Allocate(&a->allocator, sizeof(T) * size, alignof(T));
    if (!new_ptr) return false;

    if (a->data) 
    {
        // NOTE(doyle): Realloc, I don't like and don't support. Use virtual arrays
        DQN_MEMCOPY(new_ptr, a->data, a->size * sizeof(T));
        Dqn_Allocator_Free(&a->allocator, a->data);
    }

    a->data = new_ptr;
    a->max  = size;
    return true;
}

template <typename T> void Dqn_Array_Free(Dqn_Array<T> *a)
{
    Dqn_Allocator_Free(&a->allocator, a->data);
}

template <typename T> bool Dqn_Array__GrowIfNeeded(Dqn_Array<T> *a, Dqn_isize num_to_add)
{
    Dqn_isize new_size = a->size + num_to_add;
    bool result        = true;
    if (new_size > a->max)
    {
        Dqn_isize num_items = DQN_MAX(4, DQN_MAX(new_size, (a->max * 2)));
        result              = Dqn_Array_Reserve(a, num_items);
    }

    return result;
}

template <typename T> T * Dqn_Array_Add(Dqn_Array<T> *a, T const *items, Dqn_isize num)
{
    if (!Dqn_Array__GrowIfNeeded(a, num))
        return nullptr;
    T *result = static_cast<T *>(DQN_MEMCOPY(a->data + a->size, items, sizeof(T) * num));
    a->size += num;
    return result;
}

template <typename T> T * Dqn_Array_Add(Dqn_Array<T> *a, T const item)
{
    if (!Dqn_Array__GrowIfNeeded(a, 1))
        return nullptr;
    a->data[a->size++] = item;
    return &a->data[a->size - 1];
}
template <typename T> T * Dqn_Array_Make(Dqn_Array<T> *a, Dqn_isize num)
{
    if (!Dqn_Array__GrowIfNeeded(a, num))
        return nullptr;
    T *result = a->data + a->size;
    a->size += num;
    return result;
}

template <typename T> void Dqn_Array_Clear(Dqn_Array<T> *a, bool zero_mem = false)
{
    a->size = 0;
    if (zero_mem) DQN_MEMSET(a->data, 0, sizeof(T) * a->max);
}

template <typename T> void Dqn_Array_EraseStable(Dqn_Array<T> *a, Dqn_isize index)
{
    Dqn__EraseStableFromCArray<T>(a->data, a->size--, a->max, index);
}

template <typename T> void Dqn_Array_EraseUnstable(Dqn_Array<T> *a, Dqn_isize index)
{
    DQN_ASSERT(index >= 0 && index < a->size);
    if (--a->size == 0) return;
    a->data[index] = a->data[a->size];
}

template <typename T> void Dqn_Array_Pop(Dqn_Array<T> *a, Dqn_isize num)
{
    DQN_ASSERT(a->size - num >= 0);
    a->size -= num;
}

template <typename T> T * Dqn_Array_Peek(Dqn_Array<T> *a)
{
    T *result = (a->size == 0) ? nullptr : a->data + (a->size - 1);
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_FixedString
//
// -------------------------------------------------------------------------------------------------
template <Dqn_isize MAX_>
struct Dqn_FixedString
{
    union { char data[MAX_]; char str[MAX_]; char buf[MAX_]; };
    Dqn_isize size;
    Dqn_isize max = MAX_;

    Dqn_FixedString() { data[0] = 0; size = 0; }
    Dqn_FixedString(char const *fmt, ...)
    {
        *this = {};
        va_list va;
        va_start(va, fmt);
        Dqn_FixedString_AppendFmtV(this, fmt, va);
        va_end(va);
    }

    Dqn_b32 operator==(Dqn_FixedString const &other) const
    {
        if (size != other.size) return false;
        bool result = memcmp(data, other.data, size);
        return result;
    }

    Dqn_b32 operator!=(Dqn_FixedString const &other) const { return !(*this == other); }

    char const &operator[]   (Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i < size, "%d >= 0 && %d < %d", i, size); return data[i]; }
    char       &operator[]   (Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i < size, "%d >= 0 && %d < %d", i, size); return data[i]; }
    char const *begin        ()        const { return data; }
    char const *end          ()        const { return data + size; }
    char       *begin        ()              { return data; }
    char       *end          ()              { return data + size; }
};

template <Dqn_isize MAX_> int Dqn_FixedString_Capacity(Dqn_FixedString<MAX_> *)
{
    int result = MAX_;
    return result;
}

template <Dqn_isize MAX_> void Dqn_FixedString_Clear(Dqn_FixedString<MAX_> *str) { *str = {}; }
template <Dqn_isize MAX_> Dqn_b32 Dqn_FixedString_AppendFmtV(Dqn_FixedString<MAX_> *str, char const *fmt, va_list va)
{
    va_list va2;
    va_copy(va2, va);
    Dqn_isize require = stbsp_vsnprintf(nullptr, 0, fmt, va) + 1;
    Dqn_isize space   = MAX_ - str->size;
    Dqn_b32 result    = require <= space;
    DQN_ASSERT(require <= space);
    str->size += stbsp_vsnprintf(str->data + str->size, static_cast<int>(space), fmt, va2);
    va_end(va2);
    return result;
}

template <Dqn_isize MAX_> Dqn_b32 Dqn_FixedString_AppendFmtF(Dqn_FixedString<MAX_> *str, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Dqn_b32 result = Dqn_FixedString_AppendFmtV(str, fmt, va);
    va_end(va);
    return result;
}

template <Dqn_isize MAX_> Dqn_b32 Dqn_FixedString_Append(Dqn_FixedString<MAX_> *str, char const *src, Dqn_isize size = -1)
{
    if (size == -1) size = DQN_CAST(Dqn_isize)DQN_STRLEN(src);
    Dqn_isize space = MAX_ - str->size;

    Dqn_b32 result = true;
    DQN_ASSERT_MSG_IF(size >= space, "size: %jd, space: %jd", size, space)
        return false;

    DQN_MEMCOPY(str->data + str->size, src, size);
    str->size += size;
    str->str[str->size] = 0;
    return result;
}

template <Dqn_isize MAX_> Dqn_String Dqn_FixedString_ToString(Dqn_FixedString<MAX_> const *str)
{
    Dqn_String result = { str->str, str->size };
    return result;
}
#endif // DQN_H

// -------------------------------------------------------------------------------------------------
//
// Implementation
//
// -------------------------------------------------------------------------------------------------
#if defined(DQN_IMPLEMENTATION)
#define STB_SPRINTF_IMPLEMENTATION
#include <stdio.h> // fprintf, FILE, stdout, stderr
#include <time.h>  // Dqn_EpochTimeToDate

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Log
//
// -------------------------------------------------------------------------------------------------
DQN_API void Dqn_LogV(Dqn_LogType type, char const *file, Dqn_usize file_len, char const *func, Dqn_usize func_len, Dqn_usize line, char const *fmt, va_list va)
{
    char const *file_ptr = file;
    auto file_ptr_len    = DQN_CAST(Dqn_isize)file_len;
    for (Dqn_isize i = (file_ptr_len - 1); i >= 0; --i)
    {
        if (file_ptr[i] == '\\' || file_ptr[i] == '/')
        {
            char const *file_end = file_ptr + file_ptr_len;
            file_ptr             = file_ptr + (i + 1);
            file_ptr_len         = static_cast<int>(file_end - file_ptr);
            break;
        }
    }

    FILE *handle = (type == Dqn_LogType::Error) ? stderr : stdout;
    fprintf(handle,
            "%s %.*s %05zu %.*s ",
            Dqn_LogTypeString[DQN_CAST(int) type],
            (int)file_ptr_len,
            file_ptr,
            line,
            (int)func_len,
            func
            );

    vfprintf(handle, fmt, va);
    fputc('\n', handle);
}

DQN_API void Dqn_Log(Dqn_LogType type, char const *file, Dqn_usize file_len, char const *func, Dqn_usize func_len, Dqn_usize line, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Dqn_LogV(type, file, file_len, func, func_len, line, fmt, va);
    va_end(va);
}

Dqn_LogProc *dqn_log = Dqn_Log;

// -------------------------------------------------------------------------------------------------
//
// Dqn_Align*
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_uintptr Dqn_AlignAddressEnsuringSpace(Dqn_uintptr address, Dqn_u8 alignment)
{
    Dqn_uintptr remainder       = address % alignment;
    Dqn_uintptr offset_to_align = alignment - remainder;
    Dqn_uintptr result          = address + offset_to_align;
    DQN_ASSERT(result % alignment == 0);
    DQN_ASSERT(result >= address);
    DQN_ASSERT(offset_to_align >= 1 && offset_to_align <= alignment);
    return result;
}

DQN_API Dqn_uintptr Dqn_AlignAddress(Dqn_uintptr address, Dqn_u8 alignment)
{
    Dqn_uintptr result = address;
    if (alignment > 0)
    {
        Dqn_uintptr remainder = result % alignment;
        if (remainder > 0)
        {
            Dqn_uintptr offset_to_align = alignment - remainder;
            result += offset_to_align;
        }
    }
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// Dqn_PointerMetadata
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_isize Dqn_PointerMetadata_SizeRequired(Dqn_isize size, Dqn_u8 alignment)
{
    DQN_ASSERT(alignment > 0);
    if (alignment <= 0) alignment = 1;
    Dqn_isize result = size + DQN_CAST(Dqn_i8)(alignment - 1) + DQN_CAST(Dqn_isize)sizeof(Dqn_PointerMetadata);
    return result;
}

DQN_API char *Dqn_PointerMetadata_Init(void *ptr, Dqn_u8 alignment)
{
    DQN_ASSERT_MSG(alignment == 1 || (alignment & 1) == 0, "Alignment must be a power of 2, %u", alignment);

    // NOTE: Given a pointer, it can misaligned by up to (Alignment - 1) bytes.
    // After calculating the offset to apply on the aligned ptr, we store the
    // allocation metadata right before the ptr for convenience, so that we just
    // need to walk back sizeof(metadata) bytes to get to the metadata from the
    // pointer.

    //                                                         [Metadata Is Stored Here]
    // [Raw Pointer] -> [Metadata Storage] [Unaligned Pointer] [Offset to Align Pointer] [Aligned Pointer]

    // NOTE: In the scenario where the pointer is already aligned after the
    // [Metadata Storage] bytes, we use the bytes allocated for the metadata
    // storage, instead of storing it into the offset storage bytes.

    //                  [Metadata Is Stored Here]
    // [Raw Pointer] -> [Metadata Storage]        [Aligned Pointer]

    // Offset is [0->Alignment-1] bytes from the Unaligned ptr.
    auto raw_ptr       = DQN_CAST(uintptr_t) ptr;
    auto unaligned_ptr = raw_ptr + sizeof(Dqn_PointerMetadata);
    auto result        = DQN_CAST(uintptr_t) unaligned_ptr;

    if ((unaligned_ptr % alignment) > 0)
    {
        uintptr_t unaligned_to_aligned_offset = alignment - (unaligned_ptr % alignment);
        result += unaligned_to_aligned_offset;
    }
    DQN_ASSERT(result % alignment == 0);
    DQN_ASSERT(result >= raw_ptr);
    ptrdiff_t difference = DQN_CAST(ptrdiff_t)result - DQN_CAST(ptrdiff_t)raw_ptr;
    DQN_ASSERT(difference <= DQN_CAST(Dqn_u8)-1);

    auto *metadata_ptr      = DQN_CAST(Dqn_PointerMetadata *)(result - sizeof(Dqn_PointerMetadata));
    metadata_ptr->alignment = alignment;
    metadata_ptr->offset    = DQN_CAST(Dqn_u8)difference;
    return DQN_CAST(char *)result;
}

DQN_API Dqn_PointerMetadata Dqn_PointerMetadata_Get(void *ptr)
{
    auto *aligned_ptr = DQN_CAST(char *) ptr;
    auto result       = *DQN_CAST(Dqn_PointerMetadata *)(aligned_ptr - sizeof(Dqn_PointerMetadata));
    return result;
}

DQN_API char *Dqn_PointerMetadata_GetRawPointer(void *ptr)
{
    Dqn_PointerMetadata metadata = Dqn_PointerMetadata_Get(ptr);
    char *result                   = DQN_CAST(char *) ptr - metadata.offset;
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_V2
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_V2I Dqn_V2_ToV2I(Dqn_V2 a)
{
    Dqn_V2I result(static_cast<Dqn_i32>(a.x), static_cast<Dqn_i32>(a.y));
    return result;
}

DQN_API Dqn_V2 Dqn_V2_Min(Dqn_V2 a, Dqn_V2 b)
{
    Dqn_V2 result = Dqn_V2(DQN_MIN(a.x, b.x), DQN_MIN(a.y, b.y));
    return result;
}

DQN_API Dqn_V2 Dqn_V2_Max(Dqn_V2 a, Dqn_V2 b)
{
    Dqn_V2 result = Dqn_V2(DQN_MAX(a.x, b.x), DQN_MAX(a.y, b.y));
    return result;
}

DQN_API Dqn_V2 Dqn_V2_Abs(Dqn_V2 a)
{
    Dqn_V2 result = Dqn_V2(DQN_ABS(a.x), DQN_ABS(a.y));
    return result;
}

DQN_API Dqn_f32 Dqn_V2_Dot(Dqn_V2 a, Dqn_V2 b)
{
    Dqn_f32 result = (a.x * b.x) + (a.y * b.y);
    return result;
}

DQN_API Dqn_f32 Dqn_V2_LengthSq(Dqn_V2 a, Dqn_V2 b)
{
    Dqn_f32 x_side = b.x - a.x;
    Dqn_f32 y_side = b.y - a.y;
    Dqn_f32 result = DQN_SQUARED(x_side) + DQN_SQUARED(y_side);
    return result;
}

DQN_API Dqn_V2 Dqn_V2_Normalise(Dqn_V2 a)
{
    Dqn_f32 length_sq = DQN_SQUARED(a.x) + DQN_SQUARED(a.y);
    Dqn_f32 length    = DQN_SQRTF(length_sq);
    Dqn_V2 result     = a / length;
    return result;
}

DQN_API Dqn_V2 Dqn_V2_Perpendicular(Dqn_V2 a)
{
    Dqn_V2 result = Dqn_V2(-a.y, a.x);
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_V4
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_f32 Dqn_V4_Dot(Dqn_V4 const *a, Dqn_V4 const *b)
{
    Dqn_f32 result = (a->x * b->x) + (a->y * b->y) + (a->z * b->z) + (a->w * b->w);
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Rect
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_Rect Dqn_Rect_InitFromPosAndSize(Dqn_V2 pos, Dqn_V2 size)
{
    Dqn_Rect result = {};
    result.min  = pos;
    if (size.w < 0) result.min.x -= size.w;
    if (size.h < 0) result.min.y -= size.h;
    result.max  = result.min + Dqn_V2_Abs(size);
    return result;
}

DQN_API Dqn_V2 Dqn_Rect_Center(Dqn_Rect rect)
{
    Dqn_V2 size   = rect.max - rect.min;
    Dqn_V2 result = rect.min + (size * 0.5f);
    return result;
}

DQN_API Dqn_b32 Dqn_Rect_ContainsPoint(Dqn_Rect rect, Dqn_V2 p)
{
    Dqn_b32 result = (p.x >= rect.min.x && p.x <= rect.max.x && p.y >= rect.min.y && p.y <= rect.max.y);
    return result;
}


DQN_API Dqn_b32 Dqn_Rect_ContainsRect(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_b32 result = (b.min >= a.min && b.max <= a.max);
    return result;
}


DQN_API Dqn_V2 Dqn_Rect_Size(Dqn_Rect rect)
{
    Dqn_V2 result = rect.max - rect.min;
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_Move(Dqn_Rect src, Dqn_V2 move_amount)
{
    Dqn_Rect result = src;
    result.min += move_amount;
    result.max += move_amount;
    return result;
}

DQN_API Dqn_b32 Dqn_Rect_Intersects(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_b32 result = (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
                     (a.min.y <= b.max.y && a.max.y >= b.min.y);
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_Intersection(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_Rect result  = {};
    if (Dqn_Rect_Intersects(a, b))
    {
        result.min.x = DQN_MAX(a.min.x, b.min.x);
        result.min.y = DQN_MAX(a.min.y, b.min.y);
        result.max.x = DQN_MIN(a.max.x, b.max.x);
        result.max.y = DQN_MIN(a.max.y, b.max.y);
    }

    return result;
}

DQN_API Dqn_Rect Dqn_Rect_Union(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_Rect result  = {};
    result.min.x = DQN_MIN(a.min.x, b.min.x);
    result.min.y = DQN_MIN(a.min.y, b.min.y);
    result.max.x = DQN_MAX(a.max.x, b.max.x);
    result.max.y = DQN_MAX(a.max.y, b.max.y);
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_FromRectI32(Dqn_RectI32 a)
{
    Dqn_Rect result = Dqn_Rect(a.min, a.max);
    return result;
}

DQN_API Dqn_V2I Dqn_RectI32_Size(Dqn_RectI32 rect)
{
    Dqn_V2I result = rect.max - rect.min;
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Mat4
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_Mat4 Dqn_Mat4_Identity()
{
    Dqn_Mat4 result =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
    return result;
}

DQN_API Dqn_Mat4 Dqn_Mat4_Scale3f(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z)
{
    Dqn_Mat4 result =
    {
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1,
    };
    return result;
}

DQN_API Dqn_Mat4 Dqn_Mat4_ScaleV3(Dqn_V3 vec)
{
    Dqn_Mat4 result = Dqn_Mat4_Scale3f(vec.x, vec.y, vec.z);
    return result;
}

DQN_API Dqn_Mat4 Dqn_Mat4_Translate3f(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z)
{
    Dqn_Mat4 result =
    {
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1,
    };
    return result;
}

DQN_API Dqn_Mat4 Dqn_Mat4_TranslateV3(Dqn_V3 vec)
{
    Dqn_Mat4 result = Dqn_Mat4_Translate3f(vec.x, vec.y, vec.z);
    return result;
}

DQN_API Dqn_Mat4 operator*(Dqn_Mat4 const &a, Dqn_Mat4 const &b)
{
    Dqn_V4 const *row1 = a.row + 0;
    Dqn_V4 const *row2 = a.row + 1;
    Dqn_V4 const *row3 = a.row + 2;
    Dqn_V4 const *row4 = a.row + 3;
    Dqn_V4 const col1  = Dqn_V4(b.row_major[0][0], b.row_major[1][0], b.row_major[2][0], b.row_major[3][0]);
    Dqn_V4 const col2  = Dqn_V4(b.row_major[0][1], b.row_major[1][1], b.row_major[2][1], b.row_major[3][1]);
    Dqn_V4 const col3  = Dqn_V4(b.row_major[0][2], b.row_major[1][2], b.row_major[2][2], b.row_major[3][3]);
    Dqn_V4 const col4  = Dqn_V4(b.row_major[0][3], b.row_major[1][3], b.row_major[2][3], b.row_major[3][3]);

    Dqn_Mat4 result =
    {
        Dqn_V4_Dot(row1, &col1), Dqn_V4_Dot(row1, &col2), Dqn_V4_Dot(row1, &col3), Dqn_V4_Dot(row1, &col4),
        Dqn_V4_Dot(row2, &col1), Dqn_V4_Dot(row2, &col2), Dqn_V4_Dot(row2, &col3), Dqn_V4_Dot(row2, &col4),
        Dqn_V4_Dot(row3, &col1), Dqn_V4_Dot(row3, &col2), Dqn_V4_Dot(row3, &col3), Dqn_V4_Dot(row3, &col4),
        Dqn_V4_Dot(row4, &col1), Dqn_V4_Dot(row4, &col2), Dqn_V4_Dot(row4, &col3), Dqn_V4_Dot(row4, &col4),
    };
    return result;
}

DQN_API Dqn_V4 operator*(Dqn_Mat4 const &mat, Dqn_V4 const &vec)
{
    Dqn_f32 x = vec.x, y = vec.y, z = vec.z, w = vec.w;
    Dqn_V4 result =
    {
        (mat[0]  * x) + (mat[1]  * y) + (mat[2]  * z) + (mat[3]  * w),
        (mat[4]  * x) + (mat[5]  * y) + (mat[6]  * z) + (mat[7]  * w),
        (mat[8]  * x) + (mat[9]  * y) + (mat[10] * z) + (mat[11] * w),
        (mat[12] * x) + (mat[13] * y) + (mat[14] * z) + (mat[15] * w),
    };
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Math Utils
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_V2 Dqn_LerpV2(Dqn_V2 a, Dqn_f32 t, Dqn_V2 b)
{
    Dqn_V2 result = {};
    result.x  = a.x + ((b.x - a.x) * t);
    result.y  = a.y + ((b.y - a.y) * t);
    return result;
}

DQN_API Dqn_f32 Dqn_LerpF32(Dqn_f32 a, Dqn_f32 t, Dqn_f32 b)
{
    Dqn_f32 result = a + ((b - a) * t);
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Allocator
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_Allocator Dqn_Allocator_InitWithNull()
{
    Dqn_Allocator result = {};
    result.type          = Dqn_Allocator_Type::Null;
    return result;
}

DQN_API Dqn_Allocator Dqn_Allocator_InitWithHeap()
{
    Dqn_Allocator result = {};
    result.type          = Dqn_Allocator_Type::Heap;
    return result;
}

DQN_API Dqn_Allocator Dqn_Allocator_InitWithXHeap()
{
    Dqn_Allocator result = {};
    result.type          = Dqn_Allocator_Type::XHeap;
    return result;
}

DQN_API Dqn_Allocator Dqn_Allocator_InitWithArena(Dqn_ArenaAllocator *arena)
{
    Dqn_Allocator result = {};
    result.type          = Dqn_Allocator_Type::Arena;
    result.context.arena = arena;
    return result;
}

DQN_API void *Dqn_Allocator_Allocate(Dqn_Allocator *allocator, Dqn_isize size, Dqn_u8 alignment, Dqn_ZeroMem zero_mem)
{
    char *result = nullptr;
    switch (allocator->type)
    {
        case Dqn_Allocator_Type::Null: return result;
        default: break;

        case Dqn_Allocator_Type::Heap:
        case Dqn_Allocator_Type::XHeap:
        {
            size      = Dqn_PointerMetadata_SizeRequired(size, alignment);
            void *ptr = zero_mem == Dqn_ZeroMem::Yes ? DQN_CALLOC(1, DQN_CAST(size_t)size) : DQN_MALLOC(size);
            result = Dqn_PointerMetadata_Init(ptr, alignment);
            if (!result && allocator->type == Dqn_Allocator_Type::XHeap)
            {
                DQN_ASSERT(result);
            }
        }
        break;

        case Dqn_Allocator_Type::Arena:
        {
            result = DQN_CAST(char *)Dqn_ArenaAllocator_Allocate(allocator->context.arena, size, alignment, zero_mem);
        }
        break;

        case Dqn_Allocator_Type::Custom:
        {
            if (allocator->allocate)
                result = DQN_CAST(char *)allocator->allocate(size, alignment, allocator->context.user);
        }
        break;
    }

    if (result)
    {
        allocator->allocations++;
        allocator->total_allocations++;
        allocator->total_bytes_allocated += size;
    }
    return result;
}

void Dqn_Allocator_Free(Dqn_Allocator *allocator, void *ptr)
{
    switch (allocator->type)
    {
        case Dqn_Allocator_Type::Null: return;
        default: break;

        case Dqn_Allocator_Type::Heap:
        case Dqn_Allocator_Type::XHeap:
        {
            char *raw_ptr = Dqn_PointerMetadata_GetRawPointer(ptr);
            DQN_FREE(raw_ptr);
        }
        break;

        case Dqn_Allocator_Type::Custom:
        {
            if (allocator->free) allocator->free(ptr, allocator->context.user);
        }
        break;

        case Dqn_Allocator_Type::Arena:
            break;
    }

    if (ptr)
    {
        allocator->allocations--;
        DQN_ASSERT(allocator->allocations >= 0);
    }
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_ArenaAllocator
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_ArenaAllocatorBlock *Dqn_ArenaAllocator__AllocateBlock(Dqn_ArenaAllocator *arena, Dqn_isize requested_size)
{
    Dqn_isize min_block_size = arena->min_block_size;
    if (min_block_size == 0) min_block_size = DQN_MEM_ARENA_DEFAULT_MIN_BLOCK_SIZE;

    Dqn_isize mem_block_size = DQN_MAX(min_block_size, requested_size);
    auto const allocate_size = DQN_CAST(Dqn_isize)(sizeof(*arena->curr_mem_block) + mem_block_size);
    auto *result             = DQN_CAST(Dqn_ArenaAllocatorBlock *)Dqn_Allocator_Allocate(&arena->allocator, allocate_size, alignof(Dqn_ArenaAllocatorBlock), Dqn_ZeroMem::No);
    if (!result) return result;

    *result        = {};
    result->size   = mem_block_size;
    result->memory = DQN_CAST(Dqn_u8 *)result + sizeof(*result);
    arena->total_allocated_mem_blocks++;
    return result;
}

DQN_API void Dqn_ArenaAllocator__FreeBlock(Dqn_ArenaAllocator *arena, Dqn_ArenaAllocatorBlock *block)
{
    if (!block)
        return;

    if (block->next)
        block->next->prev = block->prev;

    if (block->prev)
        block->prev->next = block->next;

    Dqn_Allocator_Free(&arena->allocator, block);
}

DQN_API void Dqn_ArenaAllocator__AttachBlock(Dqn_ArenaAllocator *arena, Dqn_ArenaAllocatorBlock *new_block)
{
    if (arena->top_mem_block)
    {
        DQN_ASSERT(arena->top_mem_block->next == nullptr);
        arena->top_mem_block->next = new_block;
        new_block->prev            = arena->top_mem_block;
        arena->top_mem_block       = new_block;
    }
    else
    {
        arena->top_mem_block  = new_block;
        arena->curr_mem_block = new_block;
    }
}

DQN_API Dqn_ArenaAllocator Dqn_ArenaAllocator_InitWithAllocator(Dqn_Allocator allocator, Dqn_isize size)
{
    Dqn_ArenaAllocator result = {};
    result.allocator          = allocator;
    if (size > 0)
    {
        DQN_ASSERT_MSG(size >= DQN_ISIZEOF(*result.curr_mem_block), "(%zu >= %zu) There needs to be enough space to encode the Dqn_ArenaAllocatorBlock struct into the memory buffer", size, sizeof(*result.curr_mem_block));
        Dqn_ArenaAllocatorBlock *mem_block = Dqn_ArenaAllocator__AllocateBlock(&result, size);
        Dqn_ArenaAllocator__AttachBlock(&result, mem_block);
    }
    return result;
}

DQN_API Dqn_ArenaAllocator Dqn_ArenaAllocator_InitWithMemory(void *memory, Dqn_isize size)
{
    Dqn_ArenaAllocator result = {};
    DQN_ASSERT_MSG(size >= DQN_ISIZEOF(*result.curr_mem_block), "(%zu >= %zu) There needs to be enough space to encode the Dqn_ArenaAllocatorBlock struct into the memory buffer", size, sizeof(*result.curr_mem_block));
    result.allocator    = Dqn_Allocator_InitWithNull();
    auto *mem_block     = DQN_CAST(Dqn_ArenaAllocatorBlock *) memory;
    *mem_block          = {};
    mem_block->memory   = DQN_CAST(Dqn_u8 *) memory + sizeof(*mem_block);
    mem_block->size     = size - DQN_CAST(Dqn_isize)sizeof(*mem_block);
    Dqn_ArenaAllocator__AttachBlock(&result, mem_block);
    return result;
}

DQN_API void *Dqn_ArenaAllocator_Allocate(Dqn_ArenaAllocator *arena, Dqn_isize size, Dqn_u8 alignment, Dqn_ZeroMem zero_mem)
{
    Dqn_isize allocation_size  = size + (alignment - 1);
    Dqn_b32 need_new_mem_block = true;
    for (Dqn_ArenaAllocatorBlock *mem_block = arena->curr_mem_block; mem_block; mem_block = mem_block->next)
    {
        Dqn_b32 can_fit_in_block = (mem_block->used + allocation_size) <= mem_block->size;
        if (can_fit_in_block)
        {
            arena->curr_mem_block = mem_block;
            need_new_mem_block    = false;
            break;
        }
    }

    if (need_new_mem_block)
    {
        Dqn_ArenaAllocatorBlock *new_block = Dqn_ArenaAllocator__AllocateBlock(arena, allocation_size);
        if (!new_block) return nullptr;
        Dqn_ArenaAllocator__AttachBlock(arena, new_block);
        arena->curr_mem_block = arena->top_mem_block;
    }

    Dqn_uintptr address = DQN_CAST(Dqn_uintptr) arena->curr_mem_block->memory + arena->curr_mem_block->used;
    void *result        = DQN_CAST(void *) Dqn_AlignAddress(address, alignment);

    arena->curr_mem_block->used += allocation_size;
    DQN_ASSERT(arena->curr_mem_block->used <= arena->curr_mem_block->size);
    if (zero_mem == Dqn_ZeroMem::Yes) DQN_MEMSET(DQN_CAST(void *)address, 0, allocation_size);
    return result;
}

DQN_API void Dqn_ArenaAllocator_Free(Dqn_ArenaAllocator *arena)
{
    for (Dqn_ArenaAllocatorBlock *mem_block = arena->top_mem_block; mem_block;)
    {
        Dqn_ArenaAllocatorBlock *block_to_free = mem_block;
        mem_block                   = block_to_free->prev;
        Dqn_ArenaAllocator__FreeBlock(arena, block_to_free);
    }

    auto allocator           = arena->allocator;
    auto highest_used_mark   = arena->highest_used_mark;
    *arena                   = {};
    arena->highest_used_mark = highest_used_mark;
    arena->allocator         = allocator;
}

DQN_API Dqn_b32 Dqn_ArenaAllocator_Reserve(Dqn_ArenaAllocator *arena, Dqn_isize size)
{
    if (arena->top_mem_block)
    {
        Dqn_isize remaining_space = arena->top_mem_block->size - arena->top_mem_block->used;
        if (remaining_space >= size) return true;
    }

    Dqn_ArenaAllocatorBlock *new_block = Dqn_ArenaAllocator__AllocateBlock(arena, size);
    if (!new_block) return false;
    Dqn_ArenaAllocator__AttachBlock(arena, new_block);
    return true;
}

DQN_API void Dqn_ArenaAllocator_ResetUsage(Dqn_ArenaAllocator *arena, Dqn_ZeroMem zero_mem)
{
    arena->usage_before_last_reset   = 0;
    arena->wastage_before_last_reset = 0;
    for (Dqn_ArenaAllocatorBlock *block = arena->top_mem_block; block; block = block->prev)
    {
        arena->usage_before_last_reset += block->used;
        if (block->prev)
            arena->wastage_before_last_reset += (block->size - block->used);
        else
            arena->curr_mem_block = block;

        if (zero_mem == Dqn_ZeroMem::Yes)
            DQN_MEMSET(block->memory, 0, DQN_CAST(size_t)block->used);
        block->used = 0;
    }
}

DQN_API Dqn_ArenaAllocatorRegion Dqn_ArenaAllocator_BeginRegion(Dqn_ArenaAllocator *arena)
{
    Dqn_ArenaAllocatorRegion result  = {};
    result.arena               = arena;
    result.curr_mem_block      = arena->curr_mem_block;
    result.curr_mem_block_used = (arena->curr_mem_block) ? arena->curr_mem_block->used : 0;
    result.top_mem_block       = arena->top_mem_block;
    return result;
}

DQN_API void Dqn_ArenaAllocator_EndRegion(Dqn_ArenaAllocatorRegion region)
{
    while (region.top_mem_block != region.arena->top_mem_block)
    {
        Dqn_ArenaAllocatorBlock *block_to_free = region.arena->top_mem_block;
        if (region.arena->curr_mem_block == block_to_free)
            region.arena->curr_mem_block = block_to_free->prev;
        region.arena->top_mem_block = block_to_free->prev;
        Dqn_ArenaAllocator__FreeBlock(region.arena, block_to_free);
    }

    for (Dqn_ArenaAllocatorBlock *mem_block = region.arena->top_mem_block; mem_block != region.curr_mem_block; mem_block = mem_block->prev)
        mem_block->used = 0;

    if (region.arena->curr_mem_block)
        region.arena->curr_mem_block->used = region.curr_mem_block_used;
    region = {};
}

Dqn_ArenaAllocatorScopedRegion Dqn_ArenaAllocator_MakeScopedRegion(Dqn_ArenaAllocator *arena)
{
    return Dqn_ArenaAllocatorScopedRegion(arena);
}

Dqn_ArenaAllocatorScopedRegion::Dqn_ArenaAllocatorScopedRegion(Dqn_ArenaAllocator *arena)
{
    this->region = Dqn_ArenaAllocator_BeginRegion(arena);
}

Dqn_ArenaAllocatorScopedRegion::~Dqn_ArenaAllocatorScopedRegion()
{
    Dqn_ArenaAllocator_EndRegion(this->region);
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Bit
//
// -------------------------------------------------------------------------------------------------
DQN_API void Dqn_Bit_UnsetInplace(Dqn_u32 *flags, Dqn_u32 bitfield)
{
    *flags = (*flags & ~bitfield);
}

DQN_API void Dqn_Bit_SetInplace(Dqn_u32 *flags, Dqn_u32 bitfield)
{
    *flags = (*flags | bitfield);
}

DQN_API Dqn_b32 Dqn_Bit_IsSet(Dqn_u32 bits, Dqn_u32 bits_to_set)
{
    auto result = DQN_CAST(Dqn_b32)((bits & bits_to_set) == bits_to_set);
    return result;
}

DQN_API Dqn_b32 Dqn_Bit_IsNotSet(Dqn_u32 bits, Dqn_u32 bits_to_check)
{
    auto result = !Dqn_Bit_IsSet(bits, bits_to_check);
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Safe Arithmetic
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_i64 Dqn_Safe_AddI64(Dqn_i64 a, Dqn_i64 b)
{
    DQN_ASSERT_MSG(a <= INT64_MAX - b, "%zu <= %zu", a, INT64_MAX - b);
    Dqn_i64 result = a + b;
    return result;
}

DQN_API Dqn_i64 Dqn_Safe_MulI64(Dqn_i64 a, Dqn_i64 b)
{
    DQN_ASSERT_MSG(a <= INT64_MAX / b , "%zu <= %zu", a, INT64_MAX / b);
    Dqn_i64 result = a * b;
    return result;
}

DQN_API Dqn_u64 Dqn_Safe_AddU64(Dqn_u64 a, Dqn_u64 b)
{
    DQN_ASSERT_MSG(a <= UINT64_MAX - b, "%zu <= %zu", a, UINT64_MAX - b);
    Dqn_u64 result = a + b;
    return result;
}

DQN_API Dqn_u64 Dqn_Safe_SubU64(Dqn_u64 a, Dqn_u64 b)
{
    DQN_ASSERT_MSG(a >= b, "%zu >= %zu", a, b);
    Dqn_u64 result = a - b;
    return result;
}

DQN_API Dqn_u32 Dqn_Safe_SubU32(Dqn_u32 a, Dqn_u32 b)
{
    DQN_ASSERT_MSG(a >= b, "%zu >= %zu", a, b);
    Dqn_u32 result = a - b;
    return result;
}

DQN_API Dqn_u64 Dqn_Safe_MulU64(Dqn_u64 a, Dqn_u64 b)
{
    DQN_ASSERT_MSG(a <= UINT64_MAX / b , "%zu <= %zu", a, UINT64_MAX / b);
    Dqn_u64 result = a * b;
    return result;
}

DQN_API int Dqn_Safe_TruncateISizeToInt(Dqn_isize val)
{
    DQN_ASSERT_MSG(val >= INT_MIN && val <= INT_MAX, "%zd >= %zd && %zd <= %zd", val, INT_MIN, val, INT_MAX);
    auto result = (int)val;
    return result;
}

DQN_API Dqn_i32 Dqn_Safe_TruncateISizeToI32(Dqn_isize val)
{
    DQN_ASSERT_MSG(val >= INT32_MIN && val <= INT32_MAX, "%zd >= %zd && %zd <= %zd", val, INT32_MIN, val, INT32_MAX);
    auto result = DQN_CAST(Dqn_i32)val;
    return result;
}


DQN_API Dqn_i8 Dqn_Safe_TruncateISizeToI8(Dqn_isize val)
{
    DQN_ASSERT_MSG(val >= INT8_MIN && val <= INT8_MAX, "%zd >= %zd && %zd <= %zd", val, INT8_MIN, val, INT8_MAX);
    auto result = DQN_CAST(Dqn_i8)val;
    return result;
}


DQN_API Dqn_u32 Dqn_Safe_TruncateUSizeToU32(Dqn_u64 val)
{
    DQN_ASSERT_MSG(val <= UINT32_MAX, "%zu <= %zu", val, UINT32_MAX);
    auto result = DQN_CAST(Dqn_u32)val;
    return result;
}


DQN_API int Dqn_Safe_TruncateUSizeToI32(Dqn_usize val)
{
    DQN_ASSERT_MSG(val <= INT32_MAX, "%zu <= %zd", val, INT32_MAX);
    auto result = DQN_CAST(int)val;
    return result;
}


DQN_API int Dqn_Safe_TruncateUSizeToInt(Dqn_usize val)
{
    DQN_ASSERT_MSG(val <= INT_MAX, "%zu <= %zd", val, INT_MAX);
    auto result = DQN_CAST(int)val;
    return result;
}

DQN_API Dqn_isize Dqn_Safe_TruncateUSizeToISize(Dqn_usize val)
{
    DQN_ASSERT_MSG(val <= DQN_CAST(Dqn_usize)DQN_ISIZE_MAX, "%zu <= %zu", val, DQN_CAST(Dqn_usize)DQN_ISIZE_MAX);
    auto result = DQN_CAST(Dqn_isize)val;
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Char
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_b32 Dqn_Char_IsAlpha(char ch)
{
    Dqn_b32 result = (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
    return result;
}

DQN_API Dqn_b32 Dqn_Char_IsDigit(char ch)
{
    Dqn_b32 result = (ch >= '0' && ch <= '9');
    return result;
}

DQN_API Dqn_b32 Dqn_Char_IsAlphaNum(char ch)
{
    Dqn_b32 result = Dqn_Char_IsAlpha(ch) || Dqn_Char_IsDigit(ch);
    return result;
}

DQN_API Dqn_b32 Dqn_Char_IsWhitespace(char ch)
{
    Dqn_b32 result = (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
    return result;
}

DQN_API char Dqn_Char_ToLower(char ch)
{
    char result = ch;
    if (result >= 'A' && result <= 'Z')
        result = 'a' - 'A';
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_String
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_b32 Dqn_String_Compare(Dqn_String const lhs, Dqn_String const rhs)
{
    Dqn_b32 result = false;
    if (lhs.size == rhs.size)
        result = (memcmp(lhs.str, rhs.str, DQN_CAST(size_t)lhs.size) == 0);
    return result;
}

DQN_API Dqn_b32 Dqn_String_CompareCaseInsensitive(Dqn_String const lhs, Dqn_String const rhs)
{
    Dqn_b32 result = (lhs.size == rhs.size);
    for (Dqn_isize index = 0; index < lhs.size && result; index++)
        result = (Dqn_Char_ToLower(lhs.str[index]) == Dqn_Char_ToLower(rhs.str[index]));
    return result;
}

DQN_API Dqn_String Dqn_String_Copy(Dqn_String const src, Dqn_Allocator *allocator)
{
    Dqn_String result = src;
    result.str        = allocator ? DQN_CAST(char *)Dqn_Allocator_Allocate(allocator, result.size, alignof(char)) :
                                    DQN_CAST(char *)DQN_MALLOC(result.size);
    DQN_MEMCOPY(result.str, src.str, DQN_CAST(size_t)result.size);
    return result;
}

DQN_API Dqn_String Dqn_String_TrimWhitespaceAround(Dqn_String src)
{
    Dqn_String result = src;
    if (src.size <= 0) return result;

    char *start = src.str;
    char *end   = start + (src.size - 1);
    while (Dqn_Char_IsWhitespace(start[0])) start++;
    while (end != start && Dqn_Char_IsWhitespace(end[0])) end--;

    result.str  = start;
    result.size = (end - start) + 1;
    return result;
}

DQN_API Dqn_b32 operator==(Dqn_String const &lhs, Dqn_String const &rhs)
{
    Dqn_b32 result = lhs.size == rhs.size && (DQN_MEMCMP(lhs.str, rhs.str, lhs.size) == 0);
    return result;
}

DQN_API Dqn_String Dqn_String_FmtV(Dqn_Allocator *allocator, char const *fmt, va_list va)
{
    Dqn_String result = {};
    va_list va2;
    va_copy(va2, va);
    result.size = stbsp_vsnprintf(nullptr, 0, fmt, va);
    result.str  = allocator ? DQN_CAST(char *)Dqn_Allocator_Allocate(allocator, result.size + 1, alignof(char))
                            : DQN_CAST(char *)DQN_MALLOC(result.size + 1);
    if (result.str)
    {
        stbsp_vsnprintf(result.str, Dqn_Safe_TruncateISizeToInt(result.size + 1), fmt, va2);
        result.str[result.size] = 0;
    }
    va_end(va2);
    return result;
}

DQN_API Dqn_String Dqn_String_FmtF(Dqn_Allocator *allocator, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Dqn_String result = Dqn_String_FmtV(allocator, fmt, va);
    va_end(va);
    return result;
}

DQN_API void Dqn_String_Free(Dqn_String *string, Dqn_Allocator *allocator)
{
    if (allocator) Dqn_Allocator_Free(allocator, string->str);
    else           DQN_FREE(string->str);
    *string = {};
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Str
//
// -------------------------------------------------------------------------------------------------
DQN_API Dqn_b32 Dqn_Str_Equals(char const *a, char const *b, Dqn_isize a_len, Dqn_isize b_len)
{
    if (a_len == -1) a_len = DQN_CAST(Dqn_isize)strlen(a);
    if (b_len == -1) b_len = DQN_CAST(Dqn_isize)strlen(b);
    if (a_len != b_len) return false;
    return (strncmp(a, b, DQN_CAST(size_t)a_len) == 0);
}

DQN_API char const *Dqn_Str_FindMulti(char const *buf, char const *find_list[], Dqn_isize const *find_string_lens, Dqn_isize find_len, Dqn_isize *match_index, Dqn_isize buf_len)
{
    char const *result = nullptr;
    if (find_len == 0) return result;
    if (buf_len < 0) buf_len = DQN_CAST(Dqn_isize)strlen(buf);

    char const *buf_end = buf + buf_len;
    for (; buf != buf_end; ++buf)
    {
        Dqn_isize remaining = static_cast<Dqn_isize>(buf_end - buf);
        DQN_FOR_EACH(find_index, find_len)
        {
            char const *find   = find_list[find_index];
            Dqn_isize find_str_len = find_string_lens[find_index];
            if (remaining < find_str_len) continue;

            if (strncmp(buf, find, DQN_CAST(size_t)find_str_len) == 0)
            {
                result       = buf;
                *match_index = find_index;
                return result;
            }
        }

    }
    return result;
}

DQN_API char const *Dqn_Str_Find(char const *buf, char const *find, Dqn_isize buf_len, Dqn_isize find_len)
{
    if (find_len == 0) return nullptr;
    if (buf_len < 0) buf_len = DQN_CAST(Dqn_isize)strlen(buf);
    if (find_len < 0) find_len = DQN_CAST(Dqn_isize)strlen(find);

    char const *buf_end = buf + buf_len;
    char const *result = nullptr;
    for (; buf != buf_end; ++buf)
    {
        Dqn_isize remaining = static_cast<Dqn_isize>(buf_end - buf);
        if (remaining < find_len) break;

        if (strncmp(buf, find, DQN_CAST(size_t)find_len) == 0)
        {
            result = buf;
            break;
        }
    }
    return result;
}

DQN_API Dqn_b32 Dqn_Str_Match(char const *src, char const *find, int find_len)
{
    if (find_len == -1) find_len = Dqn_Safe_TruncateUSizeToInt(strlen(find));
    auto result = DQN_CAST(Dqn_b32)(strncmp(src, find, DQN_CAST(size_t)find_len) == 0);
    return result;
}

DQN_API char const *Dqn_Str_SkipToChar(char const *src, char ch)
{
    char const *result = src;
    while (result && result[0] && result[0] != ch) ++result;
    return result;
}

DQN_API char const *Dqn_Str_SkipToNextAlphaNum(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_Char_IsAlphaNum(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_Str_SkipToNextDigit(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_Char_IsDigit(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_Str_SkipToNextChar(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_Char_IsAlpha(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_Str_SkipToNextWord(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_Char_IsWhitespace(result[0])) ++result;
    while (result && result[0] && Dqn_Char_IsWhitespace(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_Str_SkipToNextWhitespace(char const *src)
{
    char const *result = src;
    while (result && result[0] && !Dqn_Char_IsWhitespace(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_Str_SkipWhitespace(char const *src)
{
    char const *result = src;
    while (result && result[0] && Dqn_Char_IsWhitespace(result[0])) ++result;
    return result;
}

DQN_API char const *Dqn_Str_SkipToCharInPlace(char const **src, char ch)
{
    *src = Dqn_Str_SkipToChar(*src, ch);
    return *src;
}

DQN_API char const *Dqn_Str_SkipToNextAlphaNumInPlace(char const **src)
{
    *src = Dqn_Str_SkipToNextAlphaNum(*src);
    return *src;
}

DQN_API char const *Dqn_Str_SkipToNextCharInPlace(char const **src)
{
    *src = Dqn_Str_SkipToNextChar(*src);
    return *src;
}

DQN_API char const *Dqn_Str_SkipToNextWhitespaceInPlace(char const **src)
{
    *src = Dqn_Str_SkipToNextWhitespace(*src);
    return *src;
}

DQN_API char const *Dqn_Str_SkipToNextWordInPlace(char const **src)
{
    *src = Dqn_Str_SkipToNextWord(*src);
    return *src;
}

DQN_API char const *Dqn_Str_SkipWhitespaceInPlace(char const **src)
{
    *src = Dqn_Str_SkipWhitespace(*src);
    return *src;
}

DQN_API Dqn_u64 Dqn_Str_ToU64(char const *buf, int len)
{
    Dqn_u64 result = 0;
    if (!buf) return result;
    if (len == -1) len = Dqn_Safe_TruncateUSizeToInt(strlen(buf));
    if (len == 0) return result;

    char const *buf_ptr = Dqn_Str_SkipWhitespace(buf);
    len -= static_cast<int>(buf_ptr - buf);
    for (int buf_index = 0; buf_index < len; ++buf_index)
    {
        char ch = buf_ptr[buf_index];
        if (ch == ',') continue;
        if (ch < '0' || ch > '9') break;

        Dqn_u64 val = DQN_CAST(Dqn_u64)(ch - '0');
        result  = Dqn_Safe_AddU64(result, val);
        result  = Dqn_Safe_MulU64(result, 10);
    }

    result /= 10;
    return result;
}

DQN_API Dqn_i64 Dqn_Str_ToI64(char const *buf, int len)
{
    Dqn_i64 result = 0;
    if (!buf) return result;
    if (len == -1) len = Dqn_Safe_TruncateUSizeToInt(strlen(buf));
    if (len == 0) return result;

    char const *buf_ptr = Dqn_Str_SkipWhitespace(buf);
    len -= static_cast<int>(buf_ptr - buf);

    Dqn_b32 negative = (buf[0] == '-');
    if (negative)
    {
        ++buf_ptr;
        --len;
    }

    for (int buf_index = 0; buf_index < len; ++buf_index)
    {
        char ch = buf_ptr[buf_index];
        if (ch == ',') continue;
        if (ch < '0' || ch > '9') break;

        Dqn_i64 val = ch - '0';
        result  = Dqn_Safe_AddI64(result, val);
        result  = Dqn_Safe_MulI64(result, 10);
    }

    result /= 10;
    if (negative) result *= -1;
    return result;
}

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_File
//
// -------------------------------------------------------------------------------------------------
DQN_API char *Dqn_File_ReadEntireFile(char const *file, Dqn_isize *file_size, Dqn_Allocator *allocator)
{
    Dqn_isize file_size_ = 0;
    if (!file_size)
        file_size = &file_size_;

    FILE *file_handle = fopen(file, "rb");
    if (!file_handle)
    {
        DQN_LOG_E("Failed to open file '%s' using fopen\n", file);
        return nullptr;
    }

    DQN_DEFER { fclose(file_handle); };
    fseek(file_handle, 0, SEEK_END);
    *file_size = ftell(file_handle);

    if (DQN_CAST(long)(*file_size) == -1L)
    {
        DQN_LOG_E("Failed to determine '%s' file size using ftell\n", file);
        return nullptr;
    }

    rewind(file_handle);
    auto *result = allocator ? DQN_CAST(char *) Dqn_Allocator_Allocate(allocator, *file_size + 1, alignof(char))
                             : DQN_CAST(char *) DQN_MALLOC(*file_size + 1);
    if (!result)
    {
        DQN_LOG_M("Failed to allocate %td bytes to read file '%s'\n", *file_size + 1, file);
        return nullptr;
    }

    result[*file_size] = 0;
    if (fread(result, DQN_CAST(size_t)(*file_size), 1, file_handle) != 1)
    {
        if (allocator) Dqn_Allocator_Free(allocator, result);
        else           DQN_FREE(result);

        DQN_LOG_E("Failed to read %td bytes into buffer from '%s'\n", *file_size, file);
        return nullptr;
    }

    return result;
}

DQN_API Dqn_b32 Dqn_File_WriteEntireFile(char const *file, char const *buffer, Dqn_isize buffer_size)
{
    FILE *file_handle = fopen(file, "w+b");
    if (!file_handle)
    {
        DQN_LOG_E("Failed to open file '%s' using fopen\n", file);
        return false;
    }

    DQN_DEFER { fclose(file_handle); };
    Dqn_usize write_count = fwrite(buffer, buffer_size, 1 /*count*/, file_handle);

    if (write_count != 1)
    {
        DQN_LOG_E("Failed to write to file '%s' using fwrite\n", file);
        return false;
    }

    return true;
}
// -------------------------------------------------------------------------------------------------
//
// NOTE: Utilities
//
// -------------------------------------------------------------------------------------------------
DQN_API char *Dqn_EpochTimeToDate(Dqn_i64 timestamp, char *buf, Dqn_isize buf_len)
{
    DQN_ASSERT(buf_len >= 0);
    time_t time   = DQN_CAST(time_t)timestamp;
    tm *date_time = localtime(&time);
    strftime(buf, DQN_CAST(Dqn_usize)buf_len, "%c", date_time);
    return buf;
}

DQN_API char *Dqn_U64ToStr(Dqn_u64 val, Dqn_U64Str *result, Dqn_b32 comma_sep)
{
    int buf_index            = (int)(Dqn_ArrayCount(result->buf) - 1);
    result->buf[buf_index--] = 0;

    if (val == 0)
    {
        result->buf[buf_index--] = '0';
        result->len              = 1;
    }
    else
    {
        for (int digit_count = 0; val > 0; result->len++, digit_count++)
        {
            if (comma_sep && (digit_count != 0) && (digit_count % 3 == 0))
            {
                result->buf[buf_index--] = ',';
                result->len++;
            }

            auto digit               = DQN_CAST(char)(val % 10);
            result->buf[buf_index--] = '0' + digit;
            val /= 10;
        }
    }

    result->str = result->buf + (buf_index + 1);
    return result->str;
}
#endif // DQN_IMPLEMENTATION

#ifdef STB_SPRINTF_IMPLEMENTATION
#include <stdlib.h> // for va_arg()

#define stbsp__uint32 unsigned int
#define stbsp__int32 signed int

#ifdef _MSC_VER
#define stbsp__uint64 unsigned __int64
#define stbsp__int64 signed __int64
#else
#define stbsp__uint64 unsigned long long
#define stbsp__int64 signed long long
#endif
#define stbsp__uint16 unsigned short

#ifndef stbsp__uintptr
#if defined(__ppc64__) || defined(__aarch64__) || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64)
#define stbsp__uintptr stbsp__uint64
#else
#define stbsp__uintptr stbsp__uint32
#endif
#endif

#ifndef STB_SPRINTF_MSVC_MODE // used for MSVC2013 and earlier (MSVC2015 matches GCC)
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define STB_SPRINTF_MSVC_MODE
#endif
#endif

#ifdef STB_SPRINTF_NOUNALIGNED // define this before inclusion to force stbsp_sprintf to always use aligned accesses
#define STBSP__UNALIGNED(code)
#else
#define STBSP__UNALIGNED(code) code
#endif

#ifndef STB_SPRINTF_NOFLOAT
// internal float utility functions
static stbsp__int32 stbsp__real_to_str(char const **start, stbsp__uint32 *len, char *out, stbsp__int32 *decimal_pos, double value, stbsp__uint32 frac_digits);
static stbsp__int32 stbsp__real_to_parts(stbsp__int64 *bits, stbsp__int32 *expo, double value);
#define STBSP__SPECIAL 0x7000
#endif

static char stbsp__period = '.';
static char stbsp__comma = ',';
static char stbsp__digitpair[201] =
   "0001020304050607080910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576"
   "7778798081828384858687888990919293949596979899";

STBSP__PUBLICDEF void STB_SPRINTF_DECORATE(set_separators)(char pcomma, char pperiod)
{
   stbsp__period = pperiod;
   stbsp__comma = pcomma;
}

#define STBSP__LEFTJUST 1
#define STBSP__LEADINGPLUS 2
#define STBSP__LEADINGSPACE 4
#define STBSP__LEADING_0X 8
#define STBSP__LEADINGZERO 16
#define STBSP__INTMAX 32
#define STBSP__TRIPLET_COMMA 64
#define STBSP__NEGATIVE 128
#define STBSP__METRIC_SUFFIX 256
#define STBSP__HALFWIDTH 512
#define STBSP__METRIC_NOSPACE 1024
#define STBSP__METRIC_1024 2048
#define STBSP__METRIC_JEDEC 4096

static void stbsp__lead_sign(stbsp__uint32 fl, char *sign)
{
   sign[0] = 0;
   if (fl & STBSP__NEGATIVE) {
      sign[0] = 1;
      sign[1] = '-';
   } else if (fl & STBSP__LEADINGSPACE) {
      sign[0] = 1;
      sign[1] = ' ';
   } else if (fl & STBSP__LEADINGPLUS) {
      sign[0] = 1;
      sign[1] = '+';
   }
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va)
{
   static char hex[] = "0123456789abcdefxp";
   static char hexu[] = "0123456789ABCDEFXP";
   char *bf;
   char const *f;
   int tlen = 0;

   bf = buf;
   f = fmt;
   for (;;) {
      stbsp__int32 fw, pr, tz;
      stbsp__uint32 fl;

      // macros for the callback buffer stuff
      #define stbsp__chk_cb_bufL(bytes)                        \
         {                                                     \
            int len = (int)(bf - buf);                         \
            if ((len + (bytes)) >= STB_SPRINTF_MIN) {          \
               tlen += len;                                    \
               if (0 == (bf = buf = callback(buf, user, len))) \
                  goto done;                                   \
            }                                                  \
         }
      #define stbsp__chk_cb_buf(bytes)    \
         {                                \
            if (callback) {               \
               stbsp__chk_cb_bufL(bytes); \
            }                             \
         }
      #define stbsp__flush_cb()                      \
         {                                           \
            stbsp__chk_cb_bufL(STB_SPRINTF_MIN - 1); \
         } // flush if there is even one byte in the buffer
      #define stbsp__cb_buf_clamp(cl, v)                \
         cl = v;                                        \
         if (callback) {                                \
            int lg = STB_SPRINTF_MIN - (int)(bf - buf); \
            if (cl > lg)                                \
               cl = lg;                                 \
         }

      // fast copy everything up to the next % (or end of string)
      for (;;) {
         while (((stbsp__uintptr)f) & 3) {
         schk1:
            if (f[0] == '%')
               goto scandd;
         schk2:
            if (f[0] == 0)
               goto endfmt;
            stbsp__chk_cb_buf(1);
            *bf++ = f[0];
            ++f;
         }
         for (;;) {
            // Check if the next 4 bytes contain %(0x25) or end of string.
            // Using the 'hasless' trick:
            // https://graphics.stanford.edu/~seander/bithacks.html#HasLessInWord
            stbsp__uint32 v, c;
            v = *(stbsp__uint32 *)f;
            c = (~v) & 0x80808080;
            if (((v ^ 0x25252525) - 0x01010101) & c)
               goto schk1;
            if ((v - 0x01010101) & c)
               goto schk2;
            if (callback)
               if ((STB_SPRINTF_MIN - (int)(bf - buf)) < 4)
                  goto schk1;
            *(stbsp__uint32 *)bf = v;
            bf += 4;
            f += 4;
         }
      }
   scandd:

      ++f;

      // ok, we have a percent, read the modifiers first
      fw = 0;
      pr = -1;
      fl = 0;
      tz = 0;

      // flags
      for (;;) {
         switch (f[0]) {
         // if we have left justify
         case '-':
            fl |= STBSP__LEFTJUST;
            ++f;
            continue;
         // if we have leading plus
         case '+':
            fl |= STBSP__LEADINGPLUS;
            ++f;
            continue;
         // if we have leading space
         case ' ':
            fl |= STBSP__LEADINGSPACE;
            ++f;
            continue;
         // if we have leading 0x
         case '#':
            fl |= STBSP__LEADING_0X;
            ++f;
            continue;
         // if we have thousand commas
         case '\'':
            fl |= STBSP__TRIPLET_COMMA;
            ++f;
            continue;
         // if we have kilo marker (none->kilo->kibi->jedec)
         case '$':
            if (fl & STBSP__METRIC_SUFFIX) {
               if (fl & STBSP__METRIC_1024) {
                  fl |= STBSP__METRIC_JEDEC;
               } else {
                  fl |= STBSP__METRIC_1024;
               }
            } else {
               fl |= STBSP__METRIC_SUFFIX;
            }
            ++f;
            continue;
         // if we don't want space between metric suffix and number
         case '_':
            fl |= STBSP__METRIC_NOSPACE;
            ++f;
            continue;
         // if we have leading zero
         case '0':
            fl |= STBSP__LEADINGZERO;
            ++f;
            goto flags_done;
         default: goto flags_done;
         }
      }
   flags_done:

      // get the field width
      if (f[0] == '*') {
         fw = va_arg(va, stbsp__uint32);
         ++f;
      } else {
         while ((f[0] >= '0') && (f[0] <= '9')) {
            fw = fw * 10 + f[0] - '0';
            f++;
         }
      }
      // get the precision
      if (f[0] == '.') {
         ++f;
         if (f[0] == '*') {
            pr = va_arg(va, stbsp__uint32);
            ++f;
         } else {
            pr = 0;
            while ((f[0] >= '0') && (f[0] <= '9')) {
               pr = pr * 10 + f[0] - '0';
               f++;
            }
         }
      }

      // handle integer size overrides
      switch (f[0]) {
      // are we halfwidth?
      case 'h':
         fl |= STBSP__HALFWIDTH;
         ++f;
         break;
      // are we 64-bit (unix style)
      case 'l':
         ++f;
         if (f[0] == 'l') {
            fl |= STBSP__INTMAX;
            ++f;
         }
         break;
      // are we 64-bit on intmax? (c99)
      case 'j':
         fl |= STBSP__INTMAX;
         ++f;
         break;
      // are we 64-bit on size_t or ptrdiff_t? (c99)
      case 'z':
      case 't':
         fl |= ((sizeof(char *) == 8) ? STBSP__INTMAX : 0);
         ++f;
         break;
      // are we 64-bit (msft style)
      case 'I':
         if ((f[1] == '6') && (f[2] == '4')) {
            fl |= STBSP__INTMAX;
            f += 3;
         } else if ((f[1] == '3') && (f[2] == '2')) {
            f += 3;
         } else {
            fl |= ((sizeof(void *) == 8) ? STBSP__INTMAX : 0);
            ++f;
         }
         break;
      default: break;
      }

      // handle each replacement
      switch (f[0]) {
         #define STBSP__NUMSZ 512 // big enough for e308 (with commas) or e-307
         char num[STBSP__NUMSZ];
         char lead[8];
         char tail[8];
         char *s;
         char const *h;
         stbsp__uint32 l, n, cs;
         stbsp__uint64 n64;
#ifndef STB_SPRINTF_NOFLOAT
         double fv;
#endif
         stbsp__int32 dp;
         char const *sn;

      case 's':
         // get the string
         s = va_arg(va, char *);
         if (s == 0)
            s = (char *)"null";
         // get the length
         sn = s;
         for (;;) {
            if ((((stbsp__uintptr)sn) & 3) == 0)
               break;
         lchk:
            if (sn[0] == 0)
               goto ld;
            ++sn;
         }
         n = 0xffffffff;
         if (pr >= 0) {
            n = (stbsp__uint32)(sn - s);
            if (n >= (stbsp__uint32)pr)
               goto ld;
            n = ((stbsp__uint32)(pr - n)) >> 2;
         }
         while (n) {
            stbsp__uint32 v = *(stbsp__uint32 *)sn;
            if ((v - 0x01010101) & (~v) & 0x80808080UL)
               goto lchk;
            sn += 4;
            --n;
         }
         goto lchk;
      ld:

         l = (stbsp__uint32)(sn - s);
         // clamp to precision
         if (l > (stbsp__uint32)pr)
            l = pr;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         // copy the string in
         goto scopy;

      case 'c': // char
         // get the character
         s = num + STBSP__NUMSZ - 1;
         *s = (char)va_arg(va, int);
         l = 1;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;

      case 'n': // weird write-bytes specifier
      {
         int *d = va_arg(va, int *);
         *d = tlen + (int)(bf - buf);
      } break;

#ifdef STB_SPRINTF_NOFLOAT
      case 'A':              // float
      case 'a':              // hex float
      case 'G':              // float
      case 'g':              // float
      case 'E':              // float
      case 'e':              // float
      case 'f':              // float
         va_arg(va, double); // eat it
         s = (char *)"No float";
         l = 8;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;
#else
      case 'A': // hex float
      case 'a': // hex float
         h = (f[0] == 'A') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_parts((stbsp__int64 *)&n64, &dp, fv))
            fl |= STBSP__NEGATIVE;

         s = num + 64;

         stbsp__lead_sign(fl, lead);

         if (dp == -1023)
            dp = (n64) ? -1022 : 0;
         else
            n64 |= (((stbsp__uint64)1) << 52);
         n64 <<= (64 - 56);
         if (pr < 15)
            n64 += ((((stbsp__uint64)8) << 56) >> (pr * 4));
// add leading chars

#ifdef STB_SPRINTF_MSVC_MODE
         *s++ = '0';
         *s++ = 'x';
#else
         lead[1 + lead[0]] = '0';
         lead[2 + lead[0]] = 'x';
         lead[0] += 2;
#endif
         *s++ = h[(n64 >> 60) & 15];
         n64 <<= 4;
         if (pr)
            *s++ = stbsp__period;
         sn = s;

         // print the bits
         n = pr;
         if (n > 13)
            n = 13;
         if (pr > (stbsp__int32)n)
            tz = pr - n;
         pr = 0;
         while (n--) {
            *s++ = h[(n64 >> 60) & 15];
            n64 <<= 4;
         }

         // print the expo
         tail[1] = h[17];
         if (dp < 0) {
            tail[2] = '-';
            dp = -dp;
         } else
            tail[2] = '+';
         n = (dp >= 1000) ? 6 : ((dp >= 100) ? 5 : ((dp >= 10) ? 4 : 3));
         tail[0] = (char)n;
         for (;;) {
            tail[n] = '0' + dp % 10;
            if (n <= 3)
               break;
            --n;
            dp /= 10;
         }

         dp = (int)(s - sn);
         l = (int)(s - (num + 64));
         s = num + 64;
         cs = 1 + (3 << 24);
         goto scopy;

      case 'G': // float
      case 'g': // float
         h = (f[0] == 'G') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6;
         else if (pr == 0)
            pr = 1; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, (pr - 1) | 0x80000000))
            fl |= STBSP__NEGATIVE;

         // clamp the precision and delete extra zeros after clamp
         n = pr;
         if (l > (stbsp__uint32)pr)
            l = pr;
         while ((l > 1) && (pr) && (sn[l - 1] == '0')) {
            --pr;
            --l;
         }

         // should we use %e
         if ((dp <= -4) || (dp > (stbsp__int32)n)) {
            if (pr > (stbsp__int32)l)
               pr = l - 1;
            else if (pr)
               --pr; // when using %e, there is one digit before the decimal
            goto doexpfromg;
         }
         // this is the insane action to get the pr to match %g sematics for %f
         if (dp > 0) {
            pr = (dp < (stbsp__int32)l) ? l - dp : 0;
         } else {
            pr = -dp + ((pr > (stbsp__int32)l) ? l : pr);
         }
         goto dofloatfromg;

      case 'E': // float
      case 'e': // float
         h = (f[0] == 'E') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, pr | 0x80000000))
            fl |= STBSP__NEGATIVE;
      doexpfromg:
         tail[0] = 0;
         stbsp__lead_sign(fl, lead);
         if (dp == STBSP__SPECIAL) {
            s = (char *)sn;
            cs = 0;
            pr = 0;
            goto scopy;
         }
         s = num + 64;
         // handle leading chars
         *s++ = sn[0];

         if (pr)
            *s++ = stbsp__period;

         // handle after decimal
         if ((l - 1) > (stbsp__uint32)pr)
            l = pr + 1;
         for (n = 1; n < l; n++)
            *s++ = sn[n];
         // trailing zeros
         tz = pr - (l - 1);
         pr = 0;
         // dump expo
         tail[1] = h[0xe];
         dp -= 1;
         if (dp < 0) {
            tail[2] = '-';
            dp = -dp;
         } else
            tail[2] = '+';
#ifdef STB_SPRINTF_MSVC_MODE
         n = 5;
#else
         n = (dp >= 100) ? 5 : 4;
#endif
         tail[0] = (char)n;
         for (;;) {
            tail[n] = '0' + dp % 10;
            if (n <= 3)
               break;
            --n;
            dp /= 10;
         }
         cs = 1 + (3 << 24); // how many tens
         goto flt_lead;

      case 'f': // float
         fv = va_arg(va, double);
      doafloat:
         // do kilos
         if (fl & STBSP__METRIC_SUFFIX) {
            double divisor;
            divisor = 1000.0f;
            if (fl & STBSP__METRIC_1024)
               divisor = 1024.0;
            while (fl < 0x4000000) {
               if ((fv < divisor) && (fv > -divisor))
                  break;
               fv /= divisor;
               fl += 0x1000000;
            }
         }
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, pr))
            fl |= STBSP__NEGATIVE;
      dofloatfromg:
         tail[0] = 0;
         stbsp__lead_sign(fl, lead);
         if (dp == STBSP__SPECIAL) {
            s = (char *)sn;
            cs = 0;
            pr = 0;
            goto scopy;
         }
         s = num + 64;

         // handle the three decimal varieties
         if (dp <= 0) {
            stbsp__int32 i;
            // handle 0.000*000xxxx
            *s++ = '0';
            if (pr)
               *s++ = stbsp__period;
            n = -dp;
            if ((stbsp__int32)n > pr)
               n = pr;
            i = n;
            while (i) {
               if ((((stbsp__uintptr)s) & 3) == 0)
                  break;
               *s++ = '0';
               --i;
            }
            while (i >= 4) {
               *(stbsp__uint32 *)s = 0x30303030;
               s += 4;
               i -= 4;
            }
            while (i) {
               *s++ = '0';
               --i;
            }
            if ((stbsp__int32)(l + n) > pr)
               l = pr - n;
            i = l;
            while (i) {
               *s++ = *sn++;
               --i;
            }
            tz = pr - (n + l);
            cs = 1 + (3 << 24); // how many tens did we write (for commas below)
         } else {
            cs = (fl & STBSP__TRIPLET_COMMA) ? ((600 - (stbsp__uint32)dp) % 3) : 0;
            if ((stbsp__uint32)dp >= l) {
               // handle xxxx000*000.0
               n = 0;
               for (;;) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                     cs = 0;
                     *s++ = stbsp__comma;
                  } else {
                     *s++ = sn[n];
                     ++n;
                     if (n >= l)
                        break;
                  }
               }
               if (n < (stbsp__uint32)dp) {
                  n = dp - n;
                  if ((fl & STBSP__TRIPLET_COMMA) == 0) {
                     while (n) {
                        if ((((stbsp__uintptr)s) & 3) == 0)
                           break;
                        *s++ = '0';
                        --n;
                     }
                     while (n >= 4) {
                        *(stbsp__uint32 *)s = 0x30303030;
                        s += 4;
                        n -= 4;
                     }
                  }
                  while (n) {
                     if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                        cs = 0;
                        *s++ = stbsp__comma;
                     } else {
                        *s++ = '0';
                        --n;
                     }
                  }
               }
               cs = (int)(s - (num + 64)) + (3 << 24); // cs is how many tens
               if (pr) {
                  *s++ = stbsp__period;
                  tz = pr;
               }
            } else {
               // handle xxxxx.xxxx000*000
               n = 0;
               for (;;) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                     cs = 0;
                     *s++ = stbsp__comma;
                  } else {
                     *s++ = sn[n];
                     ++n;
                     if (n >= (stbsp__uint32)dp)
                        break;
                  }
               }
               cs = (int)(s - (num + 64)) + (3 << 24); // cs is how many tens
               if (pr)
                  *s++ = stbsp__period;
               if ((l - dp) > (stbsp__uint32)pr)
                  l = pr + dp;
               while (n < l) {
                  *s++ = sn[n];
                  ++n;
               }
               tz = pr - (l - dp);
            }
         }
         pr = 0;

         // handle k,m,g,t
         if (fl & STBSP__METRIC_SUFFIX) {
            char idx;
            idx = 1;
            if (fl & STBSP__METRIC_NOSPACE)
               idx = 0;
            tail[0] = idx;
            tail[1] = ' ';
            {
               if (fl >> 24) { // SI kilo is 'k', JEDEC and SI kibits are 'K'.
                  if (fl & STBSP__METRIC_1024)
                     tail[idx + 1] = "_KMGT"[fl >> 24];
                  else
                     tail[idx + 1] = "_kMGT"[fl >> 24];
                  idx++;
                  // If printing kibits and not in jedec, add the 'i'.
                  if (fl & STBSP__METRIC_1024 && !(fl & STBSP__METRIC_JEDEC)) {
                     tail[idx + 1] = 'i';
                     idx++;
                  }
                  tail[0] = idx;
               }
            }
         };

      flt_lead:
         // get the length that we copied
         l = (stbsp__uint32)(s - (num + 64));
         s = num + 64;
         goto scopy;
#endif

      case 'B': // upper binary
      case 'b': // lower binary
         h = (f[0] == 'B') ? hexu : hex;
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[0xb];
         }
         l = (8 << 4) | (1 << 8);
         goto radixnum;

      case 'o': // octal
         h = hexu;
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 1;
            lead[1] = '0';
         }
         l = (3 << 4) | (3 << 8);
         goto radixnum;

      case 'p': // pointer
         fl |= (sizeof(void *) == 8) ? STBSP__INTMAX : 0;
         pr = sizeof(void *) * 2;
         fl &= ~STBSP__LEADINGZERO; // 'p' only prints the pointer with zeros
                                    // fall through - to X

      case 'X': // upper hex
      case 'x': // lower hex
         h = (f[0] == 'X') ? hexu : hex;
         l = (4 << 4) | (4 << 8);
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[16];
         }
      radixnum:
         // get the number
         if (fl & STBSP__INTMAX)
            n64 = va_arg(va, stbsp__uint64);
         else
            n64 = va_arg(va, stbsp__uint32);

         s = num + STBSP__NUMSZ;
         dp = 0;
         // clear tail, and clear leading if value is zero
         tail[0] = 0;
         if (n64 == 0) {
            lead[0] = 0;
            if (pr == 0) {
               l = 0;
               cs = (((l >> 4) & 15)) << 24;
               goto scopy;
            }
         }
         // convert to string
         for (;;) {
            *--s = h[n64 & ((1 << (l >> 8)) - 1)];
            n64 >>= (l >> 8);
            if (!((n64) || ((stbsp__int32)((num + STBSP__NUMSZ) - s) < pr)))
               break;
            if (fl & STBSP__TRIPLET_COMMA) {
               ++l;
               if ((l & 15) == ((l >> 4) & 15)) {
                  l &= ~15;
                  *--s = stbsp__comma;
               }
            }
         };
         // get the tens and the comma pos
         cs = (stbsp__uint32)((num + STBSP__NUMSZ) - s) + ((((l >> 4) & 15)) << 24);
         // get the length that we copied
         l = (stbsp__uint32)((num + STBSP__NUMSZ) - s);
         // copy it
         goto scopy;

      case 'u': // unsigned
      case 'i':
      case 'd': // integer
         // get the integer and abs it
         if (fl & STBSP__INTMAX) {
            stbsp__int64 i64 = va_arg(va, stbsp__int64);
            n64 = (stbsp__uint64)i64;
            if ((f[0] != 'u') && (i64 < 0)) {
               n64 = (stbsp__uint64)-i64;
               fl |= STBSP__NEGATIVE;
            }
         } else {
            stbsp__int32 i = va_arg(va, stbsp__int32);
            n64 = (stbsp__uint32)i;
            if ((f[0] != 'u') && (i < 0)) {
               n64 = (stbsp__uint32)-i;
               fl |= STBSP__NEGATIVE;
            }
         }

#ifndef STB_SPRINTF_NOFLOAT
         if (fl & STBSP__METRIC_SUFFIX) {
            if (n64 < 1024)
               pr = 0;
            else if (pr == -1)
               pr = 1;
            fv = (double)(stbsp__int64)n64;
            goto doafloat;
         }
#endif

         // convert to string
         s = num + STBSP__NUMSZ;
         l = 0;

         for (;;) {
            // do in 32-bit chunks (avoid lots of 64-bit divides even with constant denominators)
            char *o = s - 8;
            if (n64 >= 100000000) {
               n = (stbsp__uint32)(n64 % 100000000);
               n64 /= 100000000;
            } else {
               n = (stbsp__uint32)n64;
               n64 = 0;
            }
            if ((fl & STBSP__TRIPLET_COMMA) == 0) {
               do {
                  s -= 2;
                  *(stbsp__uint16 *)s = *(stbsp__uint16 *)&stbsp__digitpair[(n % 100) * 2];
                  n /= 100;
               } while (n);
            }
            while (n) {
               if ((fl & STBSP__TRIPLET_COMMA) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = (char)(n % 10) + '0';
                  n /= 10;
               }
            }
            if (n64 == 0) {
               if ((s[0] == '0') && (s != (num + STBSP__NUMSZ)))
                  ++s;
               break;
            }
            while (s != o)
               if ((fl & STBSP__TRIPLET_COMMA) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = '0';
               }
         }

         tail[0] = 0;
         stbsp__lead_sign(fl, lead);

         // get the length that we copied
         l = (stbsp__uint32)((num + STBSP__NUMSZ) - s);
         if (l == 0) {
            *--s = '0';
            l = 1;
         }
         cs = l + (3 << 24);
         if (pr < 0)
            pr = 0;

      scopy:
         // get fw=leading/trailing space, pr=leading zeros
         if (pr < (stbsp__int32)l)
            pr = l;
         n = pr + lead[0] + tail[0] + tz;
         if (fw < (stbsp__int32)n)
            fw = n;
         fw -= n;
         pr -= l;

         // handle right justify and leading zeros
         if ((fl & STBSP__LEFTJUST) == 0) {
            if (fl & STBSP__LEADINGZERO) // if leading zeros, everything is in pr
            {
               pr = (fw > pr) ? fw : pr;
               fw = 0;
            } else {
               fl &= ~STBSP__TRIPLET_COMMA; // if no leading zeros, then no commas
            }
         }

         // copy the spaces and/or zeros
         if (fw + pr) {
            stbsp__int32 i;
            stbsp__uint32 c;

            // copy leading spaces (or when doing %8.4d stuff)
            if ((fl & STBSP__LEFTJUST) == 0)
               while (fw > 0) {
                  stbsp__cb_buf_clamp(i, fw);
                  fw -= i;
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i) {
                     *bf++ = ' ';
                     --i;
                  }
                  stbsp__chk_cb_buf(1);
               }

            // copy leader
            sn = lead + 1;
            while (lead[0]) {
               stbsp__cb_buf_clamp(i, lead[0]);
               lead[0] -= (char)i;
               while (i) {
                  *bf++ = *sn++;
                  --i;
               }
               stbsp__chk_cb_buf(1);
            }

            // copy leading zeros
            c = cs >> 24;
            cs &= 0xffffff;
            cs = (fl & STBSP__TRIPLET_COMMA) ? ((stbsp__uint32)(c - ((pr + cs) % (c + 1)))) : 0;
            while (pr > 0) {
               stbsp__cb_buf_clamp(i, pr);
               pr -= i;
               if ((fl & STBSP__TRIPLET_COMMA) == 0) {
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = '0';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x30303030;
                     bf += 4;
                     i -= 4;
                  }
               }
               while (i) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (cs++ == c)) {
                     cs = 0;
                     *bf++ = stbsp__comma;
                  } else
                     *bf++ = '0';
                  --i;
               }
               stbsp__chk_cb_buf(1);
            }
         }

         // copy leader if there is still one
         sn = lead + 1;
         while (lead[0]) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, lead[0]);
            lead[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy the string
         n = l;
         while (n) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, n);
            n -= i;
            STBSP__UNALIGNED(while (i >= 4) {
               *(stbsp__uint32 *)bf = *(stbsp__uint32 *)s;
               bf += 4;
               s += 4;
               i -= 4;
            })
            while (i) {
               *bf++ = *s++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy trailing zeros
         while (tz) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, tz);
            tz -= i;
            while (i) {
               if ((((stbsp__uintptr)bf) & 3) == 0)
                  break;
               *bf++ = '0';
               --i;
            }
            while (i >= 4) {
               *(stbsp__uint32 *)bf = 0x30303030;
               bf += 4;
               i -= 4;
            }
            while (i) {
               *bf++ = '0';
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy tail if there is one
         sn = tail + 1;
         while (tail[0]) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, tail[0]);
            tail[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // handle the left justify
         if (fl & STBSP__LEFTJUST)
            if (fw > 0) {
               while (fw) {
                  stbsp__int32 i;
                  stbsp__cb_buf_clamp(i, fw);
                  fw -= i;
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i--)
                     *bf++ = ' ';
                  stbsp__chk_cb_buf(1);
               }
            }
         break;

      default: // unknown, just copy code
         s = num + STBSP__NUMSZ - 1;
         *s = f[0];
         l = 1;
         fw = fl = 0;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;
      }
      ++f;
   }
endfmt:

   if (!callback)
      *bf = 0;
   else
      stbsp__flush_cb();

done:
   return tlen + (int)(bf - buf);
}

// cleanup
#undef STBSP__LEFTJUST
#undef STBSP__LEADINGPLUS
#undef STBSP__LEADINGSPACE
#undef STBSP__LEADING_0X
#undef STBSP__LEADINGZERO
#undef STBSP__INTMAX
#undef STBSP__TRIPLET_COMMA
#undef STBSP__NEGATIVE
#undef STBSP__METRIC_SUFFIX
#undef STBSP__NUMSZ
#undef stbsp__chk_cb_bufL
#undef stbsp__chk_cb_buf
#undef stbsp__flush_cb
#undef stbsp__cb_buf_clamp

// ============================================================================
//   wrapper functions

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(sprintf)(char *buf, char const *fmt, ...)
{
   int result;
   va_list va;
   va_start(va, fmt);
   result = STB_SPRINTF_DECORATE(vsprintfcb)(0, 0, buf, fmt, va);
   va_end(va);
   return result;
}

typedef struct stbsp__context {
   char *buf;
   int count;
   char tmp[STB_SPRINTF_MIN];
} stbsp__context;

static char *stbsp__clamp_callback(char *buf, void *user, int len)
{
   stbsp__context *c = (stbsp__context *)user;

   if (len > c->count)
      len = c->count;

   if (len) {
      if (buf != c->buf) {
         char *s, *d, *se;
         d = c->buf;
         s = buf;
         se = buf + len;
         do {
            *d++ = *s++;
         } while (s < se);
      }
      c->buf += len;
      c->count -= len;
   }

   if (c->count <= 0)
      return 0;
   return (c->count >= STB_SPRINTF_MIN) ? c->buf : c->tmp; // go direct into buffer if you can
}

static char * stbsp__count_clamp_callback( char * buf, void * user, int len )
{
   (void)buf;
   stbsp__context * c = (stbsp__context*)user;

   c->count += len;
   return c->tmp; // go direct into buffer if you can
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( vsnprintf )( char * buf, int count, char const * fmt, va_list va )
{
   stbsp__context c;
   int l;

   if ( (count == 0) && !buf )
   {
      c.count = 0;

      STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__count_clamp_callback, &c, c.tmp, fmt, va );
      l = c.count;
   }
   else
   {
      if ( count == 0 )
         return 0;

      c.buf = buf;
      c.count = count;

      STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__clamp_callback, &c, stbsp__clamp_callback(0,&c,0), fmt, va );

      // zero-terminate
      l = (int)( c.buf - buf );
      if ( l >= count ) // should never be greater, only equal (or less) than count
         l = count - 1;
      buf[l] = 0;
   }

   return l;
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...)
{
   int result;
   va_list va;
   va_start(va, fmt);

   result = STB_SPRINTF_DECORATE(vsnprintf)(buf, count, fmt, va);
   va_end(va);

   return result;
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintf)(char *buf, char const *fmt, va_list va)
{
   return STB_SPRINTF_DECORATE(vsprintfcb)(0, 0, buf, fmt, va);
}

// =======================================================================
//   low level float utility functions

#ifndef STB_SPRINTF_NOFLOAT

// copies d to bits w/ strict aliasing (this compiles to nothing on /Ox)
#define STBSP__COPYFP(dest, src)                   \
   {                                               \
      int cn;                                      \
      for (cn = 0; cn < 8; cn++)                   \
         ((char *)&dest)[cn] = ((char *)&src)[cn]; \
   }

// get float info
static stbsp__int32 stbsp__real_to_parts(stbsp__int64 *bits, stbsp__int32 *expo, double value)
{
   double d;
   stbsp__int64 b = 0;

   // load value and round at the frac_digits
   d = value;

   STBSP__COPYFP(b, d);

   *bits = b & ((((stbsp__uint64)1) << 52) - 1);
   *expo = (stbsp__int32)(((b >> 52) & 2047) - 1023);

   return (stbsp__int32)(b >> 63);
}

static double const stbsp__bot[23] = {
   1e+000, 1e+001, 1e+002, 1e+003, 1e+004, 1e+005, 1e+006, 1e+007, 1e+008, 1e+009, 1e+010, 1e+011,
   1e+012, 1e+013, 1e+014, 1e+015, 1e+016, 1e+017, 1e+018, 1e+019, 1e+020, 1e+021, 1e+022
};
static double const stbsp__negbot[22] = {
   1e-001, 1e-002, 1e-003, 1e-004, 1e-005, 1e-006, 1e-007, 1e-008, 1e-009, 1e-010, 1e-011,
   1e-012, 1e-013, 1e-014, 1e-015, 1e-016, 1e-017, 1e-018, 1e-019, 1e-020, 1e-021, 1e-022
};
static double const stbsp__negboterr[22] = {
   -5.551115123125783e-018,  -2.0816681711721684e-019, -2.0816681711721686e-020, -4.7921736023859299e-021, -8.1803053914031305e-022, 4.5251888174113741e-023,
   4.5251888174113739e-024,  -2.0922560830128471e-025, -6.2281591457779853e-026, -3.6432197315497743e-027, 6.0503030718060191e-028,  2.0113352370744385e-029,
   -3.0373745563400371e-030, 1.1806906454401013e-032,  -7.7705399876661076e-032, 2.0902213275965398e-033,  -7.1542424054621921e-034, -7.1542424054621926e-035,
   2.4754073164739869e-036,  5.4846728545790429e-037,  9.2462547772103625e-038,  -4.8596774326570872e-039
};
static double const stbsp__top[13] = {
   1e+023, 1e+046, 1e+069, 1e+092, 1e+115, 1e+138, 1e+161, 1e+184, 1e+207, 1e+230, 1e+253, 1e+276, 1e+299
};
static double const stbsp__negtop[13] = {
   1e-023, 1e-046, 1e-069, 1e-092, 1e-115, 1e-138, 1e-161, 1e-184, 1e-207, 1e-230, 1e-253, 1e-276, 1e-299
};
static double const stbsp__toperr[13] = {
   8388608,
   6.8601809640529717e+028,
   -7.253143638152921e+052,
   -4.3377296974619174e+075,
   -1.5559416129466825e+098,
   -3.2841562489204913e+121,
   -3.7745893248228135e+144,
   -1.7356668416969134e+167,
   -3.8893577551088374e+190,
   -9.9566444326005119e+213,
   6.3641293062232429e+236,
   -5.2069140800249813e+259,
   -5.2504760255204387e+282
};
static double const stbsp__negtoperr[13] = {
   3.9565301985100693e-040,  -2.299904345391321e-063,  3.6506201437945798e-086,  1.1875228833981544e-109,
   -5.0644902316928607e-132, -6.7156837247865426e-155, -2.812077463003139e-178,  -5.7778912386589953e-201,
   7.4997100559334532e-224,  -4.6439668915134491e-247, -6.3691100762962136e-270, -9.436808465446358e-293,
   8.0970921678014997e-317
};

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
static stbsp__uint64 const stbsp__powten[20] = {
   1,
   10,
   100,
   1000,
   10000,
   100000,
   1000000,
   10000000,
   100000000,
   1000000000,
   10000000000,
   100000000000,
   1000000000000,
   10000000000000,
   100000000000000,
   1000000000000000,
   10000000000000000,
   100000000000000000,
   1000000000000000000,
   10000000000000000000U
};
#define stbsp__tento19th ((stbsp__uint64)1000000000000000000)
#else
static stbsp__uint64 const stbsp__powten[20] = {
   1,
   10,
   100,
   1000,
   10000,
   100000,
   1000000,
   10000000,
   100000000,
   1000000000,
   10000000000ULL,
   100000000000ULL,
   1000000000000ULL,
   10000000000000ULL,
   100000000000000ULL,
   1000000000000000ULL,
   10000000000000000ULL,
   100000000000000000ULL,
   1000000000000000000ULL,
   10000000000000000000ULL
};
#define stbsp__tento19th (1000000000000000000ULL)
#endif

#define stbsp__ddmulthi(oh, ol, xh, yh)                            \
   {                                                               \
      double ahi = 0, alo, bhi = 0, blo;                           \
      stbsp__int64 bt;                                             \
      oh = xh * yh;                                                \
      STBSP__COPYFP(bt, xh);                                       \
      bt &= ((~(stbsp__uint64)0) << 27);                           \
      STBSP__COPYFP(ahi, bt);                                      \
      alo = xh - ahi;                                              \
      STBSP__COPYFP(bt, yh);                                       \
      bt &= ((~(stbsp__uint64)0) << 27);                           \
      STBSP__COPYFP(bhi, bt);                                      \
      blo = yh - bhi;                                              \
      ol = ((ahi * bhi - oh) + ahi * blo + alo * bhi) + alo * blo; \
   }

#define stbsp__ddtoS64(ob, xh, xl)          \
   {                                        \
      double ahi = 0, alo, vh, t;           \
      ob = (stbsp__int64)ph;                \
      vh = (double)ob;                      \
      ahi = (xh - vh);                      \
      t = (ahi - xh);                       \
      alo = (xh - (ahi - t)) - (vh + t);    \
      ob += (stbsp__int64)(ahi + alo + xl); \
   }

#define stbsp__ddrenorm(oh, ol) \
   {                            \
      double s;                 \
      s = oh + ol;              \
      ol = ol - (s - oh);       \
      oh = s;                   \
   }

#define stbsp__ddmultlo(oh, ol, xh, xl, yh, yl) ol = ol + (xh * yl + xl * yh);

#define stbsp__ddmultlos(oh, ol, xh, yl) ol = ol + (xh * yl);

static void stbsp__raise_to_power10(double *ohi, double *olo, double d, stbsp__int32 power) // power can be -323 to +350
{
   double ph, pl;
   if ((power >= 0) && (power <= 22)) {
      stbsp__ddmulthi(ph, pl, d, stbsp__bot[power]);
   } else {
      stbsp__int32 e, et, eb;
      double p2h, p2l;

      e = power;
      if (power < 0)
         e = -e;
      et = (e * 0x2c9) >> 14; /* %23 */
      if (et > 13)
         et = 13;
      eb = e - (et * 23);

      ph = d;
      pl = 0.0;
      if (power < 0) {
         if (eb) {
            --eb;
            stbsp__ddmulthi(ph, pl, d, stbsp__negbot[eb]);
            stbsp__ddmultlos(ph, pl, d, stbsp__negboterr[eb]);
         }
         if (et) {
            stbsp__ddrenorm(ph, pl);
            --et;
            stbsp__ddmulthi(p2h, p2l, ph, stbsp__negtop[et]);
            stbsp__ddmultlo(p2h, p2l, ph, pl, stbsp__negtop[et], stbsp__negtoperr[et]);
            ph = p2h;
            pl = p2l;
         }
      } else {
         if (eb) {
            e = eb;
            if (eb > 22)
               eb = 22;
            e -= eb;
            stbsp__ddmulthi(ph, pl, d, stbsp__bot[eb]);
            if (e) {
               stbsp__ddrenorm(ph, pl);
               stbsp__ddmulthi(p2h, p2l, ph, stbsp__bot[e]);
               stbsp__ddmultlos(p2h, p2l, stbsp__bot[e], pl);
               ph = p2h;
               pl = p2l;
            }
         }
         if (et) {
            stbsp__ddrenorm(ph, pl);
            --et;
            stbsp__ddmulthi(p2h, p2l, ph, stbsp__top[et]);
            stbsp__ddmultlo(p2h, p2l, ph, pl, stbsp__top[et], stbsp__toperr[et]);
            ph = p2h;
            pl = p2l;
         }
      }
   }
   stbsp__ddrenorm(ph, pl);
   *ohi = ph;
   *olo = pl;
}

// given a float value, returns the significant bits in bits, and the position of the
//   decimal point in decimal_pos.  +/-INF and NAN are specified by special values
//   returned in the decimal_pos parameter.
// frac_digits is absolute normally, but if you want from first significant digits (got %g and %e), or in 0x80000000
static stbsp__int32 stbsp__real_to_str(char const **start, stbsp__uint32 *len, char *out, stbsp__int32 *decimal_pos, double value, stbsp__uint32 frac_digits)
{
   double d;
   stbsp__int64 bits = 0;
   stbsp__int32 expo, e, ng, tens;

   d = value;
   STBSP__COPYFP(bits, d);
   expo = (stbsp__int32)((bits >> 52) & 2047);
   ng = (stbsp__int32)(bits >> 63);
   if (ng)
      d = -d;

   if (expo == 2047) // is nan or inf?
   {
      *start = (bits & ((((stbsp__uint64)1) << 52) - 1)) ? "NaN" : "Inf";
      *decimal_pos = STBSP__SPECIAL;
      *len = 3;
      return ng;
   }

   if (expo == 0) // is zero or denormal
   {
      if ((bits << 1) == 0) // do zero
      {
         *decimal_pos = 1;
         *start = out;
         out[0] = '0';
         *len = 1;
         return ng;
      }
      // find the right expo for denormals
      {
         stbsp__int64 v = ((stbsp__uint64)1) << 51;
         while ((bits & v) == 0) {
            --expo;
            v >>= 1;
         }
      }
   }

   // find the decimal exponent as well as the decimal bits of the value
   {
      double ph, pl;

      // log10 estimate - very specifically tweaked to hit or undershoot by no more than 1 of log10 of all expos 1..2046
      tens = expo - 1023;
      tens = (tens < 0) ? ((tens * 617) / 2048) : (((tens * 1233) / 4096) + 1);

      // move the significant bits into position and stick them into an int
      stbsp__raise_to_power10(&ph, &pl, d, 18 - tens);

      // get full as much precision from double-double as possible
      stbsp__ddtoS64(bits, ph, pl);

      // check if we undershot
      if (((stbsp__uint64)bits) >= stbsp__tento19th)
         ++tens;
   }

   // now do the rounding in integer land
   frac_digits = (frac_digits & 0x80000000) ? ((frac_digits & 0x7ffffff) + 1) : (tens + frac_digits);
   if ((frac_digits < 24)) {
      stbsp__uint32 dg = 1;
      if ((stbsp__uint64)bits >= stbsp__powten[9])
         dg = 10;
      while ((stbsp__uint64)bits >= stbsp__powten[dg]) {
         ++dg;
         if (dg == 20)
            goto noround;
      }
      if (frac_digits < dg) {
         stbsp__uint64 r;
         // add 0.5 at the right position and round
         e = dg - frac_digits;
         if ((stbsp__uint32)e >= 24)
            goto noround;
         r = stbsp__powten[e];
         bits = bits + (r / 2);
         if ((stbsp__uint64)bits >= stbsp__powten[dg])
            ++tens;
         bits /= r;
      }
   noround:;
   }

   // kill long trailing runs of zeros
   if (bits) {
      stbsp__uint32 n;
      for (;;) {
         if (bits <= 0xffffffff)
            break;
         if (bits % 1000)
            goto donez;
         bits /= 1000;
      }
      n = (stbsp__uint32)bits;
      while ((n % 1000) == 0)
         n /= 1000;
      bits = n;
   donez:;
   }

   // convert to string
   out += 64;
   e = 0;
   for (;;) {
      stbsp__uint32 n;
      char *o = out - 8;
      // do the conversion in chunks of U32s (avoid most 64-bit divides, worth it, constant denomiators be damned)
      if (bits >= 100000000) {
         n = (stbsp__uint32)(bits % 100000000);
         bits /= 100000000;
      } else {
         n = (stbsp__uint32)bits;
         bits = 0;
      }
      while (n) {
         out -= 2;
         *(stbsp__uint16 *)out = *(stbsp__uint16 *)&stbsp__digitpair[(n % 100) * 2];
         n /= 100;
         e += 2;
      }
      if (bits == 0) {
         if ((e) && (out[0] == '0')) {
            ++out;
            --e;
         }
         break;
      }
      while (out != o) {
         *--out = '0';
         ++e;
      }
   }

   *decimal_pos = tens;
   *start = out;
   *len = e;
   return ng;
}

#undef stbsp__ddmulthi
#undef stbsp__ddrenorm
#undef stbsp__ddmultlo
#undef stbsp__ddmultlos
#undef STBSP__SPECIAL
#undef STBSP__COPYFP

#endif // STB_SPRINTF_NOFLOAT

// clean up
#undef stbsp__uint16
#undef stbsp__uint32
#undef stbsp__int32
#undef stbsp__uint64
#undef stbsp__int64
#undef STBSP__UNALIGNED

#endif // STB_SPRINTF_IMPLEMENTATION

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/

#if defined(_MSC_VER)
    #if !defined(DQN_CRT_SECURE_NO_WARNINGS_PREVIOUSLY_DEFINED)
        #undef _CRT_SECURE_NO_WARNINGS
    #endif
#endif
