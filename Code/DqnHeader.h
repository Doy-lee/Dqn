#ifndef DQN_HEADER_H
#define DQN_HEADER_H
#define DQN_HEADER_COPY_PROTOTYPE(func_return, func_name_and_types) func_return func_name_and_types
#define DQN_HEADER_COPY_BEGIN
#define DQN_HEADER_COPY_END
#endif /* DQN_HEADER_H */

#ifdef DQN_HEADER_IMPLEMENTATION

#define _CRT_SECURE_NO_WARNINGS // NOTE: Undefined at end of file
#include <stddef.h>

#define DQN_IMPLEMENTATION
#include "Dqn.h"

char const *Dqn_Header__ParseFunctionReturnType(char const *ptr, isize *len)
{
    char const *result = Dqn_StrSkipWhitespace(ptr);
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

char const *Dqn_Header__ParseFunctionNameAndParameters(char const *ptr, isize *len)
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

char const *Dqn_Header__ParseFunctionComment(char const *ptr, isize *len)
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

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        fprintf(stdout, "DqnHeader Usage: %s [<source code filename>, ...]\n", argv[0]);
        return 0;
    }

    Dqn_MemArena arena = {};
    for (isize arg_index = 1; arg_index < argc; ++arg_index)
    {
        char const *file = argv[arg_index];
        isize buf_size   = 0;
        char *buf        = Dqn_FileReadWithArena(&arena, file, &buf_size);
        if (!buf)
        {
            fprintf(stderr, "Failed to read file: %s\n", file);
            continue;
        }

        char constexpr HEADER_COPY_PROTOTYPE[] = "DQN_HEADER_COPY_PROTOTYPE";
        char constexpr HEADER_COMMENT[]        = "// @";
        char constexpr HEADER_COPY_BEGIN[]     = "DQN_HEADER_COPY_BEGIN";
        char constexpr HEADER_COPY_END[]       = "DQN_HEADER_COPY_END";

        enum struct FindString
        {
            HeaderPrototype,
            HeaderComment,
            HeaderCopyBegin,
            Count
        };

        char const *find_list[]            = {HEADER_COPY_PROTOTYPE, HEADER_COMMENT, HEADER_COPY_BEGIN};
        isize constexpr find_string_lens[] = {
            Dqn_CharCountI(HEADER_COPY_PROTOTYPE),
            Dqn_CharCountI(HEADER_COMMENT),
            Dqn_CharCountI(HEADER_COPY_BEGIN),
        };

        char const *ptr          = buf;
        char const *ptr_end      = buf + buf_size;
        isize ptr_len            = buf_size;
        isize matched_find_index = -1;
        for (char const *token = Dqn_StrMultiFind(ptr, find_list, find_string_lens, Dqn_ArrayCountI(find_list), &matched_find_index, ptr_len);
             token;
             token = Dqn_StrMultiFind(ptr, find_list, find_string_lens, Dqn_ArrayCountI(find_list), &matched_find_index, ptr_len))
        {
            if (matched_find_index == (isize)FindString::HeaderPrototype)
            {
                ptr                   = token + find_string_lens[matched_find_index] + 1 /*macro start parenthesis*/;
                isize func_type_len   = 0;
                char const *func_type = Dqn_Header__ParseFunctionReturnType(ptr, &func_type_len);

                ptr                   = func_type + func_type_len + 1; // Ptr is at macro comma, skip the comma
                ptr                   = Dqn_StrSkipWhitespace(ptr);
                isize func_name_len   = 0;
                char const *func_name = Dqn_Header__ParseFunctionNameAndParameters(ptr, &func_name_len);

                ptr = func_name + func_name_len + 1; // Ptr is at macro closing paren, skip the paren
                fprintf(stdout, "%.*s %.*s;\n", (int)func_type_len, func_type, (int)func_name_len, func_name);
            }
            else if (matched_find_index == (isize)FindString::HeaderComment)
            {
                char const *comment_start = token;
                ptr                       = token;
                while (ptr[0] != '\n' && ptr[0] != '\r')
                    ptr++;

                isize comment_len = ptr - comment_start;
                fprintf(stdout, "%.*s\n", (int)comment_len, comment_start);
                ptr = comment_start + comment_len;
            }
            else if (matched_find_index == (isize)FindString::HeaderCopyBegin)
            {
                ptr                    = token + find_string_lens[matched_find_index];
                ptr                    = Dqn_StrSkipWhitespace(ptr);
                char const *copy_start = ptr;
                char const *copy_end   = Dqn_StrFind(ptr, HEADER_COPY_END, ptr_len, Dqn_CharCountI(HEADER_COPY_END));
                if (!copy_end)
                {
                    fprintf(stderr, "Header copy begin macro: %s not matched with a copy end macro: %s", HEADER_COPY_BEGIN, HEADER_COPY_END);
                    return -1;
                }

                isize copy_len = copy_end - copy_start;
                auto mem_scope = Dqn_MemArenaScopedRegion(&arena);
                char *sanitised_copy  = (char *)MEM_ARENA_ALLOC(&arena, copy_len);
                isize sanitised_len = 0;
                DQN_FOR_EACH(copy_index, copy_len)
                {
                    char ch = copy_start[copy_index];
                    if (ch == '\r') continue;
                    sanitised_copy[sanitised_len++] = ch;
                }

                ptr = copy_end;
                fprintf(stdout, "%.*s\n", (int)sanitised_len, sanitised_copy);
            }

            ptr_len = ptr_end - ptr;
        }
    }
}

#undef _CRT_SECURE_NO_WARNINGS
#endif /* DQN_HEADER_IMPLEMENTATION */
