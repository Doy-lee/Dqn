#if !defined(DQN_CPP_FILE_H)
#define DQN_CPP_FILE_H

#include <stdio.h>
#include <stdarg.h>

// -----------------------------------------------------------------------------
// NOTE: Dqn_CppFile: Helper functions to generate formatted CPP files
// -----------------------------------------------------------------------------
#define DQN_CPPF_ASSERT(expr) do { if (!(expr)) { *((int *)0) = 0; } } while (0)

struct Dqn_CppFile
{
    FILE *file;
    int   indent;
    int   space_per_indent;
    bool  append_extra_new_line;
};

int     Dqn_CppFile_SpacePerIndent(Dqn_CppFile *cpp);

void    Dqn_CppFile_LineBeginV    (Dqn_CppFile *cpp, char const *fmt, va_list args);
void    Dqn_CppFile_LineBegin     (Dqn_CppFile *cpp, char const *fmt, ...);
void    Dqn_CppFile_LineEnd       (Dqn_CppFile *cpp, char const *fmt, ...);
void    Dqn_CppFile_LineAdd       (Dqn_CppFile *cpp, char const *fmt, ...);
void    Dqn_CppFile_LineV         (Dqn_CppFile *cpp, char const *fmt, va_list args);
void    Dqn_CppFile_Line          (Dqn_CppFile *cpp, char const *fmt, ...);

void    Dqn_CppFile_NewLine       (Dqn_CppFile *cpp);
void    Dqn_CppFile_Indent        (Dqn_CppFile *cpp);
void    Dqn_CppFile_Unindent      (Dqn_CppFile *cpp);

// fmt: (Optional) The format string to print at the beginning of the block.
// When the fmt string is given, it will place a new-line at the end of the fmt
// string. When fmt is nullptr, no new line will be appended.
void    Dqn_CppFile_BeginBlock    (Dqn_CppFile *cpp, char const *fmt, ...);
void    Dqn_CppFile_EndBlock      (Dqn_CppFile *cpp, bool trailing_semicolon, bool new_line_on_next_block);

#define Dqn_CppFile_EndEnumBlock(cpp) Dqn_CppFile_EndBlock(cpp, true /*trailing_semicolon*/, true /*new_line_on_next_block*/)
#define Dqn_CppFile_EndForBlock(cpp) Dqn_CppFile_EndBlock(cpp, false /*trailing_semicolon*/, false /*new_line_on_next_block*/)
#define Dqn_CppFile_EndIfBlock(cpp) Dqn_CppFile_EndBlock(cpp, false /*trailing_semicolon*/, false /*new_line_on_next_block*/)
#define Dqn_CppFile_EndFuncBlock(cpp) Dqn_CppFile_EndBlock(cpp, false /*trailing_semicolon*/, true /*new_line_on_next_block*/)
#define Dqn_CppFile_EndStructBlock(cpp) Dqn_CppFile_EndBlock(cpp, true /*trailing_semicolon*/, true /*new_line_on_next_block*/)
#endif // DQN_CPP_FILE_H

#if defined(DQN_CPP_FILE_IMPLEMENTATION)
// -----------------------------------------------------------------------------
// NOTE: Dqn_CppFile Implementation
// -----------------------------------------------------------------------------
int Dqn_CppFile_SpacePerIndent(Dqn_CppFile *cpp)
{
    int result = cpp->space_per_indent == 0 ? 4 : cpp->space_per_indent;
    return result;
}

void Dqn_CppFile_LineBeginV(Dqn_CppFile *cpp, char const *fmt, va_list args)
{
    int spaces = cpp->indent * Dqn_CppFile_SpacePerIndent(cpp);
    fprintf(cpp->file, "%*s", spaces, "");
    vfprintf(cpp->file, fmt, args);
}

void Dqn_CppFile_LineBegin(Dqn_CppFile *cpp, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_CppFile_LineBeginV(cpp, fmt, args);
    va_end(args);
}

void Dqn_CppFile_LineEnd(Dqn_CppFile *cpp, char const *fmt, ...)
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

void Dqn_CppFile_LineAdd(Dqn_CppFile *cpp, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(cpp->file, fmt, args);
    va_end(args);
}

void Dqn_CppFile_LineV(Dqn_CppFile *cpp, char const *fmt, va_list args)
{
    Dqn_CppFile_LineBeginV(cpp, fmt, args);
    Dqn_CppFile_LineEnd(cpp, nullptr);
}

void Dqn_CppFile_Line(Dqn_CppFile *cpp, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_CppFile_LineBeginV(cpp, fmt, args);
    Dqn_CppFile_LineEnd(cpp, nullptr);
    va_end(args);
}

void Dqn_CppFile_NewLine(Dqn_CppFile *cpp)
{
    fputc('\n', cpp->file);
}

void Dqn_CppFile_Indent(Dqn_CppFile *cpp)
{
    cpp->indent++;
}

void Dqn_CppFile_Unindent(Dqn_CppFile *cpp)
{
    cpp->indent--;
    DQN_CPPF_ASSERT(cpp->indent >= 0);
}

void Dqn_CppFile_BeginBlock(Dqn_CppFile *cpp, char const *fmt, ...)
{
    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        Dqn_CppFile_LineV(cpp, fmt, args);
        va_end(args);
    }

    Dqn_CppFile_Line(cpp, "{");
    Dqn_CppFile_Indent(cpp);
}

void Dqn_CppFile_EndBlock(Dqn_CppFile *cpp, bool trailing_semicolon, bool new_line_on_next_block)
{
    Dqn_CppFile_Unindent(cpp);
    Dqn_CppFile_Line(cpp, trailing_semicolon ? "};" : "}");
    if (new_line_on_next_block) fputc('\n', cpp->file);
}
#endif // DQN_CPP_FILE_IMPLEMENTATION
