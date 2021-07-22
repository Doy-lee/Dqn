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

// NOTE: Zero initialised allocators can be a null allocator if #define
// DQN_ALLOCATOR_DEFAULT_TO_NULL is defined, so handle this case specially
// by defaulting to the heap allocator which is the behaviour it would have
// used if the hash define was not used.

// In the macro below we ensure that the allocator is not null, this idiom is
// repeated whereever we zero initialise an allocator.

#define DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, name)                                                             \
    fprintf(stdout, name "\n");                                                                                        \
    if (testing_state.arena.backup_allocator.type == Dqn_AllocatorType::Null)                                         \
        testing_state.arena.backup_allocator = Dqn_Allocator_InitWithHeap();                                          \
    DQN_DEFER                                                                                                          \
    {                                                                                                                  \
        Dqn_TestingState_PrintGroupResult(&testing_state);                                                             \
        testing_state = {};                                                                                            \
        fprintf(stdout, "\n\n");                                                                                       \
    }

#define DQN_TEST_EXPECT_MSG(testing_state, expr, msg, ...)                                                             \
    DQN_ASSERT(testing_state.test.scope_started);                                                                      \
    if (!(expr))                                                                                                       \
    {                                                                                                                  \
        testing_state.test.fail_expr = DQN_STRING(#expr);                                                              \
        testing_state.test.fail_msg  = Dqn_String_InitArenaFmt(&testing_state.arena, msg, ##__VA_ARGS__);               \
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

        {
            DQN_TEST_START_SCOPE(testing_state, "Fixed Memory: Test free on fixed memory array does nothing");
            int memory[4]        = {};
            Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*size*/);
            DQN_DEFER { Dqn_Array_Free(&array); };
        }
    }

    // NOTE: Dynamic Memory: Dqn_Array
    {
        {
            DQN_TEST_START_SCOPE(testing_state, "Dynamic Memory: Reserve and check over commit reallocates");
            Dqn_Array<int> array = {};
            if (array.allocator.type == Dqn_AllocatorType::Null)
                array.allocator = Dqn_Allocator_InitWithHeap();

            DQN_DEFER { Dqn_Array_Free(&array); };

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
        }
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

void Dqn_Test_StringBuilder()
{
    Dqn_TestingState testing_state = {};
    DQN_TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_StringBuilder");
    Dqn_Allocator allocator = Dqn_Allocator_InitWithHeap();
    // NOTE: Dqn_StringBuilder_Append
    {
        {
            DQN_TEST_START_SCOPE(testing_state, "Append variable size strings and build using heap allocator");
            Dqn_StringBuilder<> builder = {};
            if (builder.backup_allocator.type == Dqn_AllocatorType::Null)
                builder.backup_allocator = Dqn_Allocator_InitWithHeap();

            Dqn_StringBuilder_Append(&builder, "Abc", 1);
            Dqn_StringBuilder_Append(&builder, "cd");
            Dqn_isize size    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &allocator, &size);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

            char const EXPECT_STR[] = "Acd";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Append empty string and build using heap allocator");
            Dqn_StringBuilder<> builder = {};
            if (builder.backup_allocator.type == Dqn_AllocatorType::Null)
                builder.backup_allocator = Dqn_Allocator_InitWithHeap();

            Dqn_StringBuilder_Append(&builder, "");
            Dqn_StringBuilder_Append(&builder, "");
            Dqn_isize size    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &allocator, &size);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

            char const EXPECT_STR[] = "";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Append empty string onto string and build using heap allocator");
            Dqn_StringBuilder<> builder = {};
            if (builder.backup_allocator.type == Dqn_AllocatorType::Null)
                builder.backup_allocator = Dqn_Allocator_InitWithHeap();

            Dqn_StringBuilder_Append(&builder, "Acd");
            Dqn_StringBuilder_Append(&builder, "");
            Dqn_isize size    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &allocator, &size);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

            char const EXPECT_STR[] = "Acd";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Append nullptr and build using heap allocator");
            Dqn_StringBuilder<> builder = {};
            if (builder.backup_allocator.type == Dqn_AllocatorType::Null)
                builder.backup_allocator = Dqn_Allocator_InitWithHeap();

            Dqn_StringBuilder_Append(&builder, nullptr, 5);
            Dqn_isize size    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &allocator, &size);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

            char const EXPECT_STR[] = "";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Append and require new linked buffer and build using heap allocator");
            Dqn_StringBuilder<2> builder = {};
            if (builder.backup_allocator.type == Dqn_AllocatorType::Null)
                builder.backup_allocator = Dqn_Allocator_InitWithHeap();

            Dqn_StringBuilder_Append(&builder, "A");
            Dqn_StringBuilder_Append(&builder, "z"); // Should force a new memory block
            Dqn_StringBuilder_Append(&builder, "tec");
            Dqn_isize size    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &allocator, &size);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

            char const EXPECT_STR[] = "Aztec";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }
    }

    // NOTE: Dqn_StringBuilder_AppendChar
    {
        DQN_TEST_START_SCOPE(testing_state, "Append char and build using heap allocator");
        Dqn_StringBuilder<> builder = {};
        if (builder.backup_allocator.type == Dqn_AllocatorType::Null)
            builder.backup_allocator = Dqn_Allocator_InitWithHeap();

        Dqn_StringBuilder_AppendChar(&builder, 'a');
        Dqn_StringBuilder_AppendChar(&builder, 'b');
        Dqn_isize size    = 0;
        char *result = Dqn_StringBuilder_Build(&builder, &allocator, &size);
        DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

        char const EXPECT_STR[] = "ab";
        DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
        DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
    }

    // NOTE: Dqn_StringBuilder_AppendFmt
    {
        {
            DQN_TEST_START_SCOPE(testing_state, "Append format string and build using heap allocator");
            Dqn_StringBuilder<> builder = {};
            if (builder.backup_allocator.type == Dqn_AllocatorType::Null)
                builder.backup_allocator = Dqn_Allocator_InitWithHeap();

            Dqn_StringBuilder_AppendFmt(&builder, "Number: %d, String: %s, ", 4, "Hello Sailor");
            Dqn_StringBuilder_AppendFmt(&builder, "Extra Stuff");
            Dqn_isize size    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &allocator, &size);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

            char const EXPECT_STR[] = "Number: 4, String: Hello Sailor, Extra Stuff";
            DQN_TEST_EXPECT_MSG(testing_state, size == Dqn_CharCountI(EXPECT_STR), "size: %zd", size);
            DQN_TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, size) == 0, "result: %s", result);
        }

        {
            DQN_TEST_START_SCOPE(testing_state, "Append nullptr format string and build using heap allocator");
            Dqn_StringBuilder<> builder = {};
            if (builder.backup_allocator.type == Dqn_AllocatorType::Null)
                builder.backup_allocator = Dqn_Allocator_InitWithHeap();

            Dqn_StringBuilder_AppendFmt(&builder, nullptr);
            Dqn_isize size = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &allocator, &size);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

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
        DQN_TEST_EXPECT(testing_state, mutex.ticket == mutex.serving + 1);
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

        DQN_TEST_EXPECT(testing_state, mutex.ticket == mutex.serving + 1);
        DQN_TEST_EXPECT(testing_state, mutex.ticket == ticket_b + 1);
    }
}

void Dqn_Test_RunSuite()
{
    Dqn_Test_Allocator();
    Dqn_Test_Array();
    Dqn_Test_FixedArray();
    Dqn_Test_FixedString();
    Dqn_Test_M4();
    Dqn_Test_Rect();
    Dqn_Test_Str();
    Dqn_Test_StringBuilder();
    Dqn_Test_TicketMutex();
}

#if defined(DQN_TEST_WITH_MAIN)
int main(int argc, char *argv[])
{
    (void)argv; (void)argc;
    Dqn_Test_RunSuite();
    return 0;
}
#endif

