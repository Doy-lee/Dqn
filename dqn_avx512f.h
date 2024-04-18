#if !defined(DQN_AVX512F_H)
#define DQN_AVX512F_H

/*
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    $$$$$$\  $$\    $$\ $$\   $$\        $$$$$$$\    $$\    $$$$$$\  $$$$$$$$\
//   $$  __$$\ $$ |   $$ |$$ |  $$ |       $$  ____| $$$$ |  $$  __$$\ $$  _____|
//   $$ /  $$ |$$ |   $$ |\$$\ $$  |       $$ |      \_$$ |  \__/  $$ |$$ |
//   $$$$$$$$ |\$$\  $$  | \$$$$  /$$$$$$\ $$$$$$$\    $$ |   $$$$$$  |$$$$$\
//   $$  __$$ | \$$\$$  /  $$  $$< \______|\_____$$\   $$ |  $$  ____/ $$  __|
//   $$ |  $$ |  \$$$  /  $$  /\$$\        $$\   $$ |  $$ |  $$ |      $$ |
//   $$ |  $$ |   \$  /   $$ /  $$ |       \$$$$$$  |$$$$$$\ $$$$$$$$\ $$ |
//   \__|  \__|    \_/    \__|  \__|        \______/ \______|\________|\__|
//
//   dqn_avx512f.h -- Functions implemented w/ AVX512
//
////////////////////////////////////////////////////////////////////////////////////////////////////
*/

#include "dqn.h"

DQN_API Dqn_Str8FindResult        Dqn_Str8_FindStr8AVX512F       (Dqn_Str8 string, Dqn_Str8 find);
DQN_API Dqn_Str8FindResult        Dqn_Str8_FindLastStr8AVX512F   (Dqn_Str8 string, Dqn_Str8 find);
DQN_API Dqn_Str8BinarySplitResult Dqn_Str8_BinarySplitAVX512F    (Dqn_Str8 string, Dqn_Str8 find);
DQN_API Dqn_Str8BinarySplitResult Dqn_Str8_BinarySplitLastAVX512F(Dqn_Str8 string, Dqn_Str8 find);
DQN_API Dqn_usize                 Dqn_Str8_SplitAVX512F          (Dqn_Str8 string, Dqn_Str8 delimiter, Dqn_Str8 *splits, Dqn_usize splits_count, Dqn_Str8SplitIncludeEmptyStrings mode);
DQN_API Dqn_Slice<Dqn_Str8>       Dqn_Str8_SplitAllocAVX512F     (Dqn_Arena *arena, Dqn_Str8 string, Dqn_Str8 delimiter, Dqn_Str8SplitIncludeEmptyStrings mode);

#endif // DQN_AVX512F_H
