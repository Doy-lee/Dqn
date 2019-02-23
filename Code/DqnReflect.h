#ifndef DQN_REFLECT_H
#define DQN_REFLECT_H

#define DQN_REFLECT
#define DQN_REFLECT_META(...)

enum DqnReflect_StructMemberMetadataType { String, Int, Float };

struct DqnReflect_StructMemberMetadata
{
    DqnReflect_StructMemberMetadataType type;
    char const *key;
    int         key_len;
    char const *val_str;     // Metadata value is always reflected to a string
    int         val_str_len;
};

struct DqnReflect_StructMember
{
    char const *type;
    int         type_len;
    char const *name;
    int         name_len;

    DqnReflect_StructMemberMetadata const *metadata;
    int                                    metadata_len;
};

struct DqnReflect_Struct
{
    char const                    *name;
    int                            name_len;
    DqnReflect_StructMember const *members;
    int                            members_len;
};

//
// HOW TO REFLECT ANNOTATED CODE
// Define in the preprocessor, DQN_REFLECT_IMPLEMENTATION and compile
// Dqn_Reflect.h to produce the metaprogram binary. Run the binary on the
// desired C++ source files. The reflected file will be printed out to stdout.
//

//
// DqnReflect.exe YourSourceCode.cpp > YourSourceCode_Reflected.cpp
//

//
// HOW TO ANNOTATE CODE
// This header file should be included in all files containing information you
// wish to reflect, you may reflect C-like data structures, such as in the
// following example.
//

//
// Your Source Code
//

#if 0
DQN_REFLECT enum struct OpenGLShader
{
    Invalid,
    Rect DQN_REFLECT_META(VertexShaderFilePath = "Rect.vert", FragmentShaderFilePath = "Rect.frag"),
    Text DQN_REFLECT_META(VertexShaderFilePath = "Text.vert",   FragmentShaderFilePath = "Text.frag"),
};
#endif

//
// Generated code in DqnReflect_Generated.cpp
//

#if 0
#ifndef DQN_REFLECT_GENERATED_H
#define DQN_REFLECT_GENERATED_H

//
// OpenGL.h
//

#if !defined(DQN_REFLECT_DISABLE_OPENGL_H)
char const *DqnReflect_OpenGLShader_Strings[] = {"Invalid", "Bitmap", "Text" };

char const *DqnReflect_EnumString(OpenGLShader val)
{
    if (val == OpenGLShader::Invalid) return DqnReflect_OpenGLShader_Strings[0]; // "Invalid"
    if (val == OpenGLShader::Rect)    return DqnReflect_OpenGLShader_Strings[1]; // "Rect"
    if (val == OpenGLShader::Bitmap)  return DqnReflect_OpenGLShader_Strings[2]; // "Bitmap"
    return nullptr;
}

char const *DqnReflect_VertexFilePathMetadata(OpenGLShader val)
{
    if (val == OpenGLShader::Rect)   return "Rect.vert";
    if (val == OpenGLShader::Bitmap) return "Bitmap.vert";
    return nullptr;
}

char const *DqnReflect_FragmentFilePathMetadata(OpenGLShader val)
{
    if (val == OpenGLShader::Rect)   return "Rect.frag";
    if (val == OpenGLShader::Bitmap) return "Bitmap.frag";
    return nullptr;
}

#endif // DQN_REFLECT_DISABLE_OPENGL_H

#endif // DQN_REFLECT_GENERATED_H
#endif

#ifdef DQN_REFLECT_IMPLEMENTATION
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

#define REFLECT_MAX(a, b) ((a) > (b)) ? (a) : (b)
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

struct CPPReflectMetadataEntry
{
    StringLiteral key;
    StringLiteral value;
};

using CPPReflectMetadataArray = FixedArray<CPPReflectMetadataEntry, 8>;

template <typename T>
struct CPPDeclLinkedList
{
    CPPReflectMetadataArray metadata_array;
    T                       value;
    CPPDeclLinkedList      *next;
};

struct CPPVariableDecl
{
    StringLiteral type;
    StringLiteral name;
    b32           is_array;
    int           array_len;
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

CPPReflectMetadataArray ParseCPPReflectMeta(CPPTokeniser *tokeniser)
{
    CPPReflectMetadataArray result = {};
    CPPToken token                 = CPPTokeniser_NextToken(tokeniser);
    if (!ExpectToken(token, CPPTokenType::Identifier) || !IsIdentifierToken(token, STR_LITERAL("DQN_REFLECT_META")))
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

                CPPReflectMetadataEntry *entry = FixedArray_Make(&result, 1);
                entry->key                     = metadata_key;
                entry->value                   = StringLiteral(token.str, token.len);
            }
        }
    }

    while (token.type != CPPTokenType::EndOfStream &&
           (token.type != CPPTokenType::Comma && token.type != CPPTokenType::SemiColon))
        token = CPPTokeniser_NextToken(tokeniser);

    return result;
}

struct CPPDeclToMetaValue
{
    StringLiteral cpp_decl;
    StringLiteral value;
};

// i.e. DataType cpp_decl DQN_REFLECT_META(key = value, key2 = value2, ...);
struct MetadataEntry
{
    StringLiteral                      key;
    FixedArray<CPPDeclToMetaValue, 32> cpp_decl_to_val;
};

void WriteMetadataReflectionMethods()
{
}

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
                if (IsIdentifierToken(peek_token, STR_LITERAL("DQN_REFLECT_META")))
                {
                    link->metadata_array = ParseCPPReflectMeta(tokeniser);
                }
            }
        }
    }

    //
    // Write Stringified Enum Array
    //
    {
        CPPTokeniser_SprintfToFile(tokeniser, "char const *DqnReflect_%.*s_Strings[] = {", enum_name.len, enum_name.str);
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
    // Write ReflectEnumString Function
    //
    {
        CPPTokeniser_SprintfToFile(tokeniser, "char const *DqnReflect_EnumString(%.*s val)\n{\n", enum_name.len, enum_name.str);
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

                longest_decl_len                = REFLECT_MAX(longest_decl_len, required_len);
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
                                                  "return DqnReflect_%.*s_Strings[%d]; // \"%.*s\"\n",
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
            for (CPPReflectMetadataEntry const &reflect_entry : link->metadata_array)
            {
                MetadataEntry *metadata_entry_to_append_to = nullptr;
                for (MetadataEntry &check_metadata_entry : metadata_entries)
                {
                    if (StrCmp(check_metadata_entry.key, reflect_entry.key))
                    {
                        metadata_entry_to_append_to = &check_metadata_entry;
                        break;
                    }
                }

                if (!metadata_entry_to_append_to)
                {
                    metadata_entry_to_append_to      = FixedArray_Make(&metadata_entries, 1);
                    metadata_entry_to_append_to->key = reflect_entry.key;
                }

                CPPDeclToMetaValue decl_to_val = {};
                decl_to_val.cpp_decl       = StringLiteral(link->value.str, link->value.len);
                decl_to_val.value          = reflect_entry.value;
                FixedArray_Add(&metadata_entry_to_append_to->cpp_decl_to_val, decl_to_val);
            }
        }

        for (MetadataEntry const &metadata : metadata_entries)
        {
            CPPTokeniser_SprintfToFile(tokeniser,
                                       "char const *DqnReflect_%.*sMetadata(%.*s val)\n{\n",
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

    int struct_members_len                             = 0;
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

#if 0
                int asterisks_count = 0;
                for (;; ++asterisks_count)
                {
                    Token peek_token = CPPTokeniser_PeekToken(tokeniser);
                    if (peek_token == CPPTokenType::Asterisks) CPPTokeniser_NextToken(tokeniser);
                    else                                       break;
                }
#endif

                CPPToken peek_token = CPPTokeniser_PeekToken(tokeniser);
                if (peek_token.type == CPPTokenType::Identifier)
                {
                    auto *link = MEM_ARENA_ALLOC_STRUCT(&global_main_arena, CPPDeclLinkedList<CPPVariableDecl>);
                    *link      = {};
                    if (!link_iterator) struct_members      = link; // Set struct_members to first linked list entry
                    else                link_iterator->next = link;
                    link_iterator = link;
                    struct_members_len++;

                    link->value.type = StringLiteral(token.str, token.len);
                    link->value.name = StringLiteral(peek_token.str, peek_token.len);

                    CPPTokeniser_NextToken(tokeniser);
                    peek_token = CPPTokeniser_PeekToken(tokeniser);
                    if (IsIdentifierToken(peek_token, STR_LITERAL("DQN_REFLECT_META")))
                        link->metadata_array = ParseCPPReflectMeta(tokeniser);
                }
            }
        }
    }

    // TODO(doyle): Don't support anonymous/nameless structs yet
    if (name.len == 0)
        return;

    //
    // Write DqnReflect_StructMemberMetadata Definition
    //
    for (CPPDeclLinkedList<CPPVariableDecl> const *member = struct_members; member; member = member->next)
    {
        CPPVariableDecl const *decl = &member->value;
        if (member->metadata_array.len <= 0)
            continue;

        CPPTokeniser_SprintfToFile(
            tokeniser,
            "DqnReflect_StructMemberMetadata const DqnReflect_%.*s_%.*s_StructMemberMetadata[] =\n{\n",
            name.len,
            name.str,
            decl->name.len,
            decl->name.str);

        tokeniser->indent_level++;
        for (CPPReflectMetadataEntry const &entry : member->metadata_array)
        {
            CPPTokeniser_SprintfToFile(tokeniser, "{\n");
            tokeniser->indent_level++;
            CPPTokeniser_SprintfToFile(tokeniser, "DqnReflect_StructMemberMetadataType::String,\n");

            // metadata->key
            CPPTokeniser_SprintfToFile(tokeniser, "STR_AND_LEN(\"%.*s\"),\n", entry.key.len, entry.key.str);

            // metadata->value
            CPPTokeniser_SprintfToFile(tokeniser, "STR_AND_LEN(\"%.*s\"),\n", entry.value.len, entry.value.str);

            tokeniser->indent_level--;
            CPPTokeniser_SprintfToFile(tokeniser, "},\n");
        }
        tokeniser->indent_level--;
        CPPTokeniser_SprintfToFile(tokeniser, "};\n\n");
    }

    //
    // Write DqnReflect_StructMembers Definition
    //
    {
        CPPTokeniser_SprintfToFile(tokeniser, "DqnReflect_StructMember const DqnReflect_%.*s_StructMembers[] =\n{\n", name.len, name.str);
        tokeniser->indent_level++;

        for (CPPDeclLinkedList<CPPVariableDecl> const *member = struct_members; member; member = member->next)
        {
            CPPVariableDecl const *decl = &member->value;
            CPPTokeniser_SprintfToFile(tokeniser, "{\n");
            tokeniser->indent_level++;

            CPPTokeniser_SprintfToFile(tokeniser, "STR_AND_LEN(\"%.*s\"),\n", decl->type.len, decl->type.str);
            CPPTokeniser_SprintfToFile(tokeniser, "STR_AND_LEN(\"%.*s\"),\n", decl->name.len, decl->name.str);

            if (member->metadata_array.len <= 0) CPPTokeniser_SprintfToFile(tokeniser, "nullptr, // metadata\n");
            else                                 CPPTokeniser_SprintfToFile(tokeniser, "DqnReflect_%.*s_%.*s_StructMemberMetadata,\n", name.len, name.str, decl->name.len, decl->name.str);
            CPPTokeniser_SprintfToFile(tokeniser, "%d // metadata_len\n", member->metadata_array.len);

            tokeniser->indent_level--;
            CPPTokeniser_SprintfToFile(tokeniser, "},\n");
        }

        tokeniser->indent_level--;
        CPPTokeniser_SprintfToFile(tokeniser, "};\n\n");
    }

    //
    // Write DqnReflect_Struct Definition
    //
    {
        CPPTokeniser_SprintfToFile(tokeniser, "DqnReflect_Struct const DqnReflect_%.*s_Struct =\n{\n", name.len, name.str);
        tokeniser->indent_level++;

        CPPTokeniser_SprintfToFile(tokeniser, "STR_AND_LEN(\"%.*s\"),\n", name.len, name.str);
        CPPTokeniser_SprintfToFile(tokeniser, "DqnReflect_%.*s_StructMembers, // members\n", name.len, name.str);
        CPPTokeniser_SprintfToFile(tokeniser, "%d // members_len\n", struct_members_len);

        tokeniser->indent_level--;
        CPPTokeniser_SprintfToFile(tokeniser, "};\n\n");
        assert(tokeniser->indent_level == 0);
    }

    //
    // Write DqnReflect_Struct getter
    //
    {
        CPPTokeniser_SprintfToFile(tokeniser, "DqnReflect_Struct const *DqnReflect_GetStruct(%.*s const *val)\n", name.len, name.str);
        CPPTokeniser_SprintfToFile(tokeniser, "{\n");
        tokeniser->indent_level++;
        CPPTokeniser_SprintfToFile(tokeniser, "(void)val;\n");
        CPPTokeniser_SprintfToFile(tokeniser, "DqnReflect_Struct const *result = &DqnReflect_%.*s_Struct;\n", name.len, name.str);
        CPPTokeniser_SprintfToFile(tokeniser, "return result;\n");
        tokeniser->indent_level--;
        CPPTokeniser_SprintfToFile(tokeniser, "}\n\n");
    }

    //
    // Write User Annotated Metadata Getter Functions
    //
    {
    }

    int break_here = 5;
    (void)break_here;
}

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        fprintf(stdout, "Usage: %s [<filename>, ...]", argv[0]);
        return 0;
    }

    usize main_arena_mem_size = MEGABYTE(2);
    void *main_arena_mem      = malloc(main_arena_mem_size);
    global_main_arena         = MemArena_Init(main_arena_mem, main_arena_mem_size);
#if 0
    FILE *output_file         = fopen("DqnReflect_Generated.cpp", "w");
#else
    FILE *output_file         = stdout;
#endif

    fprintf(output_file,
            "#ifndef DQN_REFLECT_GENERATED_H\n"
            "#define DQN_REFLECT_GENERATED_H\n\n"
            "// This is an auto generated file using Dqn_Reflect\n"
            "\n"
            " // NOTE: These macros are undefined at the end of the file so to not pollute namespace\n"
            "#define ARRAY_COUNT(array) sizeof(array)/sizeof((array)[0])\n"
            "#define CHAR_COUNT(str) (ARRAY_COUNT(str) - 1)\n"
            "#define STR_AND_LEN(str) str, CHAR_COUNT(str)\n"
            "\n");

    for (usize arg_index = 1; arg_index < argc; ++arg_index)
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

        fprintf(output_file,
                "//\n"
                "// %s\n"
                "//\n"
                "\n"
                "#if !defined(DQN_REFLECT_DISABLE_%.*s)\n",
                file_name,
                file_include_contents_hash_define_len,
                file_include_contents_hash_define);

        CPPTokeniser tokeniser      = {};
        tokeniser.spaces_per_indent = 4;
        tokeniser.output_file       = output_file;
        tokeniser.tokens_max        = 16384;
        tokeniser.tokens            = MEM_ARENA_ALLOC_ARRAY(&global_main_arena, CPPToken, tokeniser.tokens_max);

        StringLiteral const REFLECT_MARKER = STR_LITERAL("DQN_REFLECT");
        char *file_buf_end                 = file_buf + file_size;
        StringLiteral buffer               = StringLiteral(file_buf, static_cast<int>(file_size));

        for (char *ptr = StrFind(buffer, REFLECT_MARKER);
             ptr;
             ptr = StrFind(buffer, REFLECT_MARKER))
        {
            ptr += REFLECT_MARKER.len;
            int indent_level          = 0;
            bool started_lexing_scope = false;
            for (; ptr;)
            {
                while (CharIsWhitespace(ptr[0])) ptr++;
                if (!ptr[0]) break;

                CPPToken *token = CPPTokeniser_MakeToken(&tokeniser);
                token->str      = ptr++;
                token->len      = 1;
                switch(token->str[0])
                {
                    case '{': { token->type = CPPTokenType::LeftBrace;  started_lexing_scope = true; indent_level++; } break;
                    case '}': { token->type = CPPTokenType::RightBrace; indent_level--; } break;
                    case '(': { token->type = CPPTokenType::OpenParen;   } break;
                    case ')': { token->type = CPPTokenType::CloseParen;  } break;
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
                        if (ptr[0] == '/')
                        {
                            ptr++;
                            while (ptr[0] == ' ' || ptr[0] == '\t') ptr++;
                            token->str = ptr;
                            while (ptr[0] != '\n') ptr++;
                            token->type = CPPTokenType::Comment;
                            token->len  = static_cast<int>(ptr - token->str);
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
                    tokeniser.tokens_len--;
                }
                else
                {
                    if (started_lexing_scope && indent_level == 0)
                        break;
                }
            }

            buffer.str = ptr;
            buffer.len = static_cast<int>(file_buf_end - ptr);
        }

        CPPToken *sentinel = CPPTokeniser_MakeToken(&tokeniser);
        sentinel->type     = CPPTokenType::EndOfStream;

        for (CPPToken token = CPPTokeniser_PeekToken(&tokeniser);
             ;
             token          = CPPTokeniser_PeekToken(&tokeniser))
        {
#if 0
            Token const *token = tokens + index;
            fprintf(stdout, "%.*s", token->len, token->str);
            if (index < (tokens_index - 1)) fprintf(stdout, " -> ");
#endif
            if (IsIdentifierToken(token, STR_LITERAL("enum")))
            {
                ParseCPPEnum(&tokeniser);
            }
            else if (IsIdentifierToken(token, STR_LITERAL("struct")) || IsIdentifierToken(token, STR_LITERAL("class")))
            {
                ParseCPPStruct(&tokeniser);
            }
            else
            {
                token = CPPTokeniser_NextToken(&tokeniser);
            }

            if (token.type == CPPTokenType::EndOfStream)
                break;
        }

        fprintf(output_file, "#endif // DQN_REFLECT_DISABLE_%.*s\n\n",
                file_include_contents_hash_define_len,
                file_include_contents_hash_define);
    }

    fprintf(output_file,
            "#undef ARRAY_COUNT\n"
            "#undef CHAR_COUNT\n"
            "#undef STR_AND_LEN\n"
            "#endif // DQN_REFLECT_GENERATED_H\n"
            );
    fclose(output_file);

    return 0;
}

#endif // DQN_REFLECT_IMPLEMENTATION
#endif // DQN_REFLECT_H
