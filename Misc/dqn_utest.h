#if !defined(DQN_UTEST_H)
#define DQN_UTEST_H
//
// NOTE: Overview
// -----------------------------------------------------------------------------
// A super minimal testing framework, most of the logic here is the pretty
// printing of test results.

// NOTE: Configuration
// -----------------------------------------------------------------------------
// #define DQN_UTEST_IMPLEMENTATION
//     Define this in one and only one C++ file to enable the implementation
//     code of the header file. This will also automatically enable the JSMN
//     implementation.
//
// #define DQN_UTEST_RESULT_LPAD
//     Define this to a number to specify how much to pad the output of the test
//     result line before the test result is printed.
//
// #define DQN_UTEST_RESULT_PAD_CHAR
//     Define this to a character to specify the default character to use for
//     padding. By default this is '.'
//
// #define DQN_UTEST_SPACING
//     Define this to a number to specify the number of spaces between the group
//     declaration and the test output in the group.
//
// #define DQN_UTEST_BAD_COLOR
//     Define this to a terminal color code to specify what color errors will be
//     presented as.
//
// #define DQN_UTEST_GOOD_COLOR
//     Define this to a terminal color code to specify what color sucess will be
//     presented as.

// NOTE: Macros
// -----------------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#if !defined(DQN_UTEST_RESULT_LPAD)
    #define DQN_UTEST_RESULT_LPAD 90
#endif

#if !defined(DQN_UTEST_RESULT_PAD_CHAR)
    #define DQN_UTEST_RESULT_PAD_CHAR '.'
#endif

#if !defined(DQN_UTEST_SPACING)
    #define DQN_UTEST_SPACING 2
#endif

#if !defined(DQN_UTEST_BAD_COLOR)
    #define DQN_UTEST_BAD_COLOR "\x1b[31m"
#endif

#if !defined(DQN_UTEST_GOOD_COLOR)
    #define DQN_UTEST_GOOD_COLOR "\x1b[32m"
#endif

#define DQN_UTEST_COLOR_RESET "\x1b[0m"

#define DQN_UTEST_GROUP(test, fmt, ...) \
    for (Dqn_UTest *test_var_ = (printf(fmt "\n", ## __VA_ARGS__), &test); \
         test_var_ != nullptr; \
         Dqn_UTest_PrintStats(&test), test_var_ = nullptr)

#define DQN_UTEST_TEST(fmt, ...) \
    for (int dummy_ = (Dqn_UTest_Begin(test_var_, fmt, ## __VA_ARGS__), 0); \
         (void)dummy_, test_var_->state == Dqn_UTestState_TestBegun; \
         Dqn_UTest_End(test_var_))

#define DQN_UTEST_ASSERTF(test, expr, fmt, ...) \
    DQN_UTEST_ASSERTF_AT((test), __FILE__, __LINE__, (expr), fmt, ##__VA_ARGS__)

#define DQN_UTEST_ASSERT(test, expr) \
    DQN_UTEST_ASSERT_AT((test), __FILE__, __LINE__, (expr))

// TODO: Fix the logs. They print before the tests, we should accumulate logs
// per test, then, dump them on test on. But to do this nicely without crappy 
// mem management we need to implement an arena.
#define DQN_UTEST_LOG(fmt, ...) \
    fprintf(stdout, "%*s" fmt "\n", DQN_UTEST_SPACING * 2, "", ##__VA_ARGS__)

#define DQN_UTEST_ASSERTF_AT(test, file, line, expr, fmt, ...) \
    do {                                                       \
        if (!(expr)) {                                         \
            (test)->state = Dqn_UTestState_TestFailed;         \
            fprintf(stderr,                                    \
                    "%*sAssertion Triggered\n"                 \
                    "%*sFile: %s:%d\n"                         \
                    "%*sExpression: [" #expr "]\n"             \
                    "%*sReason: " fmt "\n",                    \
                    DQN_UTEST_SPACING * 2,                     \
                    "",                                        \
                    DQN_UTEST_SPACING * 3,                     \
                    "",                                        \
                    file,                                      \
                    line,                                      \
                    DQN_UTEST_SPACING * 3,                     \
                    "",                                        \
                    DQN_UTEST_SPACING * 3,                     \
                    "",                                        \
                    ##__VA_ARGS__);                            \
        }                                                      \
    } while (0)

#define DQN_UTEST_ASSERT_AT(test, file, line, expr)    \
    do {                                               \
        if (!(expr)) {                                 \
            (test)->state = Dqn_UTestState_TestFailed; \
            fprintf(stderr,                            \
                    "%*sFile: %s:%d\n"                 \
                    "%*sExpression: [" #expr "]\n",    \
                    DQN_UTEST_SPACING * 2,             \
                    "",                                \
                    file,                              \
                    line,                              \
                    DQN_UTEST_SPACING * 2,             \
                    "");                               \
        }                                              \
    } while (0)

// NOTE: Header
// -----------------------------------------------------------------------------
typedef enum Dqn_UTestState {
    Dqn_UTestState_Nil,
    Dqn_UTestState_TestBegun,
    Dqn_UTestState_TestFailed,
} Dqn_UTestState;

typedef struct Dqn_UTest {
    int             num_tests_in_group;
    int             num_tests_ok_in_group;
    Dqn_UTestState  state;
    bool            finished;
    char            name[1024];
    size_t          name_size;
} Dqn_UTest;

void Dqn_UTest_PrintStats(Dqn_UTest *test);
void Dqn_UTest_BeginV(Dqn_UTest *test, char const *fmt, va_list args);
void Dqn_UTest_Begin(Dqn_UTest *test, char const *fmt, ...);
void Dqn_UTest_End(Dqn_UTest *test);

// NOTE: Implementation
// -----------------------------------------------------------------------------
#if defined(DQN_UTEST_IMPLEMENTATION)
void Dqn_UTest_PrintStats(Dqn_UTest *test)
{
    if (test->finished)
        return;

    test->finished = true;
    bool all_clear = test->num_tests_ok_in_group == test->num_tests_in_group;
    fprintf(stdout,
            "%s\n  %02d/%02d tests passed -- %s\n\n" DQN_UTEST_COLOR_RESET,
            all_clear ? DQN_UTEST_GOOD_COLOR : DQN_UTEST_BAD_COLOR,
            test->num_tests_ok_in_group,
            test->num_tests_in_group,
            all_clear ? "OK" : "FAILED");
}

void Dqn_UTest_BeginV(Dqn_UTest *test, char const *fmt, va_list args)
{
    assert(test->state == Dqn_UTestState_Nil &&
           "Nesting a unit test within another unit test is not allowed, ensure"
           "the first test has finished by calling Dqn_UTest_End");

    test->num_tests_in_group++;
    test->state     = Dqn_UTestState_TestBegun;

    test->name_size = 0;
    {
        va_list args_copy;
        va_copy(args_copy, args);
        test->name_size = vsnprintf(NULL, 0, fmt, args_copy);
        va_end(args_copy);
    }

    assert(test->name_size < sizeof(test->name));
    vsnprintf(test->name, sizeof(test->name), fmt, args);
}

void Dqn_UTest_Begin(Dqn_UTest *test, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_UTest_BeginV(test, fmt, args);
    va_end(args);
}

void Dqn_UTest_End(Dqn_UTest *test)
{
    assert(test->state != Dqn_UTestState_Nil && "Test was marked as ended but a test was never commenced using Dqn_UTest_Begin");
    size_t pad_size = DQN_UTEST_RESULT_LPAD - (DQN_UTEST_SPACING + test->name_size);
    if (pad_size < 0)
        pad_size = 0;

    char pad_buffer[DQN_UTEST_RESULT_LPAD] = {};
    memset(pad_buffer, DQN_UTEST_RESULT_PAD_CHAR, pad_size);

    printf("%*s%.*s%.*s", DQN_UTEST_SPACING, "", (int)test->name_size, test->name, (int)pad_size, pad_buffer);
    if (test->state == Dqn_UTestState_TestFailed) {
        printf(DQN_UTEST_BAD_COLOR " FAILED");
    } else {
        printf(DQN_UTEST_GOOD_COLOR " OK");
        test->num_tests_ok_in_group++;
    }
    printf(DQN_UTEST_COLOR_RESET "\n");
    test->state = Dqn_UTestState_Nil;
}
#endif // DQN_UTEST_IMPLEMENTATION
#endif // DQN_UTEST_H
