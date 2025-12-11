MAKEFLAGS += --no-print-directory

# ========================== Project Configuration ===========================

ARGS         ?=
SRC_DIR      ?= src
INCLUDE_DIR  ?= include
TARGET       ?= myapp
BUILD        ?= debug
INSTALL_DIR  ?= $(HOME)/.local/bin
CC           ?= clang

# --- Paths ---
BUILD_ROOT   ?= build
OBJ_DIR      := $(BUILD_ROOT)/$(BUILD)/obj
BIN_DIR      := $(BUILD_ROOT)/$(BUILD)/bin
OBJ_SUBDIR   := $(OBJ_DIR)
BIN_OUTPUT   := $(BIN_DIR)/$(TARGET)

# --- Source Files ---
SRCS         := $(shell find $(SRC_DIR) -name '*.c')
HEADERS      := $(shell find $(INCLUDE_DIR) -name '*.h')

# --- Compiler Flags ---
CFLAGS_COMMON ?= \
	-std=c17 -D_POSIX_C_SOURCE=200112L -I$(INCLUDE_DIR) -fcolor-diagnostics \
    -fPIE -fcf-protection -fstack-clash-protection -fstrict-flex-arrays=3 \
    \
	-Weverything -Wall -Wextra -Wpedantic \
    -Wmissing-prototypes -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Wold-style-definition -Wfloat-equal \
    -Wdouble-promotion -Wcast-qual -Wpointer-arith -Wredundant-decls -Wwrite-strings -Wtype-limits -Wundef \
    -Wempty-body -Wdisabled-optimization -Wnull-dereference -Wvla -Wformat=2 -Wimplicit-fallthrough -Wcast-align \
    -Wunused -Wstack-protector -Walloca -fno-common \
    -Werror=format-security -Werror=incompatible-pointer-types \
    \
    -Wno-unsafe-buffer-usage -Wno-declaration-after-statement -Wno-conditional-uninitialized \
    -Wno-covered-switch-default -Wno-cast-align \
    # -Werror 

CFLAGS_DEBUG    ?= -O0 -DDEBUG -g
CFLAGS_RELEASE  ?= -O3 -DNDEBUG -march=native -mtune=native -flto -ffunction-sections -fdata-sections 

# --- Link Flags ---
LDFLAGS_COMMON  = -pie -fuse-ld=lld -Wl,-z,nodlopen -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--as-needed -Wl,--no-copy-dt-needed-entries
LDFLAGS_RELEASE = -flto -Wl,--gc-sections
LDFLAGS_DEBUG   =

# --- Version ---
VERSION := $(shell git describe --tags --always 2>/dev/null || echo "unknown")
CFLAGS_COMMON += -DPROJECT_VERSION=\"$(VERSION)\"

# --- Sanitizers ---
SAN_FLAGS :=
ifeq ($(BUILD),debug)
  SANITIZE ?= address,undefined
# ,leak,thread
  ifneq ($(SANITIZE),)
    SAN_FLAGS := -fsanitize=$(SANITIZE) -fno-omit-frame-pointer
  endif
endif

# --- Flags Combination Logic ---
CFLAGS := $(CFLAGS_COMMON)
ifeq ($(BUILD),release)
	CFLAGS += $(CFLAGS_RELEASE)
	LDFLAGS = $(LDFLAGS_COMMON) $(LDFLAGS_RELEASE)
else
	CFLAGS += $(CFLAGS_DEBUG)
	LDFLAGS = $(LDFLAGS_COMMON) $(LDFLAGS_DEBUG)
endif
LDFLAGS += $(SAN_FLAGS)

OBJS     := $(patsubst $(SRC_DIR)/%.c, $(OBJ_SUBDIR)/%.o, $(SRCS))
DEPS     := $(OBJS:.o=.d)

# ======================= Libraries ====================================
# GSL
# GSL_PREFIX    ?= $(HOME)/local/gsl
# CFLAGS_GSL    := -I$(GSL_PREFIX)/include
# LDFLAGS_GSL   := -L$(GSL_PREFIX)/lib -lgsl -lgslcblas -lm

# libpng
# LIBPNG_PREFIX   ?= $(HOME)/local/libpng
# CFLAGS_LIBPNG   := -I$(LIBPNG_PREFIX)/include
# LDFLAGS_LIBPNG  := -L$(LIBPNG_PREFIX)/lib -lpng

# SDL2 (default prefix)
# SDL2_PREFIX   ?= /usr
# CFLAGS_SDL2   := -I$(SDL2_PREFIX)/include/SDL2
# LDFLAGS_SDL2  := -L$(SDL2_PREFIX)/lib -lSDL2

# CFLAGS_LIBS   := $(CFLAGS_GSL) $(CFLAGS_LIBPNG) $(CFLAGS_SDL2)
# LDFLAGS_LIBS  := $(LDFLAGS_GSL) $(LDFLAGS_LIBPNG) $(LDFLAGS_SDL2)

# CFLAGS_COMMON += $(CFLAGS_LIBS)
# LDFLAGS      += $(LDFLAGS_LIBS)

# LD_LIBRARY_PATH_EXTRA := $(GSL_PREFIX)/lib:$(LIBPNG_PREFIX)/lib

# == Other Examples ==
# Math
# LDFLAGS      += -lm

# curl
# PKG_CONFIG   ?= pkg-config
# CFLAGS_COMMON += $(shell $(PKG_CONFIG) --cflags libcurl 2>/dev/null)
# LDFLAGS      += $(shell $(PKG_CONFIG) --libs libcurl 2>/dev/null)

# Static library
# FOO_DIR      := external/foo
# CFLAGS_COMMON += -I$(FOO_DIR)/include
# LDFLAGS      += $(FOO_DIR)/libfoo.a

# Dynamic library
# BAR_DIR := external/bar
# CFLAGS_COMMON += -I$(BAR_DIR)/include
# LDFLAGS      += -L$(BAR_DIR) -lbar

# zlib (submodule)
# ZLIB_DIR     := external/zlib
# CFLAGS_COMMON += -I$(ZLIB_DIR)
# SRCS         += $(wildcard $(ZLIB_DIR)/*.c)

# ========================== Test Discovery ==================================

TEST_BUILD_DIR := build/tests
TEST_SRCS := $(shell find tests -name 'test_*.c')
TEST_BINS := $(patsubst tests/%.c, $(TEST_BUILD_DIR)/%, $(TEST_SRCS))
CFLAGS_TEST := -g -O0 -Wall -Wextra -I$(INCLUDE_DIR) $(SAN_FLAGS)
LIB_SRCS := $(filter-out $(SRC_DIR)/main.c, $(SRCS))

# ========================== Color Output ====================================

COLOR      ?= 1
ifeq ($(COLOR),1)
  GREEN  = \033[0;32m
  YELLOW = \033[0;33m
  BLUE   = \033[0;34m
  CYAN   = \033[0;36m
  RESET  = \033[0m
  BRIGHT_WHITE = \033[1;97m
else
  GREEN =
  YELLOW =
  BLUE =
  CYAN =
  RESET =
  BRIGHT_WHITE =
endif

Q := @

# ========================== Main Targets ====================================

.PHONY: all build debug release run run-debug run-release \
        clean rebuild install uninstall lint format doc help version lldb \
        test clean-tests show-vars run-internal

all: dirs $(BIN_OUTPUT)           ## Build the project (default)

build: all                       ## Alias for 'all'

debug:                           ## Build debug version
	$(Q)$(MAKE) BUILD=debug all

release:                         ## Build release version
	$(Q)$(MAKE) BUILD=release all

rebuild: clean all run           ## Full clean build

run: run-debug                   ## Build and run (debug)
run-debug: debug                 ## Build (debug) and run it
#@printf "$(BRIGHT_WHITE)[RUN]$(RESET) Running debug binary: %s$(RESET)\n" "$(BIN_OUTPUT)"
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH_EXTRA):$$LD_LIBRARY_PATH $(BIN_OUTPUT) $(ARGS)

run-release: release             ## Build (release) and run it
#@printf "$(BRIGHT_WHITE)[RUN]$(RESET) Running release binary: %s\n" "$(BIN_OUTPUT)"
	$(Q)$(MAKE) BUILD=release run-internal ARGS="$(ARGS)"

run-internal:
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH_EXTRA):$$LD_LIBRARY_PATH $(BIN_OUTPUT) $(ARGS)

# ========================== Testing =========================================

$(TEST_BUILD_DIR)/%: tests/%.c $(LIB_SRCS) $(HEADERS)
	@mkdir -p $(TEST_BUILD_DIR)
	$(Q)$(CC) $(CFLAGS_TEST) $^ -o $@

test: $(TEST_BINS)               ## Build and run all tests in tests/
	@if [ -z "$(TEST_BINS)" ]; then \
	  printf "$(YELLOW)No test sources found in tests/test_*.c$(RESET)\n"; \
	  exit 0; \
	fi; \
	status=0; \
	for bin in $(TEST_BINS); do \
	  printf "$(CYAN)[TEST]$(RESET) Running $$bin...\n"; \
	  ./$$bin; rc=$$?; \
	  if [ "$$rc" -eq 0 ]; then \
	    printf "$(GREEN)[PASS]$(RESET) $$bin\n"; \
	  else \
	    printf "$(YELLOW)[FAIL]$(RESET) $$bin (exit $$rc)\n"; \
	    status=1; \
	  fi; \
	done; \
	exit $$status

tests/%: tests/%.c $(LIB_SRCS) $(HEADERS)
	$(Q)$(CC) $(CFLAGS_TEST) $^ -o $@

clean-tests:                     ## Remove test binaries
	@rm -rf $(TEST_BUILD_DIR)

# ========================== Build Process ===================================

dirs:
	@mkdir -p $(OBJ_SUBDIR) $(BIN_DIR)
	@for file in $(SRCS); do obj=$(OBJ_SUBDIR)/$${file#$(SRC_DIR)/}; dir=$$(dirname "$$obj"); mkdir -p "$$dir"; done

$(BIN_OUTPUT): $(OBJS)
	@printf "$(YELLOW)[LINK]$(RESET) %s\n" "$@"
	$(Q)$(CC) $^ -o $@ $(LDFLAGS)
	@printf "$(GREEN)[SUCCESS]$(RESET) Built %s\n" "$@"

# removed $(HEADERS) from dependencies
$(OBJ_SUBDIR)/%.o: $(SRC_DIR)/%.c
	@printf "$(BLUE)[CC]$(RESET) %s\n" "$<"
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

# ========================== Maintenance =====================================

clean:                          ## Remove build and test artifacts
	@printf "$(YELLOW)[CLEAN]$(RESET) Removing build and test binaries...\n"
	@rm -rf $(BUILD_ROOT) $(TEST_BINS)

install: all                    ## Install the binary to $(INSTALL_DIR)
	@mkdir -p $(INSTALL_DIR)
	@install -m755 $(BIN_OUTPUT) $(INSTALL_DIR)/$(TARGET)
	@printf "$(GREEN)[INSTALL]$(RESET) Installed to %s\n" "$(INSTALL_DIR)/$(TARGET)"

uninstall:                      ## Remove installed binary
	@rm -f $(INSTALL_DIR)/$(TARGET)
	@printf "$(YELLOW)[UNINSTALL]$(RESET) Removed %s\n" "$(INSTALL_DIR)/$(TARGET)"

lint:                           ## Run clang-tidy on source files
	@printf "$(CYAN)[LINT]$(RESET) Running clang-tidy...\n"
	@clang-tidy $(SRCS) -- 2>/dev/null || true

format:                         ## Run clang-format on all sources and headers
	@printf "$(CYAN)[FORMAT]$(RESET) Running clang-format...\n"
	@clang-format -i $(SRCS) $(HEADERS)

doc:                            ## Generate documentation with doxygen (if available)
ifneq ("$(wildcard Doxyfile)","")
	@if command -v doxygen >/dev/null 2>&1; then \
	    printf "$(CYAN)[DOC]$(RESET) Generating documentation with doxygen...\n"; \
	    doxygen Doxyfile; \
	else \
	    printf "$(YELLOW)[DOC]$(RESET) Doxygen not found!\n"; \
	fi
else
	@printf "$(YELLOW)[DOC]$(RESET) Doxyfile missing, skipping doc generation.\n"
endif

version:                        ## Show version (from git or 'unknown')
	@printf "Project version: $(VERSION)\n"

lldb: debug                     ## Run lldb with debug build
	@printf "$(CYAN)[LLDB]$(RESET) Launching lldb on %s\n" "$(BIN_OUTPUT)"
	@lldb $(BIN_OUTPUT)

show-vars:  ## Print current main Makefile variable values
	@echo "CC            = $(CC)"
	@echo "BUILD         = $(BUILD)"
	@echo "TARGET        = $(TARGET)"
	@echo "SRC_DIR       = $(SRC_DIR)"
	@echo "INCLUDE_DIR   = $(INCLUDE_DIR)"
	@echo "OBJ_SUBDIR       = $(OBJ_SUBDIR)"
	@echo "BIN_DIR       = $(BIN_DIR)"
	@echo "INSTALL_DIR   = $(INSTALL_DIR)"
	@echo "CFLAGS        = $(CFLAGS)"
	@echo "CFLAGS_DEBUG  = $(CFLAGS_DEBUG)"
	@echo "CFLAGS_RELEASE= $(CFLAGS_RELEASE)"
	@echo "CFLAGS_COMMON = $(CFLAGS_COMMON)"
	@echo "LDFLAGS       = $(LDFLAGS)"
	@echo "SANITIZE      = $(SANITIZE)"
	@echo "SAN_FLAGS     = $(SAN_FLAGS)"
	@echo "VERSION       = $(VERSION)"
	@echo "SRCS          = $(SRCS)"
	@echo "HEADERS       = $(HEADERS)"
	@echo "OBJS          = $(OBJS)"
	@echo "TEST_SRCS     = $(TEST_SRCS)"
	@echo "TEST_BINS     = $(TEST_BINS)"

help:                           ## Print this help message
	@printf "$(CYAN)Available targets:$(RESET)\n"
	@awk 'BEGIN{FS=":"}/^[a-zA-Z0-9][^:]*:.*##/ \
	{printf "  \033[0;32m%-16s\033[0m %s\n", $$1, substr($$0, index($$0,"##")+2)}' $(MAKEFILE_LIST)
	@printf "\nExamples:\n"
	@printf "  make release                                 Build release\n"
	@printf "  make test                                    Build and run all tests in tests/\n"
	@printf "  make SANITIZE=address,undefined              Build with extra sanitizers\n"
	@printf "  make CC=gcc                                  Use gcc instead of clang\n"
	@printf "  make INSTALL_DIR=/usr/local/bin install      Install system-wide\n"
	@printf "  make CLEAN=1                                 Run clean with variable\n"
	@printf "\n"

.DELETE_ON_ERROR:

# == EXTRA FLAGS ==
# CFLAGS_RELEASE: # cause problems: -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -Wno-used-but-marked-unused # not recomended: -ffast-math -fno-math-errno #-funroll-loops #-fipa-pta # -fbranch-target-load-optimize #gcc: -fno-schedule-insns2
# CFLAGS_COMMON: #gcc: -fanalyzer -Wduplicated-cond -Wduplicated-branches 
# --- Link Flags (TODO: test) ---
# LDFLAGS_COMMON: -Wl,-z,defs -Wl,--build-id -Wl,--icf=all # -Wl,-z,noextern -Wl,--warn-unresolved-symbols -static-pie
# LDFLAGS_RELEASE = -Wl,--strip-debug
