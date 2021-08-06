#if !defined(DQN_META_DESK_H)
#define DQN_META_DESK_H
// -----------------------------------------------------------------------------
// NOTE: Dqn_MetaDesk
// -----------------------------------------------------------------------------
// Contains helpers functions to code generate formatted CPP files using
// Dion-System's MetaDesk library.
//
// MetaDesk must be available under the include path at
// "metadesk/source/md.[c|h]" when this file is compiled.
//
// -----------------------------------------------------------------------------
// NOTE: Configuration
// -----------------------------------------------------------------------------
// #define DQN_META_DESK_IMPLEMENTATION
//     Define this in one and only one C++ file to enable the implementation
//     code of the header file.
//
// #define DQN_META_DESK_STANDALONE_PROGRAM
//     Define this to enable a "main" function allowing this translation unit to
//     be compiled into an executable, by default this program will generate all
//     the possible code generation outputs. This file's implementation must
//     also be available to this translation unit.
//
// -----------------------------------------------------------------------------
// NOTE: Dqn_MetaDesk Header File
// -----------------------------------------------------------------------------
#if !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS
    #define DQN_MD_UNDEFINE_CRT_WARNING
#endif

#include <stdio.h>
#include <stdarg.h>
#include "metadesk/source/md.h"

#define DQN_MD_ASSERT_MSG(expr, fmt, ...)                                                                              \
    if (!(expr))                                                                                                       \
    {                                                                                                                  \
        fprintf(stderr, "Assertion triggered at %s line %d. ", __FILE__, __LINE__);                                    \
        fprintf(stderr, fmt, ##__VA_ARGS__);                                                                           \
        fputc('\n', stderr);                                                                                           \
        *((int *)0);                                                                                                   \
    }

#define DQN_MD_INVALID_CODE_PATH do { DQN_MD_ASSERT_MSG(0, "Invalid code path") } while (0)
#define DQN_MD_INVALID_CODE_PATH_MSG(fmt, ...) do { DQN_MD_ASSERT_MSG(0, fmt, __VA_ARGS__) } while(0)
#define DQN_MD_CAST(Type) (Type)

// -----------------------------------------------------------------------------
// NOTE: Dqn_MDCppFile: Helper functions to generate formatted CPP files
// -----------------------------------------------------------------------------
struct Dqn_MDCppFile
{
    FILE *file;
    int   indent;
    int   space_per_indent;
    bool  append_extra_new_line;
};

int     Dqn_MDCppFile_SpacePerIndent(Dqn_MDCppFile *cpp);
void    Dqn_MDCppFile_LineBeginV    (Dqn_MDCppFile *cpp, char const *fmt, va_list args);
void    Dqn_MDCppFile_LineBegin     (Dqn_MDCppFile *cpp, char const *fmt, ...);
void    Dqn_MDCppFile_LineEnd       (Dqn_MDCppFile *cpp, char const *fmt, ...);
void    Dqn_MDCppFile_LineAdd       (Dqn_MDCppFile *cpp, char const *fmt, ...);
void    Dqn_MDCppFile_LineV         (Dqn_MDCppFile *cpp, char const *fmt, va_list args);
void    Dqn_MDCppFile_Line          (Dqn_MDCppFile *cpp, char const *fmt, ...);
void    Dqn_MDCppFile_NewLine       (Dqn_MDCppFile *cpp);
void    Dqn_MDCppFile_Indent        (Dqn_MDCppFile *cpp);
void    Dqn_MDCppFile_Unindent      (Dqn_MDCppFile *cpp);
void    Dqn_MDCppFile_BeginBlock    (Dqn_MDCppFile *cpp, char const *fmt, ...);
void    Dqn_MDCppFile_EndBlock      (Dqn_MDCppFile *cpp, bool trailing_semicolon, bool new_line_on_next_block);
#define Dqn_MDCppFile_EndEnumBlock(cpp) Dqn_MDCppFile_EndBlock(cpp, true /*trailing_semicolon*/, true /*new_line_on_next_block*/)
#define Dqn_MDCppFile_EndForBlock(cpp) Dqn_MDCppFile_EndBlock(cpp, false /*trailing_semicolon*/, false /*new_line_on_next_block*/)
#define Dqn_MDCppFile_EndIfBlock(cpp) Dqn_MDCppFile_EndBlock(cpp, false /*trailing_semicolon*/, false /*new_line_on_next_block*/)
#define Dqn_MDCppFile_EndFuncBlock(cpp) Dqn_MDCppFile_EndBlock(cpp, false /*trailing_semicolon*/, true /*new_line_on_next_block*/)
#define Dqn_MDCppFile_EndStructBlock(cpp) Dqn_MDCppFile_EndBlock(cpp, true /*trailing_semicolon*/, true /*new_line_on_next_block*/)

// -----------------------------------------------------------------------------
// NOTE: Dqn_MD: Code generating helper
// -----------------------------------------------------------------------------
enum Dqn_MD_CodeGenFlag
{
    Dqn_MD_CodeGenFlag_CEnum                   = (1 << 0),
    Dqn_MD_CodeGenFlag_CppStruct               = (1 << 1),
    Dqn_MD_CodeGenFlag_ImGuiFunction           = (1 << 2),
    Dqn_MD_CodeGenFlag_JsonParsingFunction     = (1 << 3),
    Dqn_MD_CodeGenFlag_CurlQueryFunction       = (1 << 4),
    Dqn_MD_CodeGenFlag_JsonEndpointURLFunction = (1 << 5),
    Dqn_MD_CodeGenFlag_JsonRPCPostDataFunction = (1 << 5),
    Dqn_MD_CodeGenFlag_PrintFunction           = (1 << 6),
    Dqn_MD_CodeGenFlag_All                     = (0xFFFFFFFF),
};

// file_path: The path of the file to feed into the code generator to parse and spit out code.
// output_name: The name to assign the .cpp/.h file that is generated from the
// function, i.e. <output_name>.h and <output_name>.cpp
void Dqn_MD_CodeGen(char const *file_path, char const *output_name, unsigned int flags);

// -----------------------------------------------------------------------------
// NOTE: Dqn_MD: Code generating functions
// -----------------------------------------------------------------------------
/*
Generate a CPP struct for the given 'struct_node'. Children nodes of the
'struct_node' must have a 'type' child associated with it.

Input
    INS_CoinGecko_Coin:
    {
        id:              {type: string},
        symbol:          {type: string},
        name:            {type: string},
        market_cap_rank: {type: u16},
        market_data:     {type: INS_CoinGecko_CoinMarketData},
    }

Output
    struct CoinGecko_CoinMarketDataPrice
    {
        Dqn_f32 aud;
        Dqn_f32 btc;
        Dqn_f32 eth;
        Dqn_f32 usd;
    };
*/
void Dqn_MD_GenerateCppStruct(Dqn_MDCppFile *cpp, MD_Node const *struct_node);

/*
Generate for any root MetaDesk node tagged with the 'required_tag'
required_tag: The tag that must be attached to the MetaDesk node for an enum to be generated for it
name: The name of the enum to generate

Input: (With name = "INS_QueryType", required_tag = "json_endpoint")
    char const EXAMPLE[] = R"(@json_endpoint Etherscan_TXListForAddress: {}
                              @json_endpoint CoinGecko_Coin: {}");

Output
    enum INS_QueryType
    {
        INS_QueryType_Etherscan_TXListForAddress,
        INS_QueryType_CoinGecko_Coin,
        INS_QueryType_Count
    };
*/
void Dqn_MD_GenerateCEnum(Dqn_MDCppFile *cpp, MD_ParseResult *parse, MD_String8 required_tag, char const *name);

/*
Generate ImGui widgets for visualising the given 'struct_node'. If a custom type
is provided, we assume that an ImGui function has been previously generated for
the type already.

Input
    INS_CoinGecko_Coin:
    {
        id:              {type: string,                       json_type: string},
        symbol:          {type: string,                       json_type: string},
        name:            {type: string,                       json_type: string},
        market_cap_rank: {type: u16,                          json_type: number},
        market_data:     {type: INS_CoinGecko_CoinMarketData, json_type: object},
    }

Output
    void INS_CoinGecko_CoinImGui(INS_CoinGecko_Coin const *val){
        ImGui::Text("id: %.*s", DQN_STRING_FMT(val->id));
        ImGui::Text("symbol: %.*s", DQN_STRING_FMT(val->symbol));
        ImGui::Text("name: %.*s", DQN_STRING_FMT(val->name));
        ImGui::Text("market_cap_rank: %I64u", val->market_cap_rank);
        INS_CoinGecko_CoinMarketDataImGui(&val->market_data);
    }
*/
void Dqn_MD_GenerateImGuiFunction(Dqn_MDCppFile *cpp, MD_Node const *struct_node, bool header_file);

/*
Generate for any root MetaDesk node tagged with 'json_endpoint'. The MetaDesk
node containing the 'json_endpoint' can specify fields that the JSON parsing
function will attempt to pull out from the provided JSON string, powered by the
library jsmn.h. This function requires "Dqn_Jsmn.h/cpp" to be present in the
project using this generated code.

Input
    char const EXAMPLE[] = R"(
    Etherscan_TXListForAddressEntry:
    {
        block_number:        {type:  u64,    json_key: "blockNumber",       json_type: string},
        nonce:               {type:  u64,                                   json_type: string},
        block_hash:          {type:  string, json_key: "blockHash",         json_type: string},
    }

    @json_endpoint(url: "http://api.etherscan.io/api?module=account&action=txlist&address=%.*s&startblock=0&endblock=99999999&sort=asc&apikey=%.*s"
                   params: {eth_address:string, api_key: string})
    Etherscan_TXListForAddress:
    {
        status:  {type: bool,   json_type: string},
        message: {type: string, json_type: string},
        result:  {type: Etherscan_TXListForAddressEntry[], json_type: array},
    }");

Output: (Truncated for brevity)
    Etherscan_TXListForAddress Etherscan_TXListForAddressParseTokens(jsmntok_t **start_it, Dqn_JsmnErrorHandle *err_handle, Dqn_String json, Dqn_ArenaAllocator *arena)
    {
        INS_Etherscan_TXListForAddress result = {};
        jsmntok_t *it = (*start_it);
        if (!Dqn_JsmnToken_ExpectObject(*it, err_handle)) return result;
        jsmntok_t *object = it++;

        for (int index = 0; index < object->size; index++)
        {
            jsmntok_t *key     = it++;
            jsmntok_t *value   = it++;
            Dqn_String key_str = Dqn_JsmnToken_String(json, *key);
            Dqn_String value_str = Dqn_JsmnToken_String(json, *value); (void)value_str;

            if (key_str == DQN_STRING("status"))
            {
                if (Dqn_JsmnToken_ExpectString(*value, err_handle))
                {
                    result.status = DQN_CAST(Dqn_b32)Dqn_String_ToU64(value_str);
                }
            }
            ...
            else if (key_str == DQN_STRING("result"))
            {
                if (Dqn_JsmnToken_ExpectArray(*value, err_handle))
                {
                    result.result = Dqn_Array_InitWithArenaNoGrow(arena, INS_Etherscan_TXListForAddressEntry, value->size, 0, Dqn_ZeroMem::Yes);
                    for (int array_index = 0; array_index < value->size; array_index++)
                    {
                        auto *result_entry = Dqn_Array_Make(&result.result, 1);
                        *result_entry = INS_Etherscan_TXListForAddressEntryParseTokens(&it, err_handle, json, arena);
                    }
                }
            }
            ...
        }
        *start_it = it;
        return result;
    }

*/
void Dqn_MD_GenerateJsonParsingFunction(Dqn_MDCppFile *cpp, MD_Node const *struct_node, bool header_file);
void Dqn_MD_GenerateCurlQueryFunction(Dqn_MDCppFile *cpp, MD_Node *struct_node, bool header_file);


/*
Generate for any root MetaDesk node with the following tags defined
    @json_endpoint(url: <...>, params: {<...>})

Input
    char const EXAMPLE[] = R"(
    @json_endpoint(url: "http://api.etherscan.io/api?module=account&action=txlist&address=%.*s&startblock=0&endblock=99999999&sort=asc&apikey=%.*s"
                   params: {eth_address:string, api_key: string})
    Etherscan_TXListForAddress:
    {
    }");

Output
    Dqn_String Etherscan_TXListForAddressURL(Dqn_ArenaAllocator *arena, Dqn_String eth_address, Dqn_String api_key)
    {
        Dqn_String result = Dqn_String_InitArenaFmt(arena, "http://api.etherscan.io/api?module=account&action=txlist&address=%.*s&startblock=0&endblock=99999999&sort=asc&apikey=%.*s", DQN_STRING_FMT(eth_address), DQN_STRING_FMT(api_key));
        DQN_HARD_ASSERT(result.str);
        return result;
    }
*/
void Dqn_MD_GenerateJsonEndpointURLFunction(Dqn_MDCppFile *cpp, MD_Node *struct_node, bool header_file);
void Dqn_MD_GenerateJsonRPCPostDataFunction(Dqn_MDCppFile *cpp, MD_Node *root, MD_Node *struct_node, bool header_file);

void Dqn_MD_GeneratePrintFunction(Dqn_MDCppFile *cpp, MD_Node const *struct_node, bool header_file);

#if defined(DQN_MD_UNDEFINE_CRT_WARNING)
    #undef _CRT_SECURE_NO_WARNINGS
#endif
#endif // DQN_META_DESK_H

#if defined(DQN_META_DESK_IMPLEMENTATION)
// -----------------------------------------------------------------------------
// NOTE: Implementation
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// NOTE: Implementation Defines
// -----------------------------------------------------------------------------
#if defined(__clang__)
    #pragma clang diagnostic ignored "-Wwritable-strings"
#endif

// -----------------------------------------------------------------------------
// NOTE: MetaDesk Defines
// -----------------------------------------------------------------------------
// NOTE: Warning! Order is important here, clang-cl on Windows defines _MSC_VER
#if defined(__clang__)
#elif defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4244) // '=': conversion from 'MD_u64' to 'unsigned int', possible loss of data
    #pragma warning(disable: 4101) // 'consume': unreferenced local variable
#endif

#include "metadesk/source/md.c"

#if defined(__clang__)
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif

// -----------------------------------------------------------------------------
// NOTE: Dqn_MDCppFile Implementation
// -----------------------------------------------------------------------------
int Dqn_MDCppFile_SpacePerIndent(Dqn_MDCppFile *cpp)
{
    int result = cpp->space_per_indent == 0 ? 4 : cpp->space_per_indent;
    return result;
}

void Dqn_MDCppFile_LineBeginV(Dqn_MDCppFile *cpp, char const *fmt, va_list args)
{
    int spaces = cpp->indent * Dqn_MDCppFile_SpacePerIndent(cpp);
    fprintf(cpp->file, "%*s", spaces, "");
    vfprintf(cpp->file, fmt, args);
}

void Dqn_MDCppFile_LineBegin(Dqn_MDCppFile *cpp, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_MDCppFile_LineBeginV(cpp, fmt, args);
    va_end(args);
}

void Dqn_MDCppFile_LineEnd(Dqn_MDCppFile *cpp, char const *fmt, ...)
{
    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        vfprintf(cpp->file, fmt, args);
        va_end(args);
    }

    fputc('\n', cpp->file);
}

void Dqn_MDCppFile_LineAdd(Dqn_MDCppFile *cpp, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(cpp->file, fmt, args);
    va_end(args);
}

void Dqn_MDCppFile_LineV(Dqn_MDCppFile *cpp, char const *fmt, va_list args)
{
    Dqn_MDCppFile_LineBeginV(cpp, fmt, args);
    Dqn_MDCppFile_LineEnd(cpp, nullptr);
}

void Dqn_MDCppFile_Line(Dqn_MDCppFile *cpp, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_MDCppFile_LineBeginV(cpp, fmt, args);
    Dqn_MDCppFile_LineEnd(cpp, nullptr);
    va_end(args);
}

void Dqn_MDCppFile_NewLine(Dqn_MDCppFile *cpp)
{
    fputc('\n', cpp->file);
}

void Dqn_MDCppFile_Indent(Dqn_MDCppFile *cpp)
{
    cpp->indent++;
}

void Dqn_MDCppFile_Unindent(Dqn_MDCppFile *cpp)
{
    cpp->indent--;
    MD_Assert(cpp->indent >= 0);
}

void Dqn_MDCppFile_BeginBlock(Dqn_MDCppFile *cpp, char const *fmt, ...)
{
    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        Dqn_MDCppFile_LineV(cpp, fmt, args);
        va_end(args);
    }

    Dqn_MDCppFile_Line(cpp, "{");
    Dqn_MDCppFile_Indent(cpp);
}

void Dqn_MDCppFile_EndBlock(Dqn_MDCppFile *cpp, bool trailing_semicolon, bool new_line_on_next_block)
{
    Dqn_MDCppFile_Unindent(cpp);
    Dqn_MDCppFile_Line(cpp, trailing_semicolon ? "};" : "}");
    if (new_line_on_next_block) fputc('\n', cpp->file);
}

// -----------------------------------------------------------------------------
// MD Helpers
// -----------------------------------------------------------------------------
static bool Dqn_MD__NodeHasArrayNotation(MD_Node const *node)
{
    unsigned node_brackets = MD_NodeFlag_HasBracketLeft | MD_NodeFlag_HasBracketRight;
    bool     result        = (node->flags & node_brackets) == node_brackets;
    return result;
}

// -----------------------------------------------------------------------------
// Code
// -----------------------------------------------------------------------------
void Dqn_MD_CodeGen(char const *file_path, char const *output_name, unsigned int flags)
{
    MD_String8 h_file_name   = MD_S8Fmt("%s.h", output_name);
    MD_String8 cpp_file_name = MD_S8Fmt("%s.cpp", output_name);

    Dqn_MDCppFile cpp    = {};
    Dqn_MDCppFile header = {};
    cpp.file             = fopen(DQN_MD_CAST(char const *)cpp_file_name.str, "w+b");
    header.file          = fopen(DQN_MD_CAST(char const *)h_file_name.str, "w+b");

    fprintf(stdout, "Generating meta file from %s to %.*s and %.*s\n", file_path, MD_S8VArg(cpp_file_name), MD_S8VArg(h_file_name));

    if (cpp.file && header.file)
    {
        MD_ParseResult parse = MD_ParseWholeFile(MD_S8CString(DQN_MD_CAST(char *)file_path));
        if (flags & Dqn_MD_CodeGenFlag_CEnum)
            Dqn_MD_GenerateCEnum(&header, &parse, MD_S8Lit("json_endpoint"), "INS_QueryType");

        for (MD_EachNode(struct_node, parse.node->first_child))
        {
            if (flags & Dqn_MD_CodeGenFlag_CppStruct)
                Dqn_MD_GenerateCppStruct(&header, struct_node);

            if (flags & Dqn_MD_CodeGenFlag_ImGuiFunction)
                Dqn_MD_GenerateImGuiFunction(&header, struct_node, true /*header_file*/);

            if (flags & Dqn_MD_CodeGenFlag_JsonParsingFunction)
                Dqn_MD_GenerateJsonParsingFunction(&header, struct_node, true /*header_file*/);

            if (flags & Dqn_MD_CodeGenFlag_CurlQueryFunction)
                Dqn_MD_GenerateCurlQueryFunction(&header, struct_node, true /*header_file*/);

            if (flags & Dqn_MD_CodeGenFlag_JsonEndpointURLFunction)
                Dqn_MD_GenerateJsonEndpointURLFunction(&header, struct_node, true /*header_file*/);

            if (flags & Dqn_MD_CodeGenFlag_JsonRPCPostDataFunction)
                Dqn_MD_GenerateJsonRPCPostDataFunction(&header, parse.node, struct_node, true /*header_file*/);

            if (flags & Dqn_MD_CodeGenFlag_PrintFunction)
                Dqn_MD_GeneratePrintFunction(&header, struct_node, true /*header_file*/);

            Dqn_MDCppFile_NewLine(&header);

            if (flags & Dqn_MD_CodeGenFlag_ImGuiFunction)
                Dqn_MD_GenerateImGuiFunction(&cpp, struct_node, false /*header_file*/);

            if (flags & Dqn_MD_CodeGenFlag_JsonParsingFunction)
                Dqn_MD_GenerateJsonParsingFunction(&cpp, struct_node, false /*header_file*/);

            if (flags & Dqn_MD_CodeGenFlag_CurlQueryFunction)
                Dqn_MD_GenerateCurlQueryFunction(&cpp, struct_node, false /*header_file*/);

            if (flags & Dqn_MD_CodeGenFlag_JsonEndpointURLFunction)
                Dqn_MD_GenerateJsonEndpointURLFunction(&cpp, struct_node, false /*header_file*/);

            if (flags & Dqn_MD_CodeGenFlag_JsonRPCPostDataFunction)
                Dqn_MD_GenerateJsonRPCPostDataFunction(&cpp, parse.node, struct_node, false /*header_file*/);

            if (flags & Dqn_MD_CodeGenFlag_PrintFunction)
                Dqn_MD_GeneratePrintFunction(&cpp, struct_node, false /*header_file*/);
        }
    }

    if (cpp.file) fclose(cpp.file);
    if (header.file) fclose(header.file);
}

MD_String8 Dqn_MD__ConvertTypeToCppType(MD_String8 type)
{
    MD_String8 result = type;
    if      (MD_S8Match(type, MD_S8Lit("u8"), 0))     { result = MD_S8Lit("Dqn_u8"); }
    else if (MD_S8Match(type, MD_S8Lit("u16"), 0))    { result = MD_S8Lit("Dqn_u16"); }
    else if (MD_S8Match(type, MD_S8Lit("u32"), 0))    { result = MD_S8Lit("Dqn_u32"); }
    else if (MD_S8Match(type, MD_S8Lit("u64"), 0))    { result = MD_S8Lit("Dqn_u64"); }
    else if (MD_S8Match(type, MD_S8Lit("u128"), 0))   { result = MD_S8Lit("intx::uint128"); }
    else if (MD_S8Match(type, MD_S8Lit("i8"), 0))     { result = MD_S8Lit("Dqn_i8"); }
    else if (MD_S8Match(type, MD_S8Lit("i16"), 0))    { result = MD_S8Lit("Dqn_i16"); }
    else if (MD_S8Match(type, MD_S8Lit("i32"), 0))    { result = MD_S8Lit("Dqn_i32"); }
    else if (MD_S8Match(type, MD_S8Lit("i64"), 0))    { result = MD_S8Lit("Dqn_i64"); }
    else if (MD_S8Match(type, MD_S8Lit("f32"), 0))    { result = MD_S8Lit("Dqn_f32"); }
    else if (MD_S8Match(type, MD_S8Lit("f64"), 0))    { result = MD_S8Lit("Dqn_f64"); }
    else if (MD_S8Match(type, MD_S8Lit("string"), 0)) { result = MD_S8Lit("Dqn_String"); }
    else if (MD_S8Match(type, MD_S8Lit("bool"), 0))   { result = MD_S8Lit("Dqn_b32"); }
    return result;
}

bool Dqn_MD__IsTypeCppPrimitive(MD_String8 type)
{
    MD_String8 cpp_type = Dqn_MD__ConvertTypeToCppType(type);
    bool result = !MD_S8Match(cpp_type, type, 0);
    // TODO(doyle): We weren't able to convert the type to our Dqn_* type/custom
    // type, we assume that this is a custom CPP type we don't ordinarily
    // recognize.
    return result;
}

bool Dqn_MD__TypeIsUnsignedInt(MD_String8 type)
{
    bool result = MD_S8Match(type, MD_S8Lit("u8"), 0) ||
                  MD_S8Match(type, MD_S8Lit("u16"), 0) ||
                  MD_S8Match(type, MD_S8Lit("u32"), 0) ||
                  MD_S8Match(type, MD_S8Lit("u64"), 0) ||
                  MD_S8Match(type, MD_S8Lit("u128"), 0);
    return result;
}

bool Dqn_MD__TypeIsSignedInt(MD_String8 type)
{
    bool result = MD_S8Match(type, MD_S8Lit("i8"), 0) ||
                  MD_S8Match(type, MD_S8Lit("i16"), 0) ||
                  MD_S8Match(type, MD_S8Lit("i32"), 0) ||
                  MD_S8Match(type, MD_S8Lit("i64"), 0) ||
                  MD_S8Match(type, MD_S8Lit("i128"), 0);
    return result;
}

MD_String8 Dqn_MD__ConvertTypeToFmtString(MD_String8 type)
{
    MD_String8 result = type;
    if      (MD_S8Match(type, MD_S8Lit("u8"), 0))     { result = MD_S8Lit("%I64u"); }
    else if (MD_S8Match(type, MD_S8Lit("u16"), 0))    { result = MD_S8Lit("%I64u"); }
    else if (MD_S8Match(type, MD_S8Lit("u32"), 0))    { result = MD_S8Lit("%I64u"); }
    else if (MD_S8Match(type, MD_S8Lit("u64"), 0))    { result = MD_S8Lit("%I64u"); }
    else if (MD_S8Match(type, MD_S8Lit("i8"), 0))     { result = MD_S8Lit("%I64d"); }
    else if (MD_S8Match(type, MD_S8Lit("i16"), 0))    { result = MD_S8Lit("%I64d"); }
    else if (MD_S8Match(type, MD_S8Lit("i32"), 0))    { result = MD_S8Lit("%I64d"); }
    else if (MD_S8Match(type, MD_S8Lit("i64"), 0))    { result = MD_S8Lit("%I64d"); }
    else if (MD_S8Match(type, MD_S8Lit("f32"), 0))    { result = MD_S8Lit("%f"); }
    else if (MD_S8Match(type, MD_S8Lit("f64"), 0))    { result = MD_S8Lit("%f"); }
    else if (MD_S8Match(type, MD_S8Lit("string"), 0)) { result = MD_S8Lit("%.*s"); }
    else if (MD_S8Match(type, MD_S8Lit("bool"), 0))   { result = MD_S8Lit("%s"); }
    return result;
}

void Dqn_MD_GenerateCppStruct(Dqn_MDCppFile *cpp, MD_Node const *struct_node)
{
    if (!MD_NodeHasTag(DQN_MD_CAST(MD_Node *)struct_node, MD_S8Lit("cpp_struct"), DQN_MD_CAST(MD_MatchFlags)0))
        return;

    Dqn_MDCppFile_BeginBlock(cpp, "struct %.*s", MD_S8VArg(struct_node->string));
    for (MD_EachNode(field, struct_node->first_child))
    {
        MD_Node *type_node = MD_NodeFromString(field->first_child, field->last_child + 1, MD_S8Lit("type"), (MD_MatchFlags)0);
        MD_String8 type    = type_node->first_child->string;
        MD_Assert(!MD_NodeIsNil(type_node));

        MD_String8 cpp_type = Dqn_MD__ConvertTypeToCppType(type);
        bool const is_array = Dqn_MD__NodeHasArrayNotation(type_node->first_child->next);
        if (is_array)
        {
            Dqn_MDCppFile_Line(cpp, "Dqn_Array<%.*s> %.*s;", MD_S8VArg(cpp_type), MD_S8VArg(field->string));
        }
        else
        {
            Dqn_MDCppFile_Line(cpp, "%.*s %.*s;", MD_S8VArg(cpp_type), MD_S8VArg(field->string));
        }
    }
    Dqn_MDCppFile_EndStructBlock(cpp);
}

void Dqn_MD_GenerateCEnum(Dqn_MDCppFile *cpp, MD_ParseResult *parse, MD_String8 required_tag, char const *name)
{
    Dqn_MDCppFile_BeginBlock(cpp, "enum %s", name);
    for (MD_EachNode(struct_node, parse->node->first_child))
    {
        if (MD_NodeHasTag(struct_node, required_tag, DQN_MD_CAST(MD_MatchFlags)0))
            Dqn_MDCppFile_Line(cpp, "%s_%.*s,", name, MD_S8VArg(struct_node->string));
    }
    Dqn_MDCppFile_Line(cpp, "%s_Count", name);
    Dqn_MDCppFile_EndEnumBlock(cpp);
}

void Dqn_MD_GeneratePrintFunction(Dqn_MDCppFile *cpp, MD_Node const *struct_node, bool header_file)
{
    if (!MD_NodeHasTag(DQN_MD_CAST(MD_Node *)struct_node, MD_S8Lit("cpp_struct"), DQN_MD_CAST(MD_MatchFlags)0) ||
        !MD_NodeHasTag(DQN_MD_CAST(MD_Node *)struct_node, MD_S8Lit("cpp_print"), DQN_MD_CAST(MD_MatchFlags)0))
    {
        return;
    }

    const MD_String8 VAL_STR = MD_S8Lit("val");
    Dqn_MDCppFile_LineBegin(cpp,
                            "void %.*sPrint(FILE *file, %.*s const *%.*s",
                            MD_S8VArg(struct_node->string),
                            MD_S8VArg(struct_node->string),
                            MD_S8VArg(VAL_STR));

    if (header_file)
    {
        Dqn_MDCppFile_LineEnd(cpp, ");");
        return;
    }

    Dqn_MDCppFile_LineEnd(cpp, ")");
    Dqn_MDCppFile_BeginBlock(cpp, nullptr);
    for (MD_EachNode(field_node, struct_node->first_child))
    {
        MD_Node *type_node = MD_NodeFromString(field_node->first_child, field_node->last_child + 1, MD_S8Lit("type"), (MD_MatchFlags)0);
        MD_String8 type    = type_node->first_child->string;
        MD_String8 field   = field_node->string;
        MD_Assert(!MD_NodeIsNil(type_node));

        MD_String8 cpp_type = Dqn_MD__ConvertTypeToCppType(type);
        bool is_array = Dqn_MD__NodeHasArrayNotation(type_node->first_child->next);
        MD_String8 param = VAL_STR;
        MD_String8 variable_to_print = is_array ? MD_S8Lit("it") : MD_S8Fmt("%.*s->%.*s", MD_S8VArg(param), MD_S8VArg(field));
        if (is_array)
            Dqn_MDCppFile_BeginBlock(cpp, "for (%.*s const &it : %.*s->%.*s)", MD_S8VArg(cpp_type), MD_S8VArg(param), MD_S8VArg(field));

        if      (MD_S8Match(type, MD_S8Lit("u8"), 0))     Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%u\n", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("u16"), 0))    Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%u\n", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("u32"), 0))    Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%u\n", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("u64"), 0))    Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%zu\n", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("u128"), 0))   Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%.*s\n", Dqn_U128_String(%.*s, true /*separate*/, ','));)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("i8"), 0))     Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%d\n", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("i16"), 0))    Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%d\n", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("i32"), 0))    Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%d\n", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("i64"), 0))    Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%zd\n", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("f32"), 0))    Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%f\n", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("f64"), 0))    Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%f\n", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("string"), 0)) Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%.*s\n", DQN_STRING_FMT(%.*s));)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("bool"), 0))   Dqn_MDCppFile_Line(cpp, R"(fprintf(file, "%.*s: %%s\n", %.*s ? "true" : "false");)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else                                              Dqn_MDCppFile_Line(cpp, R"(%.*sPrint(file, &%.*s);)", MD_S8VArg(type), MD_S8VArg(variable_to_print));

        if (is_array)
            Dqn_MDCppFile_EndForBlock(cpp);

    }
    Dqn_MDCppFile_EndFuncBlock(cpp);
}

void Dqn_MD_GenerateImGuiFunction(Dqn_MDCppFile *cpp, MD_Node const *struct_node, bool header_file)
{
    const MD_String8 VAL_STR = MD_S8Lit("val");
    Dqn_MDCppFile_LineBegin(cpp,
                            "void %.*sImGui(%.*s const *%.*s)",
                            MD_S8VArg(struct_node->string),
                            MD_S8VArg(struct_node->string),
                            MD_S8VArg(VAL_STR));

    if (header_file)
    {
        Dqn_MDCppFile_LineEnd(cpp, ";");
        return;
    }

    Dqn_MDCppFile_BeginBlock(cpp, nullptr);
    for (MD_EachNode(field_node, struct_node->first_child))
    {
        MD_Node *type_node = MD_NodeFromString(field_node->first_child, field_node->last_child + 1, MD_S8Lit("type"), (MD_MatchFlags)0);
        MD_String8 type    = type_node->first_child->string;
        MD_String8 field   = field_node->string;
        MD_Assert(!MD_NodeIsNil(type_node));

        MD_String8 cpp_type = Dqn_MD__ConvertTypeToCppType(type);
        bool is_array = Dqn_MD__NodeHasArrayNotation(type_node->first_child->next);
        MD_String8 param = VAL_STR;
        MD_String8 variable_to_print = is_array ? MD_S8Lit("it") : MD_S8Fmt("%.*s->%.*s", MD_S8VArg(param), MD_S8VArg(field));
        if (is_array)
            Dqn_MDCppFile_BeginBlock(cpp, "for (%.*s const &it : %.*s->%.*s)", MD_S8VArg(cpp_type), MD_S8VArg(param), MD_S8VArg(field));

        if      (MD_S8Match(type, MD_S8Lit("u8"), 0))     Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%I64u", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("u16"), 0))    Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%I64u", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("u32"), 0))    Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%I64u", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("u64"), 0))    Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%I64u", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("u128"), 0))   Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%I64u", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("i8"), 0))     Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%I64d", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("i16"), 0))    Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%I64d", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("i32"), 0))    Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%I64d", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("i64"), 0))    Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%I64d", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("f32"), 0))    Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%f", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("f64"), 0))    Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%f", %.*s);)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("string"), 0)) Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%.*s", DQN_STRING_FMT(%.*s));)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else if (MD_S8Match(type, MD_S8Lit("bool"), 0))   Dqn_MDCppFile_Line(cpp, R"(ImGui::Text("%.*s: %%s", %.*s ? "true" : "false");)", MD_S8VArg(field), MD_S8VArg(variable_to_print));
        else                                              Dqn_MDCppFile_Line(cpp, R"(%.*sImGui(&%.*s);)", MD_S8VArg(type), MD_S8VArg(variable_to_print));

        if (is_array)
            Dqn_MDCppFile_EndForBlock(cpp);
    }
    Dqn_MDCppFile_EndFuncBlock(cpp);
}

void Dqn_MD__CppCodeGenDeserializeJSONPrimitiveToken(Dqn_MDCppFile *cpp,
                                                     MD_Node *json_type_node,
                                                     MD_Node *type_node,
                                                     MD_String8 field,
                                                     MD_String8 dest,
                                                     MD_String8 json_value_variable_name)
{
    // NOTE: This function is a little bit more complicated than it'd like to be
    // because we support the JSON specifiying a different type than the native
    // storage stype (i.e. the json could specify a number as a json string i.e.
    // "100" but our CPP struct wants a primitive integer.
    //
    // So we handle that and do that conversion step here.

    // -------------------------------------------------------------------------
    // Determine the source type we're converting from
    // -------------------------------------------------------------------------
    MD_String8 json_type = json_type_node->first_child->string;
    MD_String8 type = type_node->first_child->string;
    enum JsonPrimitive
    {
        Number,
        String,
        Object,
        Bool,
        Array,
    };

    JsonPrimitive json_primitive = {};
    if (MD_S8Match(json_type, MD_S8Lit("number"), (MD_MatchFlags)0))
    {
        json_primitive = JsonPrimitive::Number;
        Dqn_MDCppFile_BeginBlock(cpp, "if (Dqn_JsmnToken_ExpectNumber(json, *%.*s, err_handle))", MD_S8VArg(json_value_variable_name));
    }
    else if (MD_S8Match(json_type, MD_S8Lit("string"), (MD_MatchFlags)0))
    {
        json_primitive = JsonPrimitive::String;
        Dqn_MDCppFile_BeginBlock(cpp, "if (Dqn_JsmnToken_ExpectString(*%.*s, err_handle))", MD_S8VArg(json_value_variable_name));
    }
    else if (MD_S8Match(json_type, MD_S8Lit("bool"), (MD_MatchFlags)0))
    {
        json_primitive = JsonPrimitive::Bool;
        Dqn_MDCppFile_BeginBlock(cpp, "if (Dqn_JsmnToken_ExpectBool(json, *%.*s, err_handle))", MD_S8VArg(json_value_variable_name));
    }
    else if (MD_S8Match(json_type, MD_S8Lit("array"), (MD_MatchFlags)0))
    {
        json_primitive = JsonPrimitive::Array;
        Dqn_MDCppFile_BeginBlock(cpp, "if (Dqn_JsmnToken_ExpectArray(*%.*s, err_handle))", MD_S8VArg(json_value_variable_name));
    }
    else if (MD_S8Match(json_type, MD_S8Lit("object"), (MD_MatchFlags)0))
    {
        json_primitive = JsonPrimitive::Object;
        Dqn_MDCppFile_BeginBlock(cpp, "if (Dqn_JsmnToken_ExpectObject(*%.*s, err_handle))", MD_S8VArg(json_value_variable_name));
    }
    else
        DQN_MD_INVALID_CODE_PATH_MSG("Unhandled json type: '%.*s'. Expected 'array', 'number', 'string' or 'object'", MD_S8VArg(json_type));

    // -------------------------------------------------------------------------
    // Convert the JSON token to our CPP struct's expecting type
    // -------------------------------------------------------------------------
    if (json_primitive == JsonPrimitive::Array)
    {
        MD_String8 cpp_type    = Dqn_MD__ConvertTypeToCppType(type);
        bool const custom_type = MD_S8Match(cpp_type, type, DQN_MD_CAST(MD_MatchFlags)0); // Type was not recognised, i.e we didn't convert it

        Dqn_MDCppFile_Line(cpp, R"(%.*s = Dqn_Array_InitWithArenaNoGrow(arena, %.*s, value->size, 0, Dqn_ZeroMem::Yes);)", MD_S8VArg(dest), MD_S8VArg(cpp_type));
        Dqn_MDCppFile_BeginBlock(cpp, "for (int array_index = 0; array_index < value->size; array_index++)");
        Dqn_MDCppFile_Line(cpp,
                           R"(auto *%.*s_entry = Dqn_Array_Make(&%.*s, 1);)",
                           MD_S8VArg(field),
                           MD_S8VArg(dest));
        if (custom_type)
        {
            Dqn_MDCppFile_Line(cpp,
                               R"(*%.*s_entry = %.*sParseTokens(&it, err_handle, json, arena);)",
                               MD_S8VArg(field),
                               MD_S8VArg(type));
        }
        else
        {
            Dqn_MDCppFile_Line(cpp, "jsmntok_t *sub_value     = it++;");
            Dqn_MDCppFile_Line(cpp, "Dqn_String sub_value_str = Dqn_JsmnToken_String(json, *sub_value); (void)sub_value_str;");

            MD_String8 dest = MD_S8Fmt("*%.*s_entry", MD_S8VArg(field));
            Dqn_MD__CppCodeGenDeserializeJSONPrimitiveToken(cpp, type_node, type_node, field, dest, MD_S8Lit("sub_value"));
        }
        Dqn_MDCppFile_EndForBlock(cpp);
    }
    else if (json_primitive == JsonPrimitive::Object)
    {
        Dqn_MDCppFile_BeginBlock(cpp, "if (Dqn_JsmnToken_ExpectObject(*%.*s, err_handle))", MD_S8VArg(json_value_variable_name));
        Dqn_MDCppFile_Line(cpp, "it = %.*s; // Rewind the iterator to the object", MD_S8VArg(json_value_variable_name));
        Dqn_MDCppFile_Line(cpp, "%.*s = %.*sParseTokens(&it, err_handle, json, arena);", MD_S8VArg(dest), MD_S8VArg(type));
        Dqn_MDCppFile_EndIfBlock(cpp);
    }
    else
    {
        if (MD_S8Match(type, MD_S8Lit("f32"),  (MD_MatchFlags)0))
        {
            DQN_MD_ASSERT_MSG(json_primitive == JsonPrimitive::String || json_primitive == JsonPrimitive::Number, "Implement the other part");
            Dqn_MDCppFile_Line(cpp, "%.*s = DQN_CAST(Dqn_f32)atof(%.*s_str.str);", MD_S8VArg(dest), MD_S8VArg(json_value_variable_name));
        }
        else if (MD_S8Match(type, MD_S8Lit("f64"),  (MD_MatchFlags)0))
        {
            DQN_MD_ASSERT_MSG(json_primitive == JsonPrimitive::String || json_primitive == JsonPrimitive::Number, "Implement the other part");
            Dqn_MDCppFile_Line(cpp, "%.*s = atof(%.*s_str.str);", MD_S8VArg(dest), MD_S8VArg(json_value_variable_name));
        }
        else if (MD_S8Match(type, MD_S8Lit("u8"),  (MD_MatchFlags)0) ||
                 MD_S8Match(type, MD_S8Lit("u16"), (MD_MatchFlags)0) ||
                 MD_S8Match(type, MD_S8Lit("u32"), (MD_MatchFlags)0) ||
                 MD_S8Match(type, MD_S8Lit("u64"), (MD_MatchFlags)0))
        {
            DQN_MD_ASSERT_MSG(json_primitive == JsonPrimitive::String || json_primitive == JsonPrimitive::Number, "Implement the other part");
            if (MD_S8Match(type, MD_S8Lit("u64"), (MD_MatchFlags)0))
            {
                if (MD_NodeHasTag(json_type_node->first_child, MD_S8Lit("hex"), (MD_MatchFlags)0))
                {
                    Dqn_MDCppFile_Line(
                        cpp,
                        R"(%.*s = Dqn_Hex_StringToU64(%.*s_str);)",
                        MD_S8VArg(dest), MD_S8VArg(json_value_variable_name));
                }
                else
                {
                    Dqn_MDCppFile_Line(
                        cpp,
                        R"(%.*s = Dqn_String_ToU64(%.*s_str);)",
                        MD_S8VArg(dest), MD_S8VArg(json_value_variable_name));
                }
            }
            else
            {
                // We need to truncate the deserialized type to write it
                // to our struct.

                // NOTE: This bit capitalises the types, i.e. turns u16
                // -> U16, which matches our function naming convention
                // for Dqn_Safe_TruncateU64To<Type> and co.
                char type_all_caps[4] = {};
                memcpy(type_all_caps, type.str, type.size);
                type_all_caps[0] = MD_CharToUpper(type_all_caps[0]);

                Dqn_MDCppFile_Line(
                    cpp,
                    R"(%.*s = Dqn_Safe_TruncateU64To%s(Dqn_String_ToU64(%.*s_str));)",
                    MD_S8VArg(dest),
                    type_all_caps,
                    MD_S8VArg(json_value_variable_name));
            }
        }
        else if (MD_S8Match(type, MD_S8Lit("u128"), 0))
        {
            DQN_MD_ASSERT_MSG(json_primitive == JsonPrimitive::String || json_primitive == JsonPrimitive::Number, "Implement the other part");
            Dqn_MDCppFile_Line(
                cpp,
                R"(%.*s = Dqn_U128_FromString(%.*s_str.str, Dqn_Safe_TruncateU64ToInt(%.*s_str.size));)",
                MD_S8VArg(dest),
                MD_S8VArg(json_value_variable_name),
                MD_S8VArg(json_value_variable_name));
        }
        else if (MD_S8Match(type, MD_S8Lit("i8"), 0) ||
                 MD_S8Match(type, MD_S8Lit("i16"), 0) ||
                 MD_S8Match(type, MD_S8Lit("i32"), 0) ||
                 MD_S8Match(type, MD_S8Lit("i64"), 0))
        {
            DQN_MD_ASSERT_MSG(json_primitive == JsonPrimitive::String || json_primitive == JsonPrimitive::Number, "Implement the other part");
            char type_all_caps[4] = {};
            memcpy(type_all_caps, type.str, type.size);
            type_all_caps[0] = MD_CharToUpper(type_all_caps[0]);

            Dqn_MDCppFile_Line(
                cpp,
                R"(%.*s = Dqn_Safe_TruncateU64To%s(Dqn_String_ToU64(%.*s_str));)",
                MD_S8VArg(dest),
                type_all_caps,
                MD_S8VArg(json_value_variable_name)
                );
        }
        else if (MD_S8Match(type, MD_S8Lit("string"), 0))
        {
            DQN_MD_ASSERT_MSG(json_primitive == JsonPrimitive::String, "Implement the other part");
            Dqn_MDCppFile_Line(
                cpp,
                R"(%.*s = Dqn_String_Copy(%.*s_str, arena);)",
                MD_S8VArg(dest),
                MD_S8VArg(json_value_variable_name));
        }
        else if (MD_S8Match(type, MD_S8Lit("bool"), 0))
        {
            if (json_primitive == JsonPrimitive::Bool)
            {
                Dqn_MDCppFile_Line(
                    cpp,
                    R"(%.*s = Dqn_JsmnToken_Bool(json, *%.*s);)",
                    MD_S8VArg(dest),
                    MD_S8VArg(json_value_variable_name));
            }
            else if (json_primitive == JsonPrimitive::Number)
            {
                Dqn_MDCppFile_Line(
                    cpp,
                    R"(%.*s = DQN_CAST(Dqn_b32)Dqn_String_ToU64(%.*s_str);)",
                    MD_S8VArg(dest),
                    MD_S8VArg(json_value_variable_name));
            }
            else
            {
                DQN_MD_INVALID_CODE_PATH_MSG("Unhandled");
            }
        }
        else
        {
            // NOTE: Unhandled 'json_type' specified in the .mdesk file
            DQN_MD_INVALID_CODE_PATH;
        }
    }

    Dqn_MDCppFile_EndIfBlock(cpp);
}

void Dqn_MD_GenerateJsonParsingFunction(Dqn_MDCppFile *cpp, MD_Node const *struct_node, bool header_file)
{
    MD_Node *tag = MD_NodeFromString(struct_node->first_tag, struct_node->last_tag + 1, MD_S8Lit("json_parsing_function"), (MD_MatchFlags)0);
    if (MD_NodeIsNil(tag))
        return;

    bool return_array_type = false;
    MD_String8 return_type = struct_node->string;

    MD_Node *struct_type_node      = MD_NodeFromString(struct_node->first_child, struct_node->last_child + 1, MD_S8Lit("type"), (MD_MatchFlags)0);
    MD_Node *struct_json_type_node = MD_NodeFromString(struct_node->first_child, struct_node->last_child + 1, MD_S8Lit("json_type"), (MD_MatchFlags)0);
    if (MD_NodeIsNil(struct_type_node) && MD_NodeIsNil(struct_json_type_node))
    {
    }
    else
    {
        return_array_type = true;
        return_type = MD_S8Fmt("Dqn_Array<%.*s>", MD_S8VArg(struct_type_node->first_child->string));
    }

    // -------------------------------------------------------------------------
    // Write json helper parsing function
    // -------------------------------------------------------------------------
    Dqn_MDCppFile_LineBegin(cpp, "%.*s %.*sParse(Dqn_String json, Dqn_JsmnErrorHandle *err_handle, Dqn_ArenaAllocator *temp_arena, Dqn_ArenaAllocator *arena", MD_S8VArg(return_type), MD_S8VArg(struct_node->string));
    if (header_file)
    {
        Dqn_MDCppFile_LineEnd(cpp, ");");
    }
    else
    {
        Dqn_MDCppFile_LineEnd(cpp, ")");
        Dqn_MDCppFile_BeginBlock(cpp, nullptr);
        Dqn_MDCppFile_Line(cpp, "(void)arena; // TODO(dqn): We don't always need an Arena, but we always have it in the prototype to make life easy if somewhere deep in the json heirarchy we need an arena");
        Dqn_MDCppFile_Line(cpp, "jsmn_parser parser = {};");
        Dqn_MDCppFile_Line(cpp, "jsmn_init(&parser);");
        Dqn_MDCppFile_NewLine(cpp);
        Dqn_MDCppFile_Line(cpp, "int tokens_required = jsmn_parse(&parser, json.str, json.size, nullptr, 0);");
        Dqn_MDCppFile_Line(cpp, "DQN_HARD_ASSERT(tokens_required > 0);");
        Dqn_MDCppFile_Line(cpp, "auto *tokens = Dqn_ArenaAllocator_NewArray(temp_arena, jsmntok_t, tokens_required, Dqn_ZeroMem::No);");
        Dqn_MDCppFile_NewLine(cpp);
        Dqn_MDCppFile_Line(cpp, "jsmn_init(&parser);");
        Dqn_MDCppFile_Line(cpp, "jsmn_parse(&parser, json.str, json.size, tokens, tokens_required);");
        Dqn_MDCppFile_Line(cpp, "jsmntok_t *it = tokens + 0;");
        Dqn_MDCppFile_NewLine(cpp);
        Dqn_MDCppFile_Line(cpp, "%.*s result = %.*sParseTokens(&it, err_handle, json, arena);", MD_S8VArg(return_type), MD_S8VArg(struct_node->string));
        Dqn_MDCppFile_Line(cpp, "return result;");
        Dqn_MDCppFile_EndFuncBlock(cpp);
    }

    // -------------------------------------------------------------------------
    // Write the main work horse code
    // -------------------------------------------------------------------------
    Dqn_MDCppFile_LineBegin(cpp, "%.*s %.*sParseTokens(jsmntok_t **start_it, Dqn_JsmnErrorHandle *err_handle, Dqn_String json, Dqn_ArenaAllocator *arena", MD_S8VArg(return_type), MD_S8VArg(struct_node->string));
    if (header_file)
    {
        Dqn_MDCppFile_LineEnd(cpp, ");");
        return;
    }

    Dqn_MDCppFile_LineEnd(cpp, ")");
    Dqn_MDCppFile_BeginBlock(cpp, nullptr);
    Dqn_MDCppFile_Line(cpp, "(void)arena; // TODO(dqn): We don't always need an Arena, but we always have it in the prototype to make life easy if somewhere deep in the json heirarchy we need an arena");
    Dqn_MDCppFile_Line(cpp, "%.*s result = {};", MD_S8VArg(return_type));
    Dqn_MDCppFile_Line(cpp, "jsmntok_t *it = (*start_it);");

    if (return_array_type)
    {
        MD_String8 struct_type = struct_type_node->first_child->string;
        MD_String8 struct_json_type = struct_json_type_node->first_child->string;
        Dqn_MDCppFile_Line(cpp, "jsmntok_t *value = it++;");
        Dqn_MDCppFile_NewLine(cpp);
        Dqn_MD__CppCodeGenDeserializeJSONPrimitiveToken(cpp, struct_json_type_node, struct_type_node, MD_S8Lit("root"), MD_S8Lit("result"), MD_S8Lit("value"));
    }
    else
    {
        //----------------------------------------------------------------------
        // Convert the type's in our .mdesk struct into JSON parsing code
        //----------------------------------------------------------------------
        Dqn_MDCppFile_Line(cpp, "if (!Dqn_JsmnToken_ExpectObject(*it, err_handle)) return result;");
        Dqn_MDCppFile_Line(cpp, "jsmntok_t *object = it++;");
        Dqn_MDCppFile_NewLine(cpp);
        Dqn_MDCppFile_BeginBlock(cpp, "for (int index = 0; index < object->size; index++)");
        Dqn_MDCppFile_Line(cpp, "jsmntok_t *key       = it++;");
        Dqn_MDCppFile_Line(cpp, "jsmntok_t *value     = it++;");
        Dqn_MDCppFile_Line(cpp, "Dqn_String key_str   = Dqn_JsmnToken_String(json, *key);");
        Dqn_MDCppFile_Line(cpp, "Dqn_String value_str = Dqn_JsmnToken_String(json, *value); (void)value_str;");
        Dqn_MDCppFile_NewLine(cpp);

        MD_String8 const IF_BRANCH      = MD_S8Lit("if");
        MD_String8 const ELSE_IF_BRANCH = MD_S8Lit("else if");
        MD_String8       branch_str     = IF_BRANCH;
        for (MD_EachNode(field_node, struct_node->first_child))
        {
            MD_Node *type_node      = MD_NodeFromString(field_node->first_child, field_node->last_child + 1, MD_S8Lit("type"), (MD_MatchFlags)0);
            MD_Node *json_type_node = MD_NodeFromString(field_node->first_child, field_node->last_child + 1, MD_S8Lit("json_type"), (MD_MatchFlags)0);
            MD_Node *json_key_node  = MD_NodeFromString(field_node->first_child, field_node->last_child + 1, MD_S8Lit("json_key"), (MD_MatchFlags)0);

            MD_String8 field     = field_node->string;
            MD_String8 type      = type_node->first_child->string;
            MD_String8 json_type = json_type_node->first_child->string;
            MD_String8 json_key  = MD_NodeIsNil(json_key_node) ? field : json_key_node->first_child->string;
            MD_Assert(!MD_NodeIsNil(type_node));

            Dqn_MDCppFile_BeginBlock(
                cpp, "%.*s (key_str == DQN_STRING(\"%.*s\"))", MD_S8VArg(branch_str), MD_S8VArg(json_key));
            branch_str = ELSE_IF_BRANCH;
            MD_String8 dest = MD_S8Fmt("result.%.*s", MD_S8VArg(field));
            Dqn_MD__CppCodeGenDeserializeJSONPrimitiveToken(cpp, json_type_node, type_node, field, dest, MD_S8Lit("value"));
            Dqn_MDCppFile_EndIfBlock(cpp);
        }

        if (struct_node->first_child)
        {
            Dqn_MDCppFile_BeginBlock(cpp, "else");
            Dqn_MDCppFile_Line      (cpp, "// Potential unhandled json object and value, advance the iterator accordingly");
            Dqn_MDCppFile_Line      (cpp, "DQN_ASSERT(key->type == JSMN_STRING);");
            Dqn_MDCppFile_BeginBlock(cpp, "if (value->type == JSMN_OBJECT)");
            Dqn_MDCppFile_Line      (cpp,    "it = Dqn_JsmnToken_AdvanceItPastObject(value, err_handle, json);");
            Dqn_MDCppFile_EndIfBlock(cpp);
            Dqn_MDCppFile_BeginBlock(cpp, "else if (value->type == JSMN_ARRAY)");
            Dqn_MDCppFile_Line      (cpp,    "it = Dqn_JsmnToken_AdvanceItPastArray(value, err_handle, json);");
            Dqn_MDCppFile_EndIfBlock(cpp);
            Dqn_MDCppFile_EndIfBlock(cpp);
        }
        Dqn_MDCppFile_EndForBlock(cpp);
    }

    Dqn_MDCppFile_NewLine(cpp);
    Dqn_MDCppFile_Line(cpp, "*start_it = it;");
    Dqn_MDCppFile_Line(cpp, "return result;");
    Dqn_MDCppFile_EndFuncBlock(cpp);
}

struct Dqn_MD__Variable
{
    MD_Node    *type_node;
    MD_String8  type;
    MD_Node    *name_node;
    MD_String8  name;
    MD_String8  cpp_type;
    bool        is_array;
};

void Dqn_MD__PrintVariableAsFunctionArgument(Dqn_MDCppFile *cpp, Dqn_MD__Variable *variable)
{
    if (variable->is_array)
        Dqn_MDCppFile_LineAdd(cpp, "%.*s const *%.*s, Dqn_isize %.*s_size", MD_S8VArg(variable->cpp_type), MD_S8VArg(variable->name), MD_S8VArg(variable->name));
    else
        Dqn_MDCppFile_LineAdd(cpp, "%.*s %.*s", MD_S8VArg(variable->cpp_type), MD_S8VArg(variable->name));
}

Dqn_MD__Variable Dqn_MD__VariableFromNode(MD_Node *node)
{
    Dqn_MD__Variable result = {};
    result.type_node        = MD_NodeFromString(node->first_child, node->last_child + 1, MD_S8Lit("type"), DQN_MD_CAST(MD_MatchFlags)0);
    result.name             = node->string;
    result.type             = result.type_node->first_child->string;
    result.is_array         = Dqn_MD__NodeHasArrayNotation(result.type_node->first_child->next);
    result.cpp_type         = Dqn_MD__ConvertTypeToCppType(result.type);
    return result;
}

void Dqn_MD_GenerateJsonEndpointURLFunction(Dqn_MDCppFile *cpp, MD_Node *struct_node, bool header_file)
{
    MD_Node *json_endpoint_node = MD_NodeFromString(struct_node->first_tag, struct_node->last_tag + 1, MD_S8Lit("json_endpoint"), (MD_MatchFlags)0);
    if (MD_NodeIsNil(json_endpoint_node))
        return;

    if (MD_NodeIsNil(json_endpoint_node->first_child))
    {
        DQN_MD_INVALID_CODE_PATH;
        return;
    }

    MD_Node *url_node    = MD_NodeFromString(json_endpoint_node->first_child, json_endpoint_node->last_child + 1, MD_S8Lit("url"), (MD_MatchFlags)0);
    MD_Node *params_node = MD_NodeFromString(json_endpoint_node->first_child, json_endpoint_node->last_child + 1, MD_S8Lit("params"), (MD_MatchFlags)0);
    MD_String8 url       = url_node->first_child->string;

    if (MD_NodeIsNil(url_node))
    {
        DQN_MD_INVALID_CODE_PATH;
        return;
    }

    bool const need_arena = (!MD_NodeIsNil(params_node->first_child));

    Dqn_MDCppFile_LineBegin(cpp, "Dqn_String %.*sURL(", MD_S8VArg(struct_node->string));
    if (need_arena)
    {
        Dqn_MDCppFile_LineBegin(cpp, "Dqn_ArenaAllocator *arena");
        if (!MD_NodeIsNil(params_node->first_child)) Dqn_MDCppFile_LineAdd(cpp, ", ");
        for (MD_EachNode(param_node, params_node->first_child))
        {
            MD_String8 name = param_node->string;
            MD_String8 type = param_node->first_child->string;
            MD_Assert(!MD_NodeIsNil(param_node->first_child));
            Dqn_MDCppFile_LineAdd(cpp, "%.*s %.*s", MD_S8VArg(Dqn_MD__ConvertTypeToCppType(type)), MD_S8VArg(name));
            if (!MD_NodeIsNil(param_node->next)) Dqn_MDCppFile_LineAdd(cpp, ", ");
        }
    }

    if (header_file)
    {
        Dqn_MDCppFile_LineEnd(cpp, ");");
        return;
    }

    Dqn_MDCppFile_LineEnd(cpp, ")");
    Dqn_MDCppFile_BeginBlock(cpp, nullptr);

    if (need_arena)
    {
        Dqn_MDCppFile_LineBegin(cpp, R"(Dqn_String result = Dqn_String_Fmt(arena, "%.*s")", MD_S8VArg(url));
        if (!MD_NodeIsNil(params_node->first_child)) Dqn_MDCppFile_LineAdd(cpp, ", ");
        for (MD_EachNode(param_node, params_node->first_child))
        {
            MD_String8 name = param_node->string;
            MD_String8 type = param_node->first_child->string;
            Dqn_MD__ConvertTypeToFmtString(type);
            if (MD_S8Match(type, MD_S8Lit("string"), 0))
                Dqn_MDCppFile_LineAdd(cpp, "DQN_STRING_FMT(%.*s)", MD_S8VArg(name));
            else
                Dqn_MDCppFile_LineAdd(cpp, "%.*s", MD_S8VArg(name));

            if (!MD_NodeIsNil(param_node->next)) Dqn_MDCppFile_LineAdd(cpp, ", ");
        }
        Dqn_MDCppFile_LineEnd(cpp, ");");
        Dqn_MDCppFile_Line(cpp, "DQN_HARD_ASSERT(result.str);");
        Dqn_MDCppFile_Line(cpp, "return result;");
    }
    else
    {
        Dqn_MDCppFile_Line(cpp, R"(return DQN_STRING("%.*s");)", MD_S8VArg(url));
    }
    Dqn_MDCppFile_EndFuncBlock(cpp);
}

void Dqn_MD_GenerateJsonRPCPostDataFunction(Dqn_MDCppFile *cpp, MD_Node *root, MD_Node *struct_node, bool header_file)
{
    MD_Node *tag = MD_NodeFromString(struct_node->first_tag, struct_node->last_tag + 1, MD_S8Lit("json_rpc_post_data"), (MD_MatchFlags)0);
    if (MD_NodeIsNil(tag))
        return;

    MD_Node *jsonrpc_node = MD_NodeFromString(tag->first_child, tag->last_child + 1, MD_S8Lit("jsonrpc"), (MD_MatchFlags)0);
    MD_Node *method_node  = MD_NodeFromString(tag->first_child, tag->last_child + 1, MD_S8Lit("method"), (MD_MatchFlags)0);
    MD_Node *id_node      = MD_NodeFromString(tag->first_child, tag->last_child + 1, MD_S8Lit("id"), (MD_MatchFlags)0);
    MD_Node *params_node  = MD_NodeFromString(tag->first_child, tag->last_child + 1, MD_S8Lit("params"), (MD_MatchFlags)0);

    if (MD_NodeIsNil(jsonrpc_node) || MD_NodeIsNil(method_node) || MD_NodeIsNil(id_node))
    {
        DQN_MD_INVALID_CODE_PATH_MSG("Missing mandatory node in the RPC post data tag");
        return;
    }

    bool need_tmp_arena = false;
    bool const need_arena     = (!MD_NodeIsNil(params_node->first_child));
    Dqn_MDCppFile_LineBegin(cpp, "Dqn_String %.*sJsonRPCPostData(", MD_S8VArg(struct_node->string));
    if (need_arena)
    {
        // ---------------------------------------------------------------------
        // Add the required arenas to the function prototype
        // ---------------------------------------------------------------------
        Dqn_MDCppFile_LineBegin(cpp, "Dqn_ArenaAllocator *arena, ");
        for (MD_EachNode(param_node, params_node->first_child))
        {
            MD_Node *type_node = param_node->first_child;
            if (Dqn_MD__NodeHasArrayNotation(type_node->first_child->next))
            {
                // NOTE: We need a string builder to build this data, i.e. we
                // need some temporary memory
                need_tmp_arena = true;
                Dqn_MDCppFile_LineBegin(cpp, "Dqn_ArenaAllocator *tmp_arena, ");
                break;
            }
        }

        // ---------------------------------------------------------------------
        // Add the parameters to the function prototype
        // ---------------------------------------------------------------------
        for (MD_EachNode(param_node, params_node->first_child))
        {
            Dqn_MD__Variable variable = Dqn_MD__VariableFromNode(param_node);
            Dqn_MD__PrintVariableAsFunctionArgument(cpp, &variable);
            if (!MD_NodeIsNil(param_node->next)) Dqn_MDCppFile_LineAdd(cpp, ", ");
        }
    }

    if (header_file)
    {
        Dqn_MDCppFile_LineEnd(cpp, ");");
        return;
    }

    Dqn_MDCppFile_LineEnd(cpp, ")");
    Dqn_MDCppFile_BeginBlock(cpp, nullptr);

    MD_String8 jsonrpc = jsonrpc_node->first_child->string;
    MD_String8 method  = method_node->first_child->string;
    MD_String8 id      = id_node->first_child->string;

    if (need_arena)
    {
        if (need_tmp_arena)
        {
            Dqn_MDCppFile_Line(cpp, "Dqn_ArenaAllocatorAutoRegion mem_region = Dqn_ArenaAllocator_AutoRegion(tmp_arena);");
            Dqn_MDCppFile_Line(cpp, "Dqn_StringBuilder<> builder = {};");
            Dqn_MDCppFile_Line(cpp, "Dqn_StringBuilder_InitWithArena(&builder, tmp_arena);");
            Dqn_MDCppFile_Line(cpp, R"FOO(Dqn_StringBuilder_AppendString(&builder, DQN_STRING(R"({"jsonrpc":"%.*s", "method":"%.*s", "id":%.*s, "params":[)"));)FOO",
                                    MD_S8VArg(jsonrpc),
                                    MD_S8VArg(method),
                                    MD_S8VArg(id));

            for (MD_EachNode(param_node, params_node->first_child))
            {
                Dqn_MD__Variable variable = Dqn_MD__VariableFromNode(param_node);
                if (variable.is_array)
                {
                    Dqn_MDCppFile_BeginBlock(cpp, "for (Dqn_isize array_index = 0; array_index < %.*s_size; array_index++)", MD_S8VArg(variable.name));
                    Dqn_MDCppFile_Line(cpp, "auto *%.*s_entry = %.*s + array_index;", MD_S8VArg(variable.name), MD_S8VArg(variable.name));
                    if (Dqn_MD__IsTypeCppPrimitive(variable.type))
                    {
                        DQN_MD_INVALID_CODE_PATH_MSG("Unimplemented code path");
                    }
                    else
                    {
                        MD_Node *struct_node = MD_NodeFromString(root->first_child, root->last_child + 1, variable.type, DQN_MD_CAST(MD_MatchFlags)0);
                        DQN_MD_ASSERT_MSG(struct_node, "We don't know how to format print the parameter %.*s", MD_S8VArg(variable.type));

                        Dqn_MDCppFile_Line(cpp, "if (array_index) Dqn_StringBuilder_AppendString(&builder, DQN_STRING(\", \"));");
                        Dqn_MDCppFile_Line(cpp, "Dqn_StringBuilder_AppendChar(&builder, '{');");
                        for (MD_EachNode(struct_param, struct_node->first_child))
                        {
                            // TODO(dqn): Only support strings atm
                            Dqn_MD__Variable struct_variable = Dqn_MD__VariableFromNode(struct_param);
                            DQN_MD_ASSERT_MSG(Dqn_MD__IsTypeCppPrimitive(struct_variable.type), "We don't know how to format print the parameter %.*s", MD_S8VArg(struct_variable.type));
                            Dqn_MDCppFile_Line(cpp, R"FOO(Dqn_StringBuilder_AppendFmt(&builder, "\"%.*s\": \"%.*s\"%s", DQN_STRING_FMT(%.*s_entry->%.*s));)FOO",
                                               MD_S8VArg(struct_variable.name),
                                               MD_S8VArg(Dqn_MD__ConvertTypeToFmtString(struct_variable.type)),
                                               MD_NodeIsNil(struct_param->next) ? "" : ", ",
                                               MD_S8VArg(variable.name),
                                               MD_S8VArg(struct_variable.name));

                        }
                        Dqn_MDCppFile_Line(cpp, "Dqn_StringBuilder_AppendChar(&builder, '}');");
                    }
                    Dqn_MDCppFile_EndForBlock(cpp);
                }
                else
                {
                    Dqn_MDCppFile_Line(cpp, "Dqn_StringBuilder_AppendFmt(&builder, \"%.*s%s\", %.*s);",
                                       MD_S8VArg(Dqn_MD__ConvertTypeToFmtString(variable.type)),
                                       MD_NodeIsNil(param_node->next) ? "" : ", ",
                                       MD_S8VArg(variable.name));
                }
            }

            Dqn_MDCppFile_Line(cpp, "Dqn_StringBuilder_AppendString(&builder, DQN_STRING(\"]}\"));");
            Dqn_MDCppFile_Line(cpp, "Dqn_String result = Dqn_StringBuilder_BuildStringWithArena(&builder, arena);");
        }
        else
        {
            Dqn_MDCppFile_Line(cpp, "Dqn_String result = Dqn_String_Fmt(");
            Dqn_MDCppFile_Indent(cpp);
            Dqn_MDCppFile_Line(cpp, "arena,");
            Dqn_MDCppFile_LineBegin(cpp,
                                    R"FOO(R"({"jsonrpc":"%.*s", "method":"%.*s", "id":%.*s, "params":[)FOO",
                                    MD_S8VArg(jsonrpc),
                                    MD_S8VArg(method),
                                    MD_S8VArg(id));

            for (MD_EachNode(param_node, params_node->first_child))
            {
                Dqn_MDCppFile_LineAdd(cpp, R"("%%.*s")");
                if (!MD_NodeIsNil(param_node->next)) Dqn_MDCppFile_LineAdd(cpp, ", ");
            }
            Dqn_MDCppFile_LineEnd(cpp, R"FOO(]})",)FOO");

            for (MD_EachNode(param_node, params_node->first_child))
            {
                MD_String8 name = param_node->string;
                MD_String8 type = param_node->first_child->string;
                Dqn_MD__ConvertTypeToFmtString(type);
                if (MD_S8Match(type, MD_S8Lit("string"), 0))
                    Dqn_MDCppFile_LineBegin(cpp, "DQN_STRING_FMT(%.*s)", MD_S8VArg(name));
                else
                    Dqn_MDCppFile_LineBegin(cpp, "%.*s", MD_S8VArg(name));

                if (!MD_NodeIsNil(param_node->next))
                {
                    Dqn_MDCppFile_LineAdd(cpp, ",");
                    Dqn_MDCppFile_LineEnd(cpp, nullptr);
                }
            }
            Dqn_MDCppFile_LineEnd(cpp, ");");
            Dqn_MDCppFile_Unindent(cpp);
        }
        Dqn_MDCppFile_Line(cpp, "DQN_HARD_ASSERT(result.str);");
        Dqn_MDCppFile_Line(cpp, "return result;");
    }
    else
    {
        Dqn_MDCppFile_Line(cpp, R"FOO(return DQN_STRING(R"({"jsonrpc":"%.*s", "method":"%.*s", "id":%.*s, "params":[]})");)FOO",
                           MD_S8VArg(jsonrpc),
                           MD_S8VArg(method),
                           MD_S8VArg(id));
    }
    Dqn_MDCppFile_EndFuncBlock(cpp);
}

void Dqn_MD_GenerateCurlQueryFunction(Dqn_MDCppFile *cpp, MD_Node *struct_node, bool header_file)
{
    MD_Node *json_endpoint_node = MD_NodeFromString(struct_node->first_tag, struct_node->last_tag + 1, MD_S8Lit("json_endpoint"), (MD_MatchFlags)0);
    if (MD_NodeIsNil(json_endpoint_node))
        return;

    if (MD_NodeIsNil(json_endpoint_node->first_child))
    {
        DQN_MD_INVALID_CODE_PATH;
        return;
    }

    MD_Node *url_node    = MD_NodeFromString(json_endpoint_node->first_child, json_endpoint_node->last_child + 1, MD_S8Lit("url"), (MD_MatchFlags)0);
    MD_Node *params_node = MD_NodeFromString(json_endpoint_node->first_child, json_endpoint_node->last_child + 1, MD_S8Lit("params"), (MD_MatchFlags)0);
    MD_String8 url       = url_node->first_child->string;

    if (MD_NodeIsNil(url_node) || MD_NodeIsNil(params_node))
    {
        DQN_MD_INVALID_CODE_PATH;
        return;
    }

    Dqn_MDCppFile_LineBegin(cpp, "Dqn_b32 %.*sQuery(struct INS_Core *insight", MD_S8VArg(struct_node->string));
    if (!MD_NodeIsNil(params_node->first_child)) Dqn_MDCppFile_LineAdd(cpp, ", ");
    for (MD_EachNode(param_node, params_node->first_child))
    {
        MD_String8 name = param_node->string;
        MD_String8 type = param_node->first_child->string;
        MD_Assert(!MD_NodeIsNil(param_node->first_child));
        Dqn_MDCppFile_LineAdd(cpp, "%.*s %.*s", MD_S8VArg(Dqn_MD__ConvertTypeToCppType(type)), MD_S8VArg(name));
        if (!MD_NodeIsNil(param_node->next)) Dqn_MDCppFile_LineAdd(cpp, ", ");
    }

    if (header_file)
    {
        Dqn_MDCppFile_LineEnd(cpp, ");");
        return;
    }

    Dqn_MDCppFile_LineEnd(cpp, ")");
    Dqn_MDCppFile_BeginBlock(cpp, nullptr);

    Dqn_MDCppFile_Line(cpp, "Dqn_FixedString<1024> url = {};");
    Dqn_MDCppFile_LineBegin(cpp, R"(Dqn_FixedString_AppendFmt(&url, "%.*s")", MD_S8VArg(url));
    if (!MD_NodeIsNil(params_node->first_child)) Dqn_MDCppFile_LineAdd(cpp, ", ");
    for (MD_EachNode(param_node, params_node->first_child))
    {
        MD_String8 name = param_node->string;
        Dqn_MDCppFile_LineAdd(cpp, "DQN_STRING_FMT(%.*s)", MD_S8VArg(name));
        if (!MD_NodeIsNil(param_node->next)) Dqn_MDCppFile_LineAdd(cpp, ", ");
    }
    Dqn_MDCppFile_LineEnd(cpp, ");");
    Dqn_MDCppFile_Line(cpp, "DQN_HARD_ASSERT(url.size);");

    Dqn_MDCppFile_Line(cpp, "INS_CurlQuery *query = INS_Curl_QueueQuery(insight, INS_QueryType_%.*s, Dqn_FixedString_ToString(&url));", MD_S8VArg(struct_node->string));
    Dqn_MDCppFile_Line(cpp, "Dqn_b32 result = query != nullptr;");
    Dqn_MDCppFile_Line(cpp, "return result;");
    Dqn_MDCppFile_EndFuncBlock(cpp);
}
#endif // DQN_META_DESK_IMPLEMENTATION

#if defined(DQN_META_DESK_STANDALONE_PROGRAM)
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stdout, "Please pass a file name and output name\nUsage: metadesk <file> <output_name>\n");
        return -1;
    }

    char *file        = argv[1];
    char *output_name = argv[2];
    Dqn_MD_CodeGen(file, output_name, Dqn_MD_CodeGenFlag_All);
    return 0;
}
#endif // DQN_META_DESK_STANDALONE_PROGRAM
