#if !defined(DQN_CPP_FILE_H)
#define DQN_CPP_FILE_H

// NOTE: Dqn_CppFile: Helper functions to generate C++ files
// =============================================================================
#include <stdio.h>  /// printf, fputc
#include <stdarg.h> /// va_list...
#include <assert.h> /// assert

struct Dqn_CppFile /// Maintains state for printing C++ style formatted files
{
    FILE *file;             ///< File to print to
    int   indent;           ///< Current indent level of the printer
    int   space_per_indent; ///< Number of spaces per indent
    bool  k_and_r_indent;   ///< K&R style indenting when opening a block scope, e.g. "{\n" vs "\n{"
};

/// Print the format string indented and terminate the string with a new-line.
void Dqn_CppFLineV(Dqn_CppFile *cpp, char const *fmt, va_list args);
void Dqn_CppFLine(Dqn_CppFile *cpp, char const *fmt, ...);

/// Print the format string indented
void Dqn_CppFPrintV(Dqn_CppFile *cpp, char const *fmt, va_list args);
void Dqn_CppFPrint(Dqn_CppFile *cpp, char const *fmt, ...);

/// Print the format string
#define Dqn_CppFAppend(cpp, fmt, ...) vfprintf(cpp->file, fmt, ##__VAR_ARGS__)

/// End the current line, useful after CppFPrint and CppFAppend
#define Dqn_CppFEndLine(cpp) fputc('\n', (cpp)->file)

/// Manually modify the indent level
#define Dqn_CppFIndent(cpp) (cpp)->indent++;
#define Dqn_CppFUnindent(cpp) (cpp)->indent--; assert(cpp->indent >= 0)

/// Print the format string followed by a "{" and enter a new line whilst
/// increasing the indent level after the brace.
void Dqn_CppFBeginBlock(Dqn_CppFile *cpp, char const *fmt, ...);
void Dqn_CppFEndBlock  (Dqn_CppFile *cpp, bool trailing_semicolon, bool new_line_on_next_block);

/// End a block, specifically for the following language constructs.
#define Dqn_CppFEndEnumBlock(cpp)   Dqn_CppFEndBlock(cpp, true  /*trailing_semicolon*/, true  /*new_line_on_next_block*/)
#define Dqn_CppFEndForBlock(cpp)    Dqn_CppFEndBlock(cpp, false /*trailing_semicolon*/, false /*new_line_on_next_block*/)
#define Dqn_CppFEndIfBlock(cpp)     Dqn_CppFEndBlock(cpp, false /*trailing_semicolon*/, false /*new_line_on_next_block*/)
#define Dqn_CppFEndFuncBlock(cpp)   Dqn_CppFEndBlock(cpp, false /*trailing_semicolon*/, true  /*new_line_on_next_block*/)
#define Dqn_CppFEndStructBlock(cpp) Dqn_CppFEndBlock(cpp, true  /*trailing_semicolon*/, true  /*new_line_on_next_block*/)
#endif // DQN_CPP_FILE_H

#if defined(DQN_CPP_FILE_IMPLEMENTATION)
// NOTE: Dqn_CppFile Implementation
// =============================================================================
void Dqn_CppFLineV(Dqn_CppFile *cpp, char const *fmt, va_list args)
{
    Dqn_CppFPrintV(cpp, fmt, args);
    Dqn_CppFLineEnd(cpp, nullptr);
}

void Dqn_CppFLine(Dqn_CppFile *cpp, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_CppFLineV(cpp, fmt, args);
    va_end(args);
}

void Dqn_CppFPrintV(Dqn_CppFile *cpp, char const *fmt, va_list args)
{
    int space_per_indent = cpp->space_per_indent == 0 ? 4 : cpp->space_per_indent;
    int spaces           = fmt ? (cpp->indent * space_per_indent) : 0;
    fprintf(cpp->file, "%*s", spaces, "");
    vfprintf(cpp->file, fmt, args);
}

void Dqn_CppFPrint(Dqn_CppFile *cpp, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_CppFPrintV(cpp, fmt, args);
    va_end(args);
}

void Dqn_CppFBeginBlock(Dqn_CppFile *cpp, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_CppFLineV(cpp, fmt, args);
    va_end(args);
    Dqn_CppFPrint(cpp, "%s", cpp->k_and_r_indent ? "{\n" : "\n{");
    Dqn_CppFIndent(cpp);
}

void Dqn_CppFEndBlock(Dqn_CppFile *cpp, bool trailing_semicolon, bool new_line_on_next_block)
{
    Dqn_CppFUnindent(cpp);
    Dqn_CppFLine(cpp, trailing_semicolon ? "};" : "}");
    if (new_line_on_next_block)
        fputc('\n', cpp->file);
}
#endif // DQN_CPP_FILE_IMPLEMENTATION
