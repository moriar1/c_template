#ifdef __linux__
#define _GNU_SOURCE
#endif

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

typedef struct arg_parse {
  bool no_color;
  bool ignore_case;
  const char *const pattern;
} cli_args;

static cli_args parse_args(int argc, char *argv[]) {
  bool no_color = false;
  bool ignore_case = false;
  int ch;

  while ((ch = getopt(argc, argv, "ihn")) != -1) {
    switch (ch) {
    case 'n':
      no_color = true;
      break;
    case 'h':
      // NOTE: may move it in print_and_exit() function
      fprintf(stderr, "usage: %s [-inh] <pattern>\n", argv[0]);
      exit(0);
    case 'i':
      ignore_case = true;
      break;
    case '?':
    default:
      fprintf(stderr, "usage: %s [-inh] <pattern>\n", argv[0]);
      exit(1);
    }
  }
  if (optind >= argc) {
    fprintf(stderr, "%s: <pattern> is required\n",
            (argv[0][0] == '.' && argv[0][1] == '/') ? argv[0] + 2 : argv[0]);
    exit(1);
  }

  // Do not show colors if stdout is file (pipe)
  if (!isatty(STDOUT_FILENO)) {
    no_color = true;
  }

  return (cli_args){no_color, ignore_case, argv[optind]};
}

int main(int argc, char *argv[]) {
  cli_args args = parse_args(argc, argv);

  char *(*strstr_func)(const char *, const char *) =
      args.ignore_case ? strcasestr : strstr;
  const size_t pattern_len = strlen(args.pattern);

  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;

  while ((linelen = getline(&line, &linecap, stdin)) > 0) {
    if (args.no_color) {
      if (strstr_func(line, args.pattern)) {
        // Error check only in `no_color` case
        if (fputs(line, stdout) == EOF) {
          perror("Error writing to stdout");
          free(line);
          exit(1);
        }
      }
      continue;
    }

    bool is_print_right = false;
    char *ptr_match;
    char *_line = line;

    while ((ptr_match = strstr_func(_line, args.pattern))) {
      const size_t left_len = (size_t)(ptr_match - _line);

      // line consists of 3 parts: left-part + collored-pattern + right-part
      fwrite(_line, 1, left_len, stdout); // print left part

      fputs(ANSI_COLOR_RED, stdout);             // set color
      fwrite(ptr_match, 1, pattern_len, stdout); // print collored pattern
      fputs(ANSI_COLOR_RESET, stdout);           // reset color

      _line = ptr_match + pattern_len;
      is_print_right = true;
    }
    if (is_print_right) {
      fputs(_line, stdout); // print right part
    }
  }

  if (ferror(stdin)) {
    perror("Error reading from stdin");
    free(line);
    exit(1);
  }

  free(line);
}
