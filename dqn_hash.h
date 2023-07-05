// NOTE: Table Of Contents =========================================================================
// Index                   | Disable #define | Description
// =================================================================================================
// [$FNV1] Dqn_FNV1A       |                 | Hash(x) -> 32/64bit via FNV1a
// [$MMUR] Dqn_MurmurHash3 |                 | Hash(x) -> 32/128bit via MurmurHash3
// =================================================================================================

// NOTE: [$FNV1] Dqn_FNV1A =========================================================================
#ifndef DQN_FNV1A32_SEED
    #define DQN_FNV1A32_SEED 2166136261U
#endif

#ifndef DQN_FNV1A64_SEED
    #define DQN_FNV1A64_SEED 14695981039346656037ULL
#endif

/// @begincode
/// char buffer1[128] = {random bytes};
/// char buffer2[128] = {random bytes};
/// uint64_t hash     = Dqn_FNV1A64_Hash(buffer1, sizeof(buffer1));
/// hash              = Dqn_FNV1A64_Iterate(buffer2, sizeof(buffer2), hash); // subsequent hashing
/// @endcode
DQN_API uint32_t Dqn_FNV1A32_Hash   (void const *bytes, Dqn_usize size);
DQN_API uint64_t Dqn_FNV1A64_Hash   (void const *bytes, Dqn_usize size);
DQN_API uint32_t Dqn_FNV1A32_Iterate(void const *bytes, Dqn_usize size, uint32_t hash);
DQN_API uint64_t Dqn_FNV1A64_Iterate(void const *bytes, Dqn_usize size, uint64_t hash);

// NOTE: [$MMUR] Dqn_MurmurHash3 ===================================================================
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author (Austin Appleby) hereby disclaims copyright to this source
// code.
//
// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.
struct Dqn_MurmurHash3 { uint64_t e[2]; };

DQN_API uint32_t        Dqn_MurmurHash3_x86U32 (void const *key, int len, uint32_t seed);
DQN_API Dqn_MurmurHash3 Dqn_MurmurHash3_x64U128(void const *key, int len, uint32_t seed);
#define Dqn_MurmurHash3_x64U128AsU64(key, len, seed) (Dqn_MurmurHash3_x64U128(key, len, seed).e[0])
#define Dqn_MurmurHash3_x64U128AsU32(key, len, seed) (DQN_CAST(uint32_t)Dqn_MurmurHash3_x64U128(key, len, seed).e[0])

