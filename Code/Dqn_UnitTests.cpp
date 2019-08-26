#define _CRT_SECURE_NO_WARNINGS
#include "Dqn.h"

struct TestState
{
    Dqn_MemArena arena;
    int          indent_level;
    Slice<char>  name;
    Slice<char>  fail_expr;
    Slice<char>  fail_msg;
    bool         scope_started;
};

struct TestingState
{
    int       num_tests_in_group;
    int       num_tests_ok_in_group;
    TestState test;
};

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define TEST_START_SCOPE(testing_state, test_name)                                                                     \
    DQN_DEFER                                                                                                          \
    {                                                                                                                  \
        if (testing_state.test.fail_expr.len == 0) testing_state.num_tests_ok_in_group++;                              \
        TestState_PrintResult(&testing_state.test);                                                                    \
        testing_state.test = {};                                                                                       \
    };                                                                                                                 \
    testing_state.test.name          = AsprintfSlice(&testing_state.test.arena, test_name);                            \
    testing_state.test.scope_started = true;                                                                           \
    testing_state.num_tests_in_group++

#define TEST_DECLARE_GROUP_SCOPED(testing_state, name)                                                                 \
    fprintf(stdout, "\n" name "\n");                                                                                   \
    DQN_DEFER                                                                                                          \
    {                                                                                                                  \
        TestingState_PrintGroupResult(&testing_state);                                                                 \
        testing_state = {};                                                                                            \
    }

#define TEST_EXPECT_MSG(testing_state, expr, msg, ...)                                                                 \
    DQN_ASSERT(testing_state.test.scope_started);                                                                      \
    if (!(expr))                                                                                                       \
    {                                                                                                                  \
        testing_state.test.fail_expr = AsprintfSlice(&testing_state.test.arena, #expr);                                \
        testing_state.test.fail_msg  = AsprintfSlice(&testing_state.test.arena, msg, ##__VA_ARGS__);                   \
    }

#define TEST_EXPECT(testing_state, expr) TEST_EXPECT_MSG(testing_state, expr, "")

void TestingState_PrintGroupResult(TestingState const *result)
{
    char constexpr INDENT[]   = "  ";
    int constexpr DESIRED_LEN = 72;
    char const STATUS_OK[]    = "OK";
    char const STATUS_FAIL[]  = "FAIL";

    bool all_tests_passed = (result->num_tests_ok_in_group == result->num_tests_in_group);
    char buf[256] = {};
    int len = snprintf(buf, Dqn_ArrayCount(buf), "%02d/%02d Tests Passed ", result->num_tests_ok_in_group, result->num_tests_in_group);
    isize remaining_len = DESIRED_LEN - len - 1;
    remaining_len       = (all_tests_passed) ? remaining_len - Dqn_CharCount(STATUS_OK) : remaining_len - Dqn_CharCount(STATUS_FAIL);
    remaining_len       = DQN_MAX(remaining_len, 0);
    DQN_FOR_EACH(i, remaining_len) fprintf(stdout, " ");

    fprintf(stdout, "%s", buf);
    if (result->num_tests_ok_in_group == result->num_tests_in_group)
        fprintf(stdout, ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, STATUS_OK);
    else
        fprintf(stdout, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET, STATUS_FAIL);
}

void TestState_PrintResult(TestState const *result)
{
    char constexpr INDENT[]   = "  ";
    int constexpr DESIRED_LEN = 72;
    fprintf(stdout, "%s%s", INDENT, result->name.str);

    char const STATUS_OK[]   = "OK";
    char const STATUS_FAIL[] = "FAIL";

    isize remaining_len = DESIRED_LEN - result->name.len - Dqn_CharCount(INDENT);
    remaining_len       = (result->fail_expr.str) ? remaining_len - Dqn_CharCount(STATUS_FAIL) : remaining_len - Dqn_CharCount(STATUS_OK);
    remaining_len       = DQN_MAX(remaining_len, 0);

    DQN_FOR_EACH(i, remaining_len) fprintf(stdout, ".");
    if (result->fail_expr.str)
    {
        fprintf(stdout, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET "\n", STATUS_FAIL);
        fprintf(stdout, "%s%sReason: [%s] %s\n", INDENT, INDENT, result->fail_expr.str, result->fail_msg.str);
    }
    else
    {
        fprintf(stdout, ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET "\n", STATUS_OK);
    }
}

FILE_SCOPE void UnitTests()
{
    TestingState testing_state = {};
    // ---------------------------------------------------------------------------------------------
    //
    // NOTE: Dqn_StringBuilder
    //
    // ---------------------------------------------------------------------------------------------
    {
        TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_StringBuilder");
        // NOTE: Dqn_StringBuilder_Append
        {
            {
                TEST_START_SCOPE(testing_state, "Append variable length strings and build using malloc");
                Dqn_StringBuilder<> builder = {};
                Dqn_StringBuilder_Append(&builder, "Abc", 1);
                Dqn_StringBuilder_Append(&builder, "cd");
                isize len    = 0;
                char *result = Dqn_StringBuilder_BuildFromMalloc(&builder, &len);
                DQN_DEFER { free(result); };

                char constexpr EXPECT_STR[] = "Acd";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR) + 1 /*null terminator*/, "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }

            {
                TEST_START_SCOPE(testing_state, "Append empty string and build using malloc");
                Dqn_StringBuilder<> builder = {};
                Dqn_StringBuilder_Append(&builder, "");
                Dqn_StringBuilder_Append(&builder, "");
                isize len    = 0;
                char *result = Dqn_StringBuilder_BuildFromMalloc(&builder, &len);
                DQN_DEFER { free(result); };

                char constexpr EXPECT_STR[] = "";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR) + 1 /*null terminator*/, "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }

            {
                TEST_START_SCOPE(testing_state, "Append empty string onto string and build using malloc");
                Dqn_StringBuilder<> builder = {};
                Dqn_StringBuilder_Append(&builder, "Acd");
                Dqn_StringBuilder_Append(&builder, "");
                isize len    = 0;
                char *result = Dqn_StringBuilder_BuildFromMalloc(&builder, &len);
                DQN_DEFER { free(result); };

                char constexpr EXPECT_STR[] = "Acd";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR) + 1 /*null terminator*/, "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }

            {
                TEST_START_SCOPE(testing_state, "Append nullptr and build using malloc");
                Dqn_StringBuilder<> builder = {};
                Dqn_StringBuilder_Append(&builder, nullptr, 5);
                isize len    = 0;
                char *result = Dqn_StringBuilder_BuildFromMalloc(&builder, &len);
                DQN_DEFER { free(result); };

                char constexpr EXPECT_STR[] = "";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR) + 1 /*null terminator*/, "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }

            {
                TEST_START_SCOPE(testing_state, "Append and require new linked buffer and build using malloc");
                Dqn_StringBuilder<2> builder = {};
                Dqn_StringBuilder_Append(&builder, "A");
                Dqn_StringBuilder_Append(&builder, "z"); // Should force a new memory block
                Dqn_StringBuilder_Append(&builder, "tec");
                isize len    = 0;
                char *result = Dqn_StringBuilder_BuildFromMalloc(&builder, &len);
                DQN_DEFER { free(result); };

                char constexpr EXPECT_STR[] = "Aztec";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR) + 1 /*null terminator*/, "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }
        }


        // NOTE: Dqn_StringBuilder_AppendChar
        {
            TEST_START_SCOPE(testing_state, "Append char and build using malloc");
            Dqn_StringBuilder<> builder = {};
            Dqn_StringBuilder_AppendChar(&builder, 'a');
            Dqn_StringBuilder_AppendChar(&builder, 'b');
            isize len    = 0;
            char *result = Dqn_StringBuilder_BuildFromMalloc(&builder, &len);
            DQN_DEFER { free(result); };

            char constexpr EXPECT_STR[] = "ab";
            TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR) + 1 /*null terminator*/, "len: %zd", len);
            TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
        }

        // NOTE: Dqn_StringBuilder_FmtAppend
        {
            {
                TEST_START_SCOPE(testing_state, "Append format string and build using malloc");
                Dqn_StringBuilder<> builder = {};
                Dqn_StringBuilder_FmtAppend(&builder, "Number: %d, String: %s, ", 4, "Hello Sailor");
                Dqn_StringBuilder_FmtAppend(&builder, "Extra Stuff");
                isize len    = 0;
                char *result = Dqn_StringBuilder_BuildFromMalloc(&builder, &len);
                DQN_DEFER { free(result); };

                char constexpr EXPECT_STR[] = "Number: 4, String: Hello Sailor, Extra Stuff";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR) + 1 /*null terminator*/, "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }

            {
                TEST_START_SCOPE(testing_state, "Append nullptr format string and build using malloc");
                Dqn_StringBuilder<> builder = {};
                Dqn_StringBuilder_FmtAppend(&builder, nullptr);
                isize len    = 0;
                char *result = Dqn_StringBuilder_BuildFromMalloc(&builder, &len);
                DQN_DEFER { free(result); };

                char constexpr EXPECT_STR[] = "";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR) + 1 /*null terminator*/, "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }
        }
    }

    // ---------------------------------------------------------------------------------------------
    //
    // NOTE: Dqn_FixedArray
    //
    // ---------------------------------------------------------------------------------------------
    {
        TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_FixedArray");
        // NOTE: Dqn_FixedArray_Init
        {
            TEST_START_SCOPE(testing_state, "Initialise from raw array");
            int raw_array[] = {1, 2};
            auto array = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
            TEST_EXPECT(testing_state, array.len == 2);
            TEST_EXPECT(testing_state, array[0] == 1);
            TEST_EXPECT(testing_state, array[1] == 2);
        }

        // NOTE: Dqn_FixedArray_EraseStable
        {
            TEST_START_SCOPE(testing_state, "Erase stable 1 element from array");
            int raw_array[] = {1, 2, 3};
            auto array = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
            Dqn_FixedArray_EraseStable(&array, 1);
            TEST_EXPECT(testing_state, array.len == 2);
            TEST_EXPECT(testing_state, array[0] == 1);
            TEST_EXPECT(testing_state, array[1] == 3);
        }

        // NOTE: Dqn_FixedArray_EraseUnstable
        {
            TEST_START_SCOPE(testing_state, "Erase unstable 1 element from array");
            int raw_array[] = {1, 2, 3};
            auto array = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
            Dqn_FixedArray_EraseUnstable(&array, 0);
            TEST_EXPECT(testing_state, array.len == 2);
            TEST_EXPECT(testing_state, array[0] == 3);
            TEST_EXPECT(testing_state, array[1] == 2);
        }

        // NOTE: Dqn_FixedArray_Add
        {
            TEST_START_SCOPE(testing_state, "Add 1 element to array");
            int const ITEM  = 2;
            int raw_array[] = {1};
            auto array      = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
            Dqn_FixedArray_Add(&array, ITEM);
            TEST_EXPECT(testing_state, array.len == 2);
            TEST_EXPECT(testing_state, array[0] == 1);
            TEST_EXPECT(testing_state, array[1] == ITEM);
        }

        // NOTE: Dqn_FixedArray_Clear
        {
            TEST_START_SCOPE(testing_state, "Clear array");
            int raw_array[] = {1};
            auto array      = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
            Dqn_FixedArray_Clear(&array);
            TEST_EXPECT(testing_state, array.len == 0);
        }
    }

    // ---------------------------------------------------------------------------------------------
    //
    // NOTE: Dqn_StrToI64
    //
    // ---------------------------------------------------------------------------------------------
    {
        TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_StrToI64");
        {
            TEST_START_SCOPE(testing_state, "Convert nullptr");
            i64 result = Dqn_StrToI64(nullptr);
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert empty string");
            i64 result = Dqn_StrToI64("");
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1\"");
            i64 result = Dqn_StrToI64("1");
            TEST_EXPECT(testing_state, result == 1);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"-0\"");
            i64 result = Dqn_StrToI64("-0");
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"-1\"");
            i64 result = Dqn_StrToI64("-1");
            TEST_EXPECT(testing_state, result == -1);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1.2\"");
            i64 result = Dqn_StrToI64("1.2");
            TEST_EXPECT(testing_state, result == 1);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1,234\"");
            i64 result = Dqn_StrToI64("1,234");
            TEST_EXPECT(testing_state, result == 1234);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1,2\"");
            i64 result = Dqn_StrToI64("1,2");
            TEST_EXPECT(testing_state, result == 12);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"12a3\"");
            i64 result = Dqn_StrToI64("12a3");
            TEST_EXPECT(testing_state, result == 12);
        }
    }

    // ---------------------------------------------------------------------------------------------
    //
    // NOTE: Dqn_StrToU64
    //
    // ---------------------------------------------------------------------------------------------
    {
        TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_StrToU64");
        {
            TEST_START_SCOPE(testing_state, "Convert nullptr");
            u64 result = Dqn_StrToU64(nullptr);
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert empty string");
            u64 result = Dqn_StrToU64("");
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1\"");
            u64 result = Dqn_StrToU64("1");
            TEST_EXPECT(testing_state, result == 1);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"-0\"");
            u64 result = Dqn_StrToU64("-0");
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"-1\"");
            u64 result = Dqn_StrToU64("-1");
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1.2\"");
            u64 result = Dqn_StrToU64("1.2");
            TEST_EXPECT(testing_state, result == 1);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1,234\"");
            u64 result = Dqn_StrToU64("1,234");
            TEST_EXPECT(testing_state, result == 1234);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1,2\"");
            u64 result = Dqn_StrToU64("1,2");
            TEST_EXPECT(testing_state, result == 12);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"12a3\"");
            u64 result = Dqn_StrToU64("12a3");
            TEST_EXPECT(testing_state, result == 12);
        }
    }
}

int main(char *argv[], int argc)
{
    (void)argv; (void)argc;
    UnitTests();

    return 0;
}

