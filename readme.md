# Dqn
Personal utility library.

## DqnHeader
A simple C++ introspection metaprogram designed as a prebuild step and generates a summary of function prototypes and comments in a header file based on annotations. It is not designed to be used as a header file for compilation and will most likely fail syntax rules if tried.

The generated file is written to stdout.

### Build
Build DqnHeader by defining `DQN_HEADER_IMPLEMENTATION` before compiling and execute it as follows

`DqnHeader.exe SourceCode.h > GeneratedFile.h`

### Usage
Include `DqnHeader.h` in a file and use the macros, annotations as described.
  * Extract function prototypes using the `DQN_HEADER_COPY_PROTOTYPE` macro
  * Copy comments by writing comments with `// @` as the prefix
  * Copy many lines of code by enclosing it in `DQN_HEADER_COPY_BEGIN` and `DQN_HEADER_COPY_END` macros

```
#include "DqnHeader.h"

// @ ptr1: Pointer to the first block of memory
// @ ptr2: Pointer to the second block of memory
// @ num_bytes: The number of bytes to compare in both blocks of memory
DQN_HEADER_COPY_PROTOTYPE(int, Dqn_MemCmp(void const *ptr1, void const *ptr2, size_t num_bytes))
{
    int result = memcmp(ptr1, ptr2, num_bytes);
    return result;
}

DQN_HEADER_COPY_BEGIN
struct HelloWorld
{
    int foo, bar;
};
DQN_HEADER_COPY_END

```

Which generates the following output

```
// ptr1: Pointer to the first block of memory
// ptr2: Pointer to the second block of memory
// num_bytes: The number of bytes to compare in both blocks of memory
int Dqn_MemCmp(void const *ptr1, void const *ptr2, size_t num_bytes);
struct HelloWorld
{
    int foo, bar;
};
```

