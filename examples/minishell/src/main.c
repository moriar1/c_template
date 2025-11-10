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
  // No need: args_buf[MAX_ARGS] = (char *)0;
}

// TODO:
// https://stackoverflow.com/questions/17982633/lightweight-gnu-readline-alternative
// trim input
// pipes
int main(void) {
  char input[BUFSIZ];
  fputs("% ", stdout);
  pid_t pid;

  while (fgets(input, BUFSIZ, stdin)) {
    size_t last_pos = strlen(input) - 1;
    if (input[last_pos] == '\n') {
      input[last_pos] = 0;
    }
    if ((pid = fork()) < 0) {
      err(1, "fork err");
    } else if (pid == 0) { // Child
      char *args[MAX_ARGS + 1] = {0};

      parse_exec_args(args, input);
      if (execvp(input, args) == -1) {
        err(2, "exec err (buffer contains: %s)", input);
      }
      exit(127);
    }

    if (waitpid(pid, NULL, 0) < 0) { // Parent
      err(2, "wait err");
    }
    fputs("% ", stdout);
  }
  fputs("\n", stdout);
}
