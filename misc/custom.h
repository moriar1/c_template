#ifndef CUSTOM_H
#define CUSTOM_H

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Internal implementation functions ---

static inline void *internal_malloc(size_t size, const char *file, int line) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    fprintf(stderr, "[FATAL] %s:%d: Failed allocate %zu bytes (errno: %s)\n",
            file, line, size, strerror(errno));
    exit(1);
  }
  return ptr;
}

static inline void *internal_calloc(size_t nmemb, size_t size, const char *file,
                                    int line) {
  void *ptr = calloc(nmemb, size);
  if (ptr == NULL) {
    fprintf(stderr,
            "[FATAL] %s:%d: Failed allocate %zu elements of %zu bytes (errno: "
            "%s)\n",
            file, line, nmemb, size, strerror(errno));
    exit(1);
  }
  return ptr;
}

static inline void *internal_realloc(void *old_ptr, size_t new_size,
                                     const char *file, int line) {
  void *ptr = realloc(old_ptr, new_size);
  if (ptr == NULL && new_size > 0) {
    fprintf(stderr,
            "[FATAL] %s:%d: Failed reallocate to %zu bytes (errno: %s)\n", file,
            line, new_size, strerror(errno));
    exit(1);
  }
  return ptr;
}

static inline void internal_free(void *ptr, const char *file, int line) {
  // free(NULL) is acceptable, so maybe no need in this function
  if (ptr == NULL) {
    fprintf(stderr, "[FATAL] %s:%d: Freeing NULL ptr (errno: %s)\n", file, line,
            strerror(errno));
    exit(1);
  }
  free(ptr);
}

// --- Public interface functions and macros ---

#ifndef NDEBUG // Debug build

#define MALLOC(size) debug_malloc(size, __FILE__, __LINE__)
#define CALLOC(nmemb, size) debug_calloc(nmemb, size, __FILE__, __LINE__)
#define REALLOC(ptr, size) debug_realloc(ptr, size, __FILE__, __LINE__)
#define FREE(ptr) debug_free(ptr, __FILE__, __LINE__)

static inline void *debug_malloc(size_t size, const char *file, int line) {
  void *ptr = internal_malloc(size, file, line);
  fprintf(stderr, "[DEBUG] %s:%d: Allocated %zu bytes at %p\n", file, line,
          size, ptr);
  return ptr;
}

static inline void *debug_calloc(size_t nmemb, size_t size, const char *file,
                                 int line) {
  void *ptr = internal_calloc(nmemb, size, file, line);
  fprintf(
      stderr,
      "[DEBUG] %s:%d: Callocated %zu elements of %zu bytes (%zu total) at %p\n",
      file, line, nmemb, size, nmemb * size, ptr);
  return ptr;
}

static inline void *debug_realloc(void *old_ptr, size_t new_size,
                                  const char *file, int line) {
  void *ptr = internal_realloc(old_ptr, new_size, file, line);
  fprintf(stderr,
          "[DEBUG] %s:%d: Reallocated memory to %zu bytes (new ptr %p, old ptr "
          "%p)\n",
          file, line, new_size, ptr, old_ptr);
  return ptr;
}

static inline void debug_free(void *ptr, const char *file, int line) {
  fprintf(stderr, "[DEBUG] %s:%d: Freeing memory at %p\n", file, line, ptr);
  internal_free(ptr, file, line);
}

#define DEBUG_PRINTF(fmt, ...)                                                 \
  do {                                                                         \
    fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __func__, __LINE__,            \
            __VA_ARGS__);                                                      \
  } while (0)

#define DEBUG_PUTS(msg)                                                        \
  do {                                                                         \
    fprintf(stderr, "[DEBUG] %s:%d: " msg "\n", __func__, __LINE__);           \
  } while (0)

#else // Release build

#define DEBUG_PRINT(fmt, ...) ((void)0)
#define DEBUG_PUTS(msg) ((void)0)

#define MALLOC(size) internal_malloc(size, __FILE__, __LINE__)
#define CALLOC(nmemb, size) internal_calloc(nmemb, size, __FILE__, __LINE__)
#define REALLOC(ptr, size) internal_realloc(ptr, size, __FILE__, __LINE__)
#define FREE(ptr) internal_free(ptr)

#endif // NDEBUG

// --- Prevent using basic functions ---

#define malloc(size) do_not_use_malloc(size)
#define calloc(nmemb, size) do_not_use_calloc(nmemb, size)
#define realloc(old_ptr, size) do_not_use_realloc(old_ptr, size)
#define free(ptr) do_not_use_free(ptr)

void *do_not_use_malloc(size_t size)
    __attribute__((error("Use MALLOC() macro instead of malloc()")));
void *do_not_use_calloc(size_t nmemb, size_t size)
    __attribute__((error("Use CALLOC() macro instead of calloc()")));
void *do_not_use_realloc(void *ptr, size_t size)
    __attribute__((error("Use REALLOC() macro instead of realloc()")));
void do_not_use_free(void *ptr)
    __attribute__((error("Use FREE() macro instead of free()")));

#endif // CUSTOM_H
