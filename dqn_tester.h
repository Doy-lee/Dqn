#if !defined(DQN_TESTER_H)
#define DQN_TESTER_H
//
// NOTE: Overview
// -----------------------------------------------------------------------------
// A super minimal testing framework, most of the logic here is the pretty
// printing of test results.

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

// NOTE: Macros
// -----------------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

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
#define DQN_TESTER_TOKEN_COMBINE2(x, y) x ## y_
#define DQN_TESTER_TOKEN_COMBINE(x, y) DQN_TESTER_TOKEN_COMBINE2(x, y)

/// Creates <prefix><line_number>_ .e. group_123_
#define DQN_TESTER_UNIQUE_NAME(prefix) \
    DQN_TESTER_TOKEN_COMBINE(DQN_TESTER_TOKEN_COMBINE(prefix, __LINE__), _)

#define DQN_TESTER_GROUP(group, fmt, ...) \
    for (Dqn_Tester *test_group_ = (group = Dqn_Tester_BeginGroup(fmt, ## __VA_ARGS__), &group); \
         !test_group_->finished; \
         Dqn_Tester_EndGroup(test_group_))

#define DQN_TESTER_TEST(fmt, ...) \
    for (int DQN_TESTER_UNIQUE_NAME(dummy_) = (Dqn_Tester_Begin(test_group_, fmt, ## __VA_ARGS__), 0); \
         (void)DQN_TESTER_UNIQUE_NAME(dummy_), test_group_->state == Dqn_TesterTestState_TestBegun; \
         Dqn_Tester_End(test_group_))

#define DQN_TESTER_ASSERTF(test, expr, fmt, ...) \
    DQN_TESTER_ASSERTF_AT((test), __FILE__, __LINE__, (expr), fmt, ##__VA_ARGS__)

#define DQN_TESTER_ASSERT(test, expr) DQN_TESTER_ASSERT_AT((test), __FILE__, __LINE__, (expr))

#define DQN_TESTER_LOG(test, fmt, ...)                                                       \
    do {                                                                                     \
        if ((test)->log_count++ == 0) {                                                      \
            fprintf(stdout, "\n");                                                           \
        }                                                                                    \
        fprintf(stdout, "%*sLog: " fmt "\n", DQN_TESTER_SPACING * 2, "", ##__VA_ARGS__); \
    } while (0)

#define DQN_TESTER_ASSERTF_AT(test, file, line, expr, fmt, ...) \
    do {                                                        \
        if (!(expr)) {                                          \
            if ((test)->log_count++ == 0) {                     \
                fprintf(stdout, "\n");                          \
            }                                                   \
            (test)->state = Dqn_TesterTestState_TestFailed;         \
            fprintf(stderr,                                     \
                    "%*sAssertion Triggered\n"                  \
                    "%*sFile: %s:%d\n"                          \
                    "%*sExpression: [" #expr "]\n"              \
                    "%*sReason: " fmt "\n",                     \
                    DQN_TESTER_SPACING * 2,                     \
                    "",                                         \
                    DQN_TESTER_SPACING * 3,                     \
                    "",                                         \
                    file,                                       \
                    line,                                       \
                    DQN_TESTER_SPACING * 3,                     \
                    "",                                         \
                    DQN_TESTER_SPACING * 3,                     \
                    "",                                         \
                    ##__VA_ARGS__);                             \
        }                                                       \
    } while (0)

#define DQN_TESTER_ASSERT_AT(test, file, line, expr)    \
    do {                                                \
        if (!(expr)) {                                  \
            if ((test)->log_count++ == 0) {             \
                fprintf(stdout, "\n");                  \
            }                                           \
            (test)->state = Dqn_TesterTestState_TestFailed; \
            fprintf(stderr,                             \
                    "%*sFile: %s:%d\n"                  \
                    "%*sExpression: [" #expr "]\n",     \
                    DQN_TESTER_SPACING * 2,             \
                    "",                                 \
                    file,                               \
                    line,                               \
                    DQN_TESTER_SPACING * 2,             \
                    "");                                \
        }                                               \
    } while (0)

// NOTE: Header
// -----------------------------------------------------------------------------
typedef enum Dqn_TesterTestState {
    Dqn_TesterTestState_Nil,
    Dqn_TesterTestState_TestBegun,
    Dqn_TesterTestState_TestFailed,
} Dqn_TesterTestState;

typedef struct Dqn_Tester {
    int                 num_tests_in_group;
    int                 num_tests_ok_in_group;
    int                 log_count;
    Dqn_TesterTestState state;
    bool                finished;
} Dqn_Tester;

Dqn_Tester Dqn_Tester_BeginGroupV(char const *fmt, va_list args);
Dqn_Tester Dqn_Tester_BeginGroup(char const *fmt, ...);
void Dqn_Tester_EndGroup(Dqn_Tester *test);

void Dqn_Tester_BeginV(Dqn_Tester *test, char const *fmt, va_list args);
void Dqn_Tester_Begin(Dqn_Tester *test, char const *fmt, ...);
void Dqn_Tester_End(Dqn_Tester *test);

// NOTE: Implementation
// -----------------------------------------------------------------------------
#if defined(DQN_TESTER_IMPLEMENTATION)
Dqn_Tester Dqn_Tester_BeginGroupV(char const *fmt, va_list args)
{
    fprintf(stdout, fmt, args);
    fputc('\n', stdout);
    Dqn_Tester result = {};
    return result;
}

Dqn_Tester Dqn_Tester_BeginGroup(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Tester result = Dqn_Tester_BeginGroupV(fmt, args);
    va_end(args);
    return result;
}

void Dqn_Tester_EndGroup(Dqn_Tester *test)
{
    if (test->finished)
        return;

    test->finished = true;
    bool all_clear = test->num_tests_ok_in_group == test->num_tests_in_group;
    fprintf(stdout,
            "%s\n  %02d/%02d tests passed -- %s\n\n" DQN_TESTER_COLOR_RESET,
            all_clear ? DQN_TESTER_GOOD_COLOR : DQN_TESTER_BAD_COLOR,
            test->num_tests_ok_in_group,
            test->num_tests_in_group,
            all_clear ? "OK" : "FAILED");
}

void Dqn_Tester_BeginV(Dqn_Tester *test, char const *fmt, va_list args)
{
    assert(test->state == Dqn_TesterTestState_Nil &&
           "Nesting a unit test within another unit test is not allowed, ensure"
           "the first test has finished by calling Dqn_Tester_End");

    test->num_tests_in_group++;
    test->state     = Dqn_TesterTestState_TestBegun;
    test->log_count = 0;

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
}

void Dqn_Tester_Begin(Dqn_Tester *test, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_Tester_BeginV(test, fmt, args);
    va_end(args);
}

void Dqn_Tester_End(Dqn_Tester *test)
{
    assert(test->state != Dqn_TesterTestState_Nil && "Test was marked as ended but a test was never commenced using Dqn_Tester_Begin");
    if (test->log_count != 0) {
        // NOTE: We try and print the result on the same line as the test name,
        // but if there were logs printed throughout the test then we must print
        // the result on a new line.
        printf("%*s", DQN_TESTER_SPACING, "");
        for (int pad = DQN_TESTER_SPACING; pad < DQN_TESTER_RESULT_LPAD; pad++)
            putc(DQN_TESTER_RESULT_PAD_CHAR, stdout);
    }

    if (test->state == Dqn_TesterTestState_TestFailed) {
        fprintf(stdout, DQN_TESTER_BAD_COLOR " FAILED");
    } else {
        fprintf(stdout, DQN_TESTER_GOOD_COLOR " OK");
        test->num_tests_ok_in_group++;
    }

    fprintf(stdout, DQN_TESTER_COLOR_RESET "\n");
    if (test->log_count != 0) {
        putc('\n', stdout);
    }

    test->state = Dqn_TesterTestState_Nil;
}
#endif // DQN_TESTER_IMPLEMENTATION
#endif // DQN_TESTER_H
