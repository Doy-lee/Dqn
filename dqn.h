// NOTE: [$CFGM] Config macros =====================================================================
//
// #define DQN_IMPLEMENTATION
//     Define this in one and only one C++ file to enable the implementation
//     code of the header file
//
// #define DQN_NO_ASSERT
//     Turn all assertion macros to no-ops
//
// #define DQN_NO_CHECK_BREAK
//     Disable debug break when a check macro's expression fails. Instead only 
//     the error will be logged.
//
// #define DQN_NO_WIN32_MIN_HEADER
//     Define this to stop this library from defining a minimal subset of Win32
//     prototypes and definitions in this file. Useful for stopping redefinition
//     of symbols if another library includes "Windows.h"
//
// #define DQN_STATIC_API
//     Apply static to all function definitions and disable external linkage to
//     other translation units.
//
// #define DQN_STB_SPRINTF_HEADER_ONLY
//     Define this to stop this library from defining
//     STB_SPRINTF_IMPLEMENTATION. Useful if another library uses and includes
//     "stb_sprintf.h"
//
// #define DQN_MEMSET_BYTE 0
//     Change the byte that DQN_MEMSET will clear memory with. By default this
//     is set to 0. Some of this library API accepts are clear memory parameter
//     to scrub memory after certain operations.
//
// #define DQN_LEAK_TRACING
//     When defined to some allocating calls in the library will automatically
//     get passed in the file name, function name, line number and an optional
//     leak_msg.

#if defined(DQN_LEAK_TRACING)
#error Leak tracing not supported because we enter an infinite leak tracing loop tracing our own allocations made to tracks leaks in the internal leak table.
#endif

//
// #define DQN_DEBUG_THREAD_CONTEXT
//     Define this macro to record allocation stats for arenas used in the
//     thread context. The thread context arena stats can be printed by using
//     Dqn_Library_DumpThreadContextArenaStat.
#if !defined(DQN_H)
    #define DQN_H
    #include <stdarg.h> // va_list
    #include <stdio.h>  // fprintf, FILE, stdout, stderr
    #include <stdint.h> // [u]int_*, ...
    #include <limits.h> // [U]INT_MAX, ...

    #include "dqn_core.h"
    #include "dqn_memory.h"
    #include "dqn_print.h"
    #include "dqn_strings.h"
    #include "dqn_platform.h"
    #include "dqn_containers.h"
    #include "dqn_math.h"
    #include "dqn_misc.h"
    #include "dqn_hash.h"
#endif // DQN_H

#if defined(DQN_IMPLEMENTATION)
    #include "dqn_platform.cpp"
    #include "dqn_memory.cpp"
    #include "dqn_print.cpp"
    #include "dqn_strings.cpp"
    #include "dqn_containers.cpp"
    #include "dqn_math.cpp"
    #include "dqn_misc.cpp"
    #include "dqn_hash.cpp"
    #include "dqn_core.cpp"
#endif // DQN_IMPLEMENTATION
