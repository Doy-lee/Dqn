// dqn.h "Personal standard library" | MIT licensed | git.doylet.dev/dqn
//
// This library is a single-header file-esque library with inspiration taken
// from STB libraries for ease of integration and use. It defines a bunch of
// primitives and standard library functions that are missing and or more
// appropriate for development in modern day computing (e.g. cache friendly
// memory management, 64bit MMU, non-pessimized APIs that aren't constrained by
// the language specification and operate closer to the OS).
//
// Define DQN_IMPLEMENTATION macro in one and only one translation unit to
// enable the implementation of this library, for example:
//
//     #define DQN_IMEPLEMENTATION
//     #include "dqn.h"
//
// Additionally, this library supports including/excluding specific sections
// of the library by using #define on the name of the section. These names are
// documented in the section table of contents at the #define column, for 
// example:
//
//     #define DQN_ONLY_VARRAY
//     #define DQN_ONLY_WIN
//
// Compiles the library with all optional APIs turned off except virtual arrays
// and the Win32 helpers. Alternatively:
//
//     #define DQN_NO_VARRAY
//     #define DQN_NO_WIN
//
// Compiles the library with all optional APIs turned on except the previously
// mentioned APIs.
//
// Below is a table of contents that describes what you can find in each header
// of this library and additional macros that can be defined to customise the
// behaviour of this library.

#if !defined(DQN_H)
#if defined(DQN_ONLY_VARRAY)       || \
    defined(DQN_ONLY_SARRAY)       || \
    defined(DQN_ONLY_FARRAY)       || \
    defined(DQN_ONLY_SLICE)        || \
    defined(DQN_ONLY_DSMAP)        || \
    defined(DQN_ONLY_LIST)         || \
    defined(DQN_ONLY_FSTR8)        || \
    defined(DQN_ONLY_FS)           || \
    defined(DQN_ONLY_WINNET)       || \
    defined(DQN_ONLY_WIN)          || \
    defined(DQN_ONLY_V2)           || \
    defined(DQN_ONLY_V3)           || \
    defined(DQN_ONLY_V4)           || \
    defined(DQN_ONLY_M4)           || \
    defined(DQN_ONLY_RECT)         || \
    defined(DQN_ONLY_JSON_BUILDER) || \
    defined(DQN_ONLY_BIN)          || \
    defined(DQN_ONLY_PROFILER)

    #if !defined(DQN_ONLY_VARRAY)
    #define DQN_NO_VARRAY
    #endif
    #if !defined(DQN_ONLY_FARRAY)
    #define DQN_NO_FARRAY
    #endif
    #if !defined(DQN_ONLY_SARRAY)
    #define DQN_NO_SARRAY
    #endif
    #if !defined(DQN_ONLY_SLICE)
    #define DQN_NO_SLICE
    #endif
    #if !defined(DQN_ONLY_DSMAP)
    #define DQN_NO_DSMAP
    #endif
    #if !defined(DQN_ONLY_LIST)
    #define DQN_NO_LIST
    #endif
    #if !defined(DQN_ONLY_FSTR8)
    #define DQN_NO_FSTR8
    #endif
    #if !defined(DQN_ONLY_FS)
    #define DQN_NO_FS
    #endif
    #if !defined(DQN_ONLY_WINNET)
    #define DQN_NO_WINNET
    #endif
    #if !defined(DQN_ONLY_WIN)
    #define DQN_NO_WIN
    #endif
    #if !defined(DQN_ONLY_V2)
    #define DQN_NO_V2
    #endif
    #if !defined(DQN_ONLY_V3)
    #define DQN_NO_V3
    #endif
    #if !defined(DQN_ONLY_V4)
    #define DQN_NO_V4
    #endif
    #if !defined(DQN_ONLY_M4)
    #define DQN_NO_M4
    #endif
    #if !defined(DQN_ONLY_RECT)
    #define DQN_NO_RECT
    #endif
    #if !defined(DQN_ONLY_JSON_BUILDER)
    #define DQN_NO_JSON_BUILDER
    #endif
    #if !defined(DQN_ONLY_BIN)
    #define DQN_NO_BIN
    #endif
    #if !defined(DQN_ONLY_PROFILER)
    #define DQN_NO_PROFILER
    #endif
#endif

// NOTE: Table of Contents =========================================================================
// Index                      | #define Label               | Description
// NOTE: C Headers =================================================================================
#include <stdarg.h> //        |                             | va_list
#include <stdio.h>  //        |                             | fprintf, FILE, stdout, stderr
#include <stdint.h> //        |                             | [u]int_*, ...
#include <limits.h> //        |                             | [U]INT_MAX, ...

// NOTE: Dqn_Base ==================================================================================
// [$CMAC] Compiler macros    |                             | Macros for the compiler
// [$MACR] Macros             |                             | Define macros used in the library
// [$TYPE] Types              |                             | Basic types and typedefs
// [$INTR] Intrinsics         |                             | Atomics, cpuid, ticket mutex
// [$CALL] Dqn_CallSite       |                             | Source code location/tracing
// [$TMUT] Dqn_TicketMutex    |                             | Userland mutex via spinlocking atomics
// [$ALLO] Dqn_Allocator      |                             | Generic allocator interface
// [$PRIN] Dqn_Print          |                             | Console printing
// [$LLOG] Dqn_Log            |                             | Console logging macros

// NOTE: Additional Configuration
// - Override the default heap-allocation routine that is called when the
//   default Dqn_Allocator is used by #define-ing. By default we use the OS's 
//   virtual memory allocators (e.g. VirtualAlloc on Windows and mmap on Linux).
//
//     DQN_ALLOC(size)
//     DQN_DEALLOC(ptr, size)
//
// - Override the byte-manipulation routines by #define-ing. By default we use 
//   <strings.h>
//
//     DQN_MEMCPY(dest, src, count)
//     DQN_MEMSET(dest, value, count)
//     DQN_MEMCMP(lhs, rhs, count)
//     DQN_MEMMOVE(dest, src, count)
//
// - Override these math functions. By default we use <math.h>
//
//     DQN_SQRTF(val)
//     DQN_SINF(val)
//     DQN_COSF(val)
//     DQN_TANF(val)
//
// - Change the prefix to all declared functions in the library by #define-ing.
//
//     DQN_API
//
// - Apply static to all function definitions and disable external linkage to
//   other translation units by #define-ing. This macro is only used if DQN_API 
//   is not overriden.
//
//     DQN_STATIC_API
//
// - Turn all assertion macros to no-ops except for hard asserts (which are
//   always enabled and represent unrecoverable errors in the library).
//
//     DQN_NO_ASSERT
//
// - Augment DQN_CHECK(expr) macro's behaviour. By default it will trigger a
//   debugger break when when the expression evalutes false otherwise by
//   #define-ing this macro it will evaluate to false and DQN_CHECK is usually
//   used in a if branch to recover gracefully from the failed condition.
//
//     DQN_NO_CHECK_BREAK

#include "dqn_base.h"

// NOTE: Dqn_External ==============================================================================
// [$OS_H] OS Headers         |                             | Headers from the operating system
// [$STBS] stb_sprintf        |                             | Portable sprintf
#include "dqn_external.h"
#include "dqn_win32.h"

// NOTE: Additional Configuration
// - Define this to stop this library from defining a minimal subset of Win32
//   prototypes and definitions in this file. You should use this macro if you
//   intend to #include <Windows.h> yourself to avoid symbol conflicts with
//   the redefined declarations in this library.
//
//     DQN_NO_WIN32_MIN_HEADER
//
// - Define this to stop this library from defining STB_SPRINTF_IMPLEMENTATION.
//   Useful if another library uses and includes "stb_sprintf.h"
//
//     DQN_STB_SPRINTF_HEADER_ONLY

// NOTE: Dqn_Memory ================================================================================
// [$VMEM] Dqn_VMem           |                             | Virtual memory allocation
// [$MEMB] Dqn_MemBlock       |                             | Virtual memory blocks
// [$AREN] Dqn_Arena          |                             | Growing bump allocator
// [$ACAT] Dqn_ArenaCatalog   |                             | Collate, create & manage arenas in a catalog
#include "dqn_memory.h"

// NOTE: Dqn_Debug =================================================================================
// [$DEBM] Debug Macros       |                             |
// [$ASAN] Dqn_Asan           |                             | Helpers to manually poison memory using ASAN
// [$STKT] Dqn_StackTrace     |                             | Create stack traces
// [$DEBG] Dqn_Debug          |                             | Debugging tools/helpers
#include "dqn_debug.h"

// NOTE: Dqn_Strings ===============================================================================
// [$CSTR] Dqn_CStr8          |                             | C-string helpers
// [$STR8] Dqn_Str8           |                             | Pointer and length strings
// [$FSTR] Dqn_FStr8          | DQN_FSTr8                   | Fixed-size strings
// [$STRB] Dqn_Str8Builder    |                             |
// [$CHAR] Dqn_Char           |                             | Character ascii/digit.. helpers
// [$UTFX] Dqn_UTF            |                             | Unicode helpers
#include "dqn_strings.h"

// NOTE: Dqn_Containers ============================================================================
// [$CARR] Dqn_CArray         |                             | Basic operations on C arrays for VArray/SArray/FArray to reuse
// [$VARR] Dqn_VArray         | DQN_VARRAY                  | Array backed by virtual memory arena
// [$SARR] Dqn_SArray         | DQN_SARRAY                  | Array that are allocated but cannot resize
// [$FARR] Dqn_FArray         | DQN_FARRAY                  | Fixed-size arrays
// [$SLIC] Dqn_Slice          | DQN_SLICE                   | Pointe and length container of data
// [$DMAP] Dqn_DSMap          | DQN_DSMAP                   | Hashtable, 70% max load, PoT size, linear probe, chain repair
// [$LIST] Dqn_List           | DQN_LIST                    | Chunked linked lists, append only
#include "dqn_containers.h"

// NOTE: Additional Configuration
// - Override the default break into the active debugger function. By default
//   we use __debugbreak() on Windows and raise(SIGTRAP) on other platforms.
//
//     DQN_DEBUG_BREAK
//
// - Change the byte that DQN_MEMSET will clear memory with. By default this
//   is set to 0. Some of the API's in this library accept a Dqn_ZeroMem enum
//   which scrubs memory with this #define-d value.
//
//     DQN_MEMSET_BYTE
//
// - Define this macro to enable emory leak tracking when requesting memory
//   from the OS via this library. For example calls to Dqn_VMem_Reserve or
//   DQN_ALLOC are recorded to the leak table.
//
//   Allocations are stored in a global hash-table and their respective stack
//   traces for the allocation location. Memory leaks can be dumped at the end
//   of the program or some epoch by calling Dqn_Library_DumpLeaks()
//
//   You may mark sections of your program as allowed to leak memory by setting
//   the arena's or Dqn_Library's runtime struct `allocs_are_allowed_to_leak`
//   flag.
//
//     DQN_LEAK_TRACING
//
// - Define this macro to 1 to enable poisoning of memory from arenas when ASAN
//   `-fsanitize=address` is enabled. Enabling this will detect memory overwrite
//   by padding allocated before and after with poisoned memory which will raise
//   a use-after-poison in ASAN on read/write. This is a no-op if the library is
//   not compiled with ASAN.
//
//     DQN_ASAN_POISON 1
//
// - Define this macro 1 to enable sanity checks for manually poisoned memory in
//   this library when ASAN `-fsanitize=address` is enabled. These sanity checks
//   ensure that memory from arenas are correctly un/poisoned when pointers are
//   allocated and returned to the memory arena's. This is a no-op if we are not
//   compiled with ASAN or `DQN_ASAN_POISON` is not set to `1`.
//
//     DQN_ASAN_VET_POISON 1
//
// - Define this macro to the size of the guard memory reserved before and after
//   allocations made that are poisoned to protect against out-of-bounds memory
//   accesses. By default the library sets the guard to 128 bytes.
//
//     DQN_ASAN_POISON_GUARD_SIZE 128

// NOTE: Dqn_Platform ==============================================================================
// [$FSYS] Dqn_Fs             | DQN_FS                      | Filesystem helpers
// [$DATE] Dqn_Date           |                             | Date-time helpers
// [$WIND] Dqn_Win            |                             | Windows OS helpers
// [$WINN] Dqn_WinNet         | DQN_WINNET                  | Windows internet download/query helpers
// [$OSYS] Dqn_OS             | DQN_WIN                     | Operating-system APIs
// [$TCTX] Dqn_ThreadContext  |                             | Per-thread data structure e.g. temp arenas
#include "dqn_platform.h"

// NOTE: Dqn_OS ====================================================================================
// [$EXEC] Dqn_OSExec         |                             | Execute programs programatically
#include "dqn_os.h"

// NOTE: Dqn_Math ==================================================================================
// [$VEC2] Dqn_V2, V2i        | DQN_V2                      |
// [$VEC3] Dqn_V3, V3i        | DQN_V3                      |
// [$VEC4] Dqn_V4, V4i        | DQN_V4                      |
// [$MAT4] Dqn_M4             | DQN_M4                      |
// [$RECT] Dqn_Rect           | DQN_RECT                    |
// [$MATH] Other              |                             |
#include "dqn_math.h"

// NOTE: Dqn_Hash ==================================================================================
// [$FNV1] Dqn_FNV1A          |                             | Hash(x) -> 32/64bit via FNV1a
// [$MMUR] Dqn_MurmurHash3    |                             | Hash(x) -> 32/128bit via MurmurHash3
#include "dqn_hash.h"

// NOTE: Dqn_Helpers ===============================================================================
// [$PCG3] Dqn_PCG32          |                             | RNG from the PCG family
// [$JSON] Dqn_JSONBuilder    | DQN_JSON_BUILDER            | Construct json output
// [$BHEX] Dqn_Bin            | DQN_BIN                     | Binary <-> hex helpers
// [$BSEA] Dqn_BinarySearch   |                             | Binary search
// [$BITS] Dqn_Bit            |                             | Bitset manipulation
// [$SAFE] Dqn_Safe           |                             | Safe arithmetic, casts, asserts
// [$MISC] Misc               |                             | Uncategorised helper functions
// [$DLIB] Dqn_Library        |                             | Globally shared runtime data for this library
// [$PROF] Dqn_Profiler       | DQN_PROFILER                | Profiler that measures using a timestamp counter
#include "dqn_helpers.h"
#endif // DQN_H

#if defined(DQN_IMPLEMENTATION)
#include "dqn_base.cpp"
#include "dqn_external.cpp"
#include "dqn_memory.cpp"
#include "dqn_debug.cpp"
#include "dqn_strings.cpp"
#include "dqn_containers.cpp"
#include "dqn_platform.cpp"
#include "dqn_os.cpp"
#include "dqn_math.cpp"
#include "dqn_hash.cpp"
#include "dqn_helpers.cpp"
#endif // DQN_IMPLEMENTATION
