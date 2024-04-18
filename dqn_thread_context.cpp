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
//   dqn_thread_context.cpp
//
////////////////////////////////////////////////////////////////////////////////////////////////////
*/

DQN_THREAD_LOCAL Dqn_ThreadContext g_dqn_thread_context;

// NOTE: [$TCTX] Dqn_ThreadContext /////////////////////////////////////////////////////////////////
Dqn_Scratch::Dqn_Scratch(Dqn_ThreadContext *context, uint8_t context_index)
{
    arena      = context->scratch_arenas[context_index];
    temp_mem   = Dqn_Arena_TempMemBegin(arena);
    destructed = false;
}

Dqn_Scratch::~Dqn_Scratch()
{
    DQN_ASSERT(destructed == false);
    Dqn_Arena_TempMemEnd(temp_mem);
    destructed = true;
}

DQN_API bool Dqn_Thread_ContextIsInit()
{
    bool result = g_dqn_thread_context.init;
    return result;
}

DQN_API Dqn_ThreadContext *Dqn_ThreadContext_Get()
{
    Dqn_ThreadContext *result = &g_dqn_thread_context;
    if (result->init)
        return result;

    result->init              = true;
    Dqn_ArenaCatalog *catalog = &g_dqn_library->arena_catalog;
    DQN_HARD_ASSERTF(g_dqn_library && g_dqn_library->lib_init, "Library must be initialised by calling Dqn_Library_Init()");

    // NOTE: Setup scratch arenas //////////////////////////////////////////////////////////////////
    Dqn_TicketMutex_Begin(&g_dqn_library->thread_context_init_mutex);
    DQN_FOR_UINDEX (index, DQN_ARRAY_UCOUNT(result->scratch_arenas)) {

        // NOTE: We allocate arenas so that they all come from the memory
        // allocated from the address space of this library. This allows the
        // library to be used across DLL boundaries for example as long as
        // across the boundaries the same g_dqn_library instance is shared.
        //
        // On unload of the DLL, the address space is deallocated. If we stored
        // these as TLS stack variables, these arenas would persist and point to
        // invalid memory addresses.
        Dqn_FStr8<128> label = Dqn_FStr8_InitF<128>("T%05u Scratch %zu", Dqn_OS_ThreadID(), index);

        // NOTE: Hence here we search for the arena. If it already exists then
        // we are in that DLL boundary situation where the TLS data has been
        // reinitialised and zero-ed out. We will try and find the matching
        // arena in the catalog and re-use it.
        //
        // NOTE: This operation is so infrequent and the number of arenas one
        // has in their program should be low that a string look-up should be
        // cheap and fine.
        Dqn_ArenaCatalogItem *catalog_item = Dqn_ArenaCatalog_Find(catalog, Dqn_FStr8_ToStr8(&label));
        if (catalog_item == &catalog->sentinel) {
            Dqn_Arena *scratch            = Dqn_ArenaCatalog_AllocLabelCopy(catalog, 0, 0, Dqn_ArenaFlag_AllocCanLeak, Dqn_FStr8_ToStr8(&label));
            result->scratch_arenas[index] = scratch;
        } else {
            // NOTE: Reuse the arena
            result->scratch_arenas[index] = catalog_item->arena;
        }
    }

    // NOTE: Setup error sink //////////////////////////////////////////////////////////////////////
    {
        Dqn_FStr8<128>        label        = Dqn_FStr8_InitF<128>("T%05u Error Sink", Dqn_OS_ThreadID());
        Dqn_ArenaCatalogItem *catalog_item = Dqn_ArenaCatalog_Find(catalog, Dqn_FStr8_ToStr8(&label));
        if (catalog_item == &catalog->sentinel) {
            result->error_sink_arena = Dqn_ArenaCatalog_AllocLabelCopy(catalog, 0, 0, Dqn_ArenaFlag_AllocCanLeak, Dqn_FStr8_ToStr8(&label));
        } else {
            // NOTE: Reuse the arena
            result->error_sink_arena = catalog_item->arena;
        }
        result->error_sink.arena = result->error_sink_arena;
    }
    Dqn_TicketMutex_End(&g_dqn_library->thread_context_init_mutex);
    return result;
}

// TODO: Is there a way to handle conflict arenas without the user needing to
// manually pass it in?
DQN_API Dqn_Scratch Dqn_Scratch_Get(void const *conflict_arena)
{
    Dqn_ThreadContext *context       = Dqn_ThreadContext_Get();
    uint8_t            context_index = (uint8_t)-1;
    for (uint8_t index = 0; index < DQN_ARRAY_UCOUNT(context->scratch_arenas); index++) {
        Dqn_Arena *arena = context->scratch_arenas[index];
        if (!conflict_arena || arena != conflict_arena) {
            context_index = index;
            break;
        }
    }

    DQN_ASSERT(context_index != (uint8_t)-1);
    return Dqn_Scratch(context, context_index);
}
