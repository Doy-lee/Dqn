// NOTE: [$INTR] Intrinsics ========================================================================
#if !defined(DQN_OS_ARM64)
#if defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
#include <cpuid.h>
#endif

Dqn_CPUIDRegisters Dqn_CPUID(int function_id)
{
    Dqn_CPUIDRegisters result = {};
    #if defined(DQN_COMPILER_W32_MSVC)
    __cpuid(DQN_CAST(int *)result.array, function_id);
    #elif defined(DQN_COMPILER_GCC) || defined(DQN_COMPILER_CLANG)
    __get_cpuid(function_id, &result.array[0] /*eax*/, &result.array[1] /*ebx*/, &result.array[2] /*ecx*/ , &result.array[3] /*edx*/);
    #else
    #error "Compiler not supported"
    #endif
    return result;
}
#endif // !defined(DQN_OS_ARM64)

// NOTE: [$TMUT] Dqn_TicketMutex ===================================================================
DQN_API void Dqn_TicketMutex_Begin(Dqn_TicketMutex *mutex)
{
    unsigned int ticket = Dqn_Atomic_AddU32(&mutex->ticket, 1);
    Dqn_TicketMutex_BeginTicket(mutex, ticket);
}

DQN_API void Dqn_TicketMutex_End(Dqn_TicketMutex *mutex)
{
    Dqn_Atomic_AddU32(&mutex->serving, 1);
}

DQN_API Dqn_uint Dqn_TicketMutex_MakeTicket(Dqn_TicketMutex *mutex)
{
    Dqn_uint result = Dqn_Atomic_AddU32(&mutex->ticket, 1);
    return result;
}

DQN_API void Dqn_TicketMutex_BeginTicket(Dqn_TicketMutex const *mutex, Dqn_uint ticket)
{
    DQN_ASSERTF(mutex->serving <= ticket,
                "Mutex skipped ticket? Was ticket generated by the correct mutex via MakeTicket? ticket = %u, "
                "mutex->serving = %u",
                ticket,
                mutex->serving);
    while (ticket != mutex->serving) {
        // NOTE: Use spinlock intrinsic
        _mm_pause();
    }
}

DQN_API bool Dqn_TicketMutex_CanLock(Dqn_TicketMutex const *mutex, Dqn_uint ticket)
{
    bool result = (ticket == mutex->serving);
    return result;
}

#if defined(DQN_COMPILER_W32_MSVC) || defined(DQN_COMPILER_W32_CLANG)
    #if !defined(DQN_CRT_SECURE_NO_WARNINGS_PREVIOUSLY_DEFINED)
        #undef _CRT_SECURE_NO_WARNINGS
    #endif
#endif