# ---- Compiler Hardening and Warning Flags ----

# Flags for GCC and Clang (both release and debug)
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
    -Wthread-safety
    -Wthread-safety-beta
    -Weverything
    -Wno-padded # struct padding
    -Wno-implicit-void-ptr-cast # malloc etc.
    -Wno-unsafe-buffer-usage
    -Wno-declaration-after-statement
    -Wno-conditional-uninitialized
    -Wno-covered-switch-default
    -Wno-disabled-macro-expansion)

set(GCC_FLAGS -fdiagnostics-color=always -Wstringop-overflow=4
              -Wduplicated-cond -Wduplicated-branches -Wlogical-op)
# -fanalyzer -ftrivial-auto-var-init=zero

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

# Check lld linker availability for clang
include(CheckLinkerFlag)
check_linker_flag(C "-fuse-ld=lld" HAVE_LLD_LINKER)
set(USE_LLD_LINKER
    $<$<AND:$<C_COMPILER_ID:Clang>,$<BOOL:${HAVE_LLD_LINKER}>>:ON>)

# ---- Configuration-specific Flags ----

# Debug
set(DEBUG_FLAGS -g3 -Og -DDEBUG) # -O0
set(DEBUG_LINK_FLAGS "-Wl,--export-dynamic")

# Release
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

# RelWithDebInfo
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

# ---- Apply Compiler and Linker Flags ----

target_compile_options(
  ${PROJECT_NAME} PRIVATE $<$<C_COMPILER_ID:GNU>:${GCC_FLAGS}>
                          $<$<C_COMPILER_ID:Clang>:${CLANG_FLAGS}>)

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
# NOTE: do not use with sanitazers and without -O1 -O2 -O3 flags
target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:_FORTIFY_SOURCE=3>)

# ---- Sanitizers ----
include(Sanitizers)
