# Requirements

- Unix-like OS
- clang
- clang-format
- clang-tidy
- lld
- git
- ninja
- cmake

**Debian**:

```sh
sudo apt update && sudo apt upgrade -y
sudo apt install -y git clang clangd clang-tidy clang-format llvm-19 cmake ninja-build build-essential
```

**Arch Linux**:

```sh
sudo pacman -Syu
sudo pacman -S git clang lld ninja cmake base-devel llvm20 
```

**FreeBSD**:

```sh
sudo pkg update && sudo pkg upgrade -y
sudo pkg install -y git cmake ninja llvm21
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

# Tests
cmake --preset clang-debug
cmake --build --preset clang-debug
ctest --preset clang-debug
```

# TODO


## Macros

(see ./misc/custom.h): https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c
- Make functions (fprintf) thread safe using queue, print logs when exit().
- Add or replace MALLOC with try_malloc and xmalloc

```c
static inline void *try_malloc(size_t n) {
    return malloc(n);
}
#define TRY_MALLOC(ptr, size) do { ptr = try_malloc(size); if (!ptr) return false; } while(0)
```

- test WRAPPER for pthread_create

```c
#define DEFINE_THREAD_WRAPPER(func_name, data_type) \
    void *func_name##_wrapper(void *arg) { \
        data_type *data = (data_type *)arg; \
        func_name(data); \
        return NULL; \
    }

DEFINE_THREAD_WRAPPER(mystruct_sum_print_impl, MyStruct)
```

## Cmake

- use `find_package(LLVM REQUIRED CONFIG)` `LLVM_PACKAGE_VERSION` in clang-foramt target

- FetchContent example
- targets: doc, uninstall
- CTest
- CI/CD
- clang-tidy: https://github.com/jenisys/explore.clang-tidy

https://gitlab.kitware.com/cmake/community/-/wikis/FAQ

<!-- https://habr.com/ru/companies/pvs-studio/articles/708138/ -->

<!-- https://github.com/cpm-cmake/CPM.cmake -->

<!-- https://github.com/friendlyanon/cmake-init -->

<!-- (CMakePresets) https://github.com/friendlyanon/cmake-init-executable -->

<!-- https://github.com/TheLartians/ModernCppStarter -->

<!-- https://github.com/pananton/cpp-lib-template -->

<!-- https://github.com/cginternals/cmake-init -->

<!-- ## Compilers -->

<!-- https://clang.llvm.org/docs/UsersManual.html -->

<!-- https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html -->
