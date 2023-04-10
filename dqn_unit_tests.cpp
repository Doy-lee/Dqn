// -------------------------------------------------------------------------------------------------
// NOTE: Preprocessor Config
// -------------------------------------------------------------------------------------------------
/*
#define DQN_TEST_WITH_MAIN   Define this to enable the main function and allow standalone compiling
                             and running of the file.
#define DQN_TEST_WITH_KECCAK Define this to enable the main function and allow standalone compiling
                             and running of the file.
*/

#if defined(DQN_TEST_WITH_MAIN)
    #define DQN_IMPLEMENTATION
    #include "dqn.h"
#endif

#if defined(DQN_TEST_WITH_KECCAK)
    #define DQN_KECCAK_IMPLEMENTATION
    #include "dqn_keccak.h"
    #include "dqn_tests_helpers.cpp"
#endif

#define DQN_TESTER_IMPLEMENTATION
#include "dqn_tester.h"

Dqn_Tester TestBinarySearch()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_BinarySearch") {
        DQN_TESTER_TEST("Search array of 1 item") {
            uint32_t array[] = {1};
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 0 /*find*/, Dqn_BinarySearchType_Match) == -1);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 1 /*find*/, Dqn_BinarySearchType_Match) == +0);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 2 /*find*/, Dqn_BinarySearchType_Match) == -1);

            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 0 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +0);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 1 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +1);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 2 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +1);
        }

        DQN_TESTER_TEST("Search array of 2 items") {
            uint32_t array[] = {1, 2};
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 0 /*find*/, Dqn_BinarySearchType_Match) == -1);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 1 /*find*/, Dqn_BinarySearchType_Match) == +0);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 2 /*find*/, Dqn_BinarySearchType_Match) == +1);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 3 /*find*/, Dqn_BinarySearchType_Match) == -1);

            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 0 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +0);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 1 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +1);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 2 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +2);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 3 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +2);
        }

        DQN_TESTER_TEST("Search array of 3 items") {
            uint32_t array[] = {1, 2, 3};
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 0 /*find*/, Dqn_BinarySearchType_Match) == -1);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 1 /*find*/, Dqn_BinarySearchType_Match) == +0);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 2 /*find*/, Dqn_BinarySearchType_Match) == +1);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 3 /*find*/, Dqn_BinarySearchType_Match) == +2);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 4 /*find*/, Dqn_BinarySearchType_Match) == -1);

            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 0 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +0);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 1 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +1);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 2 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +2);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 3 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +3);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 4 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +3);
        }

        DQN_TESTER_TEST("Search array of 4 items") {
            uint32_t array[] = {1, 2, 3, 4};
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 0 /*find*/, Dqn_BinarySearchType_Match) == -1);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 1 /*find*/, Dqn_BinarySearchType_Match) == +0);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 2 /*find*/, Dqn_BinarySearchType_Match) == +1);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 3 /*find*/, Dqn_BinarySearchType_Match) == +2);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 4 /*find*/, Dqn_BinarySearchType_Match) == +3);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 5 /*find*/, Dqn_BinarySearchType_Match) == -1);

            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 0 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +0);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 1 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +1);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 2 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +2);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 3 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +3);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 4 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +4);
            DQN_TESTER_ASSERT(&test, Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_ICOUNT(array), 5 /*find*/, Dqn_BinarySearchType_OnePastMatch) == +4);
        }
    }
    return test;
}

Dqn_Tester TestFs()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_Fs") {
        DQN_TESTER_TEST("Make directory recursive \"abcd/efgh\"") {
            DQN_TESTER_ASSERTF(&test, Dqn_Fs_MakeDir(DQN_STRING8("abcd/efgh")), "Failed to make directory");
            DQN_TESTER_ASSERTF(&test, Dqn_Fs_DirExists(DQN_STRING8("abcd")), "Directory was not made");
            DQN_TESTER_ASSERTF(&test, Dqn_Fs_DirExists(DQN_STRING8("abcd/efgh")), "Subdirectory was not made");
            DQN_TESTER_ASSERTF(&test, Dqn_Fs_Exists(DQN_STRING8("abcd")) == false, "This function should only return true for files");
            DQN_TESTER_ASSERTF(&test, Dqn_Fs_Exists(DQN_STRING8("abcd/efgh")) == false, "This function should only return true for files");
            DQN_TESTER_ASSERTF(&test, Dqn_Fs_Delete(DQN_STRING8("abcd/efgh")), "Failed to delete directory");
            DQN_TESTER_ASSERTF(&test, Dqn_Fs_Delete(DQN_STRING8("abcd")), "Failed to cleanup directory");
        }

        DQN_TESTER_TEST("Write file, read it, copy it, move it and delete it") {
            // NOTE: Write step
            Dqn_String8 const SRC_FILE = DQN_STRING8("dqn_test_file");
            Dqn_b32 write_result = Dqn_Fs_WriteCString8(SRC_FILE.data, SRC_FILE.size, "test", 4);
            DQN_TESTER_ASSERT(&test, write_result);
            DQN_TESTER_ASSERT(&test, Dqn_Fs_Exists(SRC_FILE));

            // NOTE: Read step
            Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
            Dqn_String8 read_file = Dqn_Fs_ReadString8(SRC_FILE, scratch.allocator);
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
        }
    }
    return test;
}

Dqn_Tester TestFixedArray()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_FArray") {
        DQN_TESTER_TEST("Initialise from raw array") {
            int raw_array[] = {1, 2};
            auto array = Dqn_FArray_Init<int, 4>(raw_array, DQN_ARRAY_ICOUNT(raw_array));
            DQN_TESTER_ASSERT(&test, array.size == 2);
            DQN_TESTER_ASSERT(&test, array.data[0] == 1);
            DQN_TESTER_ASSERT(&test, array.data[1] == 2);
        }

        DQN_TESTER_TEST("Erase stable 1 element from array") {
            int raw_array[] = {1, 2, 3};
            auto array = Dqn_FArray_Init<int, 4>(raw_array, DQN_ARRAY_ICOUNT(raw_array));
            Dqn_FArray_EraseStable(&array, 1);
            DQN_TESTER_ASSERT(&test, array.size == 2);
            DQN_TESTER_ASSERT(&test, array.data[0] == 1);
            DQN_TESTER_ASSERT(&test, array.data[1] == 3);
        }

        DQN_TESTER_TEST("Erase unstable 1 element from array") {
            int raw_array[] = {1, 2, 3};
            auto array = Dqn_FArray_Init<int, 4>(raw_array, DQN_ARRAY_ICOUNT(raw_array));
            DQN_TESTER_ASSERT(&test, Dqn_FArray_EraseUnstable(&array, 0));
            DQN_TESTER_ASSERT(&test, array.size == 2);
            DQN_TESTER_ASSERT(&test, array.data[0] == 3);
            DQN_TESTER_ASSERT(&test, array.data[1] == 2);
        }

        DQN_TESTER_TEST("Add 1 element to array") {
            int const ITEM  = 2;
            int raw_array[] = {1};
            auto array      = Dqn_FArray_Init<int, 4>(raw_array, DQN_ARRAY_ICOUNT(raw_array));
            Dqn_FArray_Add(&array, ITEM);
            DQN_TESTER_ASSERT(&test, array.size == 2);
            DQN_TESTER_ASSERT(&test, array.data[0] == 1);
            DQN_TESTER_ASSERT(&test, array.data[1] == ITEM);
        }

        DQN_TESTER_TEST("Clear array") {
            int raw_array[] = {1};
            auto array      = Dqn_FArray_Init<int, 4>(raw_array, DQN_ARRAY_ICOUNT(raw_array));
            Dqn_FArray_Clear(&array, Dqn_ZeroMem_No);
            DQN_TESTER_ASSERT(&test, array.size == 0);
        }
    }
    return test;
}

Dqn_Tester TestFString8()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_FString8") {
        DQN_TESTER_TEST("Append too much fails") {
            Dqn_FString8<4> str = {};
            DQN_TESTER_ASSERT(&test, !Dqn_FString8_AppendCString8(&str, "abcde"));
        }

        DQN_TESTER_TEST("Append format string too much fails") {
            Dqn_FString8<4> str = {};
            DQN_TESTER_ASSERT(&test, !Dqn_FString8_AppendF(&str, "abcde"));
        }
    }
    return test;
}

Dqn_Tester TestHex()
{
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_Tester test           = {};
    DQN_TESTER_GROUP(test, "Dqn_Hex") {
        DQN_TESTER_TEST("Convert 0x123") {
            uint64_t result = Dqn_Hex_String8ToU64(DQN_STRING8("0x123"));
            DQN_TESTER_ASSERTF(&test, result == 0x123, "result: %zu", result);
        }

        DQN_TESTER_TEST("Convert 0xFFFF") {
            uint64_t result = Dqn_Hex_String8ToU64(DQN_STRING8("0xFFFF"));
            DQN_TESTER_ASSERTF(&test, result == 0xFFFF, "result: %zu", result);
        }

        DQN_TESTER_TEST("Convert FFFF") {
            uint64_t result = Dqn_Hex_String8ToU64(DQN_STRING8("FFFF"));
            DQN_TESTER_ASSERTF(&test, result == 0xFFFF, "result: %zu", result);
        }

        DQN_TESTER_TEST("Convert abCD") {
            uint64_t result = Dqn_Hex_String8ToU64(DQN_STRING8("abCD"));
            DQN_TESTER_ASSERTF(&test, result == 0xabCD, "result: %zu", result);
        }

        DQN_TESTER_TEST("Convert 0xabCD") {
            uint64_t result = Dqn_Hex_String8ToU64(DQN_STRING8("0xabCD"));
            DQN_TESTER_ASSERTF(&test, result == 0xabCD, "result: %zu", result);
        }

        DQN_TESTER_TEST("Convert 0x") {
            uint64_t result = Dqn_Hex_String8ToU64(DQN_STRING8("0x"));
            DQN_TESTER_ASSERTF(&test, result == 0x0, "result: %zu", result);
        }

        DQN_TESTER_TEST("Convert 0X") {
            uint64_t result = Dqn_Hex_String8ToU64(DQN_STRING8("0X"));
            DQN_TESTER_ASSERTF(&test, result == 0x0, "result: %zu", result);
        }

        DQN_TESTER_TEST("Convert 3") {
            uint64_t result = Dqn_Hex_String8ToU64(DQN_STRING8("3"));
            DQN_TESTER_ASSERTF(&test, result == 3, "result: %zu", result);
        }

        DQN_TESTER_TEST("Convert f") {
            uint64_t result = Dqn_Hex_String8ToU64(DQN_STRING8("f"));
            DQN_TESTER_ASSERTF(&test, result == 0xf, "result: %zu", result);
        }

        DQN_TESTER_TEST("Convert g") {
            uint64_t result = Dqn_Hex_String8ToU64(DQN_STRING8("g"));
            DQN_TESTER_ASSERTF(&test, result == 0, "result: %zu", result);
        }

        DQN_TESTER_TEST("Convert -0x3") {
            uint64_t result = Dqn_Hex_String8ToU64(DQN_STRING8("-0x3"));
            DQN_TESTER_ASSERTF(&test, result == 0, "result: %zu", result);
        }

        uint32_t number = 0xd095f6;
        DQN_TESTER_TEST("Convert %x to string", number) {
            Dqn_String8 number_hex = Dqn_Hex_BytesToString8Arena(scratch.arena, &number, sizeof(number));
            DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(number_hex, DQN_STRING8("f695d000")), "number_hex=%.*s", DQN_STRING_FMT(number_hex));
        }

        number = 0xf6ed00;
        DQN_TESTER_TEST("Convert %x to string", number) {
            Dqn_String8 number_hex = Dqn_Hex_BytesToString8Arena(scratch.arena, &number, sizeof(number));
            DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(number_hex, DQN_STRING8("00edf600")), "number_hex=%.*s", DQN_STRING_FMT(number_hex));
        }

        Dqn_String8 hex = DQN_STRING8("0xf6ed00");
        DQN_TESTER_TEST("Convert %.*s to bytes", DQN_STRING_FMT(hex)) {
            Dqn_String8 bytes = Dqn_Hex_String8ToBytesArena(scratch.arena, hex);
            DQN_TESTER_ASSERTF(&test,
                               Dqn_String8_Eq(bytes, DQN_STRING8("\xf6\xed\x00")),
                               "number_hex=%.*s",
                               DQN_STRING_FMT(Dqn_Hex_BytesToString8Arena(scratch.arena, bytes.data, bytes.size)));
        }

    }
    return test;
}

Dqn_Tester TestM4()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_M4") {
        DQN_TESTER_TEST("Simple translate and scale matrix") {
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
        }
    }
    return test;
}

Dqn_Tester TestDSMap()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_DSMap") {
        Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
        {
            uint32_t const MAP_SIZE = 64;
            Dqn_DSMap<uint64_t>  map      = Dqn_DSMap_Init<uint64_t>(MAP_SIZE);
            DQN_DEFER { Dqn_DSMap_Deinit(&map); };

            DQN_TESTER_TEST("Find non-existent value") {
                uint64_t *value = Dqn_DSMap_Find(&map, Dqn_DSMap_KeyCStringLit(&map, "Foo"));
                DQN_TESTER_ASSERT(&test, !value);
                DQN_TESTER_ASSERT(&test, map.size         == MAP_SIZE);
                DQN_TESTER_ASSERT(&test, map.initial_size == MAP_SIZE);
                DQN_TESTER_ASSERT(&test, map.occupied     == 1 /*Sentinel*/);
            }

            Dqn_DSMapKey key = Dqn_DSMap_KeyCStringLit(&map, "Bar");
            DQN_TESTER_TEST("Insert value and lookup") {
                uint64_t desired_value  = 0xF00BAA;
                uint64_t *slot_value = Dqn_DSMap_Set(&map, key, desired_value, nullptr /*found*/);
                DQN_TESTER_ASSERT(&test, slot_value);
                DQN_TESTER_ASSERT(&test, map.size == MAP_SIZE);
                DQN_TESTER_ASSERT(&test, map.initial_size == MAP_SIZE);
                DQN_TESTER_ASSERT(&test, map.occupied == 2);

                uint64_t *value = Dqn_DSMap_Find(&map, key);
                DQN_TESTER_ASSERT(&test, value);
                DQN_TESTER_ASSERT(&test, *value == desired_value);
            }

            DQN_TESTER_TEST("Remove key") {
                Dqn_DSMap_Erase(&map, key);
                DQN_TESTER_ASSERT(&test, map.size         == MAP_SIZE);
                DQN_TESTER_ASSERT(&test, map.initial_size == MAP_SIZE);
                DQN_TESTER_ASSERT(&test, map.occupied     == 1 /*Sentinel*/);
            }
        }

        enum DSMapTestType { DSMapTestType_Set, DSMapTestType_MakeSlot, DSMapTestType_Count };
        for (int test_type = 0; test_type < DSMapTestType_Count; test_type++) {
            Dqn_String8 prefix = {};
            switch (test_type) {
                case DSMapTestType_Set:      prefix = DQN_STRING8("Set"); break;
                case DSMapTestType_MakeSlot: prefix = DQN_STRING8("Make slot"); break;
            }

            DQN_ARENA_TEMP_MEMORY_SCOPE(scratch.arena);
            uint32_t const MAP_SIZE = 64;
            Dqn_DSMap<uint64_t>  map      = Dqn_DSMap_Init<uint64_t>(MAP_SIZE);
            DQN_DEFER { Dqn_DSMap_Deinit(&map); };

            DQN_TESTER_TEST("%.*s: Test growing", DQN_STRING_FMT(prefix)) {
                uint64_t map_start_size  = map.size;
                uint64_t value          = 0;
                uint64_t grow_threshold = map_start_size * 3 / 4;
                for (; map.occupied != grow_threshold; value++) {
                    uint64_t *val_copy = Dqn_Arena_Copy(scratch.arena, uint64_t, &value, 1);
                    Dqn_DSMapKey key   = Dqn_DSMap_KeyBuffer(&map, (char *)val_copy, sizeof(*val_copy));
                    DQN_TESTER_ASSERT(&test, !Dqn_DSMap_Find<uint64_t>(&map, key));
                    DQN_TESTER_ASSERT(&test, !Dqn_DSMap_FindSlot<uint64_t>(&map, key));
                    bool found = false;
                    if (test_type == DSMapTestType_Set) {
                        Dqn_DSMap_Set(&map, key, value, &found);
                    } else {
                        Dqn_DSMap_MakeSlot(&map, key, &found);
                    }
                    DQN_TESTER_ASSERT(&test, !found);
                    DQN_TESTER_ASSERT(&test, Dqn_DSMap_Find<uint64_t>(&map, key));
                    DQN_TESTER_ASSERT(&test, Dqn_DSMap_FindSlot<uint64_t>(&map, key));
                }
                DQN_TESTER_ASSERT(&test, map.initial_size == MAP_SIZE);
                DQN_TESTER_ASSERT(&test, map.size         == map_start_size);
                DQN_TESTER_ASSERT(&test, map.occupied     == 1 /*Sentinel*/ + value);

                { // NOTE: One more item should cause the table to grow by 2x
                    uint64_t *val_copy = Dqn_Arena_Copy(scratch.arena, uint64_t, &value, 1);
                    Dqn_DSMapKey key   = Dqn_DSMap_KeyBuffer(&map, (char *)val_copy, sizeof(*val_copy));
                    bool found         = false;
                    if (test_type == DSMapTestType_Set) {
                        Dqn_DSMap_Set(&map, key, value, &found);
                    } else {
                        Dqn_DSMap_MakeSlot(&map, key, &found);
                    }

                    value++;
                    DQN_TESTER_ASSERT(&test, !found);
                    DQN_TESTER_ASSERT(&test, map.size         == map_start_size * 2);
                    DQN_TESTER_ASSERT(&test, map.initial_size == MAP_SIZE);
                    DQN_TESTER_ASSERT(&test, map.occupied     == 1 /*Sentinel*/ + value);
                }
            }

            DQN_TESTER_TEST("%.*s: Check the sentinel is present", DQN_STRING_FMT(prefix)) {
                Dqn_DSMapSlot<uint64_t> NIL_SLOT = {};
                Dqn_DSMapSlot<uint64_t> sentinel = map.slots[DQN_DS_MAP_SENTINEL_SLOT];
                DQN_TESTER_ASSERT(&test, DQN_MEMCMP(&sentinel, &NIL_SLOT, sizeof(NIL_SLOT)) == 0);
            }

            DQN_TESTER_TEST("%.*s: Recheck all the hash tables values after growing", DQN_STRING_FMT(prefix)) {
                for (uint64_t index = 1 /*Sentinel*/; index < map.occupied; index++) {
                    Dqn_DSMapSlot<uint64_t> const *slot = map.slots + index;

                    // NOTE: Validate each slot value
                    uint64_t value_test = index - 1;
                    Dqn_DSMapKey key    = Dqn_DSMap_KeyBuffer(&map, &value_test, sizeof(value_test));
                    DQN_TESTER_ASSERT(&test, Dqn_DSMap_KeyEquals(slot->key, key));
                    if (test_type == DSMapTestType_Set) {
                        DQN_TESTER_ASSERT(&test, slot->value == value_test);
                    } else {
                        DQN_TESTER_ASSERT(&test, slot->value == 0); // NOTE: Make slot does not set the key so should be 0
                    }
                    DQN_TESTER_ASSERT(&test, slot->key.hash == Dqn_DSMap_Hash(&map, slot->key));

                    // NOTE: Check the reverse lookup is correct
                    Dqn_DSMapSlot<uint64_t> const *check = Dqn_DSMap_FindSlot(&map, slot->key);
                    DQN_TESTER_ASSERT(&test, slot == check);
                }
            }

            DQN_TESTER_TEST("%.*s: Test shrinking", DQN_STRING_FMT(prefix)) {
                uint64_t start_map_size     = map.size;
                uint64_t start_map_occupied = map.occupied;
                uint64_t value              = 0;
                uint64_t shrink_threshold   = map.size * 1 / 4;
                for (; map.occupied != shrink_threshold; value++) {
                    uint64_t *val_copy = Dqn_Arena_Copy(scratch.arena, uint64_t, &value, 1);
                    Dqn_DSMapKey key   = Dqn_DSMap_KeyBuffer(&map, (char *)val_copy, sizeof(*val_copy));

                    DQN_TESTER_ASSERT(&test, Dqn_DSMap_Find<uint64_t>(&map, key));
                    DQN_TESTER_ASSERT(&test, Dqn_DSMap_FindSlot<uint64_t>(&map, key));
                    Dqn_DSMap_Erase(&map, key);
                    DQN_TESTER_ASSERT(&test, !Dqn_DSMap_Find<uint64_t>(&map, key));
                    DQN_TESTER_ASSERT(&test, !Dqn_DSMap_FindSlot(&map, key));
                }
                DQN_TESTER_ASSERT(&test, map.size == start_map_size);
                DQN_TESTER_ASSERT(&test, map.occupied == start_map_occupied - value);

                { // NOTE: One more item should cause the table to grow by 2x
                    uint64_t *val_copy = Dqn_Arena_Copy(scratch.arena, uint64_t, &value, 1);
                    Dqn_DSMapKey key   = Dqn_DSMap_KeyBuffer(&map, (char *)val_copy, sizeof(*val_copy));
                    Dqn_DSMap_Erase(&map, key);
                    value++;

                    DQN_TESTER_ASSERT(&test, map.size     == start_map_size / 2);
                    DQN_TESTER_ASSERT(&test, map.occupied == start_map_occupied - value);
                }

                { // NOTE: Check the sentinel is present
                    Dqn_DSMapSlot<uint64_t> NIL_SLOT = {};
                    Dqn_DSMapSlot<uint64_t> sentinel = map.slots[DQN_DS_MAP_SENTINEL_SLOT];
                    DQN_TESTER_ASSERT(&test, DQN_MEMCMP(&sentinel, &NIL_SLOT, sizeof(NIL_SLOT)) == 0);
                }

                // NOTE: Recheck all the hash table values after growing
                for (uint64_t index = 1 /*Sentinel*/; index < map.occupied; index++) {

                    // NOTE: Generate the key
                    uint64_t value_test = value + (index - 1);
                    Dqn_DSMapKey key    = Dqn_DSMap_KeyBuffer(&map, (char *)&value_test, sizeof(value_test));

                    // NOTE: Validate each slot value
                    Dqn_DSMapSlot<uint64_t> const *slot = Dqn_DSMap_FindSlot(&map, key);
                    DQN_TESTER_ASSERT(&test, slot);
                    DQN_TESTER_ASSERT(&test, slot->key == key);
                    if (test_type == DSMapTestType_Set) {
                        DQN_TESTER_ASSERT(&test, slot->value == value_test);
                    } else {
                        DQN_TESTER_ASSERT(&test, slot->value == 0); // NOTE: Make slot does not set the key so should be 0
                    }
                    DQN_TESTER_ASSERT(&test, slot->key.hash == Dqn_DSMap_Hash(&map, slot->key));

                    // NOTE: Check the reverse lookup is correct
                    Dqn_DSMapSlot<uint64_t> const *check = Dqn_DSMap_FindSlot(&map, slot->key);
                    DQN_TESTER_ASSERT(&test, slot == check);
                }

                for (; map.occupied != 1; value++) { // NOTE: Remove all items from the table
                    uint64_t *val_copy = Dqn_Arena_Copy(scratch.arena, uint64_t, &value, 1);
                    Dqn_DSMapKey key   = Dqn_DSMap_KeyBuffer(&map, (char *)val_copy, sizeof(*val_copy));
                    DQN_TESTER_ASSERT(&test, Dqn_DSMap_Find<uint64_t>(&map, key));
                    Dqn_DSMap_Erase(&map, key);
                    DQN_TESTER_ASSERT(&test, !Dqn_DSMap_Find<uint64_t>(&map, key));
                }
                DQN_TESTER_ASSERT(&test, map.initial_size == MAP_SIZE);
                DQN_TESTER_ASSERT(&test, map.size == map.initial_size);
                DQN_TESTER_ASSERT(&test, map.occupied == 1 /*Sentinel*/);
            }
        }
    }
    return test;
}

Dqn_Tester TestIntrinsics()
{
    Dqn_Tester test = {};
    // TODO(dqn): We don't have meaningful tests here, but since
    // atomics/intrinsics are implemented using macros we ensure the macro was
    // written properly with these tests.

    DQN_TESTER_GROUP(test, "Dqn_Atomic") {
        DQN_TESTER_TEST("Dqn_Atomic_AddU32") {
            uint32_t val = 0;
            Dqn_Atomic_AddU32(&val, 1);
            DQN_TESTER_ASSERTF(&test, val == 1, "val: %u", val);
        }

        DQN_TESTER_TEST("Dqn_Atomic_AddU64") {
            uint64_t val = 0;
            Dqn_Atomic_AddU64(&val, 1);
            DQN_TESTER_ASSERTF(&test, val == 1, "val: %zu", val);
        }

        DQN_TESTER_TEST("Dqn_Atomic_SubU32") {
            uint32_t val = 1;
            Dqn_Atomic_SubU32(&val, 1);
            DQN_TESTER_ASSERTF(&test, val == 0, "val: %u", val);
        }

        DQN_TESTER_TEST("Dqn_Atomic_SubU64") {
            uint64_t val = 1;
            Dqn_Atomic_SubU64(&val, 1);
            DQN_TESTER_ASSERTF(&test, val == 0, "val: %zu", val);
        }

        DQN_TESTER_TEST("Dqn_Atomic_SetValue32") {
            long a = 0;
            long b = 111;
            Dqn_Atomic_SetValue32(&a, b);
            DQN_TESTER_ASSERTF(&test, a == b, "a: %lu, b: %lu", a, b);
        }

        DQN_TESTER_TEST("Dqn_Atomic_SetValue64") {
            int64_t a = 0;
            int64_t b = 111;
            Dqn_Atomic_SetValue64(DQN_CAST(uint64_t *)&a, b);
            DQN_TESTER_ASSERTF(&test, a == b, "a: %I64i, b: %I64i", a, b);
        }

        Dqn_Tester_Begin(&test, "Dqn_CPUClockCycle");
        Dqn_CPUClockCycle();
        Dqn_Tester_End(&test);

        Dqn_Tester_Begin(&test, "Dqn_CompilerReadBarrierAndCPUReadFence");
        Dqn_CompilerReadBarrierAndCPUReadFence;
        Dqn_Tester_End(&test);

        Dqn_Tester_Begin(&test, "Dqn_CompilerWriteBarrierAndCPUWriteFence");
        Dqn_CompilerWriteBarrierAndCPUWriteFence;
        Dqn_Tester_End(&test);
    }

    return test;
}

Dqn_Tester TestRect()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_Rect") {
        DQN_TESTER_TEST("No intersection") {
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
        }

        DQN_TESTER_TEST("A's min intersects B") {
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
        }

        DQN_TESTER_TEST("B's min intersects A") {
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
        }

        DQN_TESTER_TEST("A's max intersects B") {
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
        }

        DQN_TESTER_TEST("B's max intersects A") {
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
        }


        DQN_TESTER_TEST("B contains A") {
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
        }

        DQN_TESTER_TEST("A contains B") {
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
        }

        DQN_TESTER_TEST("A equals B") {
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
        }
    }
    return test;
}

Dqn_Tester TestPerfCounter()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_OS_PerfCounter") {
        DQN_TESTER_TEST("Dqn_OS_PerfCounterNow") {
            uint64_t result = Dqn_OS_PerfCounterNow();
            DQN_TESTER_ASSERT(&test, result != 0);
        }

        DQN_TESTER_TEST("Consecutive ticks are ordered") {
            uint64_t a = Dqn_OS_PerfCounterNow();
            uint64_t b = Dqn_OS_PerfCounterNow();
            DQN_TESTER_ASSERTF(&test, b >= a, "a: %zu, b: %zu", a, b);
        }

        DQN_TESTER_TEST("Ticks to time are a correct order of magnitude") {
            uint64_t a      = Dqn_OS_PerfCounterNow();
            uint64_t b      = Dqn_OS_PerfCounterNow();
            Dqn_f64 s       = Dqn_OS_PerfCounterS(a, b);
            Dqn_f64 ms      = Dqn_OS_PerfCounterMs(a, b);
            Dqn_f64 micro_s = Dqn_OS_PerfCounterMicroS(a, b);
            Dqn_f64 ns      = Dqn_OS_PerfCounterNs(a, b);
            DQN_TESTER_ASSERTF(&test, s <= ms, "s: %f, ms: %f", s, ms);
            DQN_TESTER_ASSERTF(&test, ms <= micro_s, "ms: %f, micro_s: %f", ms, micro_s);
            DQN_TESTER_ASSERTF(&test, micro_s <= ns, "micro_s: %f, ns: %f", micro_s, ns);
        }
    }
    return test;
}

Dqn_Tester TestOS()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_OS_") {
        DQN_TESTER_TEST("Generate secure RNG bytes with nullptr") {
            Dqn_b32 result = Dqn_OS_SecureRNGBytes(nullptr, 1);
            DQN_TESTER_ASSERT(&test, result == false);
        }

        DQN_TESTER_TEST("Generate secure RNG 32 bytes") {
            char const ZERO[32] = {};
            char       buf[32]  = {};
            bool result         = Dqn_OS_SecureRNGBytes(buf, DQN_ARRAY_UCOUNT(buf));
            DQN_TESTER_ASSERT(&test, result);
            DQN_TESTER_ASSERT(&test, DQN_MEMCMP(buf, ZERO, DQN_ARRAY_UCOUNT(buf)) != 0);
        }

        DQN_TESTER_TEST("Generate secure RNG 0 bytes") {
            char buf[32] = {};
            buf[0] = 'Z';
            Dqn_b32 result = Dqn_OS_SecureRNGBytes(buf, 0);
            DQN_TESTER_ASSERT(&test, result);
            DQN_TESTER_ASSERT(&test, buf[0] == 'Z');
        }

        DQN_TESTER_TEST("Query executable directory") {
            Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
            Dqn_String8 result = Dqn_OS_EXEDir(scratch.allocator);
            DQN_TESTER_ASSERT(&test, Dqn_String8_IsValid(result));
            DQN_TESTER_ASSERTF(&test, Dqn_Fs_DirExists(result), "result(%zu): %.*s", result.size, DQN_STRING_FMT(result));
        }
    }
    return test;
}

Dqn_Tester TestCString8()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_CString8") {
        // NOTE: Dqn_CString8_ToI64
        // ---------------------------------------------------------------------------------------------
        DQN_TESTER_TEST("To I64: Convert nullptr") {
            int64_t result = Dqn_CString8_ToI64(nullptr);
            DQN_TESTER_ASSERT(&test, result == 0);
        }

        DQN_TESTER_TEST("To I64: Convert empty string") {
            int64_t result = Dqn_CString8_ToI64("");
            DQN_TESTER_ASSERT(&test, result == 0);
        }

        DQN_TESTER_TEST("To I64: Convert \"1\"") {
            int64_t result = Dqn_CString8_ToI64("1");
            DQN_TESTER_ASSERT(&test, result == 1);
        }

        DQN_TESTER_TEST("To I64: Convert \"-0\"") {
            int64_t result = Dqn_CString8_ToI64("-0");
            DQN_TESTER_ASSERT(&test, result == 0);
        }

        DQN_TESTER_TEST("To I64: Convert \"-1\"") {
            int64_t result = Dqn_CString8_ToI64("-1");
            DQN_TESTER_ASSERT(&test, result == -1);
        }

        DQN_TESTER_TEST("To I64: Convert \"1.2\"") {
            int64_t result = Dqn_CString8_ToI64("1.2");
            DQN_TESTER_ASSERT(&test, result == 1);
        }

        DQN_TESTER_TEST("To I64: Convert \"1,234\"") {
            int64_t result = Dqn_CString8_ToI64("1,234");
            DQN_TESTER_ASSERT(&test, result == 1234);
        }

        DQN_TESTER_TEST("To I64: Convert \"1,2\"") {
            int64_t result = Dqn_CString8_ToI64("1,2");
            DQN_TESTER_ASSERT(&test, result == 12);
        }

        DQN_TESTER_TEST("To I64: Convert \"12a3\"") {
            int64_t result = Dqn_CString8_ToI64("12a3");
            DQN_TESTER_ASSERT(&test, result == 12);
        }

        // NOTE: Dqn_CString8_ToU64
        // ---------------------------------------------------------------------------------------------
        DQN_TESTER_TEST("To U64: Convert nullptr") {
            uint64_t result = Dqn_CString8_ToU64(nullptr);
            DQN_TESTER_ASSERTF(&test, result == 0, "result: %zu", result);
        }

        DQN_TESTER_TEST("To U64: Convert empty string") {
            uint64_t result = Dqn_CString8_ToU64("");
            DQN_TESTER_ASSERTF(&test, result == 0, "result: %zu", result);
        }

        DQN_TESTER_TEST("To U64: Convert \"1\"") {
            uint64_t result = Dqn_CString8_ToU64("1");
            DQN_TESTER_ASSERTF(&test, result == 1, "result: %zu", result);
        }

        DQN_TESTER_TEST("To U64: Convert \"-0\"") {
            uint64_t result = Dqn_CString8_ToU64("-0");
            DQN_TESTER_ASSERTF(&test, result == 0, "result: %zu", result);
        }

        DQN_TESTER_TEST("To U64: Convert \"-1\"") {
            uint64_t result = Dqn_CString8_ToU64("-1");
            DQN_TESTER_ASSERTF(&test, result == 0, "result: %zu", result);
        }

        DQN_TESTER_TEST("To U64: Convert \"1.2\"") {
            uint64_t result = Dqn_CString8_ToU64("1.2");
            DQN_TESTER_ASSERTF(&test, result == 1, "result: %zu", result);
        }

        DQN_TESTER_TEST("To U64: Convert \"1,234\"") {
            uint64_t result = Dqn_CString8_ToU64("1,234");
            DQN_TESTER_ASSERTF(&test, result == 1234, "result: %zu", result);
        }

        DQN_TESTER_TEST("To U64: Convert \"1,2\"") {
            uint64_t result = Dqn_CString8_ToU64("1,2");
            DQN_TESTER_ASSERTF(&test, result == 12, "result: %zu", result);
        }

        DQN_TESTER_TEST("To U64: Convert \"12a3\"") {
            uint64_t result = Dqn_CString8_ToU64("12a3");
            DQN_TESTER_ASSERTF(&test, result == 12, "result: %zu", result);
        }

        // NOTE: Dqn_CString8_Find
        // ---------------------------------------------------------------------------------------------
        DQN_TESTER_TEST("Find: String (char) is not in buffer") {
            char const buf[]  = "836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55";
            char const find[] = "2";
            char const *result = Dqn_CString8_Find(buf, find, Dqn_CString8_ArrayCountI(buf), Dqn_CString8_ArrayCountI(find));
            DQN_TESTER_ASSERT(&test, result == nullptr);
        }

        DQN_TESTER_TEST("Find: String (char) is in buffer") {
            char const buf[]  = "836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55";
            char const find[] = "6";
            char const *result = Dqn_CString8_Find(buf, find, Dqn_CString8_ArrayCountI(buf), Dqn_CString8_ArrayCountI(find));
            DQN_TESTER_ASSERT(&test, result != nullptr);
            DQN_TESTER_ASSERT(&test, result[0] == '6' && result[1] == 'a');
        }

        // NOTE: Dqn_CString8_FileNameFromPath
        // ---------------------------------------------------------------------------------------------
        DQN_TESTER_TEST("File name from Windows path") {
            Dqn_isize file_name_size = 0;
            char const buf[]  = "C:\\ABC\\test.exe";
            char const *result = Dqn_CString8_FileNameFromPath(buf, Dqn_CString8_ArrayCountI(buf), &file_name_size);
            DQN_TESTER_ASSERTF(&test, file_name_size == 8, "size: %I64d", file_name_size);
            DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(Dqn_String8_Init(result, file_name_size), DQN_STRING8("test.exe")), "%.*s", DQN_CAST(int)file_name_size, result);
        }

        DQN_TESTER_TEST("File name from Linux path") {
            Dqn_isize file_name_size = 0;
            char const buf[]  = "/ABC/test.exe";
            char const *result = Dqn_CString8_FileNameFromPath(buf, Dqn_CString8_ArrayCountI(buf), &file_name_size);
            DQN_TESTER_ASSERTF(&test, file_name_size == 8, "size: %I64d", file_name_size);
            DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(Dqn_String8_Init(result, file_name_size), DQN_STRING8("test.exe")), "%.*s", (int)file_name_size, result);
        }

        // NOTE: Dqn_CString8_TrimPrefix
        // ---------------------------------------------------------------------------------------------
        DQN_TESTER_TEST("Trim prefix") {
            char const  prefix[]     = "@123";
            char const  buf[]        = "@123string";
            Dqn_isize   trimmed_size = 0;
            char const *result       = Dqn_CString8_TrimPrefix(buf, prefix, Dqn_CString8_ArrayCountI(buf), Dqn_CString8_ArrayCountI(prefix), Dqn_CString8EqCase_Sensitive, &trimmed_size);
            DQN_TESTER_ASSERTF(&test, trimmed_size == 6, "size: %I64d", trimmed_size);
            DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(Dqn_String8_Init(result, trimmed_size), DQN_STRING8("string")), "%.*s", (int)trimmed_size, result);
        }

        DQN_TESTER_TEST("Trim prefix, nullptr trimmed size") {
            char const  prefix[]     = "@123";
            char const  buf[]        = "@123string";
            char const *result       = Dqn_CString8_TrimPrefix(buf, prefix, Dqn_CString8_ArrayCountI(buf), Dqn_CString8_ArrayCountI(prefix), Dqn_CString8EqCase_Sensitive, nullptr);
            DQN_TESTER_ASSERT(&test, result);
        }

        // NOTE: Dqn_CString8_IsAllDigits
        // ---------------------------------------------------------------------------------------------
        DQN_TESTER_TEST("Is all digits fails on non-digit string") {
            char const buf[]  = "@123string";
            Dqn_b32    result = Dqn_CString8_IsAllDigits(buf, Dqn_CString8_ArrayCountI(buf));
            DQN_TESTER_ASSERT(&test, result == false);
        }

        DQN_TESTER_TEST("Is all digits fails on nullptr") {
            Dqn_b32 result = Dqn_CString8_IsAllDigits(nullptr, 0);
            DQN_TESTER_ASSERT(&test, result == false);
        }

        DQN_TESTER_TEST("Is all digits fails on nullptr w/ size") {
            Dqn_b32 result = Dqn_CString8_IsAllDigits(nullptr, 1);
            DQN_TESTER_ASSERT(&test, result == false);
        }

        DQN_TESTER_TEST("Is all digits fails on 0 size w/ string") {
            char const buf[]  = "@123string";
            Dqn_b32    result = Dqn_CString8_IsAllDigits(buf, 0);
            DQN_TESTER_ASSERT(&test, result == false);
        }

        DQN_TESTER_TEST("Is all digits success") {
            char const buf[]  = "23";
            Dqn_b32    result = Dqn_CString8_IsAllDigits(buf, Dqn_CString8_ArrayCountI(buf));
            DQN_TESTER_ASSERT(&test, DQN_CAST(bool)result == true);
        }

        DQN_TESTER_TEST("Is all digits fails on whitespace") {
            char const buf[]  = "23 ";
            Dqn_b32    result = Dqn_CString8_IsAllDigits(buf, Dqn_CString8_ArrayCountI(buf));
            DQN_TESTER_ASSERT(&test, DQN_CAST(bool)result == false);
        }
    }
    return test;
}

Dqn_Tester TestString8()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_String8") {
        DQN_TESTER_TEST("Initialise with string literal w/ macro") {
            Dqn_String8 string = DQN_STRING8("AB");
            DQN_TESTER_ASSERTF(&test, string.size == 2, "size: %I64d", string.size);
            DQN_TESTER_ASSERTF(&test, string.data[0] == 'A', "string[0]: %c", string.data[0]);
            DQN_TESTER_ASSERTF(&test, string.data[1] == 'B', "string[1]: %c", string.data[1]);
        }

        DQN_TESTER_TEST("Initialise with format string") {
            Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
            Dqn_String8 string = Dqn_String8_InitF(scratch.allocator, "%s", "AB");
            DQN_TESTER_ASSERTF(&test, string.size == 2, "size: %I64d", string.size);
            DQN_TESTER_ASSERTF(&test, string.data[0] == 'A', "string[0]: %c", string.data[0]);
            DQN_TESTER_ASSERTF(&test, string.data[1] == 'B', "string[1]: %c", string.data[1]);
            DQN_TESTER_ASSERTF(&test, string.data[2] == 0, "string[2]: %c", string.data[2]);
        }

        DQN_TESTER_TEST("Copy string") {
            Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
            Dqn_String8 string        = DQN_STRING8("AB");
            Dqn_String8 copy          = Dqn_String8_Copy(scratch.allocator, string);
            DQN_TESTER_ASSERTF(&test, copy.size == 2, "size: %I64d", copy.size);
            DQN_TESTER_ASSERTF(&test, copy.data[0] == 'A', "copy[0]: %c", copy.data[0]);
            DQN_TESTER_ASSERTF(&test, copy.data[1] == 'B', "copy[1]: %c", copy.data[1]);
            DQN_TESTER_ASSERTF(&test, copy.data[2] ==  0,  "copy[2]: %c", copy.data[2]);
        }

        DQN_TESTER_TEST("Trim whitespace around string") {
            Dqn_String8 string = Dqn_String8_TrimWhitespaceAround(DQN_STRING8(" AB "));
            DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(string, DQN_STRING8("AB")), "[string=%.*s]", DQN_STRING_FMT(string));
        }

        DQN_TESTER_TEST("Allocate string from arena") {
            Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
            Dqn_String8 string = Dqn_String8_Allocate(scratch.allocator, 2, Dqn_ZeroMem_No);
            DQN_TESTER_ASSERTF(&test, string.size == 2, "size: %I64d", string.size);
        }

        // NOTE: Dqn_CString8_Trim[Prefix/Suffix]
        // ---------------------------------------------------------------------------------------------
        DQN_TESTER_TEST("Trim prefix with matching prefix") {
            Dqn_String8 input  = DQN_STRING8("nft/abc");
            Dqn_String8 result = Dqn_String8_TrimPrefix(input, DQN_STRING8("nft/"));
            DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(result, DQN_STRING8("abc")), "%.*s", DQN_STRING_FMT(result));
        }

        DQN_TESTER_TEST("Trim prefix with non matching prefix") {
            Dqn_String8 input  = DQN_STRING8("nft/abc");
            Dqn_String8 result = Dqn_String8_TrimPrefix(input, DQN_STRING8(" ft/"));
            DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(result, input), "%.*s", DQN_STRING_FMT(result));
        }

        DQN_TESTER_TEST("Trim suffix with matching suffix") {
            Dqn_String8 input  = DQN_STRING8("nft/abc");
            Dqn_String8 result = Dqn_String8_TrimSuffix(input, DQN_STRING8("abc"));
            DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(result, DQN_STRING8("nft/")), "%.*s", DQN_STRING_FMT(result));
        }

        DQN_TESTER_TEST("Trim suffix with non matching suffix") {
            Dqn_String8 input  = DQN_STRING8("nft/abc");
            Dqn_String8 result = Dqn_String8_TrimSuffix(input, DQN_STRING8("ab"));
            DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(result, input), "%.*s", DQN_STRING_FMT(result));
        }

        // NOTE: Dqn_String8_IsAllDigits
        // ---------------------------------------------------------------------------------------------
        DQN_TESTER_TEST("Is all digits fails on non-digit string") {
            Dqn_b32 result = Dqn_String8_IsAllDigits(DQN_STRING8("@123string"));
            DQN_TESTER_ASSERT(&test, result == false);
        }

        DQN_TESTER_TEST("Is all digits fails on nullptr") {
            Dqn_b32 result = Dqn_String8_IsAllDigits(Dqn_String8_Init(nullptr, 0));
            DQN_TESTER_ASSERT(&test, result == false);
        }

        DQN_TESTER_TEST("Is all digits fails on nullptr w/ size") {
            Dqn_b32 result = Dqn_String8_IsAllDigits(Dqn_String8_Init(nullptr, 1));
            DQN_TESTER_ASSERT(&test, result == false);
        }

        DQN_TESTER_TEST("Is all digits fails on string w/ 0 size") {
            char const buf[]  = "@123string";
            Dqn_b32    result = Dqn_String8_IsAllDigits(Dqn_String8_Init(buf, 0));
            DQN_TESTER_ASSERT(&test, result == false);
        }

        DQN_TESTER_TEST("Is all digits success") {
            Dqn_b32 result = Dqn_String8_IsAllDigits(DQN_STRING8("23"));
            DQN_TESTER_ASSERT(&test, DQN_CAST(bool)result == true);
        }

        DQN_TESTER_TEST("Is all digits fails on whitespace") {
            Dqn_b32 result = Dqn_String8_IsAllDigits(DQN_STRING8("23 "));
            DQN_TESTER_ASSERT(&test, DQN_CAST(bool)result == false);
        }

        // NOTE: Dqn_String8_BinarySplit
        // ---------------------------------------------------------------------------------------------
        {
            char const *TEST_FMT = "Dqn_String8_BinarySplit \"%s\" with 'c'";

            char delimiter    = '/';
            Dqn_String8 input = DQN_STRING8("abcdef");
            DQN_TESTER_TEST(TEST_FMT, input.data, delimiter) {
                Dqn_String8 rhs = {};
                Dqn_String8 lhs = Dqn_String8_BinarySplit(input, delimiter, &rhs);

                DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(lhs, DQN_STRING8("abcdef")), "[lhs=%.*s]", DQN_STRING_FMT(lhs));
                DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(rhs, DQN_STRING8("")), "[rhs=%.*s]", DQN_STRING_FMT(rhs));
            }

            input = DQN_STRING8("abc/def");
            DQN_TESTER_TEST(TEST_FMT, input.data, delimiter) {
                Dqn_String8 rhs = {};
                Dqn_String8 lhs = Dqn_String8_BinarySplit(input, delimiter, &rhs);
                DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(lhs, DQN_STRING8("abc")), "[lhs=%.*s]", DQN_STRING_FMT(lhs));
                DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(rhs, DQN_STRING8("def")), "[rhs=%.*s]", DQN_STRING_FMT(rhs));
            }

            input = DQN_STRING8("/abcdef");
            DQN_TESTER_TEST(TEST_FMT, input.data, delimiter) {
                Dqn_String8 rhs = {};
                Dqn_String8 lhs = Dqn_String8_BinarySplit(input, delimiter, &rhs);
                DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(lhs, DQN_STRING8("")), "[lhs=%.*s]", DQN_STRING_FMT(lhs));
                DQN_TESTER_ASSERTF(&test, Dqn_String8_Eq(rhs, DQN_STRING8("abcdef")), "[rhs=%.*s]", DQN_STRING_FMT(rhs));
            }
        }
    }
    return test;
}

Dqn_Tester TestTicketMutex()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_TicketMutex") {
        DQN_TESTER_TEST("Ticket mutex start and stop") {
            // TODO: We don't have a meaningful test but since atomics are
            // implemented with a macro this ensures that we test that they are
            // written correctly.
            Dqn_TicketMutex mutex = {};
            Dqn_TicketMutex_Begin(&mutex);
            Dqn_TicketMutex_End(&mutex);
            DQN_TESTER_ASSERT(&test, mutex.ticket == mutex.serving);
        }

        DQN_TESTER_TEST("Ticket mutex start and stop w/ advanced API") {
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
        }
    }
    return test;
}

Dqn_Tester TestVArray()
{
    Dqn_Tester test            = {};
    Dqn_ThreadScratch scratch  = Dqn_Thread_GetScratch(nullptr);
    Dqn_VArray<uint32_t> array = Dqn_VArray_InitByteSize<uint32_t>(scratch.arena, DQN_KILOBYTES(64));

    DQN_TESTER_GROUP(test, "Dqn_VArray") {
        DQN_TESTER_TEST("Test adding an array of items to the array") {
            uint32_t array_literal[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
            Dqn_VArray_Add<uint32_t>(&array, array_literal, DQN_ARRAY_UCOUNT(array_literal));
            DQN_TESTER_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
            DQN_TESTER_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
        }

        DQN_TESTER_TEST("Test stable erase, 1 item, the '2' value from the array") {
            Dqn_VArray_EraseRange(&array, 2 /*begin_index*/, 1 /*count*/, Dqn_VArrayErase_Stable);
            uint32_t array_literal[] = {0, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
            DQN_TESTER_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
            DQN_TESTER_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
        }

        DQN_TESTER_TEST("Test unstable erase, 1 item, the '1' value from the array") {
            Dqn_VArray_EraseRange(&array, 1 /*begin_index*/, 1 /*count*/, Dqn_VArrayErase_Unstable);
            uint32_t array_literal[] = {0, 15, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
            DQN_TESTER_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
            DQN_TESTER_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
        }

        Dqn_VArrayErase erase_enums[] = {Dqn_VArrayErase_Stable, Dqn_VArrayErase_Unstable};
        DQN_TESTER_TEST("Test un/stable erase, OOB") {
            for (Dqn_VArrayErase erase : erase_enums) {
                uint32_t array_literal[] = {0, 15, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
                Dqn_VArray_EraseRange(&array, DQN_ARRAY_UCOUNT(array_literal) /*begin_index*/, DQN_ARRAY_UCOUNT(array_literal) + 100 /*count*/, erase);
                DQN_TESTER_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
                DQN_TESTER_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
            }
        }

        DQN_TESTER_TEST("Test flipped begin/end index stable erase, 2 items, the '15, 3' value from the array") {
            Dqn_VArray_EraseRange(&array, 2 /*begin_index*/, -2 /*count*/, Dqn_VArrayErase_Stable);
            uint32_t array_literal[] = {0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
            DQN_TESTER_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
            DQN_TESTER_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
        }

        DQN_TESTER_TEST("Test flipped begin/end index unstable erase, 2 items, the '4, 5' value from the array") {
            Dqn_VArray_EraseRange(&array, 2 /*begin_index*/, -2 /*count*/, Dqn_VArrayErase_Unstable);
            uint32_t array_literal[] = {0, 13, 14, 6, 7, 8, 9, 10, 11, 12};
            DQN_TESTER_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
            DQN_TESTER_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
        }

        DQN_TESTER_TEST("Test stable erase range, 2+1 (oob) item, the '13, 14, +1 OOB' value from the array") {
            Dqn_VArray_EraseRange(&array, 8 /*begin_index*/, 3 /*count*/, Dqn_VArrayErase_Stable);
            uint32_t array_literal[] = {0, 13, 14, 6, 7, 8, 9, 10};
            DQN_TESTER_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
            DQN_TESTER_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
        }

        DQN_TESTER_TEST("Test unstable erase range, 3+1 (oob) item, the '11, 12, +1 OOB' value from the array") {
            Dqn_VArray_EraseRange(&array, 6 /*begin_index*/, 3 /*count*/, Dqn_VArrayErase_Unstable);
            uint32_t array_literal[] = {0, 13, 14, 6, 7, 8};
            DQN_TESTER_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
            DQN_TESTER_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
        }

        DQN_TESTER_TEST("Test stable erase, negative overflow OOB items, erasing the '0, 13' value from the array") {
            Dqn_VArray_EraseRange(&array, 1 /*begin_index*/, -DQN_ISIZE_MAX /*count*/, Dqn_VArrayErase_Stable);
            uint32_t array_literal[] = {14, 6, 7, 8};
            DQN_TESTER_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
            DQN_TESTER_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
        }

        DQN_TESTER_TEST("Test unstable erase, positive overflow OOB items, erasing the '0, 13' value from the array") {
            Dqn_VArray_EraseRange(&array, 2 /*begin_index*/, DQN_ISIZE_MAX /*count*/, Dqn_VArrayErase_Unstable);
            uint32_t array_literal[] = {14, 6};
            DQN_TESTER_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
            DQN_TESTER_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
        }
    }
    return test;
}

Dqn_Tester TestWin()
{
    Dqn_Tester test = {};
    DQN_TESTER_GROUP(test, "Dqn_Win") {
        DQN_TESTER_TEST("String8 to String16 size required") {
            int result = Dqn_Win_String8ToCString16(DQN_STRING8("a"), nullptr, 0);
            DQN_TESTER_ASSERTF(&test, result == 1, "Size returned: %d. This size should not include the null-terminator", result);
        }

        DQN_TESTER_TEST("String16 to String8 size required") {
            int result = Dqn_Win_String16ToCString8(DQN_STRING16(L"a"), nullptr, 0);
            DQN_TESTER_ASSERTF(&test, result == 1, "Size returned: %d. This size should not include the null-terminator", result);
        }

        DQN_TESTER_TEST("String8 to String16 size required") {
            int result = Dqn_Win_String8ToCString16(DQN_STRING8("String"), nullptr, 0);
            DQN_TESTER_ASSERTF(&test, result == 6, "Size returned: %d. This size should not include the null-terminator", result);
        }

        DQN_TESTER_TEST("String16 to String8 size required") {
            int result = Dqn_Win_String16ToCString8(DQN_STRING16(L"String"), nullptr, 0);
            DQN_TESTER_ASSERTF(&test, result == 6, "Size returned: %d. This size should not include the null-terminator", result);
        }

        DQN_TESTER_TEST("String8 to String16") {
            Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
            Dqn_String8 const INPUT   = DQN_STRING8("String");
            int size_required         = Dqn_Win_String8ToCString16(INPUT, nullptr, 0);
            wchar_t *string           = Dqn_Arena_NewArray(scratch.arena, wchar_t, size_required + 1, Dqn_ZeroMem_No);

            // Fill the string with error sentinels, which ensures the string is zero terminated
            DQN_MEMSET(string, 'Z', size_required + 1);

            int size_returned = Dqn_Win_String8ToCString16(INPUT, string, size_required + 1);
            wchar_t const EXPECTED[] = {L'S', L't', L'r', L'i', L'n', L'g', 0};

            DQN_TESTER_ASSERTF(&test, size_required == size_returned, "string_size: %d, result: %d", size_required, size_returned);
            DQN_TESTER_ASSERTF(&test, size_returned == DQN_ARRAY_UCOUNT(EXPECTED) - 1, "string_size: %d, expected: %zu", size_returned, DQN_ARRAY_UCOUNT(EXPECTED) - 1);
            DQN_TESTER_ASSERT(&test, DQN_MEMCMP(EXPECTED, string, sizeof(EXPECTED)) == 0);
        }

        DQN_TESTER_TEST("String16 to String8: No null-terminate") {
            Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
            Dqn_String16 INPUT        = DQN_STRING16(L"String");
            int size_required         = Dqn_Win_String16ToCString8(INPUT, nullptr, 0);
            char *string              = Dqn_Arena_NewArray(scratch.arena, char, size_required + 1, Dqn_ZeroMem_No);

            // Fill the string with error sentinels, which ensures the string is zero terminated
            DQN_MEMSET(string, 'Z', size_required + 1);

            int size_returned = Dqn_Win_String16ToCString8(INPUT, string, size_required + 1);
            char const EXPECTED[] = {'S', 't', 'r', 'i', 'n', 'g', 0};

            DQN_TESTER_ASSERTF(&test, size_required == size_returned, "string_size: %d, result: %d", size_required, size_returned);
            DQN_TESTER_ASSERTF(&test, size_returned == DQN_ARRAY_UCOUNT(EXPECTED) - 1, "string_size: %d, expected: %zu", size_returned, DQN_ARRAY_UCOUNT(EXPECTED) - 1);
            DQN_TESTER_ASSERT(&test, DQN_MEMCMP(EXPECTED, string, sizeof(EXPECTED)) == 0);
        }
    }
    return test;
}

#if defined(DQN_TEST_WITH_KECCAK)
#define DQN_TESTER_HASH_X_MACRO \
    DQN_TESTER_HASH_X_ENTRY(SHA3_224, "SHA3-224") \
    DQN_TESTER_HASH_X_ENTRY(SHA3_256, "SHA3-256") \
    DQN_TESTER_HASH_X_ENTRY(SHA3_384, "SHA3-384") \
    DQN_TESTER_HASH_X_ENTRY(SHA3_512, "SHA3-512") \
    DQN_TESTER_HASH_X_ENTRY(Keccak_224, "Keccak-224") \
    DQN_TESTER_HASH_X_ENTRY(Keccak_256, "Keccak-256") \
    DQN_TESTER_HASH_X_ENTRY(Keccak_384, "Keccak-384") \
    DQN_TESTER_HASH_X_ENTRY(Keccak_512, "Keccak-512") \
    DQN_TESTER_HASH_X_ENTRY(Count, "Keccak-512")

enum Dqn_Tests__HashType
{
#define DQN_TESTER_HASH_X_ENTRY(enum_val, string) Hash_##enum_val,
    DQN_TESTER_HASH_X_MACRO
#undef DQN_TESTER_HASH_X_ENTRY
};

Dqn_String8 const DQN_TESTER_HASH_STRING_[] =
{
#define DQN_TESTER_HASH_X_ENTRY(enum_val, string) DQN_STRING8(string),
    DQN_TESTER_HASH_X_MACRO
#undef DQN_TESTER_HASH_X_ENTRY
};

void TestKeccakDispatch_(Dqn_Tester *test, int hash_type, Dqn_String8 input)
{
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_String8 input_hex     = Dqn_Hex_BytesToString8Arena(scratch.arena, input.data, input.size);

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
}

Dqn_Tester TestKeccak()
{
    Dqn_Tester test = {};
    Dqn_String8 const INPUTS[] = {
        DQN_STRING8("abc"),
        DQN_STRING8(""),
        DQN_STRING8("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
        DQN_STRING8("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmno"
                    "pqrstnopqrstu"),
    };

    DQN_TESTER_GROUP(test, "Dqn_Keccak")
    {
        for (int hash_type = 0; hash_type < Hash_Count; hash_type++) {
            pcg32_random_t rng = {};
            pcg32_srandom_r(&rng, 0xd48e'be21'2af8'733d, 0x3f89'3bd2'd6b0'4eef);

            for (Dqn_String8 input : INPUTS) {
                Dqn_Tester_Begin(&test, "%.*s - Input: %.*s", DQN_STRING_FMT(DQN_TESTER_HASH_STRING_[hash_type]), DQN_CAST(int)DQN_MIN(input.size, 54), input.data);
                TestKeccakDispatch_(&test, hash_type, input);
                Dqn_Tester_End(&test);
            }

            Dqn_Tester_Begin(&test, "%.*s - Deterministic random inputs", DQN_STRING_FMT(DQN_TESTER_HASH_STRING_[hash_type]));
            for (int index = 0; index < 128; index++) {
                char    src[4096] = {};
                uint32_t src_size  = pcg32_boundedrand_r(&rng, sizeof(src));

                for (int src_index = 0; src_index < src_size; src_index++)
                    src[src_index] = pcg32_boundedrand_r(&rng, 255);

                Dqn_String8 input = Dqn_String8_Init(src, src_size);
                TestKeccakDispatch_(&test, hash_type, input);
            }
            Dqn_Tester_End(&test);
        }
    }
    return test;
}
#endif // defined(DQN_TEST_WITH_KECCAK)

void TestRunSuite()
{
    Dqn_Tester tests[]
    {
        TestBinarySearch(),
        TestCString8(),
        TestDSMap(),
        TestFString8(),
        TestFs(),
        TestFixedArray(),
        TestHex(),
        TestIntrinsics(),
        #if defined(DQN_TEST_WITH_KECCAK)
        TestKeccak(),
        #endif
        TestM4(),
        TestOS(),
        TestPerfCounter(),
        TestRect(),
        TestString8(),
        TestTicketMutex(),
        TestVArray(),
        TestWin(),
    };

    int total_tests = 0;
    int total_good_tests = 0;
    for (Dqn_Tester &test : tests) {
        total_tests += test.num_tests_in_group;
        total_good_tests += test.num_tests_ok_in_group;
    }

    fprintf(stdout, "Summary: %d/%d tests succeeded\n", total_good_tests, total_tests);
}

#if defined(DQN_TEST_WITH_MAIN)
int main(int argc, char *argv[])
{
    (void)argv; (void)argc;
    TestRunSuite();
    return 0;
}
#endif

