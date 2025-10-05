# Requirements

- Unix-like OS
- git
- clang
- ldd
- ninja
- cmake

# Usage

```sh
git clone https://github.com/moriar1/c_template
cp -ir c_template/cmake_template MyProject
cd MyProject
cmake --preset clang-debug
cmake --build --preset clang-debug
./build/clang-debug/MyProject
```

# TODO

## Cmake

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

### gcc
```cmake
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
target_compile_options(${PROJECT_NAME} PRIVATE -fcolor-diagnostics
-Weverything) elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
target_compile_options(${PROJECT_NAME} PRIVATE -fdiagnostics-color=always)
endif()
```
