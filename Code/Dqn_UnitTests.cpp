#define DQN_USE_PRIMITIVE_TYPEDEFS
#define DQN_IMPLEMENTATION
#include "Dqn.h"

struct TestState
{
    int           indent_level;
    Dqn_String    name;
    Dqn_String    fail_expr;
    Dqn_String    fail_msg;
    bool          scope_started;
};

struct TestingState
{
    int           num_tests_in_group;
    int           num_tests_ok_in_group;
    TestState     test;
    Dqn_MemArena  arena_;
    Dqn_Allocator allocator;
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
        Dqn_MemArena_ResetUsage(&testing_state.arena_, Dqn_ZeroMem::No);                                               \
        testing_state.allocator = Dqn_Allocator_Arena(&testing_state.arena_);                                          \
        testing_state.test      = {};                                                                                  \
    };                                                                                                                 \
    testing_state.test.name          = Dqn_Asprintf(&testing_state.allocator, test_name);                              \
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
        testing_state.test.fail_expr = Dqn_Asprintf(&testing_state.allocator, #expr);                                  \
        testing_state.test.fail_msg  = Dqn_Asprintf(&testing_state.allocator, msg, ##__VA_ARGS__);                     \
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
    Dqn_isize remaining_len = DESIRED_LEN - len - 1;
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

    Dqn_isize remaining_len = DESIRED_LEN - result->name.len - Dqn_CharCount(INDENT);
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

DQN_FILE_SCOPE void UnitTests()
{
    TestingState testing_state = {};
    // ---------------------------------------------------------------------------------------------
    //
    // NOTE: Dqn_Allocator
    //
    // ---------------------------------------------------------------------------------------------
    {
        TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Allocator");

        // NOTE: Various allocator test
        {
            {
                TEST_START_SCOPE(testing_state, "HeapAllocator - Allocate Small");
                Dqn_Allocator allocator = Dqn_Allocator_Heap();
                char constexpr EXPECT[] = "hello_world";
                char *buf               = DQN_CAST(char *)Dqn_Allocator_Allocate(&allocator, Dqn_ArrayCount(EXPECT), alignof(char));
                DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
                memcpy(buf, EXPECT, Dqn_ArrayCount(EXPECT));
                TEST_EXPECT_MSG(testing_state, memcmp(EXPECT, buf, Dqn_ArrayCount(EXPECT)) == 0, "buf: %s, expect: %s", buf, EXPECT);
            }

            {
                TEST_START_SCOPE(testing_state, "XHeapAllocator - Allocate Small");
                Dqn_Allocator allocator = Dqn_Allocator_XHeap();
                char constexpr EXPECT[] = "hello_world";
                char *buf               = DQN_CAST(char *)Dqn_Allocator_Allocate(&allocator, Dqn_ArrayCount(EXPECT), alignof(char));
                DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
                memcpy(buf, EXPECT, Dqn_ArrayCount(EXPECT));
                TEST_EXPECT_MSG(testing_state, memcmp(EXPECT, buf, Dqn_ArrayCount(EXPECT)) == 0, "buf: %s, expect: %s", buf, EXPECT);
            }

            {
                TEST_START_SCOPE(testing_state, "ArenaAllocator - Allocate Small");
                Dqn_MemArena arena      = {};
                arena.allocator         = Dqn_Allocator_Heap();
                Dqn_Allocator allocator = Dqn_Allocator_Arena(&arena);
                char constexpr EXPECT[] = "hello_world";
                char *buf               = DQN_CAST(char *)Dqn_Allocator_Allocate(&allocator, Dqn_ArrayCount(EXPECT), alignof(char));
                DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
                memcpy(buf, EXPECT, Dqn_ArrayCount(EXPECT));
                TEST_EXPECT_MSG(testing_state, memcmp(EXPECT, buf, Dqn_ArrayCount(EXPECT)) == 0, "buf: %s, expect: %s", buf, EXPECT);
            }
        }

        // NOTE: Alignment Test
        {
            Dqn_u8 const ALIGNMENT3 = 3;
            Dqn_u8 const NUM_BYTES  = sizeof(Dqn_u32);
            {
                TEST_START_SCOPE(testing_state, "HeapAllocator - Align to 3 bytes");
                Dqn_Allocator allocator = Dqn_Allocator_Heap();
                auto *buf               = DQN_CAST(Dqn_u32 *)Dqn_Allocator_Allocate(&allocator, NUM_BYTES, ALIGNMENT3);
                DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
                int buf_mod_alignment = DQN_CAST(int)(DQN_CAST(uintptr_t)buf % ALIGNMENT3);
                TEST_EXPECT_MSG(testing_state, buf_mod_alignment == 0, "buf_mod_alignment: %d", buf_mod_alignment);
            }

            {
                TEST_START_SCOPE(testing_state, "XHeapAllocator - Align to 3 bytes");
                Dqn_Allocator allocator = Dqn_Allocator_XHeap();
                auto *buf               = DQN_CAST(Dqn_u32 *)Dqn_Allocator_Allocate(&allocator, NUM_BYTES, ALIGNMENT3);
                DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
                int buf_mod_alignment = DQN_CAST(int)(DQN_CAST(uintptr_t)buf % ALIGNMENT3);
                TEST_EXPECT_MSG(testing_state, buf_mod_alignment == 0, "buf_mod_alignment: %d", buf_mod_alignment);
            }

            {
                TEST_START_SCOPE(testing_state, "ArenaAllocator - Align to 3 bytes");
                Dqn_MemArena arena      = {};
                Dqn_Allocator allocator = Dqn_Allocator_Arena(&arena);
                auto *buf               = DQN_CAST(Dqn_u32 *)Dqn_Allocator_Allocate(&allocator, NUM_BYTES, ALIGNMENT3);
                int buf_mod_alignment = DQN_CAST(int)(DQN_CAST(uintptr_t)buf % ALIGNMENT3);
                TEST_EXPECT_MSG(testing_state, buf_mod_alignment == 0, "buf_mod_alignment: %d", buf_mod_alignment);
            }
        }

        // NOTE: Dqn_AllocateMetadata tests
        {
            Dqn_u8 const ALIGNMENT3 = 3;
            Dqn_u8 const NUM_BYTES  = 4;
            Dqn_u8 const MAX_OFFSET = (ALIGNMENT3 - 1) + sizeof(Dqn_AllocateMetadata);
            {
                TEST_START_SCOPE(testing_state, "HeapAllocator - Allocation metadata initialised");
                Dqn_Allocator allocator = Dqn_Allocator_Heap();
                char *buf               = DQN_CAST(char *)Dqn_Allocator_Allocate(&allocator, NUM_BYTES, ALIGNMENT3);
                DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
                Dqn_AllocateMetadata metadata = Dqn_AllocateMetadata_Get(buf);
                TEST_EXPECT_MSG(testing_state, metadata.alignment == ALIGNMENT3, "metadata.alignment: %u, ALIGNMENT3: %u", metadata.alignment, ALIGNMENT3);
                TEST_EXPECT_MSG(testing_state, metadata.offset <= MAX_OFFSET, "metadata.offset: %u, MAX_OFFSET: %u", metadata.offset, MAX_OFFSET);
            }

            {
                TEST_START_SCOPE(testing_state, "XHeapAllocator - Allocation metadata initialised");
                Dqn_Allocator allocator = Dqn_Allocator_XHeap();
                char *buf               = DQN_CAST(char *)Dqn_Allocator_Allocate(&allocator, NUM_BYTES, ALIGNMENT3);
                DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
                Dqn_AllocateMetadata metadata = Dqn_AllocateMetadata_Get(buf);
                TEST_EXPECT_MSG(testing_state, metadata.alignment == ALIGNMENT3, "metadata.alignment: %u, ALIGNMENT3: %u", metadata.alignment, ALIGNMENT3);
                TEST_EXPECT_MSG(testing_state, metadata.offset <= MAX_OFFSET, "metadata.offset: %u, MAX_OFFSET: %u", metadata.offset, MAX_OFFSET);
            }

            {
                TEST_START_SCOPE(testing_state, "ArenaAllocator - Allocation metadata initialised");
                Dqn_MemArena arena      = {};
                Dqn_Allocator allocator = Dqn_Allocator_Arena(&arena);
                char *buf               = DQN_CAST(char *)Dqn_Allocator_Allocate(&allocator, NUM_BYTES, ALIGNMENT3);
                DQN_DEFER { Dqn_Allocator_Free(&allocator, buf); };
                Dqn_AllocateMetadata metadata = Dqn_AllocateMetadata_Get(buf);
                TEST_EXPECT_MSG(testing_state, metadata.alignment == ALIGNMENT3, "metadata.alignment: %u, ALIGNMENT3: %u", metadata.alignment, ALIGNMENT3);
                TEST_EXPECT_MSG(testing_state, metadata.offset <= MAX_OFFSET, "metadata.offset: %u, MAX_OFFSET: %u", metadata.offset, MAX_OFFSET);
            }
        }
    }

    // ---------------------------------------------------------------------------------------------
    //
    // NOTE: Dqn_Array
    //
    // ---------------------------------------------------------------------------------------------
    {
        TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Array");
        // NOTE: Dqn_Array_InitWithMemory
        {
            {
                TEST_START_SCOPE(testing_state, "Fixed Memory: Test add single item and can't allocate more");
                int memory[4]        = {};
                Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*len*/);
                Dqn_Array_Add(&array, 1);
                Dqn_Array_Add(&array, 2);
                Dqn_Array_Add(&array, 3);
                Dqn_Array_Add(&array, 4);
                TEST_EXPECT_MSG(testing_state, array.data[0] == 1, "array.data %d", array.data[0]);
                TEST_EXPECT_MSG(testing_state, array.data[1] == 2, "array.data %d", array.data[1]);
                TEST_EXPECT_MSG(testing_state, array.data[2] == 3, "array.data %d", array.data[2]);
                TEST_EXPECT_MSG(testing_state, array.data[3] == 4, "array.data %d", array.data[3]);
                TEST_EXPECT_MSG(testing_state, array.len == 4, "array.len: %d", array.len);

                int *added_item = Dqn_Array_Add(&array, 5);
                TEST_EXPECT(testing_state, added_item == nullptr);
                TEST_EXPECT_MSG(testing_state, array.len == 4, "array.len: %d", array.len);
                TEST_EXPECT_MSG(testing_state, array.max == 4, "array.max: %d", array.max);
            }

            {
                TEST_START_SCOPE(testing_state, "Fixed Memory: Test add array of items");
                int memory[4]        = {};
                int DATA[]           = {1, 2, 3};
                Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*len*/);
                Dqn_Array_Add(&array, DATA, Dqn_ArrayCount(DATA));
                TEST_EXPECT_MSG(testing_state, array.data[0] == 1, "array.data %d", array.data[0]);
                TEST_EXPECT_MSG(testing_state, array.data[1] == 2, "array.data %d", array.data[1]);
                TEST_EXPECT_MSG(testing_state, array.data[2] == 3, "array.data %d", array.data[2]);
                TEST_EXPECT_MSG(testing_state, array.len == 3, "array.len: %d", array.len);
                TEST_EXPECT_MSG(testing_state, array.max == 4, "array.max: %d", array.max);
            }

            {
                TEST_START_SCOPE(testing_state, "Fixed Memory: Test clear and clear with memory zeroed");
                int memory[4]        = {};
                int DATA[]           = {1, 2, 3};
                Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*len*/);
                Dqn_Array_Add(&array, DATA, Dqn_ArrayCount(DATA));
                Dqn_Array_Clear(&array, false /*zero_mem*/);
                TEST_EXPECT_MSG(testing_state, array.len == 0, "array.len: %d", array.len);
                TEST_EXPECT_MSG(testing_state, array.max == 4, "array.max: %d", array.max);
                TEST_EXPECT_MSG(testing_state, array.data[0] == 1, "array.data %d. Clear but don't zero memory so old values should still remain", array.data[0]);

                Dqn_Array_Clear(&array, true /*zero_mem*/);
                TEST_EXPECT_MSG(testing_state, array.data[0] == 0, "array.data %d. Clear but zero memory old values should not remain", array.data[0]);
            }

            {
                TEST_START_SCOPE(testing_state, "Fixed Memory: Test erase stable and erase unstable");
                int memory[4]        = {};
                int DATA[]           = {1, 2, 3, 4};
                Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*len*/);
                Dqn_Array_Add(&array, DATA, Dqn_ArrayCount(DATA));
                Dqn_Array_EraseUnstable(&array, 1);
                TEST_EXPECT_MSG(testing_state, array.data[0] == 1, "array.data %d", array.data[0]);
                TEST_EXPECT_MSG(testing_state, array.data[1] == 4, "array.data %d", array.data[1]);
                TEST_EXPECT_MSG(testing_state, array.data[2] == 3, "array.data %d", array.data[2]);
                TEST_EXPECT_MSG(testing_state, array.len == 3, "array.len: %d", array.len);

                Dqn_Array_EraseStable(&array, 0);
                TEST_EXPECT_MSG(testing_state, array.data[0] == 4, "array.data: %d", array.data[0]);
                TEST_EXPECT_MSG(testing_state, array.data[1] == 3, "array.data: %d", array.data[1]);
                TEST_EXPECT_MSG(testing_state, array.len == 2, "array.len: %d", array.len);
            }

            {
                TEST_START_SCOPE(testing_state, "Fixed Memory: Test array pop and peek");
                int memory[4]        = {};
                int DATA[]           = {1, 2, 3};
                Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*len*/);
                Dqn_Array_Add(&array, DATA, Dqn_ArrayCount(DATA));
                Dqn_Array_Pop(&array, 2);
                TEST_EXPECT_MSG(testing_state, array.data[0] == 1, "array.data: %d", array.data[0]);
                TEST_EXPECT_MSG(testing_state, array.len == 1, "array.len: %d", array.len);
                TEST_EXPECT_MSG(testing_state, array.max == 4, "array.max: %d", array.max);

                int *peek_item = Dqn_Array_Peek(&array);
                TEST_EXPECT_MSG(testing_state, *peek_item == 1, "peek: %d", *peek_item);
                TEST_EXPECT_MSG(testing_state, array.len == 1, "array.len: %d", array.len);
                TEST_EXPECT_MSG(testing_state, array.max == 4, "array.max: %d", array.max);
            }

            {
                TEST_START_SCOPE(testing_state, "Fixed Memory: Test free on fixed memory array does nothing");
                int memory[4]        = {};
                Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_ArrayCount(memory), 0 /*len*/);
                DQN_DEFER { Dqn_Array_Free(&array); };
            }
        }

        // NOTE: Dynamic Memory: Dqn_Array
        {
            {
                TEST_START_SCOPE(testing_state, "Dynamic Memory: Test reserve and over commit reallocates");
                Dqn_Array<int> array = {};
                DQN_DEFER { Dqn_Array_Free(&array); };

                Dqn_Array_Reserve(&array, 4);
                TEST_EXPECT_MSG(testing_state, array.len == 0, "array.len: %d", array.len);
                TEST_EXPECT_MSG(testing_state, array.max == 4, "array.max: %d", array.max);

                int DATA[] = {1, 2, 3, 4};
                Dqn_Array_Add(&array, DATA, Dqn_ArrayCount(DATA));
                TEST_EXPECT_MSG(testing_state, array.data[0] == 1, "array.data: %d", array.data[0]);
                TEST_EXPECT_MSG(testing_state, array.data[1] == 2, "array.data: %d", array.data[1]);
                TEST_EXPECT_MSG(testing_state, array.data[2] == 3, "array.data: %d", array.data[2]);
                TEST_EXPECT_MSG(testing_state, array.data[3] == 4, "array.data: %d", array.data[3]);
                TEST_EXPECT_MSG(testing_state, array.len == 4, "array.len: %d", array.len);

                int *added_item = Dqn_Array_Add(&array, 5);
                TEST_EXPECT_MSG(testing_state, *added_item == 5, "added_item: %d", *added_item);
                TEST_EXPECT_MSG(testing_state, array.data[4] == 5, "array.data: %d", array.data[4]);
                TEST_EXPECT_MSG(testing_state, array.len == 5, "array.len: %d", array.len);
                TEST_EXPECT_MSG(testing_state, array.max >= 5, "array.max: %d", array.max);
            }
        }
    }

    // ---------------------------------------------------------------------------------------------
    //
    // NOTE: Dqn_Rect
    //
    // ---------------------------------------------------------------------------------------------
    {
        TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Rect");
        // NOTE: Dqn_Rect_Intersection
        {
            {
                TEST_START_SCOPE(testing_state, "No intersection");
                Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(0, 0), Dqn_V2(100, 100));
                Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(200, 0), Dqn_V2(200, 200));
                Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

                TEST_EXPECT_MSG(testing_state,
                                ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 0 && ab.max.y == 0,
                                "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                                ab.min.x,
                                ab.min.y,
                                ab.max.x,
                                ab.max.y);
            }

            {
                TEST_START_SCOPE(testing_state, "A's min intersects B");
                Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(50, 50), Dqn_V2(100, 100));
                Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
                Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

                TEST_EXPECT_MSG(testing_state,
                                ab.min.x == 50 && ab.min.y == 50 && ab.max.x == 100 && ab.max.y == 100,
                                "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                                ab.min.x,
                                ab.min.y,
                                ab.max.x,
                                ab.max.y);
            }

            {
                TEST_START_SCOPE(testing_state, "B's min intersects A");
                Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
                Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(50, 50), Dqn_V2(100, 100));
                Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

                TEST_EXPECT_MSG(testing_state,
                                ab.min.x == 50 && ab.min.y == 50 && ab.max.x == 100 && ab.max.y == 100,
                                "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                                ab.min.x,
                                ab.min.y,
                                ab.max.x,
                                ab.max.y);
            }

            {
                TEST_START_SCOPE(testing_state, "A's max intersects B");
                Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(-50, -50), Dqn_V2(100, 100));
                Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(  0,   0), Dqn_V2(100, 100));
                Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

                TEST_EXPECT_MSG(testing_state,
                                ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 50 && ab.max.y == 50,
                                "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                                ab.min.x,
                                ab.min.y,
                                ab.max.x,
                                ab.max.y);
            }

            {
                TEST_START_SCOPE(testing_state, "B's max intersects A");
                Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(  0,   0), Dqn_V2(100, 100));
                Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(-50, -50), Dqn_V2(100, 100));
                Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

                TEST_EXPECT_MSG(testing_state,
                                ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 50 && ab.max.y == 50,
                                "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                                ab.min.x,
                                ab.min.y,
                                ab.max.x,
                                ab.max.y);
            }


            {
                TEST_START_SCOPE(testing_state, "B contains A");
                Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(25, 25), Dqn_V2( 25,  25));
                Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
                Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

                TEST_EXPECT_MSG(testing_state,
                                ab.min.x == 25 && ab.min.y == 25 && ab.max.x == 50 && ab.max.y == 50,
                                "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                                ab.min.x,
                                ab.min.y,
                                ab.max.x,
                                ab.max.y);
            }

            {
                TEST_START_SCOPE(testing_state, "A contains B");
                Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
                Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(25, 25), Dqn_V2( 25,  25));
                Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

                TEST_EXPECT_MSG(testing_state,
                                ab.min.x == 25 && ab.min.y == 25 && ab.max.x == 50 && ab.max.y == 50,
                                "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                                ab.min.x,
                                ab.min.y,
                                ab.max.x,
                                ab.max.y);
            }

            {
                TEST_START_SCOPE(testing_state, "A equals B");
                Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(0, 0), Dqn_V2(100, 100));
                Dqn_Rect b  = a;
                Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

                TEST_EXPECT_MSG(testing_state,
                                ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 100 && ab.max.y == 100,
                                "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                                ab.min.x,
                                ab.min.y,
                                ab.max.x,
                                ab.max.y);
            }
        }
    }

    // ---------------------------------------------------------------------------------------------
    //
    // NOTE: Dqn_StringBuilder
    //
    // ---------------------------------------------------------------------------------------------
    {
        TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_StringBuilder");
        Dqn_Allocator allocator = Dqn_Allocator_Heap();
        // NOTE: Dqn_StringBuilder_Append
        {
            {
                TEST_START_SCOPE(testing_state, "Append variable length strings and build using heap allocator");
                Dqn_StringBuilder<> builder = {};
                Dqn_StringBuilder_Append(&builder, "Abc", 1);
                Dqn_StringBuilder_Append(&builder, "cd");
                Dqn_isize len    = 0;
                char *result = Dqn_StringBuilder_Build(&builder, &allocator, &len);
                DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

                char constexpr EXPECT_STR[] = "Acd";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR), "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }

            {
                TEST_START_SCOPE(testing_state, "Append empty string and build using heap allocator");
                Dqn_StringBuilder<> builder = {};
                Dqn_StringBuilder_Append(&builder, "");
                Dqn_StringBuilder_Append(&builder, "");
                Dqn_isize len    = 0;
                char *result = Dqn_StringBuilder_Build(&builder, &allocator, &len);
                DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

                char constexpr EXPECT_STR[] = "";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR), "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }

            {
                TEST_START_SCOPE(testing_state, "Append empty string onto string and build using heap allocator");
                Dqn_StringBuilder<> builder = {};
                Dqn_StringBuilder_Append(&builder, "Acd");
                Dqn_StringBuilder_Append(&builder, "");
                Dqn_isize len    = 0;
                char *result = Dqn_StringBuilder_Build(&builder, &allocator, &len);
                DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

                char constexpr EXPECT_STR[] = "Acd";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR), "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }

            {
                TEST_START_SCOPE(testing_state, "Append nullptr and build using heap allocator");
                Dqn_StringBuilder<> builder = {};
                Dqn_StringBuilder_Append(&builder, nullptr, 5);
                Dqn_isize len    = 0;
                char *result = Dqn_StringBuilder_Build(&builder, &allocator, &len);
                DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

                char constexpr EXPECT_STR[] = "";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR), "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }

            {
                TEST_START_SCOPE(testing_state, "Append and require new linked buffer and build using heap allocator");
                Dqn_StringBuilder<2> builder = {};
                Dqn_StringBuilder_Append(&builder, "A");
                Dqn_StringBuilder_Append(&builder, "z"); // Should force a new memory block
                Dqn_StringBuilder_Append(&builder, "tec");
                Dqn_isize len    = 0;
                char *result = Dqn_StringBuilder_Build(&builder, &allocator, &len);
                DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

                char constexpr EXPECT_STR[] = "Aztec";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR), "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }
        }

        // NOTE: Dqn_StringBuilder_AppendChar
        {
            TEST_START_SCOPE(testing_state, "Append char and build using heap allocator");
            Dqn_StringBuilder<> builder = {};
            Dqn_StringBuilder_AppendChar(&builder, 'a');
            Dqn_StringBuilder_AppendChar(&builder, 'b');
            Dqn_isize len    = 0;
            char *result = Dqn_StringBuilder_Build(&builder, &allocator, &len);
            DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

            char constexpr EXPECT_STR[] = "ab";
            TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR), "len: %zd", len);
            TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
        }

        // NOTE: Dqn_StringBuilder_FmtAppend
        {
            {
                TEST_START_SCOPE(testing_state, "Append format string and build using heap allocator");
                Dqn_StringBuilder<> builder = {};
                Dqn_StringBuilder_FmtAppend(&builder, "Number: %d, String: %s, ", 4, "Hello Sailor");
                Dqn_StringBuilder_FmtAppend(&builder, "Extra Stuff");
                Dqn_isize len    = 0;
                char *result = Dqn_StringBuilder_Build(&builder, &allocator, &len);
                DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

                char constexpr EXPECT_STR[] = "Number: 4, String: Hello Sailor, Extra Stuff";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR), "len: %zd", len);
                TEST_EXPECT_MSG(testing_state, strncmp(result, EXPECT_STR, len) == 0, "result: %s", result);
            }

            {
                TEST_START_SCOPE(testing_state, "Append nullptr format string and build using heap allocator");
                Dqn_StringBuilder<> builder = {};
                Dqn_StringBuilder_FmtAppend(&builder, nullptr);
                Dqn_isize len = 0;
                char *result = Dqn_StringBuilder_Build(&builder, &allocator, &len);
                DQN_DEFER { Dqn_Allocator_Free(&allocator, result); };

                char constexpr EXPECT_STR[] = "";
                TEST_EXPECT_MSG(testing_state, len == Dqn_CharCountI(EXPECT_STR), "len: %zd", len);
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
    // NOTE: Dqn_FixedString
    //
    // ---------------------------------------------------------------------------------------------
    {
        TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_FixedString");

        // NOTE: Dqn_FixedString_Append
        {
            TEST_START_SCOPE(testing_state, "Append too much fails");
            Dqn_FixedString<4> str = {};
            TEST_EXPECT_MSG(testing_state, Dqn_FixedString_Append(&str, "abcd") == false, "We need space for the null-terminator");
        }

        // NOTE: Dqn_FixedString_AppendFmt
        {
            TEST_START_SCOPE(testing_state, "Append format string too much fails");
            Dqn_FixedString<4> str = {};
            TEST_EXPECT_MSG(testing_state, Dqn_FixedString_AppendFmt(&str, "abcd") == false, "We need space for the null-terminator");
        }
    }

    // ---------------------------------------------------------------------------------------------
    //
    // NOTE: Dqn_Str_ToI64
    //
    // ---------------------------------------------------------------------------------------------
    {
        TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Str_ToI64");
        {
            TEST_START_SCOPE(testing_state, "Convert nullptr");
            Dqn_i64 result = Dqn_Str_ToI64(nullptr);
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert empty string");
            Dqn_i64 result = Dqn_Str_ToI64("");
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1\"");
            Dqn_i64 result = Dqn_Str_ToI64("1");
            TEST_EXPECT(testing_state, result == 1);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"-0\"");
            Dqn_i64 result = Dqn_Str_ToI64("-0");
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"-1\"");
            Dqn_i64 result = Dqn_Str_ToI64("-1");
            TEST_EXPECT(testing_state, result == -1);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1.2\"");
            Dqn_i64 result = Dqn_Str_ToI64("1.2");
            TEST_EXPECT(testing_state, result == 1);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1,234\"");
            Dqn_i64 result = Dqn_Str_ToI64("1,234");
            TEST_EXPECT(testing_state, result == 1234);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1,2\"");
            Dqn_i64 result = Dqn_Str_ToI64("1,2");
            TEST_EXPECT(testing_state, result == 12);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"12a3\"");
            Dqn_i64 result = Dqn_Str_ToI64("12a3");
            TEST_EXPECT(testing_state, result == 12);
        }
    }

    // ---------------------------------------------------------------------------------------------
    //
    // NOTE: Dqn_Str_ToU64
    //
    // ---------------------------------------------------------------------------------------------
    {
        TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Str_ToU64");
        {
            TEST_START_SCOPE(testing_state, "Convert nullptr");
            Dqn_u64 result = Dqn_Str_ToU64(nullptr);
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert empty string");
            Dqn_u64 result = Dqn_Str_ToU64("");
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1\"");
            Dqn_u64 result = Dqn_Str_ToU64("1");
            TEST_EXPECT(testing_state, result == 1);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"-0\"");
            Dqn_u64 result = Dqn_Str_ToU64("-0");
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"-1\"");
            Dqn_u64 result = Dqn_Str_ToU64("-1");
            TEST_EXPECT(testing_state, result == 0);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1.2\"");
            Dqn_u64 result = Dqn_Str_ToU64("1.2");
            TEST_EXPECT(testing_state, result == 1);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1,234\"");
            Dqn_u64 result = Dqn_Str_ToU64("1,234");
            TEST_EXPECT(testing_state, result == 1234);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"1,2\"");
            Dqn_u64 result = Dqn_Str_ToU64("1,2");
            TEST_EXPECT(testing_state, result == 12);
        }

        {
            TEST_START_SCOPE(testing_state, "Convert \"12a3\"");
            Dqn_u64 result = Dqn_Str_ToU64("12a3");
            TEST_EXPECT(testing_state, result == 12);
        }
    }

    // ---------------------------------------------------------------------------------------------
    //
    // NOTE: Dqn_Str_Find
    //
    // ---------------------------------------------------------------------------------------------
    {
        TEST_DECLARE_GROUP_SCOPED(testing_state, "Dqn_Str_Find");
        {
            TEST_START_SCOPE(testing_state, "String (char) is not in buffer");
            char const buf[]  = "836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55";
            char const find[] = "2";
            char const *result = Dqn_Str_Find(buf, find, Dqn_CharCountI(buf), Dqn_CharCountI(find));
            TEST_EXPECT(testing_state, result == nullptr);
        }

        {
            TEST_START_SCOPE(testing_state, "String (char) is in buffer");
            char const buf[]  = "836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55";
            char const find[] = "6";
            char const *result = Dqn_Str_Find(buf, find, Dqn_CharCountI(buf), Dqn_CharCountI(find));
            TEST_EXPECT(testing_state, result != nullptr);
            TEST_EXPECT(testing_state, result[0] == '6' && result[1] == 'a');
        }
    }
}

int main(int argc, char *argv[])
{
    (void)argv; (void)argc;
    UnitTests();
    return 0;
}

