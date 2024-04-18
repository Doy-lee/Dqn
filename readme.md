# Dqn

My personal standard library that provides allocator aware data structures,
custom memory allocators and various miscellaneous helpers for prototyping. The
library is a unity-build style library where data structures and functions are
separated by category into files for organisation. You only need to include
`dqn.h` which amalgamates all the files into one translation unit.

## Build

To build with this library, copy all the `*.[h|cpp]` files at the root of the
repository to your desired location and compile `dqn.cpp` or include it into one
of your translation units.

Finally ensure that the compiler has in its search paths for the include
directory where headers are located, e.g. `-I <path/to/dqn/headers>`.

## Customisation

The headers provide macros to compile out sections that are not needed. This can
be useful to speed up compile times or reduce binary size if you only need
a particular part of the library. Each header contains a table-of-contents that
denotes the macro to define to disable that section that should be defined
before the header include.

```cpp
#define DQN_NO_VARRAY       // Disable virtual array container
#define DQN_NO_JSON_BUILDER // Disable the JSON string builder
#include "dqn.h"
```
