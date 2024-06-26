#pragma once
#include "dqn.h"

/*
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   $$$$$$$$\ $$\   $$\ $$$$$$$\  $$$$$$$$\  $$$$$$\  $$$$$$$\
//   \__$$  __|$$ |  $$ |$$  __$$\ $$  _____|$$  __$$\ $$  __$$\
//      $$ |   $$ |  $$ |$$ |  $$ |$$ |      $$ /  $$ |$$ |  $$ |
//      $$ |   $$$$$$$$ |$$$$$$$  |$$$$$\    $$$$$$$$ |$$ |  $$ |
//      $$ |   $$  __$$ |$$  __$$< $$  __|   $$  __$$ |$$ |  $$ |
//      $$ |   $$ |  $$ |$$ |  $$ |$$ |      $$ |  $$ |$$ |  $$ |
//      $$ |   $$ |  $$ |$$ |  $$ |$$$$$$$$\ $$ |  $$ |$$$$$$$  |
//      \__|   \__|  \__|\__|  \__|\________|\__|  \__|\_______/
//
//    $$$$$$\   $$$$$$\  $$\   $$\ $$$$$$$$\ $$$$$$$$\ $$\   $$\ $$$$$$$$\
//   $$  __$$\ $$  __$$\ $$$\  $$ |\__$$  __|$$  _____|$$ |  $$ |\__$$  __|
//   $$ /  \__|$$ /  $$ |$$$$\ $$ |   $$ |   $$ |      \$$\ $$  |   $$ |
//   $$ |      $$ |  $$ |$$ $$\$$ |   $$ |   $$$$$\     \$$$$  /    $$ |
//   $$ |      $$ |  $$ |$$ \$$$$ |   $$ |   $$  __|    $$  $$<     $$ |
//   $$ |  $$\ $$ |  $$ |$$ |\$$$ |   $$ |   $$ |      $$  /\$$\    $$ |
//   \$$$$$$  | $$$$$$  |$$ | \$$ |   $$ |   $$$$$$$$\ $$ /  $$ |   $$ |
//    \______/  \______/ \__|  \__|   \__|   \________|\__|  \__|   \__|
//
//   dqn_thread_context.h -- Per thread data (e.g. scratch arenas)
//
////////////////////////////////////////////////////////////////////////////////////////////////////
*/

struct Dqn_ThreadContext
{
    Dqn_b32       init;
    Dqn_Arena    *scratch_arenas[2];
    Dqn_Arena    *error_sink_arena;
    Dqn_CallSite  call_site;
    Dqn_ErrorSink error_sink;
};

struct Dqn_Scratch
{
    Dqn_Scratch(Dqn_ThreadContext *context, uint8_t context_index);
    ~Dqn_Scratch();

    Dqn_Arena        *arena;
    Dqn_b32           destructed;
    Dqn_ArenaTempMem  temp_mem;
};

DQN_API bool               Dqn_ThreadContext_IsInit();
DQN_API Dqn_ThreadContext *Dqn_ThreadContext_Get();
#define                    Dqn_ThreadContext_SaveCallSite do { Dqn_ThreadContext_Get()->call_site = DQN_CALL_SITE; } while (0)
DQN_API Dqn_Scratch        Dqn_Scratch_Get(void const *conflict_arena);
