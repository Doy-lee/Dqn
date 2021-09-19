#if !defined(DQN_KECCAK_H)
#define DQN_KECCAK_H
// -----------------------------------------------------------------------------
// NOTE: Overview
// -----------------------------------------------------------------------------
// Implementation of the Keccak hashing algorithms from the Keccak and SHA3
// families (including the FIPS202 published algorithms and the non-finalized
// ones, i.e. the ones used in Ethereum and Monero which adopted SHA3 before it
// was finalized. The only difference between the 2 is a different delimited
// suffix).
//
// -----------------------------------------------------------------------------
// NOTE: MIT License
// -----------------------------------------------------------------------------
// Copyright (c) 2021 github.com/doy-lee
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// -----------------------------------------------------------------------------
// NOTE: Macros
// -----------------------------------------------------------------------------
// #define DQN_KECCAK_IMPLEMENTATION
//     Define this in one and only one C++ file to enable the implementation
//     code of the header file.
//
// #define DQN_KECCAK_MEMCOPY
//     Define this macro to override the memcpy implementation and avoid pulling
//     in string.h.
//
// #define DQN_KECCAK_MEMCMP
//     Define this macro to override the memcmp implementation and avoid pulling
//     in string.h.
//
// #define DQN_NO_MALLOC_FUNCTIONS
//     Define this macro to disable the non-essential helper functions that use
//     malloc.
//
// #define DQN_KECCAK_MALLOC
//     Define this macro to override the malloc implementation. This library
//     provides helper functions that use malloc if DQN_NO_MALLOC_FUNCTIONS is
//     not defined.
//
// #define DQN_KECCAK_ASSERT
//     Define this macro to override the assert implementation.

#if !defined(DQN_KECCAK_MEMCOPY)
    #include <string.h>
    #define DQN_KECCAK_MEMCOPY(dest, src, count) memcpy(dest, src, count)
#endif

#if !defined(DQN_KECCAK_MEMCMP)
    #include <string.h>
    #define DQN_KECCAK_MEMCMP(dest, src, count) memcmp(dest, src, count)
#endif

#if !defined(DQN_NO_MALLOC_FUNCTIONS)
    #if !defined(DQN_KECCAK_MALLOC)
        #include <stdlib.h>
        #define DQN_KECCAK_MALLOC(size) malloc(size)
    #endif
#endif

#if !defined(DQN_KECCAK_ASSERT)
    #if defined(NDEBUG)
        #define DQN_KECCAK_ASSERT(expr)
    #else
        #define DQN_KECCAK_ASSERT(expr)                                                            \
            do                                                                                     \
            {                                                                                      \
                if (!(expr))                                                                       \
                {                                                                                  \
                    (*(volatile int *)0) = 0;                                                      \
                }                                                                                  \
            } while (0)
    #endif
#endif

// Use this macro in a printf-like function,
/*
    Dqn_KeccakString64 string = {};
    printf("%.*s\n", DQN_KECCAK_STRING64_FMT(string));
*/
#define DQN_KECCAK_STRING56_FMT(string) 56, string
#define DQN_KECCAK_STRING64_FMT(string) 64, string
#define DQN_KECCAK_STRING96_FMT(string) 96, string
#define DQN_KECCAK_STRING128_FMT(string) 128, string

// -----------------------------------------------------------------------------
// NOTE: Data structures
// -----------------------------------------------------------------------------
struct Dqn_KeccakBytes28  { char data[28]; }; // 224 bit
struct Dqn_KeccakBytes32  { char data[32]; }; // 256 bit
struct Dqn_KeccakBytes48  { char data[48]; }; // 384 bit
struct Dqn_KeccakBytes64  { char data[64]; }; // 512 bit

struct Dqn_KeccakString56  { char str[(sizeof(Dqn_KeccakBytes28) * 2) + 1]; };
struct Dqn_KeccakString64  { char str[(sizeof(Dqn_KeccakBytes32) * 2) + 1]; };
struct Dqn_KeccakString96  { char str[(sizeof(Dqn_KeccakBytes48) * 2) + 1]; };
struct Dqn_KeccakString128 { char str[(sizeof(Dqn_KeccakBytes64) * 2) + 1]; };

// -----------------------------------------------------------------------------
// NOTE: API
// -----------------------------------------------------------------------------
// TODO(dqn): Write a streaming API
typedef unsigned char  Dqn_Keccak_u8;
typedef unsigned short Dqn_Keccak_u16;
typedef unsigned int   Dqn_Keccak_u32;
typedef unsigned int   Dqn_Keccak_uint;
#ifdef _MSC_VER
    typedef unsigned __int64 Dqn_Keccak_u64;
#else
    typedef unsigned long long Dqn_Keccak_u64;
#endif

// -----------------------------------------------------------------------------
// NOTE: SHA3
// -----------------------------------------------------------------------------
// Applies the FIPS 202 SHA3 algorithm on the supplied buffer. The size of
// the returned hash is (bit-rate/8) bytes, i.e. the dest_size must be at least
// 32 bytes for SHA-256 and so forth.

// dest_size: The passed in destination buffer must be >= 28 (bytes), otherwise
// the function asserts or does no-op in release.
void              Dqn_Keccak_SHA3_224(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size);
Dqn_KeccakBytes28 Dqn_Keccak_SHA3_224_ToBytes28(void *bytes, Dqn_Keccak_u64 bytes_size);

// dest_size: The passed in destination buffer must be >= 32 (bytes), otherwise
// the function asserts or does no-op in release.
void              Dqn_Keccak_SHA3_256(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size);
Dqn_KeccakBytes32 Dqn_Keccak_SHA3_256_ToBytes32(void *bytes, Dqn_Keccak_u64 bytes_size);

// dest_size: The passed in destination buffer must be >= 48 (bytes), otherwise
// the function asserts or does no-op in release.
void              Dqn_Keccak_SHA3_384(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size);
Dqn_KeccakBytes48 Dqn_Keccak_SHA3_384_ToBytes48(void *bytes, Dqn_Keccak_u64 bytes_size);

// dest_size: The passed in destination buffer must be >= 64 (bytes), otherwise
// the function asserts or does no-op in release.
void              Dqn_Keccak_SHA3_512(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size);
Dqn_KeccakBytes64 Dqn_Keccak_SHA3_512_ToBytes64(void *bytes, Dqn_Keccak_u64 bytes_size);

#if defined(DQN_H)
// -----------------------------------------------------------------------------
// NOTE: SHA3 - Helpers for Dqn data structures
// -----------------------------------------------------------------------------
Dqn_KeccakBytes28 Dqn_Keccak_SHA3_224_StringToBytes28(Dqn_String string);
Dqn_KeccakBytes32 Dqn_Keccak_SHA3_256_StringToBytes32(Dqn_String string);
Dqn_KeccakBytes48 Dqn_Keccak_SHA3_384_StringToBytes48(Dqn_String string);
Dqn_KeccakBytes64 Dqn_Keccak_SHA3_512_StringToBytes64(Dqn_String string);

Dqn_KeccakBytes28 Dqn_Keccak_SHA3_224_U8ArrayToBytes28(Dqn_Array<Dqn_Keccak_u8> array);
Dqn_KeccakBytes32 Dqn_Keccak_SHA3_256_U8ArrayToBytes32(Dqn_Array<Dqn_Keccak_u8> array);
Dqn_KeccakBytes48 Dqn_Keccak_SHA3_384_U8ArrayToBytes48(Dqn_Array<Dqn_Keccak_u8> array);
Dqn_KeccakBytes64 Dqn_Keccak_SHA3_512_U8ArrayToBytes64(Dqn_Array<Dqn_Keccak_u8> array);
#endif // DQN_H

// -----------------------------------------------------------------------------
// NOTE: Keccak
// -----------------------------------------------------------------------------
// Applies the non-finalized SHA3 algorithm (i.e. a delimited suffix of 0x1
// instead of 0x6 in SHA3). This is the version of the algorithm used by
// Ethereum and Monero as they adopted SHA3 before it was finalized.

// dest_size: The passed in destination buffer must be >= 28 (bytes), otherwise
// the function asserts or does no-op in release.
void              Dqn_Keccak_224(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size);
Dqn_KeccakBytes28 Dqn_Keccak_224_ToBytes28(void *bytes, Dqn_Keccak_u64 bytes_size);

// dest_size: The passed in destination buffer must be >= 32 (bytes), otherwise
// the function asserts or does no-op in release.
void              Dqn_Keccak_256(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size);
Dqn_KeccakBytes32 Dqn_Keccak_256_ToBytes32(void *bytes, Dqn_Keccak_u64 bytes_size);

// dest_size: The passed in destination buffer must be >= 48 (bytes), otherwise
// the function asserts or does no-op in release.
void              Dqn_Keccak_384(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size);
Dqn_KeccakBytes48 Dqn_Keccak_384_ToBytes48(void *bytes, Dqn_Keccak_u64 bytes_size);

// dest_size: The passed in destination buffer must be >= 64 (bytes), otherwise
// the function asserts or does no-op in release.
void              Dqn_Keccak_512(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size);
Dqn_KeccakBytes64 Dqn_Keccak_512_ToBytes64(void *bytes, Dqn_Keccak_u64 bytes_size);

#if defined(DQN_H)
// -----------------------------------------------------------------------------
// NOTE: Keccak - Helpers for Dqn data structures
// -----------------------------------------------------------------------------
Dqn_KeccakBytes28 Dqn_Keccak_224_StringToBytes28(Dqn_String string);
Dqn_KeccakBytes32 Dqn_Keccak_256_StringToBytes32(Dqn_String string);
Dqn_KeccakBytes48 Dqn_Keccak_384_StringToBytes48(Dqn_String string);
Dqn_KeccakBytes64 Dqn_Keccak_512_StringToBytes64(Dqn_String string);

Dqn_KeccakBytes28 Dqn_Keccak_224_U8ArrayToBytes28(Dqn_Array<Dqn_Keccak_u8> array);
Dqn_KeccakBytes32 Dqn_Keccak_256_U8ArrayToBytes32(Dqn_Array<Dqn_Keccak_u8> array);
Dqn_KeccakBytes48 Dqn_Keccak_384_U8ArrayToBytes48(Dqn_Array<Dqn_Keccak_u8> array);
Dqn_KeccakBytes64 Dqn_Keccak_512_U8ArrayToBytes64(Dqn_Array<Dqn_Keccak_u8> array);
#endif // DQN_H

// -----------------------------------------------------------------------------
// NOTE: Helper functions
// -----------------------------------------------------------------------------
// Convert a binary buffer into its hex representation into dest. The dest
// buffer must be large enough to contain the hex representation, i.e.
// at least src_size * 2). This function does *not* null-terminate the buffer.
// The returned result does *not* include a leading 0x prefix.
void Dqn_Keccak_BytesToHex(void const *src, Dqn_Keccak_u64 src_size, char *dest, Dqn_Keccak_u64 dest_size);

#if defined(DQN_NO_MALLOC_FUNCTIONS)
// Convert the src bytes into a null-terminated c-string using malloc. Calls
// into Dqn_Keccak_BytesToHex under the hood.
// src: If src is nullptr, the function returns an empty null-terminated
// string, otherwise, the bytes will be converted and returned as hex.
// return: A null-terminated c-string. This string must be freed by the user
// using the CRT free(..). If malloc fails the returned string is nullptr.
char *Dqn_Keccak_BytesToHexCString(void const *src, Dqn_Keccak_u64 src_size);
#endif // DQN_NO_MALLOC_FUNCTIONS

// Converts a fixed amount of bytes into a hexadecimal string. Helper functions
// that call into Dqn_Keccak_BytesToHex.
// return: The hexadecimal string of the bytes, null-terminated.
Dqn_KeccakString56  Dqn_Keccak_Bytes28ToHex(Dqn_KeccakBytes28 const *bytes);
Dqn_KeccakString64  Dqn_Keccak_Bytes32ToHex(Dqn_KeccakBytes32 const *bytes);
Dqn_KeccakString96  Dqn_Keccak_Bytes48ToHex(Dqn_KeccakBytes48 const *bytes);
Dqn_KeccakString128 Dqn_Keccak_Bytes64ToHex(Dqn_KeccakBytes64 const *bytes);

// Compares byte data structures for byte equality (via memcmp).
// return: 1 if the contents are equal otherwise 0.
int Dqn_Keccak_Bytes28Equals(Dqn_KeccakBytes28 const *a, Dqn_KeccakBytes28 const *b);
int Dqn_Keccak_Bytes32Equals(Dqn_KeccakBytes32 const *a, Dqn_KeccakBytes32 const *b);
int Dqn_Keccak_Bytes48Equals(Dqn_KeccakBytes48 const *a, Dqn_KeccakBytes48 const *b);
int Dqn_Keccak_Bytes64Equals(Dqn_KeccakBytes64 const *a, Dqn_KeccakBytes64 const *b);

#if defined(DQN_H)
// -----------------------------------------------------------------------------
// NOTE: Other helper functions for Dqn data structures
// -----------------------------------------------------------------------------
// Converts a 64 character hex string into the 32 byte binary representation.
// Invalid hex characters in the string will be represented as 0.
// hex: Must be exactly a 64 character hex string.
Dqn_KeccakBytes32 Dqn_Keccak_Hex64StringToBytes(Dqn_String hex);
#endif // DQN_H
#endif // DQN_KECCAK_H

#if defined(DQN_KECCAK_IMPLEMENTATION)
Dqn_Keccak_u64 const DQN_KECCAK_ROUNDS[] = {
    0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000, 0x000000000000808B,
    0x0000000080000001, 0x8000000080008081, 0x8000000000008009, 0x000000000000008A, 0x0000000000000088,
    0x0000000080008009, 0x000000008000000A, 0x000000008000808B, 0x800000000000008B, 0x8000000000008089,
    0x8000000000008003, 0x8000000000008002, 0x8000000000000080, 0x000000000000800A, 0x800000008000000A,
    0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008,
};

Dqn_Keccak_u64 const DQN_KECCAK_ROTATIONS[][5] =
{
    {0, 36, 3, 41, 18},
    {1, 44, 10, 45, 2},
    {62, 6, 43, 15, 61},
    {28, 55, 25, 21, 56},
    {27, 20, 39, 8, 14},
};

#define DQN_KECCAK_LANE_SIZE_U64 5
#define DQN_KECCAK_ROL64(val, rotate) (((val) << (rotate)) | (((val) >> (64 - (rotate)))))

static void Dqn_Keccak__ReferencePermute(Dqn_Keccak_u64 *A)
{
    // TODO(dqn): Do some more tests and remove.
    // NOTE: Reference permutation taken for cross-reference and back verifying
    // against from digestpp by kerukuro
    // https://github.com/kerukuro/digestpp/blob/master/algorithm/detail/sha3_provider.hpp

    for (int round = 0; round < 24; round++)
    {
        Dqn_Keccak_u64 C[5], D[5];
        C[0] = A[0 * 5 + 0] ^ A[1 * 5 + 0] ^ A[2 * 5 + 0] ^ A[3 * 5 + 0] ^ A[4 * 5 + 0];
        C[1] = A[0 * 5 + 1] ^ A[1 * 5 + 1] ^ A[2 * 5 + 1] ^ A[3 * 5 + 1] ^ A[4 * 5 + 1];
        C[2] = A[0 * 5 + 2] ^ A[1 * 5 + 2] ^ A[2 * 5 + 2] ^ A[3 * 5 + 2] ^ A[4 * 5 + 2];
        C[3] = A[0 * 5 + 3] ^ A[1 * 5 + 3] ^ A[2 * 5 + 3] ^ A[3 * 5 + 3] ^ A[4 * 5 + 3];
        C[4] = A[0 * 5 + 4] ^ A[1 * 5 + 4] ^ A[2 * 5 + 4] ^ A[3 * 5 + 4] ^ A[4 * 5 + 4];

        D[0] = C[4] ^ DQN_KECCAK_ROL64(C[1], 1);
        D[1] = C[0] ^ DQN_KECCAK_ROL64(C[2], 1);
        D[2] = C[1] ^ DQN_KECCAK_ROL64(C[3], 1);
        D[3] = C[2] ^ DQN_KECCAK_ROL64(C[4], 1);
        D[4] = C[3] ^ DQN_KECCAK_ROL64(C[0], 1);

        Dqn_Keccak_u64 B00 = A[0 * 5 + 0] ^ D[0];
        Dqn_Keccak_u64 B10 = DQN_KECCAK_ROL64(A[0 * 5 + 1] ^ D[1], 1);
        Dqn_Keccak_u64 B20 = DQN_KECCAK_ROL64(A[0 * 5 + 2] ^ D[2], 62);
        Dqn_Keccak_u64 B5  = DQN_KECCAK_ROL64(A[0 * 5 + 3] ^ D[3], 28);
        Dqn_Keccak_u64 B15 = DQN_KECCAK_ROL64(A[0 * 5 + 4] ^ D[4], 27);

        Dqn_Keccak_u64 B16 = DQN_KECCAK_ROL64(A[1 * 5 + 0] ^ D[0], 36);
        Dqn_Keccak_u64 B1  = DQN_KECCAK_ROL64(A[1 * 5 + 1] ^ D[1], 44);
        Dqn_Keccak_u64 B11 = DQN_KECCAK_ROL64(A[1 * 5 + 2] ^ D[2], 6);
        Dqn_Keccak_u64 B21 = DQN_KECCAK_ROL64(A[1 * 5 + 3] ^ D[3], 55);
        Dqn_Keccak_u64 B6  = DQN_KECCAK_ROL64(A[1 * 5 + 4] ^ D[4], 20);

        Dqn_Keccak_u64 B7  = DQN_KECCAK_ROL64(A[2 * 5 + 0] ^ D[0], 3);
        Dqn_Keccak_u64 B17 = DQN_KECCAK_ROL64(A[2 * 5 + 1] ^ D[1], 10);
        Dqn_Keccak_u64 B2  = DQN_KECCAK_ROL64(A[2 * 5 + 2] ^ D[2], 43);
        Dqn_Keccak_u64 B12 = DQN_KECCAK_ROL64(A[2 * 5 + 3] ^ D[3], 25);
        Dqn_Keccak_u64 B22 = DQN_KECCAK_ROL64(A[2 * 5 + 4] ^ D[4], 39);

        Dqn_Keccak_u64 B23 = DQN_KECCAK_ROL64(A[3 * 5 + 0] ^ D[0], 41);
        Dqn_Keccak_u64 B8  = DQN_KECCAK_ROL64(A[3 * 5 + 1] ^ D[1], 45);
        Dqn_Keccak_u64 B18 = DQN_KECCAK_ROL64(A[3 * 5 + 2] ^ D[2], 15);
        Dqn_Keccak_u64 B3  = DQN_KECCAK_ROL64(A[3 * 5 + 3] ^ D[3], 21);
        Dqn_Keccak_u64 B13 = DQN_KECCAK_ROL64(A[3 * 5 + 4] ^ D[4], 8);

        Dqn_Keccak_u64 B14 = DQN_KECCAK_ROL64(A[4 * 5 + 0] ^ D[0], 18);
        Dqn_Keccak_u64 B24 = DQN_KECCAK_ROL64(A[4 * 5 + 1] ^ D[1], 2);
        Dqn_Keccak_u64 B9  = DQN_KECCAK_ROL64(A[4 * 5 + 2] ^ D[2], 61);
        Dqn_Keccak_u64 B19 = DQN_KECCAK_ROL64(A[4 * 5 + 3] ^ D[3], 56);
        Dqn_Keccak_u64 B4  = DQN_KECCAK_ROL64(A[4 * 5 + 4] ^ D[4], 14);

#if 0
        printf("B00: %024llu\n", A[0 * 5 + 0] ^ D[0]);
        printf("B10: %024llu\n", DQN_KECCAK_ROL64(A[0 * 5 + 1] ^ D[1], 1));
        printf("B20: %024llu\n", DQN_KECCAK_ROL64(A[0 * 5 + 2] ^ D[2], 62));
        printf("B05: %024llu\n", DQN_KECCAK_ROL64(A[0 * 5 + 3] ^ D[3], 28));
        printf("B15: %024llu\n\n", DQN_KECCAK_ROL64(A[0 * 5 + 4] ^ D[4], 27));

        printf("B16: %024llu\n", DQN_KECCAK_ROL64(A[1 * 5 + 0] ^ D[0], 36));
        printf("B01: %024llu\n", DQN_KECCAK_ROL64(A[1 * 5 + 1] ^ D[1], 44));
        printf("B11: %024llu\n", DQN_KECCAK_ROL64(A[1 * 5 + 2] ^ D[2], 6));
        printf("B21: %024llu\n", DQN_KECCAK_ROL64(A[1 * 5 + 3] ^ D[3], 55));
        printf("B06: %024llu\n\n", DQN_KECCAK_ROL64(A[1 * 5 + 4] ^ D[4], 20));

        printf("B07: %024llu\n", DQN_KECCAK_ROL64(A[2 * 5 + 0] ^ D[0], 3));
        printf("B17: %024llu\n", DQN_KECCAK_ROL64(A[2 * 5 + 1] ^ D[1], 10));
        printf("B02: %024llu\n", DQN_KECCAK_ROL64(A[2 * 5 + 2] ^ D[2], 43));
        printf("B12: %024llu\n", DQN_KECCAK_ROL64(A[2 * 5 + 3] ^ D[3], 25));
        printf("B22: %024llu\n\n", DQN_KECCAK_ROL64(A[2 * 5 + 4] ^ D[4], 39));

        printf("B23: %024llu\n", DQN_KECCAK_ROL64(A[3 * 5 + 0] ^ D[0], 41));
        printf("B08: %024llu\n", DQN_KECCAK_ROL64(A[3 * 5 + 1] ^ D[1], 45));
        printf("B18: %024llu\n", DQN_KECCAK_ROL64(A[3 * 5 + 2] ^ D[2], 15));
        printf("B03: %024llu\n", DQN_KECCAK_ROL64(A[3 * 5 + 3] ^ D[3], 21));
        printf("B13: %024llu\n\n", DQN_KECCAK_ROL64(A[3 * 5 + 4] ^ D[4], 8));

        printf("B14: %024llu\n", DQN_KECCAK_ROL64(A[4 * 5 + 0] ^ D[0], 18));
        printf("B24: %024llu\n", DQN_KECCAK_ROL64(A[4 * 5 + 1] ^ D[1], 2));
        printf("B09: %024llu\n", DQN_KECCAK_ROL64(A[4 * 5 + 2] ^ D[2], 61));
        printf("B19: %024llu\n", DQN_KECCAK_ROL64(A[4 * 5 + 3] ^ D[3], 56));
        printf("B04: %024llu\n\n", DQN_KECCAK_ROL64(A[4 * 5 + 4] ^ D[4], 14));
#endif

        A[0 * 5 + 0] = B00 ^ ((~B1) & B2);
        A[0 * 5 + 1] = B1 ^ ((~B2) & B3);
        A[0 * 5 + 2] = B2 ^ ((~B3) & B4);
        A[0 * 5 + 3] = B3 ^ ((~B4) & B00);
        A[0 * 5 + 4] = B4 ^ ((~B00) & B1);

        A[1 * 5 + 0] = B5 ^ ((~B6) & B7);
        A[1 * 5 + 1] = B6 ^ ((~B7) & B8);
        A[1 * 5 + 2] = B7 ^ ((~B8) & B9);
        A[1 * 5 + 3] = B8 ^ ((~B9) & B5);
        A[1 * 5 + 4] = B9 ^ ((~B5) & B6);

        A[2 * 5 + 0] = B10 ^ ((~B11) & B12);
        A[2 * 5 + 1] = B11 ^ ((~B12) & B13);
        A[2 * 5 + 2] = B12 ^ ((~B13) & B14);
        A[2 * 5 + 3] = B13 ^ ((~B14) & B10);
        A[2 * 5 + 4] = B14 ^ ((~B10) & B11);

        A[3 * 5 + 0] = B15 ^ ((~B16) & B17);
        A[3 * 5 + 1] = B16 ^ ((~B17) & B18);
        A[3 * 5 + 2] = B17 ^ ((~B18) & B19);
        A[3 * 5 + 3] = B18 ^ ((~B19) & B15);
        A[3 * 5 + 4] = B19 ^ ((~B15) & B16);

        A[4 * 5 + 0] = B20 ^ ((~B21) & B22);
        A[4 * 5 + 1] = B21 ^ ((~B22) & B23);
        A[4 * 5 + 2] = B22 ^ ((~B23) & B24);
        A[4 * 5 + 3] = B23 ^ ((~B24) & B20);
        A[4 * 5 + 4] = B24 ^ ((~B20) & B21);

        A[0] ^= DQN_KECCAK_ROUNDS[round];

#if 0
        for (int y = 0; y < 5; y++)
        {
            for (int x = 0; x < 5; x++)
            {
                Dqn_Keccak_u64 lane = A[x + (y * 5)];
                printf("[%d,%d] %024llu ", x, y, lane);
            }

            printf("\n");
        }
#endif

    }
}

void Dqn_Keccak__Permute(void *state)
{
    // TODO(dqn): Do some profiling on unrolling and can we SIMD some part of
    // this? Unroll loop, look at data dependencies and investigate.

#if 1
    Dqn_Keccak_u64 *lanes_u64 = (Dqn_Keccak_u64 *)state;
    for (int round_index = 0; round_index < 24; round_index++)
    {
        #define LANE_INDEX(x, y) ((x) + ((y) * DQN_KECCAK_LANE_SIZE_U64))
        // ---------------------------------------------------------------------
        // θ step
        // ---------------------------------------------------------------------
#if 1
        Dqn_Keccak_u64 c[DQN_KECCAK_LANE_SIZE_U64];
        for (int x = 0; x < DQN_KECCAK_LANE_SIZE_U64; x++)
        {
            c[x] = lanes_u64[LANE_INDEX(x, 0)] ^
                   lanes_u64[LANE_INDEX(x, 1)] ^
                   lanes_u64[LANE_INDEX(x, 2)] ^
                   lanes_u64[LANE_INDEX(x, 3)] ^
                   lanes_u64[LANE_INDEX(x, 4)];
        }

        Dqn_Keccak_u64 d[DQN_KECCAK_LANE_SIZE_U64];
        for (int x = 0; x < DQN_KECCAK_LANE_SIZE_U64; x++)
            d[x] = c[(x + 4) % DQN_KECCAK_LANE_SIZE_U64] ^ DQN_KECCAK_ROL64(c[(x + 1) % DQN_KECCAK_LANE_SIZE_U64], 1);

        for (int y = 0; y < DQN_KECCAK_LANE_SIZE_U64; y++)
            for (int x = 0; x < DQN_KECCAK_LANE_SIZE_U64; x++)
                lanes_u64[LANE_INDEX(x, y)] ^= d[x];
#else
        Dqn_Keccak_u64 c[5], d[5];
        c[0] = lanes_u64[0 * 5 + 0] ^ lanes_u64[1 * 5 + 0] ^ lanes_u64[2 * 5 + 0] ^ lanes_u64[3 * 5 + 0] ^ lanes_u64[4 * 5 + 0];
        c[1] = lanes_u64[0 * 5 + 1] ^ lanes_u64[1 * 5 + 1] ^ lanes_u64[2 * 5 + 1] ^ lanes_u64[3 * 5 + 1] ^ lanes_u64[4 * 5 + 1];
        c[2] = lanes_u64[0 * 5 + 2] ^ lanes_u64[1 * 5 + 2] ^ lanes_u64[2 * 5 + 2] ^ lanes_u64[3 * 5 + 2] ^ lanes_u64[4 * 5 + 2];
        c[3] = lanes_u64[0 * 5 + 3] ^ lanes_u64[1 * 5 + 3] ^ lanes_u64[2 * 5 + 3] ^ lanes_u64[3 * 5 + 3] ^ lanes_u64[4 * 5 + 3];
        c[4] = lanes_u64[0 * 5 + 4] ^ lanes_u64[1 * 5 + 4] ^ lanes_u64[2 * 5 + 4] ^ lanes_u64[3 * 5 + 4] ^ lanes_u64[4 * 5 + 4];

        d[0] = c[4] ^ DQN_KECCAK_ROL64(c[1], 1);
        d[1] = c[0] ^ DQN_KECCAK_ROL64(c[2], 1);
        d[2] = c[1] ^ DQN_KECCAK_ROL64(c[3], 1);
        d[3] = c[2] ^ DQN_KECCAK_ROL64(c[4], 1);
        d[4] = c[3] ^ DQN_KECCAK_ROL64(c[0], 1);
#endif

        // ---------------------------------------------------------------------
        // ρ and π steps
        // ---------------------------------------------------------------------
        Dqn_Keccak_u64 b[DQN_KECCAK_LANE_SIZE_U64 * DQN_KECCAK_LANE_SIZE_U64];
        for (int y = 0; y < DQN_KECCAK_LANE_SIZE_U64; y++)
        {
            for (int x = 0; x < DQN_KECCAK_LANE_SIZE_U64; x++)
            {
                Dqn_Keccak_u64 lane                          = lanes_u64[LANE_INDEX(x, y)];
                Dqn_Keccak_u64 rotate_count                  = DQN_KECCAK_ROTATIONS[x][y];
                b[LANE_INDEX(y, (2 * x + 3 * y) % 5)] = DQN_KECCAK_ROL64(lane, rotate_count);

#if 0
                int index = LANE_INDEX(y, (2 * x + 3 * y) % 5);
                printf("B%02d: %024llu\n", index, b[index]);
#endif
            }

#if 0
            printf("\n");
#endif
        }

        // ---------------------------------------------------------------------
        // χ step
        // ---------------------------------------------------------------------
        for (int y = 0; y < DQN_KECCAK_LANE_SIZE_U64; y++)
        {
            for (int x = 0; x < DQN_KECCAK_LANE_SIZE_U64; x++)
            {
                Dqn_Keccak_u64 rhs = ~b[LANE_INDEX((x + 1) % 5, y)] &
                               b[LANE_INDEX((x + 2) % 5, y)];

                lanes_u64[LANE_INDEX(x, y)] = b[LANE_INDEX(x, y)] ^ rhs;
            }
        }

        // ---------------------------------------------------------------------
        // ι step
        // ---------------------------------------------------------------------
        lanes_u64[LANE_INDEX(0, 0)] ^= DQN_KECCAK_ROUNDS[round_index];

#if 0
        for (int y = 0; y < 5; y++)
        {
            for (int x = 0; x < 5; x++)
            {
                Dqn_Keccak_u64 lane = lanes_u64[x + (y * 5)];
                printf("[%d,%d] %024llu ", x, y, lane);
            }

            printf("\n");
        }
#endif

        #undef LANE_INDEX
        #undef DQN_KECCAK_ROL64
    }
#else
    Dqn_Keccak__ReferencePermute((Dqn_Keccak_u64 *)state);
#endif
}

static void Dqn_Keccak__Construction(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size, int bitrate, char delimited_suffix)
{
    int const ABSORB_SIZE     = bitrate / 8;
    int const ABSORB_SIZE_U64 = ABSORB_SIZE / 8;
    Dqn_Keccak_u64 state_u64[DQN_KECCAK_LANE_SIZE_U64 * DQN_KECCAK_LANE_SIZE_U64] = {};

    // ---------------------------------------------------------------------
    // Sponge Step: Absorb all the bytes into the state
    // ---------------------------------------------------------------------
    Dqn_Keccak_u64 const *block = (Dqn_Keccak_u64 const *)src;
    int const absorb_count      = src_size / ABSORB_SIZE;
    for (int absorb_index = 0; absorb_index < absorb_count; absorb_index++, block += ABSORB_SIZE_U64)
    {
        for (int index = 0; index < ABSORB_SIZE_U64; index++)
            state_u64[index] ^= block[index];
        Dqn_Keccak__Permute(state_u64);
    }

    // ---------------------------------------------------------------------
    // Sponge Finalization Step: Remaining source bytes + padding
    // ---------------------------------------------------------------------
    // NOTE: Do the remainder bytes not divisible in the block, then, also,
    // complete the sponge by adding the padding bits and delimited suffix.
    {
        Dqn_Keccak_u8 *      state_u8  = (Dqn_Keccak_u8 *) state_u64;
        Dqn_Keccak_u8 const *block_u8  = (Dqn_Keccak_u8 const *) block;
        int const            remainder = src_size % ABSORB_SIZE;
        for (int index = 0; index < remainder; index++)
            state_u8[index] ^= block_u8[index];

        // NOTE: (remainder + 1) can never point out of array bounds, the
        // remainder is guaranteed to be less than the  ABSORB_SIZE, since we
        // processed all the full blocks above.
        int const delimited_suffix_index = remainder;
        int const INDEX_OF_0X80_BYTE     = ABSORB_SIZE - 1;
        state_u8[delimited_suffix_index] ^= delimited_suffix;

        // NOTE: In the reference implementation, it checks that if the
        // delimited suffix is set to the padding bit (0x80), then we need to
        // permute twice. Once for the delimited suffix, and a second time for
        // the "padding" permute.
        //
        // However all standard algorithms either specify a 0x01, or 0x06, 0x04
        // delimited suffix and so forth- so this case is never hit. We can omit
        // this from the implementation here.

        state_u8[INDEX_OF_0X80_BYTE] ^= 0x80;
        Dqn_Keccak__Permute(state_u64);
    }

    // ---------------------------------------------------------------------
    // Squeeze Step: Squeeze bytes from the state into our hash
    // ---------------------------------------------------------------------
    Dqn_Keccak_u8 * dest_u8       = (Dqn_Keccak_u8 *) dest;
    int const       squeeze_count = dest_size / ABSORB_SIZE;
    int             squeeze_index = 0;
    for (; squeeze_index < squeeze_count; squeeze_index++)
    {
        if (squeeze_index) Dqn_Keccak__Permute(state_u64);
        DQN_KECCAK_MEMCOPY(dest_u8, state_u64, ABSORB_SIZE);
        dest_u8 += ABSORB_SIZE;
    }

    // ---------------------------------------------------------------------
    // Squeeze Finalisation Step: Remainder bytes in hash
    // ---------------------------------------------------------------------
    {
        int const remainder = dest_size % ABSORB_SIZE;
        if (remainder)
        {
            if (squeeze_index) Dqn_Keccak__Permute(state_u64);
            DQN_KECCAK_MEMCOPY(dest_u8, state_u64, remainder);
        }
    }
}

#define DQN_KECCAK_SHA3_DELIMITED_SUFFIX 0x06
#define DQN_KECCAK_DELIMITED_SUFFIX 0x01

// -----------------------------------------------------------------------------
// NOTE: SHA3-224
// -----------------------------------------------------------------------------
void Dqn_Keccak_SHA3_224(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size)
{
    int const BITRATE = 1152;
    DQN_KECCAK_ASSERT(dest_size >= 224 / 8);
    Dqn_Keccak__Construction(src, src_size, dest, dest_size, BITRATE, DQN_KECCAK_SHA3_DELIMITED_SUFFIX);
}

Dqn_KeccakBytes28 Dqn_Keccak_SHA3_224_ToBytes28(void *bytes, Dqn_Keccak_u64 bytes_size)
{
    Dqn_KeccakBytes28 result;
    Dqn_Keccak_SHA3_224(bytes, bytes_size, result.data, sizeof(result));
    return result;
}

// -----------------------------------------------------------------------------
// NOTE: SHA3-256
// -----------------------------------------------------------------------------
void Dqn_Keccak_SHA3_256(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size)
{
    int const BITRATE = 1088;
    DQN_KECCAK_ASSERT(dest_size >= 256 / 8);
    Dqn_Keccak__Construction(src, src_size, dest, dest_size, BITRATE, DQN_KECCAK_SHA3_DELIMITED_SUFFIX);
}

Dqn_KeccakBytes32 Dqn_Keccak_SHA3_256_ToBytes32(void *bytes, Dqn_Keccak_u64 bytes_size)
{
    Dqn_KeccakBytes32 result;
    Dqn_Keccak_SHA3_256(bytes, bytes_size, result.data, sizeof(result));
    return result;
}

// -----------------------------------------------------------------------------
// NOTE: SHA3-384
// -----------------------------------------------------------------------------
void Dqn_Keccak_SHA3_384(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size)
{
    int const BITRATE = 832;
    DQN_KECCAK_ASSERT(dest_size >= 384 / 8);
    Dqn_Keccak__Construction(src, src_size, dest, dest_size, BITRATE, DQN_KECCAK_SHA3_DELIMITED_SUFFIX);
}

Dqn_KeccakBytes48 Dqn_Keccak_SHA3_384_ToBytes48(void *bytes, Dqn_Keccak_u64 bytes_size)
{
    Dqn_KeccakBytes48 result;
    Dqn_Keccak_SHA3_384(bytes, bytes_size, result.data, sizeof(result));
    return result;
}

// -----------------------------------------------------------------------------
// NOTE: SHA3-512
// -----------------------------------------------------------------------------
void Dqn_Keccak_SHA3_512(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size)
{
    int const BITRATE = 576;
    DQN_KECCAK_ASSERT(dest_size >= 512 / 8);
    Dqn_Keccak__Construction(src, src_size, dest, dest_size, BITRATE, DQN_KECCAK_SHA3_DELIMITED_SUFFIX);
}

Dqn_KeccakBytes64 Dqn_Keccak_SHA3_512_ToBytes64(void *bytes, Dqn_Keccak_u64 bytes_size)
{
    Dqn_KeccakBytes64 result;
    Dqn_Keccak_SHA3_512(bytes, bytes_size, result.data, sizeof(result));
    return result;
}

#if defined(DQN_H)
// -----------------------------------------------------------------------------
// NOTE: SHA3 - Helpers for Dqn data structures
// -----------------------------------------------------------------------------
Dqn_KeccakBytes28 Dqn_Keccak_SHA3_224_StringToBytes28(Dqn_String string)
{
    Dqn_KeccakBytes28 result;
    Dqn_Keccak_SHA3_224(string.str, string.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes28 Dqn_Keccak_SHA3_224_U8ArrayToBytes28(Dqn_Array<Dqn_Keccak_u8> array)
{
    Dqn_KeccakBytes28 result;
    Dqn_Keccak_SHA3_224(array.data, array.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes32 Dqn_Keccak_SHA3_256_StringToBytes32(Dqn_String string)
{
    Dqn_KeccakBytes32 result;
    Dqn_Keccak_SHA3_256(string.str, string.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes32 Dqn_Keccak_SHA3_256_U8ArrayToBytes32(Dqn_Array<Dqn_Keccak_u8> array)
{
    Dqn_KeccakBytes32 result;
    Dqn_Keccak_SHA3_256(array.data, array.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes48 Dqn_Keccak_SHA3_384_StringToBytes48(Dqn_String string)
{
    Dqn_KeccakBytes48 result;
    Dqn_Keccak_SHA3_384(string.str, string.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes48 Dqn_Keccak_SHA3_384_U8ArrayToBytes48(Dqn_Array<Dqn_Keccak_u8> array)
{
    Dqn_KeccakBytes48 result;
    Dqn_Keccak_SHA3_384(array.data, array.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes64 Dqn_Keccak_SHA3_512_StringToBytes64(Dqn_String string)
{
    Dqn_KeccakBytes64 result;
    Dqn_Keccak_SHA3_512(string.str, string.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes64 Dqn_Keccak_SHA3_512_U8ArrayToBytes64(Dqn_Array<Dqn_Keccak_u8> array)
{
    Dqn_KeccakBytes64 result;
    Dqn_Keccak_SHA3_512(array.data, array.size, result.data, sizeof(result));
    return result;
}
#endif // DQN_H

// -----------------------------------------------------------------------------
// NOTE: Keccak-224
// -----------------------------------------------------------------------------
void Dqn_Keccak_224(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size)
{
    int const BITRATE = 1152;
    DQN_KECCAK_ASSERT(dest_size >= 224 / 8);
    Dqn_Keccak__Construction(src, src_size, dest, dest_size, BITRATE, DQN_KECCAK_DELIMITED_SUFFIX);
}

Dqn_KeccakBytes28 Dqn_Keccak_224_ToBytes28(void *bytes, Dqn_Keccak_u64 bytes_size)
{
    Dqn_KeccakBytes28 result;
    Dqn_Keccak_224(bytes, bytes_size, result.data, sizeof(result));
    return result;
}

// -----------------------------------------------------------------------------
// NOTE: Keccak-256
// -----------------------------------------------------------------------------
void Dqn_Keccak_256(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size)
{
    int const BITRATE = 1088;
    DQN_KECCAK_ASSERT(dest_size >= 256 / 8);
    Dqn_Keccak__Construction(src, src_size, dest, dest_size, BITRATE, DQN_KECCAK_DELIMITED_SUFFIX);
}

Dqn_KeccakBytes32 Dqn_Keccak_256_ToBytes32(void *bytes, Dqn_Keccak_u64 bytes_size)
{
    Dqn_KeccakBytes32 result;
    Dqn_Keccak_256(bytes, bytes_size, result.data, sizeof(result));
    return result;
}

// -----------------------------------------------------------------------------
// NOTE: Keccak-384
// -----------------------------------------------------------------------------
void Dqn_Keccak_384(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size)
{
    int const BITRATE = 832;
    DQN_KECCAK_ASSERT(dest_size >= 384 / 8);
    Dqn_Keccak__Construction(src, src_size, dest, dest_size, BITRATE, DQN_KECCAK_DELIMITED_SUFFIX);
}

Dqn_KeccakBytes48 Dqn_Keccak_384_ToBytes48(void *bytes, Dqn_Keccak_u64 bytes_size)
{
    Dqn_KeccakBytes48 result;
    Dqn_Keccak_384(bytes, bytes_size, result.data, sizeof(result));
    return result;
}

// -----------------------------------------------------------------------------
// NOTE: Keccak-512
// -----------------------------------------------------------------------------
void Dqn_Keccak_512(void const *src, Dqn_Keccak_u64 src_size, void *dest, int dest_size)
{
    int const BITRATE = 576;
    DQN_KECCAK_ASSERT(dest_size >= 512 / 8);
    Dqn_Keccak__Construction(src, src_size, dest, dest_size, BITRATE, DQN_KECCAK_DELIMITED_SUFFIX);
}

Dqn_KeccakBytes64 Dqn_Keccak_512_ToBytes64(void *bytes, Dqn_Keccak_u64 bytes_size)
{
    Dqn_KeccakBytes64 result;
    Dqn_Keccak_512(bytes, bytes_size, result.data, sizeof(result));
    return result;
}

#if defined(DQN_H)
// -----------------------------------------------------------------------------
// NOTE: Keccak - Helpers for Dqn data structures
// -----------------------------------------------------------------------------
Dqn_KeccakBytes28 Dqn_Keccak_224_StringToBytes28(Dqn_String string)
{
    Dqn_KeccakBytes28 result;
    Dqn_Keccak_224(string.str, string.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes28 Dqn_Keccak_224_U8ArrayToBytes28(Dqn_Array<Dqn_Keccak_u8> array)
{
    Dqn_KeccakBytes28 result;
    Dqn_Keccak_224(array.data, array.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes32 Dqn_Keccak_256_StringToBytes32(Dqn_String string)
{
    Dqn_KeccakBytes32 result;
    Dqn_Keccak_256(string.str, string.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes32 Dqn_Keccak_256_U8ArrayToBytes32(Dqn_Array<Dqn_Keccak_u8> array)
{
    Dqn_KeccakBytes32 result;
    Dqn_Keccak_256(array.data, array.size, result.data, sizeof(result));
    return result;
}


Dqn_KeccakBytes48 Dqn_Keccak_384_StringToBytes48(Dqn_String string)
{
    Dqn_KeccakBytes48 result;
    Dqn_Keccak_384(string.str, string.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes48 Dqn_Keccak_384_U8ArrayToBytes48(Dqn_Array<Dqn_Keccak_u8> array)
{
    Dqn_KeccakBytes48 result;
    Dqn_Keccak_384(array.data, array.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes64 Dqn_Keccak_512_StringToBytes64(Dqn_String string)
{
    Dqn_KeccakBytes64 result;
    Dqn_Keccak_512(string.str, string.size, result.data, sizeof(result));
    return result;
}

Dqn_KeccakBytes64 Dqn_Keccak_512_U8ArrayToBytes64(Dqn_Array<Dqn_Keccak_u8> array)
{
    Dqn_KeccakBytes64 result;
    Dqn_Keccak_512(array.data, array.size, result.data, sizeof(result));
    return result;
}
#endif // DQN_H

// -----------------------------------------------------------------------------
// NOTE: Helper functions
// -----------------------------------------------------------------------------
void Dqn_Keccak_BytesToHex(void const *src, Dqn_Keccak_u64 src_size, char *dest, Dqn_Keccak_u64 dest_size)
{
    (void)src_size; (void)dest_size;
    DQN_KECCAK_ASSERT(dest_size >= src_size * 2);

    unsigned char *src_u8 = (unsigned char *)src;
    for (Dqn_Keccak_u64 src_index = 0, dest_index = 0;
         src_index < src_size;
         src_index += 1, dest_index += 2)
    {
        char byte            = src_u8[src_index];
        char hex01           = (byte >> 4) & 0b1111;
        char hex02           = (byte >> 0) & 0b1111;
        dest[dest_index + 0] = hex01 < 10 ? (hex01 + '0') : (hex01 - 10) + 'a';
        dest[dest_index + 1] = hex02 < 10 ? (hex02 + '0') : (hex02 - 10) + 'a';
    }
}

#if defined(DQN_NO_MALLOC_FUNCTIONS)
char *Dqn_Keccak_BytesToHexCString(void const *src, Dqn_Keccak_u64 src_size)
{
    int result_size = (src_size * 2);
    char *result = DQN_KECCAK_MALLOC(result_size + 1 /*null-terminator*/);
    if (result)
    {
        Dqn_Keccak_BytesToHex(src, src_size, result, result_size);
        result[result_size] = 0;
    }

    return result;
}
#endif // DQN_NO_MALLOC_FUNCTIONS

Dqn_KeccakString56 Dqn_Keccak_Bytes28ToHex(Dqn_KeccakBytes28 const *bytes)
{
    Dqn_KeccakString56 result;
    Dqn_Keccak_BytesToHex(bytes->data, sizeof(bytes->data), result.str, sizeof(result.str));
    result.str[sizeof(result.str) - 1] = 0;
    return result;
}

Dqn_KeccakString64 Dqn_Keccak_Bytes32ToHex(Dqn_KeccakBytes32 const *bytes)
{
    Dqn_KeccakString64 result;
    Dqn_Keccak_BytesToHex(bytes->data, sizeof(bytes->data), result.str, sizeof(result.str));
    result.str[sizeof(result.str) - 1] = 0;
    return result;
}

Dqn_KeccakString96 Dqn_Keccak_Bytes48ToHex(Dqn_KeccakBytes48 const *bytes)
{
    Dqn_KeccakString96 result;
    Dqn_Keccak_BytesToHex(bytes->data, sizeof(bytes->data), result.str, sizeof(result.str));
    result.str[sizeof(result.str) - 1] = 0;
    return result;
}

Dqn_KeccakString128 Dqn_Keccak_Bytes64ToHex(Dqn_KeccakBytes64 const *bytes)
{
    Dqn_KeccakString128 result;
    Dqn_Keccak_BytesToHex(bytes->data, sizeof(bytes->data), result.str, sizeof(result.str));
    result.str[sizeof(result.str) - 1] = 0;
    return result;
}

int Dqn_Keccak_Bytes28Equals(Dqn_KeccakBytes28 const *a, Dqn_KeccakBytes28 const *b)
{
    int result = DQN_KECCAK_MEMCMP(a->data, b->data, sizeof(*a)) == 0;
    return result;
}

int Dqn_Keccak_Bytes32Equals(Dqn_KeccakBytes32 const *a, Dqn_KeccakBytes32 const *b)
{
    int result = DQN_KECCAK_MEMCMP(a->data, b->data, sizeof(*a)) == 0;
    return result;
}

int Dqn_Keccak_Bytes48Equals(Dqn_KeccakBytes48 const *a, Dqn_KeccakBytes48 const *b)
{
    int result = DQN_KECCAK_MEMCMP(a->data, b->data, sizeof(*a)) == 0;
    return result;
}

int Dqn_Keccak_Bytes64Equals(Dqn_KeccakBytes64 const *a, Dqn_KeccakBytes64 const *b)
{
    int result = DQN_KECCAK_MEMCMP(a->data, b->data, sizeof(*a)) == 0;
    return result;
}

#if defined(DQN_H)
// -----------------------------------------------------------------------------
// NOTE: Other helper functions for Dqn data structures
// -----------------------------------------------------------------------------
Dqn_KeccakBytes32 Dqn_Keccak_Hex64StringToBytes(Dqn_String hex)
{
    DQN_KECCAK_ASSERT(hex.size == 64);
    Dqn_KeccakBytes32 result;
    Dqn_Hex_HexToBytes(hex.str, hex.size, result.data, sizeof(result));
    return result;
}
#endif // DQN_H

#endif // DQN_KECCAK_IMPLEMENTATION
