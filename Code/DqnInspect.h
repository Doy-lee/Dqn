#ifndef DQN_INSPECT_H
#define DQN_INSPECT_H

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
// For full example see Data/DqnInspect_TestData.h

/*
DQN_INSPECT enum struct OpenGLShader
{
    Invalid,
    Rect DQN_INSPECT_META(VertexShaderFilePath = "Rect.vert", FragmentShaderFilePath = "Rect.frag"),
    Text DQN_INSPECT_META(VertexShaderFilePath = "Text.vert", FragmentShaderFilePath = "Text.frag"),
    Count,
};

struct V4 { float test; };
template <typename T, int Size>
struct Array { T data[Size]; };

DQN_INSPECT struct OpenGLState
{
    Array<V4, 32>   camera_matrixes;
    char          **bitmaps;
    int             shaders[(int)OpenGLShader::Count];
    void           *win32_handle;
    int             ebo DQN_INSPECT_META(DisplayName = "Element Buffer Object"), vbo, vao DQN_INSPECT_META(DisplayName = "Vertex Array Object", OpenGLVersion = "330");
    V4              draw_color DQN_INSPECT_META(DisplayName = "HelloWorld");
    int             draw_call_count;
};

//
// INSPECTION OUTPUT EXAMPLE
// =================================================================================================
// For full example see Data/DqnInspect_TestDataGenerated.cpp


// This is an auto generated file using Dqn_Inspect

//
// ..\Data\DqnInspect_TestData.h
//

#ifndef DQN_INSPECT_DQNINSPECT_TESTDATA_H
#define DQN_INSPECT_DQNINSPECT_TESTDATA_H

 // NOTE: These macros are undefined at the end of the file so to not pollute namespace
#define ARRAY_COUNT(array) sizeof(array)/sizeof((array)[0])
#define CHAR_COUNT(str) (ARRAY_COUNT(str) - 1)
#define STR_AND_LEN(str) str, CHAR_COUNT(str)

char const *DqnInspect_OpenGLShader_Strings[] = {"Invalid", "Rect", "Text", "Count", };

char const *DqnInspect_EnumString(OpenGLShader val)
{
    if (val == OpenGLShader::Invalid) return DqnInspect_OpenGLShader_Strings[0]; // "Invalid"
    if (val == OpenGLShader::Rect)    return DqnInspect_OpenGLShader_Strings[1]; // "Rect"
    if (val == OpenGLShader::Text)    return DqnInspect_OpenGLShader_Strings[2]; // "Text"
    if (val == OpenGLShader::Count)   return DqnInspect_OpenGLShader_Strings[3]; // "Count"
    return nullptr;
}

char const *DqnInspect_VertexShaderFilePathMetadata(OpenGLShader val)
{
    if (val == OpenGLShader::Rect) return "Rect.vert";
    if (val == OpenGLShader::Text) return "Text.vert";
    return nullptr;
}

char const *DqnInspect_FragmentShaderFilePathMetadata(OpenGLShader val)
{
    if (val == OpenGLShader::Rect) return "Rect.frag";
    if (val == OpenGLShader::Text) return "Text.frag";
    return nullptr;
}

DqnInspect_StructMemberMetadata const DqnInspect_OpenGLState_ebo_StructMemberMetadata[] =
{
    {
        DqnInspect_StructMemberMetadataType::String,
        STR_AND_LEN("DisplayName"), STR_AND_LEN("Element Buffer Object"),
    },
};

DqnInspect_StructMemberMetadata const DqnInspect_OpenGLState_vao_StructMemberMetadata[] =
{
    {
        DqnInspect_StructMemberMetadataType::String,
        STR_AND_LEN("DisplayName"), STR_AND_LEN("Vertex Array Object"),
    },
    {
        DqnInspect_StructMemberMetadataType::String,
        STR_AND_LEN("OpenGLVersion"), STR_AND_LEN("330"),
    },
};

DqnInspect_StructMemberMetadata const DqnInspect_OpenGLState_draw_color_StructMemberMetadata[] =
{
    {
        DqnInspect_StructMemberMetadataType::String,
        STR_AND_LEN("DisplayName"), STR_AND_LEN("HelloWorld"),
    },
};

DqnInspect_StructMember const DqnInspect_OpenGLState_StructMembers[] =
{
    {
        STR_AND_LEN("Array"), STR_AND_LEN("camera_matrixes"),
        STR_AND_LEN("V4, 3"), // template_expr
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("char"), STR_AND_LEN("bitmaps"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        2 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("shaders"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("void"), STR_AND_LEN("win32_handle"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("ebo"),
        nullptr, 0, // template_expr and template_expr_len
        DqnInspect_OpenGLState_ebo_StructMemberMetadata, 1,
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("vbo"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("vao"),
        nullptr, 0, // template_expr and template_expr_len
        DqnInspect_OpenGLState_vao_StructMemberMetadata, 2,
        0 // array_dimensions
    },
    {
        STR_AND_LEN("V4"), STR_AND_LEN("draw_color"),
        nullptr, 0, // template_expr and template_expr_len
        DqnInspect_OpenGLState_draw_color_StructMemberMetadata, 1,
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("draw_call_count"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
};

DqnInspect_Struct const DqnInspect_OpenGLState_Struct =
{
    STR_AND_LEN("OpenGLState"),
    DqnInspect_OpenGLState_StructMembers, // members
    ARRAY_COUNT(DqnInspect_OpenGLState_StructMembers) // members_len
};

DqnInspect_Struct const *DqnInspect_GetStruct(OpenGLState const *val)
{
    (void)val;
    DqnInspect_Struct const *result = &DqnInspect_OpenGLState_Struct;
    return result;
}

#undef ARRAY_COUNT
#undef CHAR_COUNT
#undef STR_AND_LEN
#endif // DQN_INSPECT_DQNINSPECT_TESTDATA_H
*/

#define DQN_INSPECT
#define DQN_INSPECT_META(...)
#define DQN_INSPECT_GENERATE_PROTOTYPE(...)

enum DqnInspect_StructMemberMetadataType { String, Int, Float };

struct DqnInspect_StructMemberMetadata
{
    DqnInspect_StructMemberMetadataType type;
    char const *key;
    int         key_len;
    char const *val_str;     // Metadata value is always inspected to a string
    int         val_str_len;

    // TODO(doyle): Implement
    union
    {
        int   int_val;
        float flt_val;
    };
};

struct DqnInspect_StructMember
{
    char const *type;
    int         type_len;
    char const *name;
    int         name_len;
    char const *template_expr;
    int         template_expr_len;

    DqnInspect_StructMemberMetadata const *metadata;
    int                                    metadata_len;

    int         array_dimensions; // > 0 means array
};

struct DqnInspect_Struct
{
    char const                    *name;
    int                            name_len;
    DqnInspect_StructMember const *members;
    int                            members_len;
};

#ifdef DQN_INSPECT_EXECUTABLE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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

#define STR_LITERAL(str) {str, CHAR_COUNT(str)}
struct StringLiteral
{
    StringLiteral() = default;
    StringLiteral(char *string, int string_len) : str(string) , len(string_len) { }
    char *str;
    int   len;
};

template <typename T>
struct LinkedList
{
    T           value;
    LinkedList *next;
};

#define INSPECT_MAX(a, b) ((a) > (b)) ? (a) : (b)
#define ARRAY_COUNT(str) sizeof(str)/sizeof(str[0])
#define CHAR_COUNT(str) (ARRAY_COUNT(str) - 1)
#define TOKEN_COMBINE(x, y) x ## y
#define TOKEN_COMBINE2(x, y) TOKEN_COMBINE(x, y)
#define DEFER auto const TOKEN_COMBINE(defer_lambda_, __COUNTER__) = DeferHelper() + [&]()

#define KILOBYTE(val) (1024ULL * (val))
#define MEGABYTE(val) (1024ULL * KILOBYTE(val))

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

char *StrFind(StringLiteral src, StringLiteral find)
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

b32 StrCmp(StringLiteral a, StringLiteral b)
{
    if (a.len != b.len)
        return false;
    b32 result = (memcmp(a.str, b.str, a.len) == 0);
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
    X(InspectCode, "DQN_INSPECT") \
    X(InspectGeneratePrototype, "DQN_INSPECT_GENERATE_PROTOTYPE") \
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

struct CPPInspectMetadataEntry
{
    StringLiteral key;
    StringLiteral value;
};

using CPPInspectMetadataArray = FixedArray<CPPInspectMetadataEntry, 8>;

template <typename T>
struct CPPDeclLinkedList
{
    CPPInspectMetadataArray metadata_array;
    T                       value;
    CPPDeclLinkedList      *next;
};

struct CPPVariableDecl
{
    StringLiteral type;
    StringLiteral name;
    StringLiteral template_expr;
    int           array_dimensions;
    StringLiteral default_value;
};

struct CPPTokeniser
{
    CPPToken  *tokens;
    int        tokens_index;
    int        tokens_len;
    int        tokens_max;

    int        spaces_per_indent;
    int        indent_level;
    FILE      *output_file;
};

void CPPTokeniser_SprintfToFile(CPPTokeniser *tokeniser, char const *fmt, ...)
{
    int const num_spaces = tokeniser->spaces_per_indent * tokeniser->indent_level;
    fprintf(tokeniser->output_file, "%*s", num_spaces, "");

    va_list va;
    va_start(va, fmt);
    vfprintf(tokeniser->output_file, fmt, va);
    va_end(va);
}

void CPPTokeniser_SprintfToFileNoIndenting(CPPTokeniser *tokeniser, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vfprintf(tokeniser->output_file, fmt, va);
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

void CPPTokeniser_SkipToIndentLevel(CPPTokeniser *tokeniser, int indent_level)
{
    assert(tokeniser->indent_level >= indent_level);
    if (tokeniser->indent_level == indent_level) return;

    for (CPPToken token = CPPTokeniser_NextToken(tokeniser);
         tokeniser->indent_level > indent_level && token.type != CPPTokenType::EndOfStream;
         )
    {
        token = CPPTokeniser_NextToken(tokeniser);
    }
}

bool CPPTokeniser_AcceptTokenIfType(CPPTokeniser *tokeniser, CPPTokenType type, CPPToken *token)
{
    CPPToken check = CPPTokeniser_PeekToken(tokeniser);
    bool result    = (check.type == type);
    if (result && token)
    {
        CPPTokeniser_NextToken(tokeniser);
        *token = check;
    }

    return result;
}

//
// CPP Parsing Helpers
//

b32 IsIdentifierToken(CPPToken token, StringLiteral expect_str)
{
    b32 result = (token.type == CPPTokenType::Identifier) && (StrCmp(StringLiteral(token.str, token.len), expect_str));
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

CPPInspectMetadataArray ParseCPPInspectMeta(CPPTokeniser *tokeniser)
{
    CPPInspectMetadataArray result = {};
    CPPToken token                 = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::Identifier) || !IsIdentifierToken(token, STR_LITERAL("DQN_INSPECT_META")))
        return result;

    token = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::OpenParen)) return result;

    for (token       = CPPTokeniser_NextToken(tokeniser);
         token.type != CPPTokenType::EndOfStream && token.type != CPPTokenType::CloseParen;
         token       = CPPTokeniser_NextToken(tokeniser))
    {
        if (token.type == CPPTokenType::Identifier)
        {
            auto metadata_key   = StringLiteral(token.str, token.len);
            CPPToken peek_token = CPPTokeniser_PeekToken(tokeniser);
            if (!ExpectToken(peek_token, CPPTokenType::Equals))
                continue;

            token      = CPPTokeniser_NextToken(tokeniser);
            peek_token = CPPTokeniser_PeekToken(tokeniser);

            if (peek_token.type == CPPTokenType::String || peek_token.type == CPPTokenType::Identifier)
            {
                token = CPPTokeniser_NextToken(tokeniser);
                if (IsIdentifierToken(token, STR_LITERAL("nullptr"))) continue;

                CPPInspectMetadataEntry *entry = FixedArray_Make(&result, 1);
                entry->key                     = metadata_key;
                entry->value                   = StringLiteral(token.str, token.len);
            }
        }
    }

    while (token.type != CPPTokenType::EndOfStream && token.type != CPPTokenType::CloseParen)
        token = CPPTokeniser_NextToken(tokeniser);

    return result;
}

struct CPPDeclToMetaValue
{
    StringLiteral cpp_decl;
    StringLiteral value;
};

// i.e. DataType cpp_decl DQN_INSPECT_META(key = value, key2 = value2, ...);
struct MetadataEntry
{
    StringLiteral                      key;
    FixedArray<CPPDeclToMetaValue, 32> cpp_decl_to_val;
};

void ParseCPPEnum(CPPTokeniser *tokeniser)
{
    CPPToken token = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::Identifier) || !IsIdentifierToken(token, STR_LITERAL("enum")))
        return;

    token                       = CPPTokeniser_NextToken(tokeniser);
    b32 enum_is_struct_or_class = false;

    if (IsIdentifierToken(token, STR_LITERAL("class")) ||
        IsIdentifierToken(token, STR_LITERAL("struct")))
    {
        enum_is_struct_or_class = true;
        token                   = CPPTokeniser_NextToken(tokeniser);
    }

    if (!ExpectToken(token, CPPTokenType::Identifier))
        return;

    int original_indent_level = tokeniser->indent_level;
    CPPToken const enum_name  = token;
    token                     = CPPTokeniser_NextToken(tokeniser);

    if (!ExpectToken(token, CPPTokenType::LeftBrace))
        return;

    CPPDeclLinkedList<StringLiteral> *enum_members = nullptr;
    MemArenaScopedRegion mem_region = MemArena_MakeScopedRegion(&global_main_arena);

    {
        CPPDeclLinkedList<StringLiteral> *link_iterator = nullptr;
        for (token       = CPPTokeniser_NextToken(tokeniser);
             tokeniser->indent_level != original_indent_level && token.type != CPPTokenType::EndOfStream;
             token       = CPPTokeniser_NextToken(tokeniser))
        {
            if (token.type == CPPTokenType::Identifier)
            {
                auto *link = MEM_ARENA_ALLOC_STRUCT(&global_main_arena, CPPDeclLinkedList<StringLiteral>);
                *link      = {};
                if (!link_iterator) enum_members        = link; // Set members to first linked list entry
                else                link_iterator->next = link;
                link_iterator = link;

                link->value         = StringLiteral(token.str, token.len);
                CPPToken peek_token = CPPTokeniser_PeekToken(tokeniser);
                if (IsIdentifierToken(peek_token, STR_LITERAL("DQN_INSPECT_META")))
                {
                    link->metadata_array = ParseCPPInspectMeta(tokeniser);
                }
            }
        }
    }

    //
    // Write Stringified Enum Array
    //
    {
        CPPTokeniser_SprintfToFile(tokeniser, "char const *DqnInspect_%.*s_Strings[] = {", enum_name.len, enum_name.str);
        tokeniser->indent_level++;
        for (CPPDeclLinkedList<StringLiteral> *link = enum_members; link; link = link->next)
        {
            StringLiteral enum_value = link->value;
            CPPTokeniser_SprintfToFileNoIndenting(tokeniser, "\"%.*s\", ", enum_value.len, enum_value.str);
        }

        tokeniser->indent_level--;
        CPPTokeniser_SprintfToFile(tokeniser, "};\n\n");
    }

    //
    // Write InspectEnumString Function
    //
    {
        CPPTokeniser_SprintfToFile(tokeniser, "char const *DqnInspect_EnumString(%.*s val)\n{\n", enum_name.len, enum_name.str);
        tokeniser->indent_level++;
        DEFER
        {
            CPPTokeniser_SprintfToFile(tokeniser, "return nullptr;\n");
            tokeniser->indent_level--;
            CPPTokeniser_SprintfToFile(tokeniser, "}\n\n");
        };

        struct SourceCode
        {
            StringLiteral decl;
            StringLiteral enum_value;
        };

        LinkedList<SourceCode> src_code = {};
        int longest_decl_len            = 0;
        {
            LinkedList<SourceCode> *curr_src_code = nullptr;
            char const *fmt = (enum_is_struct_or_class) ? "if (val == %.*s::%.*s) " : "if (val == %.*s) ";
            for (CPPDeclLinkedList<StringLiteral> *link = enum_members; link; link = link->next)
            {
                if (!curr_src_code) curr_src_code = &src_code;
                else
                {
                    curr_src_code->next = static_cast<LinkedList<SourceCode> *>( MemArena_Alloc(&global_main_arena, sizeof(*curr_src_code)));
                    curr_src_code       = curr_src_code->next;
                }

                StringLiteral enum_value = link->value;
                int required_len         = 0;

                if (enum_is_struct_or_class) required_len = snprintf(nullptr, 0, fmt, enum_name.len, enum_name.str, enum_value.len, enum_value.str) + 1;
                else                         required_len = snprintf(nullptr, 0, fmt, enum_value.len, enum_value.str) + 1;

                longest_decl_len                = INSPECT_MAX(longest_decl_len, required_len);
                curr_src_code->value.decl.str   = MEM_ARENA_ALLOC_ARRAY(&global_main_arena, char, required_len);
                curr_src_code->value.decl.len   = required_len;
                curr_src_code->value.enum_value = enum_value;

                if (enum_is_struct_or_class) snprintf(curr_src_code->value.decl.str, curr_src_code->value.decl.len, fmt, enum_name.len, enum_name.str, enum_value.len, enum_value.str);
                else                         snprintf(curr_src_code->value.decl.str, curr_src_code->value.decl.len, fmt, enum_value.len, enum_value.str);
            }
            curr_src_code->next = nullptr;
        }

        int enum_index = 0;
        for (LinkedList<SourceCode> *src_code_ptr = &src_code;
             src_code_ptr;
             src_code_ptr = src_code_ptr->next, ++enum_index)
        {
            StringLiteral enum_value = src_code_ptr->value.enum_value;
            int padding              = longest_decl_len - src_code_ptr->value.decl.len;
            CPPTokeniser_SprintfToFile(tokeniser, "%.*s%*s", src_code_ptr->value.decl.len, src_code_ptr->value.decl.str, padding, "");
            CPPTokeniser_SprintfToFileNoIndenting(tokeniser,
                                                  "return DqnInspect_%.*s_Strings[%d]; // \"%.*s\"\n",
                                                  enum_name.len, enum_name.str,
                                                  enum_index,
                                                  enum_value.len, enum_value.str);
        }
    }

    //
    // Write User Annotated Metadata Getter Functions
    //
    {
        FixedArray<MetadataEntry, 32> metadata_entries = {};
        for (CPPDeclLinkedList<StringLiteral> *link = enum_members;
             link;
             link = link->next)
        {
            for (CPPInspectMetadataEntry const &inspect_entry : link->metadata_array)
            {
                MetadataEntry *metadata_entry_to_append_to = nullptr;
                for (MetadataEntry &check_metadata_entry : metadata_entries)
                {
                    if (StrCmp(check_metadata_entry.key, inspect_entry.key))
                    {
                        metadata_entry_to_append_to = &check_metadata_entry;
                        break;
                    }
                }

                if (!metadata_entry_to_append_to)
                {
                    metadata_entry_to_append_to      = FixedArray_Make(&metadata_entries, 1);
                    metadata_entry_to_append_to->key = inspect_entry.key;
                }

                CPPDeclToMetaValue decl_to_val = {};
                decl_to_val.cpp_decl       = StringLiteral(link->value.str, link->value.len);
                decl_to_val.value          = inspect_entry.value;
                FixedArray_Add(&metadata_entry_to_append_to->cpp_decl_to_val, decl_to_val);
            }
        }

        for (MetadataEntry const &metadata : metadata_entries)
        {
            CPPTokeniser_SprintfToFile(tokeniser,
                                       "char const *DqnInspect_%.*sMetadata(%.*s val)\n{\n",
                                       metadata.key.len, metadata.key.str,
                                       enum_name.len, enum_name.str);

            tokeniser->indent_level++;
            DEFER
            {
                CPPTokeniser_SprintfToFile(tokeniser, "return nullptr;\n");
                tokeniser->indent_level--;
                CPPTokeniser_SprintfToFile(tokeniser, "}\n\n");
            };

            for (CPPDeclToMetaValue const &decl_to_val : metadata.cpp_decl_to_val)
            {
                StringLiteral const *cpp_decl = &decl_to_val.cpp_decl;
                StringLiteral const *value    = &decl_to_val.value;

                if (enum_is_struct_or_class)
                {
                    CPPTokeniser_SprintfToFile(tokeniser,
                                               "if (val == %.*s::%.*s) ",
                                               enum_name.len, enum_name.str,
                                               cpp_decl->len, cpp_decl->str);
                }
                else
                {
                    CPPTokeniser_SprintfToFile(tokeniser,
                                               "if (val == %.*s) ",
                                               cpp_decl->len, cpp_decl->str);
                }
                CPPTokeniser_SprintfToFileNoIndenting(tokeniser, "return \"%.*s\";\n", value->len, value->str);
            }
        }
    }
}

int ConsumeAsterisks(CPPTokeniser *tokeniser)
{
    int result = 0;
    for (CPPToken peek_token = CPPTokeniser_PeekToken(tokeniser);
         peek_token.type == CPPTokenType::Asterisks;
         ++result)
    {
        CPPTokeniser_NextToken(tokeniser);
        peek_token = CPPTokeniser_PeekToken(tokeniser);
    }

    return result;
}

b32 ConsumeConstIdentifier(CPPTokeniser *tokeniser)
{
    b32 result = false;
    for (CPPToken peek_token = CPPTokeniser_PeekToken(tokeniser);
         IsIdentifierToken(peek_token, STR_LITERAL("const"));
         peek_token = CPPTokeniser_PeekToken(tokeniser))
    {
        result = true;
        CPPTokeniser_NextToken(tokeniser);
    }

    return result;
}

b32 ParseCPPVariableType(CPPTokeniser *tokeniser, StringLiteral *type)
{
    CPPToken token = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::Identifier))
        return false;

    char *var_type_start = token.str;
    ConsumeConstIdentifier(tokeniser);

    //
    // Parse Template If Any
    //
    StringLiteral template_expr = {};
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

                template_expr.str = expr_start;
                template_expr.len = expr_len;
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
};

// NOTE(doyle): Doesn't parse the ending semicolon so we can reuse this function for parsing function arguments
CPPDeclLinkedList<CPPVariableDecl> *ParseCPPTypeAndVariableDecl(CPPTokeniser *tokeniser, b32 parse_function_param)
{
    CPPDeclLinkedList<CPPVariableDecl> *result        = nullptr;
    CPPDeclLinkedList<CPPVariableDecl> *link_iterator = nullptr;

    CPPToken token = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::Identifier))
        return result;

    ConsumeConstIdentifier(tokeniser);
    CPPToken variable_type = token;
    for (int total_asterisks_count = 0;;)
    {
        CPPToken peek_token                  = {};
        StringLiteral variable_template_expr = {};
        if (CPPTokeniser_AcceptTokenIfType(tokeniser, CPPTokenType::LessThan, &peek_token))
        {
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

                variable_template_expr.str = expr_start;
                variable_template_expr.len = expr_len;
            }
        }

        total_asterisks_count = ConsumeAsterisks(tokeniser);
        if (ConsumeConstIdentifier(tokeniser))
        {
            total_asterisks_count += ConsumeAsterisks(tokeniser);
            ConsumeConstIdentifier(tokeniser);
        }

        peek_token             = CPPTokeniser_PeekToken(tokeniser);
        CPPToken variable_name = peek_token;
        if (variable_name.type != CPPTokenType::Identifier)
            break;

        // Allocate A Member Declaration
        auto *link = MEM_ARENA_ALLOC_STRUCT(&global_main_arena, CPPDeclLinkedList<CPPVariableDecl>);
        *link      = {};
        if (!result) result              = link; // Set result to first linked list entry
        else         link_iterator->next = link;
        link_iterator = link;

        link->value.type             = StringLiteral(variable_type.str, variable_type.len);
        link->value.name             = StringLiteral(variable_name.str, variable_name.len);
        link->value.template_expr    = variable_template_expr;
        link->value.array_dimensions = total_asterisks_count;

        CPPTokeniser_NextToken(tokeniser);
        for (peek_token = CPPTokeniser_PeekToken(tokeniser);
             peek_token.type == CPPTokenType::LeftSqBracket && peek_token.type != CPPTokenType::EndOfStream;
             peek_token = CPPTokeniser_NextToken(tokeniser))
        {
            // TODO(doyle): Parsing array size is difficult if it's an expression, so maybe don't do it at all
            ++link->value.array_dimensions;
            while (token.type != CPPTokenType::RightSqBracket && token.type != CPPTokenType::EndOfStream)
                token = CPPTokeniser_NextToken(tokeniser);
        }

        if (IsIdentifierToken(peek_token, STR_LITERAL("DQN_INSPECT_META")))
        {
            link->metadata_array = ParseCPPInspectMeta(tokeniser);
            peek_token           = CPPTokeniser_PeekToken(tokeniser);
        }

        if (peek_token.type == CPPTokenType::Comma)
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

void ParseCPPStruct(CPPTokeniser *tokeniser)
{
    CPPToken token = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::Identifier) ||
        (!IsIdentifierToken(token, STR_LITERAL("struct")) && !IsIdentifierToken(token, STR_LITERAL("class"))))
        return;

    int const original_indent_level = tokeniser->indent_level;
    token                           = CPPTokeniser_NextToken(tokeniser);

    StringLiteral name = STR_LITERAL("");
    if (token.type != CPPTokenType::LeftBrace)
    {
        if (!ExpectToken(token, CPPTokenType::Identifier)) return;
        name = StringLiteral(token.str, token.len);
    }

    CPPDeclLinkedList<CPPVariableDecl> *struct_members = nullptr;
    MemArenaScopedRegion mem_region                    = MemArena_MakeScopedRegion(&global_main_arena);

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
                if (!struct_members) struct_members = link;
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
    if (name.len == 0)
        return;

    //
    // Write DqnInspect_StructMemberMetadata Definition
    //
    for (CPPDeclLinkedList<CPPVariableDecl> const *member = struct_members; member; member = member->next)
    {
        CPPVariableDecl const *decl = &member->value;
        if (member->metadata_array.len <= 0)
            continue;

        CPPTokeniser_SprintfToFile(
            tokeniser,
            "DqnInspect_StructMemberMetadata const DqnInspect_%.*s_%.*s_StructMemberMetadata[] =\n{\n",
            name.len,
            name.str,
            decl->name.len,
            decl->name.str);

        tokeniser->indent_level++;
        for (CPPInspectMetadataEntry const &entry : member->metadata_array)
        {
            CPPTokeniser_SprintfToFile(tokeniser, "{\n");
            tokeniser->indent_level++;
            CPPTokeniser_SprintfToFile(tokeniser, "DqnInspect_StructMemberMetadataType::String,\n");

            // metadata->key
            CPPTokeniser_SprintfToFile(tokeniser, "STR_AND_LEN(\"%.*s\"), ", entry.key.len, entry.key.str);

            // metadata->value
            CPPTokeniser_SprintfToFileNoIndenting(tokeniser, "STR_AND_LEN(\"%.*s\"),\n", entry.value.len, entry.value.str);

            tokeniser->indent_level--;
            CPPTokeniser_SprintfToFile(tokeniser, "},\n");
        }
        tokeniser->indent_level--;
        CPPTokeniser_SprintfToFile(tokeniser, "};\n\n");
    }

    //
    // Write DqnInspect_StructMembers Definition
    //
    {
        CPPTokeniser_SprintfToFile(tokeniser, "DqnInspect_StructMember const DqnInspect_%.*s_StructMembers[] =\n{\n", name.len, name.str);
        tokeniser->indent_level++;

        for (CPPDeclLinkedList<CPPVariableDecl> const *member = struct_members; member; member = member->next)
        {
            CPPVariableDecl const *decl = &member->value;
            CPPTokeniser_SprintfToFile(tokeniser, "{\n");
            tokeniser->indent_level++;

            CPPTokeniser_SprintfToFile(tokeniser, "STR_AND_LEN(\"%.*s\"), ", decl->type.len, decl->type.str);
            CPPTokeniser_SprintfToFileNoIndenting(tokeniser, "STR_AND_LEN(\"%.*s\"),\n", decl->name.len, decl->name.str);

            if (decl->template_expr.len <= 0)
                CPPTokeniser_SprintfToFile(tokeniser, "nullptr, 0, // template_expr and template_expr_len\n");
            else
                CPPTokeniser_SprintfToFile(tokeniser, "STR_AND_LEN(\"%.*s\"), // template_expr\n", decl->template_expr.len, decl->template_expr.str);

            if (member->metadata_array.len <= 0) CPPTokeniser_SprintfToFile(tokeniser, "nullptr, 0, // metadata and metadata_len\n");
            else                                 CPPTokeniser_SprintfToFile(tokeniser, "DqnInspect_%.*s_%.*s_StructMemberMetadata, %d,\n", name.len, name.str, decl->name.len, decl->name.str, member->metadata_array.len);
            CPPTokeniser_SprintfToFile(tokeniser, "%d // array_dimensions\n", decl->array_dimensions);

            tokeniser->indent_level--;
            CPPTokeniser_SprintfToFile(tokeniser, "},\n");
        }

        tokeniser->indent_level--;
        CPPTokeniser_SprintfToFile(tokeniser, "};\n\n");
    }

    //
    // Write DqnInspect_Struct Definition
    //
    {
        CPPTokeniser_SprintfToFile(tokeniser, "DqnInspect_Struct const DqnInspect_%.*s_Struct =\n{\n", name.len, name.str);
        tokeniser->indent_level++;

        CPPTokeniser_SprintfToFile(tokeniser, "STR_AND_LEN(\"%.*s\"),\n", name.len, name.str);
        CPPTokeniser_SprintfToFile(tokeniser, "DqnInspect_%.*s_StructMembers, // members\n", name.len, name.str);
        CPPTokeniser_SprintfToFile(tokeniser, "ARRAY_COUNT(DqnInspect_%.*s_StructMembers) // members_len\n", name.len, name.str);

        tokeniser->indent_level--;
        CPPTokeniser_SprintfToFile(tokeniser, "};\n\n");
        assert(tokeniser->indent_level == 0);
    }

    //
    // Write DqnInspect_Struct getter
    //
    {
        CPPTokeniser_SprintfToFile(tokeniser, "DqnInspect_Struct const *DqnInspect_GetStruct(%.*s const *val)\n", name.len, name.str);
        CPPTokeniser_SprintfToFile(tokeniser, "{\n");
        tokeniser->indent_level++;
        CPPTokeniser_SprintfToFile(tokeniser, "(void)val;\n");
        CPPTokeniser_SprintfToFile(tokeniser, "DqnInspect_Struct const *result = &DqnInspect_%.*s_Struct;\n", name.len, name.str);
        CPPTokeniser_SprintfToFile(tokeniser, "return result;\n");
        tokeniser->indent_level--;
        CPPTokeniser_SprintfToFile(tokeniser, "}\n\n");
    }
}

void SkipFunctionParam(CPPTokeniser *tokeniser)
{
    CPPToken token = CPPTokeniser_PeekToken(tokeniser);
    while (token.type != CPPTokenType::EndOfStream && token.type != CPPTokenType::Comma &&
           token.type != CPPTokenType::CloseParen)
    {
        CPPTokeniser_NextToken(tokeniser);
        token = CPPTokeniser_PeekToken(tokeniser);
    }
}

void ParseCPPInspectPrototype(CPPTokeniser *tokeniser)
{
    CPPToken token = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::InspectGeneratePrototype))
        return;

    struct FunctionDefaultParam
    {
        StringLiteral name;
        StringLiteral value;
    };

    LinkedList<FunctionDefaultParam> *default_param_list = nullptr;
    MemArenaScopedRegion mem_region                      = MemArena_MakeScopedRegion(&global_main_arena);
    {
        LinkedList<FunctionDefaultParam> *link_iterator = nullptr;
        token = CPPTokeniser_PeekToken(tokeniser);
        if (!ExpectToken(token, CPPTokenType::OpenParen)) return;
        token = CPPTokeniser_NextToken(tokeniser);

        for (token = CPPTokeniser_NextToken(tokeniser);
             token.type != CPPTokenType::CloseParen && token.type != CPPTokenType::EndOfStream;
             token = CPPTokeniser_NextToken(tokeniser))
        {
            if (token.type == CPPTokenType::Comma)
                continue;

            StringLiteral default_param_name = StringLiteral(token.str, token.len);
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

            token                     = CPPTokeniser_NextToken(tokeniser);
            char *default_param_start = token.str;

            // NOTE(doyle): Include the quotes in the param value
            CPPToken prev_token = CPPTokeniser_PrevToken(tokeniser);
            if (prev_token.type == CPPTokenType::String)
                default_param_start--;

            SkipFunctionParam(tokeniser);
            CPPToken peek_token = CPPTokeniser_PeekToken(tokeniser);
            if (peek_token.type != CPPTokenType::Comma && peek_token.type != CPPTokenType::CloseParen)
                continue;

            char *default_param_end = peek_token.str;

            auto *link = MEM_ARENA_ALLOC_STRUCT(&global_main_arena, LinkedList<FunctionDefaultParam>);
            *link      = {};
            if (!default_param_list) default_param_list = link;
            else link_iterator->next                    = link;
            link_iterator = link;

            link->value.name  = default_param_name;
            link->value.value = StringLiteral(default_param_start, static_cast<int>(default_param_end - default_param_start));
        }
    }

    struct FunctionParam
    {
        StringLiteral type;
        StringLiteral name;

         // NOTE(doyle): This is resolved after function parsing, iterate over
         // the default params specified in the macro and match them to the
         // first param that has the same name
        StringLiteral default_value;
    };

    StringLiteral return_type   = {};
    StringLiteral function_name = {};
    // Grab return type token
    {
        token = CPPTokeniser_PeekToken(tokeniser);
        if (!ExpectToken(token, CPPTokenType::Identifier))  return;
        if (!ParseCPPVariableType(tokeniser, &return_type)) return;
    }

    // Grab function name token
    {
        token = CPPTokeniser_PeekToken(tokeniser);
        if (!ExpectToken(token, CPPTokenType::Identifier)) return;

        char *name_start = token.str;
        while (token.type != CPPTokenType::OpenParen && token.type != CPPTokenType::EndOfStream)
        {
            CPPTokeniser_NextToken(tokeniser);
            token = CPPTokeniser_PeekToken(tokeniser);
        }

        if (!ExpectToken(token, CPPTokenType::OpenParen)) return;
        char *name_end = token.str;
        function_name        = StringLiteral(name_start, static_cast<int>(name_end - name_start));
    }

    token = CPPTokeniser_PeekToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::OpenParen))
        return;

    CPPDeclLinkedList<CPPVariableDecl> *param_list = nullptr;
    {
        CPPDeclLinkedList<CPPVariableDecl> *link_iterator = nullptr;
        token = CPPTokeniser_NextToken(tokeniser);
        for (token = CPPTokeniser_NextToken(tokeniser);
             token.type != CPPTokenType::CloseParen && token.type != CPPTokenType::EndOfStream;
             token = CPPTokeniser_NextToken(tokeniser))
        {
            if (token.type == CPPTokenType::Identifier)
            {
                CPPTokeniser_RewindToken(tokeniser);
                CPPDeclLinkedList<CPPVariableDecl> *link = ParseCPPTypeAndVariableDecl(tokeniser, true);
                if (!param_list)    param_list = link;
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

    for (LinkedList<FunctionDefaultParam> const *default_link = default_param_list; default_link; default_link = default_link->next)
    {
        FunctionDefaultParam const *default_param = &default_link->value;
        for (CPPDeclLinkedList<CPPVariableDecl> *param_link = param_list; param_link; param_link = param_link->next)
        {
            CPPVariableDecl *decl = &param_link->value;
            if (StrCmp(decl->name, default_param->name))
            {
                decl->default_value = default_param->value;
                break;
            }
        }
    }

    if (return_type.str[return_type.len-1] == '*') // NOTE(doyle): Align the pointer to the name
        CPPTokeniser_SprintfToFile(tokeniser, "%.*s%.*s(", return_type.len, return_type.str, function_name.len, function_name.str);
    else
        CPPTokeniser_SprintfToFile(tokeniser, "%.*s %.*s(", return_type.len, return_type.str, function_name.len, function_name.str);

    for (CPPDeclLinkedList<CPPVariableDecl> *param_link = param_list; param_link; param_link = param_link->next)
    {
        // TODO(doyle): HACK. We should parse ptrs into the CPPVariableDecl, fixed size arrays into the name and const-ness into the type
        CPPVariableDecl *decl        = &param_link->value;
        StringLiteral *type          = &decl->type;
        char *type_end = (decl->template_expr.len > 0) ? decl->template_expr.str + decl->template_expr.len + 1 // +1 for the ending ">" on the template
                                                       : type->str + type->len;

        StringLiteral *name = &decl->name;
        char *name_start    = type_end + 1;
        char *name_end      = name->str + name->len;
        auto hack_decl_name = StringLiteral(name_start, static_cast<int>(name_end - name_start));

        CPPTokeniser_SprintfToFileNoIndenting(tokeniser, "%.*s", type->len, type->str);
        if (decl->template_expr.len > 0)
            CPPTokeniser_SprintfToFileNoIndenting(tokeniser, "<%.*s>", decl->template_expr.len, decl->template_expr.str);
        CPPTokeniser_SprintfToFileNoIndenting(tokeniser, " %.*s", hack_decl_name.len, hack_decl_name.str);

        if (decl->default_value.len > 0)
            CPPTokeniser_SprintfToFileNoIndenting(tokeniser, " = %.*s", decl->default_value.len, decl->default_value.str);

        if (param_link->next)
            CPPTokeniser_SprintfToFileNoIndenting(tokeniser, ", ", return_type.len, return_type.str, function_name.len, function_name.str);
    }
    CPPTokeniser_SprintfToFileNoIndenting(tokeniser, ");\n");
}

enum struct InspectMode
{
    All,
    Code,
    GenerateProtypes
};

char *EnumOrStructOrFunctionLexer(CPPTokeniser *tokeniser, char *ptr, b32 lexing_function)
{
    int indent_level                = 0;
    bool started_lexing_brace_scope = false;
    bool started_lexing_function    = false;
    int open_paren_level            = 0;
    int close_paren_level           = 0;
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
            case '(': { token->type = CPPTokenType::OpenParen;   started_lexing_function = true; open_paren_level++; } break;
            case ')': { token->type = CPPTokenType::CloseParen;  close_paren_level++; } break;
            case ',': { token->type = CPPTokenType::Comma;       } break;
            case ';': { token->type = CPPTokenType::SemiColon;   } break;
            case '=': { token->type = CPPTokenType::Equals;      } break;
            case '<': { token->type = CPPTokenType::LessThan;    } break;
            case '>': { token->type = CPPTokenType::GreaterThan; } break;
            case ':': { token->type = CPPTokenType::Colon;       } break;
            case '*': { token->type = CPPTokenType::Asterisks;   } break;
            case '/':
            {
                token->type = CPPTokenType::FwdSlash;
                if (ptr[0] == '/' || ptr[0] == '*')
                {
                    token->type = CPPTokenType::Comment;
                    if (ptr[0] == '/')
                    {
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
                    if (CharIsDigit(ptr[0]))
                    {
                        while (CharIsDigit(ptr[0]) || ptr[0] == 'x' || ptr[0] == 'b' || ptr[0] == 'e' || ptr[0] == '.' || ptr[0] == 'f')
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

        if (token->len == 0)
        {
            *token = {};
            tokeniser->tokens_len--;
        }
        else
        {
            if (lexing_function)
            {
                if (started_lexing_function && open_paren_level == close_paren_level && open_paren_level == 2)
                    return ptr;
            }
            else
            {
                if (started_lexing_brace_scope && indent_level == 0)
                    return ptr;
            }
        }
    }

    return ptr;
}

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        fprintf(stdout,
                "Usage: %s [code|generate_prototypes] [<source code filename>, ...]\n"
                "Options: If ommitted, both modes are run\n"
                " code                Only generate the inspection data for structs/enums marked with DQN_INSPECT\n"
                " generate_prototypes Only generate the function prototypes for functions marked with DQN_INSPECT_GENERATE_PROTOTYPE\n",
                argv[0]);
        return 0;
    }

    usize starting_arg_index = 1;
    char const *mode_str     = argv[1];
    InspectMode mode         = InspectMode::All;
    if (strcmp(mode_str, "code") == 0)                     mode = InspectMode::Code;
    else if (strcmp(mode_str, "generate_prototypes") == 0) mode = InspectMode::GenerateProtypes;

    if (mode != InspectMode::All)
        starting_arg_index++;

    usize main_arena_mem_size = MEGABYTE(2);
    void *main_arena_mem      = malloc(main_arena_mem_size);
    global_main_arena         = MemArena_Init(main_arena_mem, main_arena_mem_size);
#if 0
    FILE *output_file         = fopen("DqnInspect_Generated.cpp", "w");
#else
    FILE *output_file         = stdout;
#endif


    fprintf(output_file,
            "// This is an auto generated file using Dqn_Inspect\n\n");

    for (usize arg_index = starting_arg_index; arg_index < argc; ++arg_index)
    {
        MemArenaScopedRegion mem_region = MemArena_MakeScopedRegion(&global_main_arena);
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

        int const file_name_len                   = (int)strlen(file_name);
        int file_include_contents_hash_define_len = 0;
        char *file_include_contents_hash_define   = nullptr;
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

            file_include_contents_hash_define     = extracted_file_name_buf + file_name_len - extracted_file_name_len;
            file_include_contents_hash_define_len = extracted_file_name_len;
        }

        if (mode == InspectMode::Code)
        {
            fprintf(output_file,
                    "//\n"
                    "// %s\n"
                    "//\n"
                    "\n"
                    "#ifndef DQN_INSPECT_%.*s\n"
                    "#define DQN_INSPECT_%.*s\n"
                    "\n"
                    " // NOTE: These macros are undefined at the end of the file so to not pollute namespace\n"
                    "#define ARRAY_COUNT(array) sizeof(array)/sizeof((array)[0])\n"
                    "#define CHAR_COUNT(str) (ARRAY_COUNT(str) - 1)\n"
                    "#define STR_AND_LEN(str) str, CHAR_COUNT(str)\n"
                    "\n",
                    file_name,
                    file_include_contents_hash_define_len,
                    file_include_contents_hash_define,
                    file_include_contents_hash_define_len,
                    file_include_contents_hash_define
                    );
        }
        else
        {
            fprintf(output_file,
                    "//\n"
                    "// %s\n"
                    "//\n"
                    "\n"
                    "#ifndef DQN_INSPECT_%.*s\n"
                    "#define DQN_INSPECT_%.*s\n"
                    "\n",
                    file_name,
                    file_include_contents_hash_define_len,
                    file_include_contents_hash_define,
                    file_include_contents_hash_define_len,
                    file_include_contents_hash_define
                    );
        }

        CPPTokeniser tokeniser      = {};
        tokeniser.spaces_per_indent = 4;
        tokeniser.output_file       = output_file;
        tokeniser.tokens_max        = 16384;
        tokeniser.tokens            = MEM_ARENA_ALLOC_ARRAY(&global_main_arena, CPPToken, tokeniser.tokens_max);

        StringLiteral const INSPECT_PROTOTYPE = STR_LITERAL("DQN_INSPECT_GENERATE_PROTOTYPE");
        StringLiteral const INSPECT_PREFIX    = STR_LITERAL("DQN_INSPECT");
        char *file_buf_end                    = file_buf + file_size;
        StringLiteral buffer                  = StringLiteral(file_buf, static_cast<int>(file_size));

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
            if (StrCmp(StringLiteral(ptr, INSPECT_PROTOTYPE.len), INSPECT_PROTOTYPE))
            {
                inspect_type = CPPTokenType::InspectGeneratePrototype;
            }

            if (inspect_type == CPPTokenType::InspectCode)
            {
                ptr += INSPECT_PREFIX.len;
                if (mode == InspectMode::GenerateProtypes) continue;
            }
            else
            {
                ptr += INSPECT_PROTOTYPE.len;
                if (mode == InspectMode::Code) continue;
            }

            CPPToken *inspect_token = CPPTokeniser_MakeToken(&tokeniser);
            inspect_token->type     = inspect_type;
            inspect_token->str      = marker_str;
            inspect_token->len      = marker_len;
            ptr = EnumOrStructOrFunctionLexer(&tokeniser, ptr, inspect_type == CPPTokenType::InspectGeneratePrototype);

        }

        CPPToken *sentinel = CPPTokeniser_MakeToken(&tokeniser);
        sentinel->type     = CPPTokenType::EndOfStream;

        for (CPPToken token = CPPTokeniser_PeekToken(&tokeniser);
             ;
             token          = CPPTokeniser_PeekToken(&tokeniser))
        {
            if (token.type == CPPTokenType::InspectCode || token.type == CPPTokenType::InspectGeneratePrototype)
            {
                if (token.type == CPPTokenType::InspectCode)
                {
                    token = CPPTokeniser_NextToken(&tokeniser);
                    token = CPPTokeniser_PeekToken(&tokeniser);

                    if (IsIdentifierToken(token, STR_LITERAL("enum")))
                    {
                        ParseCPPEnum(&tokeniser);
                    }
                    else if (IsIdentifierToken(token, STR_LITERAL("struct")) || IsIdentifierToken(token, STR_LITERAL("class")))
                    {
                        ParseCPPStruct(&tokeniser);
                    }
                }
                else
                {
                    ParseCPPInspectPrototype(&tokeniser);
                }
            }
            else
            {
                token = CPPTokeniser_NextToken(&tokeniser);
            }

            if (token.type == CPPTokenType::EndOfStream)
                break;
        }

        if (mode == InspectMode::Code)
        {
            fprintf(output_file,
                    "\n#undef ARRAY_COUNT\n"
                    "#undef CHAR_COUNT\n"
                    "#undef STR_AND_LEN\n"
                    "#endif // DQN_INSPECT_%.*s\n\n",
                    file_include_contents_hash_define_len,
                    file_include_contents_hash_define);
        }
        else
        {
            fprintf(output_file,
                    "\n#endif // DQN_INSPECT_%.*s\n\n",
                    file_include_contents_hash_define_len,
                    file_include_contents_hash_define);
        }
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
