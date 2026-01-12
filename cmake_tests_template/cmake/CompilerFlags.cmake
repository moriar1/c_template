# ==== Abstract Interface Libraries for Flag Groups ====
# Usage: target_link_libraries(your_target PRIVATE compiler_flags_group_name)

# ---- Common Warnings & Security (GCC and Clang) ----
add_library(compiler_options_warnings INTERFACE)
target_compile_options(
  compiler_options_warnings
  INTERFACE -fPIE
            -fcf-protection=full
            -fstack-protector-strong
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
            -Wmissing-variable-declarations
            -Wformat=2
            -Wstrict-overflow=4
            -Wswitch-enum
            -Wmissing-include-dirs
            # Security as errors
            -Werror=format-security
            -Werror=incompatible-pointer-types
            -Wno-pre-c11-compat # because of Unity
)

# ---- Common Linker Hardening ----
add_library(linker_options_security INTERFACE)
target_link_options(
  linker_options_security
  INTERFACE
  -pie
  -Wl,-z,noexecstack
  -Wl,-z,relro
  -Wl,-z,now
  -Wl,--as-needed
  -Wl,--no-copy-dt-needed-entries
  -Wl,-z,nodlopen
  -Wl,-z,defs)

# ---- Clang Specific Flags ----
add_library(compiler_options_clang INTERFACE)
target_compile_options(
  compiler_options_clang
  INTERFACE $<$<C_COMPILER_ID:Clang>:
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
            -Wthread-safety
            -Wthread-safety-beta
            -Weverything
            -Wno-padded
            -Wno-implicit-void-ptr-cast
            -Wno-unsafe-buffer-usage
            -Wno-declaration-after-statement
            -Wno-conditional-uninitialized
            -Wno-covered-switch-default
            -Wno-disabled-macro-expansion
            >)

# ---- GCC Specific Flags ----
add_library(compiler_options_gcc INTERFACE)
target_compile_options(
  compiler_options_gcc
  INTERFACE $<$<C_COMPILER_ID:GNU>:
            -fdiagnostics-color=always
            -Wstringop-overflow=4
            -Wduplicated-cond
            -Wduplicated-branches
            -Wlogical-op
            >)

# ---- LLD Linker ----
include(CheckLinkerFlag)
check_linker_flag(C "-fuse-ld=lld" HAVE_LLD_LINKER)
add_library(linker_options_lld INTERFACE)
target_link_options(
  linker_options_lld INTERFACE
  $<$<AND:$<C_COMPILER_ID:Clang>,$<BOOL:${HAVE_LLD_LINKER}>>:-fuse-ld=lld>)

# ---- Configuration: Debug ----
add_library(config_debug INTERFACE)
target_compile_options(config_debug INTERFACE $<$<CONFIG:Debug>: -g3 -Og
                                              -DDEBUG >)
target_link_options(config_debug INTERFACE
                    $<$<CONFIG:Debug>:-Wl,--export-dynamic>)

# ---- Configuration: Release ----
add_library(config_release INTERFACE)
target_compile_options(
  config_release
  INTERFACE $<$<CONFIG:Release>:
            -O3
            -DNDEBUG
            -march=native
            -mtune=native
            -flto
            -ffunction-sections
            -fdata-sections
            >)
# GCC Extra Optimizations
target_compile_options(
  config_release INTERFACE $<$<AND:$<C_COMPILER_ID:GNU>,$<CONFIG:Release>>:
                           -fipa-pta -fbranch-target-load-optimize >)
# Clang Extra Optimizations
target_compile_options(
  config_release
  INTERFACE $<$<AND:$<C_COMPILER_ID:Clang>,$<CONFIG:Release>>:-mretpoline>)
target_link_options(config_release INTERFACE $<$<CONFIG:Release>:-flto
                    -Wl,--gc-sections>)

# ---- Configuration: RelWithDebInfo ----
add_library(config_relwithdebinfo INTERFACE)
target_compile_options(
  config_relwithdebinfo
  INTERFACE $<$<CONFIG:RelWithDebInfo>:
            -O2
            -g
            -DNDEBUG
            -march=native
            -mtune=native
            -flto
            -ffunction-sections
            -fdata-sections
            >)
target_link_options(config_relwithdebinfo INTERFACE
                    $<$<CONFIG:RelWithDebInfo>:-flto -Wl,--gc-sections>)

# ==== This library collects all flags into one convenient package. ====

add_library(project_compile_options INTERFACE)

# Base Warnings
target_link_libraries(
  project_compile_options INTERFACE compiler_options_warnings
                                    compiler_options_clang compiler_options_gcc)

# Config specific flags (Debug, Release, etc.)
target_link_libraries(
  project_compile_options INTERFACE config_debug config_release
                                    config_relwithdebinfo)

# Security Definitions (Fortify Source) NOTE: do not use with sanitizers and
# without -O1 -O2 -O3 flags
target_compile_definitions(
  project_compile_options
  INTERFACE
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:_FORTIFY_SOURCE=3>)

add_library(project_linker_options INTERFACE)
target_link_libraries(
  project_linker_options
  INTERFACE linker_options_security
            linker_options_lld
            config_debug # Contains Debug link flags
            config_release # Contains Release link flags
            config_relwithdebinfo)

# ==== Sanitizers ====
option(
  ENABLE_SANITIZERS
  "Enable AddressSanitizer and UndefinedBehaviorSanitizer (Debug/RelWithDebInfo only)"
  ON)

if(ENABLE_SANITIZERS)
  add_library(sanitizer_options INTERFACE)
  set(SAN_FLAGS
      -fsanitize=address
      # -fsanitize=thread
      -fsanitize=undefined
      -fno-omit-frame-pointer
      -fsanitize=float-divide-by-zero
      -fsanitize=float-cast-overflow)
  target_compile_options(
    sanitizer_options
    INTERFACE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:${SAN_FLAGS}>)
  target_link_options(
    sanitizer_options INTERFACE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:${SAN_FLAGS}>)
  # Use _FORTIFY_SOURCE=0 target_compile_definitions(sanitizer_options INTERFACE
  # $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:_FORTIFY_SOURCE=0> )
  message(STATUS "Sanitizers interface library 'sanitizer_options' created.")
endif()

# ==== Apply to Main Project ====
target_link_libraries(
  ${PROJECT_NAME} PRIVATE project_compile_options project_linker_options
                          sanitizer_options)
