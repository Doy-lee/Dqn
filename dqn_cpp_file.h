#if !defined(DQN_CPP_FILE_H)
#define DQN_CPP_FILE_H

#include <stdio.h>
#include <stdarg.h>

// NOTE: Dqn_CppFile: Helper functions to generate formatted CPP files
// -----------------------------------------------------------------------------
#define DQN_CPPF_ASSERT(expr)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            *((int volatile *)0) = 0;                                                                                  \
        }                                                                                                              \
    } while (0)

struct Dqn_CppFile
{
    FILE *file;
    int   indent;
    int   space_per_indent;
    bool  append_extra_new_line;
};

int     Dqn_CppFSpacePerIndent(Dqn_CppFile *cpp);

void    Dqn_CppFLineBeginV    (Dqn_CppFile *cpp, char const *fmt, va_list args);
void    Dqn_CppFLineBegin     (Dqn_CppFile *cpp, char const *fmt, ...);
void    Dqn_CppFLineEnd       (Dqn_CppFile *cpp, char const *fmt, ...);
void    Dqn_CppFLineAdd       (Dqn_CppFile *cpp, char const *fmt, ...);
void    Dqn_CppFLineV         (Dqn_CppFile *cpp, char const *fmt, va_list args);
void    Dqn_CppFLine          (Dqn_CppFile *cpp, char const *fmt, ...);

void    Dqn_CppFNewLine       (Dqn_CppFile *cpp);
void    Dqn_CppFIndent        (Dqn_CppFile *cpp);
void    Dqn_CppFUnindent      (Dqn_CppFile *cpp);

// fmt: (Optional) The format string to print at the beginning of the block.
// When the fmt string is given, it will place a new-line at the end of the fmt
// string. When fmt is nullptr, no new line will be appended.
void    Dqn_CppFBeginBlock    (Dqn_CppFile *cpp, char const *fmt, ...);
void    Dqn_CppFEndBlock      (Dqn_CppFile *cpp, bool trailing_semicolon, bool new_line_on_next_block);

#define Dqn_CppFEndEnumBlock(cpp) Dqn_CppFEndBlock(cpp, true /*trailing_semicolon*/, true /*new_line_on_next_block*/)
#define Dqn_CppFEndForBlock(cpp) Dqn_CppFEndBlock(cpp, false /*trailing_semicolon*/, false /*new_line_on_next_block*/)
#define Dqn_CppFEndIfBlock(cpp) Dqn_CppFEndBlock(cpp, false /*trailing_semicolon*/, false /*new_line_on_next_block*/)
#define Dqn_CppFEndFuncBlock(cpp) Dqn_CppFEndBlock(cpp, false /*trailing_semicolon*/, true /*new_line_on_next_block*/)
#define Dqn_CppFEndStructBlock(cpp) Dqn_CppFEndBlock(cpp, true /*trailing_semicolon*/, true /*new_line_on_next_block*/)
#endif // DQN_CPP_FILE_H

#if defined(DQN_CPP_FILE_IMPLEMENTATION)
// -----------------------------------------------------------------------------
// NOTE: Dqn_CppFile Implementation
// -----------------------------------------------------------------------------
int Dqn_CppFSpacePerIndent(Dqn_CppFile *cpp)
{
    int result = cpp->space_per_indent == 0 ? 4 : cpp->space_per_indent;
    return result;
}

void Dqn_CppFLineBeginV(Dqn_CppFile *cpp, char const *fmt, va_list args)
{
    int spaces = cpp->indent * Dqn_CppFSpacePerIndent(cpp);
    fprintf(cpp->file, "%*s", spaces, "");
    vfprintf(cpp->file, fmt, args);
}

void Dqn_CppFLineBegin(Dqn_CppFile *cpp, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_CppFLineBeginV(cpp, fmt, args);
    va_end(args);
}

void Dqn_CppFLineEnd(Dqn_CppFile *cpp, char const *fmt, ...)
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

void Dqn_CppFLineAdd(Dqn_CppFile *cpp, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(cpp->file, fmt, args);
    va_end(args);
}

void Dqn_CppFLineV(Dqn_CppFile *cpp, char const *fmt, va_list args)
{
    Dqn_CppFLineBeginV(cpp, fmt, args);
    Dqn_CppFLineEnd(cpp, nullptr);
}

void Dqn_CppFLine(Dqn_CppFile *cpp, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_CppFLineBeginV(cpp, fmt, args);
    Dqn_CppFLineEnd(cpp, nullptr);
    va_end(args);
}

void Dqn_CppFNewLine(Dqn_CppFile *cpp)
{
    fputc('\n', cpp->file);
}

void Dqn_CppFIndent(Dqn_CppFile *cpp)
{
    cpp->indent++;
}

void Dqn_CppFUnindent(Dqn_CppFile *cpp)
{
    cpp->indent--;
    DQN_CPPF_ASSERT(cpp->indent >= 0);
}

void Dqn_CppFBeginBlock(Dqn_CppFile *cpp, char const *fmt, ...)
{
    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        Dqn_CppFLineV(cpp, fmt, args);
        va_end(args);
    }

    Dqn_CppFLine(cpp, "{");
    Dqn_CppFIndent(cpp);
}

void Dqn_CppFEndBlock(Dqn_CppFile *cpp, bool trailing_semicolon, bool new_line_on_next_block)
{
    Dqn_CppFUnindent(cpp);
    Dqn_CppFLine(cpp, trailing_semicolon ? "};" : "}");
    if (new_line_on_next_block) fputc('\n', cpp->file);
}
#endif // DQN_CPP_FILE_IMPLEMENTATION
