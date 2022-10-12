// -------------------------------------------------------------------------------------------------
// NOTE: Preprocessor Config
// -------------------------------------------------------------------------------------------------
/*
#define DQN_TEST_WITH_MAIN Define this to enable the main function and allow standalone compiling
                           and running of the file.
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

#define DQN_TESTER_IMPLEMENTATION
#include "dqn_tester.h"

Dqn_Tester Dqn_Test_Array()
{
    Dqn_Tester test = {};
    DQN_TESTER_BEGIN_GROUP("Dqn_Array");
    // NOTE: Dqn_Array_InitWithMemory
    {
        {
            Dqn_TesterBegin(&test, "Fixed Memory: Test add single item and can't allocate more");
            int memory[4]        = {};
            Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_CArray_Count(memory), 0 /*size*/);
            Dqn_Array_Add(&array, 1);
            Dqn_Array_Add(&array, 2);
            Dqn_Array_Add(&array, 3);
            Dqn_Array_Add(&array, 4);
            DQN_TESTER_ASSERTF(&test, array.data[0] == 1, "array.data %d", array.data[0]);
            DQN_TESTER_ASSERTF(&test, array.data[1] == 2, "array.data %d", array.data[1]);
            DQN_TESTER_ASSERTF(&test, array.data[2] == 3, "array.data %d", array.data[2]);
            DQN_TESTER_ASSERTF(&test, array.data[3] == 4, "array.data %d", array.data[3]);
            DQN_TESTER_ASSERTF(&test, array.size    == 4, "array.size: %zu", array.size);

            int *added_item = Dqn_Array_Add(&array, 5);
            DQN_TESTER_ASSERTF(&test, added_item == nullptr, "Failed to add item to array");
            DQN_TESTER_ASSERTF(&test, array.size == 4, "array.size: %zu", array.size);
            DQN_TESTER_ASSERTF(&test, array.max == 4, "array.max: %zu", array.max);
            Dqn_TesterEnd(&test);
        }

        {
            Dqn_TesterBegin(&test, "Fixed Memory: Test add array of items");
            int memory[4]        = {};
            int DATA[]           = {1, 2, 3};
            Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_CArray_Count(memory), 0 /*size*/);
            Dqn_Array_AddArray(&array, DATA, Dqn_CArray_Count(DATA));
            DQN_TESTER_ASSERTF(&test, array.data[0] == 1, "array.data %d", array.data[0]);
            DQN_TESTER_ASSERTF(&test, array.data[1] == 2, "array.data %d", array.data[1]);
            DQN_TESTER_ASSERTF(&test, array.data[2] == 3, "array.data %d", array.data[2]);
            DQN_TESTER_ASSERTF(&test, array.size == 3, "array.size: %zu", array.size);
            DQN_TESTER_ASSERTF(&test, array.max == 4, "array.max: %zu", array.max);
            Dqn_TesterEnd(&test);
        }

        {
            Dqn_TesterBegin(&test, "Fixed Memory: Test clear and clear with memory zeroed");
            int memory[4]        = {};
            int DATA[]           = {1, 2, 3};
            Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_CArray_Count(memory), 0 /*size*/);
            Dqn_Array_AddArray(&array, DATA, Dqn_CArray_Count(DATA));
            Dqn_Array_Clear(&array, Dqn_ZeroMem_No);
            DQN_TESTER_ASSERTF(&test, array.size == 0, "array.size: %zu", array.size);
            DQN_TESTER_ASSERTF(&test, array.max == 4, "array.max: %zu", array.max);
            DQN_TESTER_ASSERTF(&test, array.data[0] == 1, "array.data %d. Clear but don't zero memory so old values should still remain", array.data[0]);

            Dqn_Array_Clear(&array, Dqn_ZeroMem_Yes);
            DQN_TESTER_ASSERTF(&test, array.data[0] == 0, "array.data %d. Clear but zero memory old values should not remain", array.data[0]);
            Dqn_TesterEnd(&test);
        }

        {
            Dqn_TesterBegin(&test, "Fixed Memory: Test erase stable and erase unstable");
            int memory[4]        = {};
            int DATA[]           = {1, 2, 3, 4};
            Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_CArray_Count(memory), 0 /*size*/);
            Dqn_Array_AddArray(&array, DATA, Dqn_CArray_Count(DATA));
            Dqn_Array_EraseUnstable(&array, 1);
            DQN_TESTER_ASSERTF(&test, array.data[0] == 1, "array.data %d", array.data[0]);
            DQN_TESTER_ASSERTF(&test, array.data[1] == 4, "array.data %d", array.data[1]);
            DQN_TESTER_ASSERTF(&test, array.data[2] == 3, "array.data %d", array.data[2]);
            DQN_TESTER_ASSERTF(&test, array.size == 3, "array.size: %zu", array.size);

            Dqn_Array_EraseStable(&array, 0);
            DQN_TESTER_ASSERTF(&test, array.data[0] == 4, "array.data: %d", array.data[0]);
            DQN_TESTER_ASSERTF(&test, array.data[1] == 3, "array.data: %d", array.data[1]);
            DQN_TESTER_ASSERTF(&test, array.size == 2, "array.size: %zu", array.size);
            Dqn_TesterEnd(&test);
        }

        {
            Dqn_TesterBegin(&test, "Fixed Memory: Test array pop and peek");
            int memory[4]        = {};
            int DATA[]           = {1, 2, 3};
            Dqn_Array<int> array = Dqn_Array_InitWithMemory(memory, Dqn_CArray_Count(memory), 0 /*size*/);
            Dqn_Array_AddArray(&array, DATA, Dqn_CArray_Count(DATA));
            Dqn_Array_Pop(&array, 2);
            DQN_TESTER_ASSERTF(&test, array.data[0] == 1, "array.data: %d", array.data[0]);
            DQN_TESTER_ASSERTF(&test, array.size == 1, "array.size: %zu", array.size);
            DQN_TESTER_ASSERTF(&test, array.max == 4, "array.max: %zu", array.max);

            int *peek_item = Dqn_Array_Peek(&array);
            DQN_TESTER_ASSERTF(&test, *peek_item == 1, "peek: %d", *peek_item);
            DQN_TESTER_ASSERTF(&test, array.size == 1, "array.size: %zu", array.size);
            DQN_TESTER_ASSERTF(&test, array.max == 4, "array.max: %zu", array.max);
            Dqn_TesterEnd(&test);
        }
    }

    // NOTE: Dynamic Memory: Dqn_Array
    {
        Dqn_TesterBegin(&test, "Dynamic Memory: Reserve and check over commit reallocates");
        Dqn_Arena arena = {};
        Dqn_Array<int> array = {};
        array.arena = &arena;

        Dqn_Array_Reserve(&array, 4);
        DQN_TESTER_ASSERTF(&test, array.size == 0, "array.size: %zu", array.size);
        DQN_TESTER_ASSERTF(&test, array.max == 4, "array.max: %zu", array.max);

        int DATA[] = {1, 2, 3, 4};
        Dqn_Array_AddArray(&array, DATA, Dqn_CArray_Count(DATA));
        DQN_TESTER_ASSERTF(&test, array.data[0] == 1, "array.data: %d", array.data[0]);
        DQN_TESTER_ASSERTF(&test, array.data[1] == 2, "array.data: %d", array.data[1]);
        DQN_TESTER_ASSERTF(&test, array.data[2] == 3, "array.data: %d", array.data[2]);
        DQN_TESTER_ASSERTF(&test, array.data[3] == 4, "array.data: %d", array.data[3]);
        DQN_TESTER_ASSERTF(&test, array.size == 4, "array.size: %zu", array.size);

        int *added_item = Dqn_Array_Add(&array, 5);
        DQN_TESTER_ASSERTF(&test, *added_item == 5, "added_item: %d", *added_item);
        DQN_TESTER_ASSERTF(&test, array.data[4] == 5, "array.data: %d", array.data[4]);
        DQN_TESTER_ASSERTF(&test, array.size == 5, "array.size: %zu", array.size);
        DQN_TESTER_ASSERTF(&test, array.max >= 5, "array.max: %zu", array.max);

        Dqn_Arena_Free(&arena, false /*clear_mem*/);
        Dqn_TesterEnd(&test);
    }

    DQN_TESTER_END_GROUP(&test);
    return test;
}

Dqn_Tester Dqn_Test_File()
{
    Dqn_Tester test = {};
    DQN_TESTER_BEGIN_GROUP("Dqn_Fs");
    {
        Dqn_Arena arena = {};
        Dqn_Allocator allocator = Dqn_Arena_Allocator(&arena);
        Dqn_TesterBegin(&test, "Make directory recursive \"abcd/efgh\"");
        DQN_TESTER_ASSERTF(&test, Dqn_Fs_MakeDir(DQN_STRING8("abcd/efgh")), "Failed to make directory");
        DQN_TESTER_ASSERTF(&test, Dqn_Fs_DirExists(DQN_STRING8("abcd")), "Directory was not made");
        DQN_TESTER_ASSERTF(&test, Dqn_Fs_DirExists(DQN_STRING8("abcd/efgh")), "Subdirectory was not made");
        DQN_TESTER_ASSERTF(&test, Dqn_Fs_Exists(DQN_STRING8("abcd")) == false, "This function should only return true for files");
        DQN_TESTER_ASSERTF(&test, Dqn_Fs_Exists(DQN_STRING8("abcd/efgh")) == false, "This function should only return true for files");
        DQN_TESTER_ASSERTF(&test, Dqn_Fs_Delete(DQN_STRING8("abcd/efgh")), "Failed to delete directory");
        DQN_TESTER_ASSERTF(&test, Dqn_Fs_Delete(DQN_STRING8("abcd")), "Failed to cleanup directory");
        Dqn_Arena_Free(&arena, false /*clear_mem*/);
        Dqn_TesterEnd(&test);
    }

    {
        // NOTE: Write step
        Dqn_String8 const SRC_FILE = DQN_STRING8("dqn_test_file");
        Dqn_TesterBegin(&test, "Write file, read it, copy it, move it and delete it");
        Dqn_b32 write_result = Dqn_Fs_WriteCString8ToFileCString8(SRC_FILE.data, SRC_FILE.size, "test", 4);
        DQN_TESTER_ASSERT(&test, write_result);
        DQN_TESTER_ASSERT(&test, Dqn_Fs_Exists(SRC_FILE));

        // NOTE: Read step
        Dqn_Arena arena = {};
        Dqn_String8 read_file = Dqn_Fs_ReadFileCString8ToString8Arena(SRC_FILE.data, SRC_FILE.size, &arena);
        DQN_TESTER_ASSERTF(&test, Dqn_String8_IsValid(read_file), "Failed to load file");
        DQN_TESTER_ASSERTF(&test, read_file.size == 4, "File read wrong amount of bytes");
        DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(read_file, DQN_STRING8("test")), "read(%zu): %.*s", read_file.size, DQN_STRING_FMT(read_file));

        // NOTE: Copy step
        Dqn_String8 const COPY_FILE = DQN_STRING8("dqn_test_file_copy");
        Dqn_b32 copy_result = Dqn_Fs_Copy(SRC_FILE, COPY_FILE, true /*overwrite*/);
        DQN_TESTER_ASSERT(&test, copy_result);
        DQN_TESTER_ASSERT(&test, Dqn_Fs_Exists(COPY_FILE));

        // NOTE: Move step
        Dqn_String8 const MOVE_FILE = DQN_STRING8("dqn_test_file_move");
        Dqn_b32 move_result = Dqn_Fs_Move(COPY_FILE, MOVE_FILE, true /*overwrite*/);
        DQN_TESTER_ASSERT(&test, move_result);
        DQN_TESTER_ASSERT(&test, Dqn_Fs_Exists(MOVE_FILE));
        DQN_TESTER_ASSERTF(&test, Dqn_Fs_Exists(COPY_FILE) == false, "Moving a file should remove the original");

        // NOTE: Delete step
        Dqn_b32 delete_src_file   = Dqn_Fs_Delete(SRC_FILE);
        Dqn_b32 delete_moved_file = Dqn_Fs_Delete(MOVE_FILE);
        DQN_TESTER_ASSERT(&test, delete_src_file);
        DQN_TESTER_ASSERT(&test, delete_moved_file);

        // NOTE: Deleting non-existent file fails
        Dqn_b32 delete_non_existent_src_file   = Dqn_Fs_Delete(SRC_FILE);
        Dqn_b32 delete_non_existent_moved_file = Dqn_Fs_Delete(MOVE_FILE);
        DQN_TESTER_ASSERT(&test, delete_non_existent_moved_file == false);
        DQN_TESTER_ASSERT(&test, delete_non_existent_src_file == false);

        Dqn_Arena_Free(&arena, false /*clear_mem*/);
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
    return test;
}

Dqn_Tester Dqn_Test_FixedArray()
{
    Dqn_Tester test = {};
#if defined(DQN_WITH_FIXED_ARRAY)
    DQN_TESTER_BEGIN_GROUP("Dqn_FixedArray");
    // NOTE: Dqn_FixedArray_Init
    {
        Dqn_TesterBegin(&test, "Initialise from raw array");
        int raw_array[] = {1, 2};
        auto array = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_CArray_Count(raw_array));
        DQN_TESTER_ASSERT(&test, array.size == 2);
        DQN_TESTER_ASSERT(&test, array[0] == 1);
        DQN_TESTER_ASSERT(&test, array[1] == 2);
        Dqn_TesterEnd(&test);
    }

    // NOTE: Dqn_FixedArray_EraseStable
    {
        Dqn_TesterBegin(&test, "Erase stable 1 element from array");
        int raw_array[] = {1, 2, 3};
        auto array = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_CArray_Count(raw_array));
        Dqn_FixedArray_EraseStable(&array, 1);
        DQN_TESTER_ASSERT(&test, array.size == 2);
        DQN_TESTER_ASSERT(&test, array[0] == 1);
        DQN_TESTER_ASSERT(&test, array[1] == 3);
        Dqn_TesterEnd(&test);
    }

    // NOTE: Dqn_FixedArray_EraseUnstable
    {
        Dqn_TesterBegin(&test, "Erase unstable 1 element from array");
        int raw_array[] = {1, 2, 3};
        auto array = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_CArray_Count(raw_array));
        Dqn_FixedArray_EraseUnstable(&array, 0);
        DQN_TESTER_ASSERT(&test, array.size == 2);
        DQN_TESTER_ASSERT(&test, array[0] == 3);
        DQN_TESTER_ASSERT(&test, array[1] == 2);
        Dqn_TesterEnd(&test);
    }

    // NOTE: Dqn_FixedArray_Add
    {
        Dqn_TesterBegin(&test, "Add 1 element to array");
        int const ITEM  = 2;
        int raw_array[] = {1};
        auto array      = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_CArray_Count(raw_array));
        Dqn_FixedArray_Add(&array, ITEM);
        DQN_TESTER_ASSERT(&test, array.size == 2);
        DQN_TESTER_ASSERT(&test, array[0] == 1);
        DQN_TESTER_ASSERT(&test, array[1] == ITEM);
        Dqn_TesterEnd(&test);
    }

    // NOTE: Dqn_FixedArray_Clear
    {
        Dqn_TesterBegin(&test, "Clear array");
        int raw_array[] = {1};
        auto array      = Dqn_FixedArray_Init<int, 4>(raw_array, (int)Dqn_CArray_Count(raw_array));
        Dqn_FixedArray_Clear(&array);
        DQN_TESTER_ASSERT(&test, array.size == 0);
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
#endif // DQN_WITH_FIXED_ARRAY
    return test;
}

Dqn_Tester Dqn_Test_FixedString()
{
    Dqn_Tester test = {};
#if defined(DQN_WITH_FIXED_STRING)
    DQN_TESTER_BEGIN_GROUP("Dqn_FixedString");

    // NOTE: Dqn_FixedString_Append
    {
        Dqn_TesterBegin(&test, "Append too much fails");
        Dqn_FixedString<4> str = {};
        DQN_TESTER_ASSERTF(&test, Dqn_FixedString_Append(&str, "abcd") == false, "We need space for the null-terminator");
        Dqn_TesterEnd(&test);
    }

    // NOTE: Dqn_FixedString_AppendFmt
    {
        Dqn_TesterBegin(&test, "Append format string too much fails");
        Dqn_FixedString<4> str = {};
        DQN_TESTER_ASSERTF(&test, Dqn_FixedString_AppendFmt(&str, "abcd") == false, "We need space for the null-terminator");
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
#endif // DQN_WITH_FIXED_STRING
    return test;
}

Dqn_Tester Dqn_Test_Hex()
{
    Dqn_Tester test = {};
#if defined(DQN_WITH_HEX)
    DQN_TESTER_BEGIN_GROUP("Dqn_Hex");
    {
        Dqn_TesterBegin(&test, "Convert 0x123");
        uint64_t result = Dqn_Hex_StringToU64(DQN_STRING8("0x123"));
        DQN_TESTER_ASSERTF(&test, result == 0x123, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Convert 0xFFFF");
        uint64_t result = Dqn_Hex_StringToU64(DQN_STRING8("0xFFFF"));
        DQN_TESTER_ASSERTF(&test, result == 0xFFFF, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Convert FFFF");
        uint64_t result = Dqn_Hex_StringToU64(DQN_STRING8("FFFF"));
        DQN_TESTER_ASSERTF(&test, result == 0xFFFF, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Convert abCD");
        uint64_t result = Dqn_Hex_StringToU64(DQN_STRING8("abCD"));
        DQN_TESTER_ASSERTF(&test, result == 0xabCD, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Convert 0xabCD");
        uint64_t result = Dqn_Hex_StringToU64(DQN_STRING8("0xabCD"));
        DQN_TESTER_ASSERTF(&test, result == 0xabCD, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Convert 0x");
        uint64_t result = Dqn_Hex_StringToU64(DQN_STRING8("0x"));
        DQN_TESTER_ASSERTF(&test, result == 0x0, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Convert 0X");
        uint64_t result = Dqn_Hex_StringToU64(DQN_STRING8("0X"));
        DQN_TESTER_ASSERTF(&test, result == 0x0, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Convert 3");
        uint64_t result = Dqn_Hex_StringToU64(DQN_STRING8("3"));
        DQN_TESTER_ASSERTF(&test, result == 3, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Convert f");
        uint64_t result = Dqn_Hex_StringToU64(DQN_STRING8("f"));
        DQN_TESTER_ASSERTF(&test, result == 0xf, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Convert g");
        uint64_t result = Dqn_Hex_StringToU64(DQN_STRING8("g"));
        DQN_TESTER_ASSERTF(&test, result == 0, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Convert -0x3");
        uint64_t result = Dqn_Hex_StringToU64(DQN_STRING8("-0x3"));
        DQN_TESTER_ASSERTF(&test, result == 0, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
#endif // DQN_WITH_HEX
    return test;
}

Dqn_Tester Dqn_Test_M4()
{
    Dqn_Tester test = {};
#if defined(DQN_WITH_MATH)
    DQN_TESTER_BEGIN_GROUP("Dqn_M4");
    {
        Dqn_TesterBegin(&test, "Simple translate and scale matrix");
        Dqn_M4 translate = Dqn_M4_TranslateF(1, 2, 3);
        Dqn_M4 scale     = Dqn_M4_ScaleF(2, 2, 2);
        Dqn_M4 result    = Dqn_M4_Mul(translate, scale);

        const Dqn_M4 EXPECT = {{
            {2, 0, 0, 0},
            {0, 2, 0, 0},
            {0, 0, 2, 0},
            {1, 2, 3, 1},
        }};

        DQN_TESTER_ASSERTF(&test,
                           memcmp(result.columns, EXPECT.columns, sizeof(EXPECT)) == 0,
                           "\nresult =\n%s\nexpected =\n%s",
                           Dqn_M4_ColumnMajorString(result).data,
                           Dqn_M4_ColumnMajorString(EXPECT).data);
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
#endif // DQN_WITH_MATH
    return test;
}

Dqn_Tester Dqn_Test_DSMap()
{
    Dqn_Tester test = {};
#if defined(DQN_WITH_DSMAP)
    DQN_TESTER_BEGIN_GROUP("Dqn_DSMap");
    {
        Dqn_TesterBegin(&test, "Add r-value item to map");
        Dqn_DSMap<int> map         = Dqn_DSMap_Init<int>(128);
        Dqn_DSMapEntry<int> *entry = Dqn_DSMap_AddCopy(&map, 3 /*hash*/, 5 /*value*/);
        DQN_TESTER_ASSERTF(&test, map.size == 128, "size: %I64d", map.size);
        DQN_TESTER_ASSERTF(&test, map.count == 1, "count: %zu", map.count);
        DQN_TESTER_ASSERTF(&test, entry->hash == 3, "hash: %zu", entry->hash);
        DQN_TESTER_ASSERTF(&test, entry->value == 5, "value: %d", entry->value);
        Dqn_DSMap_Free(&map, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Add l-value item to map");
        Dqn_DSMap<int>       map   = Dqn_DSMap_Init<int>(128);
        int                  value = 5;
        Dqn_DSMapEntry<int> *entry = Dqn_DSMap_Add(&map, 3 /*hash*/, value);
        DQN_TESTER_ASSERTF(&test, map.size == 128, "size: %I64d", map.size);
        DQN_TESTER_ASSERTF(&test, map.count == 1, "count: %zu", map.count);
        DQN_TESTER_ASSERTF(&test, entry->hash == 3, "hash: %zu", entry->hash);
        DQN_TESTER_ASSERTF(&test, entry->value == 5, "value: %d", entry->value);
        Dqn_DSMap_Free(&map, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Get item from map");
        Dqn_DSMap<int> map         = Dqn_DSMap_Init<int>(128);
        Dqn_DSMapEntry<int> *entry = Dqn_DSMap_AddCopy(&map, 3 /*hash*/, 5 /*value*/);
        Dqn_DSMapEntry<int> *get_entry = Dqn_DSMap_Get(&map, 3 /*hash*/);
        DQN_TESTER_ASSERTF(&test, get_entry == entry, "get_entry: %p, entry: %p", get_entry, entry);
        Dqn_DSMap_Free(&map, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Get non-existent item from map");
        Dqn_DSMap<int> map         = Dqn_DSMap_Init<int>(128);
        Dqn_DSMapEntry<int> *entry = Dqn_DSMap_Get(&map, 3 /*hash*/);
        DQN_TESTER_ASSERT(&test, entry == nullptr);
        Dqn_DSMap_Free(&map, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Erase item from map");
        Dqn_DSMap<int> map = Dqn_DSMap_Init<int>(128);
        Dqn_DSMap_AddCopy(&map, 3 /*hash*/, 5 /*value*/);
        DQN_TESTER_ASSERTF(&test, map.count == 1, "count: %I64d", map.count);
        Dqn_DSMap_Erase(&map, 3 /*hash*/, Dqn_ZeroMem_No);
        DQN_TESTER_ASSERTF(&test, map.count == 0, "count: %I64d", map.count);
        Dqn_DSMap_Free(&map, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Erase non-existent item from map");
        Dqn_DSMap<int> map = Dqn_DSMap_Init<int>(128);
        Dqn_DSMap_Erase(&map, 3 /*hash*/, Dqn_ZeroMem_No);
        DQN_TESTER_ASSERTF(&test, map.count == 0, "count: %I64d", map.count);
        Dqn_DSMap_Free(&map, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Test resize on maximum load");
        const Dqn_isize INIT_SIZE = 4;
        Dqn_DSMap<int> map = Dqn_DSMap_Init<int>(INIT_SIZE);
        Dqn_DSMap_AddCopy(&map, 0 /*hash*/, 5 /*value*/);
        Dqn_DSMap_AddCopy(&map, 1 /*hash*/, 5 /*value*/);
        DQN_TESTER_ASSERTF(&test, map.count == 2, "count: %I64d", map.count);

        // This *should* cause a resize because 3/4 slots filled is 75% load
        Dqn_DSMap_AddCopy(&map, 6 /*hash*/, 5 /*value*/);
        DQN_TESTER_ASSERTF(&test, map.count == 3, "count: %I64d", map.count);
        DQN_TESTER_ASSERTF(&test, map.size == INIT_SIZE * 2, "size: %I64d", map.size);

        // Check that the elements are rehashed where we expected them to be
        DQN_TESTER_ASSERT(&test, map.slots[0].occupied == DQN_CAST(uint8_t)true);
        DQN_TESTER_ASSERT(&test, map.slots[1].occupied == DQN_CAST(uint8_t)true);
        DQN_TESTER_ASSERT(&test, map.slots[2].occupied == DQN_CAST(uint8_t)false);
        DQN_TESTER_ASSERT(&test, map.slots[3].occupied == DQN_CAST(uint8_t)false);
        DQN_TESTER_ASSERT(&test, map.slots[4].occupied == DQN_CAST(uint8_t)false);
        DQN_TESTER_ASSERT(&test, map.slots[5].occupied == DQN_CAST(uint8_t)false);
        DQN_TESTER_ASSERT(&test, map.slots[6].occupied == DQN_CAST(uint8_t)true);
        DQN_TESTER_ASSERT(&test, map.slots[7].occupied == DQN_CAST(uint8_t)false);

        DQN_TESTER_ASSERTF(&test, map.slots[0].value == 5, "value: %d", map.slots[0].value);
        DQN_TESTER_ASSERTF(&test, map.slots[1].value == 5, "value: %d", map.slots[1].value);
        DQN_TESTER_ASSERTF(&test, map.slots[6].value == 5, "value: %d", map.slots[6].value);

        Dqn_DSMap_Free(&map, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
#endif // DQN_WITH_DSMAP
    return test;
}

Dqn_Tester Dqn_Test_Map()
{
    Dqn_Tester test = {};
#if defined(DQN_WITH_MAP)
    DQN_TESTER_BEGIN_GROUP("Dqn_Map");
    Dqn_Arena arena = {};
    {
        Dqn_TesterBegin(&test, "Add r-value item to map");
        Dqn_Map<int> map         = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_MapEntry<int> *entry = Dqn_Map_AddCopy(&map, 3 /*hash*/, 5 /*value*/, Dqn_MapCollideRule::Overwrite);
        DQN_TESTER_ASSERTF(&test, map.size == 1, "size: %I64d", map.size);
        DQN_TESTER_ASSERTF(&test, map.count == 1, "count: %zu", map.count);
        DQN_TESTER_ASSERTF(&test, map.chain_count == 0, "chain_count: %zu", map.chain_count);
        DQN_TESTER_ASSERTF(&test, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TESTER_ASSERTF(&test, entry->hash == 3, "hash: %zu", entry->hash);
        DQN_TESTER_ASSERTF(&test, entry->value == 5, "value: %d", entry->value);
        DQN_TESTER_ASSERTF(&test, entry->next == nullptr, "next: %p", entry->next);
        Dqn_Arena_Free(&arena, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Add l-value item to map");
        Dqn_Map<int>       map   = Dqn_Map_InitWithArena<int>(&arena, 1);
        int                value = 5;
        Dqn_MapEntry<int> *entry = Dqn_Map_Add(&map, 3 /*hash*/, value, Dqn_MapCollideRule::Overwrite);
        DQN_TESTER_ASSERTF(&test, map.size == 1, "size: %I64d", map.size);
        DQN_TESTER_ASSERTF(&test, map.count == 1, "count: %zu", map.count);
        DQN_TESTER_ASSERTF(&test, map.chain_count == 0, "chain_count: %zu", map.chain_count);
        DQN_TESTER_ASSERTF(&test, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TESTER_ASSERTF(&test, entry->hash == 3, "hash: %zu", entry->hash);
        DQN_TESTER_ASSERTF(&test, entry->value == 5, "value: %d", entry->value);
        DQN_TESTER_ASSERTF(&test, entry->next == nullptr, "next: %p", entry->next);
        Dqn_Arena_Free(&arena, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Add r-value item and overwrite on collision");
        Dqn_Map<int>       map   = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_MapEntry<int> *entry_a = Dqn_Map_AddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapEntry<int> *entry_b = Dqn_Map_AddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Overwrite);
        DQN_TESTER_ASSERTF(&test, map.size == 1, "size: %zu", map.size);
        DQN_TESTER_ASSERTF(&test, map.count == 1, "count: %zu", map.count);
        DQN_TESTER_ASSERTF(&test, map.chain_count == 0, "chain_count: %zu", map.chain_count);
        DQN_TESTER_ASSERTF(&test, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TESTER_ASSERTF(&test, entry_a == entry_b, "Expected entry to be overwritten");
        DQN_TESTER_ASSERTF(&test, entry_b->hash == 4, "hash: %zu", entry_b->hash);
        DQN_TESTER_ASSERTF(&test, entry_b->value == 6, "value: %d", entry_b->value);
        DQN_TESTER_ASSERTF(&test, entry_b->next == nullptr, "next: %p", entry_b->next);
        Dqn_Arena_Free(&arena, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Add r-value item and fail on collision");
        Dqn_Map<int> map = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_Map_AddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapEntry<int> *entry_b = Dqn_Map_AddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Fail);
        DQN_TESTER_ASSERTF(&test, entry_b == nullptr, "Expected entry to be overwritten");
        DQN_TESTER_ASSERTF(&test, map.size == 1, "size: %zu", map.size);
        DQN_TESTER_ASSERTF(&test, map.count == 1, "count: %zu", map.count);
        DQN_TESTER_ASSERTF(&test, map.chain_count == 0, "chain_count: %zu", map.chain_count);
        DQN_TESTER_ASSERTF(&test, map.free_list == nullptr, "free_list: %p", map.free_list);
        Dqn_Arena_Free(&arena, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Add r-value item and chain on collision");
        Dqn_Map<int>       map   = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_MapEntry<int> *entry_a = Dqn_Map_AddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapEntry<int> *entry_b = Dqn_Map_AddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Chain);
        DQN_TESTER_ASSERTF(&test, map.size == 1, "size: %zu", map.size);
        DQN_TESTER_ASSERTF(&test, map.count == 1, "count: %zu", map.count);
        DQN_TESTER_ASSERTF(&test, map.chain_count == 1, "chain_count: %zu", map.chain_count);
        DQN_TESTER_ASSERTF(&test, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TESTER_ASSERTF(&test, entry_a != entry_b, "Expected colliding entry to be chained");
        DQN_TESTER_ASSERTF(&test, entry_a->next == entry_b, "Expected chained entry to be next to our first map entry");
        DQN_TESTER_ASSERTF(&test, entry_b->hash == 4, "hash: %zu", entry_b->hash);
        DQN_TESTER_ASSERTF(&test, entry_b->value == 6, "value: %d", entry_b->value);
        DQN_TESTER_ASSERTF(&test, entry_b->next == nullptr, "next: %p", entry_b->next);
        Dqn_Arena_Free(&arena, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Add r-value item and get them back out again");
        Dqn_Map<int>       map   = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_MapEntry<int> *entry_a = Dqn_Map_AddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_MapEntry<int> *entry_b = Dqn_Map_AddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Chain);

        Dqn_MapEntry<int> *entry_a_copy = Dqn_Map_Get(&map, 3 /*hash*/);
        Dqn_MapEntry<int> *entry_b_copy = Dqn_Map_Get(&map, 4 /*hash*/);
        DQN_TESTER_ASSERTF(&test, map.size == 1, "size: %zu", map.size);
        DQN_TESTER_ASSERTF(&test, map.count == 1, "count: %zu", map.count);
        DQN_TESTER_ASSERTF(&test, map.chain_count == 1, "chain_count: %zu", map.chain_count);
        DQN_TESTER_ASSERTF(&test, map.free_list == nullptr, "free_list: %p", map.free_list);
        DQN_TESTER_ASSERT(&test, entry_a_copy == entry_a);
        DQN_TESTER_ASSERT(&test, entry_b_copy == entry_b);
        Dqn_Arena_Free(&arena, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Add r-value item and erase it");
        Dqn_Map<int>       map   = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_Map_AddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_Map_AddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Chain);
        Dqn_Map_Get(&map, 3 /*hash*/);

        Dqn_Map_Erase(&map, 3 /*hash*/, Dqn_ZeroMem_No);
        DQN_TESTER_ASSERTF(&test, map.size == 1, "size: %zu", map.size);
        DQN_TESTER_ASSERTF(&test, map.count == 1, "count: %zu", map.count);
        DQN_TESTER_ASSERTF(&test, map.chain_count == 0, "chain_count: %zu", map.chain_count);
        DQN_TESTER_ASSERTF(&test, map.free_list != nullptr, "free_list: %p", map.free_list);

        DQN_TESTER_ASSERTF(&test, map.free_list->hash == 3, "Entry should not be zeroed out on erase");
        DQN_TESTER_ASSERTF(&test, map.free_list->value == 5, "Entry should not be zeroed out on erase");
        DQN_TESTER_ASSERTF(&test, map.free_list->next == nullptr, "This should be the first and only entry in the free list");

        Dqn_MapEntry<int> *entry = Dqn_Map_Get(&map, 4 /*hash*/);
        DQN_TESTER_ASSERTF(&test, entry->hash == 4, "hash: %zu", entry->hash);
        DQN_TESTER_ASSERTF(&test, entry->value == 6, "value: %d", entry->value);
        DQN_TESTER_ASSERTF(&test, entry->next == nullptr, "next: %p", entry->next);

        Dqn_Arena_Free(&arena, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Add r-value item and erase it, zeroing the memory out");
        Dqn_Map<int>       map   = Dqn_Map_InitWithArena<int>(&arena, 1);
        Dqn_Map_AddCopy(&map, 3 /*hash*/, 5, Dqn_MapCollideRule::Overwrite);
        Dqn_Map_AddCopy(&map, 4 /*hash*/, 6, Dqn_MapCollideRule::Chain);
        Dqn_Map_Get(&map, 3 /*hash*/);

        Dqn_Map_Erase(&map, 3 /*hash*/, Dqn_ZeroMem_Yes);
        DQN_TESTER_ASSERTF(&test, map.size == 1, "size: %zu", map.size);
        DQN_TESTER_ASSERTF(&test, map.count == 1, "count: %zu", map.count);
        DQN_TESTER_ASSERTF(&test, map.chain_count == 0, "chain_count: %zu", map.chain_count);
        DQN_TESTER_ASSERTF(&test, map.free_list != nullptr, "free_list: %p", map.free_list);

        DQN_TESTER_ASSERTF(&test, map.free_list->hash == 0, "Entry should be zeroed out on erase");
        DQN_TESTER_ASSERTF(&test, map.free_list->value == 0, "Entry should be zeroed out on erase");
        DQN_TESTER_ASSERTF(&test, map.free_list->next == nullptr, "This should be the first and only entry in the free list");

        Dqn_MapEntry<int> *entry = Dqn_Map_Get(&map, 4 /*hash*/);
        DQN_TESTER_ASSERTF(&test, entry->hash == 4, "hash: %zu", entry->hash);
        DQN_TESTER_ASSERTF(&test, entry->value == 6, "value: %d", entry->value);
        DQN_TESTER_ASSERTF(&test, entry->next == nullptr, "next: %p", entry->next);

        Dqn_Arena_Free(&arena, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }

    // TODO(dqn): Test free list is chained correctly
    // TODO(dqn): Test deleting 'b' from the list in the situation [map] - [a]->[b], we currently only test deleting a
    DQN_TESTER_END_GROUP(&test);
#endif // DQN_WITH_MAP
    return test;
}

Dqn_Tester Dqn_Test_Intrinsics()
{
    Dqn_Tester test = {};
    // TODO(dqn): We don't have meaningful tests here, but since
    // atomics/intrinsics are implemented using macros we ensure the macro was
    // written properly with these tests.

    DQN_TESTER_BEGIN_GROUP("Dqn_Atomic_");
    {
        Dqn_TesterBegin(&test, "Dqn_Atomic_AddU32");
        uint32_t val = 0;
        Dqn_Atomic_AddU32(&val, 1);
        DQN_TESTER_ASSERTF(&test, val == 1, "val: %u", val);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Dqn_Atomic_AddU64");
        uint64_t val = 0;
        Dqn_Atomic_AddU64(&val, 1);
        DQN_TESTER_ASSERTF(&test, val == 1, "val: %zu", val);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Dqn_Atomic_SubU32");
        uint32_t val = 1;
        Dqn_Atomic_SubU32(&val, 1);
        DQN_TESTER_ASSERTF(&test, val == 0, "val: %u", val);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Dqn_Atomic_SubU64");
        uint64_t val = 1;
        Dqn_Atomic_SubU64(&val, 1);
        DQN_TESTER_ASSERTF(&test, val == 0, "val: %zu", val);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Dqn_Atomic_SetValue32");
        long a = 0;
        long b = 111;
        Dqn_Atomic_SetValue32(&a, b);
        DQN_TESTER_ASSERTF(&test, a == b, "a: %lu, b: %lu", a, b);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Dqn_Atomic_SetValue64");
        int64_t a = 0;
        int64_t b = 111;
        Dqn_Atomic_SetValue64(DQN_CAST(uint64_t *)&a, b);
        DQN_TESTER_ASSERTF(&test, a == b, "a: %I64i, b: %I64i", a, b);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Dqn_CPUClockCycle");
        Dqn_CPUClockCycle();
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Dqn_CompilerReadBarrierAndCPUReadFence");
        Dqn_CompilerReadBarrierAndCPUReadFence;
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Dqn_CompilerWriteBarrierAndCPUWriteFence");
        Dqn_CompilerWriteBarrierAndCPUWriteFence;
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
    return test;
}

Dqn_Tester Dqn_Test_Rect()
{
    Dqn_Tester test = {};
#if defined(DQN_WITH_MATH)
    DQN_TESTER_BEGIN_GROUP("Dqn_Rect");
    // NOTE: Dqn_Rect_Intersection
    {
        {
            Dqn_TesterBegin(&test, "No intersection");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(0, 0), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(200, 0), Dqn_V2(200, 200));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TESTER_ASSERTF(&test,
                            ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 0 && ab.max.y == 0,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
            Dqn_TesterEnd(&test);
        }

        {
            Dqn_TesterBegin(&test, "A's min intersects B");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(50, 50), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TESTER_ASSERTF(&test,
                            ab.min.x == 50 && ab.min.y == 50 && ab.max.x == 100 && ab.max.y == 100,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
            Dqn_TesterEnd(&test);
        }

        {
            Dqn_TesterBegin(&test, "B's min intersects A");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(50, 50), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TESTER_ASSERTF(&test,
                            ab.min.x == 50 && ab.min.y == 50 && ab.max.x == 100 && ab.max.y == 100,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
            Dqn_TesterEnd(&test);
        }

        {
            Dqn_TesterBegin(&test, "A's max intersects B");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(-50, -50), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(  0,   0), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TESTER_ASSERTF(&test,
                            ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 50 && ab.max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
            Dqn_TesterEnd(&test);
        }

        {
            Dqn_TesterBegin(&test, "B's max intersects A");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(  0,   0), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(-50, -50), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TESTER_ASSERTF(&test,
                            ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 50 && ab.max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
            Dqn_TesterEnd(&test);
        }


        {
            Dqn_TesterBegin(&test, "B contains A");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(25, 25), Dqn_V2( 25,  25));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TESTER_ASSERTF(&test,
                            ab.min.x == 25 && ab.min.y == 25 && ab.max.x == 50 && ab.max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
            Dqn_TesterEnd(&test);
        }

        {
            Dqn_TesterBegin(&test, "A contains B");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2( 0,  0), Dqn_V2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(25, 25), Dqn_V2( 25,  25));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TESTER_ASSERTF(&test,
                            ab.min.x == 25 && ab.min.y == 25 && ab.max.x == 50 && ab.max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
            Dqn_TesterEnd(&test);
        }

        {
            Dqn_TesterBegin(&test, "A equals B");
            Dqn_Rect a  = Dqn_Rect_InitFromPosAndSize(Dqn_V2(0, 0), Dqn_V2(100, 100));
            Dqn_Rect b  = a;
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            DQN_TESTER_ASSERTF(&test,
                            ab.min.x == 0 && ab.min.y == 0 && ab.max.x == 100 && ab.max.y == 100,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.min.x,
                            ab.min.y,
                            ab.max.x,
                            ab.max.y);
            Dqn_TesterEnd(&test);
        }
    }
    DQN_TESTER_END_GROUP(&test);
#endif // DQN_WITH_MATH
    return test;
}

Dqn_Tester Dqn_Test_PerfCounter()
{
    Dqn_Tester test = {};
    DQN_TESTER_BEGIN_GROUP("Dqn_Perf_Counter");
    {
        Dqn_TesterBegin(&test, "Dqn_Perf_CounterNow");
        uint64_t result = Dqn_Perf_CounterNow();
        DQN_TESTER_ASSERT(&test, result != 0);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Consecutive ticks are ordered");
        uint64_t a = Dqn_Perf_CounterNow();
        uint64_t b = Dqn_Perf_CounterNow();
        DQN_TESTER_ASSERTF(&test, b >= a, "a: %zu, b: %zu", a, b);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Ticks to time are a correct order of magnitude");
        uint64_t a = Dqn_Perf_CounterNow();
        uint64_t b = Dqn_Perf_CounterNow();

        Dqn_f64 s       = Dqn_Perf_CounterS(a, b);
        Dqn_f64 ms      = Dqn_Perf_CounterMs(a, b);
        Dqn_f64 micro_s = Dqn_Perf_CounterMicroS(a, b);
        Dqn_f64 ns      = Dqn_Perf_CounterNs(a, b);
        DQN_TESTER_ASSERTF(&test, s <= ms, "s: %f, ms: %f", s, ms);
        DQN_TESTER_ASSERTF(&test, ms <= micro_s, "ms: %f, micro_s: %f", ms, micro_s);
        DQN_TESTER_ASSERTF(&test, micro_s <= ns, "micro_s: %f, ns: %f", micro_s, ns);
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
    return test;
}

Dqn_Tester Dqn_Test_OS()
{
    Dqn_Tester test = {};
    DQN_TESTER_BEGIN_GROUP("Dqn_OS_");
    {
        Dqn_TesterBegin(&test, "Generate secure RNG bytes with nullptr");
        Dqn_b32 result = Dqn_OS_SecureRNGBytes(nullptr, 1);
        DQN_TESTER_ASSERT(&test, result == false);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Generate secure RNG 32 bytes");
        char const ZERO[32] = {};
        char       buf[32]  = {};
        Dqn_b32 result = Dqn_OS_SecureRNGBytes(buf, Dqn_CArray_CountI(buf));
        DQN_TESTER_ASSERT(&test, result);
        DQN_TESTER_ASSERT(&test, DQN_MEMCMP(buf, ZERO, Dqn_CArray_Count(buf)) != 0);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Generate secure RNG 0 bytes");
        char buf[32] = {};
        buf[0] = 'Z';
        Dqn_b32 result = Dqn_OS_SecureRNGBytes(buf, 0);
        DQN_TESTER_ASSERT(&test, result);
        DQN_TESTER_ASSERT(&test, buf[0] == 'Z');
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Query executable directory");
        Dqn_Arena arena = {};
        Dqn_String8 result = Dqn_OS_EXEDir(Dqn_Arena_Allocator(&arena));
        DQN_TESTER_ASSERT(&test, Dqn_String8_IsValid(result));
        DQN_TESTER_ASSERTF(&test, Dqn_Fs_DirExists(result), "result(%zu): %.*s", result.size, DQN_STRING_FMT(result));
        Dqn_Arena_Free(&arena, Dqn_ZeroMem_No);
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
    return test;
}

Dqn_Tester Dqn_Test_Str()
{
    Dqn_Tester test = {};
    DQN_TESTER_BEGIN_GROUP("Dqn_Str");
    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_CString8_ToI64
    // ---------------------------------------------------------------------------------------------
    {
        Dqn_TesterBegin(&test, "To I64: Convert nullptr");
        int64_t result = Dqn_CString8_ToI64(nullptr);
        DQN_TESTER_ASSERT(&test, result == 0);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To I64: Convert empty string");
        int64_t result = Dqn_CString8_ToI64("");
        DQN_TESTER_ASSERT(&test, result == 0);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To I64: Convert \"1\"");
        int64_t result = Dqn_CString8_ToI64("1");
        DQN_TESTER_ASSERT(&test, result == 1);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To I64: Convert \"-0\"");
        int64_t result = Dqn_CString8_ToI64("-0");
        DQN_TESTER_ASSERT(&test, result == 0);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To I64: Convert \"-1\"");
        int64_t result = Dqn_CString8_ToI64("-1");
        DQN_TESTER_ASSERT(&test, result == -1);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To I64: Convert \"1.2\"");
        int64_t result = Dqn_CString8_ToI64("1.2");
        DQN_TESTER_ASSERT(&test, result == 1);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To I64: Convert \"1,234\"");
        int64_t result = Dqn_CString8_ToI64("1,234");
        DQN_TESTER_ASSERT(&test, result == 1234);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To I64: Convert \"1,2\"");
        int64_t result = Dqn_CString8_ToI64("1,2");
        DQN_TESTER_ASSERT(&test, result == 12);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To I64: Convert \"12a3\"");
        int64_t result = Dqn_CString8_ToI64("12a3");
        DQN_TESTER_ASSERT(&test, result == 12);
        Dqn_TesterEnd(&test);
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_CString8_ToU64
    // ---------------------------------------------------------------------------------------------
    {
        Dqn_TesterBegin(&test, "To U64: Convert nullptr");
        uint64_t result = Dqn_CString8_ToU64(nullptr);
        DQN_TESTER_ASSERTF(&test, result == 0, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To U64: Convert empty string");
        uint64_t result = Dqn_CString8_ToU64("");
        DQN_TESTER_ASSERTF(&test, result == 0, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To U64: Convert \"1\"");
        uint64_t result = Dqn_CString8_ToU64("1");
        DQN_TESTER_ASSERTF(&test, result == 1, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To U64: Convert \"-0\"");
        uint64_t result = Dqn_CString8_ToU64("-0");
        DQN_TESTER_ASSERTF(&test, result == 0, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To U64: Convert \"-1\"");
        uint64_t result = Dqn_CString8_ToU64("-1");
        DQN_TESTER_ASSERTF(&test, result == 0, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To U64: Convert \"1.2\"");
        uint64_t result = Dqn_CString8_ToU64("1.2");
        DQN_TESTER_ASSERTF(&test, result == 1, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To U64: Convert \"1,234\"");
        uint64_t result = Dqn_CString8_ToU64("1,234");
        DQN_TESTER_ASSERTF(&test, result == 1234, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To U64: Convert \"1,2\"");
        uint64_t result = Dqn_CString8_ToU64("1,2");
        DQN_TESTER_ASSERTF(&test, result == 12, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "To U64: Convert \"12a3\"");
        uint64_t result = Dqn_CString8_ToU64("12a3");
        DQN_TESTER_ASSERTF(&test, result == 12, "result: %zu", result);
        Dqn_TesterEnd(&test);
    }

    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_CString8_Find
    // ---------------------------------------------------------------------------------------------
    {
        Dqn_TesterBegin(&test, "Find: String (char) is not in buffer");
        char const buf[]  = "836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55";
        char const find[] = "2";
        char const *result = Dqn_CString8_Find(buf, find, Dqn_CString8_ArrayCountI(buf), Dqn_CString8_ArrayCountI(find));
        DQN_TESTER_ASSERT(&test, result == nullptr);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Find: String (char) is in buffer");
        char const buf[]  = "836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55";
        char const find[] = "6";
        char const *result = Dqn_CString8_Find(buf, find, Dqn_CString8_ArrayCountI(buf), Dqn_CString8_ArrayCountI(find));
        DQN_TESTER_ASSERT(&test, result != nullptr);
        DQN_TESTER_ASSERT(&test, result[0] == '6' && result[1] == 'a');
        Dqn_TesterEnd(&test);
    }

    // NOTE: Dqn_CString8_FileNameFromPath
    // ---------------------------------------------------------------------------------------------
    {
        Dqn_TesterBegin(&test, "File name from Windows path");
        Dqn_isize file_name_size = 0;
        char const buf[]  = "C:\\ABC\\test.exe";
        char const *result = Dqn_CString8_FileNameFromPath(buf, Dqn_CString8_ArrayCountI(buf), &file_name_size);
        DQN_TESTER_ASSERTF(&test, file_name_size == 8, "size: %I64d", file_name_size);
        DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(Dqn_String8_Init(result, file_name_size), DQN_STRING8("test.exe")), "%.*s", DQN_CAST(int)file_name_size, result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "File name from Linux path");
        Dqn_isize file_name_size = 0;
        char const buf[]  = "/ABC/test.exe";
        char const *result = Dqn_CString8_FileNameFromPath(buf, Dqn_CString8_ArrayCountI(buf), &file_name_size);
        DQN_TESTER_ASSERTF(&test, file_name_size == 8, "size: %I64d", file_name_size);
        DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(Dqn_String8_Init(result, file_name_size), DQN_STRING8("test.exe")), "%.*s", (int)file_name_size, result);
        Dqn_TesterEnd(&test);
    }

    // NOTE: Dqn_CString8_TrimPrefix
    // ---------------------------------------------------------------------------------------------
    {
        Dqn_TesterBegin(&test, "Trim prefix");
        char const  prefix[]     = "@123";
        char const  buf[]        = "@123string";
        Dqn_isize   trimmed_size = 0;
        char const *result       = Dqn_CString8_TrimPrefix(buf, Dqn_CString8_ArrayCountI(buf), prefix, Dqn_CString8_ArrayCountI(prefix), &trimmed_size);
        DQN_TESTER_ASSERTF(&test, trimmed_size == 6, "size: %I64d", trimmed_size);
        DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(Dqn_String8_Init(result, trimmed_size), DQN_STRING8("string")), "%.*s", (int)trimmed_size, result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Trim prefix, nullptr trimmed size");
        char const  prefix[]     = "@123";
        char const  buf[]        = "@123string";
        char const *result = Dqn_CString8_TrimPrefix(buf, Dqn_CString8_ArrayCountI(buf), prefix, Dqn_CString8_ArrayCountI(prefix), nullptr);
        DQN_TESTER_ASSERT(&test, result);
        Dqn_TesterEnd(&test);
    }

    // NOTE: Dqn_CString8_IsAllDigits
    // ---------------------------------------------------------------------------------------------
    {
        Dqn_TesterBegin(&test, "Is all digits fails on non-digit string");
        char const buf[]  = "@123string";
        Dqn_b32    result = Dqn_CString8_IsAllDigits(buf, Dqn_CString8_ArrayCountI(buf));
        DQN_TESTER_ASSERT(&test, result == false);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Is all digits fails on nullptr");
        Dqn_b32 result = Dqn_CString8_IsAllDigits(nullptr, 0);
        DQN_TESTER_ASSERT(&test, result == false);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Is all digits fails on nullptr w/ size");
        Dqn_b32 result = Dqn_CString8_IsAllDigits(nullptr, 1);
        DQN_TESTER_ASSERT(&test, result == false);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Is all digits fails on 0 size w/ string");
        char const buf[]  = "@123string";
        Dqn_b32    result = Dqn_CString8_IsAllDigits(buf, 0);
        DQN_TESTER_ASSERT(&test, result == false);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Is all digits success");
        char const buf[]  = "23";
        Dqn_b32    result = Dqn_CString8_IsAllDigits(buf, Dqn_CString8_ArrayCountI(buf));
        DQN_TESTER_ASSERT(&test, DQN_CAST(bool)result == true);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Is all digits fails on whitespace");
        char const buf[]  = "23 ";
        Dqn_b32    result = Dqn_CString8_IsAllDigits(buf, Dqn_CString8_ArrayCountI(buf));
        DQN_TESTER_ASSERT(&test, DQN_CAST(bool)result == false);
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
    return test;
}

Dqn_Tester Dqn_Test_String()
{
    Dqn_Tester test = {};
    DQN_TESTER_BEGIN_GROUP("Dqn_String8");
    {
        Dqn_TesterBegin(&test, "Initialise with string literal w/ macro");
        Dqn_String8 string = DQN_STRING8("AB");
        DQN_TESTER_ASSERTF(&test, string.size == 2, "size: %I64d", string.size);
        DQN_TESTER_ASSERTF(&test, string.data[0] == 'A', "string[0]: %c", string.data[0]);
        DQN_TESTER_ASSERTF(&test, string.data[1] == 'B', "string[1]: %c", string.data[1]);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Initialise with format string");
        Dqn_Arena arena   = {};
        Dqn_String8 string = Dqn_String8_Fmt(Dqn_Arena_Allocator(&arena), "%s", "AB");
        DQN_TESTER_ASSERTF(&test, string.size == 2, "size: %I64d", string.size);
        DQN_TESTER_ASSERTF(&test, string.data[0] == 'A', "string[0]: %c", string.data[0]);
        DQN_TESTER_ASSERTF(&test, string.data[1] == 'B', "string[1]: %c", string.data[1]);
        DQN_TESTER_ASSERTF(&test, string.data[2] == 0, "string[2]: %c", string.data[2]);
        Dqn_Arena_Free(&arena, false /*clear_mem*/);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Copy string");
        Dqn_Arena arena  = {};
        Dqn_String8         string = DQN_STRING8("AB");
        Dqn_String8         copy   = Dqn_String8_Copy(Dqn_Arena_Allocator(&arena), string);
        DQN_TESTER_ASSERTF(&test, copy.size == 2, "size: %I64d", copy.size);
        DQN_TESTER_ASSERTF(&test, copy.data[0] == 'A', "copy[0]: %c", copy.data[0]);
        DQN_TESTER_ASSERTF(&test, copy.data[1] == 'B', "copy[1]: %c", copy.data[1]);
        DQN_TESTER_ASSERTF(&test, copy.data[2] ==  0,  "copy[2]: %c", copy.data[2]);
        Dqn_Arena_Free(&arena, false /*clear_mem*/);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Trim whitespace around string");
        Dqn_String8         string = Dqn_String8_TrimWhitespaceAround(DQN_STRING8(" AB "));
        DQN_TESTER_ASSERTF(&test, string.size == 2, "size: %I64d", string.size);
        DQN_TESTER_ASSERTF(&test, string.data[0] == 'A', "string[0]: %c", string.data[0]);
        DQN_TESTER_ASSERTF(&test, string.data[1] == 'B', "string[1]: %c", string.data[1]);
        DQN_TESTER_ASSERTF(&test, string.data[2] == ' ', "string[1]: %c", string.data[1]);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Allocate string from arena");
        Dqn_Arena arena   = {};
        Dqn_String8 string = Dqn_String8_Allocate(Dqn_Arena_Allocator(&arena), 2, Dqn_ZeroMem_No);
        DQN_TESTER_ASSERTF(&test, string.size == 2, "size: %I64d", string.size);
        Dqn_Arena_Free(&arena, false /*clear_mem*/);
        Dqn_TesterEnd(&test);
    }

    // NOTE: Dqn_CString8_Trim[Prefix/Suffix]
    // ---------------------------------------------------------------------------------------------
    {
        Dqn_TesterBegin(&test, "Trim prefix with matching prefix");
        Dqn_String8 input  = DQN_STRING8("nft/abc");
        Dqn_String8 result = Dqn_String8_TrimPrefix(input, DQN_STRING8("nft/"));
        DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(result, DQN_STRING8("abc")), "%.*s", DQN_STRING_FMT(result));
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Trim prefix with non matching prefix");
        Dqn_String8 input  = DQN_STRING8("nft/abc");
        Dqn_String8 result = Dqn_String8_TrimPrefix(input, DQN_STRING8(" ft/"));
        DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(result, input), "%.*s", DQN_STRING_FMT(result));
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Trim suffix with matching suffix");
        Dqn_String8 input  = DQN_STRING8("nft/abc");
        Dqn_String8 result = Dqn_String8_TrimSuffix(input, DQN_STRING8("abc"));
        DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(result, DQN_STRING8("nft/")), "%.*s", DQN_STRING_FMT(result));
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Trim suffix with non matching suffix");
        Dqn_String8 input  = DQN_STRING8("nft/abc");
        Dqn_String8 result = Dqn_String8_TrimSuffix(input, DQN_STRING8("ab"));
        DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(result, input), "%.*s", DQN_STRING_FMT(result));
        Dqn_TesterEnd(&test);
    }


    // ---------------------------------------------------------------------------------------------
    // NOTE: Dqn_String8_IsAllDigits
    // ---------------------------------------------------------------------------------------------
    {
        Dqn_TesterBegin(&test, "Is all digits fails on non-digit string");
        Dqn_b32 result = Dqn_String8_IsAllDigits(DQN_STRING8("@123string"));
        DQN_TESTER_ASSERT(&test, result == false);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Is all digits fails on nullptr");
        Dqn_b32 result = Dqn_String8_IsAllDigits(Dqn_String8_Init(nullptr, 0));
        DQN_TESTER_ASSERT(&test, result == false);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Is all digits fails on nullptr w/ size");
        Dqn_b32 result = Dqn_String8_IsAllDigits(Dqn_String8_Init(nullptr, 1));
        DQN_TESTER_ASSERT(&test, result == false);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Is all digits fails on string w/ 0 size");
        char const buf[]  = "@123string";
        Dqn_b32    result = Dqn_String8_IsAllDigits(Dqn_String8_Init(buf, 0));
        DQN_TESTER_ASSERT(&test, result == false);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Is all digits success");
        Dqn_b32 result = Dqn_String8_IsAllDigits(DQN_STRING8("23"));
        DQN_TESTER_ASSERT(&test, DQN_CAST(bool)result == true);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Is all digits fails on whitespace");
        Dqn_b32 result = Dqn_String8_IsAllDigits(DQN_STRING8("23 "));
        DQN_TESTER_ASSERT(&test, DQN_CAST(bool)result == false);
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
    return test;
}

Dqn_Tester Dqn_Test_TicketMutex()
{
    Dqn_Tester test = {};
    DQN_TESTER_BEGIN_GROUP("Dqn_TicketMutex");
    {
        // TODO: We don't have a meaningful test but since atomics are
        // implemented with a macro this ensures that we test that they are
        // written correctly.
        Dqn_TesterBegin(&test, "Ticket mutex start and stop");
        Dqn_TicketMutex mutex = {};
        Dqn_TicketMutex_Begin(&mutex);
        Dqn_TicketMutex_End(&mutex);
        DQN_TESTER_ASSERT(&test, mutex.ticket == mutex.serving);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "Ticket mutex start and stop w/ advanced API");
        Dqn_TicketMutex mutex = {};
        unsigned int ticket_a = Dqn_TicketMutex_MakeTicket(&mutex);
        unsigned int ticket_b = Dqn_TicketMutex_MakeTicket(&mutex);
        DQN_TESTER_ASSERT(&test, DQN_CAST(bool)Dqn_TicketMutex_CanLock(&mutex, ticket_b) == false);
        DQN_TESTER_ASSERT(&test, DQN_CAST(bool)Dqn_TicketMutex_CanLock(&mutex, ticket_a) == true);

        Dqn_TicketMutex_BeginTicket(&mutex, ticket_a);
        Dqn_TicketMutex_End(&mutex);
        Dqn_TicketMutex_BeginTicket(&mutex, ticket_b);
        Dqn_TicketMutex_End(&mutex);

        DQN_TESTER_ASSERT(&test, mutex.ticket == mutex.serving);
        DQN_TESTER_ASSERT(&test, mutex.ticket == ticket_b + 1);
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
    return test;
}

Dqn_Tester Dqn_Test_Win()
{
    Dqn_Tester test = {};
#if defined(DQN_OS_WIN32)
    DQN_TESTER_BEGIN_GROUP("Dqn_Win");
    {
        Dqn_TesterBegin(&test, "String8 to String16 size required");
        int result = Dqn_Win_String8ToCString16(DQN_STRING8("a"), nullptr, 0);
        DQN_TESTER_ASSERTF(&test, result == 2, "Size returned: %d. This size should include the null-terminator", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "String16 to String8 size required");
        int result = Dqn_Win_String16ToCString8(DQN_STRING16(L"a"), nullptr, 0);
        DQN_TESTER_ASSERTF(&test, result == 2, "Size returned: %d. This size should include the null-terminator", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "String8 to String16 size required");
        int result = Dqn_Win_String8ToCString16(DQN_STRING8("String"), nullptr, 0);
        DQN_TESTER_ASSERTF(&test, result == 7, "Size returned: %d. This size should include the null-terminator", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "String16 to String8 size required");
        int result = Dqn_Win_String16ToCString8(DQN_STRING16(L"String"), nullptr, 0);
        DQN_TESTER_ASSERTF(&test, result == 7, "Size returned: %d. This size should include the null-terminator", result);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "String8 to String16");
        Dqn_Arena arena        = {};
        Dqn_String8 const INPUT = DQN_STRING8("String");
        int size_required      = Dqn_Win_String8ToCString16(INPUT, nullptr, 0);
        wchar_t *string        = Dqn_Arena_NewArray(&arena, wchar_t, size_required, Dqn_ZeroMem_No);

        // Fill the string with error sentinels, which ensures the string is zero terminated
        DQN_MEMSET(string, 'Z', size_required);

        int size_returned = Dqn_Win_String8ToCString16(INPUT, string, size_required);
        wchar_t const EXPECTED[] = {L'S', L't', L'r', L'i', L'n', L'g', 0};

        DQN_TESTER_ASSERTF(&test, size_required == size_returned, "string_size: %d, result: %d", size_required, size_returned);
        DQN_TESTER_ASSERTF(&test, size_returned == Dqn_CArray_Count(EXPECTED), "string_size: %d, expected: %zu", size_returned, Dqn_CArray_Count(EXPECTED));
        DQN_TESTER_ASSERT(&test, DQN_MEMCMP(EXPECTED, string, sizeof(EXPECTED)) == 0);
        Dqn_Arena_Free(&arena, false /*clear_mem*/);
        Dqn_TesterEnd(&test);
    }

    {
        Dqn_TesterBegin(&test, "String16 to String8: No null-terminate");
        Dqn_Arena arena    = {};
        Dqn_String16 INPUT = DQN_STRING16(L"String");
        int size_required  = Dqn_Win_String16ToCString8(INPUT, nullptr, 0);
        char *string       = Dqn_Arena_NewArray(&arena, char, size_required, Dqn_ZeroMem_No);

        // Fill the string with error sentinels, which ensures the string is zero terminated
        DQN_MEMSET(string, 'Z', size_required);

        int size_returned = Dqn_Win_String16ToCString8(INPUT, string, size_required);
        char const EXPECTED[] = {'S', 't', 'r', 'i', 'n', 'g', 0};

        DQN_TESTER_ASSERTF(&test, size_required == size_returned, "string_size: %d, result: %d", size_required, size_returned);
        DQN_TESTER_ASSERTF(&test, size_returned == Dqn_CArray_Count(EXPECTED), "string_size: %d, expected: %zu", size_returned, Dqn_CArray_Count(EXPECTED));
        DQN_TESTER_ASSERT(&test, DQN_MEMCMP(EXPECTED, string, sizeof(EXPECTED)) == 0);
        Dqn_Arena_Free(&arena, false /*clear_mem*/);
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
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

Dqn_String8 const DQN_TESTS__HASH_STRING[] =
{
#define DQN_TESTS_HASH_X_ENTRY(enum_val, string) DQN_STRING8(string),
    DQN_TESTS_HASH_X_MACRO
#undef DQN_TESTS_HASH_X_ENTRY
};

void Dqn_Test__KeccakDispatch(Dqn_Tester *test, int hash_type, Dqn_String8 input)
{
#if defined(DQN_KECCAK_H)
    Dqn_ThreadTempArena scratch = Dqn_Thread_TempArena();
    Dqn_String8 input_hex = Dqn_Hex_BytesToHexStringArena(input.data, input.size, scratch.arena);

    switch(hash_type)
    {
        case Hash_SHA3_224:
        {
            Dqn_KeccakBytes28 hash = Dqn_SHA3_224StringToBytes28(input);
            Dqn_KeccakBytes28 expect;
            FIPS202_SHA3_224(DQN_CAST(u8 *)input.data, input.size, (u8 *)expect.data);
            DQN_TESTER_ASSERTF(test,
                                Dqn_KeccakBytes28Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING56_FMT(Dqn_KeccakBytes28ToHex(&hash).data),
                                DQN_KECCAK_STRING56_FMT(Dqn_KeccakBytes28ToHex(&expect).data));
        }
        break;

        case Hash_SHA3_256:
        {
            Dqn_KeccakBytes32 hash = Dqn_SHA3_256StringToBytes32(input);
            Dqn_KeccakBytes32 expect;
            FIPS202_SHA3_256(DQN_CAST(u8 *)input.data, input.size, (u8 *)expect.data);
            DQN_TESTER_ASSERTF(test,
                                Dqn_KeccakBytes32Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING64_FMT(Dqn_KeccakBytes32ToHex(&hash).data),
                                DQN_KECCAK_STRING64_FMT(Dqn_KeccakBytes32ToHex(&expect).data));
        }
        break;

        case Hash_SHA3_384:
        {
            Dqn_KeccakBytes48 hash = Dqn_SHA3_384StringToBytes48(input);
            Dqn_KeccakBytes48 expect;
            FIPS202_SHA3_384(DQN_CAST(u8 *)input.data, input.size, (u8 *)expect.data);
            DQN_TESTER_ASSERTF(test,
                                Dqn_KeccakBytes48Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING96_FMT(Dqn_KeccakBytes48ToHex(&hash).data),
                                DQN_KECCAK_STRING96_FMT(Dqn_KeccakBytes48ToHex(&expect).data));
        }
        break;

        case Hash_SHA3_512:
        {
            Dqn_KeccakBytes64 hash = Dqn_SHA3_512StringToBytes64(input);
            Dqn_KeccakBytes64 expect;
            FIPS202_SHA3_512(DQN_CAST(u8 *)input.data, input.size, (u8 *)expect.data);
            DQN_TESTER_ASSERTF(test,
                                Dqn_KeccakBytes64Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING128_FMT(Dqn_KeccakBytes64ToHex(&hash).data),
                                DQN_KECCAK_STRING128_FMT(Dqn_KeccakBytes64ToHex(&expect).data));
        }
        break;

        case Hash_Keccak_224:
        {
            Dqn_KeccakBytes28 hash = Dqn_Keccak224StringToBytes28(input);
            Dqn_KeccakBytes28 expect;
            Keccak(1152, 448, DQN_CAST(u8 *)input.data, input.size, 0x01, (u8 *)expect.data, sizeof(expect));
            DQN_TESTER_ASSERTF(test,
                                Dqn_KeccakBytes28Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING56_FMT(Dqn_KeccakBytes28ToHex(&hash).data),
                                DQN_KECCAK_STRING56_FMT(Dqn_KeccakBytes28ToHex(&expect).data));
        }
        break;

        case Hash_Keccak_256:
        {
            Dqn_KeccakBytes32 hash = Dqn_Keccak256StringToBytes32(input);
            Dqn_KeccakBytes32 expect;
            Keccak(1088, 512, DQN_CAST(u8 *)input.data, input.size, 0x01, (u8 *)expect.data, sizeof(expect));
            DQN_TESTER_ASSERTF(test,
                                Dqn_KeccakBytes32Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING64_FMT(Dqn_KeccakBytes32ToHex(&hash).data),
                                DQN_KECCAK_STRING64_FMT(Dqn_KeccakBytes32ToHex(&expect).data));
        }
        break;

        case Hash_Keccak_384:
        {
            Dqn_KeccakBytes48 hash = Dqn_Keccak384StringToBytes48(input);
            Dqn_KeccakBytes48 expect;
            Keccak(832, 768, DQN_CAST(u8 *)input.data, input.size, 0x01, (u8 *)expect.data, sizeof(expect));
            DQN_TESTER_ASSERTF(test,
                                Dqn_KeccakBytes48Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING96_FMT(Dqn_KeccakBytes48ToHex(&hash).data),
                                DQN_KECCAK_STRING96_FMT(Dqn_KeccakBytes48ToHex(&expect).data));
        }
        break;

        case Hash_Keccak_512:
        {
            Dqn_KeccakBytes64 hash = Dqn_Keccak512StringToBytes64(input);
            Dqn_KeccakBytes64 expect;
            Keccak(576, 1024, DQN_CAST(u8 *)input.data, input.size, 0x01, (u8 *)expect.data, sizeof(expect));
            DQN_TESTER_ASSERTF(test,
                                Dqn_KeccakBytes64Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STRING_FMT(input_hex),
                                DQN_KECCAK_STRING128_FMT(Dqn_KeccakBytes64ToHex(&hash).data),
                                DQN_KECCAK_STRING128_FMT(Dqn_KeccakBytes64ToHex(&expect).data));
        }
        break;

    }
#endif // DQN_KECCAK_H
}

Dqn_Tester Dqn_Test_Keccak()
{
    Dqn_Tester test = {};
#if defined(DQN_KECCAK_H)
    Dqn_String8 const INPUTS[] = {
        DQN_STRING8("abc"),
        DQN_STRING8(""),
        DQN_STRING8("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
        DQN_STRING8("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmno"
                    "pqrstnopqrstu"),
    };

    DQN_TESTER_BEGIN_GROUP("Dqn_Keccak");
    for (int hash_type = 0; hash_type < Hash_Count; hash_type++)
    {
        pcg32_random_t rng = {};
        pcg32_srandom_r(&rng, 0xd48e'be21'2af8'733d, 0x3f89'3bd2'd6b0'4eef);

        for (Dqn_String8 input : INPUTS)
        {
            Dqn_TesterBegin(&test, "%.*s - Input: %.*s", DQN_STRING_FMT(DQN_TESTS__HASH_STRING[hash_type]), DQN_CAST(int)DQN_MIN(input.size, 54), input.data);
            Dqn_Test__KeccakDispatch(&test, hash_type, input);
            Dqn_TesterEnd(&test);
        }

        Dqn_TesterBegin(&test, "%.*s - Deterministic random inputs", DQN_STRING_FMT(DQN_TESTS__HASH_STRING[hash_type]));
        for (int index = 0; index < 128; index++)
        {
            char    src[4096] = {};
            uint32_t src_size  = pcg32_boundedrand_r(&rng, sizeof(src));

            for (int src_index = 0; src_index < src_size; src_index++)
                src[src_index] = pcg32_boundedrand_r(&rng, 255);

            Dqn_String8 input = Dqn_String8_Init(src, src_size);
            Dqn_Test__KeccakDispatch(&test, hash_type, input);
        }
        Dqn_TesterEnd(&test);
    }
    DQN_TESTER_END_GROUP(&test);
#endif // DQN_KECCAK_H
    return test;
}

void Dqn_Test_RunSuite()
{
    Dqn_Tester tests[]
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
    for (Dqn_Tester &test : tests)
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

