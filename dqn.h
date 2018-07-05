// Dqn.h Usage
// =================================================================================================
/*
    // Define this wherever you want access to DQN code that uses the platform.
    #define DQN_PLATFORM_HEADER         // Enable function prototypes for xplatform/platform code
    #define DQN_IMPLEMENTATION          // Enable the implementation
    #define DQN_PLATFORM_IMPLEMENTATION // Enable platform specific implementation on Win32 you must link against user32.lib and kernel32.lib
    #define DQN_MAKE_STATIC             // Make all functions be static
    #include "dqn.h"
 */

// Conventions
// All data structures fields are exposed by default, with exceptions here and there. The rationale
// is I rarely go into libraries and start changing values around in fields unless I know the
// implementation OR we're required to fill out a struct for some function.

// Just treat all struct fields to be internal and read-only unless explicitly stated otherwise.

// Table Of Contents #TOC #TableOfContents
// =================================================================================================
// You can search by #<entry> to jump straight to the section.
// The first match is the public API, the next matche(s) are the implementation

// #Portable Code
// #DqnSprintf     Cross-platform Sprintf Implementation (Public Domain lib stb_sprintf)
// #DqnAssert      Assertions and Logging
// #DqnSlice       Slices
// #DqnChar        Char  Operations (IsDigit(), IsAlpha() etc)
// #DqnStr         Str   Operations (Str_Len(), Str_Copy() etc)
// #DqnWChar       WChar Operations (IsDigit(), IsAlpha() etc)
// #DqnRnd         Random Number Generator (ints and floats)
// #Dqn_*          Utility code, (qsort, quick file reading)
// #DqnMem         Memory Allocation
// #DqnMemAPI      Custom memory API for Dqn Data Structures
// #DqnPool        Pool objects
// #DqnArray       Dynamic Array using Templates
// #DqnMemStack    Memory Allocator, Push, Pop Style
// #DqnHash        Hashing using Murmur
// #DqnMath        Simple Math Helpers (Lerp etc.)
// #DqnV2          2D  Math Vectors
// #DqnV3          3D  Math Vectors
// #DqnV4          4D  Math Vectors
// #DqnMat4        4x4 Math Matrix
// #DqnRect        Rectangles
// #DqnString      String library
// #DqnFixedString Fixed sized strings at compile time.
// #DqnHashTable   Hash Tables using Templates

// #XPlatform (Win32 & Unix)
// #DqnFile      File I/O (Read, Write, Delete)
// #DqnTimer     High Resolution Timer
// #DqnLock      Mutex Synchronisation
// #DqnJobQueue  Multithreaded Job Queue
// #DqnAtomic    Interlocks/Atomic Operations
// #DqnOS        Common Platform API helpers

// #Platform
//   - #DqnWin32     Common Win32 API Helpers

// TODO
// - DqnMemStack
//   - Allow 0 size memblock stack initialisation/block-less stack for situations where you don't
//     care about specifying a size upfront
//   - Default block size for when new blocks are required
//
// - Win32
//   - Get rid of reliance on MAX_PATH
//
// - Mbuildake lib compile and run on Linux with GCC using -03
// - Make DqnV* operations be static to class for consistency?

// Preprocessor Checks
// =================================================================================================
// This needs to be above the portable layer so  that, if the user requests a platform
// implementation, platform specific implementations in the portable layer will get activated.
#if (defined(_WIN32) || defined(_WIN64))
    #define DQN__IS_WIN32 1
#else
    #define DQN__IS_UNIX 1
#endif

#if defined(DQN_PLATFORM_IMPLEMENTATION)
    #define DQN__XPLATFORM_LAYER 1
#endif

// #Portable Code
// =================================================================================================
#ifndef DQN_H
#define DQN_H

#ifdef DQN_MAKE_STATIC
    #define DQN_FILE_SCOPE static
#else
    #define DQN_FILE_SCOPE
#endif

#include <stdint.h> // For standard types
#include <stddef.h> // For standard types
#include <string.h> // memmove
#include <stdarg.h> // va_list
#include <float.h>  // FLT_MAX
#define LOCAL_PERSIST static
#define FILE_SCOPE    static

using usize = size_t;
using isize = ptrdiff_t;

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8  = uint8_t;

using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8  = int8_t;

using b32 = i32;

using f64 = double;
using f32 = float;

#define DQN_F32_MIN   -FLT_MAX;
#define DQN_I64_MAX  INT64_MAX;
#define DQN_U64_MAX UINT64_MAX;

#define DQN_TERABYTE(val) (DQN_GIGABYTE(val) * 1024LL)

#define DQN_TERABYTE(val) (DQN_GIGABYTE(val) * 1024LL)
#define DQN_GIGABYTE(val) (DQN_MEGABYTE(val) * 1024LL)
#define DQN_MEGABYTE(val) (DQN_KILOBYTE(val) * 1024LL)
#define DQN_KILOBYTE(val) ((val) * 1024LL)

#define DQN_DAY_TO_S(val) ((DQN_HOUR_TO_S(val)) * 24)
#define DQN_HOUR_TO_S(val) ((DQN_MINUTE_TO_S(val)) * 60)
#define DQN_MINUTE_TO_S(val) ((val) * 60)

#define DQN_ALIGN_POW_N(val, align) ((((usize)val) + ((usize)align-1)) & (~(usize)(align-1)))
#define DQN_ALIGN_POW_4(val)        DQN_ALIGN_POW_N(val, 4)

#define DQN_INVALID_CODE_PATH 0
#define DQN_CHAR_COUNT(charArray) DQN_ARRAY_COUNT(charArray) - 1
#define DQN_ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

#define DQN_PI 3.14159265359f
#define DQN_SQUARED(x) ((x) * (x))
#define DQN_ABS(x) (((x) < 0) ? (-(x)) : (x))
#define DQN_DEGREES_TO_RADIANS(x) ((x * (DQN_PI / 180.0f)))
#define DQN_RADIANS_TO_DEGREES(x) ((x * (180.0f / DQN_PI)))

#define DQN_MAX(a, b) ((a) < (b) ? (b) : (a))
#define DQN_MIN(a, b) ((a) < (b) ? (a) : (b))
#define DQN_SWAP(type, a, b) do { type tmp = a; a = b; b = tmp; } while(0)

// NOTE: Directives don't get replaced if there's a stringify or paste (# or ##) so TOKEN_COMBINE2 is needed
//       to let directives get expanded (i.e. __COUNTER__), then we can combine.
#define DQN_TOKEN_COMBINE(x, y) x ## y
#define DQN_TOKEN_COMBINE2(x, y) DQN_TOKEN_COMBINE(x, y)

// Produce a unique name with prefix and counter. i.e. where prefix is "data" then it gives "data1"
#define DQN_UNIQUE_NAME(prefix) DQN_TOKEN_COMBINE2(prefix, __COUNTER__)
#define DQN_FOR_EACH(i, lim) for (isize (i) = 0; (i) < (isize)(lim); ++(i))

template <typename Proc>
struct DqnDefer__
{
     DqnDefer__(Proc p) : proc(p) { }
    ~DqnDefer__() { proc(); }
    Proc proc;
};

template <typename Proc>
DqnDefer__<Proc> DqnDeferFunc__(Proc p)
{
    return DqnDefer__<Proc>(p);
}

#define DQN_DEFER(code) auto DQN_UNIQUE_NAME(dqnDeferLambda__) = DqnDeferFunc__([&]()->void {code;})

// #Dqn Namespace
namespace Dqn
{
enum struct ZeroClear   { No = 0, Yes = 1};
enum struct IgnoreCase  { No = 0, Yes = 1};
FILE_SCOPE const bool IS_DEBUG = true;
}; // namespace Dqn

// #External Code
// =================================================================================================
#ifndef STB_SPRINTF_H_INCLUDE
#define STB_SPRINTF_H_INCLUDE
#define STB_SPRINTF_DECORATE(name) Dqn_##name

////////////////////////////////////////////////////////////////////////////////
// #DqnSprintf Public API - Cross-platform Sprintf Implementation
////////////////////////////////////////////////////////////////////////////////
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

// Convert a va_list arg list into a buffer. vsnprintf always returns a zero-terminated string (unlike regular snprintf).
// return: The number of characters copied into the buffer
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintf)(char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsnprintf)(char *buf, int count, char const *fmt, va_list va);

// Write format string to buffer. It always writes the whole string and appends a null.
// return: The number of characters copied into the buffer not including the null terminator.
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(sprintf)(char *buf, char const *fmt, ...);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...);

// snprintf() always returns a zero-terminated string (unlike regular snprintf).
// Convert into a buffer, calling back every STB_SPRINTF_MIN chars.
// Your callback can then copy the chars out, print them or whatever.
// This function is actually the workhorse for everything else.
// The buffer you pass in must hold at least STB_SPRINTF_MIN characters.
// You return the next buffer to use or 0 to stop converting
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va);

// Set the comma and period characters to use.
STBSP__PUBLICDEF void STB_SPRINTF_DECORATE(set_separators)(char comma, char period);
#endif // STB_SPRINTF_H_INCLUDE

// #DqnAssert API
// =================================================================================================
// NOTE: "## __VA_ARGS__" is a GCC hack. Zero variadic arguments won't compile
// because there will be a trailing ',' at the end. Pasting it swallows it. MSVC
// implicitly swallows the trailing comma.

#define DQN_ASSERT(expr) DQN_ASSERTM(expr, "");
#define DQN_ASSERTM(expr, msg, ...)                                                                \
    do                                                                                             \
    {                                                                                              \
        if (!(expr))                                                                               \
        {                                                                                          \
            DqnLogExpr(__FILE__, __func__, __LINE__, #expr, msg, ## __VA_ARGS__);                  \
            (*((int *)0)) = 0;                                                                     \
        }                                                                                          \
    } while (0)

#define DQN_LOGE(msg, ...) DqnLog(__FILE__, __func__, __LINE__, msg, ## __VA_ARGS__);
#define DQN_LOGD(msg, ...) DqnLog(__FILE__, __func__, __LINE__, msg, ## __VA_ARGS__);

// Use macro above.
DQN_FILE_SCOPE void DqnLog    (char const *file, char const *functionName, i32 const lineNum, char const *msg, ...);
DQN_FILE_SCOPE void DqnLogExpr(char const *file, char const *functionName, i32 const lineNum, char const *expr, char const *msg, ...);

// Assert at compile time by making a type that is invalid depending on the expression result
#define DQN_COMPILE_ASSERT(expr)                DQN_COMPILE_ASSERT_INTERNAL(expr, DQN_UNIQUE_NAME(DqnCompileAssertInternal_))
#define DQN_COMPILE_ASSERT_INTERNAL(expr, name) typedef char name[((int)(expr)) * 2 - 1];

DQN_COMPILE_ASSERT(sizeof(isize) == sizeof(usize));

// #DqnSlice API
// =================================================================================================
template <typename T>
struct DqnSlice
{
    T   *data;
    i32  len;

    DqnSlice() = default;
    DqnSlice(T *str, i32 len) { this->data = str; this->len = len; }
};
#define DQN_SLICE(literal) DqnSlice<char const>(literal, DQN_CHAR_COUNT(literal))

#define DQN_SLICE_CMP(a, b, ignoreCase)  (a.len == b.len && (DqnStr_Cmp(a.data, b.data, a.len, ignoreCase) == 0))
// #DqnChar API
// =================================================================================================
DQN_FILE_SCOPE char DqnChar_ToLower     (char c);
DQN_FILE_SCOPE char DqnChar_ToUpper     (char c);
DQN_FILE_SCOPE bool DqnChar_IsDigit     (char c);
DQN_FILE_SCOPE bool DqnChar_IsAlpha     (char c);
DQN_FILE_SCOPE bool DqnChar_IsAlphaNum  (char c);
DQN_FILE_SCOPE bool DqnChar_IsEndOfLine (char c);
DQN_FILE_SCOPE bool DqnChar_IsWhitespace(char c);

DQN_FILE_SCOPE char *DqnChar_TrimWhitespaceAround(char const *src, i32 srcLen, i32 *newLen);
DQN_FILE_SCOPE char *DqnChar_SkipWhitespace      (char const *ptr);

// TODO(doyle): this is NOT UTF8 safe
// ch:        Char to find
// len:       The length of the string stored in ptr, (doesn't care if it includes null terminator)
// lenToChar: The length to the char from end of the ptr, i.e. (ptr + len)
// return:    The ptr to the last char, null if it could not find.
DQN_FILE_SCOPE char *DqnChar_FindLastChar  (char *ptr, char ch, i32 len, u32 *lenToChar);

// Finds up to the first [\r]\n and destroy the line, giving you a null terminated line at the newline.
// returns: The value to advance the ptr by, this can be different from the line
//          length if there are new lines or leading whitespaces in the next line
DQN_FILE_SCOPE i32   DqnChar_FindNextLine(char *ptr, i32 *lineLength);
DQN_FILE_SCOPE char *DqnChar_GetNextLine (char *ptr, i32 *lineLength);

// #DqnStr API
// =================================================================================================
// numBytesToCompare: If -1, cmp runs until \0 is encountered.
// return:            0 if equal. 0 < if a is before b, > 0 if a is after b
DQN_FILE_SCOPE        i32            DqnStr_Cmp                   (char const *a, char const *b, i32 numBytesToCompare = -1, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No);

// strLen: Len of string, if -1, StrLen is used.
// return: Pointer in str to the last slash, if none then the original string.
DQN_FILE_SCOPE        char          *DqnStr_GetPtrToLastSlash     (char const *str, i32 strLen = -1);

// return: String length not including the nullptr terminator. 0 if invalid args.
DQN_FILE_SCOPE        i32            DqnStr_Len                   (char const *a);
DQN_FILE_SCOPE        i32            DqnStr_LenUTF8               (u32 const *a, i32 *lenInBytes = nullptr);

// return: String length starting from a, up to and not including the first delimiter character.
DQN_FILE_SCOPE        i32            DqnStr_LenDelimitWith        (char const *a, char delimiter);

// return: The dest argument, nullptr if args invalid (i.e. nullptr pointers or numChars < 0)
DQN_FILE_SCOPE        void           DqnStr_Reverse               (char *buf, isize bufSize);

// return: Number of bytes in codepoint, 0 if *a becomes invalid or end of stream.
DQN_FILE_SCOPE        i32            DqnStr_ReadUTF8Codepoint     (u32 const *a, u32 *outCodepoint);

// return: The offset into the src to first char of the found string. Returns -1 if not found
DQN_FILE_SCOPE        i32            DqnStr_FindFirstOccurence    (char const *src, i32 srcLen, char const *find, i32 findLen, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No);
DQN_FILE_SCOPE        bool           DqnStr_EndsWith              (char const *src, i32 srcLen, char const *find, i32 findLen, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No);

// return: Helper function that returns the pointer to the first occurence, nullptr if not found.
DQN_FILE_SCOPE        char          *DqnStr_GetFirstOccurence     (char const *src, i32 srcLen, char const *find, i32 findLen, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No);
DQN_FILE_SCOPE        bool           DqnStr_HasSubstring          (char const *src, i32 srcLen, char const *find, i32 findLen, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No);

DQN_FILE_SCOPE        DqnSlice<char> DqnStr_RemoveLeadTrailChar   (char const *str, i32 strLen, char leadChar, char trailChar);
DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailQuotes (DqnSlice<char> slice);
DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailBraces (DqnSlice<char> slice);
DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailQuotes (char const *str, i32 strLen);
DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailBraces (char const *str, i32 strLen);


#define DQN_32BIT_NUM_MAX_STR_SIZE 11
#define DQN_64BIT_NUM_MAX_STR_SIZE 21
// Return the len of the derived string. If buf is nullptr and or bufSize is 0 the function returns the
// required string length for the integer
// TODO NOTE(doyle): Parsing stops when a non-digit is encountered, so numbers with ',' don't work atm.
DQN_FILE_SCOPE        i32 Dqn_I64ToStr(i64 const value, char *buf, i32 bufSize);
DQN_FILE_SCOPE        i64 Dqn_StrToI64(char const *buf, i64 bufSize);
DQN_FILE_SCOPE inline i64 Dqn_StrToI64(DqnSlice<char const> buf) { return Dqn_StrToI64(buf.data, buf.len); }
DQN_FILE_SCOPE inline i64 Dqn_StrToI64(DqnSlice<char> buf)       { return Dqn_StrToI64(buf.data, buf.len); }

// WARNING: Not robust, precision errors and whatnot but good enough!
DQN_FILE_SCOPE f32 Dqn_StrToF32(char const *buf, i64 bufSize);

// Both return the number of bytes read, return 0 if invalid codepoint or UTF8
DQN_FILE_SCOPE u32 Dqn_UCSToUTF8(u32 *dest, u32 character);
DQN_FILE_SCOPE u32 Dqn_UTF8ToUCS(u32 *dest, u32 character);

// #DqnWChar API
// =================================================================================================
// NOTE: See above for documentation
DQN_FILE_SCOPE bool     DqnWChar_IsDigit          (wchar_t c);
DQN_FILE_SCOPE wchar_t  DqnWChar_ToLower          (wchar_t c);

DQN_FILE_SCOPE wchar_t *DqnWChar_SkipWhitespace   (wchar_t *ptr);
DQN_FILE_SCOPE wchar_t *DqnWChar_FindLastChar     (wchar_t *ptr, wchar_t ch, i32 len, u32 *lenToChar);
DQN_FILE_SCOPE i32      DqnWChar_GetNextLine      (wchar_t *ptr, i32 *lineLength);

DQN_FILE_SCOPE i32      DqnWStr_Cmp               (wchar_t const *a, wchar_t const *b);
DQN_FILE_SCOPE i32      DqnWStr_FindFirstOccurence(wchar_t const *src, i32 srcLen, wchar_t const *find, i32 findLen);
DQN_FILE_SCOPE bool     DqnWStr_HasSubstring      (wchar_t const *src, i32 srcLen, wchar_t const *find, i32 findLen);
DQN_FILE_SCOPE i32      DqnWStr_Len               (wchar_t const *a);
DQN_FILE_SCOPE i32      DqnWStr_LenDelimitWith    (wchar_t const *a, wchar_t delimiter);
DQN_FILE_SCOPE void     DqnWStr_Reverse           (wchar_t *buf, i32 bufSize);

DQN_FILE_SCOPE i32      Dqn_WStrToI32             (wchar_t const *buf, i32 bufSize);
DQN_FILE_SCOPE i32      Dqn_I32ToWStr             (i32 value, wchar_t *buf, i32 bufSize);

// #DqnRnd API
// =================================================================================================
struct DqnRndPCG // PCG (Permuted Congruential Generator)
{
    u64 state[2];

    DqnRndPCG();
    DqnRndPCG(u32 seed);

    u32 Next ();                 // return: A random number  N between [0, 0xFFFFFFFF]
    f32 Nextf();                 // return: A random float   N between [0.0, 1.0f]
    i32 Range(i32 min, i32 max); // return: A random integer N between [min, max]
};

// #Dqn_* API
// =================================================================================================
// return: The number of splits in the array. If array is null this returns the required size of the array.
i32 Dqn_SplitString(char const *src, i32 srcLen, char splitChar, DqnSlice<char> *array = nullptr, i32 size = 0);

// Util function that uses Dqn_SplitString
// return: The number of splits, splitting by "splitChar" would generate.
i32 Dqn_GetNumSplits(char const *src, i32 srcLen, char splitChar);

DQN_FILE_SCOPE inline bool Dqn_BitIsSet (u32 bits, u32 flag);
DQN_FILE_SCOPE inline u32  Dqn_BitSet   (u32 bits, u32 flag);
DQN_FILE_SCOPE inline u32  Dqn_BitUnset (u32 bits, u32 flag);
DQN_FILE_SCOPE inline u32  Dqn_BitToggle(u32 bits, u32 flag);

template <typename T>
using Dqn_QuickSortLessThanCallback = bool (*)(T const *, T const *, void *);

DQN_FILE_SCOPE inline bool Dqn_QuickSortDqnStringSorter(struct DqnString const *a, struct DqnString const *b, void *);

template <typename T>
DQN_FILE_SCOPE void Dqn_QuickSort(T *array, i64 size,
                                  Dqn_QuickSortLessThanCallback<T> IsLessThan, void *userContext = nullptr)
{
    if (!array || size <= 1 || !IsLessThan) return;

#if 1
    // Insertion Sort, under 24->32 is an optimal amount
    const i32 QUICK_SORT_THRESHOLD = 24;
    if (size < QUICK_SORT_THRESHOLD)
    {
        i32 itemToInsertIndex = 1;
        while (itemToInsertIndex < size)
        {
            for (i32 checkIndex = 0; checkIndex < itemToInsertIndex; checkIndex++)
            {
                if (!IsLessThan(&array[checkIndex], &array[itemToInsertIndex], userContext))
                {
                    T itemToInsert = array[itemToInsertIndex];
                    for (i32 i   = itemToInsertIndex; i > checkIndex; i--)
                        array[i] = array[i - 1];

                    array[checkIndex] = itemToInsert;
                    break;
                }
            }
            itemToInsertIndex++;
        }

        return;
    }
#endif

    auto state          = DqnRndPCG();
    auto lastIndex      = size - 1;
    auto pivotIndex     = (i64)state.Range(0, (i32)lastIndex);
    auto partitionIndex = 0;
    auto startIndex     = 0;

    // Swap pivot with last index, so pivot is always at the end of the array.
    // This makes logic much simpler.
    DQN_SWAP(T, array[lastIndex], array[pivotIndex]);
    pivotIndex = lastIndex;

    // 4^, 8, 7, 5, 2, 3, 6
    if (IsLessThan(&array[startIndex], &array[pivotIndex], userContext)) partitionIndex++;
    startIndex++;

    // 4, |8, 7, 5^, 2, 3, 6*
    // 4, 5, |7, 8, 2^, 3, 6*
    // 4, 5, 2, |8, 7, ^3, 6*
    // 4, 5, 2, 3, |7, 8, ^6*
    for (auto checkIndex = startIndex; checkIndex < lastIndex; checkIndex++)
    {
        if (IsLessThan(&array[checkIndex], &array[pivotIndex], userContext))
        {
            DQN_SWAP(T, array[partitionIndex], array[checkIndex]);
            partitionIndex++;
        }
    }

    // Move pivot to right of partition
    // 4, 5, 2, 3, |6, 8, ^7*
    DQN_SWAP(T, array[partitionIndex], array[pivotIndex]);
    Dqn_QuickSort(array, partitionIndex, IsLessThan, userContext);

    // Skip the value at partion index since that is guaranteed to be sorted.
    // 4, 5, 2, 3, (x), 8, 7
    i32 oneAfterPartitionIndex = partitionIndex + 1;
    Dqn_QuickSort(array + oneAfterPartitionIndex, (size - oneAfterPartitionIndex), IsLessThan, userContext);
}

template <typename T>
DQN_FILE_SCOPE void Dqn_QuickSort(T *array, i64 size)
{
    if (!array || size <= 1) return;

#if 1
    // Insertion Sort, under 24->32 is an optimal amount
    const i32 QUICK_SORT_THRESHOLD = 24;
    if (size < QUICK_SORT_THRESHOLD)
    {
        i32 itemToInsertIndex = 1;
        while (itemToInsertIndex < size)
        {
            for (i32 checkIndex = 0; checkIndex < itemToInsertIndex; checkIndex++)
            {
                if (!(array[checkIndex] < array[itemToInsertIndex]))
                {
                    T itemToInsert = array[itemToInsertIndex];
                    for (i32 i   = itemToInsertIndex; i > checkIndex; i--)
                        array[i] = array[i - 1];

                    array[checkIndex] = itemToInsert;
                    break;
                }
            }
            itemToInsertIndex++;
        }

        return;
    }
#endif

    auto state          = DqnRndPCG();
    auto lastIndex      = size - 1;
    auto pivotIndex     = (i64)state.Range(0, (i32)lastIndex); // TODO(doyle): RNG 64bit
    auto partitionIndex = 0;
    auto startIndex     = 0;

    // Swap pivot with last index, so pivot is always at the end of the array.
    // This makes logic much simpler.
    DQN_SWAP(T, array[lastIndex], array[pivotIndex]);
    pivotIndex = lastIndex;

    // 4^, 8, 7, 5, 2, 3, 6
    if (array[startIndex] < array[pivotIndex]) partitionIndex++;
    startIndex++;

    // 4, |8, 7, 5^, 2, 3, 6*
    // 4, 5, |7, 8, 2^, 3, 6*
    // 4, 5, 2, |8, 7, ^3, 6*
    // 4, 5, 2, 3, |7, 8, ^6*
    for (auto checkIndex = startIndex; checkIndex < lastIndex; checkIndex++)
    {
        if (array[checkIndex] < array[pivotIndex])
        {
            DQN_SWAP(T, array[partitionIndex], array[checkIndex]);
            partitionIndex++;
        }
    }

    // Move pivot to right of partition
    // 4, 5, 2, 3, |6, 8, ^7*
    DQN_SWAP(T, array[partitionIndex], array[pivotIndex]);
    Dqn_QuickSort(array, partitionIndex);

    // Skip the value at partion index since that is guaranteed to be sorted.
    // 4, 5, 2, 3, (x), 8, 7
    i32 oneAfterPartitionIndex = partitionIndex + 1;
    Dqn_QuickSort(array + oneAfterPartitionIndex, (size - oneAfterPartitionIndex));
}

template <typename T>
using Dqn_BSearchLessThanCallback = bool (*)(const T&, const T&);

template <typename T>
using Dqn_BSearchEqualsCallback   = bool (*)(const T&, const T&);

enum Dqn_BSearchBound
{
    Dqn_BSearchBound_Normal, // Return the index of the first item that matches the find value
    Dqn_BSearchBound_Lower,  // Return the index of the first item lower than the find value
    Dqn_BSearchBound_Higher, // Return the index of the first item higher than the find value

    Dqn_BSearchBound_NormalLower,  // Return the index of the matching item if not found the first item lower
    Dqn_BSearchBound_NormalHigher, // Return the index of the matching item if not found the first item higher
};

// bound: The behaviour of the binary search,
// return: -1 if element not found, otherwise index of the element.
//         For higher and lower bounds return -1 if there is no element higher/lower than the
//         find value (i.e. -1 if the 0th element is the find val for lower bound).
template <typename T>
DQN_FILE_SCOPE i64 Dqn_BSearch(T *array, i64 size, T const &find,
                               Dqn_BSearchEqualsCallback<T> Equals,
                               Dqn_BSearchLessThanCallback<T> IsLessThan,
                               Dqn_BSearchBound bound = Dqn_BSearchBound_Normal)
{
    if (size == 0 || !array) return -1;

    i64 start = 0;
    i64 end   = size - 1;
    i64 mid   = (i64)((start + end) * 0.5f);

    while (start <= end)
    {
        if (Equals(array[mid], find))
        {
            if (bound == Dqn_BSearchBound_Normal ||
                bound == Dqn_BSearchBound_NormalLower ||
                bound == Dqn_BSearchBound_NormalHigher)
            {
                return mid;
            }
            else if (bound == Dqn_BSearchBound_Lower)
            {
                // NOTE: We can always -1 because at worst case, 0 index will go to -1 which is
                // correct behaviour.
                return mid - 1;
            }
            else
            {
                if ((mid + 1) >= size) return -1;
                return mid + 1;
            }
        }
        else if (IsLessThan(array[mid], find)) start = mid + 1;
        else                                   end   = mid - 1;
        mid = (i64)((start + end) * 0.5f);
    }

    if (bound == Dqn_BSearchBound_Normal)
    {
        return -1;
    }
    if (bound == Dqn_BSearchBound_Lower || bound == Dqn_BSearchBound_NormalLower)
    {
        if (IsLessThan(find, array[mid])) return -1;
        return mid;
    }
    else
    {
        if (IsLessThan(array[mid], find)) return -1;
        return mid;
    }
}

DQN_FILE_SCOPE i64 Dqn_BSearch(i64 *array, i64 size, i64 find, Dqn_BSearchBound bound = Dqn_BSearchBound_Normal);

// #DqnMem API
// =================================================================================================
// TODO(doyle): Use platform allocation, fallback to malloc if platform not defined
DQN_FILE_SCOPE void *DqnMem_Alloc  (usize size);
DQN_FILE_SCOPE void *DqnMem_Calloc (usize size);
DQN_FILE_SCOPE void  DqnMem_Clear  (void *memory, u8 clearValue, usize size);
DQN_FILE_SCOPE void *DqnMem_Realloc(void *memory, usize newSize);
DQN_FILE_SCOPE void  DqnMem_Free   (void *memory);
DQN_FILE_SCOPE void  DqnMem_Copy   (void *dest, void const *src, usize numBytesToCopy);
DQN_FILE_SCOPE void *DqnMem_Set    (void *dest, u8 value,        usize numBytesToSet);

// #DqnMemAPI API
// =================================================================================================
// You only need to care about this API if you want to use custom mem-alloc routines in the data
// structures! Otherwise it already has a default one to use.

// How To Use:
// 1. Implement the allocator function, where DqnMemAPI::Request will tell you the request.
//    - (NOTE) The callback should return the resulting data into DqnMemAPI::Result
// 2. Create a DqnMemAPI struct with a function ptr to your allocator
//    - (OPTIONAL) Set the user context to your book-keeping/mem allocating service
// 3. Initialise any data structure that supports a DqnMemAPI with your struct.

// That's it! Done :) Of course, changing memAPI's after initialisation is invalid since the
// pointers belonging to your old routine may not be tracked in your new memAPI. So you're at your
// own discretion there.

class DqnMemAPI
{
public:
    enum struct Type
    {
        Uninitialised,
        Alloc,
        Calloc,
        Realloc,
        Free
    };

    struct Request
    {
        void *userContext;
        Type type;
        union
        {
            struct Alloc_
            {
                bool zeroClear;
                isize requestSize;
            } alloc;

            struct Free_
            {
                void  *ptrToFree;
                isize sizeToFree;
            } free;

            struct Realloc_
            {
                isize newSize;
                isize oldSize;
                void *oldMemPtr;
            } realloc;
        } e;
    };

    typedef u8 *Allocator(DqnMemAPI *, DqnMemAPI::Request);

    Allocator *allocator;
    void      *userContext;

    isize     bytesAllocated;
    isize     lifetimeBytesAllocated;
    isize     lifetimeBytesFreed;

    static DqnMemAPI HeapAllocator ();

    enum struct StackPushType
    {
        Head,
        Tail,
    };
    static DqnMemAPI StackAllocator(struct DqnMemStack *stack, StackPushType type = StackPushType::Head);

    void *Realloc(void  *oldPtr,    isize oldSize, isize newSize);
    void *Alloc  (isize  size,      Dqn::ZeroClear clear = Dqn::ZeroClear::Yes);
    void  Free   (void  *ptrToFree, isize sizeToFree = 0); // TODO(doyle): sizeToFree opt is iffy
    bool  IsValid() const { return (this->allocator != nullptr); }
};

// #DqnPool API
// =================================================================================================
template <typename T, i16 SIZE>
struct DqnPool
{
    struct Entry : public T
    {
        u16 nextIndex;
    };

    const static isize SENTINEL_INDEX = SIZE;

    Entry  pool[SIZE];
    i16    freeIndex;
    i16    numFree;

    DqnPool() : freeIndex(0) , numFree(SIZE)
    {
        DQN_FOR_EACH(i, SIZE - 1)
        {
            Entry *entry     = pool + i;
            entry->nextIndex = i + 1;
        }
        Entry *last     = pool + (SIZE - 1);
        last->nextIndex = SENTINEL_INDEX;
    }

    T *GetNext()
    {
        if (freeIndex == SENTINEL_INDEX) return nullptr;
        Entry *result = pool + freeIndex;
        freeIndex     = result->nextIndex;
        numFree--;
        return result;
    }

    void Return(T *item)
    {
        auto *entry      = reinterpret_cast<Entry *>(item);
        entry->nextIndex = freeIndex;
        freeIndex        = entry - pool;
        numFree++;
    }
};

FILE_SCOPE DqnMemAPI DQN_DEFAULT_HEAP_ALLOCATOR_ = DqnMemAPI::HeapAllocator();
FILE_SCOPE DqnMemAPI *DQN_DEFAULT_HEAP_ALLOCATOR = &DQN_DEFAULT_HEAP_ALLOCATOR_;

// #DqnArray API
// =================================================================================================
template<typename T>
struct DqnArray
{
    DqnMemAPI                  *memAPI = DQN_DEFAULT_HEAP_ALLOCATOR;
    isize                       count;
    isize                       max;
    T                          *data;

     DqnArray        () = default;
     DqnArray        (DqnMemAPI *memAPI_)                         { *this = {}; this->memAPI = memAPI_; }
    ~DqnArray        ()                                           { if (this->data && this->memAPI) this->memAPI->Free(data); }

    void  UseMemory  (T *data_, isize max_, isize count_ = 0)     { this->memAPI = nullptr; this->data = data_; this->max = max_; this->count = count_; }
    void  Clear      (Dqn::ZeroClear clear = Dqn::ZeroClear::Yes) { if (data) { count = 0; if (clear == Dqn::ZeroClear::Yes) DqnMem_Clear(data, 0, sizeof(T) * max); } }
    void  Free       ()                                           { if (data) { memAPI->Free(data); } *this = {}; }
    T    *Front      ()                                           { DQN_ASSERT(count > 0); return data + 0; }
    T    *Back       ()                                           { DQN_ASSERT(count > 0); return data + (count - 1); }
    void  Resize     (isize newCount)                             { if (newCount > max) Reserve(GrowCapacity_(newCount)); count = newCount; }
    void  Resize     (isize newCount, T const *v)                 { if (newCount > max) Reserve(GrowCapacity_(newCount)); if (newCount > count) for (isize n = count; n < newCount; n++) data[n] = *v; count = newCount; }
    void  Reserve    (isize newMax);
    T    *Push       (T const &v)                                 { return Insert(count, &v, 1); }
    T    *Push       (T const *v, isize numItems = 1)             { return Insert(count,  v, numItems); }
    void  Pop        ()                                           { if (count > 0) count--; }
    void  Erase      (isize index)                                { DQN_ASSERT(index >= 0 && index < count); data[index] = data[--count]; }
    void  EraseStable(isize index);
    T    *Insert     (isize index, T const *v)                    { return Insert(index,  v, 1); }
    T    *Insert     (isize index, T const &v)                    { return Insert(index, &v, 1); }
    T    *Insert     (isize index, T const *v, isize numItems);
    bool  Contains   (T const *v) const                           { T const *ptr = data;  T const *end = data + count; while (ptr < end) if (*ptr++ == *v) return true; return false; }

    T    &operator[] (isize i) const                              { DQN_ASSERT(i < count && i > 0); return this->data[i]; }
    T    *begin      ()                                           { return data; }
    T    *end        ()                                           { return data + count; }

private:
    isize GrowCapacity_(isize size) const                         { isize newMax = max ? (max * 2) : 8; return newMax > size ? newMax : size; }
};

template<typename T> T *DqnArray<T>::Insert(isize index, T const *v, isize numItems)
{
    index                = DQN_MIN(DQN_MAX(index, 0), count);
    isize const newCount = count + numItems;

    if (newCount >= max)
      Reserve(GrowCapacity_(newCount));

    T *src  = data + index;
    T *dest = src + numItems;

    if (src < dest)
        memmove(dest, src, ((data + count) - src) * sizeof(T));

    count = newCount;
    for (isize i = 0; i < numItems; i++)
        src[i] = v[i];

    return src;
}

template <typename T> void DqnArray<T>::EraseStable(isize index)
{
    DQN_ASSERT(index >= 0 && index < count);
    isize const off = (data + index) - data;
    memmove(data + off, data + off + 1, ((usize)count - (usize)off - 1) * sizeof(T));
    count--;
}

template <typename T> void DqnArray<T>::Reserve(isize newMax)
{
    if (newMax <= max) return;

    if (data)
    {
        T *newData = (T *)memAPI->Realloc(data, max * sizeof(T), newMax * sizeof(T));
        data = newData;
        max  = newMax;
    }
    else
    {
      data = (T *)memAPI->Alloc(newMax * sizeof(T));
      max  = newMax;
    }

    DQN_ASSERT(data);
}

// #DqnAllocatorMetadata
// =================================================================================================
// Allocation Layout
//                      +------------------------------------------------------------+                        +-----------------+
//                      | Allocation Head                                            |                        | Allocation Tail |
// +--------------------+------------------------------------------------------------+------------------------+-----------------+
// | Ptr From Allocator | Offset To Src | Alignment | Alloc Amount | B. Guard (Opt.) | Aligned Ptr For Client | B. Guard (Opt.) |
// +----------------------------------------------------------------------------------------------------------------------------+
// Ptr From Allocator:     The pointer returned by the allocator, not aligned
// Alignment:              The pointer given to the client is aligned to this power of two boundary
// Alloc Amount:           The requested allocation amount by the client (so does not include metadata)
// B. Guard:               Bounds Guard value.
// Aligned Ptr For Client: The allocated memory for the client.
// Offset To Src:          Number of bytes to subtract from the "Aligned Ptr For Client" to return to the "Ptr From ALlocator"
struct DqnAllocatorMetadata
{
    static u32 const HEAD_GUARD_VALUE   = 0xCAFEBABE;
    static u32 const TAIL_GUARD_VALUE   = 0xDEADBEEF;
    static u32 const OFFSET_TO_SRC_SIZE = sizeof(u8);
    static u32 const ALIGNMENT_SIZE     = sizeof(u8);
    static u32 const ALLOC_AMOUNT_SIZE  = sizeof(usize);

    DqnArray<u8 *> allocations; // When BoundsGuard is enabled, tracks all allocations.

    void   Init                (bool boundsGuard);
    void   Free                ();
    void   AddAllocation       (u8 *ptr)                        { DQN_ASSERT(allocations.Push(ptr) != nullptr); }
    void   RemoveAllocation    (u8 *ptr);
    void   CheckAllocations    ()                         const;

    auto   GetBoundsGuardSize  ()                         const { return boundsGuardSize; }
    auto   GetAllocHeadSize    ()                         const { return allocHeadSize; }
    auto   GetAllocTailSize    ()                         const { return allocTailSize; }

    isize  GetAllocationSize   (isize size, u8 alignment) const { return GetAllocHeadSize() + size + GetAllocTailSize() + (alignment - 1); }

    u32   *PtrToHeadBoundsGuard(u8 const *ptr)            const; // ptr: The ptr given to the client when allocating.
    u32   *PtrToTailBoundsGuard(u8 const *ptr)            const; // IMPORTANT: Uses "Alloc Amount" metadata to find the tail!
    u8    *PtrToAlignment      (u8 const *ptr)            const;
    u8    *PtrToOffsetToSrc    (u8 const *ptr)            const;
    isize *PtrToAllocAmount    (u8 const *ptr)            const;

private:
    u32    boundsGuardSize; // sizeof(GUARD_VALUE) OR 0 if BoundsGuard is disabled.
    u32    allocHeadSize;   // Bounds Guard Size + Offset To Src Size + Alloc Amount Size
    u32    allocTailSize;   // Bounds Guard Size
};

// #DqnMemStack API
// =================================================================================================
// DqnMemStack is an memory allocator in a stack like, push-pop style. It pre-allocates a block of
// memory at init and sub-allocates from this block to take advantage of memory locality.

// When an allocation requires a larger amount of memory than available in the block then the
// MemStack will allocate a new block of sufficient size for you in DqnMemStack_Push(..). This DOES
// mean that there will be wasted spaceat the end of each block and is a tradeoff for memory
// locality against optimal space usage.

// How To Use:
// DqnMemStack stack = {};
//             stack.Init(DQN_MEGABYTE(1), true, DqnMemStack::Flag::BoundsGuard);
// u8 *data =  stack.Push(128);

#define DQN_MEM_STACK_PUSH_STRUCT(memStack, Type, num) (Type *)(memStack)->Push(sizeof(Type) * num)
struct DqnMemStack
{
    static const i32 MINIMUM_BLOCK_SIZE = DQN_KILOBYTE(64);

    enum Flag
    {
        NonExpandable       = (1 << 0), // Disallow additional memory blocks when full.
        NonExpandableAssert = (1 << 1), // Assert when NonExpandable is set with allocation on a full stack.
        BoundsGuard         = (1 << 2), // Track, check and add guards on all allocations
        PushAssertsOnFail   = (1 << 3), // Assert when push*() fails.
        All                 = (NonExpandable | NonExpandableAssert | BoundsGuard | PushAssertsOnFail),
    };

    struct Info // Statistics of the memory stack.
    {
        isize totalUsed;
        isize totalSize;
        isize wastedSize;
        i32   numBlocks;
    };

    // Blocks are freely modifiable if you want fine grained control. Size value and memory ptr should
    // NOT be modified directly, only indirectly through the regular API.
    struct Block
    {
        u8    *memory;
        isize  size;      // Read-Only
        isize  used_;     // Read/Write at your peril.
        Block *prevBlock; // Uses a linked list approach for additional blocks

        u8 *head;
        u8 *tail;
    };

    DqnAllocatorMetadata  metadata;
    DqnMemAPI            *memAPI;          // API used to add additional memory blocks to this stack.
    DqnMemAPI             myTailAPI;       // API for data structures to allocate to the tail of the stack
    DqnMemAPI             myHeadAPI;       // API for data structures to allocate to the head of the stack
    Block                *block;           // Memory block allocated for the stack
    u32                   flags;
    i32                   tempRegionCount;

    // Initialisation API
    // =============================================================================================
    // Uses fixed buffer, allocations will be sourced from the buffer and fail after buffer is full.
    // mem:       Memory to use for the memory stack
    // return:    FALSE if args are invalid, or insufficient memSize.
    bool Init(void *mem, isize size, Dqn::ZeroClear clear, u32 flags_ = 0);

    // Memory Stack capable of expanding when full, but only if NonExpandable flag is not set.
    bool Init(isize size, Dqn::ZeroClear clear, u32 flags_ = 0, DqnMemAPI *api = DQN_DEFAULT_HEAP_ALLOCATOR);

    // Allocation API
    // =============================================================================================

    // Allocate memory from the MemStack.
    // alignment: Ptr returned from allocator is aligned to this value and MUST be power of 2.
    // return: nullptr if out of space OR stack is using fixed memory/size OR stack full and platform malloc fails.
    void *Push          (isize size, u8 alignment = 4);
    void *PushOnTail    (isize size, u8 alignment = 4);

    // Frees the given ptr. It MUST be the last allocated item in the stack, asserts otherwise.
    void  Pop           (void *ptr, Dqn::ZeroClear clear = Dqn::ZeroClear::No);
    void  PopOnTail     (void *ptr, Dqn::ZeroClear zeroClear = Dqn::ZeroClear::No);

    // Frees all blocks belonging to this stack.
    void  Free          ();

    // Frees the specified block belonging to the stack.
    // return: FALSE if block doesn't belong this into calls DqnMem_Free() or invalid args.
    bool  FreeMemBlock  (Block *memBlock);

    // Frees the last-most memory block. If last block, free that block making the MemStack blockless.
    // Next allocate will attach a block.
    bool  FreeLastBlock ();

    // Reverts the stack and its usage back to the first block
    void  Reset         ();

    void  ResetTail     ();

    // Reset the current memory block usage to 0.
    void  ClearCurrBlock(Dqn::ZeroClear clear);
    Info  GetInfo       ()                      const;

    // Temporary Memory Regions API
    // =============================================================================================
    // TODO(doyle): Look into a way of "preventing/guarding" against anual calls to free/clear in temp regions
    struct TempRegion
    {
        DqnMemStack *stack;             // Stack associated with this TempRegion
        Block       *startingBlock;     // Remember the block to revert to and its memory usage.
        u8          *startingBlockHead;
        u8          *startingBlockTail;
        bool         keepHeadChanges = false;
        bool         keepTailChanges = false;
    };

    struct TempRegionGuard_
    {
         TempRegionGuard_(DqnMemStack *stack);
        ~TempRegionGuard_();
        TempRegion region;
    };

    // Revert all allocations between the Begin() and End() regions. Guard version is RAII'ed.
    TempRegion       TempRegionBegin      ();
    void             TempRegionEnd        (TempRegion region);
    TempRegionGuard_ TempRegionGuard      ();

    // Keep allocations that have occurred since Begin(). End() does not need to be called anymore.
    void             TempRegionKeepChanges(TempRegion region);
};

inline bool                           DqnMemStack_Init                 (DqnMemStack *me, isize size, Dqn::ZeroClear clear, u32 flags_ = 0, DqnMemAPI *api = DQN_DEFAULT_HEAP_ALLOCATOR) { return me->Init(size, clear, flags_, api); }
inline void                          *DqnMemStack_Push                 (DqnMemStack *me, isize size, u8 alignment = 4)                                                                  { return me->Push(size, alignment); }
inline void                          *DqnMemStack_PushOnTail           (DqnMemStack *me, isize size, u8 alignment = 4)                                                                  { return me->PushOnTail(size, alignment); }
inline void                           DqnMemStack_Pop                  (DqnMemStack *me, void *ptr, Dqn::ZeroClear clear = Dqn::ZeroClear::No)                                       { me->Pop(ptr, clear); }
inline void                           DqnMemStack_PopOnTail            (DqnMemStack *me, void *ptr, Dqn::ZeroClear clear = Dqn::ZeroClear::No)                                       { me->PopOnTail(ptr, clear); }
inline void                           DqnMemStack_Free                 (DqnMemStack *me)                                                                                                { me->Free(); }
inline bool                           DqnMemStack_FreeMemBlock         (DqnMemStack *me, DqnMemStack::Block *memBlock)                                                                  { return me->FreeMemBlock(memBlock); }
inline bool                           DqnMemStack_FreeLastBlock        (DqnMemStack *me)                                                                                                { return me->FreeLastBlock(); }
inline void                           DqnMemStack_Reset                (DqnMemStack *me)                                                                                                { me->Reset(); }
inline void                           DqnMemStack_ResetTail            (DqnMemStack *me)                                                                                                { me->ResetTail(); }
inline void                           DqnMemStack_ClearCurrBlock       (DqnMemStack *me, Dqn::ZeroClear clear)                                                                          { me->ClearCurrBlock(clear); }
inline DqnMemStack::Info              DqnMemStack_GetInfo              (DqnMemStack *me)                                                                                                { return me->GetInfo(); }
inline DqnMemStack::TempRegion        DqnMemStack_TempRegionBegin      (DqnMemStack *me)                                                                                                { return me->TempRegionBegin(); }
inline void                           DqnMemStack_TempRegionEnd        (DqnMemStack *me, DqnMemStack::TempRegion region)                                                                { me->TempRegionEnd(region); }
inline DqnMemStack::TempRegionGuard_  DqnMemStack_TempRegionGuard      (DqnMemStack *me)                                                                                                { return me->TempRegionGuard(); }
inline void                           DqnMemStack_TempRegionKeepChanges(DqnMemStack *me, DqnMemStack::TempRegion region)                                                                { me->TempRegionKeepChanges(region); }


// Implementation taken from Milton, developed by Serge at
// https://github.com/serge-rgb/milton#license
#if 0
template <typename T>
void DqnArray<T>::Free()
{
    if (this->data)
    {
        if (this->memAPI)
        {
            auto sizeToFree = this->max * sizeof(T);
            this->memAPI->Free(this->data, sizeToFree);
        }
    }

    auto *memAPI_ = this->memAPI;
    *this = {};
    this->memAPI = memAPI_;
}

template <typename T>
bool DqnArray<T>::Reserve(isize newMax)
{
    if (newMax <= this->max)
    {
        return true;
    }

    if (!this->memAPI)
    {
        this->memAPI = DQN_DEFAULT_HEAP_ALLOCATOR;
    }

    usize oldSize = this->max * sizeof(T);
    usize newSize = newMax * sizeof(T);

    T *result = nullptr;
    if (this->data)
    {
        result = (T *)this->memAPI->Realloc(this->data, oldSize, newSize);
    }
    else
    {
        result = (T *)this->memAPI->Alloc(newSize, Dqn::ZeroClear::No);
    }

    if (result)
    {
        this->data = (T *)result;
        this->max  = newMax;
        return true;
    }
    else
    {
        DQN_LOGE("DqnArray out of memory error. Failed to request: %d bytes.", newSize);
        return false;
    }
}

template <typename T>
bool DqnArray<T>::Grow(isize multiplier)
{
    isize newMax = this->max * multiplier;
    newMax       = (newMax < 8) ? 8 : newMax;
    bool result  = this->Reserve(newMax);
    return result;
}

template <typename T>
inline bool DqnArray__TryMakeEnoughSpace(DqnArray<T> *array, isize numNewItems)
{
    isize numToReserve = numNewItems;
    if ((array->count + numNewItems) >= array->max)
    {
        numToReserve = array->count + numNewItems;
    }

    bool result = array->Reserve(numNewItems);
    return result;
}

template <typename T>
T *DqnArray<T>::Make(isize num)
{
    if (DqnArray__TryMakeEnoughSpace(this, num))
    {
        T *result = &this->data[this->count];
        this->count += num;
        return result;
    }

    return nullptr;
}

template <typename T>
T *DqnArray<T>::Push(T const *item, isize num)
{
    if (!DqnArray__TryMakeEnoughSpace(this, num))
    {
        DQN_LOGE("DqnArray could not push %d item(s) onto array because growing failed.", num);
        return nullptr;
    }

    for (auto i = 0; i < num; i++)
        this->data[this->count++] = item[i];

    DQN_ASSERT(this->count <= this->max);
    T *result = this->data + (this->count - 1);
    return result;
}

template <typename T>
T *DqnArray<T>::Push(T const item)
{
    T* result = this->Push(&item, 1);
    return result;
}

template <typename T>
T *DqnArray<T>::Insert(T const *item, isize numItems, isize index)
{
    if (!DqnArray__TryMakeEnoughSpace(this, numItems))
    {
        DQN_LOGE("DqnArray could not push %d item(s) onto array because growing failed.", numItems);
        return nullptr;
    }

    index = DQN_MIN(DQN_MAX(0, index), this->count);
    if (index >= this->count)
    {
        T *result = this->Push(item, numItems);
        return result;
    }

    isize numItemsToShift = this->count - index;
    for (isize i = 0; i < numItemsToShift; i++)
    {
        isize backwardsIndex       = this->count + numItems - 1 - i;
        this->data[backwardsIndex] = this->data[this->count - 1 - i];
    }

    this->count += numItems;
    DqnMem_Copy(this->data + index, item, sizeof(*item) * numItems);
    T *result = this->data + index;

    DQN_ASSERT(this->count <= this->max);
    return result;
}

template <typename T>
T *DqnArray<T>::Insert(T const item, isize index)
{
    T *result = this->Insert(&item, 1, index);
    return result;
}

template <typename T>
void DqnArray<T>::Pop()
{
    this->Remove(this->count - 1);
}

template <typename T>
T *DqnArray<T>::Peek()
{
    T *result = this->Get(this->count - 1);
    return result;
}

template <typename T>
T *DqnArray<T>::Get(isize index)
{
    T *result = nullptr;
    if (index >= 0 && index < this->count) result = &this->data[index];
    return result;
}

template <typename T>
void DqnArray<T>::Clear(Dqn::ZeroClear clear)
{
    if (clear == Dqn::ZeroClear::Yes)
    {
        isize sizeToClear = sizeof(T) * this->count;
        DqnMem_Clear(this->data, 0, sizeToClear);
    }
    this->count = 0;
}

template <typename T>
bool DqnArray<T>::Remove(isize index)
{
    if (index >= this->count || index < 0)
        return false;

    bool isLastElement = (index == (this->count - 1));
    if (isLastElement && this->count == 1)
    {
        this->count--;
    }
    else
    {
        this->data[index] = this->data[this->count - 1];
        this->count--;
    }

    return true;
}

template <typename T>
bool DqnArray<T>::EraseStable(isize index)
{
    if (index >= this->count || index < 0)
        return false;

    bool isLastElement = (index == (this->count - 1));
    if (isLastElement && this->count == 1)
    {
        this->count--;
        return true;
    }

    auto itemToRemoveByteOffset         = index * sizeof(T);
    auto oneAfterItemToRemoveByteOffset = (index + 1)  * sizeof(T);
    auto lastItemByteOffset             = this->count * sizeof(T);
    auto numBytesToMove                 = lastItemByteOffset - oneAfterItemToRemoveByteOffset;

    u8 *bytePtr = (u8 *)this->data;
    u8 *dest    = &bytePtr[itemToRemoveByteOffset];
    u8 *src     = &bytePtr[oneAfterItemToRemoveByteOffset];
    memmove(dest, src, numBytesToMove);

    this->count--;
    return true;
}

template <typename T>
void DqnArray<T>::EraseStable(isize *indexList, isize numIndexes)
{
    if (numIndexes == 0 || !indexList) return;

    // NOTE: Sort the index list and ensure we only remove indexes up to the size of our array
    Dqn_QuickSort<isize>(indexList, numIndexes);

    auto arrayHighestIndex = this->count - 1;
    auto realCount         = numIndexes;
    if (indexList[numIndexes - 1] > arrayHighestIndex)
    {
        auto realNumIndexes =
            Dqn_BSearch(indexList, numIndexes, arrayHighestIndex, Dqn_BSearchBound_Lower);
        // NOTE: If -1, then there's no index in the indexlist that is within the range of our array
        // i.e. no index we can remove without out of array bounds access
        if (realNumIndexes == -1)
            return;

        if (indexList[realNumIndexes] == arrayHighestIndex)
        {
            realCount = realNumIndexes++;
        }
        else
        {
            realCount = realNumIndexes += 2;
        }
        realCount = DQN_MIN(numIndexes, realCount);
    }

    if (realCount == 1)
    {
        this->EraseStable(indexList[0]);
    }
    else
    {
        auto indexListIndex  = 0;
        auto indexToCopyTo   = indexList[indexListIndex++];
        auto indexToCopyFrom = indexToCopyTo + 1;
        auto deadIndex       = indexList[indexListIndex++];

        bool breakLoop = false;
        for (;
             indexToCopyFrom < this->count;
             indexToCopyTo++, indexToCopyFrom++)
        {
            while (indexToCopyFrom == deadIndex)
            {
                deadIndex = indexList[indexListIndex++];
                indexToCopyFrom++;

                breakLoop |= (indexToCopyFrom >= this->count);
                breakLoop |= (indexListIndex > realCount);
                if (breakLoop) break;
            }
            if (breakLoop) break;

            this->data[indexToCopyTo] = this->data[indexToCopyFrom];
        }

        for (; indexToCopyFrom < this->count; indexToCopyTo++, indexToCopyFrom++)
        {
            this->data[indexToCopyTo] = this->data[indexToCopyFrom];
        }

        this->count -= realCount;
        DQN_ASSERT(this->count >= 0);
    }
}
#endif

// #DqnHash API
// =================================================================================================
DQN_FILE_SCOPE u32 DqnHash_Murmur32Seed(void const *data,  usize len, u32 seed);
DQN_FILE_SCOPE u64 DqnHash_Murmur64Seed(void const *data_, usize len, u64 seed);

DQN_FILE_SCOPE inline u32 DqnHash_Murmur32(void const *data, usize len)
{
    return DqnHash_Murmur32Seed(data, len, 0x9747b28c);
}

DQN_FILE_SCOPE inline u64 DqnHash_Murmur64(void const *data, usize len)
{
    return DqnHash_Murmur64Seed(data, len, 0x9747b28c);
}

// #DqnMath API
// =================================================================================================
DQN_FILE_SCOPE f32 DqnMath_Lerp  (f32 a, f32 t, f32 b);
DQN_FILE_SCOPE f32 DqnMath_Sqrtf (f32 a);
DQN_FILE_SCOPE f32 DqnMath_Clampf(f32 val, f32 min, f32 max);

// #DqnV2 API
// =================================================================================================
union DqnV2i
{
    struct { i32 x, y; };
    struct { i32 w, h; };
    struct { i32 min, max; };
    i32 e[2];

    DqnV2i() = default;
    DqnV2i(i32 x_, i32 y_): x(x_), y(y_) {}
    DqnV2i(f32 x_, f32 y_): x((i32)x_), y((i32)y_) {}

    bool operator==(DqnV2i const &b) const { return (this->x == b.x) && (this->y == b.y); }
    bool operator!=(DqnV2i const &b) const { return !(*this == b); }
    bool operator>=(DqnV2i const &b) const { return (this->x >= b.x) && (this->y >= b.y); }
    bool operator<=(DqnV2i const &b) const { return (this->x <= b.x) && (this->y <= b.y); }
    bool operator< (DqnV2i const &b) const { return (this->x <  b.x) && (this->y <  b.y); }
    bool operator> (DqnV2i const &b) const { return (this->x >  b.x) && (this->y >  b.y); }
};

union DqnV2
{
    struct { f32 x, y; };
    struct { f32 w, h; };
    struct { f32 min, max; };
    f32 e[2];

    DqnV2() = default;
    DqnV2(f32 xy)        : x(xy),       y(xy)       {}
    DqnV2(f32 x_, f32 y_): x(x_),       y(y_)       {}
    DqnV2(i32 x_, i32 y_): x((f32)x_),  y((f32)y_)  {}
    DqnV2(DqnV2i a)      : x((f32)a.x), y((f32)a.y) {}

    bool operator==(DqnV2 const &b) const { return (this->x == b.x) && (this->y == b.y); }
    bool operator!=(DqnV2 const &b) const { return !(*this == b); }
    bool operator>=(DqnV2 const &b) const { return (this->x >= b.x) && (this->y >= b.y); }
    bool operator<=(DqnV2 const &b) const { return (this->x <= b.x) && (this->y <= b.y); }
    bool operator< (DqnV2 const &b) const { return (this->x <  b.x) && (this->y <  b.y); }
    bool operator> (DqnV2 const &b) const { return (this->x >  b.x) && (this->y >  b.y); }
};

DQN_FILE_SCOPE DqnV2 DqnV2_Add     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Sub     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Scalei  (DqnV2 a, i32 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Scalef  (DqnV2 a, f32 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Hadamard(DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE f32   DqnV2_Dot     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE bool  DqnV2_Equals  (DqnV2 a, DqnV2 b);

DQN_FILE_SCOPE f32   DqnV2_LengthSquared(const DqnV2 a, const DqnV2 b);
DQN_FILE_SCOPE f32   DqnV2_Length       (const DqnV2 a, const DqnV2 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Normalise    (const DqnV2 a);
DQN_FILE_SCOPE bool  DqnV2_Overlaps     (      DqnV2 a,       DqnV2 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Perpendicular(const DqnV2 a);

DQN_FILE_SCOPE DqnV2 DqnV2_ResizeKeepAspectRatio(DqnV2 srcSize, DqnV2 targetSize);
DQN_FILE_SCOPE DqnV2 DqnV2_ConstrainToRatio     (DqnV2 dim, DqnV2 ratio); // Resize the dimension to fit the aspect ratio provided. Downscale only.

DQN_FILE_SCOPE inline DqnV2  operator- (DqnV2  a, DqnV2 b) { return      DqnV2_Sub     (a, b);  }
DQN_FILE_SCOPE inline DqnV2  operator+ (DqnV2  a, DqnV2 b) { return      DqnV2_Add     (a, b);  }
DQN_FILE_SCOPE inline DqnV2  operator* (DqnV2  a, DqnV2 b) { return      DqnV2_Hadamard(a, b);  }
DQN_FILE_SCOPE inline DqnV2  operator* (DqnV2  a, f32   b) { return      DqnV2_Scalef  (a, b);  }
DQN_FILE_SCOPE inline DqnV2  operator* (DqnV2  a, i32   b) { return      DqnV2_Scalei  (a, b);  }
DQN_FILE_SCOPE inline DqnV2 &operator*=(DqnV2 &a, DqnV2 b) { return (a = DqnV2_Hadamard(a, b)); }
DQN_FILE_SCOPE inline DqnV2 &operator*=(DqnV2 &a, f32   b) { return (a = DqnV2_Scalef  (a, b)); }
DQN_FILE_SCOPE inline DqnV2 &operator*=(DqnV2 &a, i32   b) { return (a = DqnV2_Scalei  (a, b)); }
DQN_FILE_SCOPE inline DqnV2 &operator-=(DqnV2 &a, DqnV2 b) { return (a = DqnV2_Sub     (a, b)); }
DQN_FILE_SCOPE inline DqnV2 &operator+=(DqnV2 &a, DqnV2 b) { return (a = DqnV2_Add     (a, b)); }

// DqnV2i
DQN_FILE_SCOPE DqnV2i DqnV2i_Add     (DqnV2i a, DqnV2i b);
DQN_FILE_SCOPE DqnV2i DqnV2i_Sub     (DqnV2i a, DqnV2i b);
DQN_FILE_SCOPE DqnV2i DqnV2i_Scalei  (DqnV2i a, i32 b);
DQN_FILE_SCOPE DqnV2i DqnV2i_Scalef  (DqnV2i a, f32 b);
DQN_FILE_SCOPE DqnV2i DqnV2i_Hadamard(DqnV2i a, DqnV2i b);
DQN_FILE_SCOPE f32    DqnV2i_Dot     (DqnV2i a, DqnV2i b);
DQN_FILE_SCOPE bool   DqnV2i_Equals  (DqnV2i a, DqnV2i b);

DQN_FILE_SCOPE inline DqnV2i  operator- (DqnV2i  a, DqnV2i b) { return      DqnV2i_Sub     (a, b);  }
DQN_FILE_SCOPE inline DqnV2i  operator+ (DqnV2i  a, DqnV2i b) { return      DqnV2i_Add     (a, b);  }
DQN_FILE_SCOPE inline DqnV2i  operator* (DqnV2i  a, DqnV2i b) { return      DqnV2i_Hadamard(a, b);  }
DQN_FILE_SCOPE inline DqnV2i  operator* (DqnV2i  a, f32    b) { return      DqnV2i_Scalef  (a, b);  }
DQN_FILE_SCOPE inline DqnV2i  operator* (DqnV2i  a, i32    b) { return      DqnV2i_Scalei  (a, b);  }
DQN_FILE_SCOPE inline DqnV2i &operator*=(DqnV2i &a, DqnV2i b) { return (a = DqnV2i_Hadamard(a, b)); }
DQN_FILE_SCOPE inline DqnV2i &operator-=(DqnV2i &a, DqnV2i b) { return (a = DqnV2i_Sub     (a, b)); }
DQN_FILE_SCOPE inline DqnV2i &operator+=(DqnV2i &a, DqnV2i b) { return (a = DqnV2i_Add     (a, b)); }

// #DqnV3 API
// =================================================================================================
union DqnV3
{
    struct { f32 x, y, z; };
    struct { f32 r, g, b; };
    DqnV2 xy;
    f32 e[3];

    DqnV3() = default;
    DqnV3(f32 xyz)               : x(xyz),     y(xyz),     z(xyz)     {}
    DqnV3(f32 x_, f32 y_, f32 z_): x(x_),      y(y_),      z(z_)      {}
    DqnV3(i32 x_, i32 y_, i32 z_): x((f32)x_), y((f32)y_), z((f32)z_) {}
};

union DqnV3i
{
    struct { i32 x, y, z; };
    struct { i32 r, g, b; };
    i32 e[3];

    DqnV3i() = default;
    DqnV3i(i32 x_, i32 y_, i32 z_): x(x_),      y(y_),      z(z_)      {}
    DqnV3i(f32 x_, f32 y_, f32 z_): x((i32)x_), y((i32)y_), z((i32)z_) {}
};

// DqnV3
DQN_FILE_SCOPE DqnV3 DqnV3_Add     (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Sub     (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Scalei  (DqnV3 a, i32 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Scalef  (DqnV3 a, f32 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Hadamard(DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE f32   DqnV3_Dot     (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE bool  DqnV3_Equals  (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Cross   (DqnV3 a, DqnV3 b);

DQN_FILE_SCOPE DqnV3 DqnV3_Normalise    (DqnV3 a);
DQN_FILE_SCOPE f32   DqnV3_Length       (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE f32   DqnV3_LengthSquared(DqnV3 a, DqnV3 b);

DQN_FILE_SCOPE inline DqnV3  operator- (DqnV3  a, DqnV3 b) { return      DqnV3_Sub     (a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator+ (DqnV3  a, DqnV3 b) { return      DqnV3_Add     (a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator+ (DqnV3  a, f32   b) { return      DqnV3_Add     (a, DqnV3(b));    }
DQN_FILE_SCOPE inline DqnV3  operator* (DqnV3  a, DqnV3 b) { return      DqnV3_Hadamard(a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator* (DqnV3  a, f32   b) { return      DqnV3_Scalef  (a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator* (DqnV3  a, i32   b) { return      DqnV3_Scalei  (a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator/ (DqnV3  a, f32   b) { return      DqnV3_Scalef  (a, (1.0f/b));    }
DQN_FILE_SCOPE inline DqnV3 &operator*=(DqnV3 &a, DqnV3 b) { return (a = DqnV3_Hadamard(a, b));          }
DQN_FILE_SCOPE inline DqnV3 &operator*=(DqnV3 &a, f32   b) { return (a = DqnV3_Scalef  (a, b));          }
DQN_FILE_SCOPE inline DqnV3 &operator*=(DqnV3 &a, i32   b) { return (a = DqnV3_Scalei  (a, b));          }
DQN_FILE_SCOPE inline DqnV3 &operator-=(DqnV3 &a, DqnV3 b) { return (a = DqnV3_Sub     (a, b));          }
DQN_FILE_SCOPE inline DqnV3 &operator+=(DqnV3 &a, DqnV3 b) { return (a = DqnV3_Add     (a, b));          }
DQN_FILE_SCOPE inline bool   operator==(DqnV3  a, DqnV3 b) { return      DqnV3_Equals  (a, b);           }

// #DqnV4 API
// =================================================================================================
union DqnV4
{
    struct { f32 x, y, z, w; };
    DqnV3 xyz;
    DqnV2 xy;

    struct { f32 r, g, b, a; };
    DqnV3 rgb;

    f32    e[4];
    DqnV2 v2[2];

    DqnV4() = default;
    DqnV4(f32 xyzw)                      : x(xyzw),    y(xyzw),    z(xyzw),    w(xyzw)    {}
    DqnV4(f32 x_, f32 y_, f32 z_, f32 w_): x(x_),      y(y_),      z(z_),      w(w_)      {}
    DqnV4(i32 x_, i32 y_, i32 z_, i32 w_): x((f32)x_), y((f32)y_), z((f32)z_), w((f32)w_) {}
    DqnV4(DqnV3 a, f32 w_)               : x(a.x),     y(a.y),     z(a.z),     w(w_)      {}

};

DQN_FILE_SCOPE DqnV4 DqnV4_Add     (DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE DqnV4 DqnV4_Sub     (DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE DqnV4 DqnV4_Scalef  (DqnV4 a, f32 b);
DQN_FILE_SCOPE DqnV4 DqnV4_Scalei  (DqnV4 a, i32 b);
DQN_FILE_SCOPE DqnV4 DqnV4_Hadamard(DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE f32   DqnV4_Dot     (DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE bool  DqnV4_Equals  (DqnV4 a, DqnV4 b);

DQN_FILE_SCOPE inline DqnV4  operator- (DqnV4  a, DqnV4 b) { return      DqnV4_Sub     (a, b);            }
DQN_FILE_SCOPE inline DqnV4  operator+ (DqnV4  a, DqnV4 b) { return      DqnV4_Add     (a, b);            }
DQN_FILE_SCOPE inline DqnV4  operator+ (DqnV4  a, f32   b) { return      DqnV4_Add     (a, DqnV4(b));     }
DQN_FILE_SCOPE inline DqnV4  operator* (DqnV4  a, DqnV4 b) { return      DqnV4_Hadamard(a, b);            }
DQN_FILE_SCOPE inline DqnV4  operator* (DqnV4  a, f32   b) { return      DqnV4_Scalef  (a, b);            }
DQN_FILE_SCOPE inline DqnV4  operator* (DqnV4  a, i32   b) { return      DqnV4_Scalei  (a, b);            }
DQN_FILE_SCOPE inline DqnV4 &operator*=(DqnV4 &a, DqnV4 b) { return (a = DqnV4_Hadamard(a, b));           }
DQN_FILE_SCOPE inline DqnV4 &operator*=(DqnV4 &a, f32   b) { return (a = DqnV4_Scalef  (a, b));           }
DQN_FILE_SCOPE inline DqnV4 &operator*=(DqnV4 &a, i32   b) { return (a = DqnV4_Scalei  (a, b));           }
DQN_FILE_SCOPE inline DqnV4 &operator-=(DqnV4 &a, DqnV4 b) { return (a = DqnV4_Sub     (a, b));           }
DQN_FILE_SCOPE inline DqnV4 &operator+=(DqnV4 &a, DqnV4 b) { return (a = DqnV4_Add     (a, b));           }
DQN_FILE_SCOPE inline bool   operator==(DqnV4 &a, DqnV4 b) { return      DqnV4_Equals  (a, b);            }

// #DqnMat4 API
// =================================================================================================
typedef union DqnMat4
{
    // TODO(doyle): Row/column instead? More cache friendly since multiplication
    // prefers rows.
    DqnV4 col[4];
    f32   e[4][4]; // Column/row
} DqnMat4;

DQN_FILE_SCOPE DqnMat4 DqnMat4_Identity    ();

DQN_FILE_SCOPE DqnMat4 DqnMat4_Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar);
DQN_FILE_SCOPE DqnMat4 DqnMat4_Perspective (f32 fovYDegrees, f32 aspectRatio, f32 zNear, f32 zFar);
DQN_FILE_SCOPE DqnMat4 DqnMat4_LookAt      (DqnV3 eye, DqnV3 center, DqnV3 up);

DQN_FILE_SCOPE DqnMat4 DqnMat4_Translate3f (f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnMat4 DqnMat4_TranslateV3 (DqnV3 vec);
DQN_FILE_SCOPE DqnMat4 DqnMat4_Rotate      (f32 radians, f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnMat4 DqnMat4_Scale       (f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnMat4 DqnMat4_ScaleV3     (DqnV3 scale);
DQN_FILE_SCOPE DqnMat4 DqnMat4_Mul         (DqnMat4 a, DqnMat4 b);
DQN_FILE_SCOPE DqnV4   DqnMat4_MulV4       (DqnMat4 a, DqnV4 b);

// #DqnRect API
// =================================================================================================
struct DqnRect
{
    DqnV2 min;
    DqnV2 max;

    DqnRect() = default;
    DqnRect(DqnV2 origin, DqnV2 size)   { this->min = origin;      this->max = origin + size; }
    DqnRect(f32 x, f32 y, f32 w, f32 h) { this->min = DqnV2(x, y); this->max = DqnV2(x + w, y + h); }
    DqnRect(i32 x, i32 y, i32 w, i32 h) { this->min = DqnV2(x, y); this->max = DqnV2(x + w, y + h); }

    f32   GetWidth ()                                    const { return max.w - min.w; }
    f32   GetHeight()                                    const { return max.h - min.h; }
    DqnV2 GetSize  ()                                    const { return max - min;     }
    void  GetSize  (f32 *const width, f32 *const height) const;
    DqnV2 GetCenter()                                    const;

    DqnRect ClipRect (DqnRect const clip)  const;
    DqnRect Move     (DqnV2   const shift) const;
    bool    ContainsP(DqnV2   const p)     const;
};

// #DqnString Public API - String library
// =================================================================================================
// String allocates +1 extra byte for the null-terminator to be completely compatible with
// C style strings, but this is not reflected in the capacity or len, and is hidden from the user.

// Usage: DqnString example = DQN_STRING_LITERAL(example, "hello world");
#define DQN_STRING_LITERAL(srcVariable, literal)                                                   \
    DQN_STRING_LITERAL_INTERNAL(srcVariable, literal, DQN_UNIQUE_NAME(dqnstring_))

struct DqnString
{
    DqnMemAPI *memAPI;
    i32        len;              // Len of the string in bytes not including null-terminator
    i32        max;              // The maximum capacity not including space for null-terminator.
    char      *str;

    // Initialisation API
    // =============================================================================================
    void Init              (DqnMemAPI   *const api = DQN_DEFAULT_HEAP_ALLOCATOR);
    void Init              (DqnMemStack *const stack);

    // return: False if (size < 0) or (memAPI allocation failed).
    bool InitSize          (i32 size, DqnMemStack *const stack);
    bool InitSize          (i32 size, DqnMemAPI   *const api = DQN_DEFAULT_HEAP_ALLOCATOR);

    // return: False if arguments are invalid.
    bool InitFixedMem      (char *const memory, const i32 sizeInBytes);

    bool InitLiteral       (char    const *const cstr, i32 const lenInBytes, DqnMemStack *const stack);
    bool InitLiteral       (char    const *const cstr,                       DqnMemStack *const stack);
    bool InitLiteral       (char    const *const cstr, i32 const lenInBytes, DqnMemAPI   *const api = DQN_DEFAULT_HEAP_ALLOCATOR);
    bool InitLiteral       (char    const *const cstr,                       DqnMemAPI   *const api = DQN_DEFAULT_HEAP_ALLOCATOR);

    bool InitLiteral       (wchar_t const *const cstr, DqnMemStack *const stack);
    bool InitLiteral       (wchar_t const *const cstr, DqnMemAPI   *const api = DQN_DEFAULT_HEAP_ALLOCATOR);

    // return: False if cstr is nullptr.
    bool InitLiteralNoAlloc(char *const cstr, i32 cstrLen = -1);

    // API
    // =============================================================================================
    // return: These functions return false if allocation failed. String is preserved.
    bool     Expand  (i32 newMax);
    bool     Sprintf (char      const *const fmt, ...);
    bool     VSprintf(char      const *const fmt, va_list argList);
    bool     Append  (DqnString const        string);
    bool     Append  (DqnString const       *string);
    bool     Append  (char      const *const cstr, i32 bytesToCopy = -1);

    void     Clear   ();
    void     Free    ();

    // return: -1 if invalid, or if bufSize is 0 the required buffer length in wchar_t characters
    i32      ToWChar(wchar_t *const buf, i32 const bufSize) const;

    // return: String allocated using api.
    wchar_t *ToWChar(DqnMemAPI *const api = DQN_DEFAULT_HEAP_ALLOCATOR) const;

    // Statics
    // =============================================================================================
    static bool Cmp(DqnString const *a, DqnString const *b, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No)
    {
        bool result = (a->len == b->len) && (DqnStr_Cmp(a->str, b->str, a->len, ignore) == 0);
        return result;
    }

    static bool Cmp(DqnString const *a, DqnSlice<char const> const b, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No)
    {
        bool result = (a->len == b.len) && (DqnStr_Cmp(a->str, b.data, b.len, ignore) == 0);
        return result;
    }

    static bool Cmp(DqnString const *a, DqnSlice<char> const b, Dqn::IgnoreCase ignore = Dqn::IgnoreCase::No)
    {
        bool result = (a->len == b.len) && (DqnStr_Cmp(a->str, b.data, b.len, ignore) == 0);
        return result;
    }
};

struct DqnSmartString : public DqnString
{
    ~DqnSmartString() { this->Free(); }
};

DQN_FILE_SCOPE DqnString DqnString_(DqnMemAPI   *const api = DQN_DEFAULT_HEAP_ALLOCATOR);
DQN_FILE_SCOPE DqnString DqnString_(DqnMemStack *const stack);

// NOTE: First level of indirection needs to turn the combined dqnstring_(guid) into a name. Otherwise
//       each use of literalVarName will increment __COUNTER__
#define DQN_STRING_LITERAL_INTERNAL(srcVariable, literal, literalVarName)                          \
    {};                                                                                            \
    char literalVarName[] = literal;                                                               \
    srcVariable.InitLiteralNoAlloc(literalVarName, DQN_CHAR_COUNT(literalVarName))

// #DqnFixedString Public API - Fixed sized strings at compile time
// =================================================================================================
FILE_SCOPE int DqnFixedString__Append(char *dest, int destSize, char const *src, int len = -1);

template <unsigned int MAX>
struct DqnFixedString
{
    int  len;
    char str[MAX];

    DqnFixedString(): len(0)                          { this->str[0] = 0; }
    DqnFixedString(char const *str)                   {                     this->len = DqnFixedString__Append(this->str, MAX, str, -1);               }
    DqnFixedString(char const *str, int len)          {                     this->len = DqnFixedString__Append(this->str, MAX, str, len);              }
    DqnFixedString(DqnSlice<char const> const &other) {                     this->len = DqnFixedString__Append(this->str, MAX, other.data, other.len); }
    DqnFixedString(DqnSlice<char> const &other)       {                     this->len = DqnFixedString__Append(this->str, MAX, other.data, other.len); }
    DqnFixedString(DqnFixedString const &other)       { if (this != &other) this->len = DqnFixedString__Append(this->str, MAX, other.str, other.len);  }

    DqnFixedString &operator+=(char const *other)                 { this->len = DqnFixedString__Append(this->str + this->len, MAX - this->len, other); return *this; }
    DqnFixedString &operator+=(DqnSlice<char const> const &other) { this->len = DqnFixedString__Append(this->str + this->len, MAX - this->len, other.data, other.len); return *this; }
    DqnFixedString &operator+=(DqnSlice<char> const &other)       { this->len = DqnFixedString__Append(this->str + this->len, MAX - this->len, other.data, other.len); return *this; }
    DqnFixedString &operator+=(DqnFixedString const &other)       { this->len = DqnFixedString__Append(this->str + this->len, MAX - this->len, other.str, other.len); return *this; }

    DqnFixedString  operator+ (char const *other)                 { DqnFixedString result = *this; result.len += DqnFixedString__Append(result.str + result.len, MAX - result.len, other); return result; }
    DqnFixedString  operator+ (DqnSlice<char const> const &other) { DqnFixedString result = *this; result.len += DqnFixedString__Append(result.str + result.len, MAX - result.len, other.data, other.len); return result; }
    DqnFixedString  operator+ (DqnSlice<char> const &other)       { DqnFixedString result = *this; result.len += DqnFixedString__Append(result.str + result.len, MAX - result.len, other.data, other.len); return result; }
    DqnFixedString  operator+ (DqnFixedString const &other)       { DqnFixedString result = *this; result.len += DqnFixedString__Append(result.str + result.len, MAX - result.len, other.str, other.len); return result; }

    // Xprintf functions always modifies buffer and null-terminates even with insufficient buffer size.
    // Asserts on failure if DQN_ASSERT is defined.
    // return: The number of characters copied to the buffer
    int Sprintf       (char const *fmt, ...);
    int SprintfAppend (char const *fmt, ...);

    int VSprintf      (char const *fmt, va_list argList);
    int VSprintfAppend(char const *fmt, va_list argList);

    void Clear        (Dqn::ZeroClear clear = Dqn::ZeroClear::No) { if (clear == Dqn::ZeroClear::Yes) DqnMem_Set(str, 0, MAX); *this = {}; }
};

template <unsigned int MAX>
FILE_SCOPE int
DqnFixedString__VSprintf(DqnFixedString<MAX> *me, char const *fmt, va_list argList, int bufOffset)
{
    char *bufStart           = me->str + bufOffset;
    i32 const remainingSpace = static_cast<i32>((me->str + MAX) - bufStart);
    int result               = Dqn_vsnprintf(bufStart, remainingSpace, fmt, argList);
    me->len                  = result;
    return result;
}

template <unsigned int MAX>
int DqnFixedString<MAX>::VSprintf(char const *fmt, va_list argList)
{
    int bufOffset = 0;
    int result    = DqnFixedString__VSprintf(this, fmt, argList, bufOffset);
    return result;
}

template <unsigned int MAX>
int DqnFixedString<MAX>::Sprintf(char const *fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);
    int result = this->VSprintf(fmt, argList);
    va_end(argList);
    return result;
}

template <unsigned int MAX>
int DqnFixedString<MAX>::VSprintfAppend(char const *fmt, va_list argList)
{
    int bufOffset = this->len;
    int result = DqnFixedString__VSprintf(this, fmt, argList, bufOffset);
    return result;
}

template <unsigned int MAX>
int DqnFixedString<MAX>::SprintfAppend(char const *fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);
    int result = this->VSprintfAppend(fmt, argList);
    va_end(argList);
    return result;
}

// TODO(doyle): Load factor
// #DqnHashTable API
// =================================================================================================
template <typename T>
struct DqnHashTable
{
    struct Entry
    {
        DqnString  key;
        T          data;
        Entry     *next;
    };

    DqnMemAPI *memAPI;

    Entry **entries;
    i64     numEntries;
    Entry  *freeList;         // Entries which are allocated and can be reused are stored here.
    i64     numFreeEntries;
    i64    *usedEntries;      // Tracks the indexes used in the entries.
    i64     usedEntriesIndex;

    // Initialisation API
    // =============================================================================================
    bool Init(i64 const numTableEntries = 1024, DqnMemAPI   *const api = DQN_DEFAULT_HEAP_ALLOCATOR);
    bool Init(i64 const numTableEntries,        DqnMemStack *const stack);

    // API
    // =============================================================================================
    // keyLen: String length not incl. null-terminator. If -1, utf8 strlen will be used.
    // return: Pre-existing entry if it exists, otherwise a nullptr.
    Entry *Get                    (DqnString   const key)                  const;
    Entry *Get                    (char const *const key, i32 keyLen = -1) const;

    // exists: Pass a bool that gets set true if a new entry was created, false if it already exists.
    // return: Pre-existing entry if it exists. Else create entry with key. Nullptr if out of memory.
    Entry *Make                   (DqnString   const key,                  bool *exists = nullptr);
    Entry *Make                   (char const *const key, i32 keyLen = -1, bool *exists = nullptr);

    void   Remove                 (DqnString   const key);
    void   Remove                 (char const *const key, i32 keyLen = -1);
    void   Clear                  ();
    void   Free                   ();

    // num: If num > 0, allocate num entries to free list. If num < 0, remove num entries from free list.
    // return: False if allocation failed.
    bool   AddNewEntriesToFreeList(i64 num);

    // newNumEntries: If different from numEntries, reallocate the table and rehash all entries in the table.
    // return: False if allocation failed.
    bool   ChangeNumEntries       (i64 newNumEntries);
};

template <typename T>
bool DqnHashTable<T>::Init(i64 const numTableEntries, DqnMemAPI *const api)
{
    usize arrayOfPtrsSize = sizeof(*this->entries) * numTableEntries;
    auto *arrayOfPtrs   = (u8 *)api->Alloc((isize)arrayOfPtrsSize);
    if (!arrayOfPtrs) return false;

    usize usedEntriesSize  = sizeof(*this->usedEntries) * numTableEntries;
    auto *usedEntriesPtr = (u8 *)api->Alloc(usedEntriesSize);
    if (!usedEntriesPtr)
    {
        api->Free((void *)arrayOfPtrs, arrayOfPtrsSize);
        return false;
    }

    this->memAPI           = api;
    this->entries          = (Entry **)arrayOfPtrs;
    this->numEntries       = numTableEntries;
    this->freeList         = nullptr;
    this->numFreeEntries   = 0;
    this->usedEntries      = (i64 *)usedEntriesPtr;
    this->usedEntriesIndex = 0;

    return true;
}

template <typename T>
bool DqnHashTable<T>::Init(i64 const numTableEntries, DqnMemStack *const stack)
{
    if (!stack) return false;
    bool result = Init(numTableEntries, &stack->myHeadAPI);
    return result;
}

template <typename T>
typename DqnHashTable<T>::Entry *DqnHashTableInternal_AllocateEntry(DqnHashTable<T> *table)
{
    auto *result = (typename DqnHashTable<T>::Entry *)table->memAPI->Alloc(sizeof(typename DqnHashTable<T>::Entry));

    if (result)
    {
        if (result->key.InitSize(0, table->memAPI))
        {
            return result;
        }

        DQN_ASSERTM(DQN_INVALID_CODE_PATH, "Out of memory error");
        table->memAPI->Free(result, sizeof(*result));
        return nullptr;
    }
    else
    {
        return nullptr;
    }
}

template <typename T>
FILE_SCOPE typename DqnHashTable<T>::Entry *
DqnHashTableInternal_GetFreeEntry(DqnHashTable<T> *table)
{
    typename DqnHashTable<T>::Entry *result = {};
    if (table->freeList)
    {
        result          = table->freeList;
        table->freeList = table->freeList->next;
        table->numFreeEntries--;
    }
    else
    {
        DQN_ASSERT(table->numFreeEntries == 0);
        result = DqnHashTableInternal_AllocateEntry(table);
    }

    return result;
}

template <typename T>
FILE_SCOPE inline i64 DqnHashTableInternal_GetHashIndex(DqnHashTable<T> const *table,
                                                        char const *const key, i32 keyLen)
{
    u64 hash      = DqnHash_Murmur64(key, keyLen);
    i64 hashIndex = hash % table->numEntries;
    return hashIndex;
}

FILE_SCOPE inline i64 DqnHashTableInternal_GetHashIndex(i64 numEntries, char const *const key,
                                                        i32 keyLen)
{
    u64 hash      = DqnHash_Murmur64(key, keyLen);
    i64 hashIndex = hash % numEntries;
    return hashIndex;
}

template <typename T>
typename DqnHashTable<T>::Entry *
DqnHashTableInternal_FindMatchingKey(typename DqnHashTable<T>::Entry *entry, char const *const key,
                                     i32 keyLen,
                                     typename DqnHashTable<T>::Entry **prevEntry = nullptr)
{
    for (;;)
    {
        if (entry->key.len == keyLen && DqnStr_Cmp(entry->key.str, key) == 0)
        {
            return entry;
        }

        if (entry->next == nullptr) break;
        if (prevEntry) *prevEntry = entry;

        entry = entry->next;
    }

    return nullptr;
}

template <typename T>
DQN_FILE_SCOPE inline typename DqnHashTable<T>::Entry *
DqnHashTableInternal_Get(DqnHashTable<T> const *table, char const *const key, i32 keyLen, i64 hashIndex)
{
    typename DqnHashTable<T>::Entry *entry  = table->entries[hashIndex];
    if (entry)
    {
        typename DqnHashTable<T>::Entry *matchingEntry
            = DqnHashTableInternal_FindMatchingKey<T>(entry, key, (i64)keyLen);
        if (matchingEntry)
            return matchingEntry;
    }

    return nullptr;
}

template <typename T>
typename DqnHashTable<T>::Entry *DqnHashTable<T>::Get(char const *const key, i32 keyLen) const
{
    if (keyLen == -1) DqnStr_LenUTF8((u32 *)key, &keyLen);
    i64 hashIndex = DqnHashTableInternal_GetHashIndex<T>(this, key, keyLen);
    Entry *result = DqnHashTableInternal_Get<T>(this, key, keyLen, hashIndex);
    return result;
}

template <typename T>
typename DqnHashTable<T>::Entry *DqnHashTable<T>::Get(DqnString const key) const
{
    Entry *result = this->Get(key.str, key.len);
    return result;
}

template <typename T>
typename DqnHashTable<T>::Entry *DqnHashTable<T>::Make(char const *const key, i32 keyLen,
                                                       bool *exists)
{
    // NOTE: Internal_Get() function because we want a way to allow re-using the hashIndex
    if (keyLen == -1) DqnStr_LenUTF8((u32 *)key, &keyLen);

    i64 hashIndex        = DqnHashTableInternal_GetHashIndex(this, key, keyLen);
    Entry *existingEntry = DqnHashTableInternal_Get(this, key, keyLen, hashIndex);

    if (exists) *exists = true;
    if (existingEntry) return existingEntry;

    Entry *newEntry = DqnHashTableInternal_GetFreeEntry(this);
    if (newEntry)
    {
        if (exists) *exists = false;

        // If entry for hashIndex not used yet, mark it down as a used slot.
        if (!this->entries[hashIndex])
        {
            i64 index = Dqn_BSearch(this->usedEntries, this->usedEntriesIndex, hashIndex,
                                    Dqn_BSearchBound_Lower);
            i64 indexToEndAt              = index;
            if (index == -1) indexToEndAt = 0;

            this->usedEntriesIndex++;
            for (i64 i = this->usedEntriesIndex; i > indexToEndAt; i--)
                this->usedEntries[i] = this->usedEntries[i - 1];

            this->usedEntries[indexToEndAt] = hashIndex;
        }

        // TODO(doyle): Is this a robust check? If we retrieve from the freeEntryList, the memAPI
        // may already be initialised.
        if (!newEntry->key.memAPI)
        {
            newEntry->key.InitSize(keyLen, this->memAPI);
            DQN_ASSERT(newEntry->key.memAPI == this->memAPI);
        }

        newEntry->key.Append(key, keyLen);
        newEntry->next           = this->entries[hashIndex];
        this->entries[hashIndex] = newEntry;

        return newEntry;
    }
    else
    {
        DQN_ASSERTM(DQN_INVALID_CODE_PATH, "DqnHashTable_Get() failed: Out of memory.");
        return nullptr;
    }
}

template <typename T>
typename DqnHashTable<T>::Entry *DqnHashTable<T>::Make(DqnString const key, bool *exists)
{
    Entry *result = this->Make(key.str, key.len, exists);
    return result;
}

template <typename T>
void DqnHashTable<T>::Remove(char const *const key, i32 keyLen)
{
    if (keyLen == -1) DqnStr_LenUTF8((u32 *)key, &keyLen);

    i64 hashIndex = DqnHashTableInternal_GetHashIndex(this, key, keyLen);
    Entry *entry  = this->entries[hashIndex];

    if (entry)
    {
        Entry prevEntry_;

        Entry *prevEntry = &prevEntry_;
        Entry *entryToFree =
            DqnHashTableInternal_FindMatchingKey<T>(entry, key, (i64)keyLen, &prevEntry);
        if (entryToFree)
        {
            if (entryToFree == entry)
            {
                // Unique entry, so remove this index from the used list as well.
                i64 indexToRemove = Dqn_BSearch(this->usedEntries, this->usedEntriesIndex,
                                                hashIndex, Dqn_BSearchBound_Lower);

                for (i64 i = indexToRemove; i < this->usedEntriesIndex - 1; i++)
                    this->usedEntries[i] = this->usedEntries[i + 1];

                this->usedEntriesIndex--;
            }

            if (prevEntry)
            {
                prevEntry->next = entryToFree->next;
            }

            entryToFree->key.Clear();
            entryToFree->data = {};
            entryToFree->next = this->freeList;

            this->freeList = entryToFree;
            this->numFreeEntries++;
        }
    }
}

template <typename T>
void DqnHashTable<T>::Remove(DqnString key)
{
    Entry result = this->Remove(key.str, key.len);
}

template <typename T>
void DqnHashTable<T>::Clear()
{
    for (auto usedIndex = 0; usedIndex < this->usedEntriesIndex; usedIndex++)
    {
        auto entryIndex  = this->usedEntries[usedIndex];
        Entry *baseEntry = this->entries[entryIndex];

        for (Entry *entry = baseEntry; entry;)
        {
            Entry *entryToFree = entry;
            entry              = entryToFree->next;

            entryToFree->key.Clear();
            entryToFree->data = {};
            entryToFree->next = this->freeList;
            this->freeList    = entryToFree;
            this->numFreeEntries++;
        }

        this->entries[entryIndex] = nullptr;
    }

    this->usedEntriesIndex = 0;
}

template <typename T>
void DqnHashTable<T>::Free()
{
    usize const ENTRY_SIZE = sizeof(*this->entries);
    for (i64 i = 0; i < usedEntriesIndex; i++)
    {
        i64 indexToFree    = usedEntries[i];
        Entry *entryToFree = *(this->entries + indexToFree);

        entryToFree->key.Free();
        this->memAPI->Free(entryToFree, ENTRY_SIZE);
    }

    // Free usedEntries list
    {
        usize sizeToFree = sizeof(*this->usedEntries) * this->numEntries;
        this->memAPI->Free(this->usedEntries, sizeToFree);
    }

    // Free freeList
    {
        Entry *entry = this->freeList;
        while (entry)
        {
            Entry *entryToFree = entry;
            entry              = entry->next;

            entryToFree->key.Free();
            this->memAPI->Free(entryToFree, ENTRY_SIZE);
        }
    }

    // Free the array of ptrs
    {
        usize sizeToFree = ENTRY_SIZE * this->numEntries;
        this->memAPI->Free(this->entries, sizeToFree);
    }

}

template <typename T>
bool DqnHashTable<T>::AddNewEntriesToFreeList(i64 num)
{
    if (num < 0)
    {
        num = DQN_ABS(num);
        for (i64 i = 0; i < num; i++)
        {
            Entry *entryToFree = this->freeList;
            if (entryToFree)
            {
                this->freeList = entryToFree->next;
                this->memAPI->Free(entryToFree, sizeof(*this->freeList));
            }
        }

        this->numFreeEntries -= num;
    }
    else
    {
        for (i64 i = 0; i < num; i++)
        {
            Entry *newEntry = DqnHashTableInternal_AllocateEntry(this);
            if (!newEntry) return false;

            newEntry->next = this->freeList;
            this->freeList = newEntry;
        }

        this->numFreeEntries += num;
    }

    DQN_ASSERT(this->numFreeEntries >= 0);
    return true;
}

template <typename T>
bool DqnHashTable<T>::ChangeNumEntries(i64 newNumEntries)
{
    if (newNumEntries == this->numEntries) return true;

    Entry **newEntries    = {};
    usize newEntriesSize = sizeof(*this->entries) * newNumEntries;

    i64 *newUsedEntries       = {};
    usize newUsedEntriesSize = sizeof(*this->usedEntries) * newNumEntries;
    i64 newUsedEntriesIndex   = 0;

    // NOTE: If you change allocation order, be sure to change the free order.
    // Allocate newEntries
    {
        auto *newEntriesPtr = (u8 *)this->memAPI->Alloc(newEntriesSize);
        if (!newEntriesPtr) return false;

        newEntries = (Entry **)newEntriesPtr;
    }

    // Allocate usedEntries
    {
        auto *usedEntriesPtr    = (u8 *)this->memAPI->Alloc(newUsedEntriesSize);
        if (!usedEntriesPtr)
        {
            this->memAPI->Free(newEntries, newEntriesSize);
            return false;
        }

        newUsedEntries = (i64 *)usedEntriesPtr;
    }

    for (i64 i = 0; i < this->usedEntriesIndex; i++)
    {
        i64 usedIndex   = this->usedEntries[i];
        Entry *oldEntry = this->entries[usedIndex];

        while (oldEntry)
        {
            i64 newHashIndex = DqnHashTableInternal_GetHashIndex(newNumEntries, oldEntry->key.str,
                                                                 oldEntry->key.len);
            Entry *entryToAppendTo = newEntries[newHashIndex];

            if (entryToAppendTo)
            {
                while (entryToAppendTo->next)
                    entryToAppendTo = entryToAppendTo->next;

                entryToAppendTo->next = oldEntry;
            }
            else
            {
                newEntries[newHashIndex]              = oldEntry;
                newUsedEntries[newUsedEntriesIndex++] = newHashIndex;
            }

            oldEntry = oldEntry->next;
        }
    }

    // Free the old entry list
    {
        usize freeSize = sizeof(*this->entries) * this->numEntries;
        this->memAPI->Free(this->entries, freeSize);
    }

    // Free the old used entry list
    {
        usize freeSize = sizeof(*this->usedEntries) * this->numEntries;
        this->memAPI->Free(this->usedEntries, freeSize);
    }

    this->entries          = newEntries;
    this->numEntries       = newNumEntries;
    this->usedEntries      = newUsedEntries;
    this->usedEntriesIndex = newUsedEntriesIndex;
    return true;
}

struct DqnJson
{
    enum struct Type
    {
        Object,
        Array,
    };

    Type           type;
    DqnSlice<char> value;
    i32            numEntries;

    operator bool () const { return (value.data != nullptr); }
    i64      ToI64() const { return Dqn_StrToI64(value.data, value.len); }
};

// Zero allocation json finder. Returns the data of the value.
// If array, it returns a slice from [..] not-inclusive, if object, it returns a slice from {..} not-inclusive
// If just name value pair, it returns the literal with quotes or just the value if it is a primitive with quotes.
DQN_FILE_SCOPE DqnJson DqnJson_Get             (char const *buf, i32 bufLen, char const *findProperty, i32 findPropertyLen);
DQN_FILE_SCOPE DqnJson DqnJson_Get             (DqnSlice<char>       const buf, DqnSlice<char>       const findProperty);
DQN_FILE_SCOPE DqnJson DqnJson_Get             (DqnSlice<char>       const buf, DqnSlice<char const> const findProperty);
DQN_FILE_SCOPE DqnJson DqnJson_Get             (DqnSlice<char const> const buf, DqnSlice<char const> const findProperty);
DQN_FILE_SCOPE DqnJson DqnJson_Get             (DqnJson const input, DqnSlice<char const> const findProperty);
DQN_FILE_SCOPE DqnJson DqnJson_Get             (DqnJson const input, DqnSlice<char>       const findProperty);

// return:   The array item.
DQN_FILE_SCOPE DqnJson DqnJson_GetNextArrayItem(DqnJson *iterator);

#endif  /* DQN_H */

// #XPlatform (Win32 & Unix) API
// =================================================================================================
// Functions in the Cross Platform are guaranteed to be supported in both Unix
// and Win32

// NOTE(doyle): DQN_PLATFORM_HEADER is enabled by the user to have the function prototypes be
// visible. DQN_PLATFORM_H is like a normal header guard that ensures singular declaration of
// functions.
#ifdef DQN_PLATFORM_HEADER

#ifndef DQN_PLATFORM_H
#define DQN_PLATFORM_H

#if defined(DQN__IS_UNIX)
    #include <pthread.h>
    #include <semaphore.h>
#endif

// XPlatform > #DqnFile API
// =================================================================================================
struct DqnFile
{
    enum PermissionFlag
    {
        FileRead  = (1 << 0),
        FileWrite = (1 << 1),
        Execute   = (1 << 2),
        All       = (1 << 3)
    };

    enum class Action
    {
        OpenOnly,         // Only open file if it exists. Fails and returns false if file did not exist or could not open.
        CreateIfNotExist, // Try and create file. Return true if it was able to create. If it already exists, this fails.
        ClearIfExist,     // Clear the file contents to zero if it exists. Fails and returns false if file does not exist.
        ForceCreate,      // Always create, even if it exists
    };

    struct Info
    {
        usize size;
        u64 createTimeInS;
        u64 lastWriteTimeInS;
        u64 lastAccessTimeInS;
    };

    u32    flags;
    void  *handle;
    usize  size;

    // API
    // ==============================================================================================
    // NOTE: W(ide) versions of functions only work on Win32, since Unix is already UTF-8 compatible.

    // Open a handle for file read and writing. Deleting files does not need a handle. Handles should be
    // closed before deleting files otherwise the OS may not be able to delete the file.
    // return: FALSE if invalid args or failed to get handle (i.e. insufficient permissions)
    bool   Open(char    const *path, u32 const flags_, Action const action);
    bool   Open(wchar_t const *path, u32 const flags_, Action const action);

    // fileOffset: The byte offset to starting writing from.
    // return:     The number of bytes written. 0 if invalid args or it failed to write.
    usize  Write(u8 const *buf, usize const numBytesToWrite, usize const fileOffset);

    // IMPORTANT: You may want to allocate size+1 for null-terminating the file contents when
    // reading into a buffer.
    // return: The number of bytes read. 0 if invalid args or it failed to read.
    usize  Read (u8 *buf, usize const numBytesToRead);

    // File close invalidates the handle after it is called.
    void   Close();

    // Static API
    // ==============================================================================================
    // Read entire file into the given buffer. To determine required bufSize size, use GetFileSize.
    // NOTE: You want size + 1 and add the null-terminator yourself if you want a null terminated buffer.
    // bytesRead: Pass in to get how many bytes of the buf was used. Basically the return value of Read
    // return:    False if insufficient bufSize OR file access failure OR nullptr arguments.
    static bool   ReadEntireFile(char    const *path, u8 *buf, usize const bufSize, usize *bytesRead);
    static bool   ReadEntireFile(wchar_t const *path, u8 *buf, usize const bufSize, usize *bytesRead);

    // Buffer is null-terminated and should be freed when done with.
    // return: False if file access failure OR nullptr arguments.
    static u8    *ReadEntireFile(char    const *path, usize *bufSize, DqnMemAPI *api = DQN_DEFAULT_HEAP_ALLOCATOR);
    static u8    *ReadEntireFile(wchar_t const *path, usize *bufSize, DqnMemAPI *api = DQN_DEFAULT_HEAP_ALLOCATOR);

    // return: False if file access failure OR nullptr arguments.
    static bool   GetFileSize   (char    const *path, usize *size);
    static bool   GetFileSize   (wchar_t const *path, usize *size);

    // info:   Pass in to fill with file attributes.
    // return: False if file access failure OR nullptr arguments.
    static bool   GetInfo       (char    const *path, Info *info);
    static bool   GetInfo       (wchar_t const *path, Info *info);

    // NOTE: You can't delete a file unless the handle has been closed to it on Win32.
    // return: False if file access failure OR nullptr arguments.
    static bool   Delete        (char    const *path);
    static bool   Delete        (wchar_t const *path);
    static bool   Copy          (char    const *src, char    const *dest);
    static bool   Copy          (wchar_t const *src, wchar_t const *dest);

    // NOTE: Win32: Current directory is "*", Unix: "."
    // numFiles: Pass in a ref to a i32. The function fills it out with the number of entries.
    // return:   An array of strings of the files in the directory in UTF-8. The directory lisiting is
    //           allocated with malloc and must be freed using free() or the helper function ListDirFree()
    static char **ListDir       (char const *dir, i32 *numFiles, DqnMemAPI *api = DQN_DEFAULT_HEAP_ALLOCATOR);
    static void   ListDirFree   (char **fileList, i32 numFiles, DqnMemAPI *api = DQN_DEFAULT_HEAP_ALLOCATOR);
};

struct DqnSmartFile : public DqnFile
{
    ~DqnSmartFile() { this->Close(); }
};

// XPlatform > #DqnTimer API
// =================================================================================================
DQN_FILE_SCOPE f64  DqnTimer_NowInMs();
DQN_FILE_SCOPE f64  DqnTimer_NowInS ();

// XPlatform > #DqnLock API
// =================================================================================================
typedef struct DqnLock
{
#if defined(DQN__IS_WIN32)
    CRITICAL_SECTION win32Handle;
#else
    pthread_mutex_t  unixHandle;
#endif

    // Win32 only, when trying to acquire a locked lock, it is the number of spins permitted
    // spinlocking on the lock before being blocked. Set before init if you want a different value.
    u32 win32SpinCount = 16000;

    bool Init   ();
    void Acquire();
    void Release();
    void Delete ();

    // Create a lock guard on the lock this is invoked on.
    struct DqnLockGuard LockGuard();
} DqnLock;

// Lock guard automatically acquires a lock on construction and releases the associated lock on
// destruction. If the lock is unable to be acquired, the program blocks at construction until it
// can.
struct DqnLockGuard
{
    // lock: Takes a pointer to a pre-existing and already initialised lock
    // bool: Pass in (optionally) a pointer to a bool which returns whether a lock was successful.
    // FALSE if lock is nullptr.
     DqnLockGuard(DqnLock *const lock_, bool *const succeeded);
    ~DqnLockGuard();

private:
    DqnLock *lock;
};

// lock: Pass in a pointer to a default DqnLock struct.
//       In Win32, you may optionally change the win32Spincount.
DQN_FILE_SCOPE bool DqnLock_Init   (DqnLock *const lock);
DQN_FILE_SCOPE void DqnLock_Acquire(DqnLock *const lock);
DQN_FILE_SCOPE void DqnLock_Release(DqnLock *const lock);
DQN_FILE_SCOPE void DqnLock_Delete (DqnLock *const lock);

// XPlatform > #DqnJobQueue API
// =================================================================================================
// DqnJobQueue is a platform abstracted "lockless" multithreaded work queue. It will create threads
// and assign threads to complete the job via the job "callback" using the "userData" supplied.

// Usage
// 1. Prepare your callback function for threads to execute following the 'DqnJob_Callback' function
//    signature.
// 2. Create a job queue with DqnJobQueue_InitWithMem()
// 3. Add jobs with DqnJobQueue_AddJob() and threads will be dispatched automatically.

// When all jobs are sent you can also utilise the main executing thread to complete jobs whilst you
// wait for all jobs to complete using DqnJobQueue_TryExecuteNextJob() or spinlock on
// DqnJobQueue_AllJobsComplete(). Alternatively you can combine both for the main thread to help
// complete work and not move on until all tasks are complete.

typedef void DqnJob_Callback(struct DqnJobQueue *const queue, void *const userData);
struct DqnJob
{
    DqnJob_Callback *callback;
    void            *userData;
};

struct DqnJobQueue
{
    // JobList Circular Array, is setup in Init()
    DqnJob *jobList;
    u32     size;

    // NOTE(doyle): Modified by main+worker threads
    i32   volatile jobToExecuteIndex;
    i32   volatile numJobsToComplete;

#if defined(DQN__IS_WIN32)
    void *semaphore;
#else
    sem_t semaphore;
#endif

    // NOTE: Modified by main thread ONLY
    i32 volatile jobInsertIndex;

    bool Init             (DqnJob *const jobList_, const u32 jobListSize, const u32 numThreads);
    bool AddJob           (const DqnJob job);

    void BlockAndCompleteAllJobs();
    bool TryExecuteNextJob();
    bool AllJobsComplete  ();
};

// TODO(doyle): Queue delete, thread delete

// queue:       Pass a pointer to a zero cleared DqnJobQueue struct
// jobList:     Pass in a pointer to an array of DqnJob's
// jobListSize: The number of elements in the jobList array
// numThreads:  The number of threads the queue should request from the OS for working on the queue
// return:      FALSE if invalid args i.e. nullptr ptrs or jobListSize & numThreads == 0
DQN_FILE_SCOPE bool DqnJobQueue_Init(DqnJobQueue *const queue, const DqnJob *const jobList,
                                     const u32 jobListSize, const u32 numThreads);

// return: FALSE if the job is not able to be added, this occurs if the queue is full.
DQN_FILE_SCOPE bool DqnJobQueue_AddJob(DqnJobQueue *const queue, const DqnJob job);

// Helper function that combines TryExecuteNextJob() and AllJobsComplete(), i.e.
// complete all work before moving on. Does nothing if queue is nullptr.
DQN_FILE_SCOPE void DqnJobQueue_BlockAndCompleteAllJobs(DqnJobQueue *const queue);

// return: TRUE if there was a job to execute (the calling thread executes it). FALSE if it could
//         not get a job. It may return FALSE whilst there are still jobs, this means that another thread
//         has taken the job before the calling thread could and should NOT be used to determine if there
//         are any remaining jobs left. That can only be definitively known using
//         DqnJobQueue_AllJobsComplete(). This is typically combined like so ..
//         while (DqnJobQueue_TryExecuteNextJob(queue) || !DqnJobQueue_AllJobsComplete(queue));
//         Return FALSE also if queue is a nullptr pointer.
DQN_FILE_SCOPE bool DqnJobQueue_TryExecuteNextJob(DqnJobQueue *const queue);
DQN_FILE_SCOPE bool DqnJobQueue_AllJobsComplete  (DqnJobQueue *const queue);

// XPlatform > #DqnAtomic API
// =================================================================================================
// All atomic operations generate a full read/write barrier. This is implicitly enforced by the
// OS calls, not explicitly in my code.

// swapVal:    The value to put into "dest", IF at point of read, "dest" has the value of "compareVal"
// compareVal: The value to check in "dest"
// return:     Return the original value that was in "dest"
DQN_FILE_SCOPE i32 DqnAtomic_CompareSwap32(i32 volatile *const dest, const i32 swapVal, const i32 compareVal);

// Add "value" to src
// return: The new value at src
DQN_FILE_SCOPE i32 DqnAtomic_Add32(i32 volatile *const src, const i32 value);

// XPlatform > #DqnOS API
// =================================================================================================
// Uses a single call to DqnMem_Calloc() and DqnMem_Free(). Not completely platform "independent" for Unix.
// numCores: numThreadsPerCore: Can be nullptr, the function will just skip it.
DQN_FILE_SCOPE void DqnOS_GetThreadsAndCores(u32 *const numCores, u32 *const numThreadsPerCore);

// #Platform Specific API
// =================================================================================================
// Functions here are only available for the #defined sections (i.e. all functions in
// DQN__IS_WIN32 only have a valid implementation in Win32.

#if defined(DQN__IS_WIN32)
// Platform > #DqnWin32 API
// =================================================================================================
#define DQN__WIN32_ERROR_BOX(text, title) MessageBoxA(nullptr, text, title, MB_OK);

// The function automatically null-terminates the output string.
// out:    A pointer to the buffer to receive the characters.
// outLen: The length/capacity of the buffer "out". If 0, the function returns the required length including null terminator.
// return: -1 if invalid, or if outLen is 0 the required buffer length.
DQN_FILE_SCOPE i32 DqnWin32_UTF8ToWChar(const char    *const in, wchar_t *const out, const i32 outLen);
DQN_FILE_SCOPE i32 DqnWin32_WCharToUTF8(const wchar_t *const in, char    *const out, const i32 outLen);

// "width" and "height" are optional and won't be used if not given by user.
// width & height: Pass in a pointer for function to fill out.
DQN_FILE_SCOPE void DqnWin32_GetClientDim     (HWND const window, LONG *width, LONG *height);
DQN_FILE_SCOPE void DqnWin32_GetRectDim       (RECT const rect,   LONG *width, LONG *height);

// Displays error in the format <errorPrefix>: <Win32 Error Message> in a Win32 Dialog Box.
// errorPrefix: The message before the Win32 error, can be nullptr
DQN_FILE_SCOPE void DqnWin32_DisplayLastError (const char *const errorPrefix);

// Asimilar to DqnWin32_DisplayLastError() a particular error can be specified in a Win32 Dialog Box.
DQN_FILE_SCOPE void DqnWin32_DisplayErrorCode (const DWORD error, const char *const errorPrefix);

// Output text to the debug console. For visual studio this is the output window and not the console.
// ...: Variable args alike printf, powered by stb_sprintf
DQN_FILE_SCOPE void DqnWin32_OutputDebugString(const char *const formatStr, ...);

// Get the full path of to the current processes executable, and return the char offset in the
// string to the last backslash, i.e. the directory.
// buf:    Filled with the path to the executable file.
// return: The offset to the last backslash. -1 if bufLen was not large enough or buf is null. (i.e.
//         buf + offsetToLastSlash + 1, gives C:/Path/)
DQN_FILE_SCOPE i32  DqnWin32_GetEXEDirectory(char *const buf, const u32 bufLen);
#endif // DQN__IS_WIN32
#endif // DQN_PLATFORM_H
#endif // DQN_PLATFORM_HEADER


#ifdef DQN_IMPLEMENTATION
// DQN_IMPLEMENTATION
// =================================================================================================
#include <math.h>    // TODO(doyle): For trigonometry functions (for now)
#include <stdlib.h>  // For calloc, malloc, free
#include <stdio.h>   // For printf

// NOTE: STB_SPRINTF is included when DQN_IMPLEMENTATION defined
// #define STB_SPRINTF_IMPLEMENTATION

// NOTE: DQN_INI_IMPLEMENTATION modified to be included when DQN_IMPLEMENTATION defined
// #define DQN_INI_IMPLEMENTATION
#define DQN_INI_STRLEN(s) DqnStr_Len(s)

// #DqnLog
// =================================================================================================
DQN_FILE_SCOPE void DqnLog(char const *file, char const *functionName, i32 lineNum,
                           char const *msg, ...)
{
    // TODO(doyle): Compress this
    auto fileLen = DqnStr_Len(file);
    for (auto i = fileLen - 1; i >= 0; i--)
    {
        if (file[i] == '\\' || file[i] == '/')
        {
            file = file + i + 1;
            break;
        }
    }

    char userMsg[2048];
    userMsg[0] = '\0';

    va_list argList;
    va_start(argList, msg);
    {
        u32 numCopied = Dqn_vsprintf(userMsg, msg, argList);
        if (numCopied > DQN_ARRAY_COUNT(userMsg))
        {
            (*((int *)0)) = 0;
        }
    }
    va_end(argList);

    char const *const formatStr = "%s:%s,%d: DqnLog: %s\n";
    fprintf(stderr, formatStr, file, functionName, lineNum, userMsg);

    #if defined(DQN__IS_WIN32)
        DqnWin32_OutputDebugString(formatStr, file, functionName, lineNum, userMsg);
    #endif
}

DQN_FILE_SCOPE void DqnLogExpr(char const *file, char const *functionName, i32 lineNum,
                               char const *expr, char const *msg, ...)
{
    auto fileLen = DqnStr_Len(file);
    for (auto i = fileLen - 1; i >= 0; i--)
    {
        if (file[i] == '\\' || file[i] == '/')
        {
            file = file + i + 1;
            break;
        }
    }

    char userMsg[2048];
    userMsg[0] = '\0';

    va_list argList;
    va_start(argList, msg);
    {
        u32 numCopied = Dqn_vsprintf(userMsg, msg, argList);
        if (numCopied > DQN_ARRAY_COUNT(userMsg))
        {
            (*((int *)0)) = 0;
        }
    }
    va_end(argList);

    char const *const formatStr = ":%s:%s,%d(%s): DqnLog: %s\n";
    fprintf(stderr, formatStr, file, functionName, lineNum, expr, userMsg);

    #if defined(DQN__IS_WIN32)
        DqnWin32_OutputDebugString(formatStr, file, functionName, lineNum, expr, userMsg);
    #endif
}

// #DqnMemory
// =================================================================================================
// NOTE: All memory allocations in dqn.h go through these functions. So they can
// be rerouted fairly easily especially for platform specific mallocs.
DQN_FILE_SCOPE void *DqnMem_Alloc(usize size)
{
    void *result = malloc(size);
    return result;
}

DQN_FILE_SCOPE void *DqnMem_Calloc(usize size)
{
    void *result = calloc(1, size);
    return result;
}

DQN_FILE_SCOPE void DqnMem_Clear(void *memory, u8 clearValue, usize size)
{
    if (memory)
    {
        DqnMem_Set(memory, clearValue, size);
    }
}

DQN_FILE_SCOPE void *DqnMem_Realloc(void *memory, usize newSize)
{
    void *result = realloc(memory, newSize);
    return result;
}

DQN_FILE_SCOPE void DqnMem_Free(void *memory)
{
    if (memory) free(memory);
}

DQN_FILE_SCOPE void DqnMem_Copy(void *dest, void const *src, usize numBytesToCopy)
{
    auto *to   = (u8 *)dest;
    auto *from = (u8 *)src;
    for (usize i = 0; i < numBytesToCopy; i++)
        to[i]   = from[i];
}

DQN_FILE_SCOPE void *DqnMem_Set(void *dest, u8 value, usize numBytesToSet)
{
    auto volatile *ptr = (u8 *)dest; // NOTE: Volatile so memset is not optimised out.
    for (usize i = 0; i < numBytesToSet; i++)
        ptr[i]  = value;

    return dest;
}

// #DqnMemAPI
// =================================================================================================
FILE_SCOPE void DqnMemAPIInternal_ValidateRequest(DqnMemAPI::Request request_)
{
    DQN_ASSERT(request_.type != DqnMemAPI::Type::Uninitialised);

    if (request_.type == DqnMemAPI::Type::Alloc)
    {
        auto *request = &request_.e.alloc;
        DQN_ASSERT(request->requestSize > 0);
        return;
    }

    if (request_.type == DqnMemAPI::Type::Realloc)
    {
        auto *request = &request_.e.realloc;
        DQN_ASSERT(request->oldSize > 0);
        DQN_ASSERT(request->newSize > 0);
        DQN_ASSERT((request->newSize - request->oldSize) != 0);
        DQN_ASSERT(request->oldMemPtr);
        return;
    }
}

FILE_SCOPE void DqnMemAPIInternal_UpdateAPIStatistics(DqnMemAPI *api, DqnMemAPI::Request *request_)
{
    if (request_->type == DqnMemAPI::Type::Alloc)
    {
        auto *request = &request_->e.alloc;
        api->bytesAllocated += request->requestSize;
        api->lifetimeBytesAllocated += request->requestSize;
        return;
    }

    if (request_->type == DqnMemAPI::Type::Realloc)
    {
        auto *request = &request_->e.realloc;
        api->lifetimeBytesAllocated += request->newSize;
        api->lifetimeBytesFreed += request->oldSize;

        api->bytesAllocated += request->newSize;
        api->bytesAllocated -= request->oldSize;
        return;
    }

    if (request_->type == DqnMemAPI::Type::Free)
    {
        auto *request = &request_->e.free;
        api->bytesAllocated     -= request->sizeToFree;
        api->lifetimeBytesFreed += request->sizeToFree;
        return;
    }
}

FILE_SCOPE u8 *DqnMemAPIInternal_HeapAllocatorCallback(DqnMemAPI *api, DqnMemAPI::Request request_)
{
    DqnMemAPIInternal_ValidateRequest(request_);
    DQN_ASSERT(!request_.userContext);

    u8 *result   = nullptr;
    bool success = false;

    if (request_.type == DqnMemAPI::Type::Alloc)
    {
        auto const *request = &request_.e.alloc;

        if (request->zeroClear) result = (u8 *)DqnMem_Calloc(request->requestSize);
        else                    result = (u8 *)DqnMem_Alloc(request->requestSize);

        success = (result != nullptr);
    }
    else if (request_.type == DqnMemAPI::Type::Realloc)
    {
        auto const *request = &request_.e.realloc;
        if (request->newSize == request->oldSize)
        {
            result = (u8 *)request->oldMemPtr;
        }
        else
        {
            result  = (u8 *)DqnMem_Realloc(request->oldMemPtr, request->newSize);
            success = (result != nullptr);
        }
    }
    else if (request_.type == DqnMemAPI::Type::Free)
    {
        auto *request = &request_.e.free;
        DqnMem_Free(request->ptrToFree);
        success = (request->ptrToFree != nullptr);
    }
    else
    {
        DQN_ASSERT(DQN_INVALID_CODE_PATH);
    }

    if (success)
    {
        DqnMemAPIInternal_UpdateAPIStatistics(api, &request_);
    }

    return result;
}

struct DqnMemAPIInternal_DqnMemStackContext
{
    enum Mode
    {
        PushToHead,
        PushToTail,
    };

    DqnMemStack *stack;
    Mode         mode;
};

FILE_SCOPE u8 *DqnMemAPIInternal_StackAllocatorCallback(DqnMemAPI *api, DqnMemAPI::Request request_,
                                                        bool pushToHead)
{
    DqnMemAPIInternal_ValidateRequest(request_);
    DQN_ASSERT(request_.userContext);

    auto *const stack = (DqnMemStack *)(request_.userContext);
    u8 *result        = nullptr;
    bool success      = false;

    enum class PtrType
    {
        NotInCurrentBlock,
        Head,
        Tail,
    };

    auto ClassifyPtr = [](DqnMemStack::Block const *block, u8 const *ptr) -> PtrType {

        PtrType result           = PtrType::NotInCurrentBlock;
        u8 const *const blockEnd = block->memory + block->size;
        if (ptr >= block->memory && ptr < block->head)
        {
            result = PtrType::Head;
        }
        else if (ptr >= block->tail && ptr < blockEnd)
        {
            result = PtrType::Tail;
        }

        return result;
    };

    auto PtrIsLastAllocationInBlock = [&ClassifyPtr](DqnAllocatorMetadata const *metadata,
                                                     DqnMemStack::Block const *block,
                                                     u8 const *ptr) -> bool {
        PtrType type = ClassifyPtr(block, ptr);
        bool result  = false;
        if (type == PtrType::Head)
        {
            isize const oldMemSize = *(metadata->PtrToAllocAmount(ptr));
            u8 const *ptrEnd       = ptr + oldMemSize + metadata->GetAllocTailSize();
            result                 = (ptrEnd == (block->head - 1));
        }
        else if (type == PtrType::Tail)
        {
            u8 offsetToSrc  = *(metadata->PtrToOffsetToSrc(ptr));
            auto *actualPtr = ptr - offsetToSrc;
            result          = (actualPtr == block->tail);
        }

        return result;
    };

    if (request_.type == DqnMemAPI::Type::Alloc)
    {
        auto *request = &request_.e.alloc;
        if (pushToHead) result = (u8 *)stack->Push(request->requestSize);
        else            result = (u8 *)stack->PushOnTail(request->requestSize);

        if (result)
        {
            success = true;
            if (request->zeroClear) DqnMem_Clear(result, 0, request->requestSize);
        }
    }
    else if (request_.type == DqnMemAPI::Type::Realloc)
    {
        // IMPORTANT: This is a _naive_ realloc scheme for stack allocation.
        auto *request = &request_.e.realloc;
        u8 *const ptr = (u8 *)request->oldMemPtr;
        for (DqnMemStack::Block *block = stack->block; block; block = block->prevBlock)
        {
            DQN_ASSERT(ptr >= block->memory && ptr <= (block->memory + block->size));
        }

        DqnMemStack::Block *const block = stack->block;
        isize const oldMemSize          = *stack->metadata.PtrToAllocAmount(ptr);
        isize const extraBytesReq       = request->newSize - oldMemSize;
        u8 const alignment              = *stack->metadata.PtrToAlignment(ptr);
        DQN_ASSERT(extraBytesReq > 0);

        PtrType type = ClassifyPtr(block, ptr);
        if (PtrIsLastAllocationInBlock(&stack->metadata, block, ptr))
        {
            bool enoughSpace = false;
            if (type == PtrType::Head)
            {
                DQN_ASSERT((block->head + extraBytesReq) >= block->memory);

                enoughSpace = (block->head + extraBytesReq) < block->tail;
                if (enoughSpace)
                {
                    stack->Pop(ptr, Dqn::ZeroClear::No);
                    result = (u8 *)stack->Push(request->newSize, alignment);
                    DQN_ASSERT(stack->block == block && result == request->oldMemPtr);
                    success = true;
                }
            }
            else
            {
                DQN_ASSERT(type == PtrType::Tail);
                DQN_ASSERT((block->tail - extraBytesReq) < (block->memory + block->size));

                enoughSpace = (block->tail - extraBytesReq) > block->head;
                if (enoughSpace)
                {
                    stack->PopOnTail(ptr, Dqn::ZeroClear::No);
                    result = (u8 *)stack->PushOnTail(request->newSize, alignment);
                    DqnMem_Copy(result, ptr, oldMemSize);
                    result[oldMemSize] = 0;

                    success = true;
                    DQN_ASSERT(stack->block == block);
                }
            }

            if (!enoughSpace)
            {
                // TODO(doyle): Does realloc need clear to zero flag as well?
                // Else, last allocation but not enough space in block. Create a new block and
                // copy
                DqnMemStack::Block *oldBlock = block;
                if (type == PtrType::Head)
                {
                    result = (u8 *)stack->Push(request->newSize, alignment);
                }
                else
                {
                    DQN_ASSERT(type == PtrType::Tail);
                    result = (u8 *)stack->PushOnTail(request->newSize, alignment);
                }

                if (result)
                {
                    DQN_ASSERT(stack->block->prevBlock == oldBlock);
                    DQN_ASSERT(stack->block != oldBlock);
                    DqnMem_Copy(result, ptr, oldMemSize);

                    // Switch to old block, pop the ptr and return the new block on top.
                    auto *newBlock = stack->block;
                    stack->block   = oldBlock;

                    if (type == PtrType::Head) stack->Pop(ptr, Dqn::ZeroClear::No);
                    else                       stack->PopOnTail(ptr, Dqn::ZeroClear::No);
                    stack->block = newBlock;
                    success      = true;
                }
            }
        }
        else
        {
            if (request->newSize < request->oldSize)
            {
                // NOTE: This is questionable behaviour. We don't reclaim data since it's not
                // well-defined in a stack allocator. This would cause gaps in memory.
                success = false; // Deny updating statistics.
                result  = ptr;
            }
            else
            {
                DQN_LOGE(
                    "Lost %$_d, the ptr to realloc is sandwiched between other allocations (LIFO)",
                    oldMemSize);

                if (type == PtrType::Head)
                {
                    result = (u8 *)stack->Push(request->newSize, alignment);
                }
                else
                {
                    DQN_ASSERT(type == PtrType::Tail);
                    result = (u8 *)stack->PushOnTail(request->newSize, alignment);
                }

                if (result)
                {
                    success = true;
                    DqnMem_Copy(result, ptr, oldMemSize);
                }
            }
        }
    }
    else
    {
        auto *request = &request_.e.free;
        DQN_ASSERT(request_.type == DqnMemAPI::Type::Free);

        DqnMemStack::Block *const block = stack->block;
        u8 *const ptr                   = (u8 *)request->ptrToFree;

        if (PtrIsLastAllocationInBlock(&stack->metadata, block, ptr))
        {
            PtrType type = ClassifyPtr(block, ptr);
            if (type == PtrType::Head) stack->Pop(ptr, Dqn::ZeroClear::No);
            else                       stack->PopOnTail(ptr, Dqn::ZeroClear::No);
        }
        else
        {
            isize const oldMemSize = *(stack->metadata.PtrToAllocAmount(ptr));
            DQN_LOGE("Lost %$_d, the ptr to free is sandwiched between other allocations (LIFO)", oldMemSize);
        }
    }

    // TODO(doyle): Stats. Irrelevant now?
    (void)api;
    if (success)
    {
#if 0
        DqnMemAPIInternal_UpdateAPIStatistics(api, &request_);
#endif
    }

    return result;
}

FILE_SCOPE u8 *DqnMemAPIInternal_StackAllocatorCallbackPushToHead(DqnMemAPI *api, DqnMemAPI::Request request_)
{
    u8 *result = DqnMemAPIInternal_StackAllocatorCallback(api, request_, true);
    return result;
}

FILE_SCOPE u8 *DqnMemAPIInternal_StackAllocatorCallbackPushToTail(DqnMemAPI *api, DqnMemAPI::Request request_)
{
    u8 *result = DqnMemAPIInternal_StackAllocatorCallback(api, request_, false);
    return result;
}

void *DqnMemAPI::Realloc(void *const oldPtr, isize oldSize, isize newSize)
{
    Request request             = {};
    request.type                = Type::Realloc;
    request.userContext         = this->userContext;
    request.e.realloc.newSize   = newSize;
    request.e.realloc.oldMemPtr = oldPtr;
    request.e.realloc.oldSize   = oldSize;

    void *result = (void *)this->allocator(this, request);
    return result;
}

void *DqnMemAPI::Alloc(isize size, Dqn::ZeroClear clear)
{
    Request request             = {};
    request.type                = Type::Alloc;
    request.userContext         = this->userContext;
    request.e.alloc.zeroClear   = (clear == Dqn::ZeroClear::Yes) ? true : false;
    request.e.alloc.requestSize = size;

    void *result = (void *)this->allocator(this, request);
    return result;
}

void DqnMemAPI::Free(void *const ptrToFree, isize sizeToFree)
{
    Request request           = {};
    request.type              = Type::Free;
    request.userContext       = this->userContext;
    request.e.free.ptrToFree  = ptrToFree;
    request.e.free.sizeToFree = sizeToFree;
    this->allocator(this, request);
}

DqnMemAPI DqnMemAPI::HeapAllocator()
{
    DqnMemAPI result   = {0};
    result.allocator   = DqnMemAPIInternal_HeapAllocatorCallback;
    result.userContext = nullptr;
    return result;
}

DqnMemAPI DqnMemAPI::StackAllocator(struct DqnMemStack *stack, StackPushType type)
{
    DQN_ASSERT(stack);
    DqnMemAPI result   = {0};
    result.allocator   = (type == StackPushType::Head)
                           ? DqnMemAPIInternal_StackAllocatorCallbackPushToHead
                           : DqnMemAPIInternal_StackAllocatorCallbackPushToTail;
    result.userContext = stack;
    return result;
}

// #DqnAllocatorMetadata
// =================================================================================================
void DqnAllocatorMetadata::Init(bool boundsGuard)
{
    // TODO(doyle): How to handle memory here.
    if (boundsGuard)
    {
        this->boundsGuardSize        = sizeof(HEAD_GUARD_VALUE);
        LOCAL_PERSIST DqnMemAPI heap = DqnMemAPI::HeapAllocator();
        this->allocations.memAPI     = &heap;
        this->allocations.Reserve(128);
    }
    else
    {
        this->boundsGuardSize = 0;
    }

    this->allocHeadSize = OFFSET_TO_SRC_SIZE + ALIGNMENT_SIZE + ALLOC_AMOUNT_SIZE + this->boundsGuardSize;
    this->allocTailSize = this->boundsGuardSize;
}

void DqnAllocatorMetadata::Free()
{
    this->allocations.Free();
}

void DqnAllocatorMetadata::RemoveAllocation(u8 *ptr)
{
    i32 deleteIndex = -1;
    for (auto i = 0; i < this->allocations.count; i++)
    {
        if (allocations.data[i] == ptr)
        {
            deleteIndex = i;
            break;
        }
    }

    DQN_ASSERT(deleteIndex != -1);
    this->allocations.EraseStable(deleteIndex);
}

void DqnAllocatorMetadata::CheckAllocations() const
{
    for (auto index = 0; index < this->allocations.count; index++)
    {
        u8 const *ptr = this->allocations.data[index];

        u32 const *headGuard = this->PtrToHeadBoundsGuard(ptr);
        u32 const *tailGuard = this->PtrToTailBoundsGuard(ptr);

        DQN_ASSERTM(*headGuard == HEAD_GUARD_VALUE,
                    "Bounds guard has been destroyed at the head end of the allocation! Expected: "
                    "%x, received: %x",
                    HEAD_GUARD_VALUE, *headGuard);

        DQN_ASSERTM(*tailGuard == TAIL_GUARD_VALUE,
                    "Bounds guard has been destroyed at the tail end of the allocation! Expected: "
                    "%x, received: %x",
                    TAIL_GUARD_VALUE, *tailGuard);
    }
}


u32 *DqnAllocatorMetadata::PtrToHeadBoundsGuard(u8 const *ptr) const
{
    union {
        u8 const *u8Ptr;
        u32 const *u32Ptr;
    };
    u8Ptr = ptr - this->allocHeadSize + OFFSET_TO_SRC_SIZE + ALIGNMENT_SIZE + ALLOC_AMOUNT_SIZE;
    return (u32 *)u32Ptr;
}

u8 *DqnAllocatorMetadata::PtrToAlignment(u8 const *ptr) const
{
    union {
        u8 const *u8Ptr;
        u32 const *u32Ptr;
    };
    u8Ptr = ptr - this->allocHeadSize + OFFSET_TO_SRC_SIZE;
    return (u8 *)u8Ptr;
}


u8 *DqnAllocatorMetadata::PtrToOffsetToSrc(u8 const *ptr) const
{
    union {
        u8 const *u8Ptr;
        u32 const *u32Ptr;
    };
    u8Ptr = ptr - this->allocHeadSize;
    return (u8 *)u8Ptr;
}

isize *DqnAllocatorMetadata::PtrToAllocAmount(u8 const *ptr) const
{
    union {
        u8 const *u8Ptr;
        u32 const *u32Ptr;
        isize const *isizePtr;
    };
    u8Ptr = ptr - this->allocHeadSize + OFFSET_TO_SRC_SIZE + ALIGNMENT_SIZE;
    return (isize *)isizePtr;
}

u32 *DqnAllocatorMetadata::PtrToTailBoundsGuard(u8 const *ptr) const
{
    isize size = *PtrToAllocAmount(ptr);
    union {
        u8 const *u8Ptr;
        u32 const *u32Ptr;
    };
    u8Ptr = (ptr + size);
    return (u32 *)u32Ptr;
}

// #DqnMemStack
// =================================================================================================
DQN_FILE_SCOPE DqnMemStack::Block *
DqnMemStackInternal_AllocateBlock(isize size, Dqn::ZeroClear clear, DqnMemAPI *api)
{
    DQN_ASSERT(size > 0);
    if (!api || !api->IsValid())
    {
        DQN_LOGE("Could not allocate block with api, api is null or is valid check failed.");
        return nullptr;
    }

    isize totalSize = sizeof(DqnMemStack::Block) + size;
    auto *result = (DqnMemStack::Block *)api->Alloc(totalSize, clear);
    if (!result)
    {
        return nullptr;
    }

    result->memory    = ((u8 *)result) + sizeof(*result);
    result->size      = size;
    result->head      = result->memory;
    result->tail      = result->memory + size;
    result->prevBlock = nullptr;
    return result;
}

bool DqnMemStack::Init(void *mem, isize size, Dqn::ZeroClear clear, u32 flags_)
{
    if (!mem)
    {
        DQN_LOGE("Supplied fixed memory buffer is nullptr, initialise with fixed memory failed.");
        return false;
    }

    if (size < static_cast<isize>(sizeof(DqnMemStack::Block)))
    {
        DQN_LOGE(
            "Memory size is too small to store the metadata, we received: %$d, the "
            "memory block requires at minimum: %$d",
            size, sizeof(*this->block));
        return false;
    }

    if (clear == Dqn::ZeroClear::Yes)
        DqnMem_Set(mem, 0, size);

    this->block            = (DqnMemStack::Block *)mem;
    this->block->memory    = (u8 *)mem + sizeof(DqnMemStack::Block);
    this->block->size      = size - sizeof(DqnMemStack::Block);
    this->block->head      = this->block->memory;
    this->block->tail      = this->block->memory + this->block->size;
    this->block->prevBlock = nullptr;

    this->memAPI          = nullptr;
    this->myHeadAPI       = DqnMemAPI::StackAllocator(this, DqnMemAPI::StackPushType::Head);
    this->myTailAPI       = DqnMemAPI::StackAllocator(this, DqnMemAPI::StackPushType::Tail);
    this->flags           = (flags_ | Flag::NonExpandable);
    this->tempRegionCount = 0;

    bool boundsGuard = Dqn_BitIsSet(this->flags, Flag::BoundsGuard);
    this->metadata.Init(boundsGuard);

    return true;
}

bool DqnMemStack::Init(isize size, Dqn::ZeroClear clear, u32 flags_, DqnMemAPI *api)
{
    if (!this || size < 0) return false;

    if (this->block)
    {
        DQN_LOGE("Memory stack has pre-existing block attached: %x\n", this->block);
        return false;
    }

    if (size == 0)
    {
        this->block = nullptr;
    }
    else
    {
        this->block = DqnMemStackInternal_AllocateBlock(size, clear, api);
        if (!this->block)
        {
            DQN_LOGE("Memory stack could not allocated memory error, requested: %d bytes", size);
            return false;
        }
    }

    this->tempRegionCount = 0;
    this->flags           = flags_;
    this->memAPI          = api;
    this->myHeadAPI       = DqnMemAPI::StackAllocator(this, DqnMemAPI::StackPushType::Head);
    this->myTailAPI       = DqnMemAPI::StackAllocator(this, DqnMemAPI::StackPushType::Tail);

    bool boundsGuard = Dqn_BitIsSet(this->flags, Flag::BoundsGuard);
    this->metadata.Init(boundsGuard);

    return true;
}

FILE_SCOPE void *DqnMemStackInternal_Push(DqnMemStack *stack, isize size, u8 alignment, bool pushToHead)
{
    DQN_ASSERT(size >= 0 && (alignment % 2 == 0));
    DQN_ASSERTM(alignment <= 128,
                "Alignment > 128 not supported. Need to update metadata to use u16 for storing the "
                "offset!");

    if (size == 0)
        return nullptr;

    // Allocate New Block If Full
    // =============================================================================================
    DqnAllocatorMetadata *myMetadata = &stack->metadata;
    isize actualSize                 = myMetadata->GetAllocationSize(size, alignment);
    bool needNewBlock                = false;
    if (stack->block)
    {
        if (pushToHead) needNewBlock = ((stack->block->head + actualSize) > stack->block->tail);
        else            needNewBlock = ((stack->block->tail - actualSize) < stack->block->head);
    }

    if (needNewBlock)
    {
        if (Dqn_BitIsSet(stack->flags, DqnMemStack::Flag::NonExpandable))
        {
            DQN_LOGE("Allocator is non-expandable and has run out of memory.");
            if (Dqn_BitIsSet(stack->flags, DqnMemStack::Flag::NonExpandableAssert))
                DQN_ASSERT(DQN_INVALID_CODE_PATH);

            return nullptr;
        }

        isize newBlockSize           = DQN_MAX(actualSize, DqnMemStack::MINIMUM_BLOCK_SIZE);
        DqnMemStack::Block *newBlock = DqnMemStackInternal_AllocateBlock(newBlockSize, Dqn::ZeroClear::No, stack->memAPI);
        if (!newBlock)
        {
            DQN_LOGE(
                "Allocator is full and could not allocate additional memory blocks, requested: %$d",
                newBlockSize);
            return nullptr;
        }

        newBlock->prevBlock = stack->block;
        stack->block         = newBlock;
    }

    // Calculate Ptr To Give Client
    // =============================================================================================
    u8 *currPtr = (pushToHead) ? (stack->block->head) : (stack->block->tail - actualSize);
    u8 *result  = (u8 *)DQN_ALIGN_POW_N((currPtr + myMetadata->GetAllocHeadSize()), alignment);

    isize const offsetToSrc = result - currPtr;
    DQN_ASSERT(offsetToSrc > 0 && offsetToSrc < (u8)-1);

    if (pushToHead)
    {
        stack->block->head += actualSize;
        DQN_ASSERT(stack->block->head <= stack->block->tail);
    }
    else
    {
        stack->block->tail -= actualSize;
        DQN_ASSERT(stack->block->tail >= stack->block->head);
    }


    // Instrument allocation with guards and metadata
    // =============================================================================================
    {
        auto *myOffsetToSrc = myMetadata->PtrToOffsetToSrc(result);
        *myOffsetToSrc      = (u8)offsetToSrc;

        auto *myAlignment = myMetadata->PtrToAlignment(result);
        *myAlignment      = alignment;

        auto *allocAmount = myMetadata->PtrToAllocAmount(result);
        *allocAmount = size;

        if (Dqn_BitIsSet(stack->flags, DqnMemStack::Flag::BoundsGuard))
        {
            auto *headGuard = myMetadata->PtrToHeadBoundsGuard(result);
            auto *tailGuard = myMetadata->PtrToTailBoundsGuard(result);
            *headGuard       = DqnAllocatorMetadata::HEAD_GUARD_VALUE;
            *tailGuard       = DqnAllocatorMetadata::TAIL_GUARD_VALUE;
        }
    }

    // Debug check (alignment, bounds guard)
    // =============================================================================================
    {
        u8 *checkAlignment = (u8 *)DQN_ALIGN_POW_N(result, alignment);
        DQN_ASSERTM(checkAlignment == result,
                    "Adding bounds guard should not destroy alignment! %p != %p", result,
                    checkAlignment);

        if (Dqn_BitIsSet(stack->flags, DqnMemStack::Flag::BoundsGuard))
        {
            myMetadata->AddAllocation(result);
            myMetadata->CheckAllocations();
        }
    }

    return result;
}

void *DqnMemStack::PushOnTail(isize size, u8 alignment)
{
    void *result = DqnMemStackInternal_Push(this, size, alignment, false);
    return result;
}

void *DqnMemStack::Push(isize size, u8 alignment)
{
    void *result = DqnMemStackInternal_Push(this, size, alignment, true);
    return result;
}

FILE_SCOPE void DqnMemStackInternal_KillMetadataPtrsExistingIn(DqnAllocatorMetadata *metadata,
                                                               u8 const *start, u8 const *end)
{
    if (start >= end) return;

    for (auto index = 0; index < metadata->allocations.count; index++)
    {
        u8 *ptr = metadata->allocations.data[index];
        if (ptr >= start && ptr < end)
        {
            metadata->allocations.EraseStable(index);
            index--;
        }
    }
}

FILE_SCOPE void DqnMemStackInternal_KillMetadataPtrsExistingInBlock(DqnAllocatorMetadata *metadata,
                                                                    DqnMemStack::Block const *block)
{
    u8 const *blockStart = block->memory;
    u8 const *blockEnd   = block->memory + block->size;
    DqnMemStackInternal_KillMetadataPtrsExistingIn(metadata, blockStart, blockEnd);
}

FILE_SCOPE void DqnMemStackInternal_Pop(DqnMemStack *stack, void *ptr, Dqn::ZeroClear clear, bool popHead)
{
    if (!ptr) return;
    DQN_ASSERT(stack->block);

    u8 *const bytePtr                = (u8 *)ptr;
    DqnAllocatorMetadata *myMetadata = &stack->metadata;

    // Check instrumented data
    if (Dqn_BitIsSet(stack->flags, DqnMemStack::Flag::BoundsGuard))
    {
        myMetadata->CheckAllocations();
        myMetadata->RemoveAllocation(bytePtr);
    }

    isize const size     = *(myMetadata->PtrToAllocAmount(bytePtr));
    u8 const alignment   = *(myMetadata->PtrToAlignment(bytePtr));
    u8 const offsetToSrc = *(myMetadata->PtrToOffsetToSrc(bytePtr));

    isize actualSize         = myMetadata->GetAllocationSize(size, alignment);
    u8 *const start          = bytePtr - offsetToSrc;
    u8 *const end            = start + actualSize;
    u8 const *const blockEnd = stack->block->memory + stack->block->size;

    if (popHead)
    {
        DQN_ASSERTM(end == stack->block->head, "Pointer to pop was not the last allocation! %p != %p", end, stack->block->head);
        stack->block->head -= actualSize;
        DQN_ASSERT(stack->block->head >= stack->block->memory);
    }
    else
    {
        DQN_ASSERTM(start == stack->block->tail, "Pointer to pop was not the last allocation! %p != %p", start, stack->block->tail);
        stack->block->tail += actualSize;
        DQN_ASSERT(stack->block->tail <= blockEnd);
    }

    if (clear == Dqn::ZeroClear::Yes)
        DqnMem_Set(start, 0, end - start);

    if (stack->block->tail == blockEnd && stack->block->head == stack->block->memory)
    {
        if (stack->block->prevBlock)
        {
            stack->FreeLastBlock();
        }
    }
}

void DqnMemStack::Pop(void *ptr, Dqn::ZeroClear clear)
{
    DqnMemStackInternal_Pop(this, ptr, clear, true);
}

void DqnMemStack::PopOnTail(void *ptr, Dqn::ZeroClear clear)
{
    DqnMemStackInternal_Pop(this, ptr, clear, false);
}

void DqnMemStack::Free()
{
    if (Dqn_BitIsSet(this->flags, Flag::BoundsGuard))
        this->metadata.allocations.Free();

    if (this->memAPI)
    {
        while (this->block)
            this->FreeLastBlock();
    }
}

bool DqnMemStack::FreeMemBlock(DqnMemStack::Block *memBlock)
{
    if (!memBlock || !this->block)
        return false;

    if (!this->memAPI)
        return false;

    DqnMemStack::Block **blockPtr = &this->block;

    while (*blockPtr && (*blockPtr) != memBlock)
        blockPtr = &((*blockPtr)->prevBlock);

    if (*blockPtr)
    {
        DqnMemStack::Block *blockToFree = *blockPtr;
        (*blockPtr)                     = blockToFree->prevBlock;

        if (Dqn_BitIsSet(this->flags, Flag::BoundsGuard))
        {
            DqnMemStackInternal_KillMetadataPtrsExistingInBlock(&this->metadata, blockToFree);
        }

        isize realSize = blockToFree->size + sizeof(DqnMemStack::Block);
        this->memAPI->Free(blockToFree, realSize);

        // No more blocks, then last block has been freed
        if (!this->block) DQN_ASSERT(this->tempRegionCount == 0);
        return true;
    }

    return false;
}

void DqnMemStack::ResetTail()
{
    u8 *start = this->block->tail;
    u8 *end   = this->block->memory + this->block->size;

    if (Dqn_BitIsSet(this->flags, Flag::BoundsGuard))
    {
        DqnMemStackInternal_KillMetadataPtrsExistingIn(&this->metadata, start, end);
    }

    this->block->tail = end;
}

void DqnMemStack::Reset()
{
    while(this->block && this->block->prevBlock)
    {
        this->FreeLastBlock();
    }
    this->ClearCurrBlock(Dqn::ZeroClear::No);
}


bool DqnMemStack::FreeLastBlock()
{
    bool result = this->FreeMemBlock(this->block);
    return result;
}

void DqnMemStack::ClearCurrBlock(Dqn::ZeroClear clear)
{
    if (this->block)
    {
        if (Dqn_BitIsSet(this->flags, Flag::BoundsGuard))
        {
            DqnMemStackInternal_KillMetadataPtrsExistingInBlock(&this->metadata, this->block);
        }

        this->block->head = this->block->memory;
        this->block->tail = this->block->memory + this->block->size;
        if (clear == Dqn::ZeroClear::Yes)
        {
            DqnMem_Clear(this->block->memory, 0, this->block->size);
        }

    }
}

DqnMemStack::Info DqnMemStack::GetInfo() const
{
    Info result = {};
    for (Block *block_ = this->block; block_; block_ = block_->prevBlock)
    {
        u8 const *blockEnd  = block_->memory + block_->size;
        isize usageFromHead = block_->head - block_->memory;
        isize usageFromTail = blockEnd - block_->tail;

        result.totalUsed += usageFromHead + usageFromTail;
        result.totalSize += block_->size;
        result.wastedSize += (block_->size - usageFromHead - usageFromTail);
        result.numBlocks++;
    }

    u8 const *blockEnd  = this->block->memory + this->block->size;
    isize usageFromHead = this->block->head - this->block->memory;
    isize usageFromTail = blockEnd - this->block->tail;
    result.wastedSize -= (this->block->size - usageFromHead - usageFromTail); // Don't include the curr block

    return result;
}

DqnMemStack::TempRegion DqnMemStack::TempRegionBegin()
{
    TempRegion result        = {};
    result.stack             = this;
    result.startingBlock     = this->block;
    result.startingBlockHead = (this->block) ? this->block->head : nullptr;
    result.startingBlockTail = (this->block) ? this->block->tail : nullptr;

    this->tempRegionCount++;
    return result;
}

void DqnMemStack::TempRegionEnd(TempRegion region)
{
    DQN_ASSERT(region.stack == this);

    this->tempRegionCount--;
    DQN_ASSERT(this->tempRegionCount >= 0);

    if (region.keepHeadChanges && region.keepTailChanges)
    {
        return;
    }

    // Free blocks until you find the first block with changes in the head or tail, this is the
    // block we want to start preserving allocation data for keepHead/TailChanges.
    if (region.keepHeadChanges)
    {
        while (this->block && this->block->head == this->block->memory)
            this->FreeLastBlock();
    }
    else if (region.keepTailChanges)
    {
        while (this->block && this->block->tail == (this->block->memory + this->block->size))
            this->FreeLastBlock();
    }
    else
    {
        while (this->block != region.startingBlock)
            this->FreeLastBlock();
    }

    for (Block *block_ = this->block; block_; block_ = block_->prevBlock)
    {
        if (block_ == region.startingBlock)
        {
            if (region.keepHeadChanges)
            {
                block_->tail = region.startingBlockTail;
            }
            else if (region.keepTailChanges)
            {
                block_->head = region.startingBlockHead;
            }
            else
            {
                block_->head = region.startingBlockHead;
                block_->tail = region.startingBlockTail;
            }

            if (Dqn_BitIsSet(this->flags, DqnMemStack::Flag::BoundsGuard))
            {
                u8 *blockStart = this->block->head;
                u8 *blockEnd   = this->block->tail;
                DqnMemStackInternal_KillMetadataPtrsExistingIn(&this->metadata, blockStart, blockEnd);
            }
            break;
        }
        else
        {
            if (region.keepHeadChanges || region.keepTailChanges)
            {
                u8 *blockStart = nullptr;
                u8 *blockEnd   = nullptr;
                if (region.keepHeadChanges)
                {
                    blockStart   = block_->tail;
                    blockEnd     = block_->memory + block_->size;
                    block_->tail = blockEnd;
                }
                else
                {
                    blockStart   = block_->memory;
                    blockEnd     = block_->memory + block_->size;
                    block_->head = blockStart;
                }

                if (Dqn_BitIsSet(this->flags, DqnMemStack::Flag::BoundsGuard))
                {
                    DqnMemStackInternal_KillMetadataPtrsExistingIn(&this->metadata, blockStart, blockEnd);
                }
            }
        }
    }
}

DqnMemStack::TempRegionGuard_ DqnMemStack::TempRegionGuard()
{
    return TempRegionGuard_(this);
}

DqnMemStack::TempRegionGuard_::TempRegionGuard_(DqnMemStack *stack)
{
    this->region = stack->TempRegionBegin();
}

DqnMemStack::TempRegionGuard_::~TempRegionGuard_()
{
    DqnMemStack *const stack = this->region.stack;
    stack->TempRegionEnd(this->region);
}

// #DqnHash
// =================================================================================================
// Taken from GingerBill single file library @ github.com/gingerbill/gb
u32 DqnHash_Murmur32Seed(void const *data, usize len, u32 seed)
{
    u32 const c1 = 0xcc9e2d51; u32 const c2 = 0x1b873593; u32 const r1 = 15;
    u32 const r2 = 13; u32 const m  = 5; u32 const n  = 0xe6546b64;

    usize i, nblocks = len / 4;
    u32 hash = seed, k1 = 0;
    u32 const *blocks = (u32 const *)data;
    u8 const *tail    = (u8 const *)(data) + nblocks * 4;

    for (i = 0; i < nblocks; i++) {
        u32 k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }

    switch (len & 3) {
    case 3: k1 ^= tail[2] << 16;
    case 2: k1 ^= tail[1] << 8;
    case 1: k1 ^= tail[0];

        k1 *= c1;
        k1 = (k1 << r1) | (k1 >> (32 - r1));
        k1 *= c2;
        hash ^= k1;
    }

    hash ^= len;        hash ^= (hash >> 16);
    hash *= 0x85ebca6b; hash ^= (hash >> 13);
    hash *= 0xc2b2ae35; hash ^= (hash >> 16);

    return hash;
}

u64 DqnHash_Murmur64Seed(void const *data_, usize len, u64 seed)
{
    u64 const m = 0xc6a4a7935bd1e995ULL;
    i32 const r = 47;

    u64 h = seed ^ (len * m);

    u64 const *data = (u64 const *)data_;
    u8 const *data2 = (u8 const *)data_;
    u64 const *end  = data + (len / 8);

    while (data != end) {
        u64 k = *data++;
        k *= m; k ^= k >> r; k *= m;
        h ^= k; h *= m;
    }

    switch (len & 7) {
    case 7: h ^= (u64)(data2[6]) << 48;
    case 6: h ^= (u64)(data2[5]) << 40;
    case 5: h ^= (u64)(data2[4]) << 32;
    case 4: h ^= (u64)(data2[3]) << 24;
    case 3: h ^= (u64)(data2[2]) << 16;
    case 2: h ^= (u64)(data2[1]) << 8;
    case 1: h ^= (u64)(data2[0]);
        h *= m;
    };

    h ^= h >> r; h *= m; h ^= h >> r;
    return h;
}

// #DqnMath
// =================================================================================================
DQN_FILE_SCOPE f32 DqnMath_Lerp(f32 a, f32 t, f32 b)
{
    /*
        Linear blend between two values. We having a starting point "a", and
        the distance to "b" is defined as (b - a). Then we can say

        a + t(b - a)

        As our linear blend fn. We start from "a" and choosing a t from 0->1
        will vary the value of (b - a) towards b. If we expand this, this
        becomes

        a + (t * b) - (a * t) == (1 - t)a + t*b
    */
    f32 result =  a + (b - a) * t;
    return result;
}

DQN_FILE_SCOPE f32 DqnMath_Sqrtf(f32 a)
{
    f32 result = sqrtf(a);
    return result;
}

DQN_FILE_SCOPE f32 DqnMath_Clampf(f32 val, f32 min, f32 max)
{
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

// #DqnV2
// =================================================================================================
DQN_FILE_SCOPE DqnV2 DqnV2_Add(DqnV2 a, DqnV2 b)
{
    DqnV2 result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] + b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Sub(DqnV2 a, DqnV2 b)
{
    DqnV2 result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] - b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Scalei(DqnV2 a, i32 b)
{
    DqnV2 result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Scalef(DqnV2 a, f32 b)
{
    DqnV2 result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Hadamard(DqnV2 a, DqnV2 b)
{
    DqnV2 result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b.e[i];

    return result;
}

DQN_FILE_SCOPE f32 DqnV2_Dot(DqnV2 a, DqnV2 b)
{
    /*
       DOT PRODUCT
       Two vectors with dot product equals |a||b|cos(theta)
       |a|   |d|
       |b| . |e| = (ad + be + cf)
       |c|   |f|
     */
    f32 result = 0;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result += (a.e[i] * b.e[i]);

    return result;
}

DQN_FILE_SCOPE bool DqnV2_Equals(DqnV2 a, DqnV2 b)
{
    bool result = true;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        if (a.e[i] != b.e[i]) result = false;
    return result;
}

DQN_FILE_SCOPE f32 DqnV2_LengthSquared(DqnV2 a, DqnV2 b)
{
    f32 x_     = b.x - a.x;
    f32 y_     = b.y - a.y;
    f32 result = (DQN_SQUARED(x_) + DQN_SQUARED(y_));
    return result;
}

DQN_FILE_SCOPE f32 DqnV2_Length(DqnV2 a, DqnV2 b)
{
    f32 lengthSq = DqnV2_LengthSquared(a, b);
    if (lengthSq == 0) return 0;

    f32 result = DqnMath_Sqrtf(lengthSq);
    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Normalise(DqnV2 a)
{
    f32 magnitude = DqnV2_Length(DqnV2(0, 0), a);
    if (magnitude == 0) return DqnV2(0.0f);

    DqnV2 result = a * (1.0f / magnitude);
    return result;
}

DQN_FILE_SCOPE bool DqnV2_Overlaps(DqnV2 a, DqnV2 b)
{
    bool result = false;

    f32 lenOfA = a.max - a.min;
    f32 lenOfB = b.max - b.min;

    if (lenOfA > lenOfB)
    {
        DqnV2 tmp = a;
        a         = b;
        b         = tmp;
    }

    if ((a.min >= b.min && a.min <= b.max) || (a.max >= b.min && a.max <= b.max))
    {
        result = true;
    }

    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Perpendicular(DqnV2 a)
{
    DqnV2 result = DqnV2(a.y, -a.x);
    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_ResizeKeepAspectRatio(DqnV2 srcSize, DqnV2 targetSize)
{
    f32 ratioA   = srcSize.w / targetSize.w;
    f32 ratioB   = srcSize.h / targetSize.h;
    f32 ratio    = DQN_MIN(ratioA, ratioB);
    DqnV2 result = DqnV2_Scalef(targetSize, ratio);
    return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_ConstrainToRatio(DqnV2 dim, DqnV2 ratio)
{
    DqnV2 result                  = {0};
    f32 numRatioIncrementsToWidth  = (f32)(dim.w / ratio.w);
    f32 numRatioIncrementsToHeight = (f32)(dim.h / ratio.h);

    f32 leastIncrementsToSide =
        DQN_MIN(numRatioIncrementsToHeight, numRatioIncrementsToWidth);

    result.w = (f32)(ratio.w * leastIncrementsToSide);
    result.h = (f32)(ratio.h * leastIncrementsToSide);
    return result;
}

// #DqnV2i
// =================================================================================================
DQN_FILE_SCOPE DqnV2i DqnV2i_Add(DqnV2i a, DqnV2i b)
{
    DqnV2i result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] + b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_Sub(DqnV2i a, DqnV2i b)
{
    DqnV2i result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] - b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_Scalef(DqnV2i a, f32 b)
{
    DqnV2i result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = (i32)(a.e[i] * b);

    return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_Scalei(DqnV2i a, i32 b)
{
    DqnV2i result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_Hadamard(DqnV2i a, DqnV2i b)
{
    DqnV2i result = {};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b.e[i];

    return result;
}

DQN_FILE_SCOPE f32 DqnV2i_Dot(DqnV2i a, DqnV2i b)
{
    /*
       DOT PRODUCT
       Two vectors with dot product equals |a||b|cos(theta)
       |a|   |d|
       |b| . |e| = (ad + be + cf)
       |c|   |f|
     */
    f32 result = 0;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result += (a.e[i] * b.e[i]);

    return result;
}

DQN_FILE_SCOPE bool DqnV2i_Equals(DqnV2i a, DqnV2i b)
{
    bool result = true;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        if (a.e[i] != b.e[i]) result = false;
    return result;
}

// #DqnV3
// =================================================================================================
DQN_FILE_SCOPE DqnV3 DqnV3_Add(DqnV3 a, DqnV3 b)
{
    DqnV3 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] + b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Sub(DqnV3 a, DqnV3 b)
{
    DqnV3 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] - b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Scalei(DqnV3 a, i32 b)
{
    DqnV3 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Scalef(DqnV3 a, f32 b)
{
    DqnV3 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Hadamard(DqnV3 a, DqnV3 b)
{
    DqnV3 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b.e[i];

    return result;
}

DQN_FILE_SCOPE f32 DqnV3_Dot(DqnV3 a, DqnV3 b)
{
    /*
       DOT PRODUCT
       Two vectors with dot product equals |a||b|cos(theta)
       |a|   |d|
       |b| . |e| = (ad + be + cf)
       |c|   |f|
     */
    f32 result = 0;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result += (a.e[i] * b.e[i]);

    return result;
}

DQN_FILE_SCOPE bool DqnV3_Equals(DqnV3 a, DqnV3 b)
{
    bool result = true;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        if (a.e[i] != b.e[i]) result = false;
    return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Cross(DqnV3 a, DqnV3 b)
{
    /*
       CROSS PRODUCT
       Generate a perpendicular vector to the 2 vectors
       |a|   |d|   |bf - ce|
       |b| x |e| = |cd - af|
       |c|   |f|   |ae - be|
     */
    DqnV3 result = {0};
    result.e[0] = (a.e[1] * b.e[2]) - (a.e[2] * b.e[1]);
    result.e[1] = (a.e[2] * b.e[0]) - (a.e[0] * b.e[2]);
    result.e[2] = (a.e[0] * b.e[1]) - (a.e[1] * b.e[0]);
    return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Normalise(DqnV3 a)
{
    f32 length    = DqnMath_Sqrtf(DQN_SQUARED(a.x) + DQN_SQUARED(a.y) + DQN_SQUARED(a.z));
    f32 invLength = 1 / length;
    DqnV3 result  = a * invLength;

    return result;
}

DQN_FILE_SCOPE f32 DqnV3_LengthSquared(DqnV3 a, DqnV3 b)
{
    f32 x      = b.x - a.x;
    f32 y      = b.y - a.y;
    f32 z      = b.z - a.z;
    f32 result = (DQN_SQUARED(x) + DQN_SQUARED(y) + DQN_SQUARED(z));
    return result;
}

DQN_FILE_SCOPE f32 DqnV3_Length(DqnV3 a, DqnV3 b)
{
    f32 lengthSq = DqnV3_LengthSquared(a, b);
    if (lengthSq == 0) return 0;

    f32 result = DqnMath_Sqrtf(lengthSq);
    return result;
}

// #DqnV4
// =================================================================================================
DQN_FILE_SCOPE DqnV4 DqnV4_Add(DqnV4 a, DqnV4 b)
{
    DqnV4 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] + b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_Sub(DqnV4 a, DqnV4 b)
{
    DqnV4 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] - b.e[i];

    return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_Scalei(DqnV4 a, i32 b)
{
    DqnV4 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_Scalef(DqnV4 a, f32 b)
{
    DqnV4 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b;

    return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_Hadamard(DqnV4 a, DqnV4 b)
{
    DqnV4 result = {0};
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result.e[i] = a.e[i] * b.e[i];

    return result;
}

DQN_FILE_SCOPE f32 DqnV4_Dot(DqnV4 a, DqnV4 b)
{
    /*
       DOT PRODUCT
       Two vectors with dot product equals |a||b|cos(theta)
       |a|   |d|
       |b| . |e| = (ad + be + cf)
       |c|   |f|
     */
    f32 result = 0;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        result += (a.e[i] * b.e[i]);

    return result;
}

DQN_FILE_SCOPE bool DqnV4_Equals(DqnV4 a, DqnV4 b)
{
    bool result = true;
    for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
        if (a.e[i] != b.e[i]) result = false;
    return result;
}

// #DqnMat4 Implementation
// =================================================================================================
DQN_FILE_SCOPE DqnMat4 DqnMat4_Identity()
{
    DqnMat4 result = {0, 0, 0, 0};
    result.e[0][0] = 1;
    result.e[1][1] = 1;
    result.e[2][2] = 1;
    result.e[3][3] = 1;
    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 zNear,
                                            f32 zFar)
{
    DqnMat4 result = DqnMat4_Identity();
    result.e[0][0] = +2.0f / (right - left);
    result.e[1][1] = +2.0f / (top   - bottom);
    result.e[2][2] = -2.0f / (zFar  - zNear);

    result.e[3][0] = -(right + left)   / (right - left);
    result.e[3][1] = -(top   + bottom) / (top   - bottom);
    result.e[3][2] = -(zFar  + zNear)  / (zFar  - zNear);

    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Perspective(f32 fovYDegrees, f32 aspectRatio, f32 zNear, f32 zFar)
{
    f32 fovYRadians         = DQN_DEGREES_TO_RADIANS(fovYDegrees);
    f32 fovYRadiansOver2    = fovYRadians * 0.5f;
    f32 tanFovYRadiansOver2 = tanf(fovYRadiansOver2);
    f32 zNearSubZFar        = zNear - zFar;

    DqnMat4 result = DqnMat4_Identity();
    result.e[0][0] = 1.0f / (aspectRatio * tanFovYRadiansOver2);
    result.e[1][1] = 1.0f / tanFovYRadiansOver2;
    result.e[2][2] = (zNear + zFar) / zNearSubZFar;
    result.e[2][3] = -1.0f;
    result.e[3][2] = (2.0f * zNear * zFar) / zNearSubZFar;
    result.e[3][3] = 0.0f;

    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_LookAt(DqnV3 eye, DqnV3 center, DqnV3 up)
{
    DqnMat4 result = {0, 0, 0, 0};

    DqnV3 f = DqnV3_Normalise(DqnV3_Sub(eye, center));
    DqnV3 s = DqnV3_Normalise(DqnV3_Cross(up, f));
    DqnV3 u = DqnV3_Cross(f, s);

    result.e[0][0] = s.x;
    result.e[0][1] = u.x;
    result.e[0][2] = -f.x;

    result.e[1][0] = s.y;
    result.e[1][1] = u.y;
    result.e[1][2] = -f.y;

    result.e[2][0] = s.z;
    result.e[2][1] = u.z;
    result.e[2][2] = -f.z;

    result.e[3][0] = -DqnV3_Dot(s, eye);
    result.e[3][1] = -DqnV3_Dot(u, eye);
    result.e[3][2] = DqnV3_Dot(f, eye);
    result.e[3][3] = 1.0f;
    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Translate3f(f32 x, f32 y, f32 z)
{
    DqnMat4 result = DqnMat4_Identity();
    result.e[3][0] = x;
    result.e[3][1] = y;
    result.e[3][2] = z;
    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_TranslateV3(DqnV3 vec)
{
    DqnMat4 result = DqnMat4_Identity();
    result.e[3][0] = vec.x;
    result.e[3][1] = vec.y;
    result.e[3][2] = vec.z;
    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Rotate(f32 radians, f32 x, f32 y, f32 z)
{
    DqnMat4 result     = DqnMat4_Identity();
    f32 sinVal         = sinf(radians);
    f32 cosVal         = cosf(radians);
    f32 oneMinusCosVal = 1 - cosVal;

    DqnV3 axis = DqnV3_Normalise(DqnV3(x, y, z));

    result.e[0][0] = (axis.x * axis.x * oneMinusCosVal) + cosVal;
    result.e[0][1] = (axis.x * axis.y * oneMinusCosVal) + (axis.z * sinVal);
    result.e[0][2] = (axis.x * axis.z * oneMinusCosVal) - (axis.y * sinVal);

    result.e[1][0] = (axis.y * axis.x * oneMinusCosVal) - (axis.z * sinVal);
    result.e[1][1] = (axis.y * axis.y * oneMinusCosVal) + cosVal;
    result.e[1][2] = (axis.y * axis.z * oneMinusCosVal) + (axis.x * sinVal);

    result.e[2][0] = (axis.z * axis.x * oneMinusCosVal) + (axis.y * sinVal);
    result.e[2][1] = (axis.z * axis.y * oneMinusCosVal) - (axis.x * sinVal);
    result.e[2][2] = (axis.z * axis.z * oneMinusCosVal) + cosVal;

    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Scale(f32 x, f32 y, f32 z)
{
    DqnMat4 result = {0, 0, 0, 0};
    result.e[0][0] = x;
    result.e[1][1] = y;
    result.e[2][2] = z;
    result.e[3][3] = 1;
    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_ScaleV3(DqnV3 scale)
{
    DqnMat4 result = {0, 0, 0, 0};
    result.e[0][0] = scale.x;
    result.e[1][1] = scale.y;
    result.e[2][2] = scale.z;
    result.e[3][3] = 1;
    return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Mul(DqnMat4 a, DqnMat4 b)
{
    DqnMat4 result = {0};
    for (u32 j = 0; j < 4; j++) {
        for (u32 i = 0; i < 4; i++)
        {
            result.e[j][i] = a.e[0][i] * b.e[j][0]
                           + a.e[1][i] * b.e[j][1]
                           + a.e[2][i] * b.e[j][2]
                           + a.e[3][i] * b.e[j][3];
        }
    }

    return result;
}

DQN_FILE_SCOPE DqnV4 DqnMat4_MulV4(DqnMat4 a, DqnV4 b)
{
    DqnV4 result = {0};
    result.x = (a.e[0][0] * b.x) + (a.e[1][0] * b.y) + (a.e[2][0] * b.z) + (a.e[3][0] * b.w);
    result.y = (a.e[0][1] * b.x) + (a.e[1][1] * b.y) + (a.e[2][1] * b.z) + (a.e[3][1] * b.w);
    result.z = (a.e[0][2] * b.x) + (a.e[1][2] * b.y) + (a.e[2][2] * b.z) + (a.e[3][2] * b.w);
    result.w = (a.e[0][3] * b.x) + (a.e[1][3] * b.y) + (a.e[2][3] * b.z) + (a.e[3][3] * b.w);

    return result;
}

// #DqnRect Implementation
// =================================================================================================
void DqnRect::GetSize(f32 *width, f32 *height) const
{
    DQN_ASSERT(this->min <= this->max);
    if (width) *width   = this->max.x - this->min.x;
    if (height) *height = this->max.y - this->min.y;
}

DqnV2 DqnRect::GetCenter() const
{
    DQN_ASSERT(this->min <= this->max);
    f32 sumX     = this->min.x + this->max.x;
    f32 sumY     = this->min.y + this->max.y;
    DqnV2 result = DqnV2(sumX, sumY) * 0.5f;
    return result;
}

DqnRect DqnRect::ClipRect(DqnRect clip) const
{
    DQN_ASSERT(this->min <= this->max);
    DQN_ASSERT(clip.min <= clip.max);
    DqnRect result = *this;

    if (clip.min.x > this->min.x && clip.min.x < this->max.x)
    {
        if (clip.min.y > this->min.y && clip.min.y < this->max.y)
        {
            result.min = clip.min;
        }
        else if (clip.max.y > this->min.y)
        {
            result.min.x = clip.min.x;
        }
    }

    if (clip.max.x > this->min.x && clip.max.x < this->max.x)
    {
        if (clip.max.y > this->min.y && clip.max.y < this->max.y)
        {
            result.max = clip.max;
        }
        else if (clip.min.y < this->max.y)
        {
            result.max.x = clip.max.x;
        }
    }

    return result;
}

DqnRect DqnRect::Move(DqnV2 shift) const
{
    DQN_ASSERT(this->min <= this->max);

    DqnRect result;
    result.min = this->min + shift;
    result.max = this->max + shift;
    return result;
}

bool DqnRect::ContainsP(DqnV2 p) const
{
    DQN_ASSERT(this->min <= this->max);

    bool outsideOfRectX = false;
    if (p.x < this->min.x || p.x > this->max.w)
        outsideOfRectX = true;

    bool outsideOfRectY = false;
    if (p.y < this->min.y || p.y > this->max.h)
        outsideOfRectY = true;

    if (outsideOfRectX || outsideOfRectY) return false;

    return true;
}

// #DqnChar Implementation
// =================================================================================================
DQN_FILE_SCOPE char DqnChar_ToLower(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        i32 shiftOffset = 'a' - 'A';
        return (c + (char)shiftOffset);
    }
    return c;
}

DQN_FILE_SCOPE char DqnChar_ToUpper(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        i32 shiftOffset = 'a' - 'A';
        return (c - (char)shiftOffset);
    }
    return c;
}

DQN_FILE_SCOPE bool DqnChar_IsEndOfLine(char c)
{
    bool result = (c == '\n');
    return result;
}

DQN_FILE_SCOPE bool DqnChar_IsWhitespace(char c)
{
    bool result = (c == ' ' || c == '\r' || c == '\n' || c == '\t');
    return result;
}

DQN_FILE_SCOPE char *DqnChar_TrimWhitespaceAround(char const *src, i32 srcLen, i32 *newLen)
{
    if (!src)       return nullptr;
    if (srcLen < 0) return (char *)src;

    char const *start = src;
    char const *end   = start + (srcLen - 1);
    while(start[0] && DqnChar_IsWhitespace(start[0]))
    {
        start++;
    }

    i32 charsSkipped = (i32)(start - src);
    i32 updatedLen   = srcLen - charsSkipped;
    if (updatedLen <= 0)
    {
        if (newLen) *newLen = 0;
        return nullptr;
    }

    while(end[0] && DqnChar_IsWhitespace(end[0]))
    {
        end--;
    }

    charsSkipped = (i32)((src + srcLen - 1) - end);
    updatedLen   = updatedLen - charsSkipped;

    if (newLen) *newLen = updatedLen;
    return (char *)start;
}

DQN_FILE_SCOPE bool DqnChar_IsDigit(char c)
{
    if (c >= '0' && c <= '9') return true;
    return false;
}

DQN_FILE_SCOPE bool DqnChar_IsAlpha(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return true;
    return false;
}

DQN_FILE_SCOPE bool DqnChar_IsAlphaNum(char c)
{
    if (DqnChar_IsAlpha(c) || DqnChar_IsDigit(c)) return true;
    return false;
}

DQN_FILE_SCOPE char *DqnChar_SkipWhitespace(char const *ptr)
{
    while (ptr && (*ptr == ' ' || *ptr == '\r' || *ptr == '\n' || *ptr == '\t')) ptr++;
    return (char *)ptr;
}

DQN_FILE_SCOPE char *DqnChar_FindLastChar(char *ptr, char ch, i32 len, u32 *lenToChar)
{
    for (i32 i = len - 1; i >= 0; i--)
    {
        if (ptr[i] == ch)
        {
            if (lenToChar) *lenToChar = (u32)len - i;
            return &ptr[i];
        }
    }

    return nullptr;
}

DQN_FILE_SCOPE i32 DqnChar_FindNextLine(char *ptr, i32 *lineLength)
{
    i32 len = 0;
    ptr     = DqnChar_SkipWhitespace(ptr);

    // Advance pointer to first new line
    while (ptr && *ptr != 0 && *ptr != '\r' && *ptr != '\n')
    {
        ptr++;
        len++;
    }

    if (!ptr || *ptr == 0)
    {
        if (lineLength) *lineLength = len;
        return -1;
    }

    // Destroy all new lines
    i32 extraChars = 0;
    while (ptr && (*ptr == '\r' || *ptr == '\n' || *ptr == ' '))
    {
        *ptr = 0;
        ptr++;
        extraChars++;
    }

    if (lineLength) *lineLength = len;
    return len + extraChars;
}

DQN_FILE_SCOPE char *DqnChar_GetNextLine (char *ptr, i32 *lineLength)
{
    i32 offsetToNextLine = DqnChar_FindNextLine(ptr, lineLength);

    char *result = nullptr;
    if (offsetToNextLine != -1)
    {
        result = ptr + offsetToNextLine;
    }

    return result;
}

// #DqnStr Implementation
// =================================================================================================
DQN_FILE_SCOPE i32 DqnStr_Cmp(char const *a, char const *b, i32 numBytesToCompare, Dqn::IgnoreCase ignore)
{
    if (!a || !b)               return -1;
    if (!a)                     return -b[0];
    if (!b)                     return -a[0];
    if (numBytesToCompare == 0) return 0;

    i32 bytesCompared = 0;
    i32 result        = 0;
    if (ignore == Dqn::IgnoreCase::Yes)
    {
        while (a[0] && (DqnChar_ToLower(a[0]) == DqnChar_ToLower(b[0])))
        {
            a++; b++;
            if (++bytesCompared == numBytesToCompare) return 0;
        }
        result = DqnChar_ToLower(a[0]) - DqnChar_ToLower(b[0]);
    }
    else
    {
        while (a[0] && (a[0] == b[0]))
        {
            a++; b++;
            if (++bytesCompared == numBytesToCompare) return 0;
        }
        result = a[0] - b[0];
    }

    return result;
}

DQN_FILE_SCOPE char *DqnStr_GetPtrToLastSlash(char const *str, i32 strLen)
{
    char const *result       = str;
    if (strLen == -1) strLen = DqnStr_Len(str);

    for (auto i = strLen - 1; i >= 0; i--)
    {
        if (result[i] == '\\' || result[i] == '/')
        {
            result = result + i + 1;
            break;
        }
    }
    return (char *)result;
}

DQN_FILE_SCOPE i32 DqnStr_Len(char const *a)
{
    i32 result = 0;
    while (a && a[result]) result++;
    return result;
}

DQN_FILE_SCOPE i32 DqnStr_LenUTF8(u32 const *a, i32 *lenInBytes)
{
    i32 utf8Len        = 0;
    i32 utf8LenInBytes = 0;
    u8 *bytePtr        = (u8 *)a;
    while (true)
    {
        u32 codepoint           = 0;
        i32 numBytesInCodepoint = DqnStr_ReadUTF8Codepoint((u32 *)bytePtr, &codepoint);

        if (numBytesInCodepoint == 0) break;
        utf8Len++;
        bytePtr        += numBytesInCodepoint;
        utf8LenInBytes += numBytesInCodepoint;
    }

    if (lenInBytes) *lenInBytes = utf8LenInBytes;
    return utf8Len;
}

DQN_FILE_SCOPE i32 DqnStr_LenDelimitWith(char const *a, char delimiter)
{
    i32 result = 0;
    while (a && a[result] && a[result] != delimiter) result++;
    return result;
}

DQN_FILE_SCOPE i32 DqnStr_ReadUTF8Codepoint(u32 const *a, u32 *outCodepoint)
{
    u8 *byte = (u8 *)a;
    if (a && byte[0])
    {
        i32 numBytesInChar = 0;
        u32 actualChar     = 0;

        if (byte[0] <= 128)
        {
            actualChar     = byte[0];
            numBytesInChar = 1;
        }
        else if ((byte[0] & 0xE0) == 0xC0)
        {
            // Header 110xxxxx 10xxxxxx
            actualChar     = ((u32)(byte[0] & 0x3F) << 6)
                           | ((u32)(byte[1] & 0x1F) << 0);
            numBytesInChar = 2;
        }
        else if ((byte[0] & 0xF0) == 0xE0)
        {
            // Header 1110xxxx 10xxxxxx 10xxxxxx
            actualChar     = ((u32)(byte[0] & 0x0F) << 12)
                           | ((u32)(byte[1] & 0x3F) << 6 )
                           | ((u32)(byte[2] & 0x3F) << 0 );
            numBytesInChar = 3;
        }
        else if ((byte[0] & 0xF8) == 0xF0)
        {
            // Header 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            actualChar = ((u32)(byte[0] & 0x07) << 18)
                       | ((u32)(byte[1] & 0x3F) << 12)
                       | ((u32)(byte[2] & 0x3F) << 6 )
                       | ((u32)(byte[3] & 0x3F) << 0 );
            numBytesInChar = 4;
        }
        else
        {
            // NOTE: Malformed utf8 stream
        }

        if (outCodepoint) *outCodepoint = actualChar;
        return numBytesInChar;
    }

    return 0;
}

DQN_FILE_SCOPE void DqnStr_Reverse(char *buf, isize bufSize)
{
    if (!buf) return;
    isize mid = bufSize / 2;

    for (isize i = 0; i < mid; i++)
    {
        char tmp               = buf[i];
        buf[i]                 = buf[(bufSize - 1) - i];
        buf[(bufSize - 1) - i] = tmp;
    }
}

DQN_FILE_SCOPE bool DqnStr_EndsWith(char const *src, i32 srcLen, char const *find, i32 findLen,
                                    Dqn::IgnoreCase ignore)
{
    if (!src || !find || findLen < 0 || srcLen < 0) return false;

    if (srcLen < findLen)
        return false;

    char const *srcEnd       = src + (srcLen);
    char const *checkSrcFrom = srcEnd - findLen;

    bool result = (DqnStr_Cmp(checkSrcFrom, find, findLen, ignore) == 0);
    return result;
}

DQN_FILE_SCOPE i32 DqnStr_FindFirstOccurence(char const *src, i32 srcLen,
                                             char const *find, i32 findLen, Dqn::IgnoreCase ignore)
{
    if (!src || !find)               return -1;
    if (srcLen == 0 || findLen == 0) return -1;
    if (srcLen < findLen)            return -1;

    for (i32 indexIntoSrc = 0; indexIntoSrc < srcLen; indexIntoSrc++)
    {
        // NOTE: As we scan through, if the src string we index into becomes
        // shorter than the substring we're checking then the substring is not
        // contained in the src string.
        i32 remainingLenInSrcStr = srcLen - indexIntoSrc;
        if (remainingLenInSrcStr < findLen) break;

        const char *srcSubStr = src + indexIntoSrc;
        if (DqnStr_Cmp(srcSubStr, find, findLen, ignore) == 0)
        {
            return indexIntoSrc;
        }
    }

    // NOTE(doyle): We have early exit, if we reach here, then the substring was
    // not found.
    return -1;
}

DQN_FILE_SCOPE char *DqnStr_GetFirstOccurence(char const *src, i32 srcLen, char const *find,
                                              i32 findLen, Dqn::IgnoreCase ignore)
{
    i32 offset = DqnStr_FindFirstOccurence(src, srcLen, find, findLen, ignore);
    if (offset == -1) return nullptr;

    char *result = (char *)(src + offset);
    return result;
}

DQN_FILE_SCOPE bool DqnStr_HasSubstring(char const *src, i32 srcLen,
                                        char const *find, i32 findLen, Dqn::IgnoreCase ignore)
{
    if (DqnStr_FindFirstOccurence(src, srcLen, find, findLen, ignore) == -1)
        return false;

    return true;
}

DQN_FILE_SCOPE i32 Dqn_I64ToStr(i64 value, char *buf, i32 bufSize)
{
    bool validBuffer = true;
    if (!buf || bufSize == 0) validBuffer = false;

    if (value == 0)
    {
        if (validBuffer)
        {
            buf[0] = '0';
            buf[1] = 0;
        }

        return 1;
    }
    
    i32 charIndex = 0;
    bool negative           = false;
    if (value < 0) negative = true;

    if (negative)
    {
        if (validBuffer) buf[charIndex] = '-';
        charIndex++;
    }

    bool lastDigitDecremented = false;
    i64 val = DQN_ABS(value);
    if (val < 0)
    {
        // TODO(doyle): This will occur if we are checking the smallest number
        // possible in i64 since the range of negative numbers is one more than
        // it is for positives, so ABS will fail.
        lastDigitDecremented = true;
        val                  = DQN_ABS(val - 1);
        DQN_ASSERT(val >= 0);
    }

    if (validBuffer)
    {
        if (lastDigitDecremented)
        {
            i64 rem = (val % 10) + 1;
            buf[charIndex++] = (u8)rem + '0';
            val /= 10;
        }

        while (val != 0 && charIndex < bufSize)
        {
            i64 rem          = val % 10;
            buf[charIndex++] = (u8)rem + '0';
            val /= 10;
        }

        // NOTE(doyle): If string is negative, we only want to reverse starting
        // from the second character, so we don't put the negative sign at the
        // end
        if (negative)
        {
            DqnStr_Reverse(buf + 1, charIndex - 1);
        }
        else
        {
            DqnStr_Reverse(buf, charIndex);
        }
    }
    else
    {
        while (val != 0)
        {
            val /= 10;
            charIndex++;
        }
    }

    buf[charIndex] = 0;
    return charIndex;
}

DQN_FILE_SCOPE i64 Dqn_StrToI64(char const *buf, i64 bufSize)
{
    if (!buf || bufSize == 0) return 0;

    i64 index = 0;
    while (buf[index] == ' ')
    {
        index++;
    }

    bool isNegative = false;
    if (buf[index] == '-' || buf[index] == '+')
    {
        if (buf[index] == '-') isNegative = true;
        index++;
    }
    else if (!DqnChar_IsDigit(buf[index]))
    {
        return 0;
    }

    i64 result = 0;
    for (auto i = index; i < bufSize; i++)
    {
        if (DqnChar_IsDigit(buf[i]))
        {
            result *= 10;
            result += (buf[i] - '0');
        }
        else
        {
            break;
        }
    }

    if (isNegative) result *= -1;

    return result;
}

DQN_FILE_SCOPE f32 Dqn_StrToF32(char const *buf, i64 bufSize)
{
    if (!buf || bufSize == 0) return 0;

    i64 index       = 0;
    bool isNegative = false;
    if (buf[index] == '-')
    {
        index++;
        isNegative = true;
    }

    bool isPastDecimal        = false;
    i64 numDigitsAfterDecimal = 0;
    i64 rawNumber             = 0;

    f32 digitShiftValue      = 1.0f;
    f32 digitShiftMultiplier = 0.1f;
    for (auto i = index; i < bufSize; i++)
    {
        char ch = buf[i];
        if (ch == '.')
        {
            isPastDecimal = true;
            continue;
        }
        // Handle scientific notation
        else if (ch == 'e')
        {
            bool digitShiftIsPositive = true;
            if (i < bufSize)
            {
                if (buf[i + 1] == '-') digitShiftIsPositive = false;
                DQN_ASSERT(buf[i + 1] == '-' || buf[i + 1] == '+');
                i += 2;
            }

            i32 exponentPow         = 0;
            bool scientificNotation = false;
            while (i < bufSize)
            {
                scientificNotation = true;
                char exponentCh    = buf[i];
                if (DqnChar_IsDigit(exponentCh))
                {
                    exponentPow *= 10;
                    exponentPow += (buf[i] - '0');
                }
                else
                {
                    i = bufSize;
                }

                i++;
            }

            // NOTE(doyle): If exponent not specified but this branch occurred,
            // the float string has a malformed scientific notation in the
            // string, i.e. "e" followed by no number.
            DQN_ASSERT(scientificNotation);

            if (digitShiftIsPositive)
            {
                numDigitsAfterDecimal -= exponentPow;
            }
            else
            {
                numDigitsAfterDecimal += exponentPow;
            }
        }
        else if (DqnChar_IsDigit(ch))
        {
            numDigitsAfterDecimal += (i32)isPastDecimal;
            rawNumber *= 10;
            rawNumber += (ch - '0');
        }
        else
        {
            break;
        }
    }

    for (auto i = 0; i < numDigitsAfterDecimal; i++)
        digitShiftValue *= digitShiftMultiplier;

    f32 result = (f32)rawNumber;
    if (numDigitsAfterDecimal > 0) result *= digitShiftValue;
    if (isNegative) result *= -1;

    return result;
}

/*
    Encoding
    The following byte sequences are used to represent a character. The sequence
    to be used depends on the UCS code number of the character:

    The extra 1's are the headers used to identify the string as a UTF-8 string.
    UCS [0x00000000, 0x0000007F] -> UTF-8 0xxxxxxx
    UCS [0x00000080, 0x000007FF] -> UTF-8 110xxxxx 10xxxxxx
    UCS [0x00000800, 0x0000FFFF] -> UTF-8 1110xxxx 10xxxxxx 10xxxxxx
    UCS [0x00010000, 0x001FFFFF] -> UTF-8 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    UCS [0x00200000, 0x03FFFFFF] -> N/A   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    UCS [0x04000000, 0x7FFFFFFF] -> N/A   1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

    The xxx bit positions are filled with the bits of the character code number
    in binary representation. Only the shortest possible multibyte sequence
    which can represent the code number of the character can be used.

    The UCS code values 0xd800–0xdfff (UTF-16 surrogates) as well as 0xfffe and
    0xffff (UCS noncharacters) should not appear in conforming UTF-8 streams.
*/
DQN_FILE_SCOPE u32 Dqn_UCSToUTF8(u32 *dest, u32 character)
{
    if (!dest) return 0;

    u8 *bytePtr = (u8 *)dest;

    // Character is within ASCII range, so it's an ascii character
    // UTF Bit Arrangement: 0xxxxxxx
    // Character          : 0xxxxxxx
    if (character >= 0 && character < 0x80)
    {
        bytePtr[0] = (u8)character;
        return 1;
    }

    // UTF Header Bits    : 11000000 00xxxxxx
    // UTF Bit Arrangement: 000xxxxx 00xxxxxx
    // Character          : 00000xxx xxxxxxxx
    if (character < 0x800)
    {
        // Add the 2nd byte, 6 bits, OR the 0xC0 (11000000) header bits
        bytePtr[1] = (u8)((character >> 6) | 0xC0);

        // Add the 1st byte, 6 bits, plus the 0x80 (10000000) header bits
        bytePtr[0] = (u8)((character & 0x3F) | 0x80);

        return 2;
    }

    // UTF Header Bits     : 11100000 10000000 10000000
    // UTF Bit Arrangement : 0000xxxx 00xxxxxx 00xxxxxx
    // Character           : 00000000 xxxxxxxx xxxxxxxx
    if (character < 0x10000)
    {
        // Add the 3rd byte, 4 bits, OR the 0xE0 (11100000) header bits
        bytePtr[2] = (u8)((character >> 12) | 0xE0);

        // Add the 2nd byte, 6 bits, OR the 0x80 (10000000) header bits
        bytePtr[1] = (u8)((character >> 6) | 0x80);

        // Add the 1st byte, 6 bits, plus the 0x80 (10000000) header bits
        bytePtr[0] = (u8)((character & 0x3F) | 0x80);

        return 3;
    }

    // UTF Header Bits     : 11110000 10000000 10000000 10000000
    // UTF Bit Arrangement : 00000xxx 00xxxxxx 00xxxxxx 00xxxxxx
    // Character           : 00000000 00000xxx xxxxxxxx xxxxxxxx
    if (character < 0x110000)
    {
        // Add the 4th byte, 3 bits, OR the 0xF0 (11110000) header bits
        bytePtr[3] = (u8)((character >> 18) | 0xF0);

        // Add the 3rd byte, 6 bits, OR the 0x80 (10000000) header bits
        bytePtr[2] = (u8)(((character >> 12) & 0x3F) | 0x80);

        // Add the 2nd byte, 6 bits, plus the 0x80 (10000000) header bits
        bytePtr[1] = (u8)(((character >> 6) & 0x3F) | 0x80);

        // Add the 2nd byte, 6 bits, plus the 0x80 (10000000) header bits
        bytePtr[0] = (u8)((character & 0x3F) | 0x80);

        return 4;
    }

    return 0;
}

DQN_FILE_SCOPE u32 Dqn_UTF8ToUCS(u32 *dest, u32 character)
{
    if (!dest) return 0;

    const u32 HEADER_BITS_4_BYTES = 0xF0808080u;
    const u32 HEADER_BITS_3_BYTES = 0xE08080u;
    const u32 HEADER_BITS_2_BYTES = 0xC000u;
    const u32 HEADER_BITS_1_BYTE  = 0x80u;

    // UTF Header Bits     : 11110000 10000000 10000000 10000000
    // UTF Bit Arrangement : 00000xxx 00xxxxxx 00xxxxxx 00xxxxxx
    // UCS                 : 00000000 00000xxx xxxxxxxx xxxxxxxx
    if ((character & HEADER_BITS_4_BYTES) == HEADER_BITS_4_BYTES)
    {
        u32 utfWithoutHeader = HEADER_BITS_4_BYTES ^ character;

        u32 firstByte  = utfWithoutHeader & 0x3F;
        u32 secondByte = (utfWithoutHeader >> 8) & 0x3F;
        u32 thirdByte  = (utfWithoutHeader >> 16) & 0x3F;
        u32 fourthByte = utfWithoutHeader >> 24;

        u32 result =
            (fourthByte << 18 | thirdByte << 12 | secondByte << 6 | firstByte);
        *dest = result;

        return 4;
    }

    // UTF Header Bits     : 11100000 10000000 10000000
    // UTF Bit Arrangement : 0000xxxx 00xxxxxx 00xxxxxx
    // UCS                 : 00000000 xxxxxxxx xxxxxxxx
    if ((character & HEADER_BITS_3_BYTES) == HEADER_BITS_3_BYTES)
    {
        u32 utfWithoutHeader = HEADER_BITS_3_BYTES ^ character;

        u32 firstByte  = utfWithoutHeader & 0x3F;
        u32 secondByte = (utfWithoutHeader >> 8) & 0x3F;
        u32 thirdByte  = utfWithoutHeader >> 16;

        u32 result = (thirdByte << 12 | secondByte << 6 | firstByte);
        *dest = result;

        return 3;
    }

    // UTF Header Bits    : 11000000 00xxxxxx
    // UTF Bit Arrangement: 000xxxxx 00xxxxxx
    // UCS                : 00000xxx xxxxxxxx
    if ((character & HEADER_BITS_2_BYTES) == HEADER_BITS_2_BYTES)
    {
        u32 utfWithoutHeader = HEADER_BITS_2_BYTES ^ character;

        u32 firstByte  = utfWithoutHeader & 0x3F;
        u32 secondByte = utfWithoutHeader >> 8;

        u32 result = (secondByte << 6 | firstByte);
        *dest = result;

        return 2;
    }

    // Character is within ASCII range, so it's an ascii character
    // UTF Bit Arrangement: 0xxxxxxx
    // UCS                : 0xxxxxxx
    if ((character & HEADER_BITS_1_BYTE) == 0)
    {
        u32 firstByte = (character & 0x3F);
        *dest         = firstByte;

        return 1;
    }

    return 0;
}

DQN_FILE_SCOPE DqnSlice<char> DqnStr_RemoveLeadTrailChar(char const *str, i32 strLen, char leadChar, char trailChar)
{
    str = DqnChar_TrimWhitespaceAround(str, strLen, &strLen);

    if (str[0] == leadChar)
    {
        str++;
        strLen--;
    }

    if (str[strLen - 1] == trailChar)
    {
        strLen--;
    }

    str = DqnChar_TrimWhitespaceAround(str, strLen, &strLen);
    DqnSlice<char> result = {(char *)str, strLen};
    return result;
}

DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailBraces(char const *str, i32 strLen)
{
    DqnSlice<char> result = DqnStr_RemoveLeadTrailChar(str, strLen, '{', '}');
    return result;
}

DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailQuotes(char const *str, i32 strLen)
{
    DqnSlice<char> result = DqnStr_RemoveLeadTrailChar(str, strLen, '"', '"');
    return result;
}

DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailBraces(DqnSlice<char> slice)
{
    DqnSlice<char> result = DqnStr_RemoveLeadTrailBraces(slice.data, slice.len);
    return result;
}

DQN_FILE_SCOPE inline DqnSlice<char> DqnStr_RemoveLeadTrailQuotes(DqnSlice<char> slice)
{
    DqnSlice<char> result = DqnStr_RemoveLeadTrailQuotes(slice.data, slice.len);
    return result;
}

// #DqnWChar
// =================================================================================================
DQN_FILE_SCOPE bool DqnWChar_IsDigit(wchar_t c)
{
    if (c >= L'0' && c <= L'9') return true;
    return false;
}

DQN_FILE_SCOPE wchar_t DqnWChar_ToLower(wchar_t c)
{
    if (c >= L'A' && c <= L'Z')
    {
        i32 shiftOffset = L'a' - L'A';
        return (c + (wchar_t)shiftOffset);
    }

    return c;
}

DQN_FILE_SCOPE wchar_t *DqnWChar_SkipWhitespace(wchar_t *ptr)
{
    while (ptr && (*ptr == ' ' || *ptr == '\r' || *ptr == '\n')) ptr++;
    return ptr;
}

DQN_FILE_SCOPE wchar_t *DqnWChar_FindLastChar(wchar_t *ptr, wchar_t ch, i32 len, u32 *lenToChar)
{
    for (i32 i = len - 1; i >= 0; i--)
    {
        if (ptr[i] == ch)
        {
            if (lenToChar) *lenToChar = (u32)len - i;
            return &ptr[i];
        }
    }

    return nullptr;
}

DQN_FILE_SCOPE i32 DqnWChar_GetNextLine(wchar_t *ptr, i32 *lineLength)
{
    i32 len = 0;
    ptr     = DqnWChar_SkipWhitespace(ptr);

    // Advance pointer to first new line
    while (ptr && *ptr != 0 && *ptr != '\r' && *ptr != '\n')
    {
        ptr++;
        len++;
    }

    if (!ptr || *ptr == 0) return -1;

    // Destroy all new lines
    i32 extraChars = 0;
    while (ptr && (*ptr == '\r' || *ptr == '\n' || *ptr == ' '))
    {
        *ptr = 0;
        ptr++;
        extraChars++;
    }

    if (lineLength) *lineLength = len;
    return len + extraChars;
}

// #DqnWStr
// =================================================================================================
DQN_FILE_SCOPE i32 DqnWStr_Cmp(wchar_t const *a, wchar_t const *b)
{
    if (!a && !b) return -1;
    if (!a) return -1;
    if (!b) return -1;

    const wchar_t *aPtr = a;
    const wchar_t *bPtr = b;

    while ((*aPtr) == (*bPtr))
    {
        if (!(*aPtr)) return 0;
        aPtr++;
        bPtr++;
    }

    return (((*aPtr) < (*bPtr)) ? -1 : 1);
}

DQN_FILE_SCOPE i32 DqnWStr_FindFirstOccurence(wchar_t const *src, i32 srcLen,
                                              wchar_t const *find, i32 findLen)
{
    if (!src || !find)               return -1;
    if (srcLen == 0 || findLen == 0) return -1;
    if (srcLen < findLen)            return -1;

    for (i32 indexIntoSrc = 0; indexIntoSrc < srcLen; indexIntoSrc++)
    {
        // NOTE: As we scan through, if the src string we index into becomes
        // shorter than the substring we're checking then the substring is not
        // contained in the src string.
        i32 remainingLenInSrcStr = srcLen - indexIntoSrc;
        if (remainingLenInSrcStr < findLen) break;

        const wchar_t *srcSubStr = &src[indexIntoSrc];
        i32 index = 0;
        for (;;)
        {
            if (DqnWChar_ToLower(srcSubStr[index]) ==
                DqnWChar_ToLower(find[index]))
            {
                index++;
                if (index >= findLen || !find[index])
                {
                    return indexIntoSrc;
                }
            }
            else
            {
                break;
            }
        }
    }

    // NOTE(doyle): We have early exit, if we reach here, then the substring was
    // not found.
    return -1;
}

DQN_FILE_SCOPE bool DqnWStr_HasSubstring(wchar_t const *src, i32 srcLen,
                                         wchar_t const *find, i32 findLen)
{
    if (DqnWStr_FindFirstOccurence(src, srcLen, find, findLen) == -1)
        return false;

    return true;
}

DQN_FILE_SCOPE i32 DqnWStr_Len(wchar_t const *a)
{
    i32 result = 0;
    while (a && a[result]) result++;
    return result;
}

DQN_FILE_SCOPE i32 DqnWStr_LenDelimitWith(wchar_t const *a, wchar_t delimiter)
{
    i32 result = 0;
    while (a && a[result] && a[result] != delimiter) result++;
    return result;
}

DQN_FILE_SCOPE void DqnWStr_Reverse(wchar_t *buf, i32 bufSize)
{
    if (!buf) return;
    i32 mid = bufSize / 2;

    for (i32 i = 0; i < mid; i++)
    {
        wchar_t tmp            = buf[i];
        buf[i]                 = buf[(bufSize - 1) - i];
        buf[(bufSize - 1) - i] = tmp;
    }
}

DQN_FILE_SCOPE i32 Dqn_WStrToI32(wchar_t const *buf, i32 bufSize)
{
    if (!buf || bufSize == 0) return 0;

    i32 index       = 0;
    bool isNegative = false;
    if (buf[index] == L'-' || buf[index] == L'+')
    {
        if (buf[index] == L'-') isNegative = true;
        index++;
    }
    else if (!DqnWChar_IsDigit(buf[index]))
    {
        return 0;
    }

    i32 result = 0;
    for (i32 i = index; i < bufSize; i++)
    {
        if (DqnWChar_IsDigit(buf[i]))
        {
            result *= 10;
            result += (buf[i] - L'0');
        }
        else
        {
            break;
        }
    }

    if (isNegative) result *= -1;

    return result;
}

DQN_FILE_SCOPE i32 Dqn_I32ToWstr(i32 value, wchar_t *buf, i32 bufSize)
{
    if (!buf || bufSize == 0) return 0;

    if (value == 0)
    {
        buf[0] = L'0';
        return 0;
    }

    // NOTE(doyle): Max 32bit integer (+-)2147483647
    i32 charIndex = 0;
    bool negative           = false;
    if (value < 0) negative = true;

    if (negative) buf[charIndex++] = L'-';

    i32 val = DQN_ABS(value);
    while (val != 0 && charIndex < bufSize)
    {
        i32 rem          = val % 10;
        buf[charIndex++] = (u8)rem + '0';
        val /= 10;
    }

    // NOTE(doyle): If string is negative, we only want to reverse starting
    // from the second character, so we don't put the negative sign at the end
    if (negative)
    {
        DqnWStr_Reverse(buf + 1, charIndex - 1);
    }
    else
    {
        DqnWStr_Reverse(buf, charIndex);
    }

    return charIndex;
}

// #DqnRnd
// =================================================================================================
// Public Domain library with thanks to Mattias Gustavsson
// https://github.com/mattiasgustavsson/libs/blob/master/docs/rnd.md

// Convert a randomized u32 value to a float value x in the range 0.0f <= x
// < 1.0f. Contributed by Jonatan Hedborg

// NOTE: This is to abide to strict aliasing rules.
union DqnRnd__U32F32
{
    u32 unsigned32;
    f32 float32;
};

FILE_SCOPE f32 DqnRnd__F32NormalizedFromU32(u32 value)
{
    u32 exponent = 127;
    u32 mantissa = value >> 9;

    union DqnRnd__U32F32 uf;
    uf.unsigned32 = (exponent << 23 | mantissa);
    return uf.float32 - 1.0f;
}

FILE_SCOPE u64 DqnRnd__Murmur3Avalanche64(u64 h)
{
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccd;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53;
    h ^= h >> 33;
    return h;
}

#if defined(DQN__IS_UNIX)
    #include <x86intrin.h> // __rdtsc
#endif

FILE_SCOPE u32 DqnRnd__MakeSeed()
{
#if defined(DQN_PLATFORM_IMPLEMENTATION) && (defined(DQN__IS_WIN32) || defined(DQN__IS_UNIX))
    i64 numClockCycles = __rdtsc();
    return (u32)numClockCycles;
#else
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return 0;
#endif
}

DqnRndPCG::DqnRndPCG()
{
    u32 seed = DqnRnd__MakeSeed();
    *this    = DqnRndPCG(seed);
}

DqnRndPCG::DqnRndPCG(u32 seed)
{
    u64 value      = (((u64)seed) << 1ULL) | 1ULL;
    value          = DqnRnd__Murmur3Avalanche64(value);
    this->state[0] = 0U;
    this->state[1] = (value << 1ULL) | 1ULL;
    this->Next();
    this->state[0] += DqnRnd__Murmur3Avalanche64(value);
    this->Next();
}

u32 DqnRndPCG::Next()
{
    u64 oldstate   = this->state[0];
    this->state[0] = oldstate * 0x5851f42d4c957f2dULL + this->state[1];
    u32 xorshifted = (u32)(((oldstate >> 18ULL) ^ oldstate) >> 27ULL);
    u32 rot        = (u32)(oldstate >> 59ULL);
    return (xorshifted >> rot) | (xorshifted << ((-(i32)rot) & 31));
}

f32 DqnRndPCG::Nextf()
{
    f32 result = DqnRnd__F32NormalizedFromU32(this->Next());
    return result;
}

i32 DqnRndPCG::Range(i32 min, i32 max)
{
    i32 const range = (max - min) + 1;
    if (range <= 0) return min;

    i32 const value = (i32)(this->Nextf() * range);
    i32 result      = min + value;
    return result;
}


// #Dqn_*
// =================================================================================================
DQN_FILE_SCOPE inline bool Dqn_BitIsSet(u32 bits, u32 flag)
{
    bool result = ((bits & flag) == flag);
    return result;
}

DQN_FILE_SCOPE inline u32 Dqn_BitSet(u32 bits, u32 flag)
{
    u32 result = (bits | flag);
    return result;
}

DQN_FILE_SCOPE inline u32 Dqn_BitUnset(u32 bits, u32 flag)
{
    u32 result = (bits & ~flag);
    return result;
}

DQN_FILE_SCOPE inline u32 Dqn_BitToggle(u32 bits, u32 flag)
{
    u32 result = (bits ^ flag);
    return result;
}


DQN_FILE_SCOPE inline bool Dqn_QuickSortDqnStringSorter(struct DqnString const *a, struct DqnString const *b, void *)
{
    bool result =  DqnString::Cmp(a, b);
    return result;
}

// #DqnString Impleemntation
// =================================================================================================
// TODO(doyle): SSO requires handling assign/copy op when copying strings, we need to reassign the
//              str to point to the new copy's SSO buffer which sort of breaks the way I want to use
//              strings. So disabled for now.
// #define DQN_STRING_ENABLE_SSO

void DqnString::Init(DqnMemAPI *api)
{
    DQN_ASSERT(api);

    this->memAPI = api;
#if defined(DQN_STRING_ENABLE_SSO)
    this->sso[0] = 0;
#endif
    this->str    = nullptr;
    this->len    = 0;
    this->max    = 0;
}

void DqnString::Init(DqnMemStack *stack)
{
    this->Init(&stack->myHeadAPI);
}

bool DqnString::InitSize(i32 size, DqnMemStack *const stack)
{
    bool result = this->InitSize(size, &stack->myHeadAPI);
    return result;
}

bool DqnString::InitSize(i32 size, DqnMemAPI *const api)
{
    // NOTE: CHAR_COUNT is (ARRAY_COUNT - 1) to leave the last spot as the implicit null-terminator.
    DQN_ASSERT(size >= 0);
#if defined(DQN_STRING_ENABLE_SSO)
    if (size < DQN_CHAR_COUNT(this->sso))
    {
        this->str = &(this->sso[0]);
    }
    else
#endif
    if (size == 0)
    {
        this->str = nullptr;
    }
    else
    {
        usize allocSize = sizeof(*(this->str)) * (size + 1);
        this->str        = (char *)api->Alloc(allocSize, Dqn::ZeroClear::No);
        if (!this->str) return false;

        this->str[0] = 0;
    }

    this->max    = size;
    this->len    = 0;
    this->memAPI = api;
    return true;
}

bool DqnString::InitFixedMem(char *const memory, i32 sizeInBytes)
{
    if (!memory || sizeInBytes == 0) return false;

    this->str    = (char *)memory;
    this->len    = 0;
    this->max    = sizeInBytes - 1;
    this->memAPI = {};

    return true;
}

bool DqnString::InitLiteral(char const *cstr, DqnMemStack *stack)
{
    bool result = this->InitLiteral(cstr, &stack->myHeadAPI);
    return result;
}

bool DqnString::InitLiteral(char const *cstr, i32 lenInBytes, DqnMemStack *stack)
{
    bool result = this->InitLiteral(cstr, lenInBytes, &stack->myHeadAPI);
    return result;
}

bool DqnString::InitLiteral(char const *cstr, i32 lenInBytes, DqnMemAPI *api)
{
    if (!this->InitSize(lenInBytes, api))
    {
        return false;
    }

    if (lenInBytes > 0)
    {
        this->str[lenInBytes] = 0;
    }

    this->len = lenInBytes;
    this->max = this->len;
    DqnMem_Copy(this->str, cstr, lenInBytes);

    return true;
}

bool DqnString::InitLiteral(char const *cstr, DqnMemAPI *api)
{
    i32 utf8LenInBytes = 0;
    DqnStr_LenUTF8((u32 *)cstr, &utf8LenInBytes);

    bool result = this->InitLiteral(cstr, utf8LenInBytes, api);

    return result;
}

bool DqnString::InitLiteral(wchar_t const *cstr, DqnMemStack *stack)
{
    bool result = this->InitLiteral(cstr, &stack->myHeadAPI);
    return result;
}

bool DqnString::InitLiteral(wchar_t const *cstr, DqnMemAPI *api)
{
#if defined(DQN__IS_WIN32) && defined(DQN_PLATFORM_IMPLEMENTATION)
    i32 reqLenInclNullTerminator = DqnWin32_WCharToUTF8(cstr, nullptr, 0);
    if (!this->InitSize(reqLenInclNullTerminator - 1, api))
    {
        return false;
    }

    this->len         = reqLenInclNullTerminator - 1;
    this->max         = this->len;
    i32 convertResult = DqnWin32_WCharToUTF8(cstr, this->str, this->len + 1);
    DQN_ASSERT(convertResult != -1);
    this->str[this->len] = 0;

    return true;

#else
    (void)cstr; (void)api;
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return false;

#endif
}

bool DqnString::InitLiteralNoAlloc(char *cstr, i32 cstrLen)
{
    if (!cstr) return false;

    this->str = cstr;
    if (cstrLen == -1)
    {
        i32 utf8LenInBytes = 0;
        DqnStr_LenUTF8((u32 *)cstr, &utf8LenInBytes);
        this->len = utf8LenInBytes;
    }
    else
    {
        this->len = cstrLen;
    }

    this->max = this->len;
    return true;
}

bool DqnString::Expand(i32 newMax)
{
    if (newMax < this->max)
    {
        return true;
    }

    if (!this->memAPI || !this->memAPI->IsValid())
    {
        return false;
    }

#if defined(DQN_STRING_ENABLE_SSO)
    if (newMax < DQN_CHAR_COUNT(this->sso))
    {
        DQN_ASSERT(this->memAPI->IsValid());
        DQN_ASSERT(this->sso == this->str || this->str == nullptr);
        this->str = this->sso;
        this->max = newMax;
        return true;
    }

    bool usingSSO = (this->str == this->sso);
    if (usingSSO)
    {
        DQN_ASSERT(newMax >= DQN_CHAR_COUNT(this->sso));
        this->str = nullptr;
    }
#endif

    usize allocSize = sizeof(*(this->str)) * (newMax + 1);
    char *result     = nullptr;

    if (this->str) result = (char *)this->memAPI->Realloc(this->str, (this->max + 1), allocSize);
    else           result = (char *)this->memAPI->Alloc(allocSize, Dqn::ZeroClear::No);

    if (result)
    {
#if defined(DQN_STRING_ENABLE_SSO)
        if (usingSSO)
            DqnMem_Copy(result, this->sso, this->max);
#endif

        this->str = (char *)result;
        this->max = newMax;
        return true;
    }
#if defined(DQN_STRING_ENABLE_SSO)
    else
    {
        // Restore the pointer to the SSO to return to the original state from before this call.
        if (usingSSO)
            this->str = this->sso;
    }
#endif

    return false;
}

DQN_FILE_SCOPE bool DqnStringInternal_Append(DqnString *str, char const *cstr,
                                             i32 bytesToCopy)
{
    if (bytesToCopy <= 0)
    {
        return true;
    }

    // Check and reserve space if needed
    i32 totalLen = str->len + bytesToCopy;
    if (totalLen > str->max)
    {
        bool result = str->Expand(totalLen);
        if (!result) return false;
    }

    char *dest = str->str + str->len;
    DqnMem_Copy(dest, cstr, bytesToCopy);

    str->len           = totalLen;
    str->str[totalLen] = 0;
    return true;
}

bool DqnString::Sprintf(char const *fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);
    bool result = this->VSprintf(fmt, argList);
    va_end(argList);

    return result;
}

bool DqnString::VSprintf(char const *fmt, va_list argList)
{
    LOCAL_PERSIST char tmp[STB_SPRINTF_MIN];

    auto PrintCallback = [](char *buf, void *user, int len) -> char *
    {
        (void)len; (void)user;
        return buf;
    };

    i32  const reqLen          = Dqn_vsprintfcb(PrintCallback, nullptr, tmp, fmt, argList);
    auto const remainingSpace  = this->max - this->len;
    if (reqLen > remainingSpace)
    {
        i32 const newMax = this->max + reqLen;
        if (!this->Expand(newMax))
        {
            return false;
        }
    }

    char *bufStart = this->str + this->len;
    i32 numCopied = Dqn_vsprintf(bufStart, fmt, argList);
    this->len += numCopied;
    DQN_ASSERT(this->len <= this->max);

    return true;
}

bool DqnString::Append(char const *cstr, i32 bytesToCopy)
{
    i32 cstrLen = 0;
    if (bytesToCopy == -1)
    {
        i32 utf8LenInBytes = 0;
        DqnStr_LenUTF8((u32 *)cstr, &utf8LenInBytes);
        cstrLen = utf8LenInBytes;
    }
    else
    {
        cstrLen = bytesToCopy;
    }

    bool result = DqnStringInternal_Append(this, cstr, cstrLen);
    return result;
}

bool DqnString::Append(DqnString const string)
{
    bool result = DqnStringInternal_Append(this, string.str, string.len);
    return result;
}

bool DqnString::Append(DqnString const *string)
{
    bool result = DqnStringInternal_Append(this, string->str, string->len);
    return result;
}

void DqnString::Clear()
{
    this->len = 0;
    if (this->str)
    {
        this->str[0] = 0;
    }
}

void DqnString::Free()
{
    if (this->str)
    {
#if defined(DQN_STRING_ENABLE_SSO)
        if (this->str == this->sso)
        {
            this->sso[0] = '\0';
            this->str    = nullptr;
        }
        else
#endif
        if (this->memAPI && this->memAPI->IsValid())
        {
            this->memAPI->Free(this->str, (this->max + 1));
            this->str = nullptr;
        }

        this->len = 0;
        this->max = 0;
    }
}

i32 DqnString::ToWChar(wchar_t *buf, i32 bufSize) const
{
#if defined(DQN__IS_WIN32) && defined(DQN_PLATFORM_IMPLEMENTATION)
    i32 result = DqnWin32_UTF8ToWChar(this->str, buf, bufSize);
    return result;

#else
    (void)buf; (void)bufSize;
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return -1;
#endif
}

wchar_t *DqnString::ToWChar(DqnMemAPI *api) const
{
    // TODO(doyle): Should the "in" string allow specifyign len? probably
    // Otherwise a c-string and a literal initiated string might have different lengths
    // to wchar will produce an unintuitive output
#if defined(DQN__IS_WIN32) && defined(DQN_PLATFORM_IMPLEMENTATION)
    i32 requiredLenInclNull = DqnWin32_UTF8ToWChar(this->str, nullptr, 0);

    i32 allocSize = sizeof(wchar_t) * requiredLenInclNull;
    auto *result  = (wchar_t *)api->Alloc(allocSize);
    if (!result) return nullptr;

    DqnWin32_UTF8ToWChar(this->str, result, requiredLenInclNull);
    result[requiredLenInclNull - 1] = 0;
    return result;

#else
    (void)api;
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return nullptr;

#endif
}

// #DqnFixedString Implementation
// =================================================================================================
// return: The number of bytes written to dest
FILE_SCOPE int DqnFixedString__Append(char *dest, int destSize, char const *src, int len)
{
    i32 realLen = 0;
    if (len <= -1)
    {
        char *ptr = dest;
        char *end = ptr + destSize;

        while (*src && ptr != end)
            *ptr++ = *src++;

        if (ptr == end)
        {
            DQN_ASSERT(!src[0]);
        }

        realLen = static_cast<i32>(ptr - dest);
    }
    else
    {
        realLen = DQN_MIN(len, destSize - 1);
        DqnMem_Copy(dest, src, realLen);
    }

    DQN_ASSERT(realLen <= destSize && realLen >= 0);
    dest[realLen] = 0;
    return realLen;
}

// #Dqn
// =================================================================================================
i32 Dqn_GetNumSplits(char const *src, i32 srcLen, char splitChar)
{
    auto result = Dqn_SplitString(src, srcLen, splitChar, nullptr, 0);
    return result;
}

i32 Dqn_SplitString(char const *src, i32 srcLen, char splitChar, DqnSlice<char> *array, i32 size)
{
    // TODO(doyle): Const correctness
    i32 sliceLen   = 0;
    i32 arrayIndex = 0;
    for (auto i = 0; i < srcLen; i++)
    {
        char *c = (char *)(src + i);
        if (*c == splitChar)
        {
            DqnSlice<char> slice = {c - sliceLen, sliceLen};
            if (array)
            {
                if (arrayIndex < size)
                {
                    array[arrayIndex] = slice;
                }
            }
            arrayIndex++;
            sliceLen = 0;
        }
        else
        {
            sliceLen++;
        }
    }

    DqnSlice<char> lastSlice = {(char *)src + srcLen - sliceLen, sliceLen};
    if (lastSlice.len > 0 && arrayIndex > 0)
    {
        if (array)
        {
            if (arrayIndex < size)
            {
                array[arrayIndex] = lastSlice;
            }
        }

        arrayIndex++;
    }

    return arrayIndex;
}

DQN_FILE_SCOPE i64 Dqn_BSearch(i64 *array, i64 size, i64 find,
                               Dqn_BSearchBound bound)
{
    if (size == 0 || !array) return -1;

    i64 start = 0;
    i64 end   = size - 1;
    i64 mid   = (i64)((start + end) * 0.5f);

    while (start <= end)
    {
        if (array[mid] == find)
        {
            if (bound == Dqn_BSearchBound_Normal ||
                bound == Dqn_BSearchBound_NormalLower ||
                bound == Dqn_BSearchBound_NormalHigher)
            {
                return mid;
            }
            else if (bound == Dqn_BSearchBound_Lower)
            {
                return mid - 1;
            }
            else
            {
                if ((mid + 1) >= size) return -1;
                return mid + 1;
            }
        }
        else if (array[mid] <  find) start = mid + 1;
        else                         end   = mid - 1;
        mid = (i64)((start + end) * 0.5f);
    }

    if (bound == Dqn_BSearchBound_Normal)
    {
        return -1;
    }
    if (bound == Dqn_BSearchBound_Lower || bound == Dqn_BSearchBound_NormalLower)
    {
        if (find < array[mid]) return -1;
        return mid;
    }
    else
    {
        if (find > array[mid]) return -1;
        return mid;
    }
}

DQN_FILE_SCOPE DqnJson DqnJson_Get(char const *buf, i32 bufLen, char const *findProperty, i32 findPropertyLen)
{
    DqnJson result = {};
    char *locate   = DqnStr_GetFirstOccurence(buf, bufLen, findProperty, findPropertyLen);

    if (!locate) return result;

    // NOTE: if find property is '{' we are looking for an object in array or the global scope etc
    // which doesn't have a specific property name
    char *startOfValue = locate;
    char const *bufPtr = startOfValue;
    if (findProperty[0] != '{' && findProperty[0] != '[')
    {
        // NOTE: When true, the findProperty already includes the quotation marks, so don't need to
        // check.
        if (!(findProperty[0] == '"' && findProperty[findPropertyLen - 1] == '"'))
        {
            if (locate[-1] != '"' || locate[findPropertyLen] != '"')
                return result;
        }

        if (!(locate[findPropertyLen + 1] && locate[findPropertyLen + 1] == ':'))
        {
            return result;
        }

        startOfValue = locate + findPropertyLen + 2;
        startOfValue = DqnChar_SkipWhitespace(startOfValue);
        bufPtr       = startOfValue;
    }

    i32 braceCount = 0, bracketCount = 0;
    if (bufPtr[0] == '[' || bufPtr[0] == '{')
    {
        startOfValue++;

        i32 *searchCharCount = nullptr;
        if (bufPtr[0] == '[')
        {
            bracketCount++;
            result.type     = DqnJson::Type::Array;
            searchCharCount = &bracketCount;
        }
        else
        {
            braceCount++;
            result.type     = DqnJson::Type::Object;
            searchCharCount = &braceCount;
        }

        result.numEntries = 0;
        for (bufPtr++; (*searchCharCount) != 0; bufPtr++)
        {
            bool countsChanged = true;
            if (!bufPtr[0])
                return result;

            if      (bufPtr[0] == '{') braceCount++;
            else if (bufPtr[0] == '}') braceCount--;
            else if (bufPtr[0] == '[') bracketCount++;
            else if (bufPtr[0] == ']') bracketCount--;
            else                       countsChanged = false;

            if (countsChanged)
            {
                if (result.type == DqnJson::Type::Array)
                {
                    if (braceCount == 0  && bracketCount == 1)
                    {
                        result.numEntries++;
                    }
                }
                else
                {
                    if (braceCount == 1 && bracketCount == 0)
                    {
                        result.numEntries++;
                    }
                }
            }
        }
        // Don't include the open and closing braces/brackets.
        bufPtr--;
    }
    else if (bufPtr[0] == '"' || DqnChar_IsAlphaNum(bufPtr[0]))
    {
        while(bufPtr[0] && (bufPtr[0] != '\n' && bufPtr[0] != ',' && bufPtr[0] != '}'))
            bufPtr++;

        if (!bufPtr[0])
            return result;

        result.numEntries = 1;
    }
    else
    {
        return result;
    }

    result.value.data = startOfValue;
    result.value.len  = static_cast<i32>(bufPtr - result.value.data);
    result.value.data = DqnChar_TrimWhitespaceAround(result.value.data, result.value.len, &result.value.len);
    return result;
}

DQN_FILE_SCOPE DqnJson DqnJson_Get(DqnSlice<char> const buf, DqnSlice<char> const findProperty)
{
    DqnJson result = DqnJson_Get(buf.data, buf.len, findProperty.data, findProperty.len);
    return result;
}

DQN_FILE_SCOPE DqnJson DqnJson_Get(DqnSlice<char const> const buf, DqnSlice<char const> const findProperty)
{
    DqnJson result = DqnJson_Get(buf.data, buf.len, findProperty.data, findProperty.len);
    return result;
}

DQN_FILE_SCOPE DqnJson DqnJson_Get(DqnSlice<char> const buf, DqnSlice<char const> const findProperty)
{
    DqnJson result = DqnJson_Get(buf.data, buf.len, findProperty.data, findProperty.len);
    return result;
}

DQN_FILE_SCOPE DqnJson DqnJson_Get(DqnJson const input, DqnSlice<char> const findProperty)
{
    DqnJson result = DqnJson_Get(input.value.data, input.value.len, findProperty.data, findProperty.len);
    return result;
}

DQN_FILE_SCOPE DqnJson DqnJson_Get(DqnJson const input, DqnSlice<char const> const findProperty)
{
    DqnJson result = DqnJson_Get(input.value.data, input.value.len, findProperty.data, findProperty.len);
    return result;
}

DQN_FILE_SCOPE DqnJson DqnJson_GetNextArrayItem(DqnJson *iterator)
{
    DqnJson result = {};
    if (iterator->type != DqnJson::Type::Array || iterator->numEntries <= 0)
        return result;

    result = DqnJson_Get(iterator->value, DQN_SLICE("{"));
    if (result)
    {
        char const *end      = iterator->value.data + iterator->value.len;
        iterator->value.data = result.value.data + result.value.len;
        iterator->numEntries--;

        while (iterator->value.data[0] && *iterator->value.data++ != '}')
            ;

        iterator->value.data = DqnChar_SkipWhitespace(iterator->value.data);
        if (iterator->value.data[0] && iterator->value.data[0] == ',')
            iterator->value.data++;

        iterator->value.data = DqnChar_SkipWhitespace(iterator->value.data);
        iterator->value.len  = (iterator->value.data) ? static_cast<i32>(end - iterator->value.data) : 0;
    }

    return result;
}


#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
// #DqnSprintf - STB_Sprintf
// =================================================================================================
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
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
#endif // DQN_IMPLEMENTATION

#if defined(DQN__XPLATFORM_LAYER)
// #XPlatform (Win32 & Unix)
// =================================================================================================
// Functions in the Cross Platform are guaranteed to be supported in both Unix
// and Win32

#ifdef DQN__IS_UNIX
    #include <stdio.h>    // Basic File I/O // TODO(doyle): Syscall versions

    #include <dirent.h>   // readdir()/opendir()/closedir()
    #include <sys/stat.h> // file size query
    #include <sys/time.h> // high resolution timer
    #include <time.h>     // timespec
    #include <unistd.h>   // unlink()
#endif

#define DQN_FILE__LIST_DIR(name) DQN_FILE_SCOPE char **name(char const *dir, i32 *numFiles, DqnMemAPI *api)

// XPlatform > #DqnFile
// =================================================================================================
#ifdef DQN__IS_WIN32

FILE_SCOPE bool
DqnFile__Win32Open(wchar_t const *path, DqnFile *file, u32 flags, DqnFile::Action action)
{
    if (!file || !path) return false;

    u32 const WIN32_GENERIC_READ    = 0x80000000L;
    u32 const WIN32_GENERIC_WRITE   = 0x40000000L;
    u32 const WIN32_GENERIC_EXECUTE = 0x20000000L;
    u32 const WIN32_GENERIC_ALL     = 0x10000000L;

    u32 const WIN32_CREATE_NEW        = 1;
    u32 const WIN32_CREATE_ALWAYS     = 2;
    u32 const WIN32_OPEN_EXISTING     = 3;
    u32 const WIN32_OPEN_ALWAYS       = 4;
    u32 const WIN32_TRUNCATE_EXISTING = 5;

    u32 const WIN32_FILE_ATTRIBUTE_NORMAL = 0x00000080;

    DWORD win32Permission = 0;
    if (flags & DqnFile::PermissionFlag::All)
    {
        win32Permission = WIN32_GENERIC_ALL;
    }
    else
    {
        if (flags & DqnFile::PermissionFlag::FileRead)  win32Permission |= WIN32_GENERIC_READ;
        if (flags & DqnFile::PermissionFlag::FileWrite) win32Permission |= WIN32_GENERIC_WRITE;
        if (flags & DqnFile::PermissionFlag::Execute)   win32Permission |= WIN32_GENERIC_EXECUTE;
    }

    DWORD win32Action = 0;
    switch (action)
    {
        // Allow fall through
        default: DQN_ASSERT(DQN_INVALID_CODE_PATH);
        case DqnFile::Action::OpenOnly:         win32Action = WIN32_OPEN_EXISTING; break;
        case DqnFile::Action::ClearIfExist:     win32Action = WIN32_TRUNCATE_EXISTING; break;
        case DqnFile::Action::CreateIfNotExist: win32Action = WIN32_CREATE_NEW; break;
        case DqnFile::Action::ForceCreate:      win32Action = WIN32_CREATE_ALWAYS; break;
    }

    HANDLE handle = CreateFileW(path, win32Permission, 0, nullptr, win32Action,
                                WIN32_FILE_ATTRIBUTE_NORMAL, nullptr);

    if (handle == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    LARGE_INTEGER size;
    if (GetFileSizeEx(handle, &size) == 0)
    {
        file->Close();
        DqnWin32_DisplayLastError("GetFileSizeEx() failed");
        return false;
    }

    file->handle = handle;
    file->size   = (usize)size.QuadPart;
    file->flags  = flags;
    return true;
}

DQN_FILE__LIST_DIR(DqnFile__PlatformListDir)
{
    if (!dir) return nullptr;

    i32 currNumFiles = 0;
    wchar_t wideDir[MAX_PATH] = {0};
    DqnWin32_UTF8ToWChar(dir, wideDir, DQN_ARRAY_COUNT(wideDir));

    // Enumerate number of files first
    {
        WIN32_FIND_DATAW findData = {0};
        HANDLE findHandle = FindFirstFileW(wideDir, &findData);
        if (findHandle == INVALID_HANDLE_VALUE)
        {
            DQN__WIN32_ERROR_BOX("FindFirstFile() failed.", nullptr);
            return nullptr;
        }

        bool stayInLoop = true;
        while (stayInLoop)
        {
            BOOL result = FindNextFileW(findHandle, &findData);
            if (result == 0)
            {
                DWORD error = GetLastError();

                u32 const WIN32_ERROR_NO_MORE_FILES = 18L;
                if (error != WIN32_ERROR_NO_MORE_FILES)
                {
                    DqnWin32_DisplayErrorCode(error, "FindNextFileW() failed");
                }

                stayInLoop = false;
            }
            else
            {
                currNumFiles++;
            }
        }
        FindClose(findHandle);
    }

    if (currNumFiles == 0)
    {
        *numFiles = 0;
        return nullptr;
    }

    {
        WIN32_FIND_DATAW initFind = {0};
        HANDLE findHandle = FindFirstFileW(wideDir, &initFind);
        if (findHandle == INVALID_HANDLE_VALUE)
        {
            DQN__WIN32_ERROR_BOX("FindFirstFile() failed.", nullptr);
            *numFiles = 0;
            return nullptr;
        }

        char **list = (char **)api->Alloc(sizeof(*list) * (currNumFiles), Dqn::ZeroClear::Yes);

        if (!list)
        {
            DQN_LOGE("Memory allocation failed, required: %$_d", sizeof(*list) * currNumFiles);
            *numFiles = 0;
            return nullptr;
        }

        for (auto i = 0; i < currNumFiles; i++)
        {
            // TODO(doyle): Max path is bad.
            list[i] = (char *)api->Alloc(sizeof(**list) * MAX_PATH, Dqn::ZeroClear::Yes);
            if (!list[i])
            {
                for (auto j = 0; j < i; j++)
                    api->Free(list[j]);

                DQN_LOGE("Memory allocation failed, required: %$_d", sizeof(**list) * MAX_PATH);
                *numFiles = 0;
                return nullptr;
            }
        }

        i32 listIndex = 0;
        WIN32_FIND_DATAW findData = {0};
        while (FindNextFileW(findHandle, &findData) != 0)
        {
            DqnWin32_WCharToUTF8(findData.cFileName, list[listIndex++], MAX_PATH);
        }

        *numFiles = currNumFiles;
        FindClose(findHandle);

        return list;
    }
}
#endif // DQN__IS_WIN32

#ifdef DQN__IS_UNIX
FILE_SCOPE bool DqnFile__UnixGetFileSize(char const *path, usize *size)
{
    struct stat fileStat = {};
    if (stat(path, &fileStat))
    {
        // TODO(doyle): Logging
        return false;
    }

    *size = fileStat.st_size;

    // NOTE: Can occur in some instances where files are generated on demand, i.e. /proc/cpuinfo.
    // But there can also be zero-byte files, we can't be sure. So manual check by counting bytes
    if (*size == 0)
    {
        FILE *file = fopen(path, "r");
        DQN_DEFER(fclose(file));
        u64 fileSizeInBytes = 0;

        char c = fgetc(file);
        while (c != EOF)
        {
            fileSizeInBytes++;
            c = fgetc(file);
        }
    }

    return true;
}

FILE_SCOPE bool
DqnFile__UnixOpen(char const *path, DqnFile *file, u32 flags, DqnFile::Action action)
{
    char operation  = 0;
    bool updateFlag = false;

    if (flags & DqnFile::PermissionFlag::FileWrite)
    {
        updateFlag = true;
        switch (action)
        {
            default: DQN_ASSERT(DQN_INVALID_CODE_PATH);
            case DqnFile::Action::OpenOnly:
            {
                operation   = 'r';
            }
            break;

            case DqnFile::Action::CreateIfNotExist:
            case DqnFile::Action::ClearIfExist:
            case DqnFile::Action::ForceCreate:
            {
                operation   = 'w';
            }
            break;
        }
    }
    else if ((flags & DqnFile::PermissionFlag::FileRead) ||
             (flags & DqnFile::PermissionFlag::Execute))
    {
        if (flags & DqnFile::PermissionFlag::Execute)
        {
            // TODO(doyle): Logging, UNIX doesn't have execute param for file
            // handles. Execution goes through system()
        }
        operation = 'r';
    }
    DQN_ASSERT(operation != 0);

    // TODO(doyle): What about not reading as a binary file and appending to end of file.
    u32 modeIndex     = 0;
    char mode[4]      = {};
    mode[modeIndex++] = operation;

    if (updateFlag) mode[modeIndex++] = '+';

    mode[modeIndex++] = 'b';
    DQN_ASSERT(modeIndex <= DQN_ARRAY_COUNT(mode) - 1);

    // TODO(doyle): Use open syscall
    // TODO(doyle): Query errno
    if (!DqnFile__UnixGetFileSize(path, &file->size))
    {
        return false;
    }

    file->flags = flags;
    return true;
}

DQN_FILE__LIST_DIR(DqnFile__PlatformListDir)
{
    if (!dir) return nullptr;

    // Enumerate num files
    i32 currNumFiles = 0;
    {
        DIR *const dirHandle = opendir(dir);
        if (!dirHandle) return nullptr;

        struct dirent *dirFile = readdir(dirHandle);
        while (dirFile)
        {
            currNumFiles++;
            dirFile = readdir(dirHandle);
        }
        closedir(dirHandle);
    }

    if (currNumFiles == 0)
    {
        *numFiles = 0;
        return nullptr;
    }

    // Create file list
    {
        DIR *const dirHandle = opendir(dir);
        if (!dirHandle) return nullptr;

        char **list = (char **)api->Alloc(sizeof(*list) * currNumFiles, Dqn::ZeroClear::Yes);
        if (!list)
        {
            DQN_LOGE("Memory allocation failed, required: %$_d", sizeof(*list) * currNumFiles);
            *numFiles = 0;
            return nullptr;
        }

        struct dirent *dirFile = readdir(dirHandle);
        for (auto i = 0; i < currNumFiles; i++)
        {
            list[i] = (char *)api->Alloc(sizeof(**list) * DQN_ARRAY_COUNT(dirFile->d_name), Dqn::ZeroClear::Yes);
            if (!list[i])
            {
                for (auto j = 0; j < i; j++) api->Free(list[j]);

                DQN_LOGE("Memory allocation failed, required: %$_d", sizeof(**list) * DQN_ARRAY_COUNT(dirFile->d_name));
                *numFiles = 0;
                return nullptr;
            }
        }

        u32 listIndex = 0;
        *numFiles      = currNumFiles;
        while (dirFile)
        {
            DqnMem_Copy(list[listIndex++], dirFile->d_name, DQN_ARRAY_COUNT(dirFile->d_name));
            dirFile = readdir(dirHandle);
        }
        closedir(dirHandle);

        return list;
    }
}
#endif // DQN__IS_UNIX

bool DqnFile::Open(char const *path, u32 flags_, Action action)
{
    if (!path) return false;

#if defined(DQN__IS_WIN32)
    // TODO(doyle): MAX PATH is baad
    wchar_t widePath[MAX_PATH] = {};
    DqnWin32_UTF8ToWChar(path, widePath, DQN_ARRAY_COUNT(widePath));
    return DqnFile__Win32Open(widePath, this, flags_, action);
#else
    return DqnFile__UnixOpen(path, this, flags_, action);
#endif
}

bool DqnFile::Open(wchar_t const *path, u32 flags_, Action action)
{
    if (!path) return false;

#if defined(DQN__IS_WIN32)
    return DqnFile__Win32Open(path, this, flags_, action);

#else
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return false;
#endif
}

usize DqnFile::Write(u8 const *buf, usize numBytesToWrite, usize fileOffset)
{
    // TODO(doyle): Implement when it's needed
    DQN_ASSERTM(fileOffset == 0, "File writing into offset is not implemented.");
    usize numBytesWritten = 0;

#if defined(DQN__IS_WIN32)
    DWORD bytesToWrite = (DWORD)numBytesToWrite;
    DWORD bytesWritten;
    BOOL result = WriteFile(this->handle, (void *)buf, bytesToWrite, &bytesWritten, nullptr);

    numBytesWritten = (usize)bytesWritten;
    // TODO(doyle): Better logging system
    if (result == 0)
    {
        DQN__WIN32_ERROR_BOX("ReadFile() failed.", nullptr);
    }

#else
    const usize ITEMS_TO_WRITE = 1;
    if (fwrite(buf, numBytesToWrite, ITEMS_TO_WRITE, (FILE *)this->handle) == ITEMS_TO_WRITE)
    {
        rewind((FILE *)this->handle);
        numBytesWritten = ITEMS_TO_WRITE * numBytesToWrite;
    }
#endif

    return numBytesWritten;
}

usize DqnFile::Read(u8 *buf, usize numBytesToRead)
{
    usize numBytesRead = 0;
    if (this->handle)
    {
#if defined(DQN__IS_WIN32)
        DWORD bytesToRead = (DWORD)numBytesToRead;
        DWORD bytesRead    = 0;
        HANDLE win32Handle = this->handle;

        BOOL result = ReadFile(win32Handle, (void *)buf, bytesToRead, &bytesRead, nullptr);

        numBytesRead = (usize)bytesRead;
        // TODO(doyle): 0 also means it is completing async, but still valid
        if (result == 0)
        {
            DQN__WIN32_ERROR_BOX("ReadFile() failed.", nullptr);
        }

#else
        // TODO(doyle): Syscall version
        const usize ITEMS_TO_READ = 1;
        if (fread(buf, numBytesToRead, ITEMS_TO_READ, (FILE *)this->handle) == ITEMS_TO_READ)
        {
            rewind((FILE *)this->handle);
            numBytesRead = ITEMS_TO_READ * numBytesToRead;
        }
        else
        {
            // TODO(doyle): Logging, failed read
        }
#endif
    }
    return numBytesRead;
}

u8 *DqnFile::ReadEntireFile(wchar_t const *path, usize *bufSize, DqnMemAPI *api)
{
    // TODO(doyle): Logging
    if (!path || !bufSize)
    {
        return nullptr;
    }

    usize requiredSize = 0;
    if (!DqnFile::GetFileSize(path, &requiredSize) || requiredSize == 0)
    {
        return nullptr;
    }

    auto *buf = (u8 *)api->Alloc(requiredSize, Dqn::ZeroClear::No);
    if (!buf)
    {
        return nullptr;
    }

    usize bytesRead = 0;
    if (DqnFile::ReadEntireFile(path, buf, requiredSize, &bytesRead))
    {
        *bufSize          = requiredSize;
        DQN_ASSERT(bytesRead == requiredSize);
        return buf;
    }

    api->Free(buf, requiredSize);
    return nullptr;
}

u8 *DqnFile::ReadEntireFile(char const *path, usize *bufSize, DqnMemAPI *api)
{
    // TODO(doyle): Logging

    usize requiredSize = 0;
    if (!DqnFile::GetFileSize(path, &requiredSize) || requiredSize == 0) return nullptr;

    auto *buf = (u8 *)api->Alloc(requiredSize, Dqn::ZeroClear::No);
    if (!buf) return nullptr;

    usize bytesRead = 0;
    if (DqnFile::ReadEntireFile(path, buf, requiredSize, &bytesRead))
    {
        *bufSize = requiredSize;
        DQN_ASSERT(bytesRead == requiredSize);
        return buf;
    }

    api->Free(buf, requiredSize);
    return nullptr;
}

bool DqnFile::ReadEntireFile(wchar_t const *path, u8 *buf, usize bufSize,
                             usize *bytesRead)
{
    if (!path || !buf || !bytesRead) return false;

    DqnFile file = {};
    bool result = file.Open(path, DqnFile::PermissionFlag::FileRead, DqnFile::Action::OpenOnly);

    // TODO(doyle): Logging
    if (!result) goto cleanup;

    if (file.size > bufSize)
    {
        result = false;
        goto cleanup;
    }

    *bytesRead = file.Read(buf, file.size);
    DQN_ASSERT(*bytesRead == file.size);

cleanup:
    file.Close();
    return result;
}

bool DqnFile::ReadEntireFile(const char *path, u8 *buf, usize bufSize, usize *bytesRead)
{
    if (!path || !buf || !bytesRead) return false;

    DqnFile file = {};
    bool result  = file.Open(path, DqnFile::PermissionFlag::FileRead, DqnFile::Action::OpenOnly);

    // TODO(doyle): Logging
    if (!result) goto cleanup;

    if (file.size > bufSize)
    {
        result = false;
        goto cleanup;
    }

    *bytesRead = file.Read(buf, file.size);
    DQN_ASSERT(*bytesRead == file.size);

cleanup:
    file.Close();
    return result;
}

void DqnFile::Close()
{
    if (this->handle)
    {
#if defined(DQN__IS_WIN32)
        CloseHandle(this->handle);
#else
        fclose((FILE *)this->handle);
#endif
        this->handle = nullptr;
    }

    this->size  = 0;
    this->flags = 0;
}

#if defined(DQN__IS_WIN32)
    DQN_COMPILE_ASSERT(sizeof(DWORD)  == sizeof(u32));
#endif
bool DqnFile::GetFileSize(wchar_t const *path, usize *size)
{
    if (!size || !path) return false;

    Info info = {};
    if (GetInfo(path, &info))
    {
        *size = info.size;
        return true;
    }

    return false;
}

bool DqnFile::GetFileSize(char const *path, usize *size)
{
    if (!path || !size) return false;

    // TODO(doyle): Logging
#if defined(DQN__IS_WIN32)
    // TODO(doyle): MAX PATH is baad
    wchar_t widePath[MAX_PATH] = {0};
    DqnWin32_UTF8ToWChar(path, widePath, DQN_ARRAY_COUNT(widePath));
    return DqnFile::GetFileSize(widePath, size);

#else
    // TODO(doyle): Error logging
    bool result = DqnFile__UnixGetFileSize(path, size);
    return result;
#endif
}

bool DqnFile::GetInfo(wchar_t const *path, Info *info)
{
    if (!path || !info) return false;

#if defined(DQN__IS_WIN32)
    auto FileTimeToSeconds = [](FILETIME const *time) -> i64 {
        ULARGE_INTEGER timeLargeInt = {};
        timeLargeInt.LowPart        = time->dwLowDateTime;
        timeLargeInt.HighPart       = time->dwHighDateTime;

        u64 result = (timeLargeInt.QuadPart / 10000000ULL) - 11644473600ULL;
        return result;
    };

    WIN32_FILE_ATTRIBUTE_DATA attribData = {};
    if (GetFileAttributesExW(path, GetFileExInfoStandard, &attribData))
    {
        info->createTimeInS     = FileTimeToSeconds(&attribData.ftCreationTime);
        info->lastAccessTimeInS = FileTimeToSeconds(&attribData.ftLastAccessTime);
        info->lastWriteTimeInS  = FileTimeToSeconds(&attribData.ftLastWriteTime);

        // TODO(doyle): What if usize is < Quad.part?
        LARGE_INTEGER largeInt = {};
        largeInt.HighPart      = attribData.nFileSizeHigh;
        largeInt.LowPart       = attribData.nFileSizeLow;
        info->size            = (usize)largeInt.QuadPart;

        return true;
    }

#else
    // NOTE: Wide char not supported on unix
    DQN_ASSERT(DQN_INVALID_CODE_PATH);

#endif

    return false;
}

bool DqnFile::GetInfo(char const *path, Info *info)
{
    // TODO(doyle): Logging
    if (!path || !info)
    {
        return false;
    }

#if defined(DQN__IS_WIN32)
    // TODO(doyle): MAX PATH is baad
    wchar_t widePath[MAX_PATH] = {};
    DqnWin32_UTF8ToWChar(path, widePath, DQN_ARRAY_COUNT(widePath));
    return DqnFile::GetInfo(widePath, info);

#else
    struct stat fileStat = {};
    if (stat(path, &fileStat))
    {
        return false;
    }

    info->size              = fileStat.st_size;
    info->createTimeInS     = 0;
    info->lastWriteTimeInS  = fileStat.st_mtime;
    info->lastAccessTimeInS = fileStat.st_atime;

    return true;
#endif
}


bool DqnFile::Delete(char const *path)
{
    if (!path) return false;

// TODO(doyle): Logging
#if defined(DQN__IS_WIN32)
    return DeleteFileA(path);

#else
    i32 result = unlink(path);

    if (result == 0) return true;
    return false;

#endif
}

bool DqnFile::Delete(wchar_t const *path)
{
    if (!path) return false;

    // TODO(doyle): Logging
#if defined(DQN__IS_WIN32)
    return DeleteFileW(path);

#else
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return false;

#endif
}

bool DqnFile::Copy(char const *src, char const *dest)
{
    if (!src || !dest) return false;

    // TODO(doyle): Logging
#if defined(DQN__IS_WIN32)
    BOOL result = (CopyFileA(src, dest, /*FailIfExist*/false) != 0);
    if (result == 0)
    {
        DqnWin32_DisplayLastError("CopyFile failed: ");
    }
    return (result != 0);

#else
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return false;

#endif
}

bool DqnFile::Copy(wchar_t const *src, wchar_t const *dest)
{
    if (!src || !dest) return false;

    // TODO(doyle): Logging
#if defined(DQN__IS_WIN32)
    return (CopyFileW(src, dest, /*FailIfExist*/false) != 0);

#else
    DQN_ASSERT(DQN_INVALID_CODE_PATH);
    return false;

#endif
}

char **DqnFile::ListDir(char const *dir, i32 *numFiles, DqnMemAPI *api)
{
    char **result = DqnFile__PlatformListDir(dir, numFiles, api);
    return result;
}

void DqnFile::ListDirFree(char **fileList, i32 numFiles, DqnMemAPI *api)
{
    if (fileList)
    {
        for (auto i = 0; i < numFiles; i++)
        {
            if (fileList[i]) api->Free(fileList[i]);
            fileList[i] = nullptr;
        }

        api->Free(fileList);
    }
}

// XPlatform > #DqnTimer
// =================================================================================================
#if defined (DQN__IS_WIN32)
FILE_SCOPE f64 DqnTimerInternal_Win32QueryPerfCounterTimeInMs()
{
    LOCAL_PERSIST LARGE_INTEGER queryPerformanceFrequency = {0};
    if (queryPerformanceFrequency.QuadPart == 0)
    {
        QueryPerformanceFrequency(&queryPerformanceFrequency);
        DQN_ASSERT(queryPerformanceFrequency.QuadPart != 0);
    }

    LARGE_INTEGER qpcResult;
    QueryPerformanceCounter(&qpcResult);

    // Convert to microseconds first then divide by ticks per second then to milliseconds
    qpcResult.QuadPart *= 1000000;
    f64 timestamp = qpcResult.QuadPart / (f64)queryPerformanceFrequency.QuadPart;
    timestamp /= 1000.0f;
    return timestamp;
}
#endif

DQN_FILE_SCOPE f64 DqnTimer_NowInMs()
{
    f64 result = 0;
#if defined(DQN__IS_WIN32)
    result = DQN_MAX(DqnTimerInternal_Win32QueryPerfCounterTimeInMs(), 0);

#else
    struct timespec timeSpec = {0};
    if (clock_gettime(CLOCK_MONOTONIC, &timeSpec))
    {
        // TODO(doyle): Failed logging
        DQN_ASSERT(DQN_INVALID_CODE_PATH);
    }
    else
    {
        result = (f64)((timeSpec.tv_sec * 1000.0f) + (timeSpec.tv_nsec / 1000000.0f));
    }

#endif
    return result;
};

DQN_FILE_SCOPE f64 DqnTimer_NowInS() { return DqnTimer_NowInMs() / 1000.0f; }

// XPlatform > #DqnLock
// =================================================================================================
bool DqnLock_Init(DqnLock *lock)
{
    if (!lock) return false;

#if defined(DQN__IS_WIN32)
    if (InitializeCriticalSectionEx(&lock->win32Handle, lock->win32SpinCount, 0))
        return true;
#else
    // NOTE: Static initialise, pre-empt a lock so that it gets initialised as per documentation
    lock->unixHandle = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
    DqnLock_Acquire(lock);
    DqnLock_Release(lock);
    return true;
#endif

    return false;
}

void DqnLock_Acquire(DqnLock *lock)
{
    if (!lock) return;

#if defined(DQN__IS_WIN32)
    EnterCriticalSection(&lock->win32Handle);

#else
    // TODO(doyle): Better error handling
    i32 error = pthread_mutex_lock(&lock->unixHandle);
    DQN_ASSERT(error == 0);
#endif
}

void DqnLock_Release(DqnLock *lock)
{
    if (!lock) return;

#if defined(DQN__IS_WIN32)
    LeaveCriticalSection(&lock->win32Handle);

#else
    // TODO(doyle): better error handling
    i32 error = pthread_mutex_unlock(&lock->unixHandle);
    DQN_ASSERT(error == 0);
#endif
}

void DqnLock_Delete(DqnLock *lock)
{
    if (!lock) return;

#if defined(DQN__IS_WIN32)
    DeleteCriticalSection(&lock->win32Handle);
#else
    i32 error = pthread_mutex_destroy(&lock->unixHandle);
    DQN_ASSERT(error == 0);
#endif
}

bool DqnLock::Init()    { return DqnLock_Init   (this); }
void DqnLock::Acquire() {        DqnLock_Acquire(this); }
void DqnLock::Release() {        DqnLock_Release(this); }
void DqnLock::Delete()  {        DqnLock_Delete (this); }

DqnLockGuard DqnLock::LockGuard()
{
    return DqnLockGuard(this, nullptr);
}

DqnLockGuard::DqnLockGuard(DqnLock *lock_, bool *succeeded)
{
    if (lock_)
    {
        this->lock = lock_;
        this->lock->Acquire();
        if (succeeded) *succeeded = true;
    }
    else
    {
        if (succeeded) *succeeded = false;
    }
}

DqnLockGuard::~DqnLockGuard()
{
    if (this->lock) this->lock->Release();
}

// XPlatform > #DqnJobQueue
// =================================================================================================
typedef void *DqnThreadCallbackInternal(void *threadParam);
usize DQN_JOB_QUEUE_INTERNAL_THREAD_DEFAULT_STACK_SIZE = 0;

FILE_SCOPE u32 DqnJobQueueInternal_ThreadCreate(usize stackSize,
                                                DqnThreadCallbackInternal *threadCallback,
                                                void *threadParam, u32 numThreads)
{
    u32 numThreadsCreated = 0;

#if defined(DQN__IS_WIN32)
    DQN_ASSERT(stackSize == 0 || !threadCallback);
    for (u32 i = 0; i < numThreads; i++)
    {
        HANDLE handle = CreateThread(nullptr, stackSize, (LPTHREAD_START_ROUTINE)threadCallback,
                                     threadParam, 0, nullptr);
        CloseHandle(handle);
        numThreadsCreated++;
    }

#else
    // TODO(doyle): Better error handling
    pthread_attr_t attribute = {};
    DQN_ASSERT(pthread_attr_init(&attribute) == 0);

    // Allows us to use pthread_join() which lets us wait till a thread finishes execution
    DQN_ASSERT(pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE) == 0);

    // TODO(doyle): Persist thread handles
    for (u32 i = 0; i < numThreads; i++)
    {
        pthread_t thread = {};
        pthread_create(&thread, &attribute, threadCallback, threadParam);
        numThreadsCreated++;
    }

    DQN_ASSERT(pthread_attr_destroy(&attribute) == 0);
#endif

    DQN_ASSERT(numThreadsCreated == numThreads);
    return numThreadsCreated;
}


FILE_SCOPE void *DqnJobQueueInternal_ThreadCallback(void *threadParam)
{
    DqnJobQueue *queue = (DqnJobQueue *)threadParam;
    for (;;)
    {
        if (!DqnJobQueue_TryExecuteNextJob(queue))
        {
#if defined(DQN__IS_WIN32)
            WaitForSingleObjectEx(queue->semaphore, INFINITE, false);
#else
            sem_wait(&queue->semaphore);
#endif
        }
    }
}

FILE_SCOPE bool DqnJobQueueInternal_CreateSemaphore(DqnJobQueue *queue, u32 initSignalCount, u32 maxSignalCount)
{
    if (!queue) return false;

#if defined(DQN__IS_WIN32)
    queue->semaphore = (void *)CreateSemaphoreA(nullptr, initSignalCount, maxSignalCount, nullptr);
    DQN_ASSERT(queue->semaphore);

#else
    // TODO(doyle): Use max count for unix
    // TODO(doyle): Error handling
    const u32 UNIX_DONT_SHARE_BETWEEN_PROCESSES = 0;
    i32 error = sem_init(&queue->semaphore, UNIX_DONT_SHARE_BETWEEN_PROCESSES, 0);
    DQN_ASSERT(error == 0);

    for (u32 i = 0; i < initSignalCount; i++)
        DQN_ASSERT(sem_post(&queue->semaphore) == 0);
#endif

    return true;
}

FILE_SCOPE bool DqnJobQueueInternal_ReleaseSemaphore(DqnJobQueue *queue)
{
    DQN_ASSERT(queue);

#if defined(DQN__IS_WIN32)
    DQN_ASSERT(queue->semaphore);
    ReleaseSemaphore(queue->semaphore, 1, nullptr);

#else
    // TODO(doyle): Error handling
    DQN_ASSERT(sem_post(&queue->semaphore) == 0);
#endif

    return true;
}

DQN_FILE_SCOPE bool DqnJobQueue_Init(DqnJobQueue *queue, DqnJob *jobList, u32 jobListSize, u32 numThreads)
{
    if (!queue || !jobList || jobListSize == 0 || numThreads == 0) return false;
    queue->jobList = jobList;
    queue->size    = jobListSize;

    DQN_ASSERT(DqnJobQueueInternal_CreateSemaphore(queue, 0, numThreads));

    // Create threads
    u32 numThreadsCreated = DqnJobQueueInternal_ThreadCreate(
        DQN_JOB_QUEUE_INTERNAL_THREAD_DEFAULT_STACK_SIZE, DqnJobQueueInternal_ThreadCallback,
        (void *)queue, numThreads);
    DQN_ASSERT(numThreads == numThreadsCreated);

    return true;
}

DQN_FILE_SCOPE bool DqnJobQueue_AddJob(DqnJobQueue *queue, const DqnJob job)
{
    i32 newJobInsertIndex = (queue->jobInsertIndex + 1) % queue->size;
    if (newJobInsertIndex == queue->jobToExecuteIndex) return false;

    queue->jobList[queue->jobInsertIndex] = job;

    DqnAtomic_Add32(&queue->numJobsToComplete, 1);
    DQN_ASSERT(DqnJobQueueInternal_ReleaseSemaphore(queue));

    queue->jobInsertIndex = newJobInsertIndex;
    return true;
}

DQN_FILE_SCOPE void DqnJobQueue_BlockAndCompleteAllJobs(DqnJobQueue *queue)
{
    while (DqnJobQueue_TryExecuteNextJob(queue) || !DqnJobQueue_AllJobsComplete(queue))
        ;
}

DQN_FILE_SCOPE bool DqnJobQueue_TryExecuteNextJob(DqnJobQueue *queue)
{
    if (!queue) return false;

    i32 originalJobToExecute = queue->jobToExecuteIndex;
    if (originalJobToExecute != queue->jobInsertIndex)
    {
        i32 newJobIndexForNextThread = (originalJobToExecute + 1) % queue->size;
        i32 index = DqnAtomic_CompareSwap32(&queue->jobToExecuteIndex, newJobIndexForNextThread,
                                            originalJobToExecute);

        // NOTE: If we weren't successful at the interlock, another thread has
        // taken the work and we can't know if there's more work or not. So
        // irrespective of that result, return true to let the thread check
        // again for more work.
        if (index == originalJobToExecute)
        {
            DqnJob job = queue->jobList[index];
            job.callback(queue, job.userData);
            DqnAtomic_Add32(&queue->numJobsToComplete, -1);
        }

        return true;
    }

    return false;
}

DQN_FILE_SCOPE bool DqnJobQueue_AllJobsComplete(DqnJobQueue *queue)
{
    if (!queue) return false;

    bool result = (queue->numJobsToComplete == 0);
    return result;
}

bool DqnJobQueue::Init(DqnJob *jobList_, u32 jobListSize, u32 numThreads)
{
    bool result = DqnJobQueue_Init(this, jobList_, jobListSize, numThreads);
    return result;
}

bool DqnJobQueue::AddJob           (const DqnJob job) { return DqnJobQueue_AddJob(this, job);             }
void DqnJobQueue::BlockAndCompleteAllJobs()           {        DqnJobQueue_BlockAndCompleteAllJobs(this); }
bool DqnJobQueue::TryExecuteNextJob()                 { return DqnJobQueue_TryExecuteNextJob(this);       }
bool DqnJobQueue::AllJobsComplete  ()                 { return DqnJobQueue_AllJobsComplete(this);         }

// XPlatform > #DqnAtomic
// =================================================================================================

#if defined(DQN__IS_WIN32)
    DQN_COMPILE_ASSERT(sizeof(LONG) == sizeof(i32));
#endif

DQN_FILE_SCOPE i32 DqnAtomic_CompareSwap32(i32 volatile *dest, i32 swapVal, i32 compareVal)
{
    i32 result = 0;
#if defined(DQN__IS_WIN32)
    result = (i32)InterlockedCompareExchange((LONG volatile *)dest, (LONG)swapVal, (LONG)compareVal);

#else
    result = __sync_val_compare_and_swap(dest, compareVal, swapVal);
#endif
    return result;
}

DQN_FILE_SCOPE i32 DqnAtomic_Add32(i32 volatile *src, i32 value)
{
    i32 result = 0;
#if defined(DQN__IS_WIN32)
    result = (i32)InterlockedAdd((LONG volatile *)src, value);

#else
    result = __sync_add_and_fetch(src, value);
#endif

    return result;
}

// XPlatform > #DqnOS
// =================================================================================================
#define DQN_OS_GET_THREADS_AND_CORES(name) \
    DQN_FILE_SCOPE void name(u32 *const numCores, u32 *const numThreadsPerCore)

#if defined(DQN__IS_UNIX)
DQN_OS_GET_THREADS_AND_CORES(DqnOS_GetThreadsAndCores)
{
    if (!numThreadsPerCore && !numCores) return;

    // TODO(doyle): Not exactly standard

    usize fileSize = 0;
    if (u8 *readBuffer = DqnFile::ReadEntireFile("/proc/cpuinfo", &fileSize))
    {
        char const *srcPtr = reinterpret_cast<char *>(readBuffer);
        usize srcLen       = fileSize;

#define DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(ptr, len, offset)                                     \
    ptr += offset;                                                                                 \
    len -= offset

        if (numThreadsPerCore)
        {
            *numThreadsPerCore = 0;
            // Find the offset to the processor field and move to it
            DqnSlice<char const> processor = DQN_SLICE("processor");
            i32 processorOffset            = DqnStr_FindFirstOccurence(srcPtr, srcLen, processor.data, processor.len);

            DQN_ASSERT(processorOffset != -1);
            DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(srcPtr, srcLen, processorOffset);

            // Find the offset to the colon delimiter and advance to 1 after it
            i32 colonOffset = DqnStr_FindFirstOccurence(srcPtr, srcLen, ":", 1) + 1;
            DQN_ASSERT(colonOffset != -1);
            DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(srcPtr, srcLen, colonOffset);

            // Read num processors, i.e. logical cores/hyper threads
            *numThreadsPerCore = Dqn_StrToI64(srcPtr, srcLen);
            if (*numThreadsPerCore == 0) *numThreadsPerCore = 1;
        }

        if (numCores)
        {
            *numCores   = 0;
            // Find the offset to the cpu cores field and move to it
            DqnSlice<char const> cpuCores = DQN_SLICE("cpu cores");
            i32 cpuCoresOffset            = DqnStr_FindFirstOccurence(srcPtr, srcLen, cpuCores.data, cpuCores.len);
            DQN_ASSERT(cpuCoresOffset != -1);

            DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(srcPtr, srcLen, cpuCoresOffset);

            // Find the offset to the colon delimiter and advance to 1 after it
            i32 colonOffset = DqnStr_FindFirstOccurence(srcPtr, srcLen, ":", 1) + 1;
            DQN_ASSERT(colonOffset != -1);
            DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(srcPtr, srcLen, colonOffset);

            // Read num cores value, i.e. physical cores
            *numCores = Dqn_StrToI64(srcPtr, srcLen);
        }
        DQN_DEFAULT_HEAP_ALLOCATOR->Free(readBuffer);
    }
    else
    {
        // TODO(doyle): Out of mem
        DQN_ASSERT(DQN_INVALID_CODE_PATH);
    }
}
#endif // DQN__IS_UNIX

#if defined(DQN__IS_WIN32)
DQN_OS_GET_THREADS_AND_CORES(DqnOS_GetThreadsAndCores)
{
    if (numThreadsPerCore)
    {
        SYSTEM_INFO systemInfo;
        GetNativeSystemInfo(&systemInfo);
        *numThreadsPerCore = systemInfo.dwNumberOfProcessors;
    }

    if (numCores)
    {
        *numCores = 0;
        DWORD requiredSize    = 0;
        u8 insufficientBuffer = {0};
        GetLogicalProcessorInformationEx(
            RelationProcessorCore, (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)insufficientBuffer,
            &requiredSize);

        auto *rawProcInfoArray = (u8 *)DqnMem_Calloc(requiredSize);
        if (!rawProcInfoArray)
        {
            DQN_LOGE("Could not allocate memory for array required: %$d\n");
            return;
        }

        if (GetLogicalProcessorInformationEx(
                RelationProcessorCore, (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)rawProcInfoArray,
                &requiredSize))
        {
            SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *logicalProcInfo =
                (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)rawProcInfoArray;
            DWORD bytesRead = 0;

            do
            {
                // NOTE: High efficiency value has greater performance and less efficiency.
                PROCESSOR_RELATIONSHIP *procInfo = &logicalProcInfo->Processor;
                // u32 efficiency                   = procInfo->EfficiencyClass;
                (*numCores)++;
                DQN_ASSERT(logicalProcInfo->Relationship == RelationProcessorCore);
                DQN_ASSERT(procInfo->GroupCount == 1);

                bytesRead += logicalProcInfo->Size;
                logicalProcInfo =
                    (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)((u8 *)logicalProcInfo +
                                                                logicalProcInfo->Size);
            } while (bytesRead < requiredSize);
        }
        else
        {
            DqnWin32_DisplayLastError("GetLogicalProcessorInformationEx() failed");
        }

        DqnMem_Free(rawProcInfoArray);
    }
}
#endif // DQN__IS_WIN32

#ifdef DQN__IS_WIN32
// #DqnWin32
// =================================================================================================
DQN_FILE_SCOPE i32 DqnWin32_UTF8ToWChar(char const *in, wchar_t *out, i32 outLen)
{
    i32 result = MultiByteToWideChar(CP_UTF8, 0, in, -1, out, outLen);

    if (result == 0xFFFD || 0)
    {
        DQN__WIN32_ERROR_BOX("WideCharToMultiByte() failed.", nullptr);
        return -1;
    }

    return result;
}

DQN_FILE_SCOPE i32 DqnWin32_WCharToUTF8(wchar_t const *in, char *out, i32 outLen)
{
    i32 result =
        WideCharToMultiByte(CP_UTF8, 0, in, -1, out, outLen, nullptr, nullptr);

    if (result == 0xFFFD || 0)
    {
        DQN__WIN32_ERROR_BOX("WideCharToMultiByte() failed.", nullptr);
        return -1;
    }

    return result;
}

DQN_FILE_SCOPE void DqnWin32_GetClientDim(HWND window, LONG *width, LONG *height)
{
    RECT rect;
    GetClientRect(window, &rect);
    if (width)  *width  = rect.right - rect.left;
    if (height) *height = rect.bottom - rect.top;
}

DQN_FILE_SCOPE void DqnWin32_GetRectDim(RECT const rect, LONG *width, LONG *height)
{
    if (width)  *width  = rect.right - rect.left;
    if (height) *height = rect.bottom - rect.top;
}

DQN_FILE_SCOPE void DqnWin32_DisplayLastError(char const *errorPrefix)
{
    DWORD error         = GetLastError();
    char errorMsg[1024] = {0};
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr, error, 0, errorMsg, DQN_ARRAY_COUNT(errorMsg), nullptr);

    if (errorPrefix)
    {
        char formattedError[2048] = {0};
        Dqn_sprintf(formattedError, "%s: %s", errorPrefix, errorMsg);
        DQN__WIN32_ERROR_BOX(formattedError, nullptr);
    }
    else
    {
        DQN__WIN32_ERROR_BOX(errorMsg, nullptr);
    }
}

const i32 DQN__WIN32_INTERNAL_ERROR_MSG_SIZE = 2048;
DQN_FILE_SCOPE void DqnWin32_DisplayErrorCode(DWORD error, char const *errorPrefix)
{
    char errorMsg[DQN__WIN32_INTERNAL_ERROR_MSG_SIZE] = {0};
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr, error, 0, errorMsg, DQN_ARRAY_COUNT(errorMsg), nullptr);

    char formattedError[2048] = {0};
    Dqn_sprintf(formattedError, "%s: %s", errorPrefix, errorMsg);
    DQN__WIN32_ERROR_BOX(formattedError, nullptr);
}

DQN_FILE_SCOPE void DqnWin32_OutputDebugString(char const *formatStr, ...)
{
    char str[DQN__WIN32_INTERNAL_ERROR_MSG_SIZE] = {0};

    va_list argList;
    va_start(argList, formatStr);
    {
        i32 numCopied = Dqn_vsprintf(str, formatStr, argList);
        DQN_ASSERT(numCopied < DQN_ARRAY_COUNT(str));
    }
    va_end(argList);

    OutputDebugStringA(str);
}

DQN_FILE_SCOPE i32 DqnWin32_GetEXEDirectory(char *buf, u32 bufLen)
{
    if (!buf || bufLen == 0) return -1;
    u32 copiedLen = GetModuleFileNameA(nullptr, buf, bufLen);
    if (copiedLen == bufLen) return -1;

    // NOTE: Should always work if GetModuleFileName works and we're running an
    // executable.
    i32 lastSlashIndex = 0;
    for (i32 i = copiedLen; i > 0; i--)
    {
        if (buf[i] == '\\')
        {
            lastSlashIndex = i;
            break;
        }
    }

    return lastSlashIndex;
}
#endif // DQN__IS_WIN32
#endif // DQN__XPLATFORM_LAYER
