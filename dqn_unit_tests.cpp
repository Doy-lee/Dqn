#define DQN_UTEST_IMPLEMENTATION
#include "dqn_utest.h"

static Dqn_UTest Dqn_Test_Arena()
{
    Dqn_UTest test = {};
    DQN_UTEST_GROUP(test, "Dqn_Arena") {
        DQN_UTEST_TEST("Reused memory is zeroed out") {
            uint8_t   alignment  = 1;
            Dqn_usize alloc_size = DQN_KILOBYTES(128);
            Dqn_Arena arena      = {};
            DQN_DEFER {
                Dqn_Arena_Deinit(&arena);
            };

            // NOTE: Allocate 128 kilobytes, fill it with garbage, then reset the arena
            uintptr_t first_ptr_address = 0;
            {
                Dqn_ArenaTempMem temp_mem = Dqn_Arena_TempMemBegin(&arena);
                void *ptr         = Dqn_Arena_Alloc(&arena, alloc_size, alignment, Dqn_ZeroMem_Yes);
                first_ptr_address = DQN_CAST(uintptr_t)ptr;
                DQN_MEMSET(ptr, 'z', alloc_size);
                Dqn_Arena_TempMemEnd(temp_mem);
            }

            // NOTE: Reallocate 128 kilobytes
            char *ptr = DQN_CAST(char *)Dqn_Arena_Alloc(&arena, alloc_size, alignment, Dqn_ZeroMem_Yes);

            // NOTE: Double check we got the same pointer
            DQN_UTEST_ASSERT(&test, first_ptr_address == DQN_CAST(uintptr_t)ptr);

            // NOTE: Check that the bytes are set to 0
            for (Dqn_usize i = 0; i < alloc_size; i++)
                DQN_UTEST_ASSERT(&test, ptr[i] == 0);
        }

        DQN_UTEST_TEST("Test arena grows naturally, 1mb + 4mb") {
            // NOTE: Allocate 1mb, then 4mb, this should force the arena to grow
            Dqn_Arena arena   = Dqn_Arena_InitSize(DQN_MEGABYTES(2), DQN_MEGABYTES(2), Dqn_ArenaFlag_Nil);
            DQN_DEFER {
                Dqn_Arena_Deinit(&arena);
            };

            char     *ptr_1mb = Dqn_Arena_NewArray(&arena, char, DQN_MEGABYTES(1), Dqn_ZeroMem_Yes);
            char     *ptr_4mb = Dqn_Arena_NewArray(&arena, char, DQN_MEGABYTES(4), Dqn_ZeroMem_Yes);
            DQN_UTEST_ASSERT(&test, ptr_1mb);
            DQN_UTEST_ASSERT(&test, ptr_4mb);

            Dqn_ArenaBlock const *block_4mb_begin = arena.curr;
            char const           *block_4mb_end   = DQN_CAST(char *)block_4mb_begin + block_4mb_begin->reserve;

            Dqn_ArenaBlock const *block_1mb_begin = block_4mb_begin->prev;
            DQN_UTEST_ASSERTF(&test, block_1mb_begin, "New block should have been allocated");
            char const           *block_1mb_end   = DQN_CAST(char *)block_1mb_begin + block_1mb_begin->reserve;

            DQN_UTEST_ASSERTF(&test, block_1mb_begin != block_4mb_begin,                                     "New block should have been allocated and linked");
            DQN_UTEST_ASSERTF(&test, ptr_1mb >= DQN_CAST(char *)block_1mb_begin && ptr_1mb <= block_1mb_end, "Pointer was not allocated from correct memory block");
            DQN_UTEST_ASSERTF(&test, ptr_4mb >= DQN_CAST(char *)block_4mb_begin && ptr_4mb <= block_4mb_end, "Pointer was not allocated from correct memory block");
        }

        DQN_UTEST_TEST("Test arena grows naturally, 1mb, temp memory 4mb") {
            Dqn_Arena arena = Dqn_Arena_InitSize(DQN_MEGABYTES(2), DQN_MEGABYTES(2), Dqn_ArenaFlag_Nil);
            DQN_DEFER {
                Dqn_Arena_Deinit(&arena);
            };

            // NOTE: Allocate 1mb, then 4mb, this should force the arena to grow
            char *ptr_1mb = DQN_CAST(char *)Dqn_Arena_Alloc(&arena, DQN_MEGABYTES(1), 1 /*align*/, Dqn_ZeroMem_Yes);
            DQN_UTEST_ASSERT(&test, ptr_1mb);

            Dqn_ArenaTempMem temp_memory = Dqn_Arena_TempMemBegin(&arena);
            {
                char *ptr_4mb = Dqn_Arena_NewArray(&arena, char, DQN_MEGABYTES(4), Dqn_ZeroMem_Yes);
                DQN_UTEST_ASSERT(&test, ptr_4mb);

                Dqn_ArenaBlock const *block_4mb_begin = arena.curr;
                char const           *block_4mb_end   = DQN_CAST(char *) block_4mb_begin + block_4mb_begin->reserve;

                Dqn_ArenaBlock const *block_1mb_begin = block_4mb_begin->prev;
                char const           *block_1mb_end   = DQN_CAST(char *) block_1mb_begin + block_1mb_begin->reserve;

                DQN_UTEST_ASSERTF(&test, block_1mb_begin != block_4mb_begin,                     "New block should have been allocated and linked");
                DQN_UTEST_ASSERTF(&test, ptr_1mb >= DQN_CAST(char *)block_1mb_begin && ptr_1mb <= block_1mb_end, "Pointer was not allocated from correct memory block");
                DQN_UTEST_ASSERTF(&test, ptr_4mb >= DQN_CAST(char *)block_4mb_begin && ptr_4mb <= block_4mb_end, "Pointer was not allocated from correct memory block");
            }
            Dqn_Arena_TempMemEnd(temp_memory);
            DQN_UTEST_ASSERT (&test, arena.curr->prev == nullptr);
            DQN_UTEST_ASSERTF(&test, arena.curr->reserve >= DQN_MEGABYTES(1), "size=%lluMiB (%lluB), expect=%lluB", (arena.curr->reserve / 1024 / 1024), arena.curr->reserve, DQN_MEGABYTES(1));
        }
    }
    return test;
}

static Dqn_UTest Dqn_Test_Bin()
{
    Dqn_Scratch scratch = Dqn_Scratch_Get(nullptr);
    Dqn_UTest test           = {};
    DQN_UTEST_GROUP(test, "Dqn_Bin") {
        DQN_UTEST_TEST("Convert 0x123") {
            uint64_t result = Dqn_Bin_HexToU64(DQN_STR8("0x123"));
            DQN_UTEST_ASSERTF(&test, result == 0x123, "result: %llu", result);
        }

        DQN_UTEST_TEST("Convert 0xFFFF") {
            uint64_t result = Dqn_Bin_HexToU64(DQN_STR8("0xFFFF"));
            DQN_UTEST_ASSERTF(&test, result == 0xFFFF, "result: %llu", result);
        }

        DQN_UTEST_TEST("Convert FFFF") {
            uint64_t result = Dqn_Bin_HexToU64(DQN_STR8("FFFF"));
            DQN_UTEST_ASSERTF(&test, result == 0xFFFF, "result: %llu", result);
        }

        DQN_UTEST_TEST("Convert abCD") {
            uint64_t result = Dqn_Bin_HexToU64(DQN_STR8("abCD"));
            DQN_UTEST_ASSERTF(&test, result == 0xabCD, "result: %llu", result);
        }

        DQN_UTEST_TEST("Convert 0xabCD") {
            uint64_t result = Dqn_Bin_HexToU64(DQN_STR8("0xabCD"));
            DQN_UTEST_ASSERTF(&test, result == 0xabCD, "result: %llu", result);
        }

        DQN_UTEST_TEST("Convert 0x") {
            uint64_t result = Dqn_Bin_HexToU64(DQN_STR8("0x"));
            DQN_UTEST_ASSERTF(&test, result == 0x0, "result: %llu", result);
        }

        DQN_UTEST_TEST("Convert 0X") {
            uint64_t result = Dqn_Bin_HexToU64(DQN_STR8("0X"));
            DQN_UTEST_ASSERTF(&test, result == 0x0, "result: %llu", result);
        }

        DQN_UTEST_TEST("Convert 3") {
            uint64_t result = Dqn_Bin_HexToU64(DQN_STR8("3"));
            DQN_UTEST_ASSERTF(&test, result == 3, "result: %llu", result);
        }

        DQN_UTEST_TEST("Convert f") {
            uint64_t result = Dqn_Bin_HexToU64(DQN_STR8("f"));
            DQN_UTEST_ASSERTF(&test, result == 0xf, "result: %llu", result);
        }

        DQN_UTEST_TEST("Convert g") {
            uint64_t result = Dqn_Bin_HexToU64(DQN_STR8("g"));
            DQN_UTEST_ASSERTF(&test, result == 0, "result: %llu", result);
        }

        DQN_UTEST_TEST("Convert -0x3") {
            uint64_t result = Dqn_Bin_HexToU64(DQN_STR8("-0x3"));
            DQN_UTEST_ASSERTF(&test, result == 0, "result: %llu", result);
        }

        uint32_t number = 0xd095f6;
        DQN_UTEST_TEST("Convert %x to string", number) {
            Dqn_Str8 number_hex = Dqn_Bin_BytesToHexArena(scratch.arena, &number, sizeof(number));
            DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(number_hex, DQN_STR8("f695d000")), "number_hex=%.*s", DQN_STR_FMT(number_hex));
        }

        number = 0xf6ed00;
        DQN_UTEST_TEST("Convert %x to string", number) {
            Dqn_Str8 number_hex = Dqn_Bin_BytesToHexArena(scratch.arena, &number, sizeof(number));
            DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(number_hex, DQN_STR8("00edf600")), "number_hex=%.*s", DQN_STR_FMT(number_hex));
        }

        Dqn_Str8 hex = DQN_STR8("0xf6ed00");
        DQN_UTEST_TEST("Convert %.*s to bytes", DQN_STR_FMT(hex)) {
            Dqn_Str8 bytes = Dqn_Bin_HexToBytesArena(scratch.arena, hex);
            DQN_UTEST_ASSERTF(&test,
                               Dqn_Str8_Eq(bytes, DQN_STR8("\xf6\xed\x00")),
                               "number_hex=%.*s",
                               DQN_STR_FMT(Dqn_Bin_BytesToHexArena(scratch.arena, bytes.data, bytes.size)));
        }

    }
    return test;
}


static Dqn_UTest Dqn_Test_BinarySearch()
{
    Dqn_UTest test = {};
    DQN_UTEST_GROUP(test, "Dqn_BinarySearch") {
        DQN_UTEST_TEST("Search array of 1 item") {
            uint32_t               array[] = {1};
            Dqn_BinarySearchResult result  = {};

            // NOTE: Match =============================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);

            // NOTE: Lower bound =======================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);

            // NOTE: Upper bound =======================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);
        }

        DQN_UTEST_TEST("Search array of 2 items") {
            uint32_t               array[] = {1};
            Dqn_BinarySearchResult result  = {};

            // NOTE: Match =============================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);

            // NOTE: Lower bound =======================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);

            // NOTE: Upper bound =======================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);
        }

        DQN_UTEST_TEST("Search array of 3 items") {
            uint32_t               array[] = {1, 2, 3};
            Dqn_BinarySearchResult result  = {};

            // NOTE: Match =============================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 3U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 2);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 4U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 3);

            // NOTE: Lower bound =======================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 3U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 2);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 4U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 3);

            // NOTE: Upper bound =======================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 2);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 3U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 3);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 4U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 3);
        }

        DQN_UTEST_TEST("Search array of 4 items") {
            uint32_t               array[] = {1, 2, 3, 4};
            Dqn_BinarySearchResult result  = {};

            // NOTE: Match =============================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 3U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 2);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 4U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 3);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 5U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 4);

            // NOTE: Lower bound =======================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 3U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 2);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 4U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 3);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 5U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 4);

            // NOTE: Upper bound =======================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 1);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 2);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 3U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 3);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 4U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 4);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 5U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 4);
        }

        DQN_UTEST_TEST("Search array with duplicate items") {
            uint32_t               array[] = {1, 1, 2, 2, 3};
            Dqn_BinarySearchResult result  = {};

            // NOTE: Match =============================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 2);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 3U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 4);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 4U /*find*/, Dqn_BinarySearchType_Match);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 5);

            // NOTE: Lower bound =======================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 2);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 3U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 4);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 4U /*find*/, Dqn_BinarySearchType_LowerBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 5);

            // NOTE: Upper bound =======================================================================
            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 0U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 1U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 2);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 2U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 4);

            result = Dqn_BinarySearch<uint32_t>(array, DQN_ARRAY_UCOUNT(array), 3U /*find*/, Dqn_BinarySearchType_UpperBound);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 5);
        }
    }
    return test;
}

static Dqn_UTest Dqn_Test_DSMap()
{
    Dqn_UTest test = {};
    DQN_UTEST_GROUP(test, "Dqn_DSMap") {
        Dqn_Scratch scratch = Dqn_Scratch_Get(nullptr);
        {
            Dqn_Arena           arena    = {};
            uint32_t const      MAP_SIZE = 64;
            Dqn_DSMap<uint64_t> map      = Dqn_DSMap_Init<uint64_t>(&arena, MAP_SIZE);
            DQN_DEFER { Dqn_DSMap_Deinit(&map, Dqn_ZeroMem_Yes); };

            DQN_UTEST_TEST("Find non-existent value") {
                uint64_t *value = Dqn_DSMap_FindKeyStr8(&map, DQN_STR8("Foo")).value;
                DQN_UTEST_ASSERT(&test, !value);
                DQN_UTEST_ASSERT(&test, map.size         == MAP_SIZE);
                DQN_UTEST_ASSERT(&test, map.initial_size == MAP_SIZE);
                DQN_UTEST_ASSERT(&test, map.occupied     == 1 /*Sentinel*/);
            }

            Dqn_DSMapKey key = Dqn_DSMap_KeyCStr8(&map, "Bar");
            DQN_UTEST_TEST("Insert value and lookup") {
                uint64_t desired_value  = 0xF00BAA;
                uint64_t *slot_value = Dqn_DSMap_Set(&map, key, desired_value).value;
                DQN_UTEST_ASSERT(&test, slot_value);
                DQN_UTEST_ASSERT(&test, map.size == MAP_SIZE);
                DQN_UTEST_ASSERT(&test, map.initial_size == MAP_SIZE);
                DQN_UTEST_ASSERT(&test, map.occupied == 2);

                uint64_t *value = Dqn_DSMap_Find(&map, key).value;
                DQN_UTEST_ASSERT(&test, value);
                DQN_UTEST_ASSERT(&test, *value == desired_value);
            }

            DQN_UTEST_TEST("Remove key") {
                Dqn_DSMap_Erase(&map, key);
                DQN_UTEST_ASSERT(&test, map.size         == MAP_SIZE);
                DQN_UTEST_ASSERT(&test, map.initial_size == MAP_SIZE);
                DQN_UTEST_ASSERT(&test, map.occupied     == 1 /*Sentinel*/);
            }
        }

        enum DSMapTestType { DSMapTestType_Set, DSMapTestType_MakeSlot, DSMapTestType_Count };
        for (int test_type = 0; test_type < DSMapTestType_Count; test_type++) {
            Dqn_Str8 prefix = {};
            switch (test_type) {
                case DSMapTestType_Set:      prefix = DQN_STR8("Set"); break;
                case DSMapTestType_MakeSlot: prefix = DQN_STR8("Make slot"); break;
            }

            Dqn_ArenaTempMemScope temp_mem_scope = Dqn_ArenaTempMemScope(scratch.arena);
            Dqn_Arena             arena          = {};
            uint32_t const        MAP_SIZE       = 64;
            Dqn_DSMap<uint64_t>   map            = Dqn_DSMap_Init<uint64_t>(&arena, MAP_SIZE);
            DQN_DEFER { Dqn_DSMap_Deinit(&map, Dqn_ZeroMem_Yes); };

            DQN_UTEST_TEST("%.*s: Test growing", DQN_STR_FMT(prefix)) {
                uint64_t map_start_size  = map.size;
                uint64_t value          = 0;
                uint64_t grow_threshold = map_start_size * 3 / 4;
                for (; map.occupied != grow_threshold; value++) {
                    uint64_t *val_copy = Dqn_Arena_NewCopy(scratch.arena, uint64_t, &value);
                    Dqn_DSMapKey key   = Dqn_DSMap_KeyBuffer(&map, (char *)val_copy, sizeof(*val_copy));
                    DQN_UTEST_ASSERT(&test, !Dqn_DSMap_Find<uint64_t>(&map, key).value);
                    Dqn_DSMapResult<uint64_t> make_result = {};
                    if (test_type == DSMapTestType_Set) {
                        make_result = Dqn_DSMap_Set(&map, key, value);
                    } else {
                        make_result = Dqn_DSMap_Make(&map, key);
                    }
                    DQN_UTEST_ASSERT(&test, !make_result.found);
                    DQN_UTEST_ASSERT(&test, Dqn_DSMap_Find<uint64_t>(&map, key).value);
                }
                DQN_UTEST_ASSERT(&test, map.initial_size == MAP_SIZE);
                DQN_UTEST_ASSERT(&test, map.size         == map_start_size);
                DQN_UTEST_ASSERT(&test, map.occupied     == 1 /*Sentinel*/ + value);

                { // NOTE: One more item should cause the table to grow by 2x
                    uint64_t *val_copy = Dqn_Arena_NewCopy(scratch.arena, uint64_t, &value);
                    Dqn_DSMapKey key   = Dqn_DSMap_KeyBuffer(&map, (char *)val_copy, sizeof(*val_copy));
                    Dqn_DSMapResult<uint64_t> make_result = {};
                    if (test_type == DSMapTestType_Set) {
                        make_result = Dqn_DSMap_Set(&map, key, value);
                    } else {
                        make_result = Dqn_DSMap_Make(&map, key);
                    }

                    value++;
                    DQN_UTEST_ASSERT(&test, !make_result.found);
                    DQN_UTEST_ASSERT(&test, map.size         == map_start_size * 2);
                    DQN_UTEST_ASSERT(&test, map.initial_size == MAP_SIZE);
                    DQN_UTEST_ASSERT(&test, map.occupied     == 1 /*Sentinel*/ + value);
                }
            }

            DQN_UTEST_TEST("%.*s: Check the sentinel is present", DQN_STR_FMT(prefix)) {
                Dqn_DSMapSlot<uint64_t> NIL_SLOT = {};
                Dqn_DSMapSlot<uint64_t> sentinel = map.slots[DQN_DS_MAP_SENTINEL_SLOT];
                DQN_UTEST_ASSERT(&test, DQN_MEMCMP(&sentinel, &NIL_SLOT, sizeof(NIL_SLOT)) == 0);
            }

            DQN_UTEST_TEST("%.*s: Recheck all the hash tables values after growing", DQN_STR_FMT(prefix)) {
                for (uint64_t index = 1 /*Sentinel*/; index < map.occupied; index++) {
                    Dqn_DSMapSlot<uint64_t> const *slot = map.slots + index;

                    // NOTE: Validate each slot value
                    uint64_t value_test = index - 1;
                    Dqn_DSMapKey key    = Dqn_DSMap_KeyBuffer(&map, &value_test, sizeof(value_test));
                    DQN_UTEST_ASSERT(&test, Dqn_DSMap_KeyEquals(slot->key, key));
                    if (test_type == DSMapTestType_Set) {
                        DQN_UTEST_ASSERT(&test, slot->value == value_test);
                    } else {
                        DQN_UTEST_ASSERT(&test, slot->value == 0); // NOTE: Make slot does not set the key so should be 0
                    }
                    DQN_UTEST_ASSERT(&test, slot->key.hash == Dqn_DSMap_Hash(&map, slot->key));

                    // NOTE: Check the reverse lookup is correct
                    Dqn_DSMapResult<uint64_t> check = Dqn_DSMap_Find(&map, slot->key);
                    DQN_UTEST_ASSERT(&test, slot->value == *check.value);
                }
            }

            DQN_UTEST_TEST("%.*s: Test shrinking", DQN_STR_FMT(prefix)) {
                uint64_t start_map_size     = map.size;
                uint64_t start_map_occupied = map.occupied;
                uint64_t value              = 0;
                uint64_t shrink_threshold   = map.size * 1 / 4;
                for (; map.occupied != shrink_threshold; value++) {
                    uint64_t *val_copy = Dqn_Arena_NewCopy(scratch.arena, uint64_t, &value);
                    Dqn_DSMapKey key   = Dqn_DSMap_KeyBuffer(&map, (char *)val_copy, sizeof(*val_copy));

                    DQN_UTEST_ASSERT(&test, Dqn_DSMap_Find<uint64_t>(&map, key).value);
                    Dqn_DSMap_Erase(&map, key);
                    DQN_UTEST_ASSERT(&test, !Dqn_DSMap_Find<uint64_t>(&map, key).value);
                }
                DQN_UTEST_ASSERT(&test, map.size == start_map_size);
                DQN_UTEST_ASSERT(&test, map.occupied == start_map_occupied - value);

                { // NOTE: One more item should cause the table to grow by 2x
                    uint64_t *val_copy = Dqn_Arena_NewCopy(scratch.arena, uint64_t, &value);
                    Dqn_DSMapKey key   = Dqn_DSMap_KeyBuffer(&map, (char *)val_copy, sizeof(*val_copy));
                    Dqn_DSMap_Erase(&map, key);
                    value++;

                    DQN_UTEST_ASSERT(&test, map.size     == start_map_size / 2);
                    DQN_UTEST_ASSERT(&test, map.occupied == start_map_occupied - value);
                }

                { // NOTE: Check the sentinel is present
                    Dqn_DSMapSlot<uint64_t> NIL_SLOT = {};
                    Dqn_DSMapSlot<uint64_t> sentinel = map.slots[DQN_DS_MAP_SENTINEL_SLOT];
                    DQN_UTEST_ASSERT(&test, DQN_MEMCMP(&sentinel, &NIL_SLOT, sizeof(NIL_SLOT)) == 0);
                }

                // NOTE: Recheck all the hash table values after growing
                for (uint64_t index = 1 /*Sentinel*/; index < map.occupied; index++) {

                    // NOTE: Generate the key
                    uint64_t value_test = value + (index - 1);
                    Dqn_DSMapKey key    = Dqn_DSMap_KeyBuffer(&map, (char *)&value_test, sizeof(value_test));

                    // NOTE: Validate each slot value
                    Dqn_DSMapResult<uint64_t> find_result = Dqn_DSMap_Find(&map, key);
                    DQN_UTEST_ASSERT(&test, find_result.value);
                    DQN_UTEST_ASSERT(&test, find_result.slot->key == key);
                    if (test_type == DSMapTestType_Set) {
                        DQN_UTEST_ASSERT(&test, *find_result.value == value_test);
                    } else {
                        DQN_UTEST_ASSERT(&test, *find_result.value == 0); // NOTE: Make slot does not set the key so should be 0
                    }
                    DQN_UTEST_ASSERT(&test, find_result.slot->key.hash == Dqn_DSMap_Hash(&map, find_result.slot->key));

                    // NOTE: Check the reverse lookup is correct
                    Dqn_DSMapResult<uint64_t> check = Dqn_DSMap_Find(&map, find_result.slot->key);
                    DQN_UTEST_ASSERT(&test, *find_result.value == *check.value);
                }

                for (; map.occupied != 1; value++) { // NOTE: Remove all items from the table
                    uint64_t *val_copy = Dqn_Arena_NewCopy(scratch.arena, uint64_t, &value);
                    Dqn_DSMapKey key   = Dqn_DSMap_KeyBuffer(&map, (char *)val_copy, sizeof(*val_copy));
                    DQN_UTEST_ASSERT(&test, Dqn_DSMap_Find<uint64_t>(&map, key).value);
                    Dqn_DSMap_Erase(&map, key);
                    DQN_UTEST_ASSERT(&test, !Dqn_DSMap_Find<uint64_t>(&map, key).value);
                }
                DQN_UTEST_ASSERT(&test, map.initial_size == MAP_SIZE);
                DQN_UTEST_ASSERT(&test, map.size == map.initial_size);
                DQN_UTEST_ASSERT(&test, map.occupied == 1 /*Sentinel*/);
            }
        }
    }
    return test;
}

static Dqn_UTest Dqn_Test_FStr8()
{
    Dqn_UTest test = {};
    DQN_UTEST_GROUP(test, "Dqn_FStr8") {
        DQN_UTEST_TEST("Append too much fails") {
            Dqn_FStr8<4> str = {};
            DQN_UTEST_ASSERT(&test, !Dqn_FStr8_Append(&str, DQN_STR8("abcde")));
        }

        DQN_UTEST_TEST("Append format string too much fails") {
            Dqn_FStr8<4> str = {};
            DQN_UTEST_ASSERT(&test, !Dqn_FStr8_AppendF(&str, "abcde"));
        }
    }
    return test;
}

static Dqn_UTest Dqn_Test_Fs()
{
    Dqn_UTest test = {};
    DQN_UTEST_GROUP(test, "Dqn_Fs") {
        DQN_UTEST_TEST("Make directory recursive \"abcd/efgh\"") {
            DQN_UTEST_ASSERTF(&test, Dqn_OS_DirMake(DQN_STR8("abcd/efgh")), "Failed to make directory");
            DQN_UTEST_ASSERTF(&test, Dqn_OS_DirExists(DQN_STR8("abcd")), "Directory was not made");
            DQN_UTEST_ASSERTF(&test, Dqn_OS_DirExists(DQN_STR8("abcd/efgh")), "Subdirectory was not made");
            DQN_UTEST_ASSERTF(&test, Dqn_OS_FileExists(DQN_STR8("abcd")) == false, "This function should only return true for files");
            DQN_UTEST_ASSERTF(&test, Dqn_OS_FileExists(DQN_STR8("abcd/efgh")) == false, "This function should only return true for files");
            DQN_UTEST_ASSERTF(&test, Dqn_OS_PathDelete(DQN_STR8("abcd/efgh")), "Failed to delete directory");
            DQN_UTEST_ASSERTF(&test, Dqn_OS_PathDelete(DQN_STR8("abcd")), "Failed to cleanup directory");
        }

        DQN_UTEST_TEST("File write, read, copy, move and delete") {
            // NOTE: Write step
            Dqn_Str8 const SRC_FILE     = DQN_STR8("dqn_test_file");
            Dqn_b32        write_result = Dqn_OS_WriteAll(SRC_FILE, DQN_STR8("test"), nullptr);
            DQN_UTEST_ASSERT(&test, write_result);
            DQN_UTEST_ASSERT(&test, Dqn_OS_FileExists(SRC_FILE));

            // NOTE: Read step
            Dqn_Scratch scratch   = Dqn_Scratch_Get(nullptr);
            Dqn_Str8    read_file = Dqn_OS_ReadAll(SRC_FILE, scratch.arena, nullptr);
            DQN_UTEST_ASSERTF(&test, Dqn_Str8_HasData(read_file), "Failed to load file");
            DQN_UTEST_ASSERTF(&test, read_file.size == 4, "File read wrong amount of bytes");
            DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(read_file, DQN_STR8("test")), "read(%zu): %.*s", read_file.size, DQN_STR_FMT(read_file));

            // NOTE: Copy step
            Dqn_Str8 const COPY_FILE   = DQN_STR8("dqn_test_file_copy");
            Dqn_b32        copy_result = Dqn_OS_FileCopy(SRC_FILE, COPY_FILE, true /*overwrite*/, nullptr);
            DQN_UTEST_ASSERT(&test, copy_result);
            DQN_UTEST_ASSERT(&test, Dqn_OS_FileExists(COPY_FILE));

            // NOTE: Move step
            Dqn_Str8 const MOVE_FILE   = DQN_STR8("dqn_test_file_move");
            Dqn_b32        move_result = Dqn_OS_FileMove(COPY_FILE, MOVE_FILE, true /*overwrite*/, nullptr);
            DQN_UTEST_ASSERT(&test, move_result);
            DQN_UTEST_ASSERT(&test,  Dqn_OS_FileExists(MOVE_FILE));
            DQN_UTEST_ASSERTF(&test, Dqn_OS_FileExists(COPY_FILE) == false, "Moving a file should remove the original");

            // NOTE: Delete step
            Dqn_b32 delete_src_file   = Dqn_OS_PathDelete(SRC_FILE);
            Dqn_b32 delete_moved_file = Dqn_OS_PathDelete(MOVE_FILE);
            DQN_UTEST_ASSERT(&test, delete_src_file);
            DQN_UTEST_ASSERT(&test, delete_moved_file);

            // NOTE: Deleting non-existent file fails
            Dqn_b32 delete_non_existent_src_file   = Dqn_OS_PathDelete(SRC_FILE);
            Dqn_b32 delete_non_existent_moved_file = Dqn_OS_PathDelete(MOVE_FILE);
            DQN_UTEST_ASSERT(&test, delete_non_existent_moved_file == false);
            DQN_UTEST_ASSERT(&test, delete_non_existent_src_file == false);
        }
    }
    return test;
}

static Dqn_UTest Dqn_Test_FixedArray()
{
    Dqn_UTest test = {};
    DQN_UTEST_GROUP(test, "Dqn_FArray") {
        DQN_UTEST_TEST("Initialise from raw array") {
            int raw_array[] = {1, 2};
            auto array = Dqn_FArray_Init<int, 4>(raw_array, DQN_ARRAY_UCOUNT(raw_array));
            DQN_UTEST_ASSERT(&test, array.size == 2);
            DQN_UTEST_ASSERT(&test, array.data[0] == 1);
            DQN_UTEST_ASSERT(&test, array.data[1] == 2);
        }

        DQN_UTEST_TEST("Erase stable 1 element from array") {
            int raw_array[] = {1, 2, 3};
            auto array = Dqn_FArray_Init<int, 4>(raw_array, DQN_ARRAY_UCOUNT(raw_array));
            Dqn_FArray_EraseRange(&array, 1 /*begin_index*/, 1 /*count*/, Dqn_ArrayErase_Stable);
            DQN_UTEST_ASSERT(&test, array.size == 2);
            DQN_UTEST_ASSERT(&test, array.data[0] == 1);
            DQN_UTEST_ASSERT(&test, array.data[1] == 3);
        }

        DQN_UTEST_TEST("Erase unstable 1 element from array") {
            int raw_array[] = {1, 2, 3};
            auto array = Dqn_FArray_Init<int, 4>(raw_array, DQN_ARRAY_UCOUNT(raw_array));
            Dqn_FArray_EraseRange(&array, 0 /*begin_index*/, 1 /*count*/, Dqn_ArrayErase_Unstable);
            DQN_UTEST_ASSERT(&test, array.size == 2);
            DQN_UTEST_ASSERT(&test, array.data[0] == 3);
            DQN_UTEST_ASSERT(&test, array.data[1] == 2);
        }

        DQN_UTEST_TEST("Add 1 element to array") {
            int const ITEM  = 2;
            int raw_array[] = {1};
            auto array      = Dqn_FArray_Init<int, 4>(raw_array, DQN_ARRAY_UCOUNT(raw_array));
            Dqn_FArray_Add(&array, ITEM);
            DQN_UTEST_ASSERT(&test, array.size == 2);
            DQN_UTEST_ASSERT(&test, array.data[0] == 1);
            DQN_UTEST_ASSERT(&test, array.data[1] == ITEM);
        }

        DQN_UTEST_TEST("Clear array") {
            int raw_array[] = {1};
            auto array      = Dqn_FArray_Init<int, 4>(raw_array, DQN_ARRAY_UCOUNT(raw_array));
            Dqn_FArray_Clear(&array);
            DQN_UTEST_ASSERT(&test, array.size == 0);
        }
    }
    return test;
}

static Dqn_UTest Dqn_Test_Intrinsics()
{
    Dqn_UTest test = {};
    // TODO(dqn): We don't have meaningful tests here, but since
    // atomics/intrinsics are implemented using macros we ensure the macro was
    // written properly with these tests.

    DQN_MSVC_WARNING_PUSH

    // NOTE: MSVC SAL complains that we are using Interlocked functionality on
    // variables it has detected as *not* being shared across threads. This is
    // fine, we're just running some basic tests, so permit it.
    //
    // Warning 28112 is a knock-on effect of this that it doesn't like us
    // reading the value of the variable that has been used in an Interlocked
    // function locally.
    DQN_MSVC_WARNING_DISABLE(28113) // Accessing a local variable val via an Interlocked function.
    DQN_MSVC_WARNING_DISABLE(28112) // A variable (val) which is accessed via an Interlocked function must always be accessed via an Interlocked function. See line 759.

    DQN_UTEST_GROUP(test, "Dqn_Atomic") {
        DQN_UTEST_TEST("Dqn_Atomic_AddU32") {
            uint32_t val = 0;
            Dqn_Atomic_AddU32(&val, 1);
            DQN_UTEST_ASSERTF(&test, val == 1, "val: %u", val);
        }

        DQN_UTEST_TEST("Dqn_Atomic_AddU64") {
            uint64_t val = 0;
            Dqn_Atomic_AddU64(&val, 1);
            DQN_UTEST_ASSERTF(&test, val == 1, "val: %llu", val);
        }

        DQN_UTEST_TEST("Dqn_Atomic_SubU32") {
            uint32_t val = 1;
            Dqn_Atomic_SubU32(&val, 1);
            DQN_UTEST_ASSERTF(&test, val == 0, "val: %u", val);
        }

        DQN_UTEST_TEST("Dqn_Atomic_SubU64") {
            uint64_t val = 1;
            Dqn_Atomic_SubU64(&val, 1);
            DQN_UTEST_ASSERTF(&test, val == 0, "val: %llu", val);
        }

        DQN_UTEST_TEST("Dqn_Atomic_SetValue32") {
            long a = 0;
            long b = 111;
            Dqn_Atomic_SetValue32(&a, b);
            DQN_UTEST_ASSERTF(&test, a == b, "a: %ld, b: %ld", a, b);
        }

        DQN_UTEST_TEST("Dqn_Atomic_SetValue64") {
            int64_t a = 0;
            int64_t b = 111;
            Dqn_Atomic_SetValue64(DQN_CAST(uint64_t *)&a, b);
            DQN_UTEST_ASSERTF(&test, a == b, "a: %lld, b: %lld", a, b);
        }

        Dqn_UTest_Begin(&test, "Dqn_CPU_TSC");
        Dqn_CPU_TSC();
        Dqn_UTest_End(&test);

        Dqn_UTest_Begin(&test, "Dqn_CompilerReadBarrierAndCPUReadFence");
        Dqn_CompilerReadBarrierAndCPUReadFence;
        Dqn_UTest_End(&test);

        Dqn_UTest_Begin(&test, "Dqn_CompilerWriteBarrierAndCPUWriteFence");
        Dqn_CompilerWriteBarrierAndCPUWriteFence;
        Dqn_UTest_End(&test);
    }
    DQN_MSVC_WARNING_POP

    return test;
}

#if defined(DQN_TEST_WITH_KECCAK)
#define DQN_UTEST_HASH_X_MACRO \
    DQN_UTEST_HASH_X_ENTRY(SHA3_224, "SHA3-224") \
    DQN_UTEST_HASH_X_ENTRY(SHA3_256, "SHA3-256") \
    DQN_UTEST_HASH_X_ENTRY(SHA3_384, "SHA3-384") \
    DQN_UTEST_HASH_X_ENTRY(SHA3_512, "SHA3-512") \
    DQN_UTEST_HASH_X_ENTRY(Keccak_224, "Keccak-224") \
    DQN_UTEST_HASH_X_ENTRY(Keccak_256, "Keccak-256") \
    DQN_UTEST_HASH_X_ENTRY(Keccak_384, "Keccak-384") \
    DQN_UTEST_HASH_X_ENTRY(Keccak_512, "Keccak-512") \
    DQN_UTEST_HASH_X_ENTRY(Count, "Keccak-512")

enum Dqn_Tests__HashType
{
#define DQN_UTEST_HASH_X_ENTRY(enum_val, string) Hash_##enum_val,
    DQN_UTEST_HASH_X_MACRO
#undef DQN_UTEST_HASH_X_ENTRY
};

Dqn_Str8 const DQN_UTEST_HASH_STRING_[] =
{
#define DQN_UTEST_HASH_X_ENTRY(enum_val, string) DQN_STR8(string),
    DQN_UTEST_HASH_X_MACRO
#undef DQN_UTEST_HASH_X_ENTRY
};

void Dqn_Test_KeccakDispatch_(Dqn_UTest *test, int hash_type, Dqn_Str8 input)
{
    Dqn_Scratch scratch = Dqn_Scratch_Get(nullptr);
    Dqn_Str8 input_hex     = Dqn_Hex_BytesToStr8Arena(scratch.arena, input.data, input.size);

    switch(hash_type)
    {
        case Hash_SHA3_224:
        {
            Dqn_KeccakBytes28 hash = Dqn_SHA3_224StringToBytes28(input);
            Dqn_KeccakBytes28 expect;
            FIPS202_SHA3_224(DQN_CAST(u8 *)input.data, input.size, (u8 *)expect.data);
            DQN_UTEST_ASSERTF(test,
                                Dqn_KeccakBytes28Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STR_FMT(input_hex),
                                DQN_KECCAK_STRING56_FMT(Dqn_KeccakBytes28ToHex(&hash).data),
                                DQN_KECCAK_STRING56_FMT(Dqn_KeccakBytes28ToHex(&expect).data));
        }
        break;

        case Hash_SHA3_256:
        {
            Dqn_KeccakBytes32 hash = Dqn_SHA3_256StringToBytes32(input);
            Dqn_KeccakBytes32 expect;
            FIPS202_SHA3_256(DQN_CAST(u8 *)input.data, input.size, (u8 *)expect.data);
            DQN_UTEST_ASSERTF(test,
                                Dqn_KeccakBytes32Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STR_FMT(input_hex),
                                DQN_KECCAK_STRING64_FMT(Dqn_KeccakBytes32ToHex(&hash).data),
                                DQN_KECCAK_STRING64_FMT(Dqn_KeccakBytes32ToHex(&expect).data));
        }
        break;

        case Hash_SHA3_384:
        {
            Dqn_KeccakBytes48 hash = Dqn_SHA3_384StringToBytes48(input);
            Dqn_KeccakBytes48 expect;
            FIPS202_SHA3_384(DQN_CAST(u8 *)input.data, input.size, (u8 *)expect.data);
            DQN_UTEST_ASSERTF(test,
                                Dqn_KeccakBytes48Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STR_FMT(input_hex),
                                DQN_KECCAK_STRING96_FMT(Dqn_KeccakBytes48ToHex(&hash).data),
                                DQN_KECCAK_STRING96_FMT(Dqn_KeccakBytes48ToHex(&expect).data));
        }
        break;

        case Hash_SHA3_512:
        {
            Dqn_KeccakBytes64 hash = Dqn_SHA3_512StringToBytes64(input);
            Dqn_KeccakBytes64 expect;
            FIPS202_SHA3_512(DQN_CAST(u8 *)input.data, input.size, (u8 *)expect.data);
            DQN_UTEST_ASSERTF(test,
                                Dqn_KeccakBytes64Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STR_FMT(input_hex),
                                DQN_KECCAK_STRING128_FMT(Dqn_KeccakBytes64ToHex(&hash).data),
                                DQN_KECCAK_STRING128_FMT(Dqn_KeccakBytes64ToHex(&expect).data));
        }
        break;

        case Hash_Keccak_224:
        {
            Dqn_KeccakBytes28 hash = Dqn_Keccak224StringToBytes28(input);
            Dqn_KeccakBytes28 expect;
            Keccak(1152, 448, DQN_CAST(u8 *)input.data, input.size, 0x01, (u8 *)expect.data, sizeof(expect));
            DQN_UTEST_ASSERTF(test,
                                Dqn_KeccakBytes28Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STR_FMT(input_hex),
                                DQN_KECCAK_STRING56_FMT(Dqn_KeccakBytes28ToHex(&hash).data),
                                DQN_KECCAK_STRING56_FMT(Dqn_KeccakBytes28ToHex(&expect).data));
        }
        break;

        case Hash_Keccak_256:
        {
            Dqn_KeccakBytes32 hash = Dqn_Keccak256StringToBytes32(input);
            Dqn_KeccakBytes32 expect;
            Keccak(1088, 512, DQN_CAST(u8 *)input.data, input.size, 0x01, (u8 *)expect.data, sizeof(expect));
            DQN_UTEST_ASSERTF(test,
                                Dqn_KeccakBytes32Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STR_FMT(input_hex),
                                DQN_KECCAK_STRING64_FMT(Dqn_KeccakBytes32ToHex(&hash).data),
                                DQN_KECCAK_STRING64_FMT(Dqn_KeccakBytes32ToHex(&expect).data));
        }
        break;

        case Hash_Keccak_384:
        {
            Dqn_KeccakBytes48 hash = Dqn_Keccak384StringToBytes48(input);
            Dqn_KeccakBytes48 expect;
            Keccak(832, 768, DQN_CAST(u8 *)input.data, input.size, 0x01, (u8 *)expect.data, sizeof(expect));
            DQN_UTEST_ASSERTF(test,
                                Dqn_KeccakBytes48Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STR_FMT(input_hex),
                                DQN_KECCAK_STRING96_FMT(Dqn_KeccakBytes48ToHex(&hash).data),
                                DQN_KECCAK_STRING96_FMT(Dqn_KeccakBytes48ToHex(&expect).data));
        }
        break;

        case Hash_Keccak_512:
        {
            Dqn_KeccakBytes64 hash = Dqn_Keccak512StringToBytes64(input);
            Dqn_KeccakBytes64 expect;
            Keccak(576, 1024, DQN_CAST(u8 *)input.data, input.size, 0x01, (u8 *)expect.data, sizeof(expect));
            DQN_UTEST_ASSERTF(test,
                                Dqn_KeccakBytes64Equals(&hash, &expect),
                                "\ninput:  %.*s"
                                "\nhash:   %.*s"
                                "\nexpect: %.*s"
                                ,
                                DQN_STR_FMT(input_hex),
                                DQN_KECCAK_STRING128_FMT(Dqn_KeccakBytes64ToHex(&hash).data),
                                DQN_KECCAK_STRING128_FMT(Dqn_KeccakBytes64ToHex(&expect).data));
        }
        break;

    }
}

Dqn_UTest Dqn_Test_Keccak()
{
    Dqn_UTest test = {};
    Dqn_Str8 const INPUTS[] = {
        DQN_STR8("abc"),
        DQN_STR8(""),
        DQN_STR8("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
        DQN_STR8("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmno"
                 "pqrstnopqrstu"),
    };

    DQN_UTEST_GROUP(test, "Dqn_Keccak")
    {
        for (int hash_type = 0; hash_type < Hash_Count; hash_type++) {
            pcg32_random_t rng = {};
            pcg32_srandom_r(&rng, 0xd48e'be21'2af8'733d, 0x3f89'3bd2'd6b0'4eef);

            for (Dqn_Str8 input : INPUTS) {
                Dqn_UTest_Begin(&test, "%.*s - Input: %.*s", DQN_STR_FMT(DQN_UTEST_HASH_STRING_[hash_type]), DQN_CAST(int)DQN_MIN(input.size, 54), input.data);
                Dqn_Test_KeccakDispatch_(&test, hash_type, input);
                Dqn_UTest_End(&test);
            }

            Dqn_UTest_Begin(&test, "%.*s - Deterministic random inputs", DQN_STR_FMT(DQN_UTEST_HASH_STRING_[hash_type]));
            for (int index = 0; index < 128; index++) {
                char    src[4096] = {};
                uint32_t src_size  = pcg32_boundedrand_r(&rng, sizeof(src));

                for (int src_index = 0; src_index < src_size; src_index++)
                    src[src_index] = pcg32_boundedrand_r(&rng, 255);

                Dqn_Str8 input = Dqn_Str8_Init(src, src_size);
                TestKeccakDispatch_(&test, hash_type, input);
            }
            Dqn_UTest_End(&test);
        }
    }
    return test;
}
#endif // defined(DQN_TEST_WITH_KECCAK)

static Dqn_UTest Dqn_Test_M4()
{
    Dqn_UTest test = {};
    DQN_UTEST_GROUP(test, "Dqn_M4") {
        DQN_UTEST_TEST("Simple translate and scale matrix") {
            Dqn_M4 translate = Dqn_M4_TranslateF(1, 2, 3);
            Dqn_M4 scale     = Dqn_M4_ScaleF(2, 2, 2);
            Dqn_M4 result    = Dqn_M4_Mul(translate, scale);

            const Dqn_M4 EXPECT = {{
                {2, 0, 0, 0},
                {0, 2, 0, 0},
                {0, 0, 2, 0},
                {1, 2, 3, 1},
            }};

            DQN_UTEST_ASSERTF(&test,
                               memcmp(result.columns, EXPECT.columns, sizeof(EXPECT)) == 0,
                               "\nresult =\n%s\nexpected =\n%s",
                               Dqn_M4_ColumnMajorString(result).data,
                               Dqn_M4_ColumnMajorString(EXPECT).data);
        }
    }
    return test;
}

static Dqn_UTest Dqn_Test_OS()
{
    Dqn_UTest test = {};
    DQN_UTEST_GROUP(test, "Dqn_OS") {
        DQN_UTEST_TEST("Generate secure RNG bytes with nullptr") {
            Dqn_b32 result = Dqn_OS_SecureRNGBytes(nullptr, 1);
            DQN_UTEST_ASSERT(&test, result == false);
        }

        DQN_UTEST_TEST("Generate secure RNG 32 bytes") {
            char const ZERO[32] = {};
            char       buf[32]  = {};
            bool result         = Dqn_OS_SecureRNGBytes(buf, DQN_ARRAY_UCOUNT(buf));
            DQN_UTEST_ASSERT(&test, result);
            DQN_UTEST_ASSERT(&test, DQN_MEMCMP(buf, ZERO, DQN_ARRAY_UCOUNT(buf)) != 0);
        }

        DQN_UTEST_TEST("Generate secure RNG 0 bytes") {
            char buf[32] = {};
            buf[0] = 'Z';
            Dqn_b32 result = Dqn_OS_SecureRNGBytes(buf, 0);
            DQN_UTEST_ASSERT(&test, result);
            DQN_UTEST_ASSERT(&test, buf[0] == 'Z');
        }

        DQN_UTEST_TEST("Query executable directory") {
            Dqn_Scratch scratch = Dqn_Scratch_Get(nullptr);
            Dqn_Str8 result = Dqn_OS_EXEDir(scratch.arena);
            DQN_UTEST_ASSERT(&test, Dqn_Str8_HasData(result));
            DQN_UTEST_ASSERTF(&test, Dqn_OS_DirExists(result), "result(%zu): %.*s", result.size, DQN_STR_FMT(result));
        }

        DQN_UTEST_TEST("Dqn_OS_PerfCounterNow") {
            uint64_t result = Dqn_OS_PerfCounterNow();
            DQN_UTEST_ASSERT(&test, result != 0);
        }

        DQN_UTEST_TEST("Consecutive ticks are ordered") {
            uint64_t a = Dqn_OS_PerfCounterNow();
            uint64_t b = Dqn_OS_PerfCounterNow();
            DQN_UTEST_ASSERTF(&test, b >= a, "a: %llu, b: %llu", a, b);
        }

        DQN_UTEST_TEST("Ticks to time are a correct order of magnitude") {
            uint64_t a  = Dqn_OS_PerfCounterNow();
            uint64_t b  = Dqn_OS_PerfCounterNow();
            Dqn_f64  s  = Dqn_OS_PerfCounterS(a, b);
            Dqn_f64  ms = Dqn_OS_PerfCounterMs(a, b);
            Dqn_f64  us = Dqn_OS_PerfCounterUs(a, b);
            Dqn_f64  ns = Dqn_OS_PerfCounterNs(a, b);
            DQN_UTEST_ASSERTF(&test, s  <= ms, "s:  %f, ms: %f", s,  ms);
            DQN_UTEST_ASSERTF(&test, ms <= us, "ms: %f, us: %f", ms, us);
            DQN_UTEST_ASSERTF(&test, us <= ns, "us: %f, ns: %f", us, ns);
        }
    }

    return test;
}

static Dqn_UTest Dqn_Test_Rect()
{
    Dqn_UTest test = {};
    DQN_UTEST_GROUP(test, "Dqn_Rect") {
        DQN_UTEST_TEST("No intersection") {
            Dqn_Rect a  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx1(0),      Dqn_V2_InitNx2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2(200, 0), Dqn_V2_InitNx2(200, 200));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            Dqn_V2 ab_max = ab.pos + ab.size;
            DQN_UTEST_ASSERTF(&test,
                              ab.pos.x == 0 && ab.pos.y == 0 && ab_max.x == 0 && ab_max.y == 0,
                              "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                              ab.pos.x,
                              ab.pos.y,
                              ab_max.x,
                              ab_max.y);
        }

        DQN_UTEST_TEST("A's min intersects B") {
            Dqn_Rect a  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2(50, 50), Dqn_V2_InitNx2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2( 0,  0), Dqn_V2_InitNx2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            Dqn_V2 ab_max = ab.pos + ab.size;
            DQN_UTEST_ASSERTF(&test,
                            ab.pos.x == 50 && ab.pos.y == 50 && ab_max.x == 100 && ab_max.y == 100,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.pos.x,
                            ab.pos.y,
                            ab_max.x,
                            ab_max.y);
        }

        DQN_UTEST_TEST("B's min intersects A") {
            Dqn_Rect a  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2( 0,  0), Dqn_V2_InitNx2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2(50, 50), Dqn_V2_InitNx2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            Dqn_V2 ab_max = ab.pos + ab.size;
            DQN_UTEST_ASSERTF(&test,
                            ab.pos.x == 50 && ab.pos.y == 50 && ab_max.x == 100 && ab_max.y == 100,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.pos.x,
                            ab.pos.y,
                            ab_max.x,
                            ab_max.y);
        }

        DQN_UTEST_TEST("A's max intersects B") {
            Dqn_Rect a  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2(-50, -50), Dqn_V2_InitNx2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2(  0,   0), Dqn_V2_InitNx2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            Dqn_V2 ab_max = ab.pos + ab.size;
            DQN_UTEST_ASSERTF(&test,
                            ab.pos.x == 0 && ab.pos.y == 0 && ab_max.x == 50 && ab_max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.pos.x,
                            ab.pos.y,
                            ab_max.x,
                            ab_max.y);
        }

        DQN_UTEST_TEST("B's max intersects A") {
            Dqn_Rect a  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2(  0,   0), Dqn_V2_InitNx2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2(-50, -50), Dqn_V2_InitNx2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            Dqn_V2 ab_max = ab.pos + ab.size;
            DQN_UTEST_ASSERTF(&test,
                            ab.pos.x == 0 && ab.pos.y == 0 && ab_max.x == 50 && ab_max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.pos.x,
                            ab.pos.y,
                            ab_max.x,
                            ab_max.y);
        }


        DQN_UTEST_TEST("B contains A") {
            Dqn_Rect a  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2(25, 25), Dqn_V2_InitNx2( 25,  25));
            Dqn_Rect b  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2( 0,  0), Dqn_V2_InitNx2(100, 100));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            Dqn_V2 ab_max = ab.pos + ab.size;
            DQN_UTEST_ASSERTF(&test,
                            ab.pos.x == 25 && ab.pos.y == 25 && ab_max.x == 50 && ab_max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.pos.x,
                            ab.pos.y,
                            ab_max.x,
                            ab_max.y);
        }

        DQN_UTEST_TEST("A contains B") {
            Dqn_Rect a  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2( 0,  0), Dqn_V2_InitNx2(100, 100));
            Dqn_Rect b  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2(25, 25), Dqn_V2_InitNx2( 25,  25));
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            Dqn_V2 ab_max = ab.pos + ab.size;
            DQN_UTEST_ASSERTF(&test,
                            ab.pos.x == 25 && ab.pos.y == 25 && ab_max.x == 50 && ab_max.y == 50,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.pos.x,
                            ab.pos.y,
                            ab_max.x,
                            ab_max.y);
        }

        DQN_UTEST_TEST("A equals B") {
            Dqn_Rect a  = Dqn_Rect_InitV2x2(Dqn_V2_InitNx2(0, 0), Dqn_V2_InitNx2(100, 100));
            Dqn_Rect b  = a;
            Dqn_Rect ab = Dqn_Rect_Intersection(a, b);

            Dqn_V2 ab_max = ab.pos + ab.size;
            DQN_UTEST_ASSERTF(&test,
                            ab.pos.x == 0 && ab.pos.y == 0 && ab_max.x == 100 && ab_max.y == 100,
                            "ab = { min.x = %.2f, min.y = %.2f, max.x = %.2f. max.y = %.2f }",
                            ab.pos.x,
                            ab.pos.y,
                            ab_max.x,
                            ab_max.y);
        }
    }
    return test;
}

static Dqn_UTest Dqn_Test_Str8()
{
    Dqn_UTest test = {};
    DQN_UTEST_GROUP(test, "Dqn_Str8") {
        DQN_UTEST_TEST("Initialise with string literal w/ macro") {
            Dqn_Str8 string = DQN_STR8("AB");
            DQN_UTEST_ASSERTF(&test, string.size == 2,      "size: %zu",   string.size);
            DQN_UTEST_ASSERTF(&test, string.data[0] == 'A', "string[0]: %c", string.data[0]);
            DQN_UTEST_ASSERTF(&test, string.data[1] == 'B', "string[1]: %c", string.data[1]);
        }

        DQN_UTEST_TEST("Initialise with format string") {
            Dqn_Scratch scratch = Dqn_Scratch_Get(nullptr);
            Dqn_Str8    string  = Dqn_Str8_InitF(scratch.arena, "%s", "AB");
            DQN_UTEST_ASSERTF(&test, string.size == 2,      "size: %zu",   string.size);
            DQN_UTEST_ASSERTF(&test, string.data[0] == 'A', "string[0]: %c", string.data[0]);
            DQN_UTEST_ASSERTF(&test, string.data[1] == 'B', "string[1]: %c", string.data[1]);
            DQN_UTEST_ASSERTF(&test, string.data[2] == 0,   "string[2]: %c", string.data[2]);
        }

        DQN_UTEST_TEST("Copy string") {
            Dqn_Scratch scratch = Dqn_Scratch_Get(nullptr);
            Dqn_Str8 string        = DQN_STR8("AB");
            Dqn_Str8 copy          = Dqn_Str8_Copy(scratch.arena, string);
            DQN_UTEST_ASSERTF(&test, copy.size == 2,      "size: %zu", copy.size);
            DQN_UTEST_ASSERTF(&test, copy.data[0] == 'A', "copy[0]: %c", copy.data[0]);
            DQN_UTEST_ASSERTF(&test, copy.data[1] == 'B', "copy[1]: %c", copy.data[1]);
            DQN_UTEST_ASSERTF(&test, copy.data[2] ==  0,  "copy[2]: %c", copy.data[2]);
        }

        DQN_UTEST_TEST("Trim whitespace around string") {
            Dqn_Str8 string = Dqn_Str8_TrimWhitespaceAround(DQN_STR8(" AB "));
            DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(string, DQN_STR8("AB")), "[string=%.*s]", DQN_STR_FMT(string));
        }

        DQN_UTEST_TEST("Allocate string from arena") {
            Dqn_Scratch scratch = Dqn_Scratch_Get(nullptr);
            Dqn_Str8 string = Dqn_Str8_Alloc(scratch.arena, 2, Dqn_ZeroMem_No);
            DQN_UTEST_ASSERTF(&test, string.size == 2, "size: %zu", string.size);
        }

        // NOTE: Dqn_CStr8_Trim[Prefix/Suffix]
        // ---------------------------------------------------------------------------------------------
        DQN_UTEST_TEST("Trim prefix with matching prefix") {
            Dqn_Str8 input  = DQN_STR8("nft/abc");
            Dqn_Str8 result = Dqn_Str8_TrimPrefix(input, DQN_STR8("nft/"));
            DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(result, DQN_STR8("abc")), "%.*s", DQN_STR_FMT(result));
        }

        DQN_UTEST_TEST("Trim prefix with non matching prefix") {
            Dqn_Str8 input  = DQN_STR8("nft/abc");
            Dqn_Str8 result = Dqn_Str8_TrimPrefix(input, DQN_STR8(" ft/"));
            DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(result, input), "%.*s", DQN_STR_FMT(result));
        }

        DQN_UTEST_TEST("Trim suffix with matching suffix") {
            Dqn_Str8 input  = DQN_STR8("nft/abc");
            Dqn_Str8 result = Dqn_Str8_TrimSuffix(input, DQN_STR8("abc"));
            DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(result, DQN_STR8("nft/")), "%.*s", DQN_STR_FMT(result));
        }

        DQN_UTEST_TEST("Trim suffix with non matching suffix") {
            Dqn_Str8 input  = DQN_STR8("nft/abc");
            Dqn_Str8 result = Dqn_Str8_TrimSuffix(input, DQN_STR8("ab"));
            DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(result, input), "%.*s", DQN_STR_FMT(result));
        }

        // NOTE: Dqn_Str8_IsAllDigits //////////////////////////////////////////////////////////////
        DQN_UTEST_TEST("Is all digits fails on non-digit string") {
            Dqn_b32 result = Dqn_Str8_IsAll(DQN_STR8("@123string"), Dqn_Str8IsAll_Digits);
            DQN_UTEST_ASSERT(&test, result == false);
        }

        DQN_UTEST_TEST("Is all digits fails on nullptr") {
            Dqn_b32 result = Dqn_Str8_IsAll(Dqn_Str8_Init(nullptr, 0), Dqn_Str8IsAll_Digits);
            DQN_UTEST_ASSERT(&test, result == false);
        }

        DQN_UTEST_TEST("Is all digits fails on nullptr w/ size") {
            Dqn_b32 result = Dqn_Str8_IsAll(Dqn_Str8_Init(nullptr, 1), Dqn_Str8IsAll_Digits);
            DQN_UTEST_ASSERT(&test, result == false);
        }

        DQN_UTEST_TEST("Is all digits fails on string w/ 0 size") {
            char const buf[]  = "@123string";
            Dqn_b32    result = Dqn_Str8_IsAll(Dqn_Str8_Init(buf, 0), Dqn_Str8IsAll_Digits);
            DQN_UTEST_ASSERT(&test, !result);
        }

        DQN_UTEST_TEST("Is all digits success") {
            Dqn_b32 result = Dqn_Str8_IsAll(DQN_STR8("23"), Dqn_Str8IsAll_Digits);
            DQN_UTEST_ASSERT(&test, DQN_CAST(bool)result == true);
        }

        DQN_UTEST_TEST("Is all digits fails on whitespace") {
            Dqn_b32 result = Dqn_Str8_IsAll(DQN_STR8("23 "), Dqn_Str8IsAll_Digits);
            DQN_UTEST_ASSERT(&test, DQN_CAST(bool)result == false);
        }

        // NOTE: Dqn_Str8_BinarySplit
        // ---------------------------------------------------------------------------------------------
        {
            {
                char const *TEST_FMT  = "Binary split \"%.*s\" with \"%.*s\"";
                Dqn_Str8 delimiter = DQN_STR8("/");
                Dqn_Str8 input     = DQN_STR8("abcdef");
                DQN_UTEST_TEST(TEST_FMT, DQN_STR_FMT(input), DQN_STR_FMT(delimiter)) {
                    Dqn_Str8BinarySplitResult split = Dqn_Str8_BinarySplit(input, delimiter);
                    DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(split.lhs, DQN_STR8("abcdef")), "[lhs=%.*s]", DQN_STR_FMT(split.lhs));
                    DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(split.rhs, DQN_STR8("")),       "[rhs=%.*s]", DQN_STR_FMT(split.rhs));
                }

                input = DQN_STR8("abc/def");
                DQN_UTEST_TEST(TEST_FMT, DQN_STR_FMT(input), DQN_STR_FMT(delimiter)) {
                    Dqn_Str8BinarySplitResult split = Dqn_Str8_BinarySplit(input, delimiter);
                    DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(split.lhs, DQN_STR8("abc")), "[lhs=%.*s]", DQN_STR_FMT(split.lhs));
                    DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(split.rhs, DQN_STR8("def")), "[rhs=%.*s]", DQN_STR_FMT(split.rhs));
                }

                input = DQN_STR8("/abcdef");
                DQN_UTEST_TEST(TEST_FMT, DQN_STR_FMT(input), DQN_STR_FMT(delimiter)) {
                    Dqn_Str8BinarySplitResult split = Dqn_Str8_BinarySplit(input, delimiter);
                    DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(split.lhs, DQN_STR8("")),       "[lhs=%.*s]", DQN_STR_FMT(split.lhs));
                    DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(split.rhs, DQN_STR8("abcdef")), "[rhs=%.*s]", DQN_STR_FMT(split.rhs));
                }
            }

            {
                Dqn_Str8 delimiter = DQN_STR8("-=-");
                Dqn_Str8 input     = DQN_STR8("123-=-456");
                DQN_UTEST_TEST("Binary split \"%.*s\" with \"%.*s\"", DQN_STR_FMT(input), DQN_STR_FMT(delimiter)) {
                    Dqn_Str8BinarySplitResult split = Dqn_Str8_BinarySplit(input, delimiter);
                    DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(split.lhs, DQN_STR8("123")), "[lhs=%.*s]", DQN_STR_FMT(split.lhs));
                    DQN_UTEST_ASSERTF(&test, Dqn_Str8_Eq(split.rhs, DQN_STR8("456")), "[rhs=%.*s]", DQN_STR_FMT(split.rhs));
                }
            }
        }

        // NOTE: Dqn_Str8_ToI64 ////////////////////////////////////////////////////////////////////
        DQN_UTEST_TEST("To I64: Convert null string") {
            Dqn_Str8ToI64Result result = Dqn_Str8_ToI64(Dqn_Str8_Init(nullptr, 5), 0);
            DQN_UTEST_ASSERT(&test, result.success);
            DQN_UTEST_ASSERT(&test, result.value == 0);
        }

        DQN_UTEST_TEST("To I64: Convert empty string") {
            Dqn_Str8ToI64Result result = Dqn_Str8_ToI64(DQN_STR8(""), 0);
            DQN_UTEST_ASSERT(&test, result.success);
            DQN_UTEST_ASSERT(&test, result.value == 0);
        }

        DQN_UTEST_TEST("To I64: Convert \"1\"") {
            Dqn_Str8ToI64Result result = Dqn_Str8_ToI64(DQN_STR8("1"), 0);
            DQN_UTEST_ASSERT(&test, result.success);
            DQN_UTEST_ASSERT(&test, result.value == 1);
        }

        DQN_UTEST_TEST("To I64: Convert \"-0\"") {
            Dqn_Str8ToI64Result result = Dqn_Str8_ToI64(DQN_STR8("-0"), 0);
            DQN_UTEST_ASSERT(&test, result.success);
            DQN_UTEST_ASSERT(&test, result.value == 0);
        }

        DQN_UTEST_TEST("To I64: Convert \"-1\"") {
            Dqn_Str8ToI64Result result = Dqn_Str8_ToI64(DQN_STR8("-1"), 0);
            DQN_UTEST_ASSERT(&test, result.success);
            DQN_UTEST_ASSERT(&test, result.value == -1);
        }

        DQN_UTEST_TEST("To I64: Convert \"1.2\"") {
            Dqn_Str8ToI64Result result = Dqn_Str8_ToI64(DQN_STR8("1.2"), 0);
            DQN_UTEST_ASSERT(&test, !result.success);
            DQN_UTEST_ASSERT(&test, result.value == 1);
        }

        DQN_UTEST_TEST("To I64: Convert \"1,234\"") {
            Dqn_Str8ToI64Result result = Dqn_Str8_ToI64(DQN_STR8("1,234"), ',');
            DQN_UTEST_ASSERT(&test, result.success);
            DQN_UTEST_ASSERT(&test, result.value == 1234);
        }

        DQN_UTEST_TEST("To I64: Convert \"1,2\"") {
            Dqn_Str8ToI64Result result = Dqn_Str8_ToI64(DQN_STR8("1,2"), ',');
            DQN_UTEST_ASSERT(&test, result.success);
            DQN_UTEST_ASSERT(&test, result.value == 12);
        }

        DQN_UTEST_TEST("To I64: Convert \"12a3\"") {
            Dqn_Str8ToI64Result result = Dqn_Str8_ToI64(DQN_STR8("12a3"), 0);
            DQN_UTEST_ASSERT(&test, !result.success);
            DQN_UTEST_ASSERT(&test, result.value == 12);
        }

        // NOTE: Dqn_Str8_ToU64
        // ---------------------------------------------------------------------------------------------
        DQN_UTEST_TEST("To U64: Convert nullptr") {
            Dqn_Str8ToU64Result result = Dqn_Str8_ToU64(Dqn_Str8_Init(nullptr, 5), 0);
            DQN_UTEST_ASSERT(&test, result.success);
            DQN_UTEST_ASSERTF(&test, result.value == 0, "result: %llu", result.value);
        }

        DQN_UTEST_TEST("To U64: Convert empty string") {
            Dqn_Str8ToU64Result result = Dqn_Str8_ToU64(DQN_STR8(""), 0);
            DQN_UTEST_ASSERT(&test, result.success);
            DQN_UTEST_ASSERTF(&test, result.value == 0, "result: %llu", result.value);
        }

        DQN_UTEST_TEST("To U64: Convert \"1\"") {
            Dqn_Str8ToU64Result result = Dqn_Str8_ToU64(DQN_STR8("1"), 0);
            DQN_UTEST_ASSERT(&test, result.success);
            DQN_UTEST_ASSERTF(&test, result.value == 1, "result: %llu", result.value);
        }

        DQN_UTEST_TEST("To U64: Convert \"-0\"") {
            Dqn_Str8ToU64Result result = Dqn_Str8_ToU64(DQN_STR8("-0"), 0);
            DQN_UTEST_ASSERT(&test, !result.success);
            DQN_UTEST_ASSERTF(&test, result.value == 0, "result: %llu", result.value);
        }

        DQN_UTEST_TEST("To U64: Convert \"-1\"") {
            Dqn_Str8ToU64Result result = Dqn_Str8_ToU64(DQN_STR8("-1"), 0);
            DQN_UTEST_ASSERT(&test, !result.success);
            DQN_UTEST_ASSERTF(&test, result.value == 0, "result: %llu", result.value);
        }

        DQN_UTEST_TEST("To U64: Convert \"1.2\"") {
            Dqn_Str8ToU64Result result = Dqn_Str8_ToU64(DQN_STR8("1.2"), 0);
            DQN_UTEST_ASSERT(&test, !result.success);
            DQN_UTEST_ASSERTF(&test, result.value == 1, "result: %llu", result.value);
        }

        DQN_UTEST_TEST("To U64: Convert \"1,234\"") {
            Dqn_Str8ToU64Result result = Dqn_Str8_ToU64(DQN_STR8("1,234"), ',');
            DQN_UTEST_ASSERT(&test, result.success);
            DQN_UTEST_ASSERTF(&test, result.value == 1234, "result: %llu", result.value);
        }

        DQN_UTEST_TEST("To U64: Convert \"1,2\"") {
            Dqn_Str8ToU64Result result = Dqn_Str8_ToU64(DQN_STR8("1,2"), ',');
            DQN_UTEST_ASSERT(&test, result.success);
            DQN_UTEST_ASSERTF(&test, result.value == 12, "result: %llu", result.value);
        }

        DQN_UTEST_TEST("To U64: Convert \"12a3\"") {
            Dqn_Str8ToU64Result result = Dqn_Str8_ToU64(DQN_STR8("12a3"), 0);
            DQN_UTEST_ASSERT(&test, !result.success);
            DQN_UTEST_ASSERTF(&test, result.value == 12, "result: %llu", result.value);
        }

        // NOTE: Dqn_Str8_Find
        // =========================================================================================
        DQN_UTEST_TEST("Find: String (char) is not in buffer") {
            Dqn_Str8 buf              = DQN_STR8("836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55");
            Dqn_Str8 find             = DQN_STR8("2");
            Dqn_Str8FindResult result = Dqn_Str8_FindFirstString(buf, find);
            DQN_UTEST_ASSERT(&test, !result.found);
            DQN_UTEST_ASSERT(&test, result.index == 0);
            DQN_UTEST_ASSERT(&test, result.match.data == nullptr);
            DQN_UTEST_ASSERT(&test, result.match.size == 0);
        }

        DQN_UTEST_TEST("Find: String (char) is in buffer") {
            Dqn_Str8 buf              = DQN_STR8("836a35becd4e74b66a0d6844d51f1a63018c7ebc44cf7e109e8e4bba57eefb55");
            Dqn_Str8 find             = DQN_STR8("6");
            Dqn_Str8FindResult result = Dqn_Str8_FindFirstString(buf, find);
            DQN_UTEST_ASSERT(&test, result.found);
            DQN_UTEST_ASSERT(&test, result.index == 2);
            DQN_UTEST_ASSERT(&test, result.match.data[0] == '6');
        }

        // NOTE: Dqn_Str8_FileNameFromPath
        // =========================================================================================
        DQN_UTEST_TEST("File name from Windows path") {
            Dqn_Str8 buf    = DQN_STR8("C:\\ABC\\test.exe");
            Dqn_Str8 result = Dqn_Str8_FileNameFromPath(buf);
            DQN_UTEST_ASSERTF(&test, result == DQN_STR8("test.exe"), "%.*s", DQN_STR_FMT(result));
        }

        DQN_UTEST_TEST("File name from Linux path") {
            Dqn_Str8 buf    = DQN_STR8("/ABC/test.exe");
            Dqn_Str8 result = Dqn_Str8_FileNameFromPath(buf);
            DQN_UTEST_ASSERTF(&test, result == DQN_STR8("test.exe"), "%.*s", DQN_STR_FMT(result));
        }

        // NOTE: Dqn_Str8_TrimPrefix
        // =========================================================================================
        DQN_UTEST_TEST("Trim prefix") {
            Dqn_Str8 prefix = DQN_STR8("@123");
            Dqn_Str8 buf    = DQN_STR8("@123string");
            Dqn_Str8 result = Dqn_Str8_TrimPrefix(buf, prefix, Dqn_Str8EqCase_Sensitive);
            DQN_UTEST_ASSERT(&test, result == DQN_STR8("string"));
        }
    }
    return test;
}

static Dqn_UTest Dqn_Test_TicketMutex()
{
    Dqn_UTest test = {};
    DQN_UTEST_GROUP(test, "Dqn_TicketMutex") {
        DQN_UTEST_TEST("Ticket mutex start and stop") {
            // TODO: We don't have a meaningful test but since atomics are
            // implemented with a macro this ensures that we test that they are
            // written correctly.
            Dqn_TicketMutex mutex = {};
            Dqn_TicketMutex_Begin(&mutex);
            Dqn_TicketMutex_End(&mutex);
            DQN_UTEST_ASSERT(&test, mutex.ticket == mutex.serving);
        }

        DQN_UTEST_TEST("Ticket mutex start and stop w/ advanced API") {
            Dqn_TicketMutex mutex = {};
            unsigned int ticket_a = Dqn_TicketMutex_MakeTicket(&mutex);
            unsigned int ticket_b = Dqn_TicketMutex_MakeTicket(&mutex);
            DQN_UTEST_ASSERT(&test, DQN_CAST(bool)Dqn_TicketMutex_CanLock(&mutex, ticket_b) == false);
            DQN_UTEST_ASSERT(&test, DQN_CAST(bool)Dqn_TicketMutex_CanLock(&mutex, ticket_a) == true);

            Dqn_TicketMutex_BeginTicket(&mutex, ticket_a);
            Dqn_TicketMutex_End(&mutex);
            Dqn_TicketMutex_BeginTicket(&mutex, ticket_b);
            Dqn_TicketMutex_End(&mutex);

            DQN_UTEST_ASSERT(&test, mutex.ticket == mutex.serving);
            DQN_UTEST_ASSERT(&test, mutex.ticket == ticket_b + 1);
        }
    }
    return test;
}

static Dqn_UTest Dqn_Test_VArray()
{
    Dqn_UTest test            = {};
    DQN_UTEST_GROUP(test, "Dqn_VArray") {
        {
            Dqn_VArray<uint32_t> array = Dqn_VArray_InitByteSize<uint32_t>(DQN_KILOBYTES(64), 0);
            DQN_DEFER {
                Dqn_VArray_Deinit(&array);
            };

            DQN_UTEST_TEST("Test adding an array of items to the array") {
                uint32_t array_literal[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
                Dqn_VArray_AddArray<uint32_t>(&array, array_literal, DQN_ARRAY_UCOUNT(array_literal));
                DQN_UTEST_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
                DQN_UTEST_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
            }

            DQN_UTEST_TEST("Test stable erase, 1 item, the '2' value from the array") {
                Dqn_VArray_EraseRange(&array, 2 /*begin_index*/, 1 /*count*/, Dqn_ArrayErase_Stable);
                uint32_t array_literal[] = {0, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
                DQN_UTEST_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
                DQN_UTEST_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
            }

            DQN_UTEST_TEST("Test unstable erase, 1 item, the '1' value from the array") {
                Dqn_VArray_EraseRange(&array, 1 /*begin_index*/, 1 /*count*/, Dqn_ArrayErase_Unstable);
                uint32_t array_literal[] = {0, 15, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
                DQN_UTEST_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
                DQN_UTEST_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
            }

            Dqn_ArrayErase erase_enums[] = {Dqn_ArrayErase_Stable, Dqn_ArrayErase_Unstable};
            DQN_UTEST_TEST("Test un/stable erase, OOB") {
                for (Dqn_ArrayErase erase : erase_enums) {
                    uint32_t array_literal[] = {0, 15, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
                    Dqn_VArray_EraseRange(&array, DQN_ARRAY_UCOUNT(array_literal) /*begin_index*/, DQN_ARRAY_UCOUNT(array_literal) + 100 /*count*/, erase);
                    DQN_UTEST_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
                    DQN_UTEST_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
                }
            }

            DQN_UTEST_TEST("Test flipped begin/end index stable erase, 2 items, the '15, 3' value from the array") {
                Dqn_VArray_EraseRange(&array, 2 /*begin_index*/, -2 /*count*/, Dqn_ArrayErase_Stable);
                uint32_t array_literal[] = {0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
                DQN_UTEST_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
                DQN_UTEST_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
            }

            DQN_UTEST_TEST("Test flipped begin/end index unstable erase, 2 items, the '4, 5' value from the array") {
                Dqn_VArray_EraseRange(&array, 2 /*begin_index*/, -2 /*count*/, Dqn_ArrayErase_Unstable);
                uint32_t array_literal[] = {0, 13, 14, 6, 7, 8, 9, 10, 11, 12};
                DQN_UTEST_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
                DQN_UTEST_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
            }

            DQN_UTEST_TEST("Test stable erase range, 2+1 (oob) item, the '13, 14, +1 OOB' value from the array") {
                Dqn_VArray_EraseRange(&array, 8 /*begin_index*/, 3 /*count*/, Dqn_ArrayErase_Stable);
                uint32_t array_literal[] = {0, 13, 14, 6, 7, 8, 9, 10};
                DQN_UTEST_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
                DQN_UTEST_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
            }

            DQN_UTEST_TEST("Test unstable erase range, 3+1 (oob) item, the '11, 12, +1 OOB' value from the array") {
                Dqn_VArray_EraseRange(&array, 6 /*begin_index*/, 3 /*count*/, Dqn_ArrayErase_Unstable);
                uint32_t array_literal[] = {0, 13, 14, 6, 7, 8};
                DQN_UTEST_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
                DQN_UTEST_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
            }

            DQN_UTEST_TEST("Test stable erase -overflow OOB, erasing the '0, 13' value from the array") {
                Dqn_VArray_EraseRange(&array, 1 /*begin_index*/, -DQN_ISIZE_MAX /*count*/, Dqn_ArrayErase_Stable);
                uint32_t array_literal[] = {14, 6, 7, 8};
                DQN_UTEST_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
                DQN_UTEST_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
            }

            DQN_UTEST_TEST("Test unstable erase +overflow OOB, erasing the '7, 8' value from the array") {
                Dqn_VArray_EraseRange(&array, 2 /*begin_index*/, DQN_ISIZE_MAX /*count*/, Dqn_ArrayErase_Unstable);
                uint32_t array_literal[] = {14, 6};
                DQN_UTEST_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(array_literal));
                DQN_UTEST_ASSERT(&test, DQN_MEMCMP(array.data, array_literal, DQN_ARRAY_UCOUNT(array_literal) * sizeof(array_literal[0])) == 0);
            }

            DQN_UTEST_TEST("Test adding an array of items after erase") {
                uint32_t array_literal[] = {0, 1, 2, 3};
                Dqn_VArray_AddArray<uint32_t>(&array, array_literal, DQN_ARRAY_UCOUNT(array_literal));

                uint32_t expected_literal[] = {14, 6, 0, 1, 2, 3};
                DQN_UTEST_ASSERT(&test, array.size == DQN_ARRAY_UCOUNT(expected_literal));
                DQN_UTEST_ASSERT(&test, DQN_MEMCMP(array.data, expected_literal, DQN_ARRAY_UCOUNT(expected_literal) * sizeof(expected_literal[0])) == 0);
            }
        }

        DQN_UTEST_TEST("Array of unaligned objects are contiguously laid out in memory") {
            // NOTE: Since we allocate from a virtual memory block, each time
            // we request memory from the block we can demand some alignment
            // on the returned pointer from the memory block. If there's
            // additional alignment done in that function then we can no
            // longer access the items in the array contiguously leading to
            // confusing memory "corruption" errors.
            //
            // This test makes sure that the unaligned objects are allocated
            // from the memory block (and hence the array) contiguously
            // when the size of the object is not aligned with the required 
            // alignment of the object.
            DQN_MSVC_WARNING_PUSH
            DQN_MSVC_WARNING_DISABLE(4324) // warning C4324: 'TestVArray::UnalignedObject': structure was padded due to alignment specifier
            struct alignas(8) UnalignedObject {
                char data[511];
            };
            DQN_MSVC_WARNING_POP

            Dqn_VArray<UnalignedObject> array = Dqn_VArray_InitByteSize<UnalignedObject>(DQN_KILOBYTES(64), 0);
            DQN_DEFER {
                Dqn_VArray_Deinit(&array);
            };

            // NOTE: Verify that the items returned from the data array are 
            // contiguous in memory.
            UnalignedObject *make_item_a = Dqn_VArray_MakeArray(&array, 1, Dqn_ZeroMem_Yes);
            UnalignedObject *make_item_b = Dqn_VArray_MakeArray(&array, 1, Dqn_ZeroMem_Yes);
            DQN_MEMSET(make_item_a->data, 'a', sizeof(make_item_a->data));
            DQN_MEMSET(make_item_b->data, 'b', sizeof(make_item_b->data));
            DQN_UTEST_ASSERT(&test, (uintptr_t)make_item_b == (uintptr_t)(make_item_a + 1));

            // NOTE: Verify that accessing the items from the data array yield
            // the same object.
            DQN_UTEST_ASSERT(&test, array.size == 2);
            UnalignedObject *data_item_a = array.data + 0;
            UnalignedObject *data_item_b = array.data + 1;
            DQN_UTEST_ASSERT(&test, (uintptr_t)data_item_b == (uintptr_t)(data_item_a + 1));
            DQN_UTEST_ASSERT(&test, (uintptr_t)data_item_b == (uintptr_t)(make_item_a + 1));
            DQN_UTEST_ASSERT(&test, (uintptr_t)data_item_b == (uintptr_t)make_item_b);

            for (Dqn_usize i = 0; i < sizeof(data_item_a->data); i++) {
                DQN_UTEST_ASSERT(&test, data_item_a->data[i] == 'a');
            }

            for (Dqn_usize i = 0; i < sizeof(data_item_b->data); i++) {
                DQN_UTEST_ASSERT(&test, data_item_b->data[i] == 'b');
            }
        }
    }
    return test;
}

#if defined(DQN_PLATFORM_WIN32)
static Dqn_UTest Dqn_Test_Win()
{
    Dqn_UTest test = {};
    DQN_UTEST_GROUP(test, "OS Win32") {
        Dqn_Scratch scratch = Dqn_Scratch_Get(nullptr);
        Dqn_Str8    input8  = DQN_STR8("String");
        Dqn_Str16   input16 = Dqn_Str16{(wchar_t *)(L"String"), sizeof(L"String") / sizeof(L"String"[0]) - 1};

        DQN_UTEST_TEST("Str8 to Str16") {
            Dqn_Str16 result = Dqn_Win_Str8ToStr16(scratch.arena, input8);
            DQN_UTEST_ASSERT(&test, result == input16);
        }

        DQN_UTEST_TEST("Str16 to Str8") {
            Dqn_Str8 result = Dqn_Win_Str16ToStr8(scratch.arena, input16);
            DQN_UTEST_ASSERT(&test, result == input8);
        }

        DQN_UTEST_TEST("Str16 to Str8: Null terminates string") {
            int   size_required = Dqn_Win_Str16ToStr8Buffer(input16, nullptr, 0);
            char *string        = Dqn_Arena_NewArray(scratch.arena, char, size_required + 1, Dqn_ZeroMem_No);

            // Fill the string with error sentinels
            DQN_MEMSET(string, 'Z', size_required + 1);

            int size_returned = Dqn_Win_Str16ToStr8Buffer(input16, string, size_required + 1);
            char const EXPECTED[] = {'S', 't', 'r', 'i', 'n', 'g', 0};

            DQN_UTEST_ASSERTF(&test, size_required == size_returned, "string_size: %d, result: %d", size_required, size_returned);
            DQN_UTEST_ASSERTF(&test, size_returned == DQN_ARRAY_UCOUNT(EXPECTED) - 1, "string_size: %d, expected: %zu", size_returned, DQN_ARRAY_UCOUNT(EXPECTED) - 1);
            DQN_UTEST_ASSERT(&test, DQN_MEMCMP(EXPECTED, string, sizeof(EXPECTED)) == 0);
        }

        DQN_UTEST_TEST("Str16 to Str8: Arena null terminates string") {
            Dqn_Str8   string8       = Dqn_Win_Str16ToStr8(scratch.arena, input16);
            int        size_returned = Dqn_Win_Str16ToStr8Buffer(input16, nullptr, 0);
            char const EXPECTED[]    = {'S', 't', 'r', 'i', 'n', 'g', 0};

            DQN_UTEST_ASSERTF(&test, DQN_CAST(int)string8.size == size_returned, "string_size: %d, result: %d", DQN_CAST(int)string8.size, size_returned);
            DQN_UTEST_ASSERTF(&test, DQN_CAST(int)string8.size == DQN_ARRAY_UCOUNT(EXPECTED) - 1, "string_size: %d, expected: %zu", DQN_CAST(int)string8.size, DQN_ARRAY_UCOUNT(EXPECTED) - 1);
            DQN_UTEST_ASSERT (&test, DQN_MEMCMP(EXPECTED, string8.data, sizeof(EXPECTED)) == 0);
        }
    }
    return test;
}
#endif // DQN_PLATFORM_WIN#@

void Dqn_Test_RunSuite()
{
    Dqn_UTest tests[] =
    {
        Dqn_Test_Arena(),
        Dqn_Test_Bin(),
        Dqn_Test_BinarySearch(),
        Dqn_Test_DSMap(),
        Dqn_Test_FStr8(),
        Dqn_Test_Fs(),
        Dqn_Test_FixedArray(),
        Dqn_Test_Intrinsics(),
        #if defined(DQN_TEST_WITH_KECCAK)
        Dqn_Test_Keccak(),
        #endif
        Dqn_Test_M4(),
        Dqn_Test_OS(),
        Dqn_Test_Rect(),
        Dqn_Test_Str8(),
        Dqn_Test_TicketMutex(),
        Dqn_Test_VArray(),
        #if defined(DQN_PLATFORM_WIN32)
        Dqn_Test_Win(),
        #endif
    };

    int total_tests = 0;
    int total_good_tests = 0;
    for (const Dqn_UTest &test : tests) {
        total_tests += test.num_tests_in_group;
        total_good_tests += test.num_tests_ok_in_group;
    }

    fprintf(stdout, "Summary: %d/%d tests succeeded\n", total_good_tests, total_tests);
}

#if defined(DQN_TEST_WITH_MAIN)
int main(int argc, char *argv[])
{
    (void)argv; (void)argc;
    Dqn_Library_Init(Dqn_LibraryOnInit_LogFeatures);
    Dqn_Test_RunSuite();
    return 0;
}
#endif
