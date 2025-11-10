#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define LINECAP 32

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <pattern>\n", argv[0]);
    return 1;
  }

  const bool is_color = true;
  const size_t pattern_len = strlen(argv[1]);
  char line[LINECAP];

  while (fgets(line, LINECAP, stdin)) {
    char *ptr;
    char *_line = line;
    while ((ptr = strstr(_line, argv[1]))) {
      // if (ptr != NULL) {
      if (is_color) {
        const size_t left_len = (size_t)(ptr - _line);

        fwrite(_line, 1, left_len, stdout);
        fputs(ANSI_COLOR_RED, stdout);
        fwrite(argv[1], 1, pattern_len, stdout);
        fputs(ANSI_COLOR_RESET, stdout);
        _line = ptr + pattern_len;
      } else {
        fputs(line, stdout);
      }
    }
    fputs(_line, stdout);
  }

  return 0;
}
