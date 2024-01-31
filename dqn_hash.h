////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   $$\   $$\  $$$$$$\   $$$$$$\  $$\   $$\
//   $$ |  $$ |$$  __$$\ $$  __$$\ $$ |  $$ |
//   $$ |  $$ |$$ /  $$ |$$ /  \__|$$ |  $$ |
//   $$$$$$$$ |$$$$$$$$ |\$$$$$$\  $$$$$$$$ |
//   $$  __$$ |$$  __$$ | \____$$\ $$  __$$ |
//   $$ |  $$ |$$ |  $$ |$$\   $$ |$$ |  $$ |
//   $$ |  $$ |$$ |  $$ |\$$$$$$  |$$ |  $$ |
//   \__|  \__|\__|  \__| \______/ \__|  \__|
//
//   dqn_hash.h -- Hashing functions
//
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// [$FNV1] Dqn_FNV1A       -- Hash(x) -> 32/64bit via FNV1a
// [$MMUR] Dqn_MurmurHash3 -- Hash(x) -> 32/128bit via MurmurHash3
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// NOTE: [$FNV1] Dqn_FNV1A /////////////////////////////////////////////////////////////////////////
#if !defined(DQN_FNV1A32_SEED)
    #define DQN_FNV1A32_SEED 2166136261U
#endif

#if !defined(DQN_FNV1A64_SEED)
    #define DQN_FNV1A64_SEED 14695981039346656037ULL
#endif

// NOTE: [$MMUR] Dqn_MurmurHash3 ///////////////////////////////////////////////////////////////////
struct Dqn_MurmurHash3 { uint64_t e[2]; };

// NOTE: [$FNV1] Dqn_FNV1A /////////////////////////////////////////////////////////////////////////
DQN_API uint32_t        Dqn_FNV1A32_Hash            (void const *bytes, Dqn_usize size);
DQN_API uint64_t        Dqn_FNV1A64_Hash            (void const *bytes, Dqn_usize size);
DQN_API uint32_t        Dqn_FNV1A32_Iterate         (void const *bytes, Dqn_usize size, uint32_t hash);
DQN_API uint64_t        Dqn_FNV1A64_Iterate         (void const *bytes, Dqn_usize size, uint64_t hash);

// NOTE: [$MMUR] Dqn_MurmurHash3 ///////////////////////////////////////////////////////////////////
DQN_API uint32_t        Dqn_MurmurHash3_x86U32      (void const *key, int len, uint32_t seed);
DQN_API Dqn_MurmurHash3 Dqn_MurmurHash3_x64U128     (void const *key, int len, uint32_t seed);
#define                 Dqn_MurmurHash3_x64U128AsU64(key, len, seed) (Dqn_MurmurHash3_x64U128(key, len, seed).e[0])
#define                 Dqn_MurmurHash3_x64U128AsU32(key, len, seed) (DQN_CAST(uint32_t)Dqn_MurmurHash3_x64U128(key, len, seed).e[0])

