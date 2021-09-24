// -------------------------------------------------------------------------------------------------
// NOTE: Preprocessor Config
// -------------------------------------------------------------------------------------------------
/*
#define DQN_TEST_WITH_MAIN Define this to enable the main function and allow standalone compiling
                           and running of the file.
#define DQN_TEST_NO_COLORS Define this to disable any ANSI terminal color codes from output
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

#if defined(DQN_TEST_NO_COLORS)
    #define DQN_TEST_COLOR_RED
    #define DQN_TEST_COLOR_GREEN
    #define DQN_TEST_COLOR_YELLOW
    #define DQN_TEST_COLOR_BLUE
    #define DQN_TEST_COLOR_MAGENTA
    #define DQN_TEST_COLOR_CYAN
    #define DQN_TEST_COLOR_RESET
#else
    #define DQN_TEST_COLOR_RED "\x1b[31m"
    #define DQN_TEST_COLOR_GREEN "\x1b[32m"
    #define DQN_TEST_COLOR_YELLOW "\x1b[33m"
    #define DQN_TEST_COLOR_BLUE "\x1b[34m"
    #define DQN_TEST_COLOR_MAGENTA "\x1b[35m"
    #define DQN_TEST_COLOR_CYAN "\x1b[36m"
    #define DQN_TEST_COLOR_RESET "\x1b[0m"
#endif

#define DQN_TEST_GROUP(test, fmt, ...)                                                                                 \
    fprintf(stdout, fmt "\n", ##__VA_ARGS__);                                                                          \
    DQN_DEFER                                                                                                          \
    {                                                                                                                  \
        bool all_clear = test.num_tests_ok_in_group == test.num_tests_in_group;                                        \
        fprintf(stdout,                                                                                                \
                "%s\n  %02d/%02d tests passed -- %s\n\n" DQN_TEST_COLOR_RESET,                                         \
                all_clear ? DQN_TEST_COLOR_GREEN : DQN_TEST_COLOR_RED,                                                 \
                test.num_tests_ok_in_group,                                                                            \
                test.num_tests_in_group,                                                                               \
                all_clear ? "OK" : "FAILED");                                                                          \
    }

#define DQN_TEST(test, fmt, ...)                                                                                       \
    Dqn_TestBegin(&test, fmt, ##__VA_ARGS__);                                                                          \
    DQN_DEFER                                                                                                          \
    {                                                                                                                  \
        if (!test.failed)                                                                                              \
        {                                                                                                              \
            fprintf(stdout, DQN_TEST_COLOR_GREEN " OK" DQN_TEST_COLOR_RESET "\n");                                     \
            test.num_tests_ok_in_group++;                                                                              \
        }                                                                                                              \
    };

#define DQN_TEST_ASSERT_MSG(test, expr, fmt, ...)                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            if (!test.failed)                                                                                          \
            {                                                                                                          \
                fprintf(stdout, DQN_TEST_COLOR_RED " FAILED" DQN_TEST_COLOR_RESET "\n");                               \
                test.failed = true;                                                                                    \
            }                                                                                                          \
                                                                                                                       \
            fprintf(stderr,                                                                                            \
                    "    File: %s:%d\n"                                                                                \
                    "    Expression: [" #expr "]\n"                                                                    \
                    "    Reason: " fmt "\n\n",                                                                         \
                    __FILE__,                                                                                          \
                    __LINE__,                                                                                          \
                    ##__VA_ARGS__);                                                                                    \
        }                                                                                                              \
    } while (0)

#define DQN_TEST_ASSERT(test, expr) DQN_TEST_ASSERT_MSG(test, expr, "")

struct Dqn_Test
{
    int num_tests_in_group;
    int num_tests_ok_in_group;
    int failed;
};

void Dqn_TestBegin(Dqn_Test *state, char const *fmt, ...)
{
    state->num_tests_in_group++;
    state->failed = false;

    va_list args;
    va_start(args, fmt);

    int size_required = 0;
    {
        va_list args_copy;
        va_copy(args_copy, args);
        size_required = vsnprintf(nullptr, 0, fmt, args_copy);
        va_end(args_copy);
    }

    printf("  ");
    vprintf(fmt, args);
    int const PAD_LENGTH = 90;
    for (int pad = size_required; pad < PAD_LENGTH; pad++)
        putc('.', stdout);

    va_end(args);
}

void Dqn_TestFinalize(Dqn_Test const *result)
{
}

Dqn_Test Dqn_Test_Array()
{
    Dqn_Test test = {};
    DQN_TEST_GROUP(test, "Dqn_Array");
    // NOTE: Dqn_ArrayInitWithMemory
    {
        {
            DQN_TEST(test, "Fixed Memory: Test add single item and can't allocate more");
            int memory[4]        = {};
            Dqn_Array<int> array = Dqn_ArrayInitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*size*/);
            Dqn_ArrayAdd(&array, 1);
            Dqn_ArrayAdd(&array, 2);
            Dqn_ArrayAdd(&array, 3);
            Dqn_ArrayAdd(&array, 4);
            DQN_TEST_ASSERT_MSG(test, array.data[0] == 1, "array.data %d", array.data[0]);
            DQN_TEST_ASSERT_MSG(test, array.data[1] == 2, "array.data %d", array.data[1]);
            DQN_TEST_ASSERT_MSG(test, array.data[2] == 3, "array.data %d", array.data[2]);
            DQN_TEST_ASSERT_MSG(test, array.data[3] == 4, "array.data %d", array.data[3]);
            DQN_TEST_ASSERT_MSG(test, array.size    == 4, "array.size: %zu", array.size);

            int *added_item = Dqn_ArrayAdd(&array, 5);
            DQN_TEST_ASSERT(test, added_item == nullptr);
            DQN_TEST_ASSERT_MSG(test, array.size == 4, "array.size: %zu", array.size);
            DQN_TEST_ASSERT_MSG(test, array.max == 4, "array.max: %zu", array.max);
        }

        {
            DQN_TEST(test, "Fixed Memory: Test add array of items");
            int memory[4]        = {};
            int DATA[]           = {1, 2, 3};
            Dqn_Array<int> array = Dqn_ArrayInitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*size*/);
            Dqn_ArrayAddArray(&array, DATA, Dqn_ArrayCount(DATA));
            DQN_TEST_ASSERT_MSG(test, array.data[0] == 1, "array.data %d", array.data[0]);
            DQN_TEST_ASSERT_MSG(test, array.data[1] == 2, "array.data %d", array.data[1]);
            DQN_TEST_ASSERT_MSG(test, array.data[2] == 3, "array.data %d", array.data[2]);
            DQN_TEST_ASSERT_MSG(test, array.size == 3, "array.size: %zu", array.size);
            DQN_TEST_ASSERT_MSG(test, array.max == 4, "array.max: %zu", array.max);
        }

        {
            DQN_TEST(test, "Fixed Memory: Test clear and clear with memory zeroed");
            int memory[4]        = {};
            int DATA[]           = {1, 2, 3};
            Dqn_Array<int> array = Dqn_ArrayInitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*size*/);
            Dqn_ArrayAddArray(&array, DATA, Dqn_ArrayCount(DATA));
            Dqn_ArrayClear(&array, Dqn_ZeroMem::No);
            DQN_TEST_ASSERT_MSG(test, array.size == 0, "array.size: %zu", array.size);
            DQN_TEST_ASSERT_MSG(test, array.max == 4, "array.max: %zu", array.max);
            DQN_TEST_ASSERT_MSG(test, array.data[0] == 1, "array.data %d. Clear but don't zero memory so old values should still remain", array.data[0]);

            Dqn_ArrayClear(&array, Dqn_ZeroMem::Yes);
            DQN_TEST_ASSERT_MSG(test, array.data[0] == 0, "array.data %d. Clear but zero memory old values should not remain", array.data[0]);
        }

        {
            DQN_TEST(test, "Fixed Memory: Test erase stable and erase unstable");
            int memory[4]        = {};
            int DATA[]           = {1, 2, 3, 4};
            Dqn_Array<int> array = Dqn_ArrayInitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*size*/);
            Dqn_ArrayAddArray(&array, DATA, Dqn_ArrayCount(DATA));
            Dqn_ArrayEraseUnstable(&array, 1);
            DQN_TEST_ASSERT_MSG(test, array.data[0] == 1, "array.data %d", array.data[0]);
            DQN_TEST_ASSERT_MSG(test, array.data[1] == 4, "array.data %d", array.data[1]);
            DQN_TEST_ASSERT_MSG(test, array.data[2] == 3, "array.data %d", array.data[2]);
            DQN_TEST_ASSERT_MSG(test, array.size == 3, "array.size: %zu", array.size);

            Dqn_ArrayEraseStable(&array, 0);
            DQN_TEST_ASSERT_MSG(test, array.data[0] == 4, "array.data: %d", array.data[0]);
            DQN_TEST_ASSERT_MSG(test, array.data[1] == 3, "array.data: %d", array.data[1]);
            DQN_TEST_ASSERT_MSG(test, array.size == 2, "array.size: %zu", array.size);
        }

        {
            DQN_TEST(test, "Fixed Memory: Test array pop and peek");
            int memory[4]        = {};
            int DATA[]           = {1, 2, 3};
            Dqn_Array<int> array = Dqn_ArrayInitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*size*/);
            Dqn_ArrayAddArray(&array, DATA, Dqn_ArrayCount(DATA));
            Dqn_ArrayPop(&array, 2);
            DQN_TEST_ASSERT_MSG(test, array.data[0] == 1, "array.data: %d", array.data[0]);
            DQN_TEST_ASSERT_MSG(test, array.size == 1, "array.size: %zu", array.size);
            DQN_TEST_ASSERT_MSG(test, array.max == 4, "array.max: %zu", array.max);

            int *peek_item = Dqn_ArrayPeek(&array);
            DQN_TEST_ASSERT_MSG(test, *peek_item == 1, "peek: %d", *peek_item);
            DQN_TEST_ASSERT_MSG(test, array.size == 1, "array.size: %zu", array.size);
            DQN_TEST_ASSERT_MSG(test, array.max == 4, "array.max: %zu", array.max);
        }
    }

    // NOTE: Dynamic Memory: Dqn_Array
    {
        DQN_TEST(test, "Dynamic Memory: Reserve and check over commit reallocates");
        Dqn_Arena arena = {};
        Dqn_Array<int> array = {};
        array.arena = &arena;

        Dqn_ArrayReserve(&array, 4);
        DQN_TEST_ASSERT_MSG(test, array.size == 0, "array.size: %zu", array.size);
        DQN_TEST_ASSERT_MSG(test, array.max == 4, "array.max: %zu", array.max);

        int DATA[] = {1, 2, 3, 4};
        Dqn_ArrayAddArray(&array, DATA, Dqn_ArrayCount(DATA));
        DQN_TEST_ASSERT_MSG(test, array.data[0] == 1, "array.data: %d", array.data[0]);
        DQN_TEST_ASSERT_MSG(test, array.data[1] == 2, "array.data: %d", array.data[1]);
        DQN_TEST_ASSERT_MSG(test, array.data[2] == 3, "array.data: %d", array.data[2]);
        DQN_TEST_ASSERT_MSG(test, array.data[3] == 4, "array.data: %d", array.data[3]);
        DQN_TEST_ASSERT_MSG(test, array.size == 4, "array.size: %zu", array.size);

        int *added_item = Dqn_ArrayAdd(&array, 5);
        DQN_TEST_ASSERT_MSG(test, *added_item == 5, "added_item: %d", *added_item);
        DQN_TEST_ASSERT_MSG(test, array.data[4] == 5, "array.data: %d", array.data[4]);
        DQN_TEST_ASSERT_MSG(test, array.size == 5, "array.size: %zu", array.size);
        DQN_TEST_ASSERT_MSG(test, array.max >= 5, "array.max: %zu", array.max);

        Dqn_ArenaFree(&arena);
    }
    return test;
}

Dqn_Test Dqn_Test_File()
{
    Dqn_Test test = {};
    DQN_TEST_GROUP(test, "Dqn_File");
    {
        Dqn_Arena arena = {};
        DQN_TEST(test, "Make directory recursive \"abcd/efgh\"");
        Dqn_b32 success = Dqn_FileMakeDir(DQN_STRING("abcd/efgh"), &arena);
        DQN_TEST_ASSERT(test, success);
        DQN_TEST_ASSERT(test, Dqn_FileDirExists(DQN_STRING("abcd")));
        DQN_TEST_ASSERT(test, Dqn_FileDirExists(DQN_STRING("abcd/efgh")));
        DQN_TEST_ASSERT_MSG(test, Dqn_FileExists(DQN_STRING("abcd")) == false, "This function should only return true for files");
        DQN_TEST_ASSERT_MSG(test, Dqn_FileExists(DQN_STRING("abcd/efgh")) == false, "This function should only return true for files");

        DQN_TEST_ASSERT(test, Dqn_FileDelete(DQN_STRING("abcd/efgh")));
        DQN_TEST_ASSERT_MSG(test, Dqn_FileDelete(DQN_STRING("abcd")), "Failed to cleanup directory");
        Dqn_ArenaFree(&arena);
    }

    {
        // NOTE: Write step
        Dqn_String const SRC_FILE = DQN_STRING("dqn_test_file");
        DQN_TEST(test, "Write file, read it, copy it, move it and delete it");
        Dqn_b32 write_result = Dqn_FileWriteFile(SRC_FILE.str, "test", 4);
        DQN_TEST_ASSERT(test, write_result);
        DQN_TEST_ASSERT(test, Dqn_FileExists(SRC_FILE));

        // NOTE: Read step
        Dqn_Arena arena = {};
        Dqn_String read_file = Dqn_FileArenaReadFileToString(SRC_FILE.str, &arena);
        DQN_TEST_ASSERT(test, Dqn_StringIsValid(read_file));
        DQN_TEST_ASSERT(test, read_file.size == 4);
        DQN_TEST_ASSERT_MSG(test, Dqn_StringEq(read_file, DQN_STRING("test")), "read(%zu): %.*s", read_file.size, DQN_STRING_FMT(read_file));

        // NOTE: Copy step
        Dqn_String const COPY_FILE = DQN_STRING("dqn_test_file_copy");
        Dqn_b32 copy_result = Dqn_FileCopy(SRC_FILE, COPY_FILE, true /*overwrite*/);
        DQN_TEST_ASSERT(test, copy_result);
        DQN_TEST_ASSERT(test, Dqn_FileExists(COPY_FILE));

        // NOTE: Move step
        Dqn_String const MOVE_FILE = DQN_STRING("dqn_test_file_move");
        Dqn_b32 move_result = Dqn_FileMove(COPY_FILE, MOVE_FILE, true /*overwrite*/);
        DQN_TEST_ASSERT(test, move_result);
        DQN_TEST_ASSERT(test, Dqn_FileExists(MOVE_FILE));
        DQN_TEST_ASSERT_MSG(test, Dqn_FileExists(COPY_FILE) == false, "Moving a file should remove the original");

        // NOTE: Delete step
        Dqn_b32 delete_src_file   = Dqn_FileDelete(SRC_FILE);
        Dqn_b32 delete_moved_file = Dqn_FileDelete(MOVE_FILE);
        DQN_TEST_ASSERT(test, delete_src_file);
        DQN_TEST_ASSERT(test, delete_moved_file);

        // NOTE: Deleting non-existent file fails
        Dqn_b32 delete_non_existent_src_file   = Dqn_FileDelete(SRC_FILE);
        Dqn_b32 delete_non_existent_moved_file = Dqn_FileDelete(MOVE_FILE);
        DQN_TEST_ASSERT(test, delete_non_existent_moved_file == false);
        DQN_TEST_ASSERT(test, delete_non_existent_src_file == false);

        Dqn_ArenaFree(&arena);
    }
    return test;
}

Dqn_Test Dqn_Test_FixedArray()
{
    Dqn_Test test = {};
#if defined(DQN_WITH_FIXED_ARRAY)
    DQN_TEST_GROUP(test, "Dqn_FixedArray");
    // NOTE: Dqn_FixedArray_Init
    {
        DQN_TEST(test, "Initialise from raw array");
        int raw_array[] = {1, 2};
        auto array = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
        DQN_TEST_ASSERT(test, array.size == 2);
        DQN_TEST_ASSERT(test, array[0] == 1);
        DQN_TEST_ASSERT(test, array[1] == 2);
    }

    // NOTE: Dqn_FixedArray_EraseStable
    {
        DQN_TEST(test, "Erase stable 1 element from array");
        int raw_array[] = {1, 2, 3};
        auto array = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
        Dqn_FixedArray_EraseStable(&array, 1);
        DQN_TEST_ASSERT(test, array.size == 2);
        DQN_TEST_ASSERT(test, array[0] == 1);
        DQN_TEST_ASSERT(test, array[1] == 3);
    }

    // NOTE: Dqn_FixedArray_EraseUnstable
    {
        DQN_TEST(test, "Erase unstable 1 element from array");
        int raw_array[] = {1, 2, 3};
        auto array = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
        Dqn_FixedArray_EraseUnstable(&array, 0);
        DQN_TEST_ASSERT(test, array.size == 2);
        DQN_TEST_ASSERT(test, array[0] == 3);
        DQN_TEST_ASSERT(test, array[1] == 2);
    }

    // NOTE: Dqn_FixedArray_Add
    {
        DQN_TEST(test, "Add 1 element to array");
        int const ITEM  = 2;
        int raw_array[] = {1};
        auto array      = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
        Dqn_FixedArray_Add(&array, ITEM);
        DQN_TEST_ASSERT(test, array.size == 2);
        DQN_TEST_ASSERT(test, array[0] == 1);
        DQN_TEST_ASSERT(test, array[1] == ITEM);
    }

    // NOTE: Dqn_FixedArray_Clear
    {
        DQN_TEST(test, "Clear array");
        int raw_array[] = {1};
        auto array      = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_ArrayCount(raw_array));
        Dqn_FixedArray_Clear(&array);
        DQN_TEST_ASSERT(test, array.size == 0);
    }
#endif // DQN_WITH_FIXED_ARRAY
    return test;
}

Dqn_Test Dqn_Test_FixedString()
{
    Dqn_Test test = {};
#if defined(DQN_WITH_FIXED_STRING)
    DQN_TEST_GROUP(test, "Dqn_FixedString");

    // NOTE: Dqn_FixedStringAppend
    {
        DQN_TEST(test, "Append too much fails");
        Dqn_FixedString<4> str = {};
        DQN_TEST_ASSERT_MSG(test, Dqn_FixedStringAppend(&str, "abcd") == false, "We need space for the null-terminator");
    }

    // NOTE: Dqn_FixedStringAppendFmt
    {
        DQN_TEST(test, "Append format string too much fails");
        Dqn_FixedString<4> str = {};
        DQN_TEST_ASSERT_MSG(test, Dqn_FixedStringAppendFmt(&str, "abcd") == false, "We need space for the null-terminator");
    }
#endif // DQN_WITH_FIXED_STRING
    return test;
}

Dqn_Test Dqn_Test_Hex()
{
    Dqn_Test test = {};
#if defined(DQN_WITH_HEX)
    DQN_TEST_GROUP(test, "Dqn_Hex");
    {
        DQN_TEST(test, "Convert 0x123");
        Dqn_u64 result = Dqn_HexStringToU64(DQN_STRING("0x123"));
        DQN_TEST_ASSERT_MSG(test, result == 0x123, "result: %zu", result);
    }

    {
        DQN_TEST(test, "Convert 0xFFFF");
        Dqn_u64 result = Dqn_HexStringToU64(DQN_STRING("0xFFFF"));
        DQN_TEST_ASSERT_MSG(test, result == 0xFFFF, "result: %zu", result);
    }

    {
        DQN_TEST(test, "Convert FFFF");
        Dqn_u64 result = Dqn_HexStringToU64(DQN_STRING("FFFF"));
        DQN_TEST_ASSERT_MSG(test, result == 0xFFFF, "result: %zu", result);
    }

    {
        DQN_TEST(test, "Convert abCD");
        Dqn_u64 result = Dqn_HexStringToU64(DQN_STRING("abCD"));
        DQN_TEST_ASSERT_MSG(test, result == 0xabCD, "result: %zu", result);
    }

    {
        DQN_TEST(test, "Convert 0xabCD");
        Dqn_u64 result = Dqn_HexStringToU64(DQN_STRING("0xabCD"));
        DQN_TEST_ASSERT_MSG(test, result == 0xabCD, "result: %zu", result);
    }

    {
        DQN_TEST(test, "Convert 0x");
        Dqn_u64 result = Dqn_HexStringToU64(DQN_STRING("0x"));
        DQN_TEST_ASSERT_MSG(test, result == 0x0, "result: %zu", result);
    }

    {
        DQN_TEST(test, "Convert 0X");
        Dqn_u64 result = Dqn_HexStringToU64(DQN_STRING("0X"));
        DQN_TEST_ASSERT_MSG(test, result == 0x0, "result: %zu", result);
    }

    {
        DQN_TEST(test, "Convert 3");
        Dqn_u64 result = Dqn_HexStringToU64(DQN_STRING("3"));
        DQN_TEST_ASSERT_MSG(test, result == 3, "result: %zu", result);
    }

    {
        DQN_TEST(test, "Convert f");
        Dqn_u64 result = Dqn_HexStringToU64(DQN_STRING("f"));
        DQN_TEST_ASSERT_MSG(test, result == 0xf, "result: %zu", result);
    }

    {
        DQN_TEST(test, "Convert g");
        Dqn_u64 result = Dqn_HexStringToU64(DQN_STRING("g"));
        DQN_TEST_ASSERT_MSG(test, result == 0, "result: %zu", result);
    }

    {
        DQN_TEST(test, "Convert -0x3");
        Dqn_u64 result = Dqn_HexStringToU64(DQN_STRING("-0x3"));
        DQN_TEST_ASSERT_MSG(test, result == 0, "result: %zu", result);
    }
#endif // DQN_WITH_HEX
    return test;
}

Dqn_Test Dqn_Test_M4()
{
    Dqn_Test test = {};
#if defined(DQN_WITH_MATH)
    DQN_TEST_GROUP(test, "Dqn_M4");
    {
        DQN_TEST(test, "Simple translate and scale matrix");
        Dqn_M4 translate = Dqn_M4TranslateF(1, 2, 3);
        Dqn_M4 scale     = Dqn_M4ScaleF(2, 2, 2);
        Dqn_M4 result    = Dqn_M4Mul(translate, scale);

        const Dqn_M4 EXPECT = {{
            {2, 0, 0, 0},
            {0, 2, 0, 0},
            {0, 0, 2, 0},
            {1, 2, 3, 1},
        }};

        DQN_TEST_ASSERT_MSG(test,
                            memcmp(result.columns, EXPECT.columns, sizeof(EXPECT)) == 0,
                            "\nresult =\n%s\nexpected =\n%s",
                            Dqn_M4ColumnMajorString(result).str,
                            Dqn_M4ColumnMajorString(EXPECT).str);
    }
#endif // DQN_WITH_MATH
    return test;
}

Dqn_Test Dqn_Test_DSMap()
{
    Dqn_Test test = {};
#if defined(DQN_WITH_DSMAP)
    DQN_TEST_GROUP(test, "Dqn_DSMap");
    {
        DQN_TEST(test, "Add r-value item to map");
        Dqn_DSMap<int> map         = Dqn_DSMapInit<int>(128);
        Dqn_DSMapEntry<int> *entry = Dqn_DSMapAddCopy(&map, 3 /*hash*/, 5 /*value*/);
        DQN_TEST_ASSERT_MSG(test, map.size == 128, "size: %I64d", map.size);
        DQN_TEST_ASSERT_MSG(test, map.count == 1, "count: %zu", map.count);
        DQN_TEST_ASSERT_MSG(test, entry->hash == 3, "hash: %zu", entry->hash);
        DQN_TEST_ASSERT_MSG(test, entry->value == 5, "value: %d", entry->value);
        Dqn_DSMapFree(&map);
    }

    {
        DQN_TEST(test, "Add l-value item to map");
        Dqn_DSMap<int>       map   = Dqn_DSMapInit<int>(128);
        int                  value = 5;
        Dqn_DSMapEntry<int> *entry = Dqn_DSMapAdd(&map, 3 /*hash*/, value);
        DQN_TEST_ASSERT_MSG(test, map.size == 128, "size: %I64d", map.size);
        DQN_TEST_ASSERT_MSG(test, map.count == 1, "count: %zu", map.count);
        DQN_TEST_ASSERT_MSG(test, entry->hash == 3, "hash: %zu", entry->hash);
        DQN_TEST_ASSERT_MSG(test, entry->value == 5, "value: %d", entry->value);
        Dqn_DSMapFree(&map);
    }

    {
        DQN_TEST(test, "Get item from map");
        Dqn_DSMap<int> map         = Dqn_DSMapInit<int>(128);
        Dqn_DSMapEntry<int> *entry = Dqn_DSMapAddCopy(&map, 3 /*hash*/, 5 /*value*/);
        Dqn_DSMapEntry<int> *get_entry = Dqn_DSMapGet(&map, 3 /*hash*/);
        DQN_TEST_ASSERT_MSG(test, get_entry == entry, "get_entry: %p, entry: %p", get_entry, entry);
        Dqn_DSMapFree(&map);
    }

    {
        DQN_TEST(test, "Get non-existent item from map");
        Dqn_DSMap<int> map         = Dqn_DSMapInit<int>(128);
        Dqn_DSMapEntry<int> *entry = Dqn_DSMapGet(&map, 3 /*hash*/);
        DQN_TEST_ASSERT(test, entry == nullptr);
        Dqn_DSMapFree(&map);
    }

    {
        DQN_TEST(test, "Erase item from map");
        Dqn_DSMap<int> map = Dqn_DSMapInit<int>(128);
        Dqn_DSMapAddCopy(&map, 3 /*hash*/, 5 /*value*/);
        DQN_TEST_ASSERT_MSG(test, map.count == 1, "count: %I64d", map.count);
        Dqn_DSMapErase(&map, 3 /*hash*/, Dqn_ZeroMem::No);
        DQN_TEST_ASSERT_MSG(test, map.count == 0, "count: %I64d", map.count);
        Dqn_DSMapFree(&map);
    }

    {
        DQN_TEST(test, "Erase non-existent item from map");
        Dqn_DSMap<int> map = Dqn_DSMapInit<int>(128);
        Dqn_DSMapErase(&map, 3 /*hash*/, Dqn_ZeroMem::No);
        DQN_TEST_ASSERT_MSG(test, map.count == 0, "count: %I64d", map.count);
        Dqn_DSMapFree(&map);
    }

    {
        DQN_TEST(test, "Test resize on maximum load");
        const Dqn_isize INIT_SIZE = 4;
        Dqn_DSMap<int> map = Dqn_DSMapInit<int>(INIT_SIZE);
        Dqn_DSMapAddCopy(&map, 0 /*hash*/, 5 /*value*/);
        Dqn_DSMapAddCopy(&map, 1 /*hash*/, 5 /*value*/);
        DQN_TEST_ASSERT_MSG(test, map.count == 2, "count: %I64d", map.count);

        // This *should* cause a resize because 3/4 slots filled is 75% load
        Dqn_DSMapAddCopy(&map, 6 /*hash*/, 5 /*value*/);
        DQN_TEST_ASSERT_MSG(test, map.count == 3, "count: %I64d", map.count);
        DQN_TEST_ASSERT_MSG(test, map.size == INIT_SIZE * 2, "size: %I64d", map.size);

        // Check that the elements are rehashed where we expected them to be
        DQN_TEST_ASSERT    (test, map.slots[0].occupied == DQN_CAST(Dqn_u8)true);
        DQN_TEST_ASSERT    (test, map.slots[1].occupied == DQN_CAST(Dqn_u8)true);
        DQN_TEST_ASSERT    (test, map.slots[2].occupied == DQN_CAST(Dqn_u8)false);
        DQN_TEST_ASSERT    (test, map.slots[3].occupied == DQN_CAST(Dqn_u8)false);
        DQN_TEST_ASSERT    (test, map.slots[4].occupied == DQN_CAST(Dqn_u8)false);
        DQN_TEST_ASSERT    (test, map.slots[5].occupied == DQN_CAST(Dqn_u8)false);
        DQN_TEST_ASSERT    (test, map.slots[6].occupied == DQN_CAST(Dqn_u8)true);
        DQN_TEST_ASSERT    (test, map.slots[7].occupied == DQN_CAST(Dqn_u8)false);

        DQN_TEST_ASSERT_MSG(test, map.slots[0].value == 5, "value: %d", map.slots[0].value);
        DQN_TEST_ASSERT_MSG(test, map.slots[1].value == 5, "value: %d", map.slots[1].value);
        DQN_TEST_ASSERT_MSG(test, map.slots[6].value == 5, "value: %d", map.slots[6].value);

        Dqn_DSMapFree(&map);
    }
#endif // DQN_WITH_DSMAP
    return test;
}

Dqn_Test Dqn_Test_Map()
{
    Dqn_Test test = {};
#if defined(DQN_WITH_MAP)
    DQN_TEST_GROUP(test, "Dqn_Map");
    Dqn_Arena arena = {};
    {
        DQN_TEST(test, "Add r-value item to map");
        Dqn_Map<int> map         = Dqn_MapInitWithArena<int>(&arena, 1);
        Dqn_MapEntry<int> *entry = Dqn_MapAddCopy(&map, 3 /*hash*/, 5 /*value*/, Dqn_MapCollideRule::Overwrite);
        DQN_TEST_ASSERT_MSG(test, map.size == 1, "size: %I64d", map.size);
        DQN_TEST_ASSERT_MSG(test, map.count == 1, "count: %zu", map.count);
        DQN_TEST_ASSERT_MSG(test, map.chain_count == 0, "chain_count: %zu", map.chain_count);
        DQN_TEST_ASSERT_MSG(test, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TEST_ASSERT_MSG(test, entry->hash == 3, "hash: %zu", entry->hash);
        DQN_TEST_ASSERT_MSG(test, entry->value == 5, "value: %d", entry->value);
        DQN_TEST_ASSERT_MSG(test, entry->next == nullptr, "next: %p", entry->next);
        Dqn_ArenaFree(&arena);
    }

    {
        DQN_TEST(test, "Add l-value item to map");
        Dqn_Map<int>       map   = Dqn_MapInitWithArena<int>(&arena, 1);
        int                value = 5;
        Dqn_MapEntry<int> *entry = Dqn_MapAdd(&map, 3 /*hash*/, value, Dqn_MapCollideRule::Overwrite);
        DQN_TEST_ASSERT_MSG(test, map.size == 1, "size: %I64d", map.size);
        DQN_TEST_ASSERT_MSG(test, map.count == 1, "count: %zu", map.count);
        DQN_TEST_ASSERT_MSG(test, map.chain_count == 0, "chain_count: %zu", map.chain_count);
        DQN_TEST_ASSERT_MSG(test, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TEST_ASSERT_MSG(test, entry->hash == 3, "hash: %zu", entry->hash);
        DQN_TEST_ASSERT_MSG(test, entry->value == 5, "value: %d", entry->value);
        DQN_TEST_ASSERT_MSG(test, entry->next == nullptr, "next: %p", entry->next);
        Dqn_ArenaFree(&arena);
    }

    {
        DQN_TEST(test, "Add r-value item and overwrite on collision");
        Dqn_Map<int>       map   = Dqn_MapInitWithArena<int>(&arena, 1);
        Dqn_MapEntry<int> *entry_a = Dqn_MapAddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapEntry<int> *entry_b = Dqn_MapAddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Overwrite);
        DQN_TEST_ASSERT_MSG(test, map.size == 1, "size: %zu", map.size);
        DQN_TEST_ASSERT_MSG(test, map.count == 1, "count: %zu", map.count);
        DQN_TEST_ASSERT_MSG(test, map.chain_count == 0, "chain_count: %zu", map.chain_count);
        DQN_TEST_ASSERT_MSG(test, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TEST_ASSERT_MSG(test, entry_a == entry_b, "Expected entry to be overwritten");
        DQN_TEST_ASSERT_MSG(test, entry_b->hash == 4, "hash: %zu", entry_b->hash);
        DQN_TEST_ASSERT_MSG(test, entry_b->value == 6, "value: %d", entry_b->value);
        DQN_TEST_ASSERT_MSG(test, entry_b->next == nullptr, "next: %p", entry_b->next);
        Dqn_ArenaFree(&arena);
    }

    {
        DQN_TEST(test, "Add r-value item and fail on collision");
        Dqn_Map<int> map = Dqn_MapInitWithArena<int>(&arena, 1);
        Dqn_MapAddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapEntry<int> *entry_b = Dqn_MapAddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Fail);
        DQN_TEST_ASSERT_MSG(test, entry_b == nullptr, "Expected entry to be overwritten");
        DQN_TEST_ASSERT_MSG(test, map.size == 1, "size: %zu", map.size);
        DQN_TEST_ASSERT_MSG(test, map.count == 1, "count: %zu", map.count);
        DQN_TEST_ASSERT_MSG(test, map.chain_count == 0, "chain_count: %zu", map.chain_count);
        DQN_TEST_ASSERT_MSG(test, map.free_list == nullptr, "free_list: %p", map.free_list);
        Dqn_ArenaFree(&arena);
    }

    {
        DQN_TEST(test, "Add r-value item and chain on collision");
        Dqn_Map<int>       map   = Dqn_MapInitWithArena<int>(&arena, 1);
        Dqn_MapEntry<int> *entry_a = Dqn_MapAddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapEntry<int> *entry_b = Dqn_MapAddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Chain);
        DQN_TEST_ASSERT_MSG(test, map.size == 1, "size: %zu", map.size);
        DQN_TEST_ASSERT_MSG(test, map.count == 1, "count: %zu", map.count);
        DQN_TEST_ASSERT_MSG(test, map.chain_count == 1, "chain_count: %zu", map.chain_count);
        DQN_TEST_ASSERT_MSG(test, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TEST_ASSERT_MSG(test, entry_a != entry_b, "Expected colliding entry to be chained");
        DQN_TEST_ASSERT_MSG(test, entry_a->next == entry_b, "Expected chained entry to be next to our first map entry");
        DQN_TEST_ASSERT_MSG(test, entry_b->hash == 4, "hash: %zu", entry_b->hash);
        DQN_TEST_ASSERT_MSG(test, entry_b->value == 6, "value: %d", entry_b->value);
        DQN_TEST_ASSERT_MSG(test, entry_b->next == nullptr, "next: %p", entry_b->next);
        Dqn_ArenaFree(&arena);
    }

    {
        DQN_TEST(test, "Add r-value item and get them back out again");
        Dqn_Map<int>       map   = Dqn_MapInitWithArena<int>(&arena, 1);
        Dqn_MapEntry<int> *entry_a = Dqn_MapAddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapEntry<int> *entry_b = Dqn_MapAddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Chain);

        Dqn_MapEntry<int> *entry_a_copy = Dqn_MapGet(&map, 3 /*hash*/);
        Dqn_MapEntry<int> *entry_b_copy = Dqn_MapGet(&map, 4 /*hash*/);
        DQN_TEST_ASSERT_MSG(test, map.size == 1, "size: %zu", map.size);
        DQN_TEST_ASSERT_MSG(test, map.count == 1, "count: %zu", map.count);
        DQN_TEST_ASSERT_MSG(test, map.chain_count == 1, "chain_count: %zu", map.chain_count);
        DQN_TEST_ASSERT_MSG(test, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TEST_ASSERT(test, entry_a_copy == entry_a);
        DQN_TEST_ASSERT(test, entry_b_copy == entry_b);
        Dqn_ArenaFree(&arena);
    }

    {
        DQN_TEST(test, "Add r-value item and erase it");
        Dqn_Map<int>       map   = Dqn_MapInitWithArena<int>(&arena, 1);
        Dqn_MapAddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapAddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Chain);
        Dqn_MapGet(&map, 3 /*hash*/);

        Dqn_MapErase(&map, 3 /*hash*/, Dqn_ZeroMem::No);
        DQN_TEST_ASSERT_MSG(test, map.size == 1, "size: %zu", map.size);
        DQN_TEST_ASSERT_MSG(test, map.count == 1, "count: %zu", map.count);
        DQN_TEST_ASSERT_MSG(test, map.chain_count == 0, "chain_count: %zu", map.chain_count);
        DQN_TEST_ASSERT_MSG(test, map.free_list != nullptr, "free_list: %p", map.free_list);

        DQN_TEST_ASSERT_MSG(test, map.free_list->hash == 3, "Entry should not be zeroed out on erase");
        DQN_TEST_ASSERT_MSG(test, map.free_list->value == 5, "Entry should not be zeroed out on erase");
        DQN_TEST_ASSERT_MSG(test, map.free_list->next == nullptr, "This should be the first and only entry in the free list");

        Dqn_MapEntry<int> *entry = Dqn_MapGet(&map, 4 /*hash*/);
        DQN_TEST_ASSERT_MSG(test, entry->hash == 4, "hash: %zu", entry->hash);
        DQN_TEST_ASSERT_MSG(test, entry->value == 6, "value: %d", entry->value);
        DQN_TEST_ASSERT_MSG(test, entry->next == nullptr, "next: %p", entry->next);

        Dqn_ArenaFree(&arena);
    }

    {
        DQN_TEST(test, "Add r-value item and erase it, zeroing the memory out");
        Dqn_Map<int>       map   = Dqn_MapInitWithArena<int>(&arena, 1);
        Dqn_MapAddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapAddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Chain);
        Dqn_MapGet(&map, 3 /*hash*/);

        Dqn_MapErase(&map, 3 /*hash*/, Dqn_ZeroMem::Yes);
        DQN_TEST_ASSERT_MSG(test, map.size == 1, "size: %zu", map.size);
        DQN_TEST_ASSERT_MSG(test, map.count == 1, "count: %zu", map.count);
        DQN_TEST_ASSERT_MSG(test, map.chain_count == 0, "chain_count: %zu", map.chain_count);
        DQN_TEST_ASSERT_MSG(test, map.free_list != nullptr, "free_list: %p", map.free_list);

        DQN_TEST_ASSERT_MSG(test, map.free_list->hash == 0, "Entry should be zeroed out on erase");
        DQN_TEST_ASSERT_MSG(test, map.free_list->value == 0, "Entry should be zeroed out on erase");
        DQN_TEST_ASSERT_MSG(test, map.free_list->next == nullptr, "This should be the first and only entry in the free list");

        Dqn_MapEntry<int> *entry = Dqn_MapGet(&map, 4 /*hash*/);
        DQN_TEST_ASSERT_MSG(test, entry->hash == 4, "hash: %zu", entry->hash);
        DQN_TEST_ASSERT_MSG(test, entry->value == 6, "value: %d", entry->value);
        DQN_TEST_ASSERT_MSG(test, entry->next == nullptr, "next: %p", entry->next);

        Dqn_ArenaFree(&arena);
    }

    // TODO(dqn): Test free list is chained correctly
    // TODO(dqn): Test deleting 'b' from the list in the situation [map] - [a]->[b], we currently only test deleting a
#endif // DQN_WITH_MAP
    return test;
}

Dqn_Test Dqn_Test_Intrinsics()
{
    Dqn_Test test = {};
    // TODO(dqn): We don't have meaningful tests here, but since
    // atomics/intrinsics are implemented using macros we ensure the macro was
    // written properly with these tests.

    DQN_TEST_GROUP(test, "Dqn_Atomic");
    {
        DQN_TEST(test, "Dqn_AtomicAddU32");
        Dqn_u32 val = 0;
        Dqn_AtomicAddU32(&val, 1);
        DQN_TEST_ASSERT_MSG(test, val == 1, "val: %u", val);
    }

    {
        DQN_TEST(test, "Dqn_AtomicAddU64");
        Dqn_u64 val = 0;
        Dqn_AtomicAddU64(&val, 1);
        DQN_TEST_ASSERT_MSG(test, val == 1, "val: %zu", val);
    }

    {
        DQN_TEST(test, "Dqn_AtomicSubU32");
        Dqn_u32 val = 1;
        Dqn_AtomicSubU32(&val, 1);
        DQN_TEST_ASSERT_MSG(test, val == 0, "val: %u", val);
    }

    {
        DQN_TEST(test, "Dqn_AtomicSubU64");
        Dqn_u64 val = 1;
        Dqn_AtomicSubU64(&val, 1);
        DQN_TEST_ASSERT_MSG(test, val == 0, "val: %zu", val);
    }

    {
        DQN_TEST(test, "Dqn_AtomicSetValue32");
        long a = 0;
        long b = 111;
        Dqn_AtomicSetValue32(&a, b);
        DQN_TEST_ASSERT_MSG(test, a == b, "a: %lu, b: %lu", a, b);
    }

    {
        DQN_TEST(test, "Dqn_AtomicSetValue64");
        Dqn_i64 a = 0;
        Dqn_i64 b = 111;
        Dqn_AtomicSetValue64(&a, b);
        DQN_TEST_ASSERT_MSG(test, a == b, "a: %I64i, b: %I64i", a, b);
    }

    {
        DQN_TEST(test, "Dqn_CPUClockCycle");
        Dqn_CPUClockCycle();
    }

    {
        DQN_TEST(test, "Dqn_CompilerReadBarrierAndCPUReadFence");
        Dqn_CompilerReadBarrierAndCPUReadFence;
    }

    {
        DQN_TEST(test, "Dqn_CompilerWriteBarrierAndCPUWriteFence");
        Dqn_CompilerWriteBarrierAndCPUWriteFence;
    }
    return test;
}

Dqn_Test Dqn_Test_Rect()
{
    Dqn_Test test = {};
#if defined(DQN_WITH_MATH)
    DQN_TEST_GROUP(test, "Dqn_Rect");
    // NOTE: Dqn_RectIntersection
    {
        {
            DQN_TEST(test, "No intersection");
            Dqn_Rect a  = Dqn_RectInitFromPosAndSize(Dqn_V2(0, 0), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_RectInitFromPosAndSize(Dqn_V2(200, 0), Dqn_V2(200, 200));
            Dqn_Rect ab = Dqn_RectIntersection(a, b);

            DQN_TEST_ASSERT_MSG(test,
                            ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 0 && ab.max.y == 0,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }

        {
            DQN_TEST(test, "A's min intersects B");
            Dqn_Rect a  = Dqn_RectInitFromPosAndSize(Dqn_V2(50, 50), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_RectInitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_RectIntersection(a, b);

            DQN_TEST_ASSERT_MSG(test,
                            ab.min.x == 50 && ab.min.y == 50 && ab.max.x == 100 && ab.max.y == 100,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }

        {
            DQN_TEST(test, "B's min intersects A");
            Dqn_Rect a  = Dqn_RectInitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_RectInitFromPosAndSize(Dqn_V2(50, 50), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_RectIntersection(a, b);

            DQN_TEST_ASSERT_MSG(test,
                            ab.min.x == 50 && ab.min.y == 50 && ab.max.x == 100 && ab.max.y == 100,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }

        {
            DQN_TEST(test, "A's max intersects B");
            Dqn_Rect a  = Dqn_RectInitFromPosAndSize(Dqn_V2(-50, -50), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_RectInitFromPosAndSize(Dqn_V2(  0,   0), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_RectIntersection(a, b);

            DQN_TEST_ASSERT_MSG(test,
                            ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 50 && ab.max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }

        {
            DQN_TEST(test, "B's max intersects A");
            Dqn_Rect a  = Dqn_RectInitFromPosAndSize(Dqn_V2(  0,   0), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_RectInitFromPosAndSize(Dqn_V2(-50, -50), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_RectIntersection(a, b);

            DQN_TEST_ASSERT_MSG(test,
                            ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 50 && ab.max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }


        {
            DQN_TEST(test, "B contains A");
            Dqn_Rect a  = Dqn_RectInitFromPosAndSize(Dqn_V2(25, 25), Dqn_V2( 25,  25));
            Dqn_Rect b  = Dqn_RectInitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_RectIntersection(a, b);

            DQN_TEST_ASSERT_MSG(test,
                            ab.min.x == 25 && ab.min.y == 25 && ab.max.x == 50 && ab.max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }

        {
            DQN_TEST(test, "A contains B");
            Dqn_Rect a  = Dqn_RectInitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_RectInitFromPosAndSize(Dqn_V2(25, 25), Dqn_V2( 25,  25));
            Dqn_Rect ab = Dqn_RectIntersection(a, b);

            DQN_TEST_ASSERT_MSG(test,
                            ab.min.x == 25 && ab.min.y == 25 && ab.max.x == 50 && ab.max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }

        {
            DQN_TEST(test, "A equals B");
            Dqn_Rect a  = Dqn_RectInitFromPosAndSize(Dqn_V2(0, 0), Dqn_V2(100, 100));
            Dqn_Rect b  = a;
            Dqn_Rect ab = Dqn_RectIntersection(a, b);

            DQN_TEST_ASSERT_MSG(test,
                            ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 100 && ab.max.y == 100,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
        }
    }
#endif // DQN_WITH_MATH
    return test;
}

Dqn_Test Dqn_Test_PerfCounter()
{
    Dqn_Test test = {};
    DQN_TEST_GROUP(test, "Dqn_PerfCounter");
    {
        DQN_TEST(test, "Dqn_PerfCounterNow");
        Dqn_u64 result = Dqn_PerfCounterNow();
        DQN_TEST_ASSERT(test, result != 0);
    }

    {
        DQN_TEST(test, "Consecutive ticks are ordered");
        Dqn_u64 a = Dqn_PerfCounterNow();
        Dqn_u64 b = Dqn_PerfCounterNow();
        DQN_TEST_ASSERT_MSG(test, b >= a, "a: %zu, b: %zu", a, b);
    }

    {
        DQN_TEST(test, "Ticks to time are a correct order of magnitude");
        Dqn_u64 a = Dqn_PerfCounterNow();
        Dqn_u64 b = Dqn_PerfCounterNow();

        Dqn_f64 s       = Dqn_PerfCounterS(a, b);
        Dqn_f64 ms      = Dqn_PerfCounterMs(a, b);
        Dqn_f64 micro_s = Dqn_PerfCounterMicroS(a, b);
        Dqn_f64 ns      = Dqn_PerfCounterNs(a, b);
        DQN_TEST_ASSERT_MSG(test, s <= ms, "s: %f, ms: %f", s, ms);
        DQN_TEST_ASSERT_MSG(test, ms <= micro_s, "ms: %f, micro_s: %f", ms, micro_s);
        DQN_TEST_ASSERT_MSG(test, micro_s <= ns, "micro_s: %f, ns: %f", micro_s, ns);
    }
    return test;
}

Dqn_Test Dqn_Test_OS()
{
    Dqn_Test test = {};
    DQN_TEST_GROUP(test, "Dqn_OS");
    {
        DQN_TEST(test, "Generate secure RNG bytes with nullptr");
        Dqn_b32 result = Dqn_OSSecureRNGBytes(nullptr, 1);
        DQN_TEST_ASSERT(test, result == false);
    }

    {
        DQN_TEST(test, "Generate secure RNG bytes with -1 size");
        char buf[1];
        Dqn_b32 result = Dqn_OSSecureRNGBytes(buf, -1);
        DQN_TEST_ASSERT(test, result == false);
    }

    {
        DQN_TEST(test, "Generate secure RNG 32 bytes");
        char const ZERO[32] = {};
        char       buf[32]  = {};
        Dqn_b32 result = Dqn_OSSecureRNGBytes(buf, Dqn_ArrayCountI(buf));
        DQN_TEST_ASSERT(test, result);
        DQN_TEST_ASSERT(test, DQN_MEMCMP(buf, ZERO, Dqn_ArrayCount(buf)) != 0);
    }

    {
        DQN_TEST(test, "Generate secure RNG 0 bytes");
        char buf[32] = {};
        buf[0] = 'Z';
        Dqn_b32 result = Dqn_OSSecureRNGBytes(buf, 0);
        DQN_TEST_ASSERT(test, result);
        DQN_TEST_ASSERT(test, buf[0] == 'Z');
    }

    {
        DQN_TEST(test, "Query executable directory");
        Dqn_Arena arena = {};
        Dqn_String result = Dqn_OSExecutableDirectory(&arena);
        DQN_TEST_ASSERT(test, Dqn_StringIsValid(result));
        DQN_TEST_ASSERT_MSG(test, Dqn_FileDirExists(result), "result(%zu): %.*s", result.size, DQN_STRING_FMT(result));
        Dqn_ArenaFree(&arena);
    }
    return test;
}

Dqn_Test Dqn_Test_Str()
{
    Dqn_Test test = {};
    DQN_TEST_GROUP(test, "Dqn_Str");
    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_CStringToI64
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST(test, "To I64: Convert nullptr");
        Dqn_i64 result = Dqn_CStringToI64(nullptr);
        DQN_TEST_ASSERT(test, result == 0);
    }

    {
        DQN_TEST(test, "To I64: Convert empty string");
        Dqn_i64 result = Dqn_CStringToI64("");
        DQN_TEST_ASSERT(test, result == 0);
    }

    {
        DQN_TEST(test, "To I64: Convert \"1\"");
        Dqn_i64 result = Dqn_CStringToI64("1");
        DQN_TEST_ASSERT(test, result == 1);
    }

    {
        DQN_TEST(test, "To I64: Convert \"-0\"");
        Dqn_i64 result = Dqn_CStringToI64("-0");
        DQN_TEST_ASSERT(test, result == 0);
    }

    {
        DQN_TEST(test, "To I64: Convert \"-1\"");
        Dqn_i64 result = Dqn_CStringToI64("-1");
        DQN_TEST_ASSERT(test, result == -1);
    }

    {
        DQN_TEST(test, "To I64: Convert \"1.2\"");
        Dqn_i64 result = Dqn_CStringToI64("1.2");
        DQN_TEST_ASSERT(test, result == 1);
    }

    {
        DQN_TEST(test, "To I64: Convert \"1,234\"");
        Dqn_i64 result = Dqn_CStringToI64("1,234");
        DQN_TEST_ASSERT(test, result == 1234);
    }

    {
        DQN_TEST(test, "To I64: Convert \"1,2\"");
        Dqn_i64 result = Dqn_CStringToI64("1,2");
        DQN_TEST_ASSERT(test, result == 12);
    }

    {
        DQN_TEST(test, "To I64: Convert \"12a3\"");
        Dqn_i64 result = Dqn_CStringToI64("12a3");
        DQN_TEST_ASSERT(test, result == 12);
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_CStringToU64
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST(test, "To U64: Convert nullptr");
        Dqn_u64 result = Dqn_CStringToU64(nullptr);
        DQN_TEST_ASSERT_MSG(test, result == 0, "result: %zu", result);
    }

    {
        DQN_TEST(test, "To U64: Convert empty string");
        Dqn_u64 result = Dqn_CStringToU64("");
        DQN_TEST_ASSERT_MSG(test, result == 0, "result: %zu", result);
    }

    {
        DQN_TEST(test, "To U64: Convert \"1\"");
        Dqn_u64 result = Dqn_CStringToU64("1");
        DQN_TEST_ASSERT_MSG(test, result == 1, "result: %zu", result);
    }

    {
        DQN_TEST(test, "To U64: Convert \"-0\"");
        Dqn_u64 result = Dqn_CStringToU64("-0");
        DQN_TEST_ASSERT_MSG(test, result == 0, "result: %zu", result);
    }

    {
        DQN_TEST(test, "To U64: Convert \"-1\"");
        Dqn_u64 result = Dqn_CStringToU64("-1");
        DQN_TEST_ASSERT_MSG(test, result == 0, "result: %zu", result);
    }

    {
        DQN_TEST(test, "To U64: Convert \"1.2\"");
        Dqn_u64 result = Dqn_CStringToU64("1.2");
        DQN_TEST_ASSERT_MSG(test, result == 1, "result: %zu", result);
    }

    {
        DQN_TEST(test, "To U64: Convert \"1,234\"");
        Dqn_u64 result = Dqn_CStringToU64("1,234");
        DQN_TEST_ASSERT_MSG(test, result == 1234, "result: %zu", result);
    }

    {
        DQN_TEST(test, "To U64: Convert \"1,2\"");
        Dqn_u64 result = Dqn_CStringToU64("1,2");
        DQN_TEST_ASSERT_MSG(test, result == 12, "result: %zu", result);
    }

    {
        DQN_TEST(test, "To U64: Convert \"12a3\"");
        Dqn_u64 result = Dqn_CStringToU64("12a3");
        DQN_TEST_ASSERT_MSG(test, result == 12, "result: %zu", result);
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_CStringFind
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST(test, "Find: String (char) is not in buffer");
        char const buf[]  = "836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55";
        char const find[] = "2";
        char const *result = Dqn_CStringFind(buf, find, Dqn_CharCountI(buf), Dqn_CharCountI(find));
        DQN_TEST_ASSERT(test, result == nullptr);
    }

    {
        DQN_TEST(test, "Find: String (char) is in buffer");
        char const buf[]  = "836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55";
        char const find[] = "6";
        char const *result = Dqn_CStringFind(buf, find, Dqn_CharCountI(buf), Dqn_CharCountI(find));
        DQN_TEST_ASSERT(test, result != nullptr);
        DQN_TEST_ASSERT(test, result[0] == '6' && result[1] == 'a');
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_CStringFileNameFromPath
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST(test, "File name from Windows path");
        Dqn_isize file_name_size = 0;
        char const buf[]  = "C:\\ABC\\test.exe";
        char const *result = Dqn_CStringFileNameFromPath(buf, Dqn_CharCountI(buf), &file_name_size);
        DQN_TEST_ASSERT_MSG(test, file_name_size == 8, "size: %I64d", file_name_size);
        DQN_TEST_ASSERT_MSG(test, Dqn_StringInit(result, file_name_size) == DQN_STRING("test.exe"), "%.*s", (int)file_name_size, result);
    }

    {
        DQN_TEST(test, "File name from Linux path");
        Dqn_isize file_name_size = 0;
        char const buf[]  = "/ABC/test.exe";
        char const *result = Dqn_CStringFileNameFromPath(buf, Dqn_CharCountI(buf), &file_name_size);
        DQN_TEST_ASSERT_MSG(test, file_name_size == 8, "size: %I64d", file_name_size);
        DQN_TEST_ASSERT_MSG(test, Dqn_StringInit(result, file_name_size) == DQN_STRING("test.exe"), "%.*s", (int)file_name_size, result);
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_CStringTrimPrefix
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST(test, "Trim prefix");
        char const  prefix[]     = "@123";
        char const  buf[]        = "@123string";
        Dqn_isize   trimmed_size = 0;
        char const *result = Dqn_CStringTrimPrefix(buf, Dqn_CharCountI(buf), prefix, Dqn_CharCountI(prefix), &trimmed_size);
        DQN_TEST_ASSERT_MSG(test, trimmed_size == 6, "size: %I64d", trimmed_size);
        DQN_TEST_ASSERT_MSG(test, Dqn_StringInit(result, trimmed_size) == DQN_STRING("string"), "%.*s", (int)trimmed_size, result);
    }

    {
        DQN_TEST(test, "Trim prefix, nullptr trimmed size");
        char const  prefix[]     = "@123";
        char const  buf[]        = "@123string";
        char const *result = Dqn_CStringTrimPrefix(buf, Dqn_CharCountI(buf), prefix, Dqn_CharCountI(prefix), nullptr);
        DQN_TEST_ASSERT(test, result);
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_CStringIsAllDigits
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST(test, "Is all digits fails on non-digit string");
        char const buf[]  = "@123string";
        Dqn_b32    result = Dqn_CStringIsAllDigits(buf, Dqn_CharCountI(buf));
        DQN_TEST_ASSERT(test, result == false);
    }

    {
        DQN_TEST(test, "Is all digits fails on nullptr");
        Dqn_b32 result = Dqn_CStringIsAllDigits(nullptr, 0);
        DQN_TEST_ASSERT(test, result == false);
    }

    {
        DQN_TEST(test, "Is all digits fails on nullptr w/ size");
        Dqn_b32 result = Dqn_CStringIsAllDigits(nullptr, 1);
        DQN_TEST_ASSERT(test, result == false);
    }

    {
        DQN_TEST(test, "Is all digits fails on 0 size w/ string");
        char const buf[]  = "@123string";
        Dqn_b32    result = Dqn_CStringIsAllDigits(buf, 0);
        DQN_TEST_ASSERT(test, result == false);
    }

    {
        DQN_TEST(test, "Is all digits success");
        char const buf[]  = "23";
        Dqn_b32    result = Dqn_CStringIsAllDigits(buf, Dqn_CharCountI(buf));
        DQN_TEST_ASSERT(test, DQN_CAST(bool)result == true);
    }

    {
        DQN_TEST(test, "Is all digits fails on whitespace");
        char const buf[]  = "23 ";
        Dqn_b32    result = Dqn_CStringIsAllDigits(buf, Dqn_CharCountI(buf));
        DQN_TEST_ASSERT(test, DQN_CAST(bool)result == false);
    }
    return test;
}

Dqn_Test Dqn_Test_String()
{
    Dqn_Test test = {};
    DQN_TEST_GROUP(test, "Dqn_String");
    {
        DQN_TEST(test, "Initialise with string literal w/ macro");
        Dqn_String string = DQN_STRING("AB");
        DQN_TEST_ASSERT_MSG(test, string.size == 2, "size: %I64d", string.size);
        DQN_TEST_ASSERT_MSG(test, string.cap  == 2, "cap: %I64d", string.cap);
        DQN_TEST_ASSERT_MSG(test, string.str[0] == 'A', "string[0]: %c", string.str[0]);
        DQN_TEST_ASSERT_MSG(test, string.str[1] == 'B', "string[1]: %c", string.str[1]);
    }

    {
        DQN_TEST(test, "Initialise with format string");
        Dqn_Arena arena  = {};
        Dqn_String         string = Dqn_StringFmt(&arena, "%s", "AB");
        DQN_TEST_ASSERT_MSG(test, string.size == 2, "size: %I64d", string.size);
        DQN_TEST_ASSERT_MSG(test, string.cap == 2, "cap: %I64d", string.cap);
        DQN_TEST_ASSERT_MSG(test, string.str[0] == 'A', "string[0]: %c", string.str[0]);
        DQN_TEST_ASSERT_MSG(test, string.str[1] == 'B', "string[1]: %c", string.str[1]);
        DQN_TEST_ASSERT_MSG(test, string.str[2] == 0, "string[2]: %c", string.str[2]);
        Dqn_ArenaFree(&arena);
    }

    {
        DQN_TEST(test, "Copy string");
        Dqn_Arena arena  = {};
        Dqn_String         string = DQN_STRING("AB");
        Dqn_String         copy   = Dqn_StringCopy(string, &arena);
        DQN_TEST_ASSERT_MSG(test, copy.size == 2, "size: %I64d", copy.size);
        DQN_TEST_ASSERT_MSG(test, copy.cap  == 2, "cap: %I64d", copy.cap);
        DQN_TEST_ASSERT_MSG(test, copy.str[0] == 'A', "copy[0]: %c", copy.str[0]);
        DQN_TEST_ASSERT_MSG(test, copy.str[1] == 'B', "copy[1]: %c", copy.str[1]);
        DQN_TEST_ASSERT_MSG(test, copy.str[2] ==  0,  "copy[2]: %c", copy.str[2]);
        Dqn_ArenaFree(&arena);
    }

    {
        DQN_TEST(test, "Trim whitespace around string");
        Dqn_String         string = Dqn_StringTrimWhitespaceAround(DQN_STRING(" AB "));
        DQN_TEST_ASSERT_MSG(test, string.size == 2, "size: %I64d", string.size);
        DQN_TEST_ASSERT_MSG(test, string.cap  == 2, "cap: %I64d", string.cap);
        DQN_TEST_ASSERT_MSG(test, string.str[0] == 'A', "string[0]: %c", string.str[0]);
        DQN_TEST_ASSERT_MSG(test, string.str[1] == 'B', "string[1]: %c", string.str[1]);
        DQN_TEST_ASSERT_MSG(test, string.str[2] == ' ', "string[1]: %c", string.str[1]);
    }

    {
        DQN_TEST(test, "Allocate string from arena");
        Dqn_Arena arena  = {};
        Dqn_String         string = Dqn_StringAllocate(&arena, 2, Dqn_ZeroMem::No);
        DQN_TEST_ASSERT_MSG(test, string.size == 0, "size: %I64d", string.size);
        DQN_TEST_ASSERT_MSG(test, string.cap  == 2, "cap: %I64d", string.cap);
        Dqn_ArenaFree(&arena);
    }

    {
        DQN_TEST(test, "Append to allocated string");
        Dqn_Arena arena  = {};
        Dqn_String         string = Dqn_StringAllocate(&arena, 2, Dqn_ZeroMem::No);
        Dqn_StringAppendFmt(&string, "%c", 'A');
        Dqn_StringAppendFmt(&string, "%c", 'B');
        DQN_TEST_ASSERT_MSG(test, string.size   ==  2,  "size: %I64d", string.size);
        DQN_TEST_ASSERT_MSG(test, string.cap    ==  2,  "cap: %I64d", string.cap);
        DQN_TEST_ASSERT_MSG(test, string.str[0] == 'A', "string[0]: %c", string.str[0]);
        DQN_TEST_ASSERT_MSG(test, string.str[1] == 'B', "string[1]: %c", string.str[1]);
        DQN_TEST_ASSERT_MSG(test, string.str[2] ==  0,  "string[2]: %c", string.str[2]);
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_CStringTrim[Prefix/Suffix]
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST(test, "Trim prefix with matching prefix");
        Dqn_String input  = DQN_STRING("nft/abc");
        Dqn_String result = Dqn_StringTrimPrefix(input, DQN_STRING("nft/"));
        DQN_TEST_ASSERT_MSG(test, result == DQN_STRING("abc"), "%.*s", DQN_STRING_FMT(result));
    }

    {
        DQN_TEST(test, "Trim prefix with non matching prefix");
        Dqn_String input  = DQN_STRING("nft/abc");
        Dqn_String result = Dqn_StringTrimPrefix(input, DQN_STRING(" ft/"));
        DQN_TEST_ASSERT_MSG(test, result == input, "%.*s", DQN_STRING_FMT(result));
    }

    {
        DQN_TEST(test, "Trim suffix with matching suffix");
        Dqn_String input  = DQN_STRING("nft/abc");
        Dqn_String result = Dqn_StringTrimSuffix(input, DQN_STRING("abc"));
        DQN_TEST_ASSERT_MSG(test, result == DQN_STRING("nft/"), "%.*s", DQN_STRING_FMT(result));
    }

    {
        DQN_TEST(test, "Trim suffix with non matching suffix");
        Dqn_String input  = DQN_STRING("nft/abc");
        Dqn_String result = Dqn_StringTrimSuffix(input, DQN_STRING("ab"));
        DQN_TEST_ASSERT_MSG(test, result == input, "%.*s", DQN_STRING_FMT(result));
    }


    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_StringIsAllDigits
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST(test, "Is all digits fails on non-digit string");
        Dqn_b32 result = Dqn_StringIsAllDigits(DQN_STRING("@123string"));
        DQN_TEST_ASSERT(test, result == false);
    }

    {
        DQN_TEST(test, "Is all digits fails on nullptr");
        Dqn_b32 result = Dqn_StringIsAllDigits(Dqn_StringInit(nullptr, 0));
        DQN_TEST_ASSERT(test, result == false);
    }

    {
        DQN_TEST(test, "Is all digits fails on nullptr w/ size");
        Dqn_b32 result = Dqn_StringIsAllDigits(Dqn_StringInit(nullptr, 1));
        DQN_TEST_ASSERT(test, result == false);
    }

    {
        DQN_TEST(test, "Is all digits fails on string w/ 0 size");
        char const buf[]  = "@123string";
        Dqn_b32    result = Dqn_StringIsAllDigits(Dqn_StringInit(buf, 0));
        DQN_TEST_ASSERT(test, result == false);
    }

    {
        DQN_TEST(test, "Is all digits success");
        Dqn_b32 result = Dqn_StringIsAllDigits(DQN_STRING("23"));
        DQN_TEST_ASSERT(test, DQN_CAST(bool)result == true);
    }

    {
        DQN_TEST(test, "Is all digits fails on whitespace");
        Dqn_b32 result = Dqn_StringIsAllDigits(DQN_STRING("23 "));
        DQN_TEST_ASSERT(test, DQN_CAST(bool)result == false);
    }
    return test;
}

Dqn_Test Dqn_Test_TicketMutex()
{
    Dqn_Test test = {};
    DQN_TEST_GROUP(test, "Dqn_TicketMutex");
    {
        // TODO: We don't have a meaningful test but since atomics are
        // implemented with a macro this ensures that we test that they are
        // written correctly.
        DQN_TEST(test, "Ticket mutex start and stop");
        Dqn_TicketMutex mutex = {};
        Dqn_TicketMutexBegin(&mutex);
        Dqn_TicketMutexEnd(&mutex);
        DQN_TEST_ASSERT(test, mutex.ticket == mutex.serving);
    }

    {
        DQN_TEST(test, "Ticket mutex start and stop w/ advanced API");
        Dqn_TicketMutex mutex = {};
        unsigned int ticket_a = Dqn_TicketMutexMakeTicket(&mutex);
        unsigned int ticket_b = Dqn_TicketMutexMakeTicket(&mutex);
        DQN_TEST_ASSERT(test, DQN_CAST(bool)Dqn_TicketMutexCanLock(&mutex, ticket_b) == false);
        DQN_TEST_ASSERT(test, DQN_CAST(bool)Dqn_TicketMutexCanLock(&mutex, ticket_a) == true);

        Dqn_TicketMutexBeginTicket(&mutex, ticket_a);
        Dqn_TicketMutexEnd(&mutex);
        Dqn_TicketMutexBeginTicket(&mutex, ticket_b);
        Dqn_TicketMutexEnd(&mutex);

        DQN_TEST_ASSERT(test, mutex.ticket == mutex.serving);
        DQN_TEST_ASSERT(test, mutex.ticket == ticket_b + 1);
    }
    return test;
}

Dqn_Test Dqn_Test_Win()
{
    Dqn_Test test = {};
#if defined(DQN_OS_WIN32)
    DQN_TEST_GROUP(test, "Dqn_Win");
    {
        DQN_TEST(test, "UTF8 to wide character size calculation");
        int result = Dqn_WinUTF8ToWCharSizeRequired(DQN_STRING("String"));
        DQN_TEST_ASSERT_MSG(test, result == 6, "Size returned: %d. This size should be including the null-terminator", result);
    }

    {
        DQN_TEST(test, "UTF8 to wide character");
        Dqn_Arena arena = {};
        Dqn_String const INPUT   = DQN_STRING("String");
        int size_required        = Dqn_WinUTF8ToWCharSizeRequired(INPUT);
        wchar_t *string          = Dqn_ArenaNewArray(&arena, wchar_t, size_required + 1, Dqn_ZeroMem::No);

        // Fill the string with error sentinels, which ensures the string is zero terminated
        DQN_MEMSET(string, 'Z', size_required + 1);

        int size_returned = Dqn_WinUTF8ToWChar(INPUT, string, size_required + 1);
        wchar_t const EXPECTED[] = {L'S', L't', L'r', L'i', L'n', L'g', 0};

        DQN_TEST_ASSERT_MSG(test, size_required == size_returned, "string_size: %d, result: %d", size_required, size_returned);
        DQN_TEST_ASSERT_MSG(test, size_returned == Dqn_ArrayCount(EXPECTED) - 1, "string_size: %d, expected: %zu", size_returned, sizeof(EXPECTED));
        DQN_TEST_ASSERT(test, DQN_MEMCMP(EXPECTED, string, sizeof(EXPECTED)) == 0);
        Dqn_ArenaFree(&arena);
    }

    {
        DQN_TEST(test, "Wide char to UTF8 size calculation");
        int result = Dqn_WinWCharToUTF8SizeRequired(DQN_STRINGW(L"String"));
        DQN_TEST_ASSERT_MSG(test, result == 6, "Size returned: %d. This size should be including the null-terminator", result);
    }

    {
        DQN_TEST(test, "Wide char to UTF8");
        Dqn_Arena arena         = {};
        Dqn_StringW const INPUT = DQN_STRINGW(L"String");
        int size_required       = Dqn_WinWCharToUTF8SizeRequired(INPUT);
        char *string            = Dqn_ArenaNewArray(&arena, char, size_required + 1, Dqn_ZeroMem::No);

        // Fill the string with error sentinels, which ensures the string is zero terminated
        DQN_MEMSET(string, 'Z', size_required + 1);

        int size_returned = Dqn_WinWCharToUTF8(INPUT, string, size_required + 1);
        char const EXPECTED[] = {'S', 't', 'r', 'i', 'n', 'g', 0};

        DQN_TEST_ASSERT_MSG(test, size_required == size_returned, "string_size: %d, result: %d", size_required, size_returned);
        DQN_TEST_ASSERT_MSG(test, size_returned == Dqn_ArrayCount(EXPECTED) - 1, "string_size: %d, expected: %zu", size_returned, sizeof(EXPECTED));
        DQN_TEST_ASSERT(test, DQN_MEMCMP(EXPECTED, string, sizeof(EXPECTED)) == 0);
        Dqn_ArenaFree(&arena);
    }
#endif // DQN_OS_WIN32

    return test;
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

void Dqn_Test__KeccakDispatch(Dqn_Test *test, int hash_type, Dqn_String input)
{
#if defined(DQN_KECCAK_H)
    Dqn_ThreadScratch scratch = Dqn_ThreadGetScratch();
    Dqn_String input_hex = Dqn_HexBytesToHexStringArena(input.str, input.size, scratch.arena);

    switch(hash_type)
    {
        case Hash_SHA3_224:
        {
            Dqn_KeccakBytes28 hash = Dqn_Keccak_SHA3_224_StringToBytes28(input);
            Dqn_KeccakBytes28 expect;
            FIPS202_SHA3_224(DQN_CAST(u8 *)input.str, input.size, (u8 *)expect.data);
            DQN_TEST_ASSERT_MSG((*test),
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
            DQN_TEST_ASSERT_MSG((*test),
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
            DQN_TEST_ASSERT_MSG((*test),
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
            DQN_TEST_ASSERT_MSG((*test),
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
            DQN_TEST_ASSERT_MSG((*test),
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
            DQN_TEST_ASSERT_MSG((*test),
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
            DQN_TEST_ASSERT_MSG((*test),
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
            DQN_TEST_ASSERT_MSG((*test),
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

Dqn_Test Dqn_Test_Keccak()
{
    Dqn_Test test = {};
#if defined(DQN_KECCAK_H)
    Dqn_String const INPUTS[] = {
        DQN_STRING("abc"),
        DQN_STRING(""),
        DQN_STRING("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
        DQN_STRING("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmno"
                   "pqrstnopqrstu"),
    };

    DQN_TEST_GROUP(test, "Dqn_Keccak");
    for (int hash_type = 0; hash_type < Hash_Count; hash_type++)
    {
        pcg32_random_t rng = {};
        pcg32_srandom_r(&rng, 0xd48e'be21'2af8'733d, 0x3f89'3bd2'd6b0'4eef);

        for (Dqn_String input : INPUTS)
        {
            DQN_TEST(test, "%.*s - Input: %.*s", DQN_STRING_FMT(DQN_TESTS__HASH_STRING[hash_type]), DQN_MIN(input.size, 54), input.str);
            Dqn_Test__KeccakDispatch(&test, hash_type, input);
        }

        DQN_TEST(test, "%.*s - Deterministic random inputs", DQN_STRING_FMT(DQN_TESTS__HASH_STRING[hash_type]));
        for (int index = 0; index < 128; index++)
        {
            char    src[4096] = {};
            Dqn_u32 src_size  = pcg32_boundedrand_r(&rng, sizeof(src));

            for (int src_index = 0; src_index < src_size; src_index++)
                src[src_index] = pcg32_boundedrand_r(&rng, 255);

            Dqn_String input = Dqn_StringInit(src, src_size);
            Dqn_Test__KeccakDispatch(&test, hash_type, input);
        }
    }
#endif // DQN_KECCAK_H
    return test;
}

void Dqn_Test_RunSuite()
{
    Dqn_Test tests[]
    {
        Dqn_Test_Array(),
        Dqn_Test_File(),
        Dqn_Test_FixedArray(),
        Dqn_Test_FixedString(),
        Dqn_Test_Hex(),
        Dqn_Test_Intrinsics(),
        Dqn_Test_M4(),
        Dqn_Test_DSMap(),
        Dqn_Test_Map(),
        Dqn_Test_Rect(),
        Dqn_Test_PerfCounter(),
        Dqn_Test_OS(),
        Dqn_Test_Keccak(),
        Dqn_Test_Str(),
        Dqn_Test_String(),
        Dqn_Test_TicketMutex(),
        Dqn_Test_Win(),
    };

    int total_tests = 0;
    int total_good_tests = 0;
    for (Dqn_Test &test : tests)
    {
        total_tests += test.num_tests_in_group;
        total_good_tests += test.num_tests_ok_in_group;
    }

    fprintf(stdout, "Summary: %d/%d tests succeeded\n", total_good_tests, total_tests);
}

#if defined(DQN_TEST_WITH_MAIN)
int main(int argc, char *argv[])
{
    (void)argv; (void)argc;
    Dqn_Test_RunSuite();
    return 0;
}
#endif

