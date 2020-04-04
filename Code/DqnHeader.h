#ifndef DQN_HEADER_H
#define DQN_HEADER_H

// Compile & Run
/*
   MSVC
   cl /O2 /MT /W4 /EHsc /FC /wd4201 /D DQN_HEADER_IMPLEMENTATION /Tp DqnHeader.h /link /nologo
   DqnHeader.exe Dqn.h > Dqn_Generated.h
*/

// NOTE: Copy the function prototype, all functions will be whitespaced aligned to the longest return type of the file
// NOTE: If you function contains a comma NOT in the argument list (i.e. multiple template parameters), this macro does NOT work.
//       Please use the DQN_HEADER_COPY_BEGIN and DQN_HEADER_COPY_END unfortunately.
#define DQN_HEADER_COPY_PROTOTYPE(func_return, func_name_and_types) func_return func_name_and_types

// NOTE: Copy all contents from the file between the begin and end macro
#define DQN_HEADER_COPY_BEGIN
#define DQN_HEADER_COPY_END

// NOTE: Copy comments by writing comments with the prefix, "// @"
// @ Hello this comment will be copied to the output

// NOTE: Example Input
#if 0
    #include "DqnHeader.h"

    // @ ptr1: Pointer to the first block of memory
    // @ ptr2: Pointer to the second block of memory
    // @ num_bytes: The number of bytes to compare in both blocks of memory
    DQN_HEADER_COPY_PROTOTYPE(int, Dqn_MemCmp(void const *ptr1, void const *ptr2, size_t num_bytes))
    {
        int result = memcmp(ptr1, ptr2, num_bytes);
        return result;
    }

    DQN_HEADER_COPY_BEGIN
    struct HelloWorld
    {
        int foo, bar;
    };
    DQN_HEADER_COPY_END
#endif

// NOTE: Example Output
#if 0
    // @ ptr1: Pointer to the first block of memory
    // @ ptr2: Pointer to the second block of memory
    // @ num_bytes: The number of bytes to compare in both blocks of memory
    int Dqn_MemCmp(void const *ptr1, void const *ptr2, size_t num_bytes);
    struct HelloWorld
    {
        int foo, bar;
    };
#endif

#endif /* DQN_HEADER_H */

#ifdef DQN_HEADER_IMPLEMENTATION

#define _CRT_SECURE_NO_WARNINGS // NOTE: Undefined at end of file
#include <stddef.h>

#define DQN_USE_PRIMITIVE_TYPEDEFS
#define DQN_IMPLEMENTATION
#include "Dqn.h"

char const *ParseFunctionReturnType(char const *ptr, isize *len)
{
    char const *result = Dqn_Str_SkipWhitespace(ptr);
    isize result_len   = 0;
    for (int scope = 0; ptr; ptr++) // NOTE: Parse the function return type
    {
        if (ptr[0] == '<' || ptr[0] == '>')
        {
            if (ptr[0] == '<') scope++;
            else               scope--;
            continue;
        }
        else if (ptr[0] == ',')
        {
            if (scope != 0) continue;
            result_len = static_cast<int>(ptr - result);
            break;
        }
    }

    if (len) *len = result_len;
    return result;
}

char const *ParseFunctionNameAndParameters(char const *ptr, isize *len)
{
    char const *result = ptr;
    int result_len = 0;
    for (int scope = 0; ptr; ptr++) // NOTE: Parse the function name and parameters
    {
        if (ptr[0] == '(') scope++;
        else if (ptr[0] == ')')
        {
            if (scope-- != 0) continue;
            result_len = static_cast<int>(ptr - result);
            break;
        }
    }

    *len = result_len;
    return result;
}

char const *ParseFunctionComment(char const *ptr, isize *len)
{
    while (ptr[0] != '"') ptr++;
    ptr++;

    char const *result = ptr;
    isize result_len = 0;
    for (;;)
    {
        while (ptr[0] != '"')
            ptr++;

        if (ptr[-1] != '\\')
        {
            result_len = ptr - result;
            break;
        }
    }

    *len = result_len;
    return result;

}

enum struct HeaderEntryType
{
    Prototype,
    Comment,
    CopyBegin,
    Count
};

struct HeaderEntry
{
    HeaderEntryType type;
    union
    {
        struct
        {
            Dqn_String return_val;
            Dqn_String name_and_args;
        } function_decl;

        Dqn_String comment;
        Dqn_String copy_range;
    };
};

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        fprintf(stdout, "Header Usage: %s [<source code filename>, ...]\n", argv[0]);
        return 0;
    }

    Dqn_MemArena arena      = {};
    arena.allocator         = Dqn_Allocator_XHeap();
    Dqn_Allocator allocator = Dqn_Allocator_Arena(&arena);
    Dqn_MemArena_Reserve(&arena, DQN_MEGABYTES(16));
    for (isize arg_index = 1; arg_index < argc; ++arg_index)
    {
        char const *file = argv[arg_index];
        isize buf_size   = 0;
        char *buf        = Dqn_File_ReadAll(&allocator, file, &buf_size);
        if (!buf)
        {
            fprintf(stderr, "Failed to read file: %s\n", file);
            continue;
        }

        char constexpr HEADER_COPY_PROTOTYPE[] = "DQN_HEADER_COPY_PROTOTYPE";
        char constexpr HEADER_COMMENT[]        = "// @";
        char constexpr HEADER_COPY_BEGIN[]     = "DQN_HEADER_COPY_BEGIN";
        char constexpr HEADER_COPY_END[]       = "DQN_HEADER_COPY_END";

        char const *find_list[]            = {HEADER_COPY_PROTOTYPE, HEADER_COMMENT, HEADER_COPY_BEGIN};
        isize constexpr find_string_lens[] = {
            Dqn_CharCountI(HEADER_COPY_PROTOTYPE),
            Dqn_CharCountI(HEADER_COMMENT),
            Dqn_CharCountI(HEADER_COPY_BEGIN),
        };

        isize num_header_entries = 0;
        {
            char const *ptr          = buf;
            char const *ptr_end      = buf + buf_size;
            isize ptr_len            = buf_size;
            isize matched_find_index = -1;
            for (char const *token = Dqn_Str_FindMulti(ptr, find_list, find_string_lens, Dqn_ArrayCountI(find_list), &matched_find_index, ptr_len);
                 token;
                 token = Dqn_Str_FindMulti(ptr, find_list, find_string_lens, Dqn_ArrayCountI(find_list), &matched_find_index, ptr_len))
            {
                ptr = token + find_string_lens[matched_find_index];
                num_header_entries++;
                ptr_len = ptr_end - ptr;
            }
        }

        auto *header_entries           = Dqn_MemArena_AllocateType<HeaderEntry>(&arena, num_header_entries);
        isize header_entries_index     = 0;
        isize max_prototype_return_val = 0;

        char const *ptr          = buf;
        char const *ptr_end      = buf + buf_size;
        isize ptr_len            = buf_size;
        isize matched_find_index = -1;
        for (char const *token = Dqn_Str_FindMulti(ptr, find_list, find_string_lens, Dqn_ArrayCountI(find_list), &matched_find_index, ptr_len);
             token;
             token = Dqn_Str_FindMulti(ptr, find_list, find_string_lens, Dqn_ArrayCountI(find_list), &matched_find_index, ptr_len))
        {
            HeaderEntry *entry = header_entries + header_entries_index++;
            entry->type           = static_cast<HeaderEntryType>(matched_find_index);
            if (matched_find_index == (isize)HeaderEntryType::Prototype)
            {
                ptr                   = token + find_string_lens[matched_find_index] + 1 /*macro start parenthesis*/;
                isize func_type_len   = 0;
                char const *func_type = ParseFunctionReturnType(ptr, &func_type_len);

                ptr                   = func_type + func_type_len + 1; // Ptr is at macro comma, skip the comma
                ptr                   = Dqn_Str_SkipWhitespace(ptr);
                isize func_name_len   = 0;
                char const *func_name = ParseFunctionNameAndParameters(ptr, &func_name_len);

                entry->function_decl.return_val  = Dqn_Asprintf(&allocator, "%.*s", func_type_len, func_type);
                entry->function_decl.name_and_args = Dqn_Asprintf(&allocator, "%.*s", func_name_len, func_name);
                ptr = func_name + func_name_len + 1; // Ptr is at macro closing paren, skip the paren

                max_prototype_return_val = DQN_MAX(max_prototype_return_val, func_type_len);
            }
            else if (matched_find_index == (isize)HeaderEntryType::Comment)
            {
                char const *comment_start = token;
                ptr                       = token;
                while (ptr[0] != '\n' && ptr[0] != '\r')
                    ptr++;
                isize comment_len = ptr - comment_start;

                entry->comment.str = Dqn_MemArena_AllocateType<char>(&arena, comment_len);
                DQN_FOR_EACH(comment_index, comment_len)
                {
                    // NOTE: We capture "// @", and we want to skip the @ symbol, its ugly which is at the index 3
                    // Also if a space is given after the '@' symbol then skip that too.
                    char ch = comment_start[comment_index];
                    if (comment_index == 3 && ch == '@') continue;
                    if (comment_index == 4 && ch == ' ') continue;
                    entry->comment.str[entry->comment.len++] = ch;
                }

                while (entry->comment.len > 0 && Dqn_Char_IsWhitespace(entry->comment.str[entry->comment.len - 1]))
                    entry->comment.len--;
                ptr = comment_start + comment_len;
            }
            else if (matched_find_index == (isize)HeaderEntryType::CopyBegin)
            {
                ptr                    = token + find_string_lens[matched_find_index];
                ptr                    = Dqn_Str_SkipWhitespace(ptr);
                char const *copy_start = ptr;
                char const *copy_end   = Dqn_Str_Find(ptr, HEADER_COPY_END, ptr_len, Dqn_CharCountI(HEADER_COPY_END));
                if (!copy_end)
                {
                    fprintf(stderr, "Header copy begin macro: %s not matched with a copy end macro: %s", HEADER_COPY_BEGIN, HEADER_COPY_END);
                    return -1;
                }

                isize copy_len        = copy_end - copy_start;
                entry->copy_range.str = Dqn_MemArena_AllocateType<char>(&arena, copy_len);
                DQN_FOR_EACH(copy_index, copy_len)
                {
                    char ch = copy_start[copy_index];
                    if (ch == '\r') continue;
                    entry->copy_range.str[entry->copy_range.len++] = ch;
                }
                ptr = copy_end;
            }

            ptr_len = ptr_end - ptr;
        }
        DQN_ASSERT(header_entries_index == num_header_entries);

        DQN_FOR_EACH(entry_index, header_entries_index)
        {
            HeaderEntry const *entry = header_entries + entry_index;
            switch(entry->type)
            {
                case HeaderEntryType::Prototype:
                {
                    isize remaining_space = max_prototype_return_val - entry->function_decl.return_val.len + 1;
                    fprintf(stdout, "%.*s", (int)entry->function_decl.return_val.len, entry->function_decl.return_val.str);
                    DQN_FOR_EACH(space, remaining_space) fputs(" ", stdout);
                    fprintf(stdout, " %.*s;\n", (int)entry->function_decl.name_and_args.len, entry->function_decl.name_and_args.str);
                }
                break;

                case HeaderEntryType::Comment:
                {
                    fprintf(stdout, "%.*s\n", (int)entry->comment.len, entry->comment.str);
                }
                break;

                case HeaderEntryType::CopyBegin:
                {
                    fprintf(stdout, "%.*s\n", (int)entry->copy_range.len, entry->copy_range.str);
                }
                break;
            }
        }
    }
}

#undef _CRT_SECURE_NO_WARNINGS
#endif /* DQN_HEADER_IMPLEMENTATION */
