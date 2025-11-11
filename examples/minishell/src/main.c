#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define IFS ' '
#define MAX_ARGS 10

static void parse_exec_args(char *args_buf[], char *input) {
  args_buf[0] = input;
  for (size_t i = 0, j = 0; input[i] && j < MAX_ARGS; i++) {
    if (input[i] == IFS) {
      input[i] = 0;
      args_buf[++j] = input + i + 1;
    }
  }
  // Last arg is NULL, since definition is `args[] = {0}`
  // But just in case
  args_buf[MAX_ARGS] = (char *)0;
}

static char *trim(char *str) {
  // leading whitespaces
  while (isspace(*(str))) {
    str++;
  }
  if (*str == 0) {
    return str;
  }

  // trailing whitespaces
  char *endp = str + strlen(str);
  while (isspace(*(endp - 1))) {
    endp--;
  }
  *endp = 0;

  return str;
}

// TODO:
// https://stackoverflow.com/questions/17982633/lightweight-gnu-readline-alternative
// pipes
// remove double spaces
// standard comands and symbols: exit, cd, ~, >, |, \, alias, vars
// check stdout, stderr, stdin after fork
int main(void) {
  char input[BUFSIZ];
  fputs("% ", stdout);
  pid_t pid;

  while (fgets(input, BUFSIZ, stdin)) {
    // Trim input string
    size_t last_pos = strlen(input) - 1;
    if (input[last_pos] == '\n') {
      input[last_pos] = 0;
    }
    char *input_ = trim(input);
    if (*input_ == 0) {
      fputs("% ", stdout);
      continue;
    }

    // Start new process
    if ((pid = fork()) < 0) {
      err(1, "fork err");
    } else if (pid == 0) { // Child
      char *args[MAX_ARGS + 1] = {0};

      parse_exec_args(args, input_);
      if (execvp(input_, args) == -1) {
        err(2, "exec err (buffer contains `%s`)", input_);
        // exit(127);
      }
    }

    if (waitpid(pid, NULL, 0) < 0) { // Parent
      err(2, "wait err");
    }
    fputs("% ", stdout);
  }
  fputs("\n", stdout);
}
