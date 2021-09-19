// -------------------------------------------------------------------------------------------------
// NOTE: Preprocessor Config
// -------------------------------------------------------------------------------------------------
/*
#define DQN_TEST_WITH_MAIN      Define this to enable the main function and allow standalone compiling
                                and running of the file.
#define DQN_TEST_NO_ANSI_COLORS Define this to disable any ANSI terminal color codes from output
*/

#if defined(DQN_TEST_WITH_MAIN)
    #define DQN_IMPLEMENTATION
    #define DQN_WITH_CRT_ALLOCATOR  // Dqn_CRTAllocator
    #define DQN_WITH_DSMAP          // Dqn_DSMap
    #define DQN_WITH_FIXED_ARRAY    // Dqn_FixedArray
    #define DQN_WITH_FIXED_STRING   // Dqn_FixedString
    #define DQN_WITH_HEX            // Dqn_Hex and friends ...
    #define DQN_WITH_JSON_WRITER    // Dqn_JsonWriter
    #define DQN_WITH_MAP            // Dqn_Map
    #define DQN_WITH_MATH           // Dqn_V2/3/4/Mat4 and friends ...
    #define DQN_WITH_THREAD_CONTEXT // Dqn_ThreadContext and friends ...
    #include "dqn.h"

    #define DQN_KECCAK_IMPLEMENTATION
    #include "dqn_keccak.h"
#endif

#include "dqn_tests_helpers.cpp"

struct Dqn_TestState
{
    int           indent_level;
    Dqn_String    name;
    Dqn_String    fail_expr;
    Dqn_String    fail_msg;
    bool          scope_started;
};

struct Dqn_TestingState
{
    int                num_tests_in_group;
    int                num_tests_ok_in_group;
    Dqn_TestState      test;
    Dqn_ArenaAllocator arena;
};

static int g_dqn_test_total_good_tests;
static int g_dqn_test_total_tests;

#if defined(DQN_TEST_NO_ANSI_COLORS)
    #define DQN_TEST_ANSI_COLOR_RED
    #define DQN_TEST_ANSI_COLOR_GREEN
    #define DQN_TEST_ANSI_COLOR_YELLOW
    #define DQN_TEST_ANSI_COLOR_BLUE
    #define DQN_TEST_ANSI_COLOR_MAGENTA
    #define DQN_TEST_ANSI_COLOR_CYAN
    #define DQN_TEST_ANSI_COLOR_RESET
#else
    #define DQN_TEST_ANSI_COLOR_RED "\x1b[31m"
    #define DQN_TEST_ANSI_COLOR_GREEN "\x1b[32m"
    #define DQN_TEST_ANSI_COLOR_YELLOW "\x1b[33m"
    #define DQN_TEST_ANSI_COLOR_BLUE "\x1b[34m"
    #define DQN_TEST_ANSI_COLOR_MAGENTA "\x1b[35m"
    #define DQN_TEST_ANSI_COLOR_CYAN "\x1b[36m"
    #define DQN_TEST_ANSI_COLOR_RESET "\x1b[0m"
#endif

#define DQN_TEST_START_SCOPE(testing_state, test_name, ...)                                                            \
    DQN_DEFER                                                                                                          \
    {                                                                                                                  \
        if (testing_state.test.fail_expr.size == 0) testing_state.num_tests_ok_in_group++;                             \
        Dqn_TestState_PrintResult(&testing_state.test);                                                                \
        Dqn_ArenaAllocator_ResetUsage(&testing_state.arena, Dqn_ZeroMem::No);                                          \
        testing_state.test = {};                                                                                       \
    };                                                                                                                 \
    testing_state.test.name          = Dqn_String_Fmt(&testing_state.arena, test_name, ##__VA_ARGS__);                 \
    testing_state.test.scope_started = true;                                                                           \
    testing_state.num_tests_in_group++

#define DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, name)                                                             \
    fprintf(stdout, name "\n");                                                                                        \
    DQN_DEFER                                                                                                          \
    {                                                                                                                  \
        g_dqn_test_total_good_tests += testing_state.num_tests_ok_in_group; \
        g_dqn_test_total_tests      += testing_state.num_tests_in_group; \
        Dqn_TestingState_PrintGroupResult(&testing_state);                                                             \
        testing_state = {};                                                                                            \
        fprintf(stdout, "\n\n");                                                                                       \
    }

#define DQN_TEST_EXPECT_MSG(testing_state, expr, msg, ...)                                                             \
    DQN_ASSERT(testing_state.test.scope_started);                                                                      \
    if (!(expr))                                                                                                       \
    {                                                                                                                  \
        testing_state.test.fail_expr = DQN_STRING(#expr);                                                              \
        testing_state.test.fail_msg  = Dqn_String_Fmt(&testing_state.arena, msg, ##__VA_ARGS__);                       \
    }

#define DQN_TEST_EXPECT(testing_state, expr) DQN_TEST_EXPECT_MSG(testing_state, expr, "")

void Dqn_TestingState_PrintGroupResult(Dqn_TestingState const *result)
{
    int const  DESIRED_LEN   = 72;
    char const STATUS_OK[]   = "OK";
    char const STATUS_FAIL[] = "FAIL";

    bool all_tests_passed = (result->num_tests_ok_in_group == result->num_tests_in_group);
    char buf[256] = {};
    int size = snprintf(buf, Dqn_ArrayCount(buf), "%02d/%02d Tests Passed ", result->num_tests_ok_in_group, result->num_tests_in_group);
    Dqn_isize remaining_size = DESIRED_LEN - size;
    remaining_size       = (all_tests_passed) ? remaining_size - Dqn_CharCount(STATUS_OK) : remaining_size - Dqn_CharCount(STATUS_FAIL);
    remaining_size       = DQN_MAX(remaining_size, 0);
    DQN_FOR_EACH(i, remaining_size) fprintf(stdout, " ");

    fprintf(stdout, "%s", buf);
    if (result->num_tests_ok_in_group == result->num_tests_in_group)
        fprintf(stdout, DQN_TEST_ANSI_COLOR_GREEN "%s" DQN_TEST_ANSI_COLOR_RESET, STATUS_OK);
    else
        fprintf(stdout, DQN_TEST_ANSI_COLOR_RED "%s" DQN_TEST_ANSI_COLOR_RESET, STATUS_FAIL);
}

void Dqn_TestState_PrintResult(Dqn_TestState const *result)
{
    char const INDENT[]   = "  ";
    int const DESIRED_LEN = 72;
    fprintf(stdout, "%s%s", INDENT, result->name.str);

    char const STATUS_OK[]   = "OK";
    char const STATUS_FAIL[] = "FAIL";

    Dqn_isize remaining_size = DESIRED_LEN - result->name.size - Dqn_CharCount(INDENT);
    remaining_size = (result->fail_expr.str) ? remaining_size - Dqn_CharCount(STATUS_FAIL) : remaining_size - Dqn_CharCount(STATUS_OK);
    remaining_size = DQN_MAX(remaining_size, 0);

    DQN_FOR_EACH(i, remaining_size) fprintf(stdout, ".");
    if (result->fail_expr.str)
    {
        fprintf(stdout, DQN_TEST_ANSI_COLOR_RED "%s" DQN_TEST_ANSI_COLOR_RESET "\n", STATUS_FAIL);
        fprintf(stdout, "%s%sReason: Expression failed (%s) %s\n", INDENT, INDENT, result->fail_expr.str, result->fail_msg.str);
    }
    else
    {
        fprintf(stdout, DQN_TEST_ANSI_COLOR_GREEN "%s" DQN_TEST_ANSI_COLOR_RESET "\n", STATUS_OK);
    }
}

void Dqn_Test_Array()
{
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Array");
    // NOTE: Dqn_Array_InitWithMemory
    {
        {
            DQN_TEST_START_SCOPE(testing_state, "Fixed Memory: Test add single item and can't allocate more");
            int memory[4]        = {};
            Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*size*/);
            Dqn_Array_Add(&array, 1);
            Dqn_Array_Add(&array, 2);
            Dqn_Array_Add(&array, 3);
            Dqn_Array_Add(&array, 4);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[0] == 1, "array.data %d", array.data[0]);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[1] == 2, "array.data %d", array.data[1]);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[2] == 3, "array.data %d", array.data[2]);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[3] == 4, "array.data %d", array.data[3]);
            DQN_TEST_EXPECT_MSG(testing_state, array.size == 4, "array.size: %d", array.size);

            int *added_item = Dqn_Array_Add(&array, 5);
            DQN_TEST_EXPECT(testing_state, added_item == nullptr);
            DQN_TEST_EXPECT_MSG(testing_state, array.size == 4, "array.size: %d", array.size);
            DQN_TEST_EXPECT_MSG(testing_state, array.max == 4, "array.max: %d", array.max);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Fixed Memory: Test add array of items");
            int memory[4]        = {};
            int DATA[]           = {1, 2, 3};
            Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*size*/);
            Dqn_Array_AddArray(&array, DATA, Dqn_ArrayCount(DATA));
            DQN_TEST_EXPECT_MSG(testing_state, array.data[0] == 1, "array.data %d", array.data[0]);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[1] == 2, "array.data %d", array.data[1]);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[2] == 3, "array.data %d", array.data[2]);
            DQN_TEST_EXPECT_MSG(testing_state, array.size == 3, "array.size: %d", array.size);
            DQN_TEST_EXPECT_MSG(testing_state, array.max == 4, "array.max: %d", array.max);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Fixed Memory: Test clear and clear with memory zeroed");
            int memory[4]        = {};
            int DATA[]           = {1, 2, 3};
            Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*size*/);
            Dqn_Array_AddArray(&array, DATA, Dqn_ArrayCount(DATA));
            Dqn_Array_Clear(&array, Dqn_ZeroMem::No);
            DQN_TEST_EXPECT_MSG(testing_state, array.size == 0, "array.size: %d", array.size);
            DQN_TEST_EXPECT_MSG(testing_state, array.max == 4, "array.max: %d", array.max);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[0] == 1, "array.data %d. Clear but don't zero memory so old values should still remain", array.data[0]);

            Dqn_Array_Clear(&array, Dqn_ZeroMem::Yes);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[0] == 0, "array.data %d. Clear but zero memory old values should not remain", array.data[0]);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Fixed Memory: Test erase stable and erase unstable");
            int memory[4]        = {};
            int DATA[]           = {1, 2, 3, 4};
            Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*size*/);
            Dqn_Array_AddArray(&array, DATA, Dqn_ArrayCount(DATA));
            Dqn_Array_EraseUnstable(&array, 1);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[0] == 1, "array.data %d", array.data[0]);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[1] == 4, "array.data %d", array.data[1]);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[2] == 3, "array.data %d", array.data[2]);
            DQN_TEST_EXPECT_MSG(testing_state, array.size == 3, "array.size: %d", array.size);

            Dqn_Array_EraseStable(&array, 0);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[0] == 4, "array.data: %d", array.data[0]);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[1] == 3, "array.data: %d", array.data[1]);
            DQN_TEST_EXPECT_MSG(testing_state, array.size == 2, "array.size: %d", array.size);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Fixed Memory: Test array pop and peek");
            int memory[4]        = {};
            int DATA[]           = {1, 2, 3};
            Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*size*/);
            Dqn_Array_AddArray(&array, DATA, Dqn_ArrayCount(DATA));
            Dqn_Array_Pop(&array, 2);
            DQN_TEST_EXPECT_MSG(testing_state, array.data[0] == 1, "array.data: %d", array.data[0]);
            DQN_TEST_EXPECT_MSG(testing_state, array.size == 1, "array.size: %d", array.size);
            DQN_TEST_EXPECT_MSG(testing_state, array.max == 4, "array.max: %d", array.max);

            int *peek_item = Dqn_Array_Peek(&array);
            DQN_TEST_EXPECT_MSG(testing_state, *peek_item == 1, "peek: %d", *peek_item);
            DQN_TEST_EXPECT_MSG(testing_state, array.size == 1, "array.size: %d", array.size);
            DQN_TEST_EXPECT_MSG(testing_state, array.max == 4, "array.max: %d", array.max);
        }
    }

    // NOTE: Dynamic Memory: Dqn_Array
    {
        DQN_TEST_START_SCOPE(testing_state, "Dynamic Memory: Reserve and check over commit reallocates");
        Dqn_ArenaAllocator arena = {};
        Dqn_Array<int> array = {};
        array.arena = &arena;

        Dqn_Array_Reserve(&array, 4);
        DQN_TEST_EXPECT_MSG(testing_state, array.size == 0, "array.size: %d", array.size);
        DQN_TEST_EXPECT_MSG(testing_state, array.max == 4, "array.max: %d", array.max);

        int DATA[] = {1, 2, 3, 4};
        Dqn_Array_AddArray(&array, DATA, Dqn_ArrayCount(DATA));
        DQN_TEST_EXPECT_MSG(testing_state, array.data[0] == 1, "array.data: %d", array.data[0]);
        DQN_TEST_EXPECT_MSG(testing_state, array.data[1] == 2, "array.data: %d", array.data[1]);
        DQN_TEST_EXPECT_MSG(testing_state, array.data[2] == 3, "array.data: %d", array.data[2]);
        DQN_TEST_EXPECT_MSG(testing_state, array.data[3] == 4, "array.data: %d", array.data[3]);
        DQN_TEST_EXPECT_MSG(testing_state, array.size == 4, "array.size: %d", array.size);

        int *added_item = Dqn_Array_Add(&array, 5);
        DQN_TEST_EXPECT_MSG(testing_state, *added_item == 5, "added_item: %d", *added_item);
        DQN_TEST_EXPECT_MSG(testing_state, array.data[4] == 5, "array.data: %d", array.data[4]);
        DQN_TEST_EXPECT_MSG(testing_state, array.size == 5, "array.size: %d", array.size);
        DQN_TEST_EXPECT_MSG(testing_state, array.max >= 5, "array.max: %d", array.max);

        Dqn_ArenaAllocator_Free(&arena);
    }
}

void Dqn_Test_File()
{
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_File");
    {
        Dqn_ArenaAllocator arena = {};
        DQN_TEST_START_SCOPE(testing_state, "Make directory recursive \"abcd/efgh\"");
        Dqn_b32 success = Dqn_File_MakeDir(DQN_STRING("abcd/efgh"), &arena);
        DQN_TEST_EXPECT(testing_state, success);
        DQN_TEST_EXPECT(testing_state, Dqn_File_DirExists(DQN_STRING("abcd")));
        DQN_TEST_EXPECT(testing_state, Dqn_File_DirExists(DQN_STRING("abcd/efgh")));
        DQN_TEST_EXPECT_MSG(testing_state, Dqn_File_Exists(DQN_STRING("abcd")) == false, "This function should only return true for files");
        DQN_TEST_EXPECT_MSG(testing_state, Dqn_File_Exists(DQN_STRING("abcd/efgh")) == false, "This function should only return true for files");

        DQN_TEST_EXPECT(testing_state, Dqn_File_Delete(DQN_STRING("abcd/efgh")));
        DQN_TEST_EXPECT_MSG(testing_state, Dqn_File_Delete(DQN_STRING("abcd")), "Failed to cleanup directory");
        Dqn_ArenaAllocator_Free(&arena);
    }

    {
        // NOTE: Write step
        Dqn_String const SRC_FILE = DQN_STRING("dqn_test_file");
        DQN_TEST_START_SCOPE(testing_state, "Write file, read it, copy it, move it and delete it");
        Dqn_b32 write_result = Dqn_File_WriteFile(SRC_FILE.str, "test", 4);
        DQN_TEST_EXPECT(testing_state, write_result);
        DQN_TEST_EXPECT(testing_state, Dqn_File_Exists(SRC_FILE));

        // NOTE: Read step
        Dqn_ArenaAllocator arena = {};
        Dqn_String read_file = Dqn_File_ArenaReadFileToString(SRC_FILE.str, &arena);
        DQN_TEST_EXPECT(testing_state, Dqn_String_IsValid(read_file));
        DQN_TEST_EXPECT(testing_state, read_file.size == 4);
        DQN_TEST_EXPECT_MSG(testing_state, Dqn_String_Eq(read_file, DQN_STRING("test")), "read(%I64u): %.*s", read_file.size, DQN_STRING_FMT(read_file));

        // NOTE: Copy step
        Dqn_String const COPY_FILE = DQN_STRING("dqn_test_file_copy");
        Dqn_b32 copy_result = Dqn_File_Copy(SRC_FILE, COPY_FILE, true /*overwrite*/);
        DQN_TEST_EXPECT(testing_state, copy_result);
        DQN_TEST_EXPECT(testing_state, Dqn_File_Exists(COPY_FILE));

        // NOTE: Move step
        Dqn_String const MOVE_FILE = DQN_STRING("dqn_test_file_move");
        Dqn_b32 move_result = Dqn_File_Move(COPY_FILE, MOVE_FILE, true /*overwrite*/);
        DQN_TEST_EXPECT(testing_state, move_result);
        DQN_TEST_EXPECT(testing_state, Dqn_File_Exists(MOVE_FILE));
        DQN_TEST_EXPECT_MSG(testing_state, Dqn_File_Exists(COPY_FILE) == false, "Moving a file should remove the original");

        // NOTE: Delete step
        Dqn_b32 delete_src_file   = Dqn_File_Delete(SRC_FILE);
        Dqn_b32 delete_moved_file = Dqn_File_Delete(MOVE_FILE);
        DQN_TEST_EXPECT(testing_state, delete_src_file);
        DQN_TEST_EXPECT(testing_state, delete_moved_file);

        // NOTE: Deleting non-existent file fails
        Dqn_b32 delete_non_existent_src_file   = Dqn_File_Delete(SRC_FILE);
        Dqn_b32 delete_non_existent_moved_file = Dqn_File_Delete(MOVE_FILE);
        DQN_TEST_EXPECT(testing_state, delete_non_existent_moved_file == false);
        DQN_TEST_EXPECT(testing_state, delete_non_existent_src_file == false);

        Dqn_ArenaAllocator_Free(&arena);
    }

}

void Dqn_Test_FixedArray()
{
#if defined(DQN_WITH_FIXED_ARRAY)
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_FixedArray");
    // NOTE: Dqn_FixedArray_Init
    {
        DQN_TEST_START_SCOPE(testing_state, "Initialise from raw array");
        int raw_array[] = {1, 2};
        auto array = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
        DQN_TEST_EXPECT(testing_state, array.size == 2);
        DQN_TEST_EXPECT(testing_state, array[0] == 1);
        DQN_TEST_EXPECT(testing_state, array[1] == 2);
    }

    // NOTE: Dqn_FixedArray_EraseStable
    {
        DQN_TEST_START_SCOPE(testing_state, "Erase stable 1 element from array");
        int raw_array[] = {1, 2, 3};
        auto array = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
        Dqn_FixedArray_EraseStable(&array, 1);
        DQN_TEST_EXPECT(testing_state, array.size == 2);
        DQN_TEST_EXPECT(testing_state, array[0] == 1);
        DQN_TEST_EXPECT(testing_state, array[1] == 3);
    }

    // NOTE: Dqn_FixedArray_EraseUnstable
    {
        DQN_TEST_START_SCOPE(testing_state, "Erase unstable 1 element from array");
        int raw_array[] = {1, 2, 3};
        auto array = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
        Dqn_FixedArray_EraseUnstable(&array, 0);
        DQN_TEST_EXPECT(testing_state, array.size == 2);
        DQN_TEST_EXPECT(testing_state, array[0] == 3);
        DQN_TEST_EXPECT(testing_state, array[1] == 2);
    }

    // NOTE: Dqn_FixedArray_Add
    {
        DQN_TEST_START_SCOPE(testing_state, "Add 1 element to array");
        int const ITEM  = 2;
        int raw_array[] = {1};
        auto array      = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
        Dqn_FixedArray_Add(&array, ITEM);
        DQN_TEST_EXPECT(testing_state, array.size == 2);
        DQN_TEST_EXPECT(testing_state, array[0] == 1);
        DQN_TEST_EXPECT(testing_state, array[1] == ITEM);
    }

    // NOTE: Dqn_FixedArray_Clear
    {
        DQN_TEST_START_SCOPE(testing_state, "Clear array");
        int raw_array[] = {1};
        auto array      = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
        Dqn_FixedArray_Clear(&array);
        DQN_TEST_EXPECT(testing_state, array.size == 0);
    }
#endif // DQN_WITH_FIXED_ARRAY
}

void Dqn_Test_FixedString()
{
#if defined(DQN_WITH_FIXED_STRING)
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_FixedString");

    // NOTE: Dqn_FixedString_Append
    {
        DQN_TEST_START_SCOPE(testing_state, "Append too much fails");
        Dqn_FixedString<4> str = {};
        DQN_TEST_EXPECT_MSG(testing_state, Dqn_FixedString_Append(&str, "abcd") == false, "We need space for the null-terminator");
    }

    // NOTE: Dqn_FixedString_AppendFmt
    {
        DQN_TEST_START_SCOPE(testing_state, "Append format string too much fails");
        Dqn_FixedString<4> str = {};
        DQN_TEST_EXPECT_MSG(testing_state, Dqn_FixedString_AppendFmt(&str, "abcd") == false, "We need space for the null-terminator");
    }
#endif // DQN_WITH_FIXED_STRING
}

void Dqn_Test_Hex()
{
#if defined(DQN_WITH_HEX)
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Hex");
    {
        DQN_TEST_START_SCOPE(testing_state, "Convert 0x123");
        Dqn_u64 result = Dqn_Hex_StringToU64(DQN_STRING("0x123"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 0x123, "result: %I64u", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Convert 0xFFFF");
        Dqn_u64 result = Dqn_Hex_StringToU64(DQN_STRING("0xFFFF"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 0xFFFF, "result: %I64u", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Convert FFFF");
        Dqn_u64 result = Dqn_Hex_StringToU64(DQN_STRING("FFFF"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 0xFFFF, "result: %I64u", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Convert abCD");
        Dqn_u64 result = Dqn_Hex_StringToU64(DQN_STRING("abCD"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 0xabCD, "result: %I64u", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Convert 0xabCD");
        Dqn_u64 result = Dqn_Hex_StringToU64(DQN_STRING("0xabCD"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 0xabCD, "result: %I64u", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Convert 0x");
        Dqn_u64 result = Dqn_Hex_StringToU64(DQN_STRING("0x"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 0x0, "result: %I64u", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Convert 0X");
        Dqn_u64 result = Dqn_Hex_StringToU64(DQN_STRING("0X"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 0x0, "result: %I64u", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Convert 3");
        Dqn_u64 result = Dqn_Hex_StringToU64(DQN_STRING("3"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 3, "result: %I64u", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Convert f");
        Dqn_u64 result = Dqn_Hex_StringToU64(DQN_STRING("f"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 0xf, "result: %I64u", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Convert g");
        Dqn_u64 result = Dqn_Hex_StringToU64(DQN_STRING("g"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 0, "result: %I64u", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Convert -0x3");
        Dqn_u64 result = Dqn_Hex_StringToU64(DQN_STRING("-0x3"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 0, "result: %I64u", result);
    }
#endif // DQN_WITH_HEX
}

void Dqn_Test_M4()
{
#if defined(DQN_WITH_MATH)
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_M4");
    {
        DQN_TEST_START_SCOPE(testing_state, "Simple translate and scale matrix");
        Dqn_M4 translate = Dqn_M4_TranslateF(1, 2, 3);
        Dqn_M4 scale     = Dqn_M4_ScaleF(2, 2, 2);
        Dqn_M4 result    = Dqn_M4_Mul(translate, scale);

        const Dqn_M4 EXPECT = {{
            {2, 0, 0, 0},
            {0, 2, 0, 0},
            {0, 0, 2, 0},
            {1, 2, 3, 1},
        }};

        DQN_TEST_EXPECT_MSG(testing_state,
                            memcmp(result.columns, EXPECT.columns, sizeof(EXPECT)) == 0,
                            "\nresult =\n%s\nexpected =\n%s",
                            Dqn_M4_ColumnMajorString(result).str,
                            Dqn_M4_ColumnMajorString(EXPECT).str);
    }
#endif // DQN_WITH_MATH
}

void Dqn_Test_DSMap()
{
#if defined(DQN_WITH_DSMAP)
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_DSMap");
    {
        DQN_TEST_START_SCOPE(testing_state, "Add r-value item to map");
        Dqn_DSMap<int> map         = Dqn_DSMap_Init<int>(128);
        Dqn_DSMapEntry<int> *entry = Dqn_DSMap_AddCopy(&map, 3 /*hash*/, 5 /*value*/);
        DQN_TEST_EXPECT_MSG(testing_state, map.size == 128, "size: %I64d", map.size);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 1, "count: %I64u", map.count);
        DQN_TEST_EXPECT_MSG(testing_state, entry->hash == 3, "hash: %I64u", entry->hash);
        DQN_TEST_EXPECT_MSG(testing_state, entry->value == 5, "value: %d", entry->value);
        Dqn_DSMap_Free(&map);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Add l-value item to map");
        Dqn_DSMap<int>       map   = Dqn_DSMap_Init<int>(128);
        int                  value = 5;
        Dqn_DSMapEntry<int> *entry = Dqn_DSMap_Add(&map, 3 /*hash*/, value);
        DQN_TEST_EXPECT_MSG(testing_state, map.size == 128, "size: %I64d", map.size);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 1, "count: %I64u", map.count);
        DQN_TEST_EXPECT_MSG(testing_state, entry->hash == 3, "hash: %I64u", entry->hash);
        DQN_TEST_EXPECT_MSG(testing_state, entry->value == 5, "value: %d", entry->value);
        Dqn_DSMap_Free(&map);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Get item from map");
        Dqn_DSMap<int> map         = Dqn_DSMap_Init<int>(128);
        Dqn_DSMapEntry<int> *entry = Dqn_DSMap_AddCopy(&map, 3 /*hash*/, 5 /*value*/);
        Dqn_DSMapEntry<int> *get_entry = Dqn_DSMap_Get(&map, 3 /*hash*/);
        DQN_TEST_EXPECT_MSG(testing_state, get_entry == entry, "get_entry: %p, entry: %p", get_entry, entry);
        Dqn_DSMap_Free(&map);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Get non-existent item from map");
        Dqn_DSMap<int> map         = Dqn_DSMap_Init<int>(128);
        Dqn_DSMapEntry<int> *entry = Dqn_DSMap_Get(&map, 3 /*hash*/);
        DQN_TEST_EXPECT(testing_state, entry == nullptr);
        Dqn_DSMap_Free(&map);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Erase item from map");
        Dqn_DSMap<int> map = Dqn_DSMap_Init<int>(128);
        Dqn_DSMap_AddCopy(&map, 3 /*hash*/, 5 /*value*/);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 1, "count: %I64d", map.count);
        Dqn_DSMap_Erase(&map, 3 /*hash*/, Dqn_ZeroMem::No);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 0, "count: %I64d", map.count);
        Dqn_DSMap_Free(&map);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Erase non-existent item from map");
        Dqn_DSMap<int> map = Dqn_DSMap_Init<int>(128);
        Dqn_DSMap_Erase(&map, 3 /*hash*/, Dqn_ZeroMem::No);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 0, "count: %I64d", map.count);
        Dqn_DSMap_Free(&map);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Test resize on maximum load");
        const Dqn_isize INIT_SIZE = 4;
        Dqn_DSMap<int> map = Dqn_DSMap_Init<int>(INIT_SIZE);
        Dqn_DSMap_AddCopy(&map, 0 /*hash*/, 5 /*value*/);
        Dqn_DSMap_AddCopy(&map, 1 /*hash*/, 5 /*value*/);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 2, "count: %I64d", map.count);

        // This *should* cause a resize because 3/4 slots filled is 75% load
        Dqn_DSMap_AddCopy(&map, 6 /*hash*/, 5 /*value*/);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 3, "count: %I64d", map.count);
        DQN_TEST_EXPECT_MSG(testing_state, map.size == INIT_SIZE * 2, "size: %I64d", map.size);

        // Check that the elements are rehashed where we expected them to be
        DQN_TEST_EXPECT    (testing_state, map.slots[0].occupied == DQN_CAST(Dqn_u8)true);
        DQN_TEST_EXPECT    (testing_state, map.slots[1].occupied == DQN_CAST(Dqn_u8)true);
        DQN_TEST_EXPECT    (testing_state, map.slots[2].occupied == DQN_CAST(Dqn_u8)false);
        DQN_TEST_EXPECT    (testing_state, map.slots[3].occupied == DQN_CAST(Dqn_u8)false);
        DQN_TEST_EXPECT    (testing_state, map.slots[4].occupied == DQN_CAST(Dqn_u8)false);
        DQN_TEST_EXPECT    (testing_state, map.slots[5].occupied == DQN_CAST(Dqn_u8)false);
        DQN_TEST_EXPECT    (testing_state, map.slots[6].occupied == DQN_CAST(Dqn_u8)true);
        DQN_TEST_EXPECT    (testing_state, map.slots[7].occupied == DQN_CAST(Dqn_u8)false);

        DQN_TEST_EXPECT_MSG(testing_state, map.slots[0].value == 5, "value: %d", map.slots[0].value);
        DQN_TEST_EXPECT_MSG(testing_state, map.slots[1].value == 5, "value: %d", map.slots[1].value);
        DQN_TEST_EXPECT_MSG(testing_state, map.slots[6].value == 5, "value: %d", map.slots[6].value);

        Dqn_DSMap_Free(&map);
    }
#endif // DQN_WITH_DSMAP
}

void Dqn_Test_Map()
{
#if defined(DQN_WITH_MAP)
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Map");
    Dqn_ArenaAllocator arena = {};
    {
        DQN_TEST_START_SCOPE(testing_state, "Add r-value item to map");
        Dqn_Map<int> map         = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_MapEntry<int> *entry = Dqn_Map_AddCopy(&map, 3 /*hash*/, 5 /*value*/, Dqn_MapCollideRule::Overwrite);
        DQN_TEST_EXPECT_MSG(testing_state, map.size == 1, "size: %I64d", map.size);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 1, "count: %I64u", map.count);
        DQN_TEST_EXPECT_MSG(testing_state, map.chain_count == 0, "chain_count: %I64u", map.chain_count);
        DQN_TEST_EXPECT_MSG(testing_state, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TEST_EXPECT_MSG(testing_state, entry->hash == 3, "hash: %I64u", entry->hash);
        DQN_TEST_EXPECT_MSG(testing_state, entry->value == 5, "value: %d", entry->value);
        DQN_TEST_EXPECT_MSG(testing_state, entry->next == nullptr, "next: %p", entry->next);
        Dqn_ArenaAllocator_Free(&arena);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Add l-value item to map");
        Dqn_Map<int>       map   = Dqn_Map_InitWithArena<int>(&arena, 1);
        int                value = 5;
        Dqn_MapEntry<int> *entry = Dqn_Map_Add(&map, 3 /*hash*/, value, Dqn_MapCollideRule::Overwrite);
        DQN_TEST_EXPECT_MSG(testing_state, map.size == 1, "size: %I64d", map.size);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 1, "count: %I64u", map.count);
        DQN_TEST_EXPECT_MSG(testing_state, map.chain_count == 0, "chain_count: %I64u", map.chain_count);
        DQN_TEST_EXPECT_MSG(testing_state, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TEST_EXPECT_MSG(testing_state, entry->hash == 3, "hash: %I64u", entry->hash);
        DQN_TEST_EXPECT_MSG(testing_state, entry->value == 5, "value: %d", entry->value);
        DQN_TEST_EXPECT_MSG(testing_state, entry->next == nullptr, "next: %p", entry->next);
        Dqn_ArenaAllocator_Free(&arena);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Add r-value item and overwrite on collision");
        Dqn_Map<int>       map   = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_MapEntry<int> *entry_a = Dqn_Map_AddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapEntry<int> *entry_b = Dqn_Map_AddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Overwrite);
        DQN_TEST_EXPECT_MSG(testing_state, map.size == 1, "size: %I64u", map.size);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 1, "count: %I64u", map.count);
        DQN_TEST_EXPECT_MSG(testing_state, map.chain_count == 0, "chain_count: %I64u", map.chain_count);
        DQN_TEST_EXPECT_MSG(testing_state, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TEST_EXPECT_MSG(testing_state, entry_a == entry_b, "Expected entry to be overwritten");
        DQN_TEST_EXPECT_MSG(testing_state, entry_b->hash == 4, "hash: %I64u", entry_b->hash);
        DQN_TEST_EXPECT_MSG(testing_state, entry_b->value == 6, "value: %d", entry_b->value);
        DQN_TEST_EXPECT_MSG(testing_state, entry_b->next == nullptr, "next: %p", entry_b->next);
        Dqn_ArenaAllocator_Free(&arena);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Add r-value item and fail on collision");
        Dqn_Map<int> map = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_Map_AddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapEntry<int> *entry_b = Dqn_Map_AddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Fail);
        DQN_TEST_EXPECT_MSG(testing_state, entry_b == nullptr, "Expected entry to be overwritten");
        DQN_TEST_EXPECT_MSG(testing_state, map.size == 1, "size: %I64u", map.size);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 1, "count: %I64u", map.count);
        DQN_TEST_EXPECT_MSG(testing_state, map.chain_count == 0, "chain_count: %I64u", map.chain_count);
        DQN_TEST_EXPECT_MSG(testing_state, map.free_list == nullptr, "free_list: %p", map.free_list);
        Dqn_ArenaAllocator_Free(&arena);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Add r-value item and chain on collision");
        Dqn_Map<int>       map   = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_MapEntry<int> *entry_a = Dqn_Map_AddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapEntry<int> *entry_b = Dqn_Map_AddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Chain);
        DQN_TEST_EXPECT_MSG(testing_state, map.size == 1, "size: %I64u", map.size);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 1, "count: %I64u", map.count);
        DQN_TEST_EXPECT_MSG(testing_state, map.chain_count == 1, "chain_count: %I64u", map.chain_count);
        DQN_TEST_EXPECT_MSG(testing_state, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TEST_EXPECT_MSG(testing_state, entry_a != entry_b, "Expected colliding entry to be chained");
        DQN_TEST_EXPECT_MSG(testing_state, entry_a->next == entry_b, "Expected chained entry to be next to our first map entry");
        DQN_TEST_EXPECT_MSG(testing_state, entry_b->hash == 4, "hash: %I64u", entry_b->hash);
        DQN_TEST_EXPECT_MSG(testing_state, entry_b->value == 6, "value: %d", entry_b->value);
        DQN_TEST_EXPECT_MSG(testing_state, entry_b->next == nullptr, "next: %p", entry_b->next);
        Dqn_ArenaAllocator_Free(&arena);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Add r-value item and get them back out again");
        Dqn_Map<int>       map   = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_MapEntry<int> *entry_a = Dqn_Map_AddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapEntry<int> *entry_b = Dqn_Map_AddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Chain);

        Dqn_MapEntry<int> *entry_a_copy = Dqn_Map_Get(&map, 3 /*hash*/);
        Dqn_MapEntry<int> *entry_b_copy = Dqn_Map_Get(&map, 4 /*hash*/);
        DQN_TEST_EXPECT_MSG(testing_state, map.size == 1, "size: %I64u", map.size);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 1, "count: %I64u", map.count);
        DQN_TEST_EXPECT_MSG(testing_state, map.chain_count == 1, "chain_count: %I64u", map.chain_count);
        DQN_TEST_EXPECT_MSG(testing_state, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TEST_EXPECT(testing_state, entry_a_copy == entry_a);
        DQN_TEST_EXPECT(testing_state, entry_b_copy == entry_b);
        Dqn_ArenaAllocator_Free(&arena);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Add r-value item and erase it");
        Dqn_Map<int>       map   = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_Map_AddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_Map_AddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Chain);
        Dqn_Map_Get(&map, 3 /*hash*/);

        Dqn_Map_Erase(&map, 3 /*hash*/, Dqn_ZeroMem::No);
        DQN_TEST_EXPECT_MSG(testing_state, map.size == 1, "size: %I64u", map.size);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 1, "count: %I64u", map.count);
        DQN_TEST_EXPECT_MSG(testing_state, map.chain_count == 0, "chain_count: %I64u", map.chain_count);
        DQN_TEST_EXPECT_MSG(testing_state, map.free_list != nullptr, "free_list: %p", map.free_list);

        DQN_TEST_EXPECT_MSG(testing_state, map.free_list->hash == 3, "Entry should not be zeroed out on erase");
        DQN_TEST_EXPECT_MSG(testing_state, map.free_list->value == 5, "Entry should not be zeroed out on erase");
        DQN_TEST_EXPECT_MSG(testing_state, map.free_list->next == nullptr, "This should be the first and only entry in the free list");

        Dqn_MapEntry<int> *entry = Dqn_Map_Get(&map, 4 /*hash*/);
        DQN_TEST_EXPECT_MSG(testing_state, entry->hash == 4, "hash: %I64u", entry->hash);
        DQN_TEST_EXPECT_MSG(testing_state, entry->value == 6, "value: %d", entry->value);
        DQN_TEST_EXPECT_MSG(testing_state, entry->next == nullptr, "next: %p", entry->next);

        Dqn_ArenaAllocator_Free(&arena);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Add r-value item and erase it, zeroing the memory out");
        Dqn_Map<int>       map   = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_Map_AddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_Map_AddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Chain);
        Dqn_Map_Get(&map, 3 /*hash*/);

        Dqn_Map_Erase(&map, 3 /*hash*/, Dqn_ZeroMem::Yes);
        DQN_TEST_EXPECT_MSG(testing_state, map.size == 1, "size: %I64u", map.size);
        DQN_TEST_EXPECT_MSG(testing_state, map.count == 1, "count: %I64u", map.count);
        DQN_TEST_EXPECT_MSG(testing_state, map.chain_count == 0, "chain_count: %I64u", map.chain_count);
        DQN_TEST_EXPECT_MSG(testing_state, map.free_list != nullptr, "free_list: %p", map.free_list);

        DQN_TEST_EXPECT_MSG(testing_state, map.free_list->hash == 0, "Entry should be zeroed out on erase");
        DQN_TEST_EXPECT_MSG(testing_state, map.free_list->value == 0, "Entry should be zeroed out on erase");
        DQN_TEST_EXPECT_MSG(testing_state, map.free_list->next == nullptr, "This should be the first and only entry in the free list");

        Dqn_MapEntry<int> *entry = Dqn_Map_Get(&map, 4 /*hash*/);
        DQN_TEST_EXPECT_MSG(testing_state, entry->hash == 4, "hash: %I64u", entry->hash);
        DQN_TEST_EXPECT_MSG(testing_state, entry->value == 6, "value: %d", entry->value);
        DQN_TEST_EXPECT_MSG(testing_state, entry->next == nullptr, "next: %p", entry->next);

        Dqn_ArenaAllocator_Free(&arena);
    }

    // TODO(dqn): Test free list is chained correctly
    // TODO(dqn): Test deleting 'b' from the list in the situation [map] - [a]->[b], we currently only test deleting a
#endif // DQN_WITH_MAP
}

void Dqn_Test_Intrinsics()
{
    // TODO(dqn): We don't have meaningful tests here, but since
    // atomics/intrinsics are implemented using macros we ensure the macro was
    // written properly with these tests.

    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Atomic");
    {
        DQN_TEST_START_SCOPE(testing_state, "Dqn_AtomicAddU32");
        Dqn_u32 val = 0;
        Dqn_AtomicAddU32(&val, 1);
        DQN_TEST_EXPECT_MSG(testing_state, val == 1, "val: %I64u", val);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Dqn_AtomicAddU64");
        Dqn_u64 val = 0;
        Dqn_AtomicAddU64(&val, 1);
        DQN_TEST_EXPECT_MSG(testing_state, val == 1, "val: %I64u", val);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Dqn_AtomicSubU32");
        Dqn_u32 val = 1;
        Dqn_AtomicSubU32(&val, 1);
        DQN_TEST_EXPECT_MSG(testing_state, val == 0, "val: %I64u", val);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Dqn_AtomicSubU64");
        Dqn_u64 val = 1;
        Dqn_AtomicSubU64(&val, 1);
        DQN_TEST_EXPECT_MSG(testing_state, val == 0, "val: %I64u", val);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Dqn_AtomicSetValue32");
        long a = 0;
        long b = 111;
        Dqn_AtomicSetValue32(&a, b);
        DQN_TEST_EXPECT_MSG(testing_state, a == b, "a: %I64d, b: %I64d", a, b);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Dqn_AtomicSetValue64");
        Dqn_i64 a = 0;
        Dqn_i64 b = 111;
        Dqn_AtomicSetValue64(&a, b);
        DQN_TEST_EXPECT_MSG(testing_state, a == b, "a: %I64i, b: %I64i", a, b);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Dqn_CPUClockCycle");
        Dqn_CPUClockCycle();
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Dqn_CompilerReadBarrierAndCPUReadFence");
        Dqn_CompilerReadBarrierAndCPUReadFence;
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Dqn_CompilerWriteBarrierAndCPUWriteFence");
        Dqn_CompilerWriteBarrierAndCPUWriteFence;
    }
}

void Dqn_Test_Rect()
{
#if defined(DQN_WITH_MATH)
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Rect");
    // NOTE: Dqn_Rect_Intersection
    {
        {
            DQN_TEST_START_SCOPE(testing_state, "No intersection");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(0, 0), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(200, 0), Dqn_V2(200, 200));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TEST_EXPECT_MSG(testing_state,
                            ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 0 && ab.max.y == 0,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "A's min intersects B");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(50, 50), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TEST_EXPECT_MSG(testing_state,
                            ab.min.x == 50 && ab.min.y == 50 && ab.max.x == 100 && ab.max.y == 100,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "B's min intersects A");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(50, 50), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TEST_EXPECT_MSG(testing_state,
                            ab.min.x == 50 && ab.min.y == 50 && ab.max.x == 100 && ab.max.y == 100,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "A's max intersects B");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(-50, -50), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(  0,   0), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TEST_EXPECT_MSG(testing_state,
                            ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 50 && ab.max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "B's max intersects A");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(  0,   0), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(-50, -50), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TEST_EXPECT_MSG(testing_state,
                            ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 50 && ab.max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }


        {
            DQN_TEST_START_SCOPE(testing_state, "B contains A");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(25, 25), Dqn_V2( 25,  25));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TEST_EXPECT_MSG(testing_state,
                            ab.min.x == 25 && ab.min.y == 25 && ab.max.x == 50 && ab.max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "A contains B");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(25, 25), Dqn_V2( 25,  25));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TEST_EXPECT_MSG(testing_state,
                            ab.min.x == 25 && ab.min.y == 25 && ab.max.x == 50 && ab.max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "A equals B");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(0, 0), Dqn_V2(100, 100));
            Dqn_Rect b  = a;
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TEST_EXPECT_MSG(testing_state,
                            ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 100 && ab.max.y == 100,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }
    }
#endif // DQN_WITH_MATH
}

void Dqn_Test_PerfCounter()
{
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_PerfCounter");
    {
        DQN_TEST_START_SCOPE(testing_state, "Dqn_PerfCounter_Now");
        Dqn_u64 result = Dqn_PerfCounter_Now();
        DQN_TEST_EXPECT(testing_state, result != 0);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Consecutive ticks are ordered");
        Dqn_u64 a = Dqn_PerfCounter_Now();
        Dqn_u64 b = Dqn_PerfCounter_Now();
        DQN_TEST_EXPECT_MSG(testing_state, b >= a, "a: %I64u, b: %I64u", a, b);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Ticks to time are a correct order of magnitude");
        Dqn_u64 a = Dqn_PerfCounter_Now();
        Dqn_u64 b = Dqn_PerfCounter_Now();

        Dqn_f64 s       = Dqn_PerfCounter_S(a, b);
        Dqn_f64 ms      = Dqn_PerfCounter_Ms(a, b);
        Dqn_f64 micro_s = Dqn_PerfCounter_MicroS(a, b);
        Dqn_f64 ns      = Dqn_PerfCounter_Ns(a, b);
        DQN_TEST_EXPECT_MSG(testing_state, s <= ms, "s: %I64u, ms: %I64u", s, ms);
        DQN_TEST_EXPECT_MSG(testing_state, ms <= micro_s, "ms: %I64u, micro_s: %I64u", ms, micro_s);
        DQN_TEST_EXPECT_MSG(testing_state, micro_s <= ns, "micro_s: %I64u, ns: %I64u", micro_s, ns);
    }
}

void Dqn_Test_OS()
{
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_OS");
    {
        DQN_TEST_START_SCOPE(testing_state, "Generate secure RNG bytes with nullptr");
        Dqn_b32 result = Dqn_OS_SecureRNGBytes(nullptr, 1);
        DQN_TEST_EXPECT(testing_state, result == false);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Generate secure RNG bytes with -1 size");
        char buf[1];
        Dqn_b32 result = Dqn_OS_SecureRNGBytes(buf, -1);
        DQN_TEST_EXPECT(testing_state, result == false);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Generate secure RNG 32 bytes");
        char const ZERO[32] = {};
        char       buf[32]  = {};
        Dqn_b32 result = Dqn_OS_SecureRNGBytes(buf, Dqn_ArrayCountI(buf));
        DQN_TEST_EXPECT(testing_state, result);
        DQN_TEST_EXPECT(testing_state, DQN_MEMCMP(buf, ZERO, Dqn_ArrayCount(buf)) != 0);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Generate secure RNG 0 bytes");
        char buf[32] = {};
        buf[0] = 'Z';
        Dqn_b32 result = Dqn_OS_SecureRNGBytes(buf, 0);
        DQN_TEST_EXPECT(testing_state, result);
        DQN_TEST_EXPECT(testing_state, buf[0] == 'Z');
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Query executable directory");
        Dqn_ArenaAllocator arena = {};
        Dqn_String result = Dqn_OS_ExecutableDirectory(&arena);
        DQN_TEST_EXPECT(testing_state, Dqn_String_IsValid(result));
        DQN_TEST_EXPECT_MSG(testing_state, Dqn_File_DirExists(result), "result(%I64u): %.*s", result.size, DQN_STRING_FMT(result));
        Dqn_ArenaAllocator_Free(&arena);
    }
}

void Dqn_Test_Str()
{
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Str");
    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_Str_ToI64
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST_START_SCOPE(testing_state, "To I64: Convert nullptr");
        Dqn_i64 result = Dqn_Str_ToI64(nullptr);
        DQN_TEST_EXPECT(testing_state, result == 0);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To I64: Convert empty string");
        Dqn_i64 result = Dqn_Str_ToI64("");
        DQN_TEST_EXPECT(testing_state, result == 0);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To I64: Convert \"1\"");
        Dqn_i64 result = Dqn_Str_ToI64("1");
        DQN_TEST_EXPECT(testing_state, result == 1);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To I64: Convert \"-0\"");
        Dqn_i64 result = Dqn_Str_ToI64("-0");
        DQN_TEST_EXPECT(testing_state, result == 0);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To I64: Convert \"-1\"");
        Dqn_i64 result = Dqn_Str_ToI64("-1");
        DQN_TEST_EXPECT(testing_state, result == -1);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To I64: Convert \"1.2\"");
        Dqn_i64 result = Dqn_Str_ToI64("1.2");
        DQN_TEST_EXPECT(testing_state, result == 1);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To I64: Convert \"1,234\"");
        Dqn_i64 result = Dqn_Str_ToI64("1,234");
        DQN_TEST_EXPECT(testing_state, result == 1234);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To I64: Convert \"1,2\"");
        Dqn_i64 result = Dqn_Str_ToI64("1,2");
        DQN_TEST_EXPECT(testing_state, result == 12);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To I64: Convert \"12a3\"");
        Dqn_i64 result = Dqn_Str_ToI64("12a3");
        DQN_TEST_EXPECT(testing_state, result == 12);
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_Str_ToU64
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST_START_SCOPE(testing_state, "To U64: Convert nullptr");
        Dqn_u64 result = Dqn_Str_ToU64(nullptr);
        DQN_TEST_EXPECT_MSG(testing_state, result == 0, "result: %zu", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To U64: Convert empty string");
        Dqn_u64 result = Dqn_Str_ToU64("");
        DQN_TEST_EXPECT_MSG(testing_state, result == 0, "result: %zu", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To U64: Convert \"1\"");
        Dqn_u64 result = Dqn_Str_ToU64("1");
        DQN_TEST_EXPECT_MSG(testing_state, result == 1, "result: %zu", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To U64: Convert \"-0\"");
        Dqn_u64 result = Dqn_Str_ToU64("-0");
        DQN_TEST_EXPECT_MSG(testing_state, result == 0, "result: %zu", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To U64: Convert \"-1\"");
        Dqn_u64 result = Dqn_Str_ToU64("-1");
        DQN_TEST_EXPECT_MSG(testing_state, result == 0, "result: %zu", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To U64: Convert \"1.2\"");
        Dqn_u64 result = Dqn_Str_ToU64("1.2");
        DQN_TEST_EXPECT_MSG(testing_state, result == 1, "result: %zu", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To U64: Convert \"1,234\"");
        Dqn_u64 result = Dqn_Str_ToU64("1,234");
        DQN_TEST_EXPECT_MSG(testing_state, result == 1234, "result: %zu", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To U64: Convert \"1,2\"");
        Dqn_u64 result = Dqn_Str_ToU64("1,2");
        DQN_TEST_EXPECT_MSG(testing_state, result == 12, "result: %zu", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "To U64: Convert \"12a3\"");
        Dqn_u64 result = Dqn_Str_ToU64("12a3");
        DQN_TEST_EXPECT_MSG(testing_state, result == 12, "result: %zu", result);
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_Str_Find
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST_START_SCOPE(testing_state, "Find: String (char) is not in buffer");
        char const buf[]  = "836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55";
        char const find[] = "2";
        char const *result = Dqn_Str_Find(buf, find, Dqn_CharCountI(buf), Dqn_CharCountI(find));
        DQN_TEST_EXPECT(testing_state, result == nullptr);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Find: String (char) is in buffer");
        char const buf[]  = "836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55";
        char const find[] = "6";
        char const *result = Dqn_Str_Find(buf, find, Dqn_CharCountI(buf), Dqn_CharCountI(find));
        DQN_TEST_EXPECT(testing_state, result != nullptr);
        DQN_TEST_EXPECT(testing_state, result[0] == '6' && result[1] == 'a');
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_Str_FileNameFromPath
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST_START_SCOPE(testing_state, "File name from Windows path");
        Dqn_isize file_name_size = 0;
        char const buf[]  = "C:\\ABC\\test.exe";
        char const *result = Dqn_Str_FileNameFromPath(buf, Dqn_CharCountI(buf), &file_name_size);
        DQN_TEST_EXPECT_MSG(testing_state, file_name_size == 8, "size: %I64d", file_name_size);
        DQN_TEST_EXPECT_MSG(testing_state, Dqn_String_Init(result, file_name_size) == DQN_STRING("test.exe"), "%.*s", file_name_size, result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "File name from Linux path");
        Dqn_isize file_name_size = 0;
        char const buf[]  = "/ABC/test.exe";
        char const *result = Dqn_Str_FileNameFromPath(buf, Dqn_CharCountI(buf), &file_name_size);
        DQN_TEST_EXPECT_MSG(testing_state, file_name_size == 8, "size: %I64d", file_name_size);
        DQN_TEST_EXPECT_MSG(testing_state, Dqn_String_Init(result, file_name_size) == DQN_STRING("test.exe"), "%.*s", file_name_size, result);
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_Str_TrimPrefix
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST_START_SCOPE(testing_state, "Trim prefix");
        char const  prefix[]     = "@123";
        char const  buf[]        = "@123string";
        Dqn_isize   trimmed_size = 0;
        char const *result = Dqn_Str_TrimPrefix(buf, Dqn_CharCountI(buf), prefix, Dqn_CharCountI(prefix), &trimmed_size);
        DQN_TEST_EXPECT_MSG(testing_state, trimmed_size == 6, "size: %I64d", trimmed_size);
        DQN_TEST_EXPECT_MSG(testing_state, Dqn_String_Init(result, trimmed_size) == DQN_STRING("string"), "%.*s", trimmed_size, result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Trim prefix, nullptr trimmed size");
        char const  prefix[]     = "@123";
        char const  buf[]        = "@123string";
        char const *result = Dqn_Str_TrimPrefix(buf, Dqn_CharCountI(buf), prefix, Dqn_CharCountI(prefix), nullptr);
        DQN_TEST_EXPECT(testing_state, result);
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_Str_IsAllDigits
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST_START_SCOPE(testing_state, "Is all digits fails on non-digit string");
        char const buf[]  = "@123string";
        Dqn_b32    result = Dqn_Str_IsAllDigits(buf, Dqn_CharCountI(buf));
        DQN_TEST_EXPECT(testing_state, result == false);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Is all digits fails on nullptr");
        Dqn_b32 result = Dqn_Str_IsAllDigits(nullptr, 0);
        DQN_TEST_EXPECT(testing_state, result == false);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Is all digits fails on nullptr w/ size");
        Dqn_b32 result = Dqn_Str_IsAllDigits(nullptr, 1);
        DQN_TEST_EXPECT(testing_state, result == false);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Is all digits fails on 0 size w/ string");
        char const buf[]  = "@123string";
        Dqn_b32    result = Dqn_Str_IsAllDigits(buf, 0);
        DQN_TEST_EXPECT(testing_state, result == false);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Is all digits success");
        char const buf[]  = "23";
        Dqn_b32    result = Dqn_Str_IsAllDigits(buf, Dqn_CharCountI(buf));
        DQN_TEST_EXPECT(testing_state, DQN_CAST(bool)result == true);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Is all digits fails on whitespace");
        char const buf[]  = "23 ";
        Dqn_b32    result = Dqn_Str_IsAllDigits(buf, Dqn_CharCountI(buf));
        DQN_TEST_EXPECT(testing_state, DQN_CAST(bool)result == false);
    }
}

void Dqn_Test_String()
{
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_String");
    {
        DQN_TEST_START_SCOPE(testing_state, "Initialise with string literal w/ macro");
        Dqn_String string = DQN_STRING("AB");
        DQN_TEST_EXPECT_MSG(testing_state, string.size == 2, "size: %I64d", string.size);
        DQN_TEST_EXPECT_MSG(testing_state, string.cap  == 2, "cap: %I64d", string.cap);
        DQN_TEST_EXPECT_MSG(testing_state, string.str[0] == 'A', "string[0]: %c", string.str[0]);
        DQN_TEST_EXPECT_MSG(testing_state, string.str[1] == 'B', "string[1]: %c", string.str[1]);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Initialise with format string");
        Dqn_ArenaAllocator arena  = {};
        Dqn_String         string = Dqn_String_Fmt(&arena, "%s", "AB");
        DQN_TEST_EXPECT_MSG(testing_state, string.size == 2, "size: %I64d", string.size);
        DQN_TEST_EXPECT_MSG(testing_state, string.cap == 2, "cap: %I64d", string.cap);
        DQN_TEST_EXPECT_MSG(testing_state, string.str[0] == 'A', "string[0]: %c", string.str[0]);
        DQN_TEST_EXPECT_MSG(testing_state, string.str[1] == 'B', "string[1]: %c", string.str[1]);
        DQN_TEST_EXPECT_MSG(testing_state, string.str[2] == 0, "string[2]: %c", string.str[2]);
        Dqn_ArenaAllocator_Free(&arena);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Copy string");
        Dqn_ArenaAllocator arena  = {};
        Dqn_String         string = DQN_STRING("AB");
        Dqn_String         copy   = Dqn_String_Copy(string, &arena);
        DQN_TEST_EXPECT_MSG(testing_state, copy.size == 2, "size: %I64d", copy.size);
        DQN_TEST_EXPECT_MSG(testing_state, copy.cap  == 2, "cap: %I64d", copy.cap);
        DQN_TEST_EXPECT_MSG(testing_state, copy.str[0] == 'A', "copy[0]: %c", copy.str[0]);
        DQN_TEST_EXPECT_MSG(testing_state, copy.str[1] == 'B', "copy[1]: %c", copy.str[1]);
        DQN_TEST_EXPECT_MSG(testing_state, copy.str[2] ==  0,  "copy[2]: %c", copy.str[2]);
        Dqn_ArenaAllocator_Free(&arena);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Trim whitespace around string");
        Dqn_String         string = Dqn_String_TrimWhitespaceAround(DQN_STRING(" AB "));
        DQN_TEST_EXPECT_MSG(testing_state, string.size == 2, "size: %I64d", string.size);
        DQN_TEST_EXPECT_MSG(testing_state, string.cap  == 2, "cap: %I64d", string.cap);
        DQN_TEST_EXPECT_MSG(testing_state, string.str[0] == 'A', "string[0]: %c", string.str[0]);
        DQN_TEST_EXPECT_MSG(testing_state, string.str[1] == 'B', "string[1]: %c", string.str[1]);
        DQN_TEST_EXPECT_MSG(testing_state, string.str[2] == ' ', "string[1]: %c", string.str[1]);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Allocate string from arena");
        Dqn_ArenaAllocator arena  = {};
        Dqn_String         string = Dqn_String_Allocate(&arena, 2, Dqn_ZeroMem::No);
        DQN_TEST_EXPECT_MSG(testing_state, string.size == 0, "size: %I64d", string.size);
        DQN_TEST_EXPECT_MSG(testing_state, string.cap  == 2, "cap: %I64d", string.cap);
        Dqn_ArenaAllocator_Free(&arena);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Append to allocated string");
        Dqn_ArenaAllocator arena  = {};
        Dqn_String         string = Dqn_String_Allocate(&arena, 2, Dqn_ZeroMem::No);
        Dqn_String_AppendFmt(&string, "%c", 'A');
        Dqn_String_AppendFmt(&string, "%c", 'B');
        DQN_TEST_EXPECT_MSG(testing_state, string.size   ==  2,  "size: %I64d", string.size);
        DQN_TEST_EXPECT_MSG(testing_state, string.cap    ==  2,  "cap: %I64d", string.cap);
        DQN_TEST_EXPECT_MSG(testing_state, string.str[0] == 'A', "string[0]: %c", string.str[0]);
        DQN_TEST_EXPECT_MSG(testing_state, string.str[1] == 'B', "string[1]: %c", string.str[1]);
        DQN_TEST_EXPECT_MSG(testing_state, string.str[2] ==  0,  "string[2]: %c", string.str[2]);
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_Str_Trim[Prefix/Suffix]
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST_START_SCOPE(testing_state, "Trim prefix with matching prefix");
        Dqn_String input  = DQN_STRING("nft/abc");
        Dqn_String result = Dqn_String_TrimPrefix(input, DQN_STRING("nft/"));
        DQN_TEST_EXPECT_MSG(testing_state, result == DQN_STRING("abc"), "%.*s", DQN_STRING_FMT(result));
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Trim prefix with non matching prefix");
        Dqn_String input  = DQN_STRING("nft/abc");
        Dqn_String result = Dqn_String_TrimPrefix(input, DQN_STRING(" ft/"));
        DQN_TEST_EXPECT_MSG(testing_state, result == input, "%.*s", DQN_STRING_FMT(result));
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Trim suffix with matching suffix");
        Dqn_String input  = DQN_STRING("nft/abc");
        Dqn_String result = Dqn_String_TrimSuffix(input, DQN_STRING("abc"));
        DQN_TEST_EXPECT_MSG(testing_state, result == DQN_STRING("nft/"), "%.*s", DQN_STRING_FMT(result));
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Trim suffix with non matching suffix");
        Dqn_String input  = DQN_STRING("nft/abc");
        Dqn_String result = Dqn_String_TrimSuffix(input, DQN_STRING("ab"));
        DQN_TEST_EXPECT_MSG(testing_state, result == input, "%.*s", DQN_STRING_FMT(result));
    }


    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_String_IsAllDigits
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST_START_SCOPE(testing_state, "Is all digits fails on non-digit string");
        Dqn_b32 result = Dqn_String_IsAllDigits(DQN_STRING("@123string"));
        DQN_TEST_EXPECT(testing_state, result == false);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Is all digits fails on nullptr");
        Dqn_b32 result = Dqn_String_IsAllDigits(Dqn_String_Init(nullptr, 0));
        DQN_TEST_EXPECT(testing_state, result == false);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Is all digits fails on nullptr w/ size");
        Dqn_b32 result = Dqn_String_IsAllDigits(Dqn_String_Init(nullptr, 1));
        DQN_TEST_EXPECT(testing_state, result == false);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Is all digits fails on string w/ 0 size");
        char const buf[]  = "@123string";
        Dqn_b32    result = Dqn_String_IsAllDigits(Dqn_String_Init(buf, 0));
        DQN_TEST_EXPECT(testing_state, result == false);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Is all digits success");
        Dqn_b32 result = Dqn_String_IsAllDigits(DQN_STRING("23"));
        DQN_TEST_EXPECT(testing_state, DQN_CAST(bool)result == true);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Is all digits fails on whitespace");
        Dqn_b32 result = Dqn_String_IsAllDigits(DQN_STRING("23 "));
        DQN_TEST_EXPECT(testing_state, DQN_CAST(bool)result == false);
    }
}

void Dqn_Test_TicketMutex()
{
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_TicketMutex");
    {
        // TODO: We don't have a meaningful test but since atomics are
        // implemented with a macro this ensures that we test that they are
        // written correctly.
        DQN_TEST_START_SCOPE(testing_state, "Ticket mutex start and stop");
        Dqn_TicketMutex mutex = {};
        Dqn_TicketMutex_Begin(&mutex);
        Dqn_TicketMutex_End(&mutex);
        DQN_TEST_EXPECT(testing_state, mutex.ticket == mutex.serving);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Ticket mutex start and stop w/ advanced API");
        Dqn_TicketMutex mutex = {};
        unsigned int ticket_a = Dqn_TicketMutex_MakeTicket(&mutex);
        unsigned int ticket_b = Dqn_TicketMutex_MakeTicket(&mutex);
        DQN_TEST_EXPECT(testing_state, DQN_CAST(bool)Dqn_TicketMutex_CanLock(&mutex, ticket_b) == false);
        DQN_TEST_EXPECT(testing_state, DQN_CAST(bool)Dqn_TicketMutex_CanLock(&mutex, ticket_a) == true);

        Dqn_TicketMutex_BeginTicket(&mutex, ticket_a);
        Dqn_TicketMutex_End(&mutex);
        Dqn_TicketMutex_BeginTicket(&mutex, ticket_b);
        Dqn_TicketMutex_End(&mutex);

        DQN_TEST_EXPECT(testing_state, mutex.ticket == mutex.serving);
        DQN_TEST_EXPECT(testing_state, mutex.ticket == ticket_b + 1);
    }
}

void Dqn_Test_Win()
{
#if defined(DQN_OS_WIN32)
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Win");
    {
        DQN_TEST_START_SCOPE(testing_state, "UTF8 to wide character size calculation");
        int result = Dqn_Win_UTF8ToWCharSizeRequired(DQN_STRING("String"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 6, "Size returned: %d. This size should be including the null-terminator", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "UTF8 to wide character");
        Dqn_ArenaAllocator arena = {};
        Dqn_String const INPUT   = DQN_STRING("String");
        int size_required        = Dqn_Win_UTF8ToWCharSizeRequired(INPUT);
        wchar_t *string          = Dqn_ArenaAllocator_NewArray(&arena, wchar_t, size_required + 1, Dqn_ZeroMem::No);

        // Fill the string with error sentinels, which ensures the string is zero terminated
        DQN_MEMSET(string, 'Z', size_required + 1);

        int size_returned = Dqn_Win_UTF8ToWChar(INPUT, string, size_required + 1);
        wchar_t const EXPECTED[] = {L'S', L't', L'r', L'i', L'n', L'g', 0};

        DQN_TEST_EXPECT_MSG(testing_state, size_required == size_returned, "string_size: %d, result: %d", size_required, size_returned);
        DQN_TEST_EXPECT_MSG(testing_state, size_returned == Dqn_ArrayCount(EXPECTED) - 1, "string_size: %d, expected: %d", size_returned, sizeof(EXPECTED));
        DQN_TEST_EXPECT(testing_state, DQN_MEMCMP(EXPECTED, string, sizeof(EXPECTED)) == 0);
        Dqn_ArenaAllocator_Free(&arena);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Wide char to UTF8 size calculation");
        int result = Dqn_Win_WCharToUTF8SizeRequired(DQN_STRINGW(L"String"));
        DQN_TEST_EXPECT_MSG(testing_state, result == 6, "Size returned: %d. This size should be including the null-terminator", result);
    }

    {
        DQN_TEST_START_SCOPE(testing_state, "Wide char to UTF8");
        Dqn_ArenaAllocator arena = {};
        Dqn_StringW const INPUT  = DQN_STRINGW(L"String");
        int size_required        = Dqn_Win_WCharToUTF8SizeRequired(INPUT);
        char *string             = Dqn_ArenaAllocator_NewArray(&arena, char, size_required + 1, Dqn_ZeroMem::No);

        // Fill the string with error sentinels, which ensures the string is zero terminated
        DQN_MEMSET(string, 'Z', size_required + 1);

        int size_returned = Dqn_Win_WCharToUTF8(INPUT, string, size_required + 1);
        char const EXPECTED[] = {'S', 't', 'r', 'i', 'n', 'g', 0};

        DQN_TEST_EXPECT_MSG(testing_state, size_required == size_returned, "string_size: %d, result: %d", size_required, size_returned);
        DQN_TEST_EXPECT_MSG(testing_state, size_returned == Dqn_ArrayCount(EXPECTED) - 1, "string_size: %d, expected: %d", size_returned, sizeof(EXPECTED));
        DQN_TEST_EXPECT(testing_state, DQN_MEMCMP(EXPECTED, string, sizeof(EXPECTED)) == 0);
        Dqn_ArenaAllocator_Free(&arena);
    }
#endif // DQN_OS_WIN32
}

#define DQN_TESTS_HASH_X_MACRO \
    DQN_TESTS_HASH_X_ENTRY(SHA3_224, "SHA3-224") \
    DQN_TESTS_HASH_X_ENTRY(SHA3_256, "SHA3-256") \
    DQN_TESTS_HASH_X_ENTRY(SHA3_384, "SHA3-384") \
    DQN_TESTS_HASH_X_ENTRY(SHA3_512, "SHA3-512") \
    DQN_TESTS_HASH_X_ENTRY(Keccak_224, "Keccak-224") \
    DQN_TESTS_HASH_X_ENTRY(Keccak_256, "Keccak-256") \
    DQN_TESTS_HASH_X_ENTRY(Keccak_384, "Keccak-384") \
    DQN_TESTS_HASH_X_ENTRY(Keccak_512, "Keccak-512") \
    DQN_TESTS_HASH_X_ENTRY(Count, "Keccak-512")

enum Dqn_Tests__HashType
{
#define DQN_TESTS_HASH_X_ENTRY(enum_val, string) Hash_##enum_val,
    DQN_TESTS_HASH_X_MACRO
#undef DQN_TESTS_HASH_X_ENTRY
};

Dqn_String const DQN_TESTS__HASH_STRING[] =
{
#define DQN_TESTS_HASH_X_ENTRY(enum_val, string) DQN_STRING(string),
    DQN_TESTS_HASH_X_MACRO
#undef DQN_TESTS_HASH_X_ENTRY
};


void Dqn_Test__KeccakDispatch(Dqn_TestingState *testing_state, int hash_type, Dqn_String input)
{
#if defined(DQN_KECCAK_H)
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch();
    Dqn_String input_hex = Dqn_Hex_BytesToHexStringArena(input.str, input.size, scratch.arena);

    switch(hash_type)
    {
        case Hash_SHA3_224:
        {
            Dqn_KeccakBytes28 hash = Dqn_Keccak_SHA3_224_StringToBytes28(input);
            Dqn_KeccakBytes28 expect;
            FIPS202_SHA3_224(DQN_CAST(u8 *)input.str, input.size, (u8 *)expect.data);
            DQN_TEST_EXPECT_MSG((*testing_state),
                                Dqn_Keccak_Bytes28Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING56_FMT(Dqn_Keccak_Bytes28ToHex(&hash).str),
                                DQN_KECCAK_STRING56_FMT(Dqn_Keccak_Bytes28ToHex(&expect).str));
        }
        break;

        case Hash_SHA3_256:
        {
            Dqn_KeccakBytes32 hash = Dqn_Keccak_SHA3_256_StringToBytes32(input);
            Dqn_KeccakBytes32 expect;
            FIPS202_SHA3_256(DQN_CAST(u8 *)input.str, input.size, (u8 *)expect.data);
            DQN_TEST_EXPECT_MSG((*testing_state),
                                Dqn_Keccak_Bytes32Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING64_FMT(Dqn_Keccak_Bytes32ToHex(&hash).str),
                                DQN_KECCAK_STRING64_FMT(Dqn_Keccak_Bytes32ToHex(&expect).str));
        }
        break;

        case Hash_SHA3_384:
        {
            Dqn_KeccakBytes48 hash = Dqn_Keccak_SHA3_384_StringToBytes48(input);
            Dqn_KeccakBytes48 expect;
            FIPS202_SHA3_384(DQN_CAST(u8 *)input.str, input.size, (u8 *)expect.data);
            DQN_TEST_EXPECT_MSG((*testing_state),
                                Dqn_Keccak_Bytes48Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING96_FMT(Dqn_Keccak_Bytes48ToHex(&hash).str),
                                DQN_KECCAK_STRING96_FMT(Dqn_Keccak_Bytes48ToHex(&expect).str));
        }
        break;

        case Hash_SHA3_512:
        {
            Dqn_KeccakBytes64 hash = Dqn_Keccak_SHA3_512_StringToBytes64(input);
            Dqn_KeccakBytes64 expect;
            FIPS202_SHA3_512(DQN_CAST(u8 *)input.str, input.size, (u8 *)expect.data);
            DQN_TEST_EXPECT_MSG((*testing_state),
                                Dqn_Keccak_Bytes64Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING128_FMT(Dqn_Keccak_Bytes64ToHex(&hash).str),
                                DQN_KECCAK_STRING128_FMT(Dqn_Keccak_Bytes64ToHex(&expect).str));
        }
        break;

        case Hash_Keccak_224:
        {
            Dqn_KeccakBytes28 hash = Dqn_Keccak_224_StringToBytes28(input);
            Dqn_KeccakBytes28 expect;
            Keccak(1152, 448, DQN_CAST(u8 *)input.str, input.size, 0x01, (u8 *)expect.data, sizeof(expect));
            DQN_TEST_EXPECT_MSG((*testing_state),
                                Dqn_Keccak_Bytes28Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING56_FMT(Dqn_Keccak_Bytes28ToHex(&hash).str),
                                DQN_KECCAK_STRING56_FMT(Dqn_Keccak_Bytes28ToHex(&expect).str));
        }
        break;

        case Hash_Keccak_256:
        {
            Dqn_KeccakBytes32 hash = Dqn_Keccak_256_StringToBytes32(input);
            Dqn_KeccakBytes32 expect;
            Keccak(1088, 512, DQN_CAST(u8 *)input.str, input.size, 0x01, (u8 *)expect.data, sizeof(expect));
            DQN_TEST_EXPECT_MSG((*testing_state),
                                Dqn_Keccak_Bytes32Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING64_FMT(Dqn_Keccak_Bytes32ToHex(&hash).str),
                                DQN_KECCAK_STRING64_FMT(Dqn_Keccak_Bytes32ToHex(&expect).str));
        }
        break;

        case Hash_Keccak_384:
        {
            Dqn_KeccakBytes48 hash = Dqn_Keccak_384_StringToBytes48(input);
            Dqn_KeccakBytes48 expect;
            Keccak(832, 768, DQN_CAST(u8 *)input.str, input.size, 0x01, (u8 *)expect.data, sizeof(expect));
            DQN_TEST_EXPECT_MSG((*testing_state),
                                Dqn_Keccak_Bytes48Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING96_FMT(Dqn_Keccak_Bytes48ToHex(&hash).str),
                                DQN_KECCAK_STRING96_FMT(Dqn_Keccak_Bytes48ToHex(&expect).str));
        }
        break;

        case Hash_Keccak_512:
        {
            Dqn_KeccakBytes64 hash = Dqn_Keccak_512_StringToBytes64(input);
            Dqn_KeccakBytes64 expect;
            Keccak(576, 1024, DQN_CAST(u8 *)input.str, input.size, 0x01, (u8 *)expect.data, sizeof(expect));
            DQN_TEST_EXPECT_MSG((*testing_state),
                                Dqn_Keccak_Bytes64Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING128_FMT(Dqn_Keccak_Bytes64ToHex(&hash).str),
                                DQN_KECCAK_STRING128_FMT(Dqn_Keccak_Bytes64ToHex(&expect).str));
        }
        break;

    }
#endif // DQN_KECCAK_H
}

void Dqn_Test_Keccak()
{
#if defined(DQN_KECCAK_H)
    Dqn_TestingState testing_state = {};

    Dqn_String const INPUTS[] = {
        DQN_STRING("abc"),
        DQN_STRING(""),
        DQN_STRING("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
        DQN_STRING("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmno"
                   "pqrstnopqrstu"),
    };

    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Keccak");
    for (int hash_type = 0; hash_type < Hash_Count; hash_type++)
    {
        pcg32_random_t rng = {};
        pcg32_srandom_r(&rng, 0xd48e'be21'2af8'733d, 0x3f89'3bd2'd6b0'4eef);

        for (Dqn_String input : INPUTS)
        {
            DQN_TEST_START_SCOPE(testing_state, "%.*s - Input: %.*s", DQN_STRING_FMT(DQN_TESTS__HASH_STRING[hash_type]), DQN_MIN(input.size, 54), input.str);
            Dqn_Test__KeccakDispatch(&testing_state, hash_type, input);
        }

        DQN_TEST_START_SCOPE(testing_state, "%.*s - Deterministic random inputs", DQN_STRING_FMT(DQN_TESTS__HASH_STRING[hash_type]));
        for (int index = 0; index < 128; index++)
        {
            char    src[4096] = {};
            Dqn_u32 src_size  = pcg32_boundedrand_r(&rng, sizeof(src));

            for (int src_index = 0; src_index < src_size; src_index++)
                src[src_index] = pcg32_boundedrand_r(&rng, 255);

            Dqn_String input = Dqn_String_Init(src, src_size);
            Dqn_Test__KeccakDispatch(&testing_state, hash_type, input);
        }
    }
#endif // DQN_KECCAK_H
}

void Dqn_Test_RunSuite()
{
    Dqn_Test_Array();
    Dqn_Test_File();
    Dqn_Test_FixedArray();
    Dqn_Test_FixedString();
    Dqn_Test_Hex();
    Dqn_Test_Intrinsics();
    Dqn_Test_M4();
    Dqn_Test_DSMap();
    Dqn_Test_Map();
    Dqn_Test_Rect();
    Dqn_Test_PerfCounter();
    Dqn_Test_OS();
    Dqn_Test_Keccak();
    Dqn_Test_Str();
    Dqn_Test_String();
    Dqn_Test_TicketMutex();
    Dqn_Test_Win();

    fprintf(stdout, "Summary: %d/%d tests succeeded\n", g_dqn_test_total_good_tests, g_dqn_test_total_tests);
}

#if defined(DQN_TEST_WITH_MAIN)
int main(int argc, char *argv[])
{
    (void)argv; (void)argc;
    Dqn_Test_RunSuite();
    return 0;
}
#endif

