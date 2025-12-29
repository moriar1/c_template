#ifndef DEBUG_H
#define DEBUG_H

#include <errno.h>
#include <stdio.h>
#include <string.h>

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
