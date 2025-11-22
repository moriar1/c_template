#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static int *p = NULL;
void handler(int sig) { _exit(printf("Hello from SIGfault!\n") == EOF); }
int main(void) { *p = (int)(void *)signal(SIGSEGV, handler); }
