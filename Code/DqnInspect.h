#ifndef DQN_INSPECT_H
#define DQN_INSPECT_H

#define _CRT_SECURE_NO_WARNINGS // NOTE: Undefined at end of file
#include <stddef.h>

//
// DqnInspect.h - Minimal Inspection System, Single Header, CRT Dependencies Only
//                Public Domain or MIT License (see bottom of file)
// =================================================================================================

//
// HOW TO BUILD AND INSPECT ANNOTATED CODE
// =================================================================================================
// Define in the preprocessor, DQN_INSPECT_EXECUTABLE_IMPLEMENTATION and compile
// Dqn_Inspect.h to produce the metaprogram binary. Run the binary on the
// desired C++ source files. The inspected file will be printed out to stdout.
//
// DqnInspect.exe YourSourceCode.cpp > YourSourceCode_Inspected.cpp

//
// HOW TO ANNOTATE CODE
// =================================================================================================
// This header only file contains all the necessary definitions for inspected
// members and should be included in all files containing information you wish
// to inspect. You may inspect C-like POD data structures, such as in the following
// example.
//

//
// INSPECT USAGE
// =================================================================================================

/*
    InspectedStruct foo = {};
    InspectedEnum   bar = InspectedEnum::HelloWorld;

    printf("%s\n", DqnInspect_EnumString(bar)); // Prints "HelloWorld"

    // Print out each member of the struct
    DqnInspect_Struct const *inspector = DqnReflect_GetStruct(&foo);
    for (int i = 0; i < inspector->members_len; ++i)
        printf("%s\n", inspect_struct->members[i].name);
*/

//
// ANNOTATION EXAMPLE
// =================================================================================================
// See Data/DqnInspect_TestData.h

// TODO(doyle): Make the enum system similar to struct members for consistency?
// TODO(doyle): Handle namespaces .. maybe.

#define DQN_INSPECT
#define DQN_INSPECT_META(...)
#define DQN_INSPECT_FUNCTION(...)

struct DqnInspectMetadata
{
    enum struct DqnInspectDeclType decl_type;
    enum struct DqnInspectMetaType meta_type;
    void const                    *data;
};

struct DqnInspectMember
{
    enum struct DqnInspectMemberType name_type;
    char const *                     name;
    int                              name_len;
    int                              pod_struct_offset;

    // Includes the template expression if there is any in declaration
    enum struct DqnInspectDeclType   decl_type;
    char const *                     decl_type_str;
    int                              decl_type_len;
    int                              decl_type_sizeof;

    // If there's a template this will be the topmost of the declaration i.e. for vector<int>
    // this will just be DqnInspectDeclType::vector_
    enum struct DqnInspectDeclType   template_parent_decl_type;

    enum struct DqnInspectDeclType   template_child_decl_type;
    char const *                     template_child_expr;
    int                              template_child_expr_len;

    int                              array_dimensions; // > 0 means array

    // NOTE: Supports maximum of 8 dimensions. 0 if unknown (i.e. just a pointer)
    int                              array_compile_time_size[8];

    DqnInspectMetadata const        *metadata;
    int                              metadata_len;
};

struct DqnInspectStruct
{
    char const             *name;
    int                     name_len;
    DqnInspectMember const *members;
    int                     members_len;
};

inline DqnInspectMetadata const *DqnInspectMember_FindMetadata(DqnInspectMember const *member,
                                                               enum struct DqnInspectMetaType meta_type,
                                                               enum struct DqnInspectDeclType *decl_types = nullptr,
                                                               int num_decl_types                         = 0)
{
    for (size_t metadata_index = 0;
         metadata_index < member->metadata_len;
         ++metadata_index)
    {
        DqnInspectMetadata const *metadata = member->metadata + metadata_index;
        if (metadata->meta_type == meta_type)
        {
            if (!decl_types || num_decl_types == 0)
                return metadata;

            for (size_t decl_type_index = 0; decl_type_index < num_decl_types; ++decl_type_index)
            {
                DqnInspectDeclType const *decl_type = decl_types + decl_type_index;
                if (metadata->decl_type == *decl_type)
                {
                    return metadata;
                }
            }
        }
    }

    return nullptr;
}

inline DqnInspectMetadata const *DqnInspectMember_FindMetadata(DqnInspectMember const *member,
                                                               enum struct DqnInspectMetaType meta_type,
                                                               enum struct DqnInspectDeclType decl_type)
{
    DqnInspectMetadata const *result = DqnInspectMember_FindMetadata(member, meta_type, &decl_type, 1);
    return result;
}

// NOTE(doyle): For compiler testing
// #include "../Data/DqnInspect_TestData.h"
// #include "../Data/DqnInspect_TestDataGenerated.cpp"

#ifdef DQN_INSPECT_EXECUTABLE_IMPLEMENTATION

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <vector>
#include <set>

static const int SPACES_PER_INDENT = 4;

using usize = size_t;
using isize = ptrdiff_t;
using i64   = int64_t;
using i32   = int32_t;
using b32   = int32_t;
using u8    = uint8_t;

template <typename Proc>
struct Defer
{
    Proc proc;
    Defer(Proc p) : proc(p) {}
    ~Defer()                { proc(); }
};

struct DeferHelper
{
    template <typename Lambda>
    Defer<Lambda> operator+(Lambda lambda) { return Defer<Lambda>(lambda); }
};

template <typename T, unsigned short N>
unsigned short ArrayCount(T (&)[N]) { return N; }

template <typename T, unsigned short N>
unsigned short CharCount(T (&)[N]) { return N - 1; }

#define INSPECT_MAX(a, b) ((a) > (b)) ? (a) : (b)
#define INSPECT_MIN(a, b) ((a) < (b)) ? (a) : (b)
#define TOKEN_COMBINE(x, y) x ## y
#define TOKEN_COMBINE2(x, y) TOKEN_COMBINE(x, y)
#define DEFER auto const TOKEN_COMBINE(defer_lambda_, __COUNTER__) = DeferHelper() + [&]()

#define KILOBYTE(val) (1024ULL * (val))
#define MEGABYTE(val) (1024ULL * KILOBYTE(val))

#define SLICE_LITERAL(str) Slice<char const>{str, CharCount(str)}
template <typename T>
struct Slice
{
    Slice() = default;
    Slice(T *str, int len) : str(str) , len(len) { }
    T    *str;
    int   len;

    bool operator<(Slice const &rhs) const
    {
        if (this->len == rhs.len)
            return (memcmp(this->str, rhs.str, this->len) < 0);
        return this->len < rhs.len;
    }
};

template <typename T>
b32 Slice_Cmp(T const a, T const b)
{
    if (a.len != b.len)
        return false;
    b32 result = (memcmp(a.str, b.str, a.len) == 0);
    return result;
}

template <typename T>
struct LinkedList
{
    T           value;
    LinkedList *next;
};

//
// Memory Utilities
//

struct MemArena
{
    void *memory;
    usize size;
    usize used;
};

struct MemArenaScopedRegion
{
    MemArenaScopedRegion (MemArena *arena_, usize used_) : arena(arena_), used(used_) {}
    ~MemArenaScopedRegion()                                                           { arena->used = used; }
    MemArena *arena;
    usize     used;
};

static MemArena global_main_arena;

#define MEM_ARENA_ALLOC_ARRAY(arena, Type, num) static_cast<Type *>(MemArena_Alloc(arena, sizeof(Type) * num))
#define MEM_ARENA_ALLOC_STRUCT(arena, Type)     static_cast<Type *>(MemArena_Alloc(arena, sizeof(Type)))
MemArena MemArena_Init(void *memory, usize size)
{
    MemArena result = {};
    result.memory   = memory;
    result.size     = size;
    result.used     = 0;
    return result;
}

MemArenaScopedRegion MemArena_MakeScopedRegion(MemArena *arena)
{
    return MemArenaScopedRegion(arena, arena->used);
}

void *MemArena_Alloc(MemArena *arena, usize size)
{
    assert(arena->used + size <= arena->size);
    void *result = static_cast<uint8_t *>(arena->memory) + arena->used;
    arena->used += size;
    return result;
}

template <typename T> void EraseStableFromCArray(T *array, isize len, isize max, isize index);
#define FIXED_ARRAY_TEMPLATE_DECL template <typename T, int MAX_>
FIXED_ARRAY_TEMPLATE_DECL struct FixedArray
{
    FixedArray() = default;
    T        data[MAX_];
    isize    len;
    isize    Max()                const   { return MAX_; }
    T const &operator[]   (isize i) const { assert(i >= 0 && i < len); return  data[i]; }
    T       &operator[]   (isize i)       { assert(i >= 0 && i < len); return  data[i]; }
    T const *begin        ()        const { return data; }
    T const *end          ()        const { return data + len; }
    T       *begin        ()              { return data; }
    T       *end          ()              { return data + len; }
};
FIXED_ARRAY_TEMPLATE_DECL FixedArray<T, MAX_>  FixedArray_Init         (T const *item, int num)                          { FixedArray<T, MAX_> result = {}; FixedArray_Add(&result, item, num); return result; }
FIXED_ARRAY_TEMPLATE_DECL T                   *FixedArray_Add          (FixedArray<T, MAX_> *a, T const *items, int num) { assert(a->len + num <= MAX_); T *result = static_cast<T *>(MemCopy(a->data + a->len, items, sizeof(T) * num)); a->len += num; return result; }
FIXED_ARRAY_TEMPLATE_DECL T                   *FixedArray_Add          (FixedArray<T, MAX_> *a, T const item)            { assert(a->len < MAX_);        a->data[a->len++] = item; return &a->data[a->len - 1]; }
FIXED_ARRAY_TEMPLATE_DECL T                   *FixedArray_Make         (FixedArray<T, MAX_> *a, int num)                 { assert(a->len + num <= MAX_); T *result = a->data + a->len; a->len += num; return result;}
FIXED_ARRAY_TEMPLATE_DECL void                 FixedArray_Clear        (FixedArray<T, MAX_> *a)                          { a->len = 0; }
FIXED_ARRAY_TEMPLATE_DECL void                 FixedArray_EraseStable  (FixedArray<T, MAX_> *a, isize index) { EraseStableFromCArray<T>(a->data, a->len--, a.Max(), index); }
FIXED_ARRAY_TEMPLATE_DECL void                 FixedArray_EraseUnstable(FixedArray<T, MAX_> *a, isize index)             { assert(index >= 0 && index < a->len); if (--a->len == 0) return; a->data[index] = a->data[a->len]; }

//
// String Utilities
//

b32 CharIsAlpha     (char ch) { return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'); }
b32 CharIsDigit     (char ch) { return (ch >= '0' && ch <= '9'); }
b32 CharIsAlphaNum  (char ch) { return CharIsAlpha(ch) || CharIsDigit(ch); }
b32 CharIsWhitespace(char ch) { return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'); }

char const *StrSkipToChar(char const *src, char ch)
{
  char const *result = src;
  while (result && result[0] && result[0] != ch) ++result;
  return result;
}

char const *StrSkipToNextAlphaNum(char const *src)
{
  char const *result = src;
  while (result && result[0] && !CharIsAlphaNum(result[0])) ++result;
  return result;
}

char const *StrSkipToNextDigit(char const *src)
{
  char const *result = src;
  while (result && result[0] && !CharIsDigit(result[0])) ++result;
  return result;
}

char const *StrSkipToNextChar(char const *src)
{
  char const *result = src;
  while (result && result[0] && !CharIsAlpha(result[0])) ++result;
  return result;
}

char const *StrSkipToNextWord(char const *src)
{
  char const *result = src;
  while (result && result[0] && !CharIsWhitespace(result[0])) ++result;
  while (result && result[0] && CharIsWhitespace(result[0])) ++result;
  return result;
}

char const *StrSkipToNextWhitespace(char const *src)
{
  char const *result = src;
  while (result && result[0] && !CharIsWhitespace(result[0])) ++result;
  return result;
}

char const *StrSkipWhitespace(char const *src)
{
  char const *result = src;
  while (result && result[0] && CharIsWhitespace(result[0])) ++result;
  return result;
}

char const *StrSkipToNextWordInplace(char const **src) { *src = StrSkipToNextWord(*src); return *src; }
char const *StrSkipWhitespaceInplace(char const **src) { *src = StrSkipWhitespace(*src); return *src; }

char *StrFind(Slice<char> src, Slice<char const> find)
{
  char *buf_ptr = src.str;
  char *buf_end = buf_ptr + src.len;
  char *result  = nullptr;

  for (;*buf_ptr; ++buf_ptr)
  {
    int len_remaining = static_cast<int>(buf_end - buf_ptr);
    if (len_remaining < find.len) break;

    if (strncmp(buf_ptr, find.str, find.len) == 0)
    {
      result = buf_ptr;
      break;
    }
  }
  return result;
}

Slice<char> RemoveConsecutiveSpaces(Slice<char> input)
{
    Slice<char> result = input;
    for (size_t i = 0; i < (result.len - 1); ++i)
    {
        char curr = result.str[i];
        char next = result.str[i + 1];
        if (curr == ' ' && next == ' ')
        {
            char const *end    = result.str + result.len;
            char const *str    = (result.str + (i + 1));
            int bytes_to_shift = static_cast<int>(end - str);
            memcpy(result.str + i, str, bytes_to_shift);
            --result.len;
            --i;
        }
    }

    return result;
}

Slice<char> TrimSpaceAround(Slice<char> const input)
{
    Slice<char> result = input;
    while (result.str[0] == ' ' && result.len > 0)
    {
        ++result.str;
        --result.len;
    }

    while (result.len > 0 && result.str[result.len - 1] == ' ')
        --result.len;

    return result;
}

Slice<char> Asprintf(MemArena *arena, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Slice<char> result         = {};
    result.len                 = vsnprintf(nullptr, 0, fmt, va);
    result.str                 = MEM_ARENA_ALLOC_ARRAY(arena, char, result.len + 1);
    vsnprintf(result.str, result.len + 1, fmt, va);
    result.str[result.len] = 0;
    va_end(va);
    return result;
}

//
// CPP Tokenisers/Tokens
//

#define CPP_TOKEN_TYPE_X_MACRO \
    X(EndOfStream, "End Of Stream") \
    X(LeftBrace, "{") \
    X(RightBrace, "}") \
    X(LeftSqBracket, "[") \
    X(RightSqBracket, "]") \
    X(OpenParen, "(") \
    X(CloseParen, ")") \
    X(Comma, ",") \
    X(Colon, ":") \
    X(FwdSlash, "/") \
    X(Comment, "comment") \
    X(LessThan, "<") \
    X(GreaterThan, ">") \
    X(Equals, "=") \
    X(String, "\"") \
    X(SemiColon, ";") \
    X(Identifier, "Identifier") \
    X(Number, "[0-9]") \
    X(Asterisks, "*") \
    X(Ampersand, "&") \
    X(VarArgs, "Variable Args (...)") \
    X(InspectCode, "DQN_INSPECT") \
    X(InspectFunction, "DQN_INSPECT_FUNCTION") \
    X(Hash, "#")

#define X(decl, str) decl,
enum struct CPPTokenType
{
    CPP_TOKEN_TYPE_X_MACRO
};
#undef X

#define X(decl, str) str,
char const *CPPTokenType_Strings[]
{
    CPP_TOKEN_TYPE_X_MACRO
};
#undef X

struct CPPToken
{
    CPPTokenType type;
    char *str;
    int   len;
};

#define ARRAY_COUNT(array) sizeof(array)/sizeof(array[0])
struct CPPVariableDecl
{
    b32         type_has_const;
    Slice<char> type;
    Slice<char> name;
    Slice<char> template_child_expr;
    b32         array_has_const;
    int         array_dimensions;
    b32         array_dimensions_has_compile_time_size[8]; // TODO(doyle): Max of 8 dimensions, whotf needs more?
    Slice<char> default_value;
};

static_assert(ARRAY_COUNT(((DqnInspectMember *)0)->array_compile_time_size) == ARRAY_COUNT(((CPPVariableDecl *)0)->array_dimensions_has_compile_time_size),
              "Sizes must match otherwise overflow on writing our Inspection data");

template <typename T>
struct CPPDeclLinkedList
{
    CPPDeclLinkedList<CPPVariableDecl> *metadata_list;
    T                                   value;
    CPPDeclLinkedList                  *next;
};

struct CPPTokeniser
{
    CPPToken  *tokens;
    int        tokens_index;
    int        tokens_len;
    int        tokens_max;

    int        indent_level;
};

void FprintfIndented(FILE *output_file, int indent_level, char const *fmt, ...)
{
    int const num_spaces = SPACES_PER_INDENT * indent_level;
    fprintf(output_file, "%*s", num_spaces, "");

    va_list va;
    va_start(va, fmt);
    vfprintf(output_file, fmt, va);
    va_end(va);
}

CPPToken CPPTokeniser_NextToken(CPPTokeniser *tokeniser, int amount = 1)
{
    CPPToken result = tokeniser->tokens[tokeniser->tokens_index];
    if (result.type != CPPTokenType::EndOfStream)
    {
        for (int i = 0; i < amount; i++)
        {
            result = tokeniser->tokens[tokeniser->tokens_index++];
            if (result.type      == CPPTokenType::LeftBrace)  tokeniser->indent_level++;
            else if (result.type == CPPTokenType::RightBrace) tokeniser->indent_level--;
            assert(tokeniser->indent_level >= 0);

            if (result.type == CPPTokenType::EndOfStream)
                break;
        }
    }
    return result;
}

CPPToken CPPTokeniser_PeekToken(CPPTokeniser *tokeniser)
{
    CPPToken result = tokeniser->tokens[tokeniser->tokens_index];
    return result;
}

void CPPTokeniser_RewindToken(CPPTokeniser *tokeniser)
{
    tokeniser->tokens[--tokeniser->tokens_index];
}

CPPToken CPPTokeniser_PrevToken(CPPTokeniser *tokeniser)
{
    CPPToken result = {};
    result.type = CPPTokenType::EndOfStream;
    if (tokeniser->tokens_index > 0)
        result = tokeniser->tokens[tokeniser->tokens_index - 1];
    return result;
}

CPPToken *CPPTokeniser_MakeToken(CPPTokeniser *tokeniser)
{
    assert(tokeniser->tokens_len < tokeniser->tokens_max);
    CPPToken *result = tokeniser->tokens + tokeniser->tokens_len++;
    return result;
}

bool CPPTokeniser_AcceptTokenIfType(CPPTokeniser *tokeniser, CPPTokenType type, CPPToken *token)
{
    CPPToken check = CPPTokeniser_PeekToken(tokeniser);
    bool result    = (check.type == type);
    if (result)
    {
        CPPTokeniser_NextToken(tokeniser);
        if (token) *token = check;
    }

    return result;
}

//
// CPP Parsing Helpers
//

b32 IsIdentifierToken(CPPToken token, Slice<char const> expect_str)
{
    b32 result = (token.type == CPPTokenType::Identifier) && (Slice_Cmp(Slice<char const>(token.str, token.len), expect_str));
    return result;
}

b32 ExpectToken(CPPToken token, CPPTokenType type)
{
    b32 result = token.type == type;
    if (!result)
    {
        fprintf(stdout, "Expected token type: %s received: %.*s\n", CPPTokenType_Strings[(int)type], token.len, token.str);
        fprintf(stdout, "Context: %.*s\n\n", (token.len > 100) ? 100 : token.len, token.str);
    }
    return result;
}

//
// CPP Parsing Functions
//

#if 0
DQN_INSPECT enum struct EnumWithMetadata
{
    Rect   DQN_INSPECT_META(char const *FilePath = "Rect.vert", V3 Coords = V3(1, 2, 3)),
    Square DQN_INSPECT_META(char *file_path = "Rect.vert"),
    Count,
};
#endif

int ConsumeToken(CPPTokeniser *tokeniser, CPPTokenType type, CPPToken *token = nullptr)
{
    int result = 0;
    CPPToken last_token = {};
    for (CPPToken peek_token = CPPTokeniser_PeekToken(tokeniser);
         peek_token.type == type;
         ++result)
    {
        last_token = peek_token;
        CPPTokeniser_NextToken(tokeniser);
        peek_token = CPPTokeniser_PeekToken(tokeniser);
    }

    if (token && last_token.type != CPPTokenType::EndOfStream) *token = last_token;
    return result;
}

int ConsumeAsterisks(CPPTokeniser *tokeniser, CPPToken *token = nullptr)
{
    int result = ConsumeToken(tokeniser, CPPTokenType::Asterisks, token);
    return result;
}

b32 ConsumeConstIdentifier(CPPTokeniser *tokeniser, CPPToken *token = nullptr)
{
    b32 result = false;
    CPPToken last_const_token = {};
    for (CPPToken peek_token = CPPTokeniser_PeekToken(tokeniser);
         IsIdentifierToken(peek_token, SLICE_LITERAL("const"));
         peek_token = CPPTokeniser_PeekToken(tokeniser))
    {
        last_const_token = peek_token;
        result = true;
        CPPTokeniser_NextToken(tokeniser);
    }

    if (token && last_const_token.type != CPPTokenType::EndOfStream) *token = last_const_token;
    return result;
}

void SkipFunctionParam(CPPTokeniser *tokeniser)
{
    CPPToken token = CPPTokeniser_PeekToken(tokeniser);
    while (token.type != CPPTokenType::EndOfStream)
    {
        if (token.type == CPPTokenType::OpenParen)
        {
            for (int level = 1; level != 0;)
            {
                CPPTokeniser_NextToken(tokeniser);
                token = CPPTokeniser_PeekToken(tokeniser);
                if (token.type == CPPTokenType::OpenParen) level++;
                if (token.type == CPPTokenType::CloseParen) level--;
            }

            CPPTokeniser_NextToken(tokeniser);
            token = CPPTokeniser_PeekToken(tokeniser);
        }

        if (token.type == CPPTokenType::LeftBrace)
        {
            for (int level = 1; level != 0;)
            {
                CPPTokeniser_NextToken(tokeniser);
                token = CPPTokeniser_PeekToken(tokeniser);
                if (token.type == CPPTokenType::LeftBrace) level++;
                if (token.type == CPPTokenType::RightBrace) level--;
            }
            CPPTokeniser_NextToken(tokeniser);
            token = CPPTokeniser_PeekToken(tokeniser);
        }

        if (token.type == CPPTokenType::CloseParen)
            break;

        if (token.type == CPPTokenType::Comma)
            break;

        CPPTokeniser_NextToken(tokeniser);
        token = CPPTokeniser_PeekToken(tokeniser);
    }
}

CPPDeclLinkedList<CPPVariableDecl> *ParseCPPInspectMeta(CPPTokeniser *tokeniser);
// NOTE(doyle): Doesn't parse the ending semicolon so we can reuse this function for parsing function arguments
CPPDeclLinkedList<CPPVariableDecl> *ParseCPPTypeAndVariableDecl(CPPTokeniser *tokeniser, b32 parse_function_param)
{
    CPPDeclLinkedList<CPPVariableDecl> *result        = nullptr;
    CPPDeclLinkedList<CPPVariableDecl> *link_iterator = nullptr;

    b32 type_has_const = ConsumeConstIdentifier(tokeniser);
    CPPToken token     = CPPTokeniser_NextToken(tokeniser);
    if (token.type != CPPTokenType::Identifier && token.type != CPPTokenType::VarArgs)
        return result;

    CPPToken variable_type = token;
    for (int total_asterisks_count = 0;;)
    {
        Slice<char> variable_template_child_expr = {};
        if (CPPTokeniser_AcceptTokenIfType(tokeniser, CPPTokenType::LessThan, &token))
        {
            CPPToken template_start_token = token;
            int template_depth = 1;
            while (template_depth != 0 && token.type != CPPTokenType::EndOfStream)
            {
                token = CPPTokeniser_NextToken(tokeniser);
                if (token.type == CPPTokenType::LessThan)
                    template_depth++;
                else if (token.type == CPPTokenType::GreaterThan)
                    template_depth--;
            }

            if (template_depth == 0)
            {
                char *expr_start = template_start_token.str + 1;
                char *expr_end   = token.str;
                int expr_len     = static_cast<int>(expr_end - expr_start);

                variable_template_child_expr.str = expr_start;
                variable_template_child_expr.len = expr_len;
            }

            for (int ch_index = 0; ch_index < variable_template_child_expr.len; ++ch_index)
            {
                if (variable_template_child_expr.str[ch_index] == ',')
                    variable_template_child_expr.str[ch_index] = '_';
            }
        }

        CPPToken last_modifier_token = {};
        ConsumeToken(tokeniser, CPPTokenType::Ampersand, &last_modifier_token);
        total_asterisks_count        = ConsumeAsterisks(tokeniser, &last_modifier_token);
        b32 array_has_const          = false;
        if (ConsumeConstIdentifier(tokeniser, &last_modifier_token))
        {
            if (type_has_const)
                array_has_const = true; // Then this const is applying on the pointer type
            else
                type_has_const = true;

            total_asterisks_count += ConsumeAsterisks(tokeniser, &last_modifier_token);
            ConsumeConstIdentifier(tokeniser, &last_modifier_token);
        }

        ConsumeToken(tokeniser, CPPTokenType::Ampersand, &last_modifier_token);

        CPPToken variable_name = {};
        if (variable_type.type == CPPTokenType::Identifier)
        {
            token    = CPPTokeniser_PeekToken(tokeniser);
            variable_name = token;
            if (variable_name.type != CPPTokenType::Identifier)
                break;

            CPPTokeniser_NextToken(tokeniser);
        }

        // Allocate A Member Declaration
        auto *link = MEM_ARENA_ALLOC_STRUCT(&global_main_arena, CPPDeclLinkedList<CPPVariableDecl>);
        *link      = {};
        if (!result) result              = link; // Set result to first linked list entry
        else         link_iterator->next = link;
        auto *prev_link = link_iterator;
        link_iterator   = link;

        Slice<char> variable_type_str_lit = {variable_type.str, variable_type.len};
        if (last_modifier_token.type != CPPTokenType::EndOfStream)
        {
            if (!prev_link)
            {
                variable_type_str_lit.len =
                    static_cast<int>((last_modifier_token.str + (last_modifier_token.len - 1)) - variable_type.str) + 1;
            }
            else
            {
                CPPTokeniser copy_tokeniser = *tokeniser;
                CPPTokeniser_RewindToken(&copy_tokeniser);
                CPPToken rewind_token = CPPTokeniser_PeekToken(&copy_tokeniser);
                while (rewind_token.type != CPPTokenType::Comma)
                {
                    CPPTokeniser_RewindToken(&copy_tokeniser);
                    rewind_token = CPPTokeniser_PeekToken(&copy_tokeniser);
                }

                Slice<char> comma_to_var_name = {};
                comma_to_var_name.str         = rewind_token.str + 1;
                comma_to_var_name.len         = static_cast<int>(variable_name.str - comma_to_var_name.str);
                comma_to_var_name             = TrimSpaceAround(comma_to_var_name);

                if (parse_function_param)
                {
                    variable_type_str_lit = Asprintf(&global_main_arena,
                                                     "%.*s",
                                                     comma_to_var_name.len,
                                                     comma_to_var_name.str);
                }
                else
                {
                    // If not function param, we're parsing something of the likes
                    // int var, const *var1

                    // In which case you want to pull the base variable type,
                    // crrently stored in variable_type and then you want to
                    // pull in everything from the comma to var1, to grab the
                    // pointer modifiers on the type.
                    variable_type_str_lit = Asprintf(&global_main_arena,
                                                     "%.*s %.*s",
                                                     variable_type.len,
                                                     variable_type.str,
                                                     comma_to_var_name.len,
                                                     comma_to_var_name.str);
                }
            }
        }
        variable_type_str_lit = TrimSpaceAround(variable_type_str_lit);
        variable_type_str_lit = RemoveConsecutiveSpaces(variable_type_str_lit);

        link->value.type_has_const   = type_has_const;
        link->value.type             = variable_type_str_lit;
        link->value.name             = Slice<char>(variable_name.str, variable_name.len);
        link->value.template_child_expr    = variable_template_child_expr;
        link->value.array_has_const  = array_has_const;
        link->value.array_dimensions = total_asterisks_count;

        for (token = CPPTokeniser_PeekToken(tokeniser);
             token.type == CPPTokenType::LeftSqBracket && token.type != CPPTokenType::EndOfStream;
             token = CPPTokeniser_NextToken(tokeniser))
        {
            // TODO(doyle): Parsing array size is difficult if it's an expression, so maybe don't do it at all
            int token_count_after_left_sq_bracket = 0;
            while (token.type != CPPTokenType::RightSqBracket && token.type != CPPTokenType::EndOfStream)
            {
                token_count_after_left_sq_bracket++;
                token = CPPTokeniser_NextToken(tokeniser);
            }

            if (token_count_after_left_sq_bracket > 0)
            {
                // NOTE(doyle): There is some sort of contents in the bracket, so, this array has a compile time size
                link->value.array_dimensions_has_compile_time_size[link->value.array_dimensions] = true;
            }
            ++link->value.array_dimensions;
        }
        assert(link->value.array_dimensions < ArrayCount(link->value.array_dimensions_has_compile_time_size));

        if (IsIdentifierToken(token, SLICE_LITERAL("DQN_INSPECT_META")))
        {
            link->metadata_list = ParseCPPInspectMeta(tokeniser);
            token               = CPPTokeniser_PeekToken(tokeniser);
        }

        if (token.type == CPPTokenType::Equals)
        {
            token                     = CPPTokeniser_NextToken(tokeniser);
            token                     = CPPTokeniser_PeekToken(tokeniser);
            char *default_value_start = token.str;

            if (token.type == CPPTokenType::String) // NOTE(doyle): Include the quotes in the param value
                default_value_start--;

            SkipFunctionParam(tokeniser);
            token = CPPTokeniser_PeekToken(tokeniser);
            if (token.type != CPPTokenType::Comma && token.type != CPPTokenType::CloseParen)
                continue;

            char *default_value_end = token.str;
            link->value.default_value = Slice<char>(default_value_start, static_cast<int>(default_value_end - default_value_start));
        }

        if (token.type == CPPTokenType::Comma)
        {
            if (parse_function_param)
            {
                CPPTokeniser_NextToken(tokeniser);
                variable_type = CPPTokeniser_PeekToken(tokeniser);
            }
            token = CPPTokeniser_NextToken(tokeniser);
        }
        else
        {
            break;
        }
    }

    return result;
}

CPPDeclLinkedList<CPPVariableDecl> *ParseCPPInspectMeta(CPPTokeniser *tokeniser)
{
    CPPToken token = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::Identifier) || !IsIdentifierToken(token, SLICE_LITERAL("DQN_INSPECT_META")))
        return nullptr;

    token = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::OpenParen)) return nullptr;

    CPPDeclLinkedList<CPPVariableDecl> *result = ParseCPPTypeAndVariableDecl(tokeniser, true);
    CPPTokeniser_AcceptTokenIfType(tokeniser, CPPTokenType::CloseParen, nullptr);
    return result;
}

enum struct ParsedCodeType
{
    Invalid,
    Struct,
    Enum,
    Function,
};

struct ParsedEnum
{
    b32                             struct_or_class_decl;
    Slice<char>                     name;
    CPPDeclLinkedList<Slice<char>> *members;
};

struct ParsedStruct
{
    Slice<char>                       name;
    CPPDeclLinkedList<CPPVariableDecl> *members;
};


struct ParsedFunction
{
    Slice<char>                         return_type;
    Slice<char>                         name;
    CPPDeclLinkedList<CPPVariableDecl> *members;
    b32                                 has_comments;
    LinkedList<Slice<char>>             comments;
};

struct ParsedCode
{
    ParsedCodeType type;
    union
    {
        ParsedEnum              parsed_enum;
        ParsedFunction parsed_func;
        ParsedStruct            parsed_struct;
    };
};

struct ParsingResult
{
    std::vector<ParsedCode> code;
    int                     file_include_contents_hash_define_len;
    char                   *file_include_contents_hash_define;
    int                     max_func_return_type_decl_len;
    int                     max_func_name_decl_len;
    Slice<char>             file_name;
};


b32 ParseCPPEnum(CPPTokeniser *tokeniser, ParsedEnum *parsed_enum)
{
    *parsed_enum = {};
    CPPToken token = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::Identifier) || !IsIdentifierToken(token, SLICE_LITERAL("enum")))
        return false;

    token = CPPTokeniser_NextToken(tokeniser);
    if (IsIdentifierToken(token, SLICE_LITERAL("class")) ||
        IsIdentifierToken(token, SLICE_LITERAL("struct")))
    {
        parsed_enum->struct_or_class_decl = true;
        token                              = CPPTokeniser_NextToken(tokeniser);
    }

    if (!ExpectToken(token, CPPTokenType::Identifier))
        return false;

    int original_indent_level = tokeniser->indent_level;
    CPPToken const enum_name  = token;
    token                     = CPPTokeniser_NextToken(tokeniser);

    if (!ExpectToken(token, CPPTokenType::LeftBrace))
        return false;

    parsed_enum->name = Slice<char>(enum_name.str, enum_name.len);
    {
        CPPDeclLinkedList<Slice<char>> *link_iterator = nullptr;
        for (token       = CPPTokeniser_NextToken(tokeniser);
             tokeniser->indent_level != original_indent_level && token.type != CPPTokenType::EndOfStream;
             token       = CPPTokeniser_NextToken(tokeniser))
        {
            if (token.type == CPPTokenType::Identifier)
            {
                auto *link = MEM_ARENA_ALLOC_STRUCT(&global_main_arena, CPPDeclLinkedList<Slice<char>>);
                *link      = {};
                if (!link_iterator) parsed_enum->members = link; // Set members to first linked list entry
                else                link_iterator->next  = link;
                link_iterator = link;

                link->value         = Slice<char>(token.str, token.len);
                CPPToken peek_token = CPPTokeniser_PeekToken(tokeniser);
                if (IsIdentifierToken(peek_token, SLICE_LITERAL("DQN_INSPECT_META")))
                {
                    link->metadata_list = ParseCPPInspectMeta(tokeniser);
                }
            }
        }
    }

    return true;
}

b32 ParseCPPVariableType(CPPTokeniser *tokeniser, Slice<char> *type)
{
    CPPToken token = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::Identifier))
        return false;

    char *var_type_start = token.str;
    ConsumeConstIdentifier(tokeniser);

    //
    // Parse Template If Any
    //
    Slice<char> template_child_expr = {};
    {
        CPPToken peek_token = CPPTokeniser_PeekToken(tokeniser);
        if (peek_token.type == CPPTokenType::LessThan)
        {
            token              = CPPTokeniser_NextToken(tokeniser);
            int template_depth = 1;
            while (template_depth != 0 && token.type != CPPTokenType::EndOfStream)
            {
                token = CPPTokeniser_NextToken(tokeniser);
                if (token.type == CPPTokenType::LessThan)
                    template_depth++;
                else if (token.type == CPPTokenType::GreaterThan)
                    template_depth--;
            }

            if (template_depth == 0)
            {
                char *expr_start = peek_token.str + 1;
                char *expr_end   = token.str;
                int expr_len     = static_cast<int>(expr_end - expr_start);

                template_child_expr.str = expr_start;
                template_child_expr.len = expr_len;
            }
        }
    }

    int total_asterisks_count = ConsumeAsterisks(tokeniser);
    if (ConsumeConstIdentifier(tokeniser))
    {
        total_asterisks_count += ConsumeAsterisks(tokeniser);
        ConsumeConstIdentifier(tokeniser);
    }

    CPPToken var_name  = CPPTokeniser_PeekToken(tokeniser);
    if (!ExpectToken(var_name, CPPTokenType::Identifier))
        return false;

    char *var_type_end = var_name.str;
    if (type)
    {
        type->str = var_type_start;
        type->len = static_cast<int>(var_type_end - var_type_start);

        // NOTE(doyle): Remove space(s) at the end of the return type if
        // any, because we parse the function type weirdly by getting the
        // identifier for the function name and looking one token back.
        while (type->len >= 1 && CharIsWhitespace(type->str[type->len - 1]))
            type->len--;
    }

    return true;
}

b32 ParseCPPStruct(CPPTokeniser *tokeniser, ParsedStruct *parsed_struct)
{
    *parsed_struct = {};
    CPPToken token = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::Identifier) ||
        (!IsIdentifierToken(token, SLICE_LITERAL("struct")) && !IsIdentifierToken(token, SLICE_LITERAL("class"))))
        return false;

    int const original_indent_level = tokeniser->indent_level;
    token                           = CPPTokeniser_NextToken(tokeniser);

    if (token.type != CPPTokenType::LeftBrace)
    {
        if (!ExpectToken(token, CPPTokenType::Identifier)) return false;
        parsed_struct->name = Slice<char>(token.str, token.len);
    }

    {
        CPPDeclLinkedList<CPPVariableDecl> *link_iterator = nullptr;
        for (token = CPPTokeniser_NextToken(tokeniser);
             tokeniser->indent_level != original_indent_level && token.type != CPPTokenType::EndOfStream;
             token = CPPTokeniser_NextToken(tokeniser))
        {
            if (token.type == CPPTokenType::Identifier)
            {
                CPPTokeniser_RewindToken(tokeniser);
                CPPDeclLinkedList<CPPVariableDecl> *link = ParseCPPTypeAndVariableDecl(tokeniser, false);
                if (!parsed_struct->members) parsed_struct->members = link;
                if (!link_iterator) link_iterator = link;
                else
                {
                    for (;link_iterator->next; link_iterator = link_iterator->next) // TODO(doyle): Meh
                        ;
                    link_iterator->next = link;
                }
            }
        }
    }

    // TODO(doyle): Don't support anonymous/nameless structs yet
    if (parsed_struct->name.len == 0)
        return false;

    return true;
}

b32 ParseCPPInspectFunction(CPPTokeniser *tokeniser, ParsedFunction *parsed_func)
{
    *parsed_func = {};
    CPPToken token = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::InspectFunction))
        return false;

    struct FunctionDefaultParam
    {
        Slice<char> name;
        Slice<char> value;
    };

    //
    // NOTE: Parse default params in the inspect prototype macro
    //
    LinkedList<FunctionDefaultParam> *default_param_list = nullptr;
    {
        LinkedList<FunctionDefaultParam> *link_iterator = nullptr;
        if (!CPPTokeniser_AcceptTokenIfType(tokeniser, CPPTokenType::OpenParen, &token))
            return false;

        for (token = CPPTokeniser_NextToken(tokeniser);
             token.type != CPPTokenType::CloseParen && token.type != CPPTokenType::EndOfStream;
             token = CPPTokeniser_NextToken(tokeniser))
        {
            if (token.type == CPPTokenType::Comma)
                continue;

            Slice<char> default_param_name = Slice<char>(token.str, token.len);
            if (token.type != CPPTokenType::Identifier)
            {
                SkipFunctionParam(tokeniser);
                continue;
            }

            token = CPPTokeniser_NextToken(tokeniser);
            if (token.type != CPPTokenType::Equals)
            {
                SkipFunctionParam(tokeniser);
                continue;
            }

            token                     = CPPTokeniser_PeekToken(tokeniser);
            char *default_value_start = token.str;

            if (token.type == CPPTokenType::String) // NOTE(doyle): Include the quotes in the param value
                default_value_start--;

            SkipFunctionParam(tokeniser);
            CPPToken peek_token = CPPTokeniser_PeekToken(tokeniser);
            if (peek_token.type != CPPTokenType::Comma && peek_token.type != CPPTokenType::CloseParen)
                continue;

            char *default_value_end = peek_token.str;

            auto *link = MEM_ARENA_ALLOC_STRUCT(&global_main_arena, LinkedList<FunctionDefaultParam>);
            *link      = {};
            if (!default_param_list) default_param_list = link;
            else link_iterator->next                    = link;
            link_iterator = link;

            link->value.name  = default_param_name;
            link->value.value = Slice<char>(default_value_start, static_cast<int>(default_value_end - default_value_start));
        }
    }

    struct FunctionParam
    {
        Slice<char> type;
        Slice<char> name;

         // NOTE(doyle): This is resolved after function parsing, iterate over
         // the default params specified in the macro and match them to the
         // first param that has the same name
        Slice<char> default_value;
    };

    // Grab all following comments
    {
        LinkedList<Slice<char>> *comments = &parsed_func->comments;
        while (CPPTokeniser_AcceptTokenIfType(tokeniser, CPPTokenType::Comment, &token))
        {
            parsed_func->has_comments = true;
            if (comments->value.str > 0)
            {
                comments->next = MEM_ARENA_ALLOC_STRUCT(&global_main_arena, LinkedList<Slice<char>>);
                comments       = comments->next;
            }

            comments->value = Slice<char>(token.str, token.len);
        }
    }


    // Grab return type token
    {
        token = CPPTokeniser_PeekToken(tokeniser);
        if (!ExpectToken(token, CPPTokenType::Identifier))  return false;
        if (!ParseCPPVariableType(tokeniser, &parsed_func->return_type)) return false;
    }

    // Grab function name token
    {
        token = CPPTokeniser_PeekToken(tokeniser);
        if (!ExpectToken(token, CPPTokenType::Identifier)) return false;

        char *name_start = token.str;
        while (token.type != CPPTokenType::OpenParen && token.type != CPPTokenType::EndOfStream)
        {
            CPPTokeniser_NextToken(tokeniser);
            token = CPPTokeniser_PeekToken(tokeniser);
        }

        if (!ExpectToken(token, CPPTokenType::OpenParen)) return false;
        char *name_end    = token.str;
        parsed_func->name = Slice<char>(name_start, static_cast<int>(name_end - name_start));
    }

    token = CPPTokeniser_PeekToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::OpenParen))
        return false;

    //
    // NOTE: Parse CPP Function Parameters
    //
    {
        CPPDeclLinkedList<CPPVariableDecl> *link_iterator = nullptr;
        token = CPPTokeniser_NextToken(tokeniser);
        for (token = CPPTokeniser_NextToken(tokeniser);
             token.type != CPPTokenType::CloseParen && token.type != CPPTokenType::EndOfStream;
             token = CPPTokeniser_NextToken(tokeniser))
        {
            if (token.type == CPPTokenType::Identifier || token.type == CPPTokenType::VarArgs)
            {
                CPPTokeniser_RewindToken(tokeniser);
                CPPDeclLinkedList<CPPVariableDecl> *link = ParseCPPTypeAndVariableDecl(tokeniser, true);
                if (!parsed_func->members) parsed_func->members = link;
                if (!link_iterator)        link_iterator = link;
                else
                {
                    for (;link_iterator->next; link_iterator = link_iterator->next) // TODO(doyle): Meh
                        ;
                    link_iterator->next = link;
                }
            }
        }
    }

    //
    // NOTE: Map Default Parameters in the inspect prototype macro to the actual declaration in the CPP Prototype
    //
    for (LinkedList<FunctionDefaultParam> const *default_link = default_param_list; default_link; default_link = default_link->next)
    {
        FunctionDefaultParam const *default_param = &default_link->value;
        for (CPPDeclLinkedList<CPPVariableDecl> *param_link = parsed_func->members; param_link; param_link = param_link->next)
        {
            CPPVariableDecl *decl = &param_link->value;
            if (Slice_Cmp(decl->name, default_param->name))
            {
                decl->default_value = default_param->value;
                break;
            }
        }
    }

    return true;
}

enum struct InspectMode
{
    All,
    Code,
    Function
};

char *EnumOrStructOrFunctionLexer(CPPTokeniser *tokeniser, char *ptr, b32 lexing_function)
{
    // NOTE(doyle): If we're lexing a function we're doing DQN_INSPECT_GENERATE_PROTOTYPE()
    // Where we want to lex the macro and then the function following it, so 2 iterations
    int iterations = lexing_function ? 2 : 1;

    for (int i = 0; i < iterations; ++i)
    {
        int indent_level                = 0;
        bool started_lexing_brace_scope = false;
        bool started_lexing_function    = false;
        int paren_level                 = 0;
        for (; ptr;)
        {
            while (CharIsWhitespace(ptr[0])) ptr++;
            if (!ptr[0]) break;

            CPPToken *token = CPPTokeniser_MakeToken(tokeniser);
            token->str      = ptr++;
            token->len      = 1;
            switch(token->str[0])
            {
                case '{': { token->type = CPPTokenType::LeftBrace;  started_lexing_brace_scope = true; indent_level++; } break;
                case '}': { token->type = CPPTokenType::RightBrace; indent_level--; } break;
                case '[': { token->type = CPPTokenType::LeftSqBracket;  } break;
                case ']': { token->type = CPPTokenType::RightSqBracket; } break;
                case '(': { token->type = CPPTokenType::OpenParen;   started_lexing_function = true; paren_level++; } break;
                case ')': { token->type = CPPTokenType::CloseParen;  paren_level--; } break;
                case ',': { token->type = CPPTokenType::Comma;       } break;
                case ';': { token->type = CPPTokenType::SemiColon;   } break;
                case '=': { token->type = CPPTokenType::Equals;      } break;
                case '<': { token->type = CPPTokenType::LessThan;    } break;
                case '>': { token->type = CPPTokenType::GreaterThan; } break;
                case ':': { token->type = CPPTokenType::Colon;       } break;
                case '*': { token->type = CPPTokenType::Asterisks;   } break;
                case '&': { token->type = CPPTokenType::Ampersand;   } break;

                case '.':
                {
                    if (token->str[1] == '.' && token->str[2] == '.')
                    {
                        token->type = CPPTokenType::VarArgs;
                        token->len  = 3;
                        ptr += 2;
                    }
                    else
                    {
                        token->len = 0; // NOTE: Skip
                    }
                }
                break;

                case '/':
                {
                    token->type = CPPTokenType::FwdSlash;
                    if (ptr[0] == '/' || ptr[0] == '*')
                    {
                        token->type = CPPTokenType::Comment;
                        if (ptr[0] == '/')
                        {
                            ptr++;
                            while (ptr[0] == ' ' || ptr[0] == '\t') ptr++;
                            token->str = ptr;
                            while (ptr[0] != '\n') ptr++;
                        }
                        else
                        {
                            for (;;)
                            {
                                while (ptr[0] != '*') ptr++;
                                ptr++;
                                if (ptr[0] == '\\') break;
                                token->len = static_cast<int>(ptr - token->str);
                            }
                        }

                        token->len = static_cast<int>(ptr - token->str);
                        if (token->len >= 1)
                            if (ptr[-1] == '\r') token->len--;
                    }
                }
                break;

                default:
                {
                    ptr--;
                    if (ptr[0] == '"')
                    {
                        token->type = CPPTokenType::String;
                        for (token->str = ++ptr;;)
                        {
                            while (ptr[0] != '"') ptr++;
                            token->len = static_cast<int>(ptr - token->str);
                            if (ptr[-1] != '\\')
                            {
                                ptr++;
                                break;
                            }
                        }
                    }
                    else
                    {
                        if (CharIsDigit(ptr[0]) || ptr[0] == '-' || ptr[0] == '*')
                        {
                            while (CharIsDigit(ptr[0]) || ptr[0] == 'x' || ptr[0] == 'b' || ptr[0] == 'e' || ptr[0] == '.' || ptr[0] == 'f' || ptr[0] == '-' || ptr[0] == '+')
                                ptr++;

                            token->type = CPPTokenType::Number;
                        }
                        else
                        {
                            token->type = CPPTokenType::Identifier;
                            if (CharIsAlpha(ptr[0]) || ptr[0] == '_')
                            {
                                ptr++;
                                while (CharIsAlphaNum(ptr[0]) || ptr[0] == '_') ptr++;
                            }
                        }

                        token->len = static_cast<int>(ptr - token->str);
                    }

                }
                break;
            }

            if (token->len == 0 && token->type != CPPTokenType::Comment)
            {
                *token = {};
                tokeniser->tokens_len--;
            }
            else
            {
                if (lexing_function)
                {
                    if (started_lexing_function && paren_level == 0)
                        break;
                }
                else
                {
                    if (started_lexing_brace_scope && indent_level == 0)
                        break;
                }
            }
        }
    }

    return ptr;
}

void FprintDeclType(FILE *output_file, Slice<char> const type, Slice<char> const template_child_expr)
{
    Slice<char> const array[] =
    {
        type,
        template_child_expr
    };

    for (int array_index = 0; array_index < ArrayCount(array); ++array_index)
    {
        Slice<char> const *slice = array + array_index;
        if (array_index > 0 && slice->len > 0)
            fputc('_', output_file);

        for (int i = 0; i < slice->len; ++i)
        {
            char ch = slice->str[i];
            if (ch == '*')
            {
                char prev_ch = slice->str[i - 1];
                if (prev_ch != ' ' && prev_ch != '*') fputc('_', output_file);

                fputs("Ptr", output_file);
                if (i < (slice->len - 1)) fputc('_', output_file);
            }
            else
            {
                if (ch != '>')
                {
                    if (ch == ' ' || ch == '<') ch = '_';
                    fputc(ch, output_file);
                }
            }
        }
    }

    fputc('_', output_file);
}

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        fprintf(stdout,
                "Usage: %s [code|generate_prototypes] [<source code filename>, ...]\n"
                "Options: If ommitted, both modes are run\n"
                " code                Only generate the inspection data for structs/enums marked with DQN_INSPECT\n"
                " generate_prototypes Only generate the function prototypes for functions marked with DQN_INSPECT_GENERATE_PROTOTYPE(...)\n",
                argv[0]);
        return 0;
    }

    usize starting_arg_index = 1;
    char const *mode_str     = argv[1];
    InspectMode mode         = InspectMode::All;
    if (strcmp(mode_str, "code") == 0)           mode = InspectMode::Code;
    else if (strcmp(mode_str, "function") == 0) mode = InspectMode::Function;

    if (mode != InspectMode::All)
        starting_arg_index++;

    usize main_arena_mem_size = MEGABYTE(8);
    void *main_arena_mem      = malloc(main_arena_mem_size);
    global_main_arena         = MemArena_Init(main_arena_mem, main_arena_mem_size);
#if 0
    FILE *output_file         = fopen("DqnInspect_Generated.cpp", "w");
#else
    FILE *output_file         = stdout;
#endif

    fprintf(output_file,
            "// This is an auto generated file using DqnInspect\n\n");

    if (mode == InspectMode::All || mode == InspectMode::Code)
    {
        fprintf(output_file,
                " // NOTE: These macros are undefined at the end of the file so to not pollute namespace\n"
                "#define ARRAY_COUNT(array) sizeof(array)/sizeof((array)[0])\n"
                "#define CHAR_COUNT(str) (ARRAY_COUNT(str) - 1)\n"
                "#define STR_AND_LEN(str) str, CHAR_COUNT(str)\n"
                "\n"
                );
    }

    std::vector<ParsingResult> parsing_results_per_file = {};
    parsing_results_per_file.reserve(64);

    for (usize arg_index = starting_arg_index; arg_index < argc; ++arg_index)
    {
        char *file_name = argv[arg_index];
        FILE *file      = fopen(file_name, "rb");
        fseek(file, 0, SEEK_END);
        usize file_size = ftell(file);
        rewind(file);

        char *file_buf      = (char *)MemArena_Alloc(&global_main_arena, file_size + 1);
        file_buf[file_size] = 0;

        if (fread(file_buf, file_size, 1, file) != 1)
        {
            fprintf(stderr, "Failed to fread: %zu bytes into buffer from file: %s\n", file_size, file_name);
            continue;
        }

        ParsingResult parsing_results = {};
        parsing_results.code.reserve(128);
        int const file_name_len = (int)strlen(file_name);
        {
            char *extracted_file_name_buf   = static_cast<char *>(MemArena_Alloc(&global_main_arena, file_name_len));
            int extracted_file_name_len     = 0;

            for (int i = file_name_len - 1; i >= 0; i--, extracted_file_name_len++)
            {
                char ch = file_name[i];
                if (ch == '.')
                    ch = '_';

                if (ch >= 'a' && ch <= 'z') ch -= 'a' - 'A';

                if (ch == '\\' || ch == '/')
                    break;

                extracted_file_name_buf[i] = ch;
            }

            parsing_results.file_include_contents_hash_define     = extracted_file_name_buf + file_name_len - extracted_file_name_len;
            parsing_results.file_include_contents_hash_define_len = extracted_file_name_len;
            parsing_results.file_name                             = Slice<char>(file_name, file_name_len);
        }

        CPPTokeniser tokeniser = {};
        tokeniser.tokens_max   = 16384;
        tokeniser.tokens       = MEM_ARENA_ALLOC_ARRAY(&global_main_arena, CPPToken, tokeniser.tokens_max);

        Slice<char const> const INSPECT_FUNCTION = SLICE_LITERAL("DQN_INSPECT_FUNCTION");
        Slice<char const> const INSPECT_PREFIX   = SLICE_LITERAL("DQN_INSPECT");
        char *file_buf_end                       = file_buf + file_size;
        Slice<char> buffer                       = Slice<char>(file_buf, static_cast<int>(file_size));

        for (char *ptr = StrFind(buffer, INSPECT_PREFIX);
             ptr;
             ptr = StrFind(buffer, INSPECT_PREFIX))
        {
            char *marker_str = ptr;
            int marker_len   = static_cast<int>(file_buf_end - ptr);

            DEFER
            {
                buffer.str = ptr;
                buffer.len = static_cast<int>(file_buf_end - ptr);
            };

            CPPTokenType inspect_type = CPPTokenType::InspectCode;
            if (Slice_Cmp(Slice<char const>(ptr, INSPECT_FUNCTION.len), INSPECT_FUNCTION))
            {
                inspect_type = CPPTokenType::InspectFunction;
            }

            if (inspect_type == CPPTokenType::InspectCode)
            {
                ptr += INSPECT_PREFIX.len;
                if (mode == InspectMode::Function) continue;
            }
            else
            {
                ptr += INSPECT_FUNCTION.len;
                if (mode == InspectMode::Code) continue;
            }

            CPPToken *inspect_token = CPPTokeniser_MakeToken(&tokeniser);
            inspect_token->type     = inspect_type;
            inspect_token->str      = marker_str;
            inspect_token->len      = marker_len;
            ptr = EnumOrStructOrFunctionLexer(&tokeniser, ptr, inspect_type == CPPTokenType::InspectFunction);

        }

        CPPToken *sentinel = CPPTokeniser_MakeToken(&tokeniser);
        sentinel->type     = CPPTokenType::EndOfStream;

        for (CPPToken token = CPPTokeniser_PeekToken(&tokeniser);
             ;
             token          = CPPTokeniser_PeekToken(&tokeniser))
        {
            ParsedCode parsed_code = {};
            if (token.type == CPPTokenType::InspectCode || token.type == CPPTokenType::InspectFunction)
            {
                if (token.type == CPPTokenType::InspectCode)
                {
                    token = CPPTokeniser_NextToken(&tokeniser);
                    token = CPPTokeniser_PeekToken(&tokeniser);

                    if (IsIdentifierToken(token, SLICE_LITERAL("enum")))
                    {
                        if (ParseCPPEnum(&tokeniser, &parsed_code.parsed_enum))
                            parsed_code.type = ParsedCodeType::Enum;
                    }
                    else if (IsIdentifierToken(token, SLICE_LITERAL("struct")) || IsIdentifierToken(token, SLICE_LITERAL("class")))
                    {
                        if (ParseCPPStruct(&tokeniser, &parsed_code.parsed_struct))
                            parsed_code.type = ParsedCodeType::Struct;
                    }
                }
                else
                {
                    if (ParseCPPInspectFunction(&tokeniser, &parsed_code.parsed_func))
                    {
                        parsed_code.type = ParsedCodeType::Function;
                        parsing_results.max_func_return_type_decl_len = INSPECT_MAX(parsing_results.max_func_return_type_decl_len, parsed_code.parsed_func.return_type.len);
                        parsing_results.max_func_name_decl_len        = INSPECT_MAX(parsing_results.max_func_name_decl_len, parsed_code.parsed_func.name.len);
                    }
                }
            }
            else
            {
                token = CPPTokeniser_NextToken(&tokeniser);
            }

            if (parsed_code.type != ParsedCodeType::Invalid)
                parsing_results.code.push_back(parsed_code);

            if (token.type == CPPTokenType::EndOfStream)
                break;
        }
        parsing_results_per_file.push_back(std::move(parsing_results));
    }

    int indent_level = 0;
    if (mode == InspectMode::All || mode == InspectMode::Code)
    {
        //
        // NOTE: Build the global definition table
        //
        {
            std::set<Slice<char>> member_type_table;
            for (ParsingResult &parsing_results : parsing_results_per_file)
            {
                for (ParsedCode &code : parsing_results.code)
                {
                    switch(code.type)
                    {
                        case ParsedCodeType::Enum:
                        {
                            ParsedEnum const *parsed_enum = &code.parsed_enum;
                            member_type_table.insert(parsed_enum->name);
                            for (CPPDeclLinkedList<Slice<char>> const *link = parsed_enum->members;
                                 link;
                                 link = link->next)
                            {
                                Slice<char> entry = Asprintf(&global_main_arena, "%.*s_%.*s",
                                                             parsed_enum->name.len, parsed_enum->name.str,
                                                             link->value.len, link->value.str
                                                             );
                                member_type_table.insert(entry);
                            }

                        }
                        break;

                        case ParsedCodeType::Struct:
                        {
                            ParsedStruct const *parsed_struct = &code.parsed_struct;
                            member_type_table.insert(parsed_struct->name);
                            for (CPPDeclLinkedList<CPPVariableDecl> const *link = parsed_struct->members;
                                 link;
                                 link = link->next)
                            {
                                Slice<char> entry = Asprintf(&global_main_arena, "%.*s_%.*s",
                                                             parsed_struct->name.len, parsed_struct->name.str,
                                                             link->value.name.len, link->value.name.str
                                                             );
                                member_type_table.insert(entry);
                            }
                        }
                        break;
                    }
                }
            }

            FprintfIndented(output_file, indent_level, "enum struct DqnInspectMemberType\n{\n");
            indent_level++;
            for (Slice<char> const &member : member_type_table)
            {
                FprintfIndented(output_file, indent_level, "%.*s,\n", member.len, member.str);
            }
            indent_level--;
            FprintfIndented(output_file, indent_level, "};\n\n");
        }

        //
        // NOTE: Build the global type table
        //
        {
            std::set<Slice<char>> unique_decl_type_table;
            for (ParsingResult &parsing_results : parsing_results_per_file)
            {
                for (ParsedCode &code : parsing_results.code)
                {
                    switch(code.type)
                    {
                        case ParsedCodeType::Enum:
                        {
                            ParsedEnum const *parsed_enum = &code.parsed_enum;
                            unique_decl_type_table.insert(parsed_enum->name);
                        }
                        break;

                        case ParsedCodeType::Struct:
                        {
                            ParsedStruct const *parsed_struct = &code.parsed_struct;
                            unique_decl_type_table.insert(parsed_struct->name);

                            for (CPPDeclLinkedList<CPPVariableDecl> const *link = parsed_struct->members;
                                 link;
                                 link = link->next)
                            {
                                CPPVariableDecl const *decl = &link->value;

                                unique_decl_type_table.insert(Asprintf(&global_main_arena, "%.*s", decl->type.len, decl->type.str));
                                if (decl->template_child_expr.len > 0)
                                {
                                    if (!CharIsDigit(decl->template_child_expr.str[0]))
                                        unique_decl_type_table.insert(Asprintf(&global_main_arena, "%.*s", decl->template_child_expr.len, decl->template_child_expr.str));
                                    unique_decl_type_table.insert(Asprintf(&global_main_arena, "%.*s<%.*s>", decl->type.len, decl->type.str, decl->template_child_expr.len, decl->template_child_expr.str));
                                }

                                for (CPPDeclLinkedList<CPPVariableDecl> const *meta_link = link->metadata_list;
                                     meta_link;
                                     meta_link = meta_link->next)
                                {
                                    CPPVariableDecl const *meta_decl = &meta_link->value;
                                    unique_decl_type_table.insert(Asprintf(&global_main_arena, "%.*s", meta_decl->type.len, meta_decl->type.str));
                                    if (meta_decl->template_child_expr.len > 0)
                                    {
                                        unique_decl_type_table.insert(Asprintf(&global_main_arena, "%.*s", meta_decl->template_child_expr.len, meta_decl->template_child_expr.str));
                                        unique_decl_type_table.insert(Asprintf(&global_main_arena, "%.*s<%.*s>", meta_decl->type.len, meta_decl->type.str, meta_decl->template_child_expr.len, meta_decl->template_child_expr.str));
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }

            FprintfIndented(output_file, indent_level, "enum struct DqnInspectDeclType\n{\n");
            indent_level++;
            FprintfIndented(output_file, indent_level, "NotAvailable_,\n");
            for (Slice<char> const &type : unique_decl_type_table)
            {
                FprintfIndented(output_file, indent_level, "");
                FprintDeclType(output_file, type, {});
                fprintf(output_file, ",\n");
            }
            indent_level--;
            FprintfIndented(output_file, indent_level, "};\n\n");
        }

        //
        // NOTE: Build the global metadata type table
        //
        {
            std::set<Slice<char>> unique_meta_types;
            for (ParsingResult &parsing_results : parsing_results_per_file)
            {
                for (ParsedCode &code : parsing_results.code)
                {
                    switch(code.type)
                    {
                        case ParsedCodeType::Struct:
                        {
                            ParsedStruct const *parsed_struct = &code.parsed_struct;
                            for (CPPDeclLinkedList<CPPVariableDecl> const *member = parsed_struct->members; member; member = member->next)
                            {
                                for (CPPDeclLinkedList<CPPVariableDecl> const *metadata_link = member->metadata_list;
                                     metadata_link;
                                     metadata_link = metadata_link->next)
                                {
                                    CPPVariableDecl const *metadata = &metadata_link->value;
                                    unique_meta_types.insert(metadata->name);
                                }
                            }
                        }
                        break;
                    }
                }
            }

            FprintfIndented(output_file, indent_level, "enum struct DqnInspectMetaType\n{\n");
            indent_level++;
            for (Slice<char> const &metadata : unique_meta_types)
                FprintfIndented(output_file, indent_level, "%.*s,\n", metadata.len, metadata.str);
            indent_level--;
            FprintfIndented(output_file, indent_level, "};\n\n");
        }

    }

    assert(indent_level == 0);
    for (ParsingResult &parsing_results : parsing_results_per_file)
    {
        fprintf(output_file,
                "//\n"
                "// %.*s\n"
                "//\n"
                "\n"
                "#if !defined(DQN_INSPECT_DISABLE_%.*s) && !defined(DQN_INSPECT_%.*s)\n"
                "#define DQN_INSPECT_%.*s\n"
                "\n",
                parsing_results.file_name.len,
                parsing_results.file_name.str,
                parsing_results.file_include_contents_hash_define_len,
                parsing_results.file_include_contents_hash_define,
                parsing_results.file_include_contents_hash_define_len,
                parsing_results.file_include_contents_hash_define,
                parsing_results.file_include_contents_hash_define_len,
                parsing_results.file_include_contents_hash_define
                );

        for (ParsedCode &code : parsing_results.code)
        {
            switch(code.type)
            {
                case ParsedCodeType::Enum:
                {
                    ParsedEnum const *parsed_enum = &code.parsed_enum;
                    //
                    // NOTE: Write Stringified Enum Array
                    //
                    {
                        FprintfIndented(output_file, indent_level, "char const *DqnInspect_%.*s_Strings[] = {", parsed_enum->name.len, parsed_enum->name.str);
                        indent_level++;
                        for (CPPDeclLinkedList<Slice<char>> const *link = parsed_enum->members; link; link = link->next)
                        {
                            Slice<char> const enum_value = link->value;
                            fprintf(output_file, "\"%.*s\", ", enum_value.len, enum_value.str);
                        }

                        indent_level--;
                        FprintfIndented(output_file, indent_level, "};\n\n");
                    }

                    //
                    // NOTE: Write DqnInspectEnum_Stringify Function
                    //
                    {
                        FprintfIndented(output_file, indent_level, "char const *DqnInspectEnum_Stringify(%.*s val, int *len = nullptr)\n{\n", parsed_enum->name.len, parsed_enum->name.str);
                        indent_level++;
                        DEFER
                        {
                            FprintfIndented(output_file, indent_level, "return nullptr;\n");
                            indent_level--;
                            FprintfIndented(output_file, indent_level, "}\n\n");
                        };

                        struct SourceCode
                        {
                            Slice<char> decl;
                            Slice<char> enum_value;
                        };

                        LinkedList<SourceCode> src_code = {};
                        int longest_decl_len            = 0;
                        {
                            LinkedList<SourceCode> *curr_src_code = nullptr;
                            char const *fmt = (parsed_enum->struct_or_class_decl) ? "if (val == %.*s::%.*s) " : "if (val == %.*s) ";
                            for (CPPDeclLinkedList<Slice<char>> *link = parsed_enum->members; link; link = link->next)
                            {
                                if (!curr_src_code) curr_src_code = &src_code;
                                else
                                {
                                    curr_src_code->next = static_cast<LinkedList<SourceCode> *>(MemArena_Alloc(&global_main_arena, sizeof(*curr_src_code)));
                                    curr_src_code       = curr_src_code->next;
                                }

                                Slice<char> enum_value = link->value;
                                int required_len         = 0;

                                if (parsed_enum->struct_or_class_decl) required_len = snprintf(nullptr, 0, fmt, parsed_enum->name.len, parsed_enum->name.str, enum_value.len, enum_value.str) + 1;
                                else                                   required_len = snprintf(nullptr, 0, fmt, enum_value.len, enum_value.str) + 1;

                                longest_decl_len                = INSPECT_MAX(longest_decl_len, required_len);
                                curr_src_code->value.decl.str   = MEM_ARENA_ALLOC_ARRAY(&global_main_arena, char, required_len);
                                curr_src_code->value.decl.len   = required_len;
                                curr_src_code->value.enum_value = enum_value;

                                if (parsed_enum->struct_or_class_decl) snprintf(curr_src_code->value.decl.str, curr_src_code->value.decl.len, fmt, parsed_enum->name.len, parsed_enum->name.str, enum_value.len, enum_value.str);
                                else                                   snprintf(curr_src_code->value.decl.str, curr_src_code->value.decl.len, fmt, enum_value.len, enum_value.str);
                            }
                            curr_src_code->next = nullptr;
                        }

                        int enum_index = 0;
                        for (LinkedList<SourceCode> *src_code_ptr = &src_code;
                             src_code_ptr;
                             src_code_ptr = src_code_ptr->next, ++enum_index)
                        {
                            Slice<char> enum_value = src_code_ptr->value.enum_value;
                            int padding              = longest_decl_len - src_code_ptr->value.decl.len;
                            FprintfIndented(output_file, indent_level, "%.*s%*s", src_code_ptr->value.decl.len, src_code_ptr->value.decl.str, padding, "");
                            fprintf(output_file,
                                                     "{ if (len) *len = CHAR_COUNT(\"%.*s\"); return DqnInspect_%.*s_Strings[%d]; }\n",
                                                     enum_value.len, enum_value.str,
                                                     parsed_enum->name.len, parsed_enum->name.str,
                                                     enum_index);
                        }
                    }

                    //
                    // NOTE: Write DqnInspectEnum_Count Function
                    //
                    {
                        int count = 0;
                        for (CPPDeclLinkedList<Slice<char>> *link = parsed_enum->members; link; link = link->next) count++;
                        FprintfIndented(output_file, indent_level, "int DqnInspectEnum_Count(%.*s) { return %d; }\n\n", parsed_enum->name.len, parsed_enum->name.str, count);
                    }

                    //
                    // NOTE: Write User Annotated Metadata Getter Functions
                    //
                    {
                        struct CPPDeclToMetaValue
                        {
                            Slice<char> cpp_decl;
                            Slice<char> value;
                        };

                        // i.e. DataType cpp_decl DQN_INSPECT_META(type1 name1 = value, type2 name2 = value2, ...);
                        struct MetadataEntry
                        {
                            Slice<char> type;
                            Slice<char> name;
                            FixedArray<CPPDeclToMetaValue, 32> cpp_decl_to_val;
                        };

                        FixedArray<MetadataEntry, 32> metadata_entries = {};
                        for (CPPDeclLinkedList<Slice<char>> *link = parsed_enum->members;
                             link;
                             link = link->next)
                        {
                            for (CPPDeclLinkedList<CPPVariableDecl> const *metadata_link = link->metadata_list;
                                 metadata_link;
                                 metadata_link = metadata_link->next)
                            {
                                MetadataEntry *metadata_entry_to_append_to = nullptr;
                                for (MetadataEntry &check_metadata_entry : metadata_entries)
                                {
                                    if (Slice_Cmp(check_metadata_entry.type, metadata_link->value.type) && Slice_Cmp(check_metadata_entry.name, metadata_link->value.name))
                                    {
                                        metadata_entry_to_append_to = &check_metadata_entry;
                                        break;
                                    }
                                }

                                if (!metadata_entry_to_append_to)
                                {
                                    metadata_entry_to_append_to       = FixedArray_Make(&metadata_entries, 1);
                                    metadata_entry_to_append_to->type = metadata_link->value.type;
                                    metadata_entry_to_append_to->name = metadata_link->value.name;
                                }

                                CPPDeclToMetaValue decl_to_val = {};
                                decl_to_val.cpp_decl       = Slice<char>(link->value.str, link->value.len);
                                decl_to_val.value          = metadata_link->value.default_value;
                                FixedArray_Add(&metadata_entry_to_append_to->cpp_decl_to_val, decl_to_val);
                            }
                        }

                        for (MetadataEntry const &metadata : metadata_entries)
                        {
                            Slice<char const> const char_type = SLICE_LITERAL("char");
                            Slice<char const> const bool_types[] =
                            {
                                SLICE_LITERAL("bool"),
                                SLICE_LITERAL("b32"),
                            };

                            b32 is_char_type                  = (metadata.type.len >= char_type.len && strncmp(metadata.type.str, char_type.str, char_type.len) == 0);
                            b32 is_bool_type = false;
                            for (size_t i = 0; i < ArrayCount(bool_types) && !is_char_type; i++)
                            {
                                Slice<char const> const *bool_type = bool_types + i;
                                is_bool_type                       = (metadata.type.len >= bool_type->len && strncmp(metadata.type.str, bool_type->str, bool_type->len) == 0);
                                if (is_bool_type) break;
                            }

                            if (is_char_type || is_bool_type)
                            {
                                FprintfIndented(output_file, indent_level,
                                              "%.*s DqnInspectMetadata_%.*s(%.*s val)\n{\n",
                                              metadata.type.len, metadata.type.str,
                                              metadata.name.len, metadata.name.str,
                                              parsed_enum->name.len, parsed_enum->name.str);

                                indent_level++;
                                DEFER
                                {
                                    if (is_char_type)
                                        FprintfIndented(output_file, indent_level, "return nullptr;\n");
                                    else
                                        FprintfIndented(output_file, indent_level, "return false;\n");
                                    indent_level--;
                                    FprintfIndented(output_file, indent_level, "}\n\n");
                                };

                                for (CPPDeclToMetaValue const &decl_to_val : metadata.cpp_decl_to_val)
                                {
                                    Slice<char> const *cpp_decl = &decl_to_val.cpp_decl;
                                    Slice<char> const *value    = &decl_to_val.value;

                                    if (parsed_enum->struct_or_class_decl)
                                    {
                                        FprintfIndented(output_file, indent_level,
                                                                   "if (val == %.*s::%.*s) ",
                                                                   parsed_enum->name.len, parsed_enum->name.str,
                                                                   cpp_decl->len, cpp_decl->str);
                                    }
                                    else
                                    {
                                        FprintfIndented(output_file, indent_level,
                                                                   "if (val == %.*s) ",
                                                                   cpp_decl->len, cpp_decl->str);
                                    }
                                    fprintf(output_file, "{ return %.*s; }\n", value->len, value->str);
                                }
                            }
                            else
                            {
                                FprintfIndented(output_file, indent_level,
                                                           "bool DqnInspectMetadata_%.*s(%.*s val, %.*s *value = nullptr)\n{\n",
                                                           metadata.name.len, metadata.name.str,
                                                           parsed_enum->name.len, parsed_enum->name.str,
                                                           metadata.type.len, metadata.type.str
                                                           );

                                indent_level++;
                                DEFER
                                {
                                    FprintfIndented(output_file, indent_level, "return false;\n");
                                    indent_level--;
                                    FprintfIndented(output_file, indent_level, "}\n\n");
                                };

                                for (CPPDeclToMetaValue const &decl_to_val : metadata.cpp_decl_to_val)
                                {
                                    Slice<char> const *cpp_decl = &decl_to_val.cpp_decl;
                                    Slice<char> const *value    = &decl_to_val.value;

                                    if (parsed_enum->struct_or_class_decl)
                                    {
                                        FprintfIndented(output_file, indent_level,
                                                                   "if (val == %.*s::%.*s) ",
                                                                   parsed_enum->name.len, parsed_enum->name.str,
                                                                   cpp_decl->len, cpp_decl->str);
                                    }
                                    else
                                    {
                                        FprintfIndented(output_file, indent_level,
                                                                   "if (val == %.*s) ",
                                                                   cpp_decl->len, cpp_decl->str);
                                    }
                                    fprintf(output_file, "{ *value = %.*s; return true; }\n", value->len, value->str);
                                }
                            }

                        }
                    }
                }
                break;

                case ParsedCodeType::Struct:
                {
                    ParsedStruct const *parsed_struct = &code.parsed_struct;
                    //
                    // NOTE: Write Metadata Global Variables
                    //
                    for (CPPDeclLinkedList<CPPVariableDecl> const *member = parsed_struct->members; member; member = member->next)
                    {
                        CPPVariableDecl const *decl = &member->value;
                        for (CPPDeclLinkedList<CPPVariableDecl> const *metadata_link = member->metadata_list;
                             metadata_link;
                             metadata_link = metadata_link->next)
                        {
                            CPPVariableDecl const *metadata = &metadata_link->value;
                            FprintfIndented(output_file, indent_level,
                                          "%.*s DqnInspectMetadata_%.*s_%.*s_%.*s = %.*s;\n",
                                          metadata->type.len, metadata->type.str,
                                          parsed_struct->name.len, parsed_struct->name.str,
                                          decl->name.len, decl->name.str,
                                          metadata->name.len, metadata->name.str,
                                          metadata->default_value.len, metadata->default_value.str
                                          );
                        }
                    }
                    FprintfIndented(output_file, indent_level, "\n");

                    //
                    // NOTE: Write metadata for each member
                    //
                    for (CPPDeclLinkedList<CPPVariableDecl> const *member = parsed_struct->members; member; member = member->next)
                    {
                        CPPVariableDecl const *decl = &member->value;

                        if (!member->metadata_list)
                            continue;

                        FprintfIndented(output_file, indent_level,
                                      "DqnInspectMetadata const DqnInspectMetadata_%.*s_%.*s[] =\n{\n",
                                      parsed_struct->name.len, parsed_struct->name.str,
                                      decl->name.len, decl->name.str
                                      );

                        indent_level++;
                        for (CPPDeclLinkedList<CPPVariableDecl> const *metadata_link = member->metadata_list;
                             metadata_link;
                             metadata_link = metadata_link->next)
                        {
                            CPPVariableDecl const *metadata = &metadata_link->value;
                            FprintfIndented(output_file, indent_level, "{ DqnInspectDeclType::");
                            FprintDeclType(output_file, metadata->type, metadata->template_child_expr);
                            fprintf(output_file,
                                    ", DqnInspectMetaType::%.*s, &DqnInspectMetadata_%.*s_%.*s_%.*s},\n",
                                    metadata->name.len, metadata->name.str,

                                    // NOTE: Assign variant data to void *, &DqnInspectMetdata ...
                                    parsed_struct->name.len, parsed_struct->name.str,
                                    decl->name.len, decl->name.str,
                                    metadata->name.len, metadata->name.str
                                    );
                        }
                        indent_level--;
                        FprintfIndented(output_file, indent_level, "};\n\n");
                    }


                    //
                    // NOTE: Write DqnInspectMember Definition
                    //
                    {
                        FprintfIndented(output_file, indent_level, "DqnInspectMember const DqnInspect_%.*s_Members[] =\n{\n", parsed_struct->name.len, parsed_struct->name.str);
                        indent_level++;

                        for (CPPDeclLinkedList<CPPVariableDecl> const *member = parsed_struct->members; member; member = member->next)
                        {
                            CPPVariableDecl const *decl = &member->value;
                            FprintfIndented(output_file, indent_level, "{\n");
                            indent_level++;

                            FprintfIndented(output_file, indent_level, "DqnInspectMemberType::%.*s_%.*s, ", parsed_struct->name.len, parsed_struct->name.str, decl->name.len, decl->name.str);
                            fprintf(output_file, "STR_AND_LEN(\"%.*s\"),\n", decl->name.len, decl->name.str);

                            // NOTE: Write pod_struct_offset
                            {
                                FprintfIndented(output_file, indent_level, "offsetof(%.*s, %.*s),\n", parsed_struct->name.len, parsed_struct->name.str, decl->name.len, decl->name.str);
                            }

                            // NOTE: Write decl_type
                            {
                                FprintfIndented(output_file, indent_level, "DqnInspectDeclType::");
                                FprintDeclType(output_file, decl->type, decl->template_child_expr);
                            }

                            // NOTE: Write decl_type_str, decl_type_len
                            {
                                if (decl->template_child_expr.len > 0)
                                    fprintf(output_file, ", STR_AND_LEN(\"%.*s<%.*s>\"),\n", decl->type.len, decl->type.str, decl->template_child_expr.len, decl->template_child_expr.str);
                                else
                                    fprintf(output_file, ", STR_AND_LEN(\"%.*s\"),\n", decl->type.len, decl->type.str);
                            }

                            // NOTE: Write decl_type_sizeof
                            {
                                FprintfIndented(output_file, indent_level, "sizeof(((%.*s *)0)->%.*s), // full_decl_type_sizeof\n", parsed_struct->name.len, parsed_struct->name.str, decl->name.len, decl->name.str);
                            }

                            // NOTE: Write template_parent_decl_type
                            {
                                FprintfIndented(output_file, indent_level, "DqnInspectDeclType::");
                                FprintDeclType(output_file, decl->type, {});
                                fprintf(output_file, ", ");
                            }

                            // NOTE: Write template_decl_type
                            {
                                if (decl->template_child_expr.len > 0 && !CharIsDigit(decl->template_child_expr.str[0]))
                                {
                                    fprintf(output_file, "DqnInspectDeclType::");
                                    FprintDeclType(output_file, decl->template_child_expr, {});
                                }
                                else
                                {
                                    fprintf(output_file, "DqnInspectDeclType::NotAvailable_");
                                }
                                fprintf(output_file, ",\n");
                            }

                            // NOTE: Write template_child_expr, template_child_expr_len
                            {
                                if (decl->template_child_expr.len > 0)
                                    FprintfIndented(output_file, indent_level, "STR_AND_LEN(\"%.*s\"), // template_child_expr\n", decl->template_child_expr.len, decl->template_child_expr.str);
                                else
                                    FprintfIndented(output_file, indent_level, "nullptr, 0, // template_child_expr and template_child_expr_len\n");
                            }

                            FprintfIndented(output_file, indent_level, "%d, // array_dimensions\n", decl->array_dimensions);

                            FprintfIndented(output_file, indent_level, "{");
                            for (int dimensions_index = 0;
                                 dimensions_index < ArrayCount(decl->array_dimensions_has_compile_time_size);
                                 ++dimensions_index)
                            {
                                bool has_compile_time_size = decl->array_dimensions_has_compile_time_size[dimensions_index];
                                if (has_compile_time_size)
                                {
                                    fprintf(output_file, "ARRAY_COUNT(((%.*s *)0)->%.*s)", parsed_struct->name.len, parsed_struct->name.str, decl->name.len, decl->name.str);
                                }
                                else
                                {
                                    fputc('0', output_file);
                                }

                                if (dimensions_index < ArrayCount(decl->array_dimensions_has_compile_time_size) - 1)
                                    fputs(", ", output_file);
                            }
                            fprintf(output_file, "}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,\n");

                            if (member->metadata_list)
                            {
                                FprintfIndented(output_file, indent_level, "DqnInspectMetadata_%.*s_%.*s, ", parsed_struct->name.len, parsed_struct->name.str, decl->name.len, decl->name.str);
                                fprintf(output_file, "ARRAY_COUNT(DqnInspectMetadata_%.*s_%.*s),", parsed_struct->name.len, parsed_struct->name.str, decl->name.len, decl->name.str);
                            }
                            else
                            {
                                FprintfIndented(output_file, indent_level, "nullptr, 0,");
                            }
                            fprintf(output_file, " // metadata array\n");

                            indent_level--;
                            FprintfIndented(output_file, indent_level, "},\n");
                        }

                        indent_level--;
                        FprintfIndented(output_file, indent_level, "};\n\n");
                    }

                    //
                    // NOTE: Write DqnInspect_Struct Definition
                    //
                    {
                        FprintfIndented(output_file, indent_level, "DqnInspectStruct const DqnInspect_%.*s_Struct =\n{\n", parsed_struct->name.len, parsed_struct->name.str);
                        indent_level++;

                        FprintfIndented(output_file, indent_level, "STR_AND_LEN(\"%.*s\"),\n", parsed_struct->name.len, parsed_struct->name.str);
                        FprintfIndented(output_file, indent_level, "DqnInspect_%.*s_Members, // members\n", parsed_struct->name.len, parsed_struct->name.str);
                        FprintfIndented(output_file, indent_level, "ARRAY_COUNT(DqnInspect_%.*s_Members) // members_len\n", parsed_struct->name.len, parsed_struct->name.str);

                        indent_level--;
                        FprintfIndented(output_file, indent_level, "};\n\n");
                        assert(indent_level == 0);
                    }

                    //
                    // NOTE: Write DqnInspect_Struct getter
                    //
                    {
                        FprintfIndented(output_file, indent_level, "DqnInspectStruct const *DqnInspect_Struct(%.*s const *)\n", parsed_struct->name.len, parsed_struct->name.str);
                        FprintfIndented(output_file, indent_level, "{\n");
                        indent_level++;
                        FprintfIndented(output_file, indent_level, "DqnInspectStruct const *result = &DqnInspect_%.*s_Struct;\n", parsed_struct->name.len, parsed_struct->name.str);
                        FprintfIndented(output_file, indent_level, "return result;\n");
                        indent_level--;
                        FprintfIndented(output_file, indent_level, "}\n\n");
                    }
                }
                break;

                case ParsedCodeType::Function:
                {
                    ParsedFunction *parsed_func = &code.parsed_func;
                    if (parsed_func->has_comments)
                    {
                        for (LinkedList<Slice<char>> const *comment_link = &parsed_func->comments;
                             comment_link;
                             comment_link                                = comment_link->next)
                        {
                            Slice<char> const comment = comment_link->value;
                            if (comment.len == 0) fprintf(output_file, "//\n");
                            else                  fprintf(output_file, "// %.*s\n", comment.len, comment.str);
                        }
                    }

                    {
                        Slice<char> return_type            = parsed_func->return_type;
                        Slice<char> func_name              = parsed_func->name;

                        int spaces_remaining = parsing_results.max_func_return_type_decl_len - return_type.len;
                        FprintfIndented(output_file, indent_level, "%.*s ", return_type.len, return_type.str);
                        for (int i = 0; i < spaces_remaining; ++i) fprintf(output_file, " ");

                        spaces_remaining = parsing_results.max_func_name_decl_len - func_name.len;
                        FprintfIndented(output_file, indent_level, "%.*s", func_name.len, func_name.str);
                        for (int i = 0; i < spaces_remaining; ++i) fprintf(output_file, " ");

                        FprintfIndented(output_file, indent_level, "(");
                    }

                    for (CPPDeclLinkedList<CPPVariableDecl> *param_link = parsed_func->members; param_link; param_link = param_link->next)
                    {
                        CPPVariableDecl *decl = &param_link->value;
                        fprintf(output_file, "%.*s", decl->type.len, decl->type.str);

                        if (decl->name.len > 0)
                            fprintf(output_file, " %.*s", decl->name.len, decl->name.str);

                        if (decl->default_value.len > 0)
                            fprintf(output_file, " = %.*s", decl->default_value.len, decl->default_value.str);

                        if (param_link->next)
                            fprintf(output_file, ", ");
                    }
                    fprintf(output_file, ");\n");
                }
                break;
            }
        }

        fprintf(output_file, "\n#endif // DQN_INSPECT_%.*s\n\n",
            parsing_results.file_include_contents_hash_define_len,
            parsing_results.file_include_contents_hash_define);
    }

    //
    // NOTE: After all the definitions have been written, write the function
    //       that maps the member_type to the DqnInspectStruct's declared above
    //
    if (mode == InspectMode::All || mode == InspectMode::Code)
    {
        FprintfIndented(output_file, indent_level, "DqnInspectStruct const *DqnInspect_Struct(DqnInspectDeclType type)\n{\n");
        indent_level++;
        FprintfIndented(output_file, indent_level, "(void)type;\n");

        for (ParsingResult &parsing_results : parsing_results_per_file)
        {
            fprintf(output_file,
                    "#ifdef DQN_INSPECT_%.*s\n",
                    parsing_results.file_include_contents_hash_define_len,
                    parsing_results.file_include_contents_hash_define
                    );

            for (ParsedCode &code : parsing_results.code)
            {
                switch(code.type)
                {
                    case ParsedCodeType::Struct:
                    {
                        ParsedStruct const *parsed_struct = &code.parsed_struct;
                        FprintfIndented(output_file, indent_level, "if (type == DqnInspectDeclType::%.*s_) return &DqnInspect_%.*s_Struct;\n", parsed_struct->name.len, parsed_struct->name.str, parsed_struct->name.len, parsed_struct->name.str);
                    }
                    break;
                }
            }

            fprintf(output_file, "#endif // DQN_INSPECT_%.*s\n\n",
                parsing_results.file_include_contents_hash_define_len,
                parsing_results.file_include_contents_hash_define);
        }
        FprintfIndented(output_file, indent_level, "return nullptr;\n");
        indent_level--;
        FprintfIndented(output_file, indent_level, "}\n\n");

        fprintf(output_file,
                "\n#undef ARRAY_COUNT\n"
                "#undef CHAR_COUNT\n"
                "#undef STR_AND_LEN\n");
    }

    fclose(output_file);

    return 0;
}

#endif // DQN_INSPECT_EXECUTABLE_IMPLEMENTATION
#endif // DQN_INSPECT_H

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2019 doy-lee
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

#undef _CRT_SECURE_NO_WARNINGS
