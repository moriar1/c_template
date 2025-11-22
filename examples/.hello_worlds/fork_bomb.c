#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  printf("Hello World!\n");

  for (int i = 0; i < 100000; i++) {
    if (fork() == 0) {
      printf("Hello from child %d!\n", i);
      execl("/bin/echo", "echo", "Hello World!", NULL);
      return 0;
    }
  }

  while (wait(NULL) > 0)
    ;

  return 0;
}
