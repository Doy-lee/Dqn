#if !defined(DQN_TESTER_H)
#define DQN_TESTER_H
// -----------------------------------------------------------------------------
// NOTE: Overview
// -----------------------------------------------------------------------------
// A super minimal testing framework, most of the logic here is the pretty
// printing of test results.

// -----------------------------------------------------------------------------
// NOTE: Configuration
// -----------------------------------------------------------------------------
// #define DQN_TESTER_IMPLEMENTATION
//     Define this in one and only one C++ file to enable the implementation
//     code of the header file. This will also automatically enable the JSMN
//     implementation.
//
// #define DQN_TESTER_RESULT_LPAD
//     Define this to a number to specify how much to pad the output of the test
//     result line before the test result is printed.
//
// #define DQN_TESTER_RESULT_PAD_CHAR
//     Define this to a character to specify the default character to use for
//     padding. By default this is '.'
//
// #define DQN_TESTER_SPACING
//     Define this to a number to specify the number of spaces between the group
//     declaration and the test output in the group.
//
// #define DQN_TESTER_BAD_COLOR
//     Define this to a terminal color code to specify what color errors will be
//     presented as.
//
// #define DQN_TESTER_GOOD_COLOR
//     Define this to a terminal color code to specify what color sucess will be
//     presented as.

// -----------------------------------------------------------------------------
// NOTE: Macros
// -----------------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>

#if !defined(DQN_TESTER_RESULT_LPAD)
    #define DQN_TESTER_RESULT_LPAD 90
#endif

#if !defined(DQN_TESTER_RESULT_PAD_CHAR)
    #define DQN_TESTER_RESULT_PAD_CHAR '.'
#endif

#if !defined(DQN_TESTER_SPACING)
    #define DQN_TESTER_SPACING 2
#endif

#if !defined(DQN_TESTER_BAD_COLOR)
    #define DQN_TESTER_BAD_COLOR "\x1b[31m"
#endif

#if !defined(DQN_TESTER_GOOD_COLOR)
    #define DQN_TESTER_GOOD_COLOR "\x1b[32m"
#endif

#define DQN_TESTER_COLOR_RESET "\x1b[0m"

#define DQN_TESTER_BEGIN_GROUP(fmt, ...) fprintf(stdout, fmt "\n", ##__VA_ARGS__)
#define DQN_TESTER_END_GROUP(test)                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        bool all_clear = (test)->num_tests_ok_in_group == (test)->num_tests_in_group;                                  \
        fprintf(stdout,                                                                                                \
                "%s\n  %02d/%02d tests passed -- %s\n\n" DQN_TESTER_COLOR_RESET,                                       \
                all_clear ? DQN_TESTER_GOOD_COLOR : DQN_TESTER_BAD_COLOR,                                              \
                (test)->num_tests_ok_in_group,                                                                         \
                (test)->num_tests_in_group,                                                                            \
                all_clear ? "OK" : "FAILED");                                                                          \
    } while (0)

#define DQN_TESTER_ASSERTF(test, expr, fmt, ...) DQN_TESTER_ASSERTF_AT((test), __FILE__, __LINE__, (expr), fmt, ## __VA_ARGS__)

#define DQN_TESTER_ASSERT(test, expr) DQN_TESTER_ASSERT_AT((test), __FILE__, __LINE__, (expr))

#define DQN_TESTER_LOG(test, fmt, ...)                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((test)->log_count++ == 0) fprintf(stdout, "\n");                                                           \
        fprintf(stdout, "%*s" fmt "\n", DQN_TESTER_SPACING * 2, "", ##__VA_ARGS__);                                    \
    } while (0)

#define DQN_TESTER_ASSERTF_AT(test, file, line, expr, fmt, ...)                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            if ((test)->log_count++ == 0) fprintf(stdout, "\n");                                                       \
            (test)->failed = true;                                                                                     \
            fprintf(stderr,                                                                                            \
                    "%*sFile: %s:%d\n"                                                                                 \
                    "%*sExpression: [" #expr "]\n"                                                                     \
                    "%*sReason: " fmt "\n",                                                                            \
                    DQN_TESTER_SPACING * 2,                                                                            \
                    "",                                                                                                \
                    file,                                                                                              \
                    line,                                                                                              \
                    DQN_TESTER_SPACING * 2,                                                                            \
                    "",                                                                                                \
                    DQN_TESTER_SPACING * 2,                                                                            \
                    "",                                                                                                \
                    ##__VA_ARGS__);                                                                                    \
        }                                                                                                              \
    } while (0)

#define DQN_TESTER_ASSERT_AT(test, file, line, expr)                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            if ((test)->log_count++ == 0) fprintf(stdout, "\n");                                                       \
            (test)->failed = true;                                                                                     \
            fprintf(stderr,                                                                                            \
                    "%*sFile: %s:%d\n"                                                                                 \
                    "%*sExpression: [" #expr "]\n",                                                                    \
                    DQN_TESTER_SPACING * 2,                                                                            \
                    "",                                                                                                \
                    file,                                                                                              \
                    line,                                                                                              \
                    DQN_TESTER_SPACING * 2,                                                                            \
                    "");                                                                                               \
        }                                                                                                              \
    } while (0)

// -----------------------------------------------------------------------------
// NOTE: Header
// -----------------------------------------------------------------------------
typedef struct Dqn_Tester
{
    int  num_tests_in_group;
    int  num_tests_ok_in_group;
    int  log_count;
    bool failed;
} Dqn_Tester;

void Dqn_TesterBegin(Dqn_Tester *test, char const *fmt, ...);
void Dqn_TesterEnd(Dqn_Tester *test);

#endif // DQN_TESTER_H

// -----------------------------------------------------------------------------
// NOTE: Implementation
// -----------------------------------------------------------------------------
#if defined(DQN_TESTER_IMPLEMENTATION)
void Dqn_TesterBegin(Dqn_Tester *test, char const *fmt, ...)
{
    test->num_tests_in_group++;
    test->failed    = false;
    test->log_count = 0;

    va_list args;
    va_start(args, fmt);

    int size_required = 0;
    {
        va_list args_copy;
        va_copy(args_copy, args);
        size_required = vsnprintf(NULL, 0, fmt, args_copy);
        va_end(args_copy);
    }

    printf("%*s", DQN_TESTER_SPACING, "");
    vprintf(fmt, args);
    for (int pad = DQN_TESTER_SPACING + size_required; pad < DQN_TESTER_RESULT_LPAD; pad++)
        putc(DQN_TESTER_RESULT_PAD_CHAR, stdout);
    va_end(args);
}

void Dqn_TesterEnd(Dqn_Tester *test)
{
    if (test->log_count != 0)
    {
        // NOTE: We try and print the result on the same line as the test name,
        // but if there were logs printed throughout the test then we must print
        // the result on a new line.

        printf("%*s", DQN_TESTER_SPACING, "");
        for (int pad = DQN_TESTER_SPACING; pad < DQN_TESTER_RESULT_LPAD; pad++)
            putc(DQN_TESTER_RESULT_PAD_CHAR, stdout);
    }

    if (test->failed)
        fprintf(stdout, DQN_TESTER_BAD_COLOR " FAILED");
    else
    {
        fprintf(stdout, DQN_TESTER_GOOD_COLOR " OK");
        test->num_tests_ok_in_group++;
    }

    fprintf(stdout, DQN_TESTER_COLOR_RESET "\n");

    if (test->log_count != 0)
        putc('\n', stdout);
}
#endif // DQN_TESTER_IMPLEMENTATION
