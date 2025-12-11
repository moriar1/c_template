# Requirements

- Unix-like OS
- git
- clang
- lld
- ninja
- cmake

**Debian/Ubuntu**:

```sh
sudo apt update && sudo apt upgrade -y
sudo apt install -y git clang clangd clang-tidy clang-format cmake ninja-build build-essential
```

# Usage

## CMakePresets
```sh
git clone https://github.com/moriar1/c_template
cp -ir c_template/cmake_template MyProject
cd MyProject
cmake --preset clang-debug
cmake --build --preset clang-debug
./build/clang-debug/MyProject
```

## CMake
 
```sh
git clone https://github.com/moriar1/c_template
cp -ir c_template/cmake_template MyProject
cd MyProject
md build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_C_COMPILER=clang -DCMAKE_C_CLANG_TIDY=clang-tidy -G Ninja -B build -S .
cmake --build build
./build/MyProject

# Optional:

# for clangd
ln -s build/compile_commands.json .

# custom targets
cmake --build build --target run-tidy-full
cmake --build build --target build-info
cmake --build build --target check-format
cmake --build build --target format
```

# TODO

## Macros

- Define debug output (see ./misc/custom.h): https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c

```c
#ifndef NDEBUG

// NOTE: always needs at least 1 arg (you can use NULL as dummy argument)
#define DEBUG_PRINT(fmt, ...)                                                  \
  do {                                                                         \
    fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __func__, __LINE__,            \
            __VA_ARGS__);                                                      \
  } while (0)
#else

#define DEBUG_PRINT(fmt, ...) ((void)0)

#endif
```

or

```c
#ifndef NDEBUG

#define DEBUG_PRINTF(fmt, ...)                                                 \
  do {                                                                         \
    fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __func__, __LINE__,            \
            __VA_ARGS__);                                                      \
  } while (0)
#define DEBUG_PUTS(msg)                                                        \
  do {                                                                         \
    fprintf(stderr, "[DEBUG] %s:%d: " msg "\n", __func__, __LINE__);           \
  } while (0)
#else

#define DEBUG_PRINT(fmt, ...) ((void)0)
#define DEBUG_PRINT_MSG(msg) ((void)0)

#endif
```

or in C23 with errno in header

```c
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifndef NDEBUG

#define DEBUG_PRINT(fmt, ...)                                                  \
  do {                                                                         \
    fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __func__,                      \
            __LINE__ __VA_OPT__(, ) __VA_ARGS__);                              \
  } while (0)

#define DEBUG_PRINT_ERRNO(fmt, ...)                                            \
  do {                                                                         \
    fprintf(stderr, "[ERROR] %s:%d: " fmt " (errno: %s)\n", __func__,          \
            __LINE__ __VA_OPT__(, ) __VA_ARGS__, strerror(errno));             \
  } while (0)
#else

#define DEBUG_PRINT(fmt, ...) ((void)0)
#define DEBUG_PRINT_ERRNO(fmt, ...) ((void)0)

#endif // NDEBUG

#endif // DEBUG_H
```

## Cmake

- `find_package(LLVM REQUIRED CONFIG)`
- `LLVM_PACKAGE_VERSION`

- other os examples
- FetchContent example
- targets: doc, uninstall
- CTest
- CI/CD
- clang-tidy: https://github.com/jenisys/explore.clang-tidy

https://gitlab.kitware.com/cmake/community/-/wikis/FAQ

https://habr.com/ru/companies/pvs-studio/articles/708138/

https://github.com/cpm-cmake/CPM.cmake

https://github.com/friendlyanon/cmake-init

(CMakePresets) https://github.com/friendlyanon/cmake-init-executable

https://github.com/TheLartians/ModernCppStarter

https://github.com/pananton/cpp-lib-template

https://github.com/cginternals/cmake-init

## Compilers

https://clang.llvm.org/docs/UsersManual.html

https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html
