# Merkle Patricia tree
![Testing](https://github.com/Azazel-h/merkle-patricia-tree/actions/workflows/cmake-multi-platform.yml/badge.svg) [![codecov](https://codecov.io/gh/Azazel-h/merkle-patricia-tree/graph/badge.svg?token=XJEFCWDOQK)](https://codecov.io/gh/Azazel-h/merkle-patricia-tree)
```bash
$ git clone https://github.com/Azazel-h/merkle-patricia-tree.git --recursive
```
Updating submodules with:
```bash
$ git submodule update --init
```

### Clean building
```bash
$ cmake -S . -B build
$ cmake --build build
```
> [!NOTE]
> If you want to build with tests just add the flag
> ```bash
> $ cmake -S . -B build -DTRIE_BUILD_TESTING=ON
> $ cmake --build build
> ```

> [!IMPORTANT]
> Be **careful**, if you are using CLion or other IDE with _СMake/CTest_ integration,
> run the **mpt-tests** to build them before using CTest.

You can run tests with smth like
```bash
$ ./build/mpt-tests
```
Or using CTest
```bash
$ ctest --verbose -j$(nproc) --test-dir build
```

### C-wrappers

> [!CAUTION]
> Was generated by AI!! Not tested at all. WIP for now.
#### Language-Linkage Example
##### - CMake
```
test-language-linkage/
├── lib/
│   └── merkle-patricia-tree/
│       └── ...
├── CMakeLists.txt
└── main.c
```
```cmake 
# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(test-language-linkage C CXX)

# Add the C++ library
add_subdirectory(lib/merkle-patricia-tree)

# Add the C executable
add_executable(main main.c)
target_link_libraries(main PRIVATE
        merkle-patricia-tree
        keccak
        intx
)
target_include_directories(main PRIVATE lib/merkle-patricia-tree/include)
```
```c
// main.c
#include <stdio.h>
#include "merkle-patricia-tree/trie/hash_builder.hpp"

int main() {
printf("%s", "Hello World");
silkworm_HashBuilder* builder = silkworm_HashBuilder_new();
printf("%p", builder);
return 0;
}
```
##### - Manually :)
```bash
    Sorry, I hate bash and compiling things with commands cause I'm a bit dummy
```