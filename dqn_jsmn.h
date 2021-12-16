#if !defined(DQN_JSMN_H)
#define DQN_JSMN_H
// -----------------------------------------------------------------------------
// NOTE: Dqn_Jsmn
// -----------------------------------------------------------------------------
// Some helper functions ontop of the jsmn API to make it more ergonomic to use.
// JSMN is embedded in this source file for convenience.
//
// -----------------------------------------------------------------------------
// NOTE: Configuration
// -----------------------------------------------------------------------------
// #define DQN_JSMN_IMPLEMENTATION
//     Define this in one and only one C++ file to enable the implementation
//     code of the header file. This will also automatically enable the JSMN
//     implementation.
//
// #define DQN_JSMN_NO_CRT
//     Define this macro to disable functions using the CRT where possible. JSMN
//     itself includes <stddef.h> and this is the only dependency.
//

#if !defined(DQN_JSMN_NO_CRT)
    #include <stdio.h>
    #include <stdlib.h>
#endif

#if defined(__cplusplus)
    #define DQN_JSMN_CLITERAL(Type) Type
    #define DQN_JSMN_ZERO_INIT {}
#else
    #define DQN_JSMN_CLITERAL(Type) (Type)
    #define DQN_JSMN_ZERO_INIT {0}
    #include <stdbool.h>
#endif

#if !defined(DQN_JSMN_DEBUG_BREAK)
    #include <signal.h>
    #if defined(NDEBUG)
        #define DQN_JSMN_DEBUG_BREAK
    #else
        #if defined(_WIN32)
            #define DQN_JSMN_DEBUG_BREAK __debugbreak()
        #else
            #define DQN_JSMN_DEBUG_BREAK raise(SIGTRAP)
        #endif
    #endif
#endif

#if !defined(DQN_JSMN_ASSERT)
    #define DQN_JSMN_ASSERT(expr)                                                    \
        do                                                                                         \
        {                                                                                          \
            if (!(expr))                                                                           \
            {                                                                                      \
                DQN_JSMN_DEBUG_BREAK;                                                              \
            }                                                                                      \
        } while (0)
#endif

// -----------------------------------------------------------------------------
// NOTE: JSMN Configuration
// -----------------------------------------------------------------------------
// JSMN has its own set of #defines that are definable by the user. See the JSMN
// header file below for more information.
//
// -----------------------------------------------------------------------------
// NOTE: JSMN Header File: commit 053d3cd29200edb1bfd181d917d140c16c1f8834
// -----------------------------------------------------------------------------
/*
 * MIT License
 *
 * Copyright (c) 2010 Serge Zaitsev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef JSMN_H
#define JSMN_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef JSMN_STATIC
#define JSMN_API static
#else
#define JSMN_API extern
#endif

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum {
  JSMN_UNDEFINED = 0,
  JSMN_OBJECT = 1,
  JSMN_ARRAY = 2,
  JSMN_STRING = 3,
  JSMN_PRIMITIVE = 4
} jsmntype_t;

enum jsmnerr {
  /* Not enough tokens were provided */
  JSMN_ERROR_NOMEM = -1,
  /* Invalid character inside JSON string */
  JSMN_ERROR_INVAL = -2,
  /* The string is not a full JSON packet, more bytes expected */
  JSMN_ERROR_PART = -3
};

/**
 * JSON token description.
 * type		type (object, array, string etc.)
 * start	start position in JSON data string
 * end		end position in JSON data string
 */
typedef struct jsmntok {
  jsmntype_t type;
  int start;
  int end;
  int size;
#ifdef JSMN_PARENT_LINKS
  int parent;
#endif
} jsmntok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string.
 */
typedef struct jsmn_parser {
  unsigned int pos;     /* offset in the JSON string */
  unsigned int toknext; /* next token to allocate */
  int toksuper;         /* superior token node, e.g. parent object or array */
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
JSMN_API void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each
 * describing
 * a single JSON object.
 */
JSMN_API int jsmn_parse(jsmn_parser *parser, const char *js, const size_t len,
                        jsmntok_t *tokens, const unsigned int num_tokens);


#ifdef __cplusplus
}
#endif
#endif // JSMN_H

// NOTE: Iterator copied from: https://github.com/zserge/jsmn/pull/69
// TODO(dqn): Write our own iterator logic in a manner that is more stateful
// than the current implementation, we should not have pass information back and
// forth between iterators, i.e. see my iterator abstraction that sits on top of
// this.
#ifndef __JSMN_ITERATOR_H__
#define __JSMN_ITERATOR_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Error return codes for jsmn iterator
 */
enum {
  /* Input parameter error */
  JSMNITER_ERR_PARAMETER  = -1,
  /* JSMN index doesn't point at an Array/Object */
  JSMNITER_ERR_TYPE       = -2,  
  /* Group item misses string identifier */
  JSMNITER_ERR_NOIDENT    = -3,
  /* Broken JSON */
  JSMNITER_ERR_BROKEN     = -4,
};


/**
 * Struct with state information for jsmn iterator
 * - When the no more items for iterator the parser_pos value will point to 
 *   JSMN index for next object after current Array/Object
 */
typedef struct {
  jsmntok_t *jsmn_tokens;
  unsigned int jsmn_len;
  unsigned int parent_pos;
  unsigned int parser_pos;
  unsigned int index;
} jsmn_iterator_t;


/**
 * @brief Takes an JSMN Array/Object and locates index for last item in collection
 * @details Iterates over JSMN Array/Object until last item is found
 * 
 * @param jsmn_tokens   JSMN tokens
 * @param jsmn_len      JSMN token count
 * @param parser_pos    Current JSMN token
 *
 * @return  < 0 - Error has occured, corresponds to one of JSMNITER_ERR_*
 *          >=0 - JSMN index for last item in Array/Object
 */
int jsmn_iterator_find_last( jsmntok_t *jsmn_tokens, unsigned int jsmn_len, unsigned int parser_pos );


/**
 * @brief Initialize iterator
 * @details Set initial value for iterator struct
 * 
 * @param iterator      Iterator struct
 * @param jsmn_tokens   JSMN tokens
 * @param jsmn_len      JSMN token count
 * @param parser_pos    Current JSMN token
 * 
 * @return  < 0 - Error has occured, corresponds to one of JSMNITER_ERR_*
 *          >=0 - Ok
 */
int jsmn_iterator_init( jsmn_iterator_t *iterator, jsmntok_t *jsmn_tokens, unsigned int jsmn_len, 
                        unsigned int parser_pos );


/**
 * @brief Get next item in JSMN Array/Object
 * @details Gets JSMN position for next identifier and value in Array/Object
 * 
 * @param iterator            Iterator struct
 * @param jsmn_identifier     Return pointer for identifier, NULL for Array
 * @param jsmn_value          Return pointer for value
 * @param next_value_index    Possible to indicate where next value begins, allows determine end of sub
 *                            Array/Object withouth manually searching for it
 * 
 * @return  < 0 - Error has occured, corresponds to one of JSMNITER_ERR_*
 *            0 - No more values
 *          > 0 - Value (and identifier) has been returned
 */
int jsmn_iterator_next( jsmn_iterator_t *iterator, jsmntok_t **jsmn_identifier, jsmntok_t **jsmn_value, 
                        unsigned int next_value_index );



/**
 * @brief Return current parser position
 * @details For Array the parser point to current value index
 *          For Object the parser points to the identifier
 * 
 * @param iterator [description]
 * @return [description]
 */
#define jsmn_iterator_position(_iterator_) ((_iterator_)->parser_pos)


#ifdef __cplusplus
}
#endif

#endif /*__JSMN_ITERATOR_H__*/

// -----------------------------------------------------------------------------
// Header File
// -----------------------------------------------------------------------------
#if defined(_WIN32)
    typedef unsigned __int64 Dqn_JsmnU64;
    typedef signed __int64 Dqn_JsmnI64;
#else
    typedef unsigned long long Dqn_JsmnU64;
    typedef signed long long Dqn_JsmnI64;
#endif

typedef struct Dqn_JsmnString
{
    union Dqn_JsmnStringBuffer { char const *const_str; char *str; } buf;
    Dqn_JsmnI64 size;
} Dqn_JsmnString;

#define DQN_JSMN_STRING(string) DQN_JSMN_CLITERAL(Dqn_JsmnString){{(string)}, sizeof(string) - 1}
#define DQN_JSMN_STRING_FMT(string) (int)((string).size), (string).buf.str

bool        Dqn_JsmnStringEq(Dqn_JsmnString lhs, Dqn_JsmnString rhs);
bool        Dqn_JsmnStringIsValid(Dqn_JsmnString string);
bool        Dqn_JsmnIsDigit(char ch);
Dqn_JsmnU64 Dqn_JsmnStringToU64(Dqn_JsmnString string);

#if defined(__cplusplus)
bool        operator==(Dqn_JsmnString lhs, Dqn_JsmnString rhs);
#endif // __cplusplus

#define DQN_JSMN_X_MACRO \
    DQN_JSMN_X_ENTRY(Invalid) \
    DQN_JSMN_X_ENTRY(Object) \
    DQN_JSMN_X_ENTRY(Array) \
    DQN_JSMN_X_ENTRY(String) \
    DQN_JSMN_X_ENTRY(Number) \
    DQN_JSMN_X_ENTRY(Bool)

typedef enum Dqn_JsmnTokenIs
{
#define DQN_JSMN_X_ENTRY(enum_val) Dqn_JsmnTokenIs_##enum_val,
    DQN_JSMN_X_MACRO
#undef DQN_JSMN_X_ENTRY
} Dqn_JsmnTokenIs;

typedef struct Dqn_JsmnError
{
    jsmntok_t        token;
    Dqn_JsmnString   json;
    Dqn_JsmnTokenIs  actual;
    Dqn_JsmnTokenIs  expected;
    char const      *cpp_file; // The file of the .cpp/h source code that triggered the error
    int              cpp_line; // The line of the .cpp/h source code that triggered the error
} Dqn_JsmnError;

#if !defined(DQN_JSMN_ERROR_HANDLE_SIZE)
    #define DQN_JSMN_ERROR_HANDLE_SIZE 128
#endif

typedef struct Dqn_JsmnErrorHandle
{
    Dqn_JsmnError errors[DQN_JSMN_ERROR_HANDLE_SIZE];
    int           errors_size;
} Dqn_JsmnErrorHandle;

typedef struct Dqn_Jsmn
{
    bool            valid;
    jsmn_parser     parser;
    Dqn_JsmnString  json;
    bool            json_needs_crt_free;
    unsigned int    tokens_size;
    jsmntok_t      *tokens;
} Dqn_Jsmn;

typedef struct Dqn_JsmnIterator
{
    bool            init;
    jsmn_iterator_t jsmn_it;
    Dqn_JsmnString  json;
    jsmntok_t      *key;
    jsmntok_t      *value;

    // When obj/array iteration is finished, we set the token_index_hint to the
    // parent iterator so that it knows where to continue off from and to skip
    // over the object/array we just iterated.
    int             token_index_hint;
} Dqn_JsmnIterator;

// Calculate the number of tokens required to parse the 'json' input.
unsigned int   Dqn_JsmnTokensRequired(char const *json, Dqn_JsmnI64 size);
Dqn_JsmnString Dqn_JsmnTokenIsToString(Dqn_JsmnTokenIs enum_val);

// Initialise a Dqn_Jsmn context by passing the JSON string and pass in a token
// array sufficient to parse the JSON string. The number of tokens that must be
// allocated and passed in can be calculated using Dqn_JsmnTokensRequire(...).
Dqn_Jsmn       Dqn_JsmnInitWithJSONCString(char const *json, Dqn_JsmnI64 size, jsmntok_t *tokens, unsigned int tokens_size);
Dqn_Jsmn       Dqn_JsmnInitWithJSONString(Dqn_JsmnString json, jsmntok_t *tokens, unsigned int tokens_size);

#if !defined(DQN_JSMN_NO_CRT)
Dqn_Jsmn       Dqn_JsmnInitWithMallocJSONCString(char const *json, Dqn_JsmnI64 size);
Dqn_Jsmn       Dqn_JsmnInitWithMallocJSON(Dqn_JsmnString json);
Dqn_Jsmn       Dqn_JsmnInitWithMallocJSONFile(Dqn_JsmnString json_file);
void           Dqn_JsmnFree(Dqn_Jsmn *jsmn);
void           Dqn_JsmnErrorHandleDumpToCRTFile(Dqn_JsmnErrorHandle const *handle, FILE *file);
void           Dqn_JsmnErrorDumpToCRTFile(Dqn_JsmnError const *error, FILE *file);
#endif // !DQN_JSMN_NO_CRT

#if defined(DQN_H)
Dqn_Jsmn       Dqn_JsmnInitWithArenaJSONCString(char const *json, Dqn_JsmnI64 size, Dqn_Arena *arena);
Dqn_Jsmn       Dqn_JsmnInitWithArenaJSON(Dqn_JsmnString json, Dqn_Arena *arena);
Dqn_Jsmn       Dqn_JsmnInitWithArenaJSONFile(Dqn_JsmnString file, Dqn_Arena *arena);
#endif // DQN_H

// return: If the token is an array, return the size of the array otherwise -1.
int            Dqn_JsmnTokenArraySize(jsmntok_t token);
Dqn_JsmnString Dqn_JsmnTokenString(jsmntok_t token, Dqn_JsmnString json);
bool           Dqn_JsmnTokenBool(jsmntok_t token, Dqn_JsmnString json);
Dqn_JsmnU64    Dqn_JsmnTokenU64(jsmntok_t token, Dqn_JsmnString json);

// Iterator abstraction over jsmn_iterator_t, example on how to use this is
// shown below. The goal here is to minimise the amount of state the user has to
// manage.
#if 0
    Dqn_JsmnString json = DQN_JSMN_STRING("{ \"test\": { \"test2\": 0 } }");
    Dqn_Jsmn jsmn_state = Dqn_JsmnInitWithMallocJSON(json);
    for (Dqn_JsmnIterator it = DQN_JSMN_ZERO_INIT; Dqn_JsmnIteratorNext(&it, &jsmn_state, NULL /*prev_it*/); )
    {
        Dqn_JsmnString key = Dqn_JsmnIteratorKey(&it);
        if (Dqn_JsmnStringEq(key, DQN_JSMN_STRING("test")))
        {
            if (!Dqn_JsmnIteratorExpectValue(&it, Dqn_JsmnTokenIs_Object, NULL))
                continue;

            for (Dqn_JsmnIterator obj_it = DQN_JSMN_ZERO_INIT; Dqn_JsmnIteratorNext(&obj_it, &jsmn_state, &it); )
            {
                Dqn_JsmnString obj_key = Dqn_JsmnIteratorKey(&obj_it);
                if (Dqn_JsmnStringEq(obj_key, DQN_JSMN_STRING("test2")))
                {
                    if (!Dqn_JsmnIteratorExpectValue(&obj_it, Dqn_JsmnTokenIs_Number, NULL))
                        continue;

                    Dqn_JsmnU64 test_2_value = Dqn_JsmnIteratorU64(&obj_it);
                    (void)test_2_value;
                }
            }
        }
    }
#endif
bool             Dqn_JsmnIteratorNext(Dqn_JsmnIterator *it, Dqn_Jsmn *jsmn_state, Dqn_JsmnIterator *prev_it);
Dqn_JsmnString   Dqn_JsmnIteratorKey(Dqn_JsmnIterator *it);
Dqn_JsmnIterator Dqn_JsmnIteratorFindKey(Dqn_Jsmn *jsmn_state, Dqn_JsmnString key, Dqn_JsmnIterator *parent_it);

#define Dqn_JsmnIteratorExpectValue(it, expected, err_handle) Dqn_Jsmn_IteratorExpectValue(it, expected, err_handle, __FILE__, __LINE__)
#define Dqn_JsmnIteratorExpectKey(it, expected, err_handle) Dqn_Jsmn_IteratorExpectKey(it, expected, err_handle, __FILE__, __LINE__)

// Convert the value part of the key-value JSON pair the iterator is currently
// pointing to, to a string/bool/u64. If the iterator's value does not point to
// the type requested, a zero initialised value is returned.
Dqn_JsmnString Dqn_JsmnIteratorString(Dqn_JsmnIterator const *it);
bool           Dqn_JsmnIteratorBool(Dqn_JsmnIterator const *it);
Dqn_JsmnU64    Dqn_JsmnIteratorU64(Dqn_JsmnIterator const *it);
#endif // DQN_JSMN_H

#if defined(DQN_JSMN_IMPLEMENTATION)
// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------
bool Dqn_JsmnStringEq(Dqn_JsmnString lhs, Dqn_JsmnString rhs)
{
    bool result = lhs.size == rhs.size;
    for (int i = 0; i < lhs.size && result; i++) result &= lhs.buf.str[i] == rhs.buf.str[i];
    return result;
}

bool Dqn_JsmnStringIsValid(Dqn_JsmnString string)
{
    bool result = string.buf.str && string.size >= 0;
    return result;
}

bool Dqn_JsmnIsDigit(char ch)
{
    bool result = (ch >= '0' && ch <= '9');
    return result;
}

Dqn_JsmnU64 Dqn_JsmnStringToU64(Dqn_JsmnString string)
{
    Dqn_JsmnU64 result = 0;
    if (!Dqn_JsmnStringIsValid(string))
        return result;

    for (int i = 0; i < string.size; i++)
    {
        char ch = string.buf.str[i];
        if (!Dqn_JsmnIsDigit(ch))
            return result;

        Dqn_JsmnU64 digit = ch - '0';
        result         = (result * 10) + digit;
    }

    return result;
}

#if defined(__cplusplus)
bool operator==(Dqn_JsmnString lhs, Dqn_JsmnString rhs)
{
    bool result = Dqn_JsmnStringEq(lhs, rhs);
    return result;
}
#endif // __cplusplus

unsigned int Dqn_JsmnTokensRequired(char const *json, Dqn_JsmnI64 size)
{
    jsmn_parser parser;
    jsmn_init(&parser);
    unsigned int result = jsmn_parse(&parser, json, size, NULL, 0);
    return result;
}

Dqn_JsmnString Dqn_JsmnTokenIsToString(Dqn_JsmnTokenIs enum_val)
{
    switch (enum_val)
    {
#define DQN_JSMN_X_ENTRY(enum_val) case Dqn_JsmnTokenIs_##enum_val: return DQN_JSMN_STRING(#enum_val);
    DQN_JSMN_X_MACRO
#undef DQN_JSMN_X_ENTRY
    }
    return DQN_JSMN_STRING("DQN_JSMN_UNHANDLED_ENUM");
}

Dqn_Jsmn Dqn_JsmnInitWithJSONCString(char const *json, Dqn_JsmnI64 size, jsmntok_t *tokens, unsigned int tokens_size)
{
    Dqn_Jsmn result = DQN_JSMN_ZERO_INIT;
    if (!tokens || tokens_size <= 0 || !json || size <= 0)
        return result;

    DQN_JSMN_ASSERT(tokens_size == Dqn_JsmnTokensRequired(json, size));
    jsmn_init(&result.parser);
    result.json.buf.const_str = json;
    result.json.size          = size;
    result.tokens             = tokens;
    result.tokens_size        = tokens_size;
    result.valid = jsmn_parse(&result.parser, result.json.buf.str, result.json.size, result.tokens, result.tokens_size) > 0;
    return result;
}

Dqn_Jsmn Dqn_JsmnInitWithJSONString(Dqn_JsmnString json, jsmntok_t *tokens, unsigned int size)
{
    Dqn_Jsmn result = Dqn_JsmnInitWithJSONCString(json.buf.str, json.size, tokens, size);
    return result;
}

#if !defined(DQN_JSMN_NO_CRT)
Dqn_Jsmn Dqn_JsmnInitWithMallocJSONCString(char const *json, Dqn_JsmnI64 size)
{
    unsigned int tokens_size = Dqn_JsmnTokensRequired(json, size);
    jsmntok_t *tokens        = (jsmntok_t *)malloc(sizeof(jsmntok_t) * tokens_size);

    Dqn_Jsmn result = DQN_JSMN_ZERO_INIT;
    if (tokens)
        result = Dqn_JsmnInitWithJSONCString(json, size, tokens, tokens_size);

    if (!result.valid)
        free(tokens);

    return result;
}

Dqn_Jsmn Dqn_JsmnInitWithMallocJSON(Dqn_JsmnString json)
{
    Dqn_Jsmn result = Dqn_JsmnInitWithMallocJSONCString(json.buf.str, json.size);
    return result;
}

Dqn_Jsmn Dqn_JsmnInitWithMallocJSONFile(Dqn_JsmnString json_file)
{
    Dqn_Jsmn result      = DQN_JSMN_ZERO_INIT;
    FILE *   file_handle = NULL;
    fopen_s(&file_handle, json_file.buf.str, "rb");
    if (!file_handle)
        return result;

    fseek(file_handle, 0, SEEK_END);
    size_t json_size = ftell(file_handle);
    rewind(file_handle);

    char *json = (char *)malloc(json_size + 1);
    if (json)
    {
        json[json_size] = 0;
        if (fread(json, json_size, 1, file_handle) == 1)
        {
            unsigned int tokens_size = Dqn_JsmnTokensRequired(json, json_size);
            jsmntok_t *  tokens      = (jsmntok_t *)malloc(sizeof(jsmntok_t) * tokens_size);
            if (tokens)
            {
                result = Dqn_JsmnInitWithJSONCString(json, json_size, tokens, tokens_size);
                result.json_needs_crt_free = true;
            }
        }
    }

    fclose(file_handle);
    if (!result.valid)
        Dqn_JsmnFree(&result);

    return result;
}

void Dqn_JsmnFree(Dqn_Jsmn *jsmn)
{
    if (jsmn->json.buf.str && jsmn->json_needs_crt_free)
        free(jsmn->json.buf.str);

    if (jsmn->tokens)
        free(jsmn->tokens);

    jsmn->valid = false;
}

void Dqn_JsmnErrorHandleDumpToCRTFile(Dqn_JsmnErrorHandle const *handle, FILE *file)
{
    for (int err_index = 0; err_index < handle->errors_size; err_index++)
    {
        Dqn_JsmnError const *error = handle->errors + err_index;
        Dqn_JsmnErrorDumpToCRTFile(error, file);
    }
}

void Dqn_JsmnErrorDumpToCRTFile(Dqn_JsmnError const *error, FILE *file)
{
    fprintf(file,
            "Json parsing error in %s:%d, expected token type: %.*s, token was: %.*s (%.*s)\n",
            error->cpp_file,
            error->cpp_line,
            DQN_JSMN_STRING_FMT(Dqn_JsmnTokenIsToString(error->expected)),
            DQN_JSMN_STRING_FMT(Dqn_JsmnTokenIsToString(error->actual)),
            DQN_JSMN_STRING_FMT(Dqn_JsmnTokenString(error->token, error->json)));
}
#endif // !DQN_JSMN_NO_CRT

#if defined(DQN_IMPLEMENTATION)
Dqn_Jsmn Dqn_JsmnInitWithArenaJSONCString(char const *json, Dqn_JsmnI64 size, Dqn_Arena *arena)
{
    int        tokens_size = Dqn_JsmnTokensRequired(json, size);
    jsmntok_t *tokens      = Dqn_ArenaNewArray(arena, jsmntok_t, tokens_size, Dqn_ZeroMem::No);
    Dqn_Jsmn   result      = Dqn_JsmnInitWithJSONCString(json, size, tokens, tokens_size);
    return result;
}

Dqn_Jsmn Dqn_JsmnInitWithArenaJSON(Dqn_JsmnString json, Dqn_Arena *arena)
{
    Dqn_Jsmn result = Dqn_JsmnInitWithArenaJSONCString(json.buf.str, json.size, arena);
    return result;
}

Dqn_Jsmn Dqn_JsmnInitWithArenaJSONFile(Dqn_JsmnString file, Dqn_Arena *arena)
{
    Dqn_String json   = Dqn_FileArenaReadToString(file.buf.str, file.size, arena);
    Dqn_Jsmn   result = Dqn_JsmnInitWithArenaJSON({{json.str}, (int)json.size}, arena);
    return result;
}
#endif // DQN_IMPLEMENTATION

int Dqn_JsmnTokenArraySize(jsmntok_t token)
{
    int result = token.type == JSMN_ARRAY ? token.size : -1;
    return result;
}

Dqn_JsmnString Dqn_JsmnTokenString(jsmntok_t token, Dqn_JsmnString json)
{
    Dqn_JsmnString result = {{json.buf.str + token.start}, token.end - token.start};
    return result;
}

bool Dqn_JsmnTokenBool(jsmntok_t token, Dqn_JsmnString json)
{
    DQN_JSMN_ASSERT(token.start < json.size);
    char    ch     = json.buf.str[token.start];
    bool result = ch == 't';
    if (!result) { DQN_JSMN_ASSERT(ch == 'f'); }
    return result;
}

Dqn_JsmnU64 Dqn_JsmnTokenU64(jsmntok_t token, Dqn_JsmnString json)
{
    DQN_JSMN_ASSERT(token.start < json.size);
    Dqn_JsmnString string = {{json.buf.str + token.start}, token.end - token.start};
    Dqn_JsmnU64   result = Dqn_JsmnStringToU64(string);
    return result;
}

void Dqn_JsmnErrorHandleAddError(Dqn_JsmnErrorHandle *handle, jsmntok_t token, Dqn_JsmnString json, Dqn_JsmnTokenIs actual, Dqn_JsmnTokenIs expected, char const *file, int line)
{
    if (!handle)
        return;

    if (handle->errors_size >= DQN_JSMN_ERROR_HANDLE_SIZE)
        return;

    Dqn_JsmnError *error = handle->errors + handle->errors_size++;
    error->token         = token;
    error->actual        = actual;
    error->expected      = expected;
    error->json          = json;
    error->cpp_file      = file;
    error->cpp_line      = line;
}

bool Dqn_JsmnIteratorNext(Dqn_JsmnIterator *it, Dqn_Jsmn *jsmn_state, Dqn_JsmnIterator *prev_it)
{
    if (!it->init)
    {
        it->init = true;
        if (jsmn_state->valid)
        {
            it->json = jsmn_state->json;
            jsmn_iterator_init(&it->jsmn_it, jsmn_state->tokens, jsmn_state->tokens_size, prev_it ? jsmn_iterator_position(&prev_it->jsmn_it) : 0);
        }
    }

    bool result = false;
    if (!Dqn_JsmnStringIsValid(it->json) || it->json.size <= 0) {
        return result;
    }

    result = jsmn_iterator_next(&it->jsmn_it, &it->key, &it->value, it->token_index_hint) > 0;
    if (!result)
    {
        // NOTE: Iterator has finished object/array, previous iterator will
        // continue off where this iterator left off.
        if (prev_it)
            prev_it->token_index_hint = jsmn_iterator_position(&it->jsmn_it);
    }

    return result;
}

Dqn_JsmnString Dqn_JsmnIteratorKey(Dqn_JsmnIterator *it)
{
    Dqn_JsmnString result = DQN_JSMN_ZERO_INIT;
    if (it && it->key)
        result = DQN_JSMN_CLITERAL(Dqn_JsmnString){{it->json.buf.str + it->key->start}, it->key->end - it->key->start};
    return result;
}

Dqn_JsmnIterator Dqn_JsmnIteratorFindKey(Dqn_Jsmn *jsmn_state, Dqn_JsmnString key, Dqn_JsmnIterator *parent_it)
{
    Dqn_JsmnIterator result = DQN_JSMN_ZERO_INIT;
    for (Dqn_JsmnIterator it = DQN_JSMN_ZERO_INIT; Dqn_JsmnIteratorNext(&it, jsmn_state, parent_it); )
    {
        Dqn_JsmnString it_key = Dqn_JsmnTokenString(*it.key, jsmn_state->json);
        if (Dqn_JsmnStringEq(it_key, key))
        {
            result = it;
            break;
        }
    }

    return result;
}

static bool Dqn_Jsmn_IteratorExpect(Dqn_JsmnIterator *it, Dqn_JsmnTokenIs expected, jsmntok_t token, Dqn_JsmnErrorHandle *err_handle, char const *file, unsigned int line)
{
    bool result = false;
    switch (expected)
    {
        case Dqn_JsmnTokenIs_Invalid: result = token.type == JSMN_UNDEFINED; break;
        case Dqn_JsmnTokenIs_Object: result  = token.type == JSMN_OBJECT; break;
        case Dqn_JsmnTokenIs_Array:  result  = token.type == JSMN_ARRAY; break;
        case Dqn_JsmnTokenIs_String: result  = token.type == JSMN_STRING; break;

        case Dqn_JsmnTokenIs_Number:
        {
            DQN_JSMN_ASSERT(token.start < it->json.size);
            char ch = it->json.buf.str[token.start];
            result  = token.type == JSMN_PRIMITIVE && (ch == '-' || Dqn_JsmnIsDigit(ch));
        }
        break;

        case Dqn_JsmnTokenIs_Bool:
        {
            DQN_JSMN_ASSERT(token.start < it->json.size);
            char ch = it->json.buf.str[token.start];
            result = token.type == JSMN_PRIMITIVE && (ch == 't' || ch == 'f');
        }
        break;
    }

    Dqn_JsmnTokenIs actual = DQN_JSMN_ZERO_INIT;
    switch (token.type)
    {
        case JSMN_UNDEFINED: actual = Dqn_JsmnTokenIs_Invalid; break;
        case JSMN_OBJECT: actual = Dqn_JsmnTokenIs_Object; break;
        case JSMN_ARRAY: actual = Dqn_JsmnTokenIs_Array; break;
        case JSMN_STRING: actual = Dqn_JsmnTokenIs_String; break;

        case JSMN_PRIMITIVE:
        {
            char first = it->json.buf.str[token.start];
            if (first == 't' || first == 'f')
                actual = Dqn_JsmnTokenIs_Bool;
            else
                actual = Dqn_JsmnTokenIs_Number;
        }
        break;
    }

    if (!result)
        Dqn_JsmnErrorHandleAddError(err_handle, token, it->json, actual, expected, file, line);

    return result;
}

bool Dqn_Jsmn_IteratorExpectValue(Dqn_JsmnIterator *it, Dqn_JsmnTokenIs expected, Dqn_JsmnErrorHandle *err_handle, char const *file, unsigned int line)
{
    bool result = it->value && Dqn_Jsmn_IteratorExpect(it, expected, *it->value, err_handle, file, line);
    return result;
}

bool Dqn_Jsmn_IteratorExpectKey(Dqn_JsmnIterator *it, Dqn_JsmnTokenIs expected, Dqn_JsmnErrorHandle *err_handle, char const *file, unsigned int line)
{
    bool result = it->key && Dqn_Jsmn_IteratorExpect(it, expected, *it->key, err_handle, file, line);
    return result;
}

Dqn_JsmnString Dqn_JsmnIteratorString(Dqn_JsmnIterator const *it)
{
    Dqn_JsmnString result = DQN_JSMN_ZERO_INIT;
    if (it->value && it->json.buf.str)
        result = DQN_JSMN_CLITERAL(Dqn_JsmnString){{it->json.buf.str + it->value->start}, it->value->end - it->value->start};
    return result;
}

bool Dqn_JsmnIteratorBool(Dqn_JsmnIterator const *it)
{
    bool result = it->value && Dqn_JsmnTokenBool(*it->value, it->json);
    return result;
}

Dqn_JsmnU64 Dqn_JsmnIteratorU64(Dqn_JsmnIterator const *it)
{
    Dqn_JsmnU64 result = it->value && Dqn_JsmnTokenU64(*it->value, it->json);
    return result;
}

// -----------------------------------------------------------------------------
// JSMN Implementation
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Allocates a fresh unused token from the token pool.
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser, jsmntok_t *tokens,
                                   const size_t num_tokens) {
  jsmntok_t *tok;
  if (parser->toknext >= num_tokens) {
    return NULL;
  }
  tok = &tokens[parser->toknext++];
  tok->start = tok->end = -1;
  tok->size = 0;
#ifdef JSMN_PARENT_LINKS
  tok->parent = -1;
#endif
  return tok;
}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token(jsmntok_t *token, const jsmntype_t type,
                            const int start, const int end) {
  token->type = type;
  token->start = start;
  token->end = end;
  token->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
static int jsmn_parse_primitive(jsmn_parser *parser, const char *js,
                                const size_t len, jsmntok_t *tokens,
                                const size_t num_tokens) {
  jsmntok_t *token;
  int start;

  start = parser->pos;

  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    switch (js[parser->pos]) {
#ifndef JSMN_STRICT
    /* In strict mode primitive must be followed by "," or "}" or "]" */
    case ':':
#endif
    case '\t':
    case '\r':
    case '\n':
    case ' ':
    case ',':
    case ']':
    case '}':
      goto found;
    default:
                   /* to quiet a warning from gcc*/
      break;
    }
    if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
      parser->pos = start;
      return JSMN_ERROR_INVAL;
    }
  }
#ifdef JSMN_STRICT
  /* In strict mode primitive must be followed by a comma/object/array */
  parser->pos = start;
  return JSMN_ERROR_PART;
#endif

found:
  if (tokens == NULL) {
    parser->pos--;
    return 0;
  }
  token = jsmn_alloc_token(parser, tokens, num_tokens);
  if (token == NULL) {
    parser->pos = start;
    return JSMN_ERROR_NOMEM;
  }
  jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
#ifdef JSMN_PARENT_LINKS
  token->parent = parser->toksuper;
#endif
  parser->pos--;
  return 0;
}

/**
 * Fills next token with JSON string.
 */
static int jsmn_parse_string(jsmn_parser *parser, const char *js,
                             const size_t len, jsmntok_t *tokens,
                             const size_t num_tokens) {
  jsmntok_t *token;

  int start = parser->pos;

  parser->pos++;

  /* Skip starting quote */
  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    char c = js[parser->pos];

    /* Quote: end of string */
    if (c == '\"') {
      if (tokens == NULL) {
        return 0;
      }
      token = jsmn_alloc_token(parser, tokens, num_tokens);
      if (token == NULL) {
        parser->pos = start;
        return JSMN_ERROR_NOMEM;
      }
      jsmn_fill_token(token, JSMN_STRING, start + 1, parser->pos);
#ifdef JSMN_PARENT_LINKS
      token->parent = parser->toksuper;
#endif
      return 0;
    }

    /* Backslash: Quoted symbol expected */
    if (c == '\\' && parser->pos + 1 < len) {
      int i;
      parser->pos++;
      switch (js[parser->pos]) {
      /* Allowed escaped symbols */
      case '\"':
      case '/':
      case '\\':
      case 'b':
      case 'f':
      case 'r':
      case 'n':
      case 't':
        break;
      /* Allows escaped symbol \uXXXX */
      case 'u':
        parser->pos++;
        for (i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0';
             i++) {
          /* If it isn't a hex character we have an error */
          if (!((js[parser->pos] >= 48 && js[parser->pos] <= 57) ||   /* 0-9 */
                (js[parser->pos] >= 65 && js[parser->pos] <= 70) ||   /* A-F */
                (js[parser->pos] >= 97 && js[parser->pos] <= 102))) { /* a-f */
            parser->pos = start;
            return JSMN_ERROR_INVAL;
          }
          parser->pos++;
        }
        parser->pos--;
        break;
      /* Unexpected symbol */
      default:
        parser->pos = start;
        return JSMN_ERROR_INVAL;
      }
    }
  }
  parser->pos = start;
  return JSMN_ERROR_PART;
}

/**
 * Parse JSON string and fill tokens.
 */
JSMN_API int jsmn_parse(jsmn_parser *parser, const char *js, const size_t len,
                        jsmntok_t *tokens, const unsigned int num_tokens) {
  int r;
  int i;
  jsmntok_t *token;
  int count = parser->toknext;

  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    char c;
    jsmntype_t type;

    c = js[parser->pos];
    switch (c) {
    case '{':
    case '[':
      count++;
      if (tokens == NULL) {
        break;
      }
      token = jsmn_alloc_token(parser, tokens, num_tokens);
      if (token == NULL) {
        return JSMN_ERROR_NOMEM;
      }
      if (parser->toksuper != -1) {
        jsmntok_t *t = &tokens[parser->toksuper];
#ifdef JSMN_STRICT
        /* In strict mode an object or array can't become a key */
        if (t->type == JSMN_OBJECT) {
          return JSMN_ERROR_INVAL;
        }
#endif
        t->size++;
#ifdef JSMN_PARENT_LINKS
        token->parent = parser->toksuper;
#endif
      }
      token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
      token->start = parser->pos;
      parser->toksuper = parser->toknext - 1;
      break;
    case '}':
    case ']':
      if (tokens == NULL) {
        break;
      }
      type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
#ifdef JSMN_PARENT_LINKS
      if (parser->toknext < 1) {
        return JSMN_ERROR_INVAL;
      }
      token = &tokens[parser->toknext - 1];
      for (;;) {
        if (token->start != -1 && token->end == -1) {
          if (token->type != type) {
            return JSMN_ERROR_INVAL;
          }
          token->end = parser->pos + 1;
          parser->toksuper = token->parent;
          break;
        }
        if (token->parent == -1) {
          if (token->type != type || parser->toksuper == -1) {
            return JSMN_ERROR_INVAL;
          }
          break;
        }
        token = &tokens[token->parent];
      }
#else
      for (i = parser->toknext - 1; i >= 0; i--) {
        token = &tokens[i];
        if (token->start != -1 && token->end == -1) {
          if (token->type != type) {
            return JSMN_ERROR_INVAL;
          }
          parser->toksuper = -1;
          token->end = parser->pos + 1;
          break;
        }
      }
      /* Error if unmatched closing bracket */
      if (i == -1) {
        return JSMN_ERROR_INVAL;
      }
      for (; i >= 0; i--) {
        token = &tokens[i];
        if (token->start != -1 && token->end == -1) {
          parser->toksuper = i;
          break;
        }
      }
#endif
      break;
    case '\"':
      r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
      if (r < 0) {
        return r;
      }
      count++;
      if (parser->toksuper != -1 && tokens != NULL) {
        tokens[parser->toksuper].size++;
      }
      break;
    case '\t':
    case '\r':
    case '\n':
    case ' ':
      break;
    case ':':
      parser->toksuper = parser->toknext - 1;
      break;
    case ',':
      if (tokens != NULL && parser->toksuper != -1 &&
          tokens[parser->toksuper].type != JSMN_ARRAY &&
          tokens[parser->toksuper].type != JSMN_OBJECT) {
#ifdef JSMN_PARENT_LINKS
        parser->toksuper = tokens[parser->toksuper].parent;
#else
        for (i = parser->toknext - 1; i >= 0; i--) {
          if (tokens[i].type == JSMN_ARRAY || tokens[i].type == JSMN_OBJECT) {
            if (tokens[i].start != -1 && tokens[i].end == -1) {
              parser->toksuper = i;
              break;
            }
          }
        }
#endif
      }
      break;
#ifdef JSMN_STRICT
    /* In strict mode primitives are: numbers and booleans */
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 't':
    case 'f':
    case 'n':
      /* And they must not be keys of the object */
      if (tokens != NULL && parser->toksuper != -1) {
        const jsmntok_t *t = &tokens[parser->toksuper];
        if (t->type == JSMN_OBJECT ||
            (t->type == JSMN_STRING && t->size != 0)) {
          return JSMN_ERROR_INVAL;
        }
      }
#else
    /* In non-strict mode every unquoted value is a primitive */
    default:
#endif
      r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
      if (r < 0) {
        return r;
      }
      count++;
      if (parser->toksuper != -1 && tokens != NULL) {
        tokens[parser->toksuper].size++;
      }
      break;

#ifdef JSMN_STRICT
    /* Unexpected char in strict mode */
    default:
      return JSMN_ERROR_INVAL;
#endif
    }
  }

  if (tokens != NULL) {
    for (i = parser->toknext - 1; i >= 0; i--) {
      /* Unmatched opened object or array */
      if (tokens[i].start != -1 && tokens[i].end == -1) {
        return JSMN_ERROR_PART;
      }
    }
  }

  return count;
}

/**
 * Creates a new parser based over a given buffer with an array of tokens
 * available.
 */
JSMN_API void jsmn_init(jsmn_parser *parser) {
  parser->pos = 0;
  parser->toknext = 0;
  parser->toksuper = -1;
}
#ifdef __cplusplus
}
#endif

/**
 * @brief Takes an JSMN Array/Object and locates index for last item in collection
 * @details Iterates over JSMN Array/Object until last item is found
 * 
 * @param jsmn_tokens   JSMN tokens
 * @param jsmn_len      JSMN token count
 * @param parser_pos    Current JSMN token
 *
 * @return  < 0 - Error has occured, corresponds to one of JSMNITER_ERR_*
 *          >=0 - JSMN index for last item in Array/Object
 */
int jsmn_iterator_find_last( jsmntok_t *jsmn_tokens, unsigned int jsmn_len, unsigned int parser_pos ) {
  int child_count;
  unsigned int child_index;
  int parent_end;

  /* No tokens */
  if (!jsmn_tokens)
    return JSMNITER_ERR_PARAMETER;
  /* pos outside tokens */
  if (parser_pos >= jsmn_len)
    return JSMNITER_ERR_PARAMETER;
  /* Not an Array/Object */
  if (jsmn_tokens[parser_pos].type != JSMN_ARRAY && 
      jsmn_tokens[parser_pos].type != JSMN_OBJECT)
    return JSMNITER_ERR_TYPE;

  /* End position for Array/Object */
  parent_end = jsmn_tokens[parser_pos].end;

  /* First child item */
  child_index = parser_pos + 1;
  
  /* Count number of children we need to iterate */
  child_count = jsmn_tokens[parser_pos].size * (jsmn_tokens[parser_pos].type == JSMN_OBJECT ? 2 : 1);

  /* Loop until end of current Array/Object */
  while(child_index < jsmn_len && jsmn_tokens[child_index].start <= parent_end && child_count >= 0) {
    /* Add item count in sub Arrays/Objects that we need to skip */
    if (jsmn_tokens[child_index].type == JSMN_ARRAY)
      child_count += jsmn_tokens[child_index].size;
    else if (jsmn_tokens[child_index].type == JSMN_OBJECT)
      child_count += jsmn_tokens[child_index].size * 2;

    child_count--;
    child_index++;
  }

  /* Validate that we have visited correct number of children */
  if (child_count != 0)
    return JSMNITER_ERR_BROKEN;

  /* Points to last index inside Array/Object */
  return (int)child_index - 1;
}


/**
 * @brief Initialize iterator
 * @details Set initial value for iterator struct
 * 
 * @param iterator      Iterator struct
 * @param jsmn_tokens   JSMN tokens
 * @param jsmn_len      JSMN token count
 * @param parser_pos    Current JSMN token
 * 
 * @return  < 0 - Error has occured, corresponds to one of JSMNITER_ERR_*
 *          >=0 - Ok
 */
int jsmn_iterator_init(jsmn_iterator_t *iterator, jsmntok_t *jsmn_tokens, unsigned int jsmn_len, unsigned int parser_pos) {
  /* No iterator */
  if (!iterator)
    return JSMNITER_ERR_PARAMETER;
  /* No tokens */
  if (!jsmn_tokens)
    return JSMNITER_ERR_PARAMETER;
  /* pos outside tokens */
  if (parser_pos >= jsmn_len)
    return JSMNITER_ERR_PARAMETER;
  /* Not an Array/Object */
  if (jsmn_tokens[parser_pos].type != JSMN_ARRAY && 
      jsmn_tokens[parser_pos].type != JSMN_OBJECT)
    return JSMNITER_ERR_TYPE;

  /* Save jsmn pointer */
  iterator->jsmn_tokens = jsmn_tokens;
  iterator->jsmn_len    = jsmn_len;

  iterator->parent_pos  = parser_pos;
  iterator->parser_pos  = parser_pos;

  iterator->index  = 0;
  return 0;
}


/**
 * @brief Get next item in JSMN Array/Object
 * @details Gets JSMN position for next identifier and value in Array/Object
 * 
 * @param iterator            Iterator struct
 * @param jsmn_identifier     Return pointer for identifier, NULL for Array
 * @param jsmn_value          Return pointer for value
 * @param next_value_index    Possible to indicate where next value begins, allows determine end of sub
 *                            Array/Object withouth manually searching for it
 * 
 * @return  < 0 - Error has occured, corresponds to one of JSMNITER_ERR_*
 *            0 - No more values
 *          > 0 - Value (and identifier) has been returned
 */
 int jsmn_iterator_next(jsmn_iterator_t *iterator, jsmntok_t **jsmn_identifier, jsmntok_t **jsmn_value, 
                        unsigned int next_value_index) {
  unsigned int is_object;
  jsmntok_t *parent_item;
  jsmntok_t *current_item;
  jsmntok_t *jsmn_tokens;
  unsigned int jsmn_len;

  /* No iterator */
  if (!iterator)
    return JSMNITER_ERR_PARAMETER;
  /* No value return pointer */
  if (!jsmn_value)
    return JSMNITER_ERR_PARAMETER;
  /* Parser position is outside JSMN tokens array */
  if (iterator->parser_pos > iterator->jsmn_len)
    return JSMNITER_ERR_BROKEN;

  jsmn_tokens = iterator->jsmn_tokens;
  jsmn_len = iterator->jsmn_len;
  parent_item  = &jsmn_tokens[iterator->parent_pos];

  /* parser_position is at the end of JSMN token array or points outside parent Array/Object */
  if (jsmn_iterator_position(iterator) == iterator->jsmn_len || jsmn_tokens[jsmn_iterator_position(iterator)].start > parent_item->end) {
    if (iterator->index != (unsigned int)parent_item->size)
      return JSMNITER_ERR_BROKEN;
    return 0;
  }

  current_item = &jsmn_tokens[jsmn_iterator_position(iterator)];

  /* Are we in an Object */
  is_object = (parent_item->type == JSMN_OBJECT ? 1 : 0);

  /* Is it item we only need jump one to the next index */
  if (jsmn_iterator_position(iterator) == iterator->parent_pos) {
    next_value_index = jsmn_iterator_position(iterator) + 1;
  }
  /* For items that isn't Array/Object we only need to take the next value */
  else if (current_item->type != JSMN_ARRAY &&
           current_item->type != JSMN_OBJECT) {
    next_value_index = jsmn_iterator_position(iterator) + 1;
  }
  /* Check if next_value_index is correct, else we need to calculate it ourself */
  else if (next_value_index == 0 ||
           next_value_index > jsmn_len || 
           next_value_index <= jsmn_iterator_position(iterator) ||
           current_item->end < jsmn_tokens[next_value_index - 1].end ||
           (next_value_index < jsmn_len && current_item->end >= jsmn_tokens[next_value_index].start)) {
    /* Find index for last item in the Array/Object manually */
    int return_pos = jsmn_iterator_find_last(jsmn_tokens, jsmn_len, jsmn_iterator_position(iterator));

    /* Error, bail out */
    if (return_pos < 0)
      return return_pos;
  
    /* Update position to the next item token */
    next_value_index = (unsigned int)(return_pos) + 1;

    /* Outside valid array */
    if (next_value_index > jsmn_len)
      return JSMNITER_ERR_BROKEN;
    /* Earlier than current value (not valid jsmn tree) */
    if (next_value_index <= jsmn_iterator_position(iterator))
      return JSMNITER_ERR_BROKEN;
    /* Previus item is NOT inside current Array/Object */
    if (jsmn_tokens[next_value_index - 1].end > current_item->end)
      return JSMNITER_ERR_BROKEN;
    /* Not last item and next item is NOT outside Array/Object */
    if (next_value_index < jsmn_len && current_item->end > jsmn_tokens[next_value_index].start)
      return JSMNITER_ERR_BROKEN;
  }

  /* Parser position is outside JSMN tokens array */
  if (next_value_index > iterator->jsmn_len)
    return JSMNITER_ERR_BROKEN;

  /* parser_position is at the end of JSMN token array or points outside parent Array/Object */
  if (next_value_index == (unsigned int)iterator->jsmn_len || jsmn_tokens[next_value_index].start > parent_item->end) {
    if (iterator->index != (unsigned int)parent_item->size)
      return JSMNITER_ERR_BROKEN;
    /* Update parser position before exit */
    iterator->parser_pos = next_value_index;
    return 0;
  }

  /* Get current value/identifier */
  current_item = &jsmn_tokens[next_value_index];

  /* We have identifier, read it */
  if (is_object) {
    /* Must be string */
    if (current_item->type != JSMN_STRING)
      return JSMNITER_ERR_NOIDENT;
    /* Ensure that we have next token */
    if (next_value_index + 1 >= jsmn_len)
      return JSMNITER_ERR_BROKEN;
    /* Missing identifier pointer */
    if (!jsmn_identifier)
      return JSMNITER_ERR_PARAMETER;
    
    /* Set identifier and update current pointer to value item */
    *jsmn_identifier = current_item;
    next_value_index++;
    current_item = &jsmn_tokens[next_value_index];
  }
  /* Clear identifier if is set */
  else if (jsmn_identifier) {
    *jsmn_identifier = NULL;
  }
  
  /* Set value */
  *jsmn_value = current_item;

  /* Update parser position */
  iterator->parser_pos = next_value_index;

  /* Increase the index and return it as the positive value */
  iterator->index++;
  return (int)iterator->index;
}
#endif // DQN_JSMN_IMPLEMENTATION
