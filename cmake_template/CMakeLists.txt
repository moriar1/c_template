cmake_minimum_required(VERSION 3.22)

project(
  MyProject
  VERSION 0.1.0
  DESCRIPTION "C Project"
  LANGUAGES C)

# ---- Sources and Includes ----
set(SOURCES src/main.c)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)
# file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.c)

# ---- Compiler Standard ----
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES C_STANDARD 17
             C_STANDARD_REQUIRED ON
             C_EXTENSIONS OFF)
# target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)

# ---- Inject Project Version as a Preprocessor Definition ----
target_compile_definitions(${PROJECT_NAME}
                           PRIVATE PROJECT_VERSION=\"${PROJECT_VERSION}\")

# Add POSIX functions for Linux
target_compile_definitions(
  ${PROJECT_NAME} PRIVATE $<$<PLATFORM_ID:Linux>:_POSIX_C_SOURCE=200809L>)

# ---- Compiler Hardening and Warning Flags ----

# Flags for GCC and Clang (release and debug)
set(COMMON_FLAGS
    # Security and Safety
    -fPIE
    -fcf-protection
    -fcf-protection=full
    -fstack-protector-strong # -all
    -fstack-clash-protection
    -fstrict-flex-arrays=3
    -fno-common
    # Warning and style checks
    -Wall
    -Wextra
    -Wpedantic
    -Wmissing-prototypes
    -Wstrict-prototypes
    -Wold-style-definition
    -Wshadow
    -Wconversion
    -Wsign-conversion
    -Wfloat-equal
    -Wdouble-promotion
    -Wcast-qual
    -Wpointer-arith
    -Wredundant-decls
    -Wwrite-strings
    -Wtype-limits
    -Wundef
    -Wunused
    -Wcast-align
    -Wstack-protector
    -Walloca
    -Wimplicit-fallthrough
    -Wnull-dereference
    -Wformat=2
    -Wformat-signedness
    -Wstrict-overflow=2
    # -Wjump-misses-init
    -Wswitch-enum
    -Wmissing-include-dirs
    # Security as errors -Werror
    -Werror=format-security
    -Werror=incompatible-pointer-types)

set(CLANG_ONLY_FLAGS
    -fcolor-diagnostics
    -Wunreachable-code-aggressive
    -Wgnu-zero-variadic-macro-arguments
    # -Wthread-safety
    -Weverything
    -Wno-unsafe-buffer-usage
    -Wno-declaration-after-statement
    -Wno-conditional-uninitialized
    -Wno-covered-switch-default
    -Wno-disabled-macro-expansion)

set(GCC_ONLY_FLAGS
    -fdiagnostics-color=always
    # -fanalyzer # FIXME: No need in release build -ftrivial-auto-var-init=zero
    -Wstringop-overflow=4 -Wduplicated-cond -Wduplicated-branches -Wlogical-op)

# ---- Common Linker Flags ----
set(COMMON_LINK_FLAGS
    -pie
    -Wl,-z,noexecstack
    -Wl,-z,relro
    -Wl,-z,now
    -Wl,--as-needed
    -Wl,--no-copy-dt-needed-entries
    -Wl,-z,nodlopen
    -Wl,-z,defs)
# Use lld linker if present.
include(CheckLinkerFlag)
check_linker_flag(C "-fuse-ld=lld" HAVE_LLD_LINKER)
if(HAVE_LLD_LINKER)
  list(APPEND COMMON_LINK_FLAGS "-fuse-ld=lld")
endif()

# ---- Configuration: Release, Debug, RelWithDebInfo ----
set(DEBUG_FLAGS -g -O0 -DDEBUG)
set(DEBUG_LINK_FLAGS "")

set(RELEASE_FLAGS
    -O3
    -DNDEBUG
    -march=native
    -mtune=native
    -flto
    -ffunction-sections
    -fdata-sections)
set(RELEASE_LINK_FLAGS -flto -Wl,--gc-sections)

set(RELWITHDEBINFO_FLAGS
    -O2
    -g
    -DNDEBUG
    -march=native
    -mtune=native
    -flto
    -ffunction-sections
    -fdata-sections)
set(RELWITHDEBINFO_LINK_FLAGS ${RELEASE_LINK_FLAGS})

set(SANITIZER_FLAGS -fsanitize=address -fsanitize=undefined
                    -fno-omit-frame-pointer)

# ---- Apply Compiler and Linker Flags ----
# TODO?: use INTERFACE libraries for many targets
target_compile_options(
  ${PROJECT_NAME}
  PRIVATE $<$<C_COMPILER_ID:GNU>:${GCC_ONLY_FLAGS}>
          $<$<C_COMPILER_ID:Clang>:${CLANG_ONLY_FLAGS}>
          ${COMMON_FLAGS}
          $<$<CONFIG:Release>:${RELEASE_FLAGS}>
          $<$<CONFIG:Debug>:${DEBUG_FLAGS}>
          $<$<CONFIG:RelWithDebInfo>:${RELWITHDEBINFO_FLAGS}>)

target_link_options(
  ${PROJECT_NAME}
  PRIVATE
  ${COMMON_LINK_FLAGS}
  $<$<CONFIG:Release>:${RELEASE_LINK_FLAGS}>
  $<$<CONFIG:Debug>:${DEBUG_LINK_FLAGS}>
  $<$<CONFIG:RelWithDebInfo>:${RELWITHDEBINFO_LINK_FLAGS}>)

# ---- Fortify Source ----
# target_compile_definitions(${PROJECT_NAME} PRIVATE
# $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:_FORTIFY_SOURCE=3> )

# ---- Sanitizers ----
option(ENABLE_SANITIZERS
       "Enable address,undefined behavior sanitizers in Debug/RelWithDebInfo"
       ON)
if(ENABLE_SANITIZERS)
  target_compile_options(
    ${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:${SANITIZER_FLAGS}>
                            $<$<CONFIG:RelWithDebInfo>:${SANITIZER_FLAGS}>)
  target_link_options(
    ${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:${SANITIZER_FLAGS}>
    $<$<CONFIG:RelWithDebInfo>:${SANITIZER_FLAGS}>)
endif()

# ---- Libraries ----
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(Threads REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads m)
endif()

# ---- Install ----
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
