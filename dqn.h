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
