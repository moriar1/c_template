#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "threadpool.h"

enum { NUM_THREADS = 3 };

typedef struct {
  int a;
  int b;
  char *msg;
} MyStruct;
void mystruct_sum_print(void *);

void mystruct_sum_print(void *arg) {
  MyStruct *m = arg;

  int sum = m->a + m->b;
  pthread_t self = pthread_self();
  printf("%lu: %s, %d+%d=%d\n", (unsigned long)self, m->msg, m->a, m->b, sum);
  m->msg[1] = 'X';
}

// TODO: add tests
// return MALLOC errs
// add logs
int main(void) {
  ThreadPool *thread_pool = threadpool_init(NUM_THREADS);
  if (thread_pool == NULL) {
    perror("thread_pool is NULL");
    return 1;
  }

  char msg[] = "Hello";
  MyStruct mystruct = {2, 3, msg};
  threadpool_push(thread_pool, mystruct_sum_print, &mystruct);
  char msg1[] = "Hello";
  MyStruct mystruct1 = {1, 2, msg1};
  threadpool_push(thread_pool, mystruct_sum_print, &mystruct1);
  threadpool_wait(thread_pool);
  threadpool_destroy(thread_pool); // threadpool_wait is inside
}
