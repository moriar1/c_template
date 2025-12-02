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

# ---- Compiler Standard ----
# set_target_properties(
#   ${PROJECT_NAME}
#   PROPERTIES C_STANDARD 17
#              C_STANDARD_REQUIRED ON
#              C_EXTENSIONS OFF)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD_REQUIRED ON
C_EXTENSIONS OFF)

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
    # -fcf-protection
    -fcf-protection=full
    -fstack-protector-strong
    # -fstack-protector-all
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
    -Wbad-function-cast
    -Wshift-count-overflow
    -Wshift-count-negative
    -Wswitch-default
    -Wenum-conversion
    -Wstack-protector
    -Walloca
    -Wimplicit-fallthrough
    -Wnull-dereference
    -Wformat=2
    -Wformat-signedness
    -Wstrict-overflow=4
    -Wswitch-enum
    -Wmissing-include-dirs
    # Security as errors -Werror
    -Werror=format-security
    -Werror=incompatible-pointer-types)

set(CLANG_FLAGS
    -fcolor-diagnostics
    -Wunreachable-code-aggressive
    -Wgnu-zero-variadic-macro-arguments
    -Wshorten-64-to-32
    -Wassign-enum
    -Warray-bounds-pointer-arithmetic
    -Wloop-analysis
    -Wtautological-constant-in-range-compare
    -Wformat-type-confusion
    -Widiomatic-parentheses
    # -Wthread-safety -Wthread-safety-beta
    -Weverything
    -Wno-unsafe-buffer-usage
    -Wno-declaration-after-statement
    -Wno-conditional-uninitialized
    -Wno-covered-switch-default
    -Wno-disabled-macro-expansion)

set(GCC_FLAGS
    -fdiagnostics-color=always
    # -fanalyzer -ftrivial-auto-var-init=zero
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
set(USE_LLD_LINKER
    $<$<AND:$<C_COMPILER_ID:Clang>,$<BOOL:${HAVE_LLD_LINKER}>>:ON>)


# ---- Configuration: Release, Debug, RelWithDebInfo ----
set(DEBUG_FLAGS -g3 -Og -DDEBUG) # -O0
set(DEBUG_LINK_FLAGS "-Wl,--export-dynamic")

# ---- Compiler-specific optimization flags ----
set(RELEASE_FLAGS
    -O3
    -DNDEBUG
    -march=native
    -mtune=native
    -flto
    -ffunction-sections
    -fdata-sections)
set(GCC_RELEASE_OPT_FLAGS -fipa-pta -fbranch-target-load-optimize)

set(CLANG_RELEASE_OPT_FLAGS # Security
    -mretpoline # alternative to GCC's thunks
)

set(RELEASE_LINK_FLAGS -flto -Wl,--gc-sections)
# -Wl,--build-id # -Wl,--icf=all

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

set(SANITIZER_FLAGS
    -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
    -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow)
# -fsanitize=shadow-call-stack #-fsanitize=thread

# ---- Apply Compiler and Linker Flags ----

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<C_COMPILER_ID:GNU>:${GCC_FLAGS}>
    $<$<C_COMPILER_ID:Clang>:${CLANG_FLAGS}>
)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
    ${COMMON_FLAGS}
    # Compiler-specific optimization flags for Release and RelWithDebInfo builds
    $<$<AND:$<C_COMPILER_ID:GNU>,$<IN_LIST:$<CONFIG>,Release;RelWithDebInfo>>:${GCC_RELEASE_OPT_FLAGS}>
    $<$<AND:$<C_COMPILER_ID:Clang>,$<IN_LIST:$<CONFIG>,Release;RelWithDebInfo>>:${CLANG_RELEASE_OPT_FLAGS}>
    # General configuration flags
    $<$<CONFIG:Release>:${RELEASE_FLAGS}>
    $<$<CONFIG:Debug>:${DEBUG_FLAGS}>
    $<$<CONFIG:RelWithDebInfo>:${RELWITHDEBINFO_FLAGS}>)

target_link_options(
  ${PROJECT_NAME}
  PRIVATE
  ${COMMON_LINK_FLAGS}
  # Use lld for clang
  $<$<BOOL:${USE_LLD_LINKER}>:-fuse-ld=lld>
  # General
  $<$<CONFIG:Release>:${RELEASE_LINK_FLAGS}>
  $<$<CONFIG:Debug>:${DEBUG_LINK_FLAGS}>
  $<$<CONFIG:RelWithDebInfo>:${RELWITHDEBINFO_LINK_FLAGS}>)

# ---- Fortify Source ----
# NOTE: do not use with sanitazers and without -O1 -O2 -O3 opts
target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:_FORTIFY_SOURCE=3>)

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

# ---- clang-tidy (runs always on all files) ----
# Usage: cmake --build build --target run-tidy-manual
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "src/*.c" "include/*.h")
add_custom_target(
  run-tidy-manual
  COMMAND clang-tidy -p ${CMAKE_BINARY_DIR} ${SOURCE_FILES}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Running clang-tidy manually on source files")

# ---- Show build info ----
# Usage: cmake --build build --target build-info
add_custom_target(build-info)
add_custom_command(
  TARGET build-info
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E env CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
          ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/build_info.cmake)
file(
  GENERATE
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_info.cmake
  CONTENT
    "
message(STATUS \"======================================\")
message(STATUS \"Build type: $<CONFIG>\")
message(STATUS \"Compiler: ${CMAKE_C_COMPILER_ID} (${CMAKE_C_COMPILER_VERSION})\")
message(STATUS \"Linker: ${CMAKE_LINKER}\")
message(STATUS \"Compile options:\")
message(STATUS \"$<TARGET_PROPERTY:${PROJECT_NAME},COMPILE_OPTIONS>\")
message(STATUS \"Link options:\")
message(STATUS \"$<TARGET_PROPERTY:${PROJECT_NAME},LINK_OPTIONS>\")
message(STATUS \"======================================\")
")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
