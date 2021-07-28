// -------------------------------------------------------------------------------------------------
//
// NOTE: Preprocessor Config
//
// -------------------------------------------------------------------------------------------------
/*
#define DQN_TEST_WITH_MAIN      Define this to enable the main function and allow standalone compiling
                                and running of the file.
#define DQN_TEST_NO_ANSI_COLORS Define this to disable any ANSI terminal color codes from output
*/

#if defined(DQN_TEST_WITH_MAIN)
    #define DQN_IMPLEMENTATION
    #include "Dqn.h"
#endif

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

#define DQN_TEST_START_SCOPE(testing_state, test_name)                                                                 \
    DQN_DEFER                                                                                                          \
    {                                                                                                                  \
        if (testing_state.test.fail_expr.size == 0) testing_state.num_tests_ok_in_group++;                             \
        Dqn_TestState_PrintResult(&testing_state.test);                                                                \
        Dqn_ArenaAllocator_ResetUsage(&testing_state.arena, Dqn_ZeroMem::No);                                          \
        testing_state.test = {};                                                                                       \
    };                                                                                                                 \
    testing_state.test.name          = DQN_STRING(test_name);                                                          \
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
        testing_state.test.fail_msg  = Dqn_String_Fmt(&testing_state.arena, msg, ##__VA_ARGS__);               \
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
    remaining_size       = DQN_M_MAX(remaining_size, 0);
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
    remaining_size = DQN_M_MAX(remaining_size, 0);

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

void Dqn_Test_Allocator()
{
#if 0
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Allocator");

    // NOTE: Various allocator test
    {
        {
            DQN_TEST_START_SCOPE(testing_state, "HeapAllocator - Allocate Small");
            Dqn_Allocator allocator = Dqn_Allocator_InitWithHeap();
            char const EXPECT[]     = "hello_world";
            char *buf               = DQN_CAST(char *)Dqn_Allocator_Allocate(&allocator, Dqn_ArrayCount(EXPECT), alignof(char), Dqn_ZeroMem::Yes);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
            memcpy(buf, EXPECT, Dqn_ArrayCount(EXPECT));
            DQN_TEST_EXPECT_MSG(testing_state, memcmp(EXPECT, buf, Dqn_ArrayCount(EXPECT)) == 0, "buf: %s, expect: %s", buf, EXPECT);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "XHeapAllocator - Allocate Small");
            Dqn_Allocator allocator = Dqn_Allocator_InitWithXHeap();
            char const EXPECT[]     = "hello_world";
            char *buf               = DQN_CAST(char *)Dqn_Allocator_Allocate(&allocator, Dqn_ArrayCount(EXPECT), alignof(char), Dqn_ZeroMem::Yes);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
            memcpy(buf, EXPECT, Dqn_ArrayCount(EXPECT));
            DQN_TEST_EXPECT_MSG(testing_state, memcmp(EXPECT, buf, Dqn_ArrayCount(EXPECT)) == 0, "buf: %s, expect: %s", buf, EXPECT);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "ArenaAllocator - Allocate Small");
            Dqn_ArenaAllocator arena = Dqn_ArenaAllocator_InitWithNewAllocator(Dqn_Allocator_InitWithHeap(), 0, nullptr);
            Dqn_Allocator allocator  = Dqn_Allocator_InitWithArena(&arena);
            char const EXPECT[]     = "hello_world";
            char *buf               = DQN_CAST(char *)Dqn_Allocator_Allocate(&allocator, Dqn_ArrayCount(EXPECT), alignof(char), Dqn_ZeroMem::Yes);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
            memcpy(buf, EXPECT, Dqn_ArrayCount(EXPECT));
            DQN_TEST_EXPECT_MSG(testing_state, memcmp(EXPECT, buf, Dqn_ArrayCount(EXPECT)) == 0, "buf: %s, expect: %s", buf, EXPECT);
        }
    }

    // NOTE: Alignment Test
    {
        Dqn_u8 const ALIGNMENT3 = 4;
        Dqn_u8 const NUM_BYTES  = sizeof(Dqn_u32);
        {
            DQN_TEST_START_SCOPE(testing_state, "HeapAllocator - Align to 32 bytes");
            Dqn_Allocator allocator = Dqn_Allocator_InitWithHeap();
            auto *buf               = DQN_CAST(Dqn_u32 *)Dqn_Allocator_Allocate(&allocator, NUM_BYTES, ALIGNMENT3, Dqn_ZeroMem::Yes);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
            int buf_mod_alignment = DQN_CAST(int)(DQN_CAST(uintptr_t)buf % ALIGNMENT3);
            DQN_TEST_EXPECT_MSG(testing_state, buf_mod_alignment == 0, "buf_mod_alignment: %d", buf_mod_alignment);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "XHeapAllocator - Align to 32 bytes");
            Dqn_Allocator allocator = Dqn_Allocator_InitWithXHeap();
            auto *buf               = DQN_CAST(Dqn_u32 *)Dqn_Allocator_Allocate(&allocator, NUM_BYTES, ALIGNMENT3, Dqn_ZeroMem::Yes);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
            int buf_mod_alignment = DQN_CAST(int)(DQN_CAST(uintptr_t)buf % ALIGNMENT3);
            DQN_TEST_EXPECT_MSG(testing_state, buf_mod_alignment == 0, "buf_mod_alignment: %d", buf_mod_alignment);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "ArenaAllocator - Align to 32 bytes");
            Dqn_ArenaAllocator arena = {};
            if (arena.backup_allocator.type == Dqn_AllocatorType::Null)
                arena.backup_allocator = Dqn_Allocator_InitWithHeap();

            Dqn_Allocator allocator = Dqn_Allocator_InitWithArena(&arena);
            auto *buf               = DQN_CAST(Dqn_u32 *)Dqn_Allocator_Allocate(&allocator, NUM_BYTES, ALIGNMENT3, Dqn_ZeroMem::Yes);
            int buf_mod_alignment = DQN_CAST(int)(DQN_CAST(uintptr_t)buf % ALIGNMENT3);
            DQN_TEST_EXPECT_MSG(testing_state, buf_mod_alignment == 0, "buf_mod_alignment: %d", buf_mod_alignment);
        }
    }

    // NOTE: Dqn_PointerMetadata tests
    {
        Dqn_u8 const ALIGNMENT3 = 4;
        Dqn_u8 const NUM_BYTES  = 4;
        Dqn_u8 const MAX_OFFSET = (ALIGNMENT3 - 1) + sizeof(Dqn_PointerMetadata);
        {
            DQN_TEST_START_SCOPE(testing_state, "HeapAllocator - Allocation metadata initialised");
            Dqn_Allocator allocator = Dqn_Allocator_InitWithHeap();
            char *buf               = DQN_CAST(char *)Dqn_Allocator_Allocate(&allocator, NUM_BYTES, ALIGNMENT3, Dqn_ZeroMem::Yes);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
            Dqn_PointerMetadata metadata = Dqn_PointerMetadata_Get(buf);
            DQN_TEST_EXPECT_MSG(testing_state, metadata.alignment == ALIGNMENT3, "metadata.alignment: %u, ALIGNMENT3: %u", metadata.alignment, ALIGNMENT3);
            DQN_TEST_EXPECT_MSG(testing_state, metadata.offset <= MAX_OFFSET, "metadata.offset: %u, MAX_OFFSET: %u", metadata.offset, MAX_OFFSET);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "XHeapAllocator - Allocation metadata initialised");
            Dqn_Allocator allocator = Dqn_Allocator_InitWithXHeap();
            char *buf               = DQN_CAST(char *)Dqn_Allocator_Allocate(&allocator, NUM_BYTES, ALIGNMENT3, Dqn_ZeroMem::Yes);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
            Dqn_PointerMetadata metadata = Dqn_PointerMetadata_Get(buf);
            DQN_TEST_EXPECT_MSG(testing_state, metadata.alignment == ALIGNMENT3, "metadata.alignment: %u, ALIGNMENT3: %u", metadata.alignment, ALIGNMENT3);
            DQN_TEST_EXPECT_MSG(testing_state, metadata.offset <= MAX_OFFSET, "metadata.offset: %u, MAX_OFFSET: %u", metadata.offset, MAX_OFFSET);
        }
    }
#endif
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

void Dqn_Test_FixedArray()
{
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
}

void Dqn_Test_FixedString()
{
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
}

void Dqn_Test_M4()
{
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
}

void Dqn_Test_Map()
{
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
}

void Dqn_Test_Str()
{
    Dqn_TestingState testing_state = {};
    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_Str_ToI64
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Str_ToI64");
        {
            DQN_TEST_START_SCOPE(testing_state, "Convert nullptr");
            Dqn_i64 result = Dqn_Str_ToI64(nullptr);
            DQN_TEST_EXPECT(testing_state, result == 0);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert empty string");
            Dqn_i64 result = Dqn_Str_ToI64("");
            DQN_TEST_EXPECT(testing_state, result == 0);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"1\"");
            Dqn_i64 result = Dqn_Str_ToI64("1");
            DQN_TEST_EXPECT(testing_state, result == 1);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"-0\"");
            Dqn_i64 result = Dqn_Str_ToI64("-0");
            DQN_TEST_EXPECT(testing_state, result == 0);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"-1\"");
            Dqn_i64 result = Dqn_Str_ToI64("-1");
            DQN_TEST_EXPECT(testing_state, result == -1);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"1.2\"");
            Dqn_i64 result = Dqn_Str_ToI64("1.2");
            DQN_TEST_EXPECT(testing_state, result == 1);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"1,234\"");
            Dqn_i64 result = Dqn_Str_ToI64("1,234");
            DQN_TEST_EXPECT(testing_state, result == 1234);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"1,2\"");
            Dqn_i64 result = Dqn_Str_ToI64("1,2");
            DQN_TEST_EXPECT(testing_state, result == 12);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"12a3\"");
            Dqn_i64 result = Dqn_Str_ToI64("12a3");
            DQN_TEST_EXPECT(testing_state, result == 12);
        }
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_Str_ToU64
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Str_ToU64");
        {
            DQN_TEST_START_SCOPE(testing_state, "Convert nullptr");
            Dqn_u64 result = Dqn_Str_ToU64(nullptr);
            DQN_TEST_EXPECT_MSG(testing_state, result == 0, "result: %zu", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert empty string");
            Dqn_u64 result = Dqn_Str_ToU64("");
            DQN_TEST_EXPECT_MSG(testing_state, result == 0, "result: %zu", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"1\"");
            Dqn_u64 result = Dqn_Str_ToU64("1");
            DQN_TEST_EXPECT_MSG(testing_state, result == 1, "result: %zu", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"-0\"");
            Dqn_u64 result = Dqn_Str_ToU64("-0");
            DQN_TEST_EXPECT_MSG(testing_state, result == 0, "result: %zu", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"-1\"");
            Dqn_u64 result = Dqn_Str_ToU64("-1");
            DQN_TEST_EXPECT_MSG(testing_state, result == 0, "result: %zu", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"1.2\"");
            Dqn_u64 result = Dqn_Str_ToU64("1.2");
            DQN_TEST_EXPECT_MSG(testing_state, result == 1, "result: %zu", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"1,234\"");
            Dqn_u64 result = Dqn_Str_ToU64("1,234");
            DQN_TEST_EXPECT_MSG(testing_state, result == 1234, "result: %zu", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"1,2\"");
            Dqn_u64 result = Dqn_Str_ToU64("1,2");
            DQN_TEST_EXPECT_MSG(testing_state, result == 12, "result: %zu", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Convert \"12a3\"");
            Dqn_u64 result = Dqn_Str_ToU64("12a3");
            DQN_TEST_EXPECT_MSG(testing_state, result == 12, "result: %zu", result);
        }
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_Str_Find
    // ---------------------------------------------------------------------------------------------
    {
        DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Str_Find");
        {
            DQN_TEST_START_SCOPE(testing_state, "String (char) is not in buffer");
            char const buf[]  = "836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55";
            char const find[] = "2";
            char const *result = Dqn_Str_Find(buf, find, Dqn_CharCountI(buf), Dqn_CharCountI(find));
            DQN_TEST_EXPECT(testing_state, result == nullptr);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "String (char) is in buffer");
            char const buf[]  = "836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55";
            char const find[] = "6";
            char const *result = Dqn_Str_Find(buf, find, Dqn_CharCountI(buf), Dqn_CharCountI(find));
            DQN_TEST_EXPECT(testing_state, result != nullptr);
            DQN_TEST_EXPECT(testing_state, result[0] == '6' && result[1] == 'a');
        }
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
}

void Dqn_Test_StringBuilder()
{
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_StringBuilder");
    Dqn_ArenaAllocator arena = {};
    // NOTE: Dqn_StringBuilder_Append
    {
        {
            DQN_TEST_START_SCOPE(testing_state, "Append variable size strings and build using heap arena");
            Dqn_StringBuilder<> builder = {};
            Dqn_StringBuilder_Append(&builder, "Abc", 1);
            Dqn_StringBuilder_Append(&builder, "cd");
            Dqn_isize size    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &arena, &size);
            DQN_DEFER { Dqn_ArenaAllocator_Free(&arena); };

            char const EXPECT_STR[] = "Acd";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Append empty string and build using heap arena");
            Dqn_StringBuilder<> builder = {};
            Dqn_StringBuilder_Append(&builder, "");
            Dqn_StringBuilder_Append(&builder, "");
            Dqn_isize size    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &arena, &size);
            DQN_DEFER { Dqn_ArenaAllocator_Free(&arena); };

            char const EXPECT_STR[] = "";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Append empty string onto string and build using heap arena");
            Dqn_StringBuilder<> builder = {};
            Dqn_StringBuilder_Append(&builder, "Acd");
            Dqn_StringBuilder_Append(&builder, "");
            Dqn_isize size    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &arena, &size);
            DQN_DEFER { Dqn_ArenaAllocator_Free(&arena); };

            char const EXPECT_STR[] = "Acd";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Append nullptr and build using heap arena");
            Dqn_StringBuilder<> builder = {};
            Dqn_StringBuilder_Append(&builder, nullptr, 5);
            Dqn_isize size    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &arena, &size);
            DQN_DEFER { Dqn_ArenaAllocator_Free(&arena); };

            char const EXPECT_STR[] = "";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Append and require new linked buffer and build using heap arena");
            Dqn_StringBuilder<2> builder = {};
            Dqn_StringBuilder_InitWithArena(&builder, &arena);
            Dqn_StringBuilder_Append(&builder, "A");
            Dqn_StringBuilder_Append(&builder, "z"); // Should force a new memory block
            Dqn_StringBuilder_Append(&builder, "tec");
            Dqn_isize size    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &arena, &size);
            DQN_DEFER { Dqn_ArenaAllocator_Free(&arena); };

            char const EXPECT_STR[] = "Aztec";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }
    }

    // NOTE: Dqn_StringBuilder_AppendChar
    {
        DQN_TEST_START_SCOPE(testing_state, "Append char and build using heap arena");
        Dqn_StringBuilder<> builder = {};
        Dqn_StringBuilder_AppendChar(&builder, 'a');
        Dqn_StringBuilder_AppendChar(&builder, 'b');
        Dqn_isize size    = 0;
        char *result = Dqn_StringBuilder_Build(&builder, &arena, &size);
        DQN_DEFER { Dqn_ArenaAllocator_Free(&arena); };

        char const EXPECT_STR[] = "ab";
        DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
        DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
    }

    // NOTE: Dqn_StringBuilder_AppendFmt
    {
        {
            DQN_TEST_START_SCOPE(testing_state, "Append format string and build using heap arena");
            Dqn_StringBuilder<> builder = {};
            Dqn_StringBuilder_AppendFmt(&builder, "Number: %d, String: %s, ", 4, "Hello Sailor");
            Dqn_StringBuilder_AppendFmt(&builder, "Extra Stuff");
            Dqn_isize size    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &arena, &size);
            DQN_DEFER { Dqn_ArenaAllocator_Free(&arena); };

            char const EXPECT_STR[] = "Number: 4, String: Hello Sailor, Extra Stuff";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Append nullptr format string and build using heap arena");
            Dqn_StringBuilder<> builder = {};
            Dqn_StringBuilder_AppendFmt(&builder, nullptr);
            Dqn_isize size = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &arena, &size);
            DQN_DEFER { Dqn_ArenaAllocator_Free(&arena); };

            char const EXPECT_STR[] = "";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }
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

void Dqn_Test_RunSuite()
{
    Dqn_Test_Allocator();
    Dqn_Test_Array();
    Dqn_Test_FixedArray();
    Dqn_Test_FixedString();
    Dqn_Test_Intrinsics();
    Dqn_Test_M4();
    Dqn_Test_Map();
    Dqn_Test_Rect();
    Dqn_Test_Str();
    Dqn_Test_String();
    Dqn_Test_StringBuilder();
    Dqn_Test_TicketMutex();
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

