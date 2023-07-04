# Dqn

Personal standard library that provides allocator aware data structures, custom
memory allocators and various miscellaneous helpers for prototyping. The library
is a unity-build style library where data structures and functions are separated
by category into files for organisation. You only need to include `dqn.h` which
amalgamates all the files into one translation unit.

## Build

To build with this library, copy all the `*.[h|cpp]` files at the root of the
repository and in one header file,

```cpp
#include "dqn.h"
```

Which includes all other files and their declaration into your header. In one
`.cpp` file defined the implementation macro to enable the implementation of the
header in that translation unit,

```cpp
#define DQN_IMPLEMENTATION
#include "dqn.h"
```

Ensure that the folder containing the files is part of the include search path
for the compiler for the amalgamated `dqn.h` to successfully locate the files.
