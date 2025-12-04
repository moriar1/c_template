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

## Cmake

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
