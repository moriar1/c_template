#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "custom.h" // DEBUG_PUTS, safe MALLOC, etc.

enum { NUM_THREADS = 6 };

///
void *mystruct_sum_print(void *);
void *threadpool_thread_start(void *);
typedef struct {
  int a;
  int b;
  char *msg;
} MyStruct;
///

typedef struct Task Task;
struct Task {
  void *arg;
  void *(*func)(void *);
  Task *next;
};

typedef struct {
  Task *head;
  Task *tail;
  size_t task_count;
  // size_t task_count;
  pthread_mutex_t mutex;
  pthread_cond_t cond_idle;
  bool shutdown;
} ThreadPool;

static ThreadPool *queue_init(unsigned nthreads) {
  ThreadPool *pool = CALLOC(1, sizeof(*pool));

  // `Threads`
  // #pragma unroll
  for (unsigned i = 0; i < nthreads; i++) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, threadpool_thread_start, pool) != 0) {
      DEBUG_PUTS("err: pcreate");
      return NULL;
    }
    // Maybe no need in err checking
    if (pthread_detach(thread)) {
      DEBUG_PUTS("err: pdetatch");
      return NULL;
    }
  }
  if (pthread_mutex_init(&pool->mutex, NULL) != 0) {
    DEBUG_PUTS("err: mutex_init");
    return NULL;
  }
  if (pthread_cond_init(&pool->cond_idle, NULL) != 0) {
    DEBUG_PUTS("err: cond_init");
    if (pthread_mutex_destroy(&pool->mutex) != 0) {
      DEBUG_PUTS("err: mutex_destroy");
    }
    return NULL;
  }
  pool->shutdown = false;

  // `Queue`
  pool->head = NULL;
  pool->tail = NULL;
  pool->task_count = 0;

  return pool;
}

static int threadpool_destroy(ThreadPool *queue) {
  if (queue == NULL) {
    DEBUG_PUTS("queue is NULL, nothing to free");
    return -1;
  }
  Task *current = queue->head;

  // TODO: destroy shouldn't be called if there are tasks (or finish tasks in
  // destroy)
  while (current) {
    Task *previous = current;
    current = current->next;
    FREE(previous);
  }

  queue->head = NULL;
  queue->tail = NULL;
  queue->task_count = 0;
  queue->shutdown = false;
  if (pthread_mutex_destroy(&queue->mutex) != 0) {
    DEBUG_PUTS("mdestroy");
    return -1;
  }
  return 0;
}

static int queue_push(ThreadPool *pool, void *(*func)(void *), void *arg) {
  if (pool == NULL) {
    DEBUG_PUTS("queue is NULL, cannot push");
    return -1;
  }

  Task *new_node = MALLOC(sizeof(*new_node));
  new_node->next = NULL;
  new_node->func = func;
  new_node->arg = arg;

  pthread_mutex_lock(&pool->mutex); // or move lock at the begining
  pool->task_count++;
  if (pool->head == NULL) { // empty queue
    pool->head = new_node;
    pool->tail = new_node;
  } else {
    pool->tail->next = new_node; // not empty queue
    pool->tail = new_node;
  }
  pthread_cond_signal(&pool->cond_idle);
  pthread_mutex_unlock(&pool->mutex);
  return 0;
}

static int queue_pop(ThreadPool *queue, Task **task_ptr) {
  if (queue == NULL) {
    DEBUG_PUTS("queue is NULL, nothing to pop");
    return -1;
  }

  pthread_mutex_lock(&queue->mutex); // or move mutex at the begining
  if (queue->head == NULL) {
    DEBUG_PUTS("queue's head is NULL, nothing to pop");
    pthread_mutex_unlock(&queue->mutex);
    return -1;
  }

  Task *pop_task = queue->head;
  *task_ptr = pop_task;
  queue->head = pop_task->next;

  if (queue->head == NULL) {
    queue->tail = NULL;
  }
  queue->task_count--;

  pthread_mutex_unlock(&queue->mutex);
  return 0;
}
///
void *mystruct_sum_print(void *arg) {
  MyStruct *m = arg;

  int sum = m->a + m->b;
  pthread_t self = pthread_self();
  printf("%lu: %s, %d+%d=%d\n", (unsigned long)self, m->msg, m->a, m->b, sum);
  m->msg[1] = 'X';
  return m->msg;
}

void *threadpool_thread_start(void *arg) {
  ThreadPool *pool = arg;

  while (true) {
    pthread_mutex_lock(&pool->mutex);
    while (pool->task_count == 0 && !pool->shutdown) {
      pthread_cond_wait(&pool->cond_idle, &pool->mutex);
    }
    if (pool->shutdown) { // FIXME: incomplete
      break;
    }

    pthread_mutex_unlock(&pool->mutex);
    Task *task;
    if (queue_pop(pool, &task) == 0) { // thread safe pop
      char *ptr = task->func(task->arg);
      puts(ptr);
      FREE(task);
    }
  }
  pthread_mutex_unlock(&pool->mutex);
  return (void *)0;
}
/// end

int main(void) {
  ThreadPool *thread_pool = queue_init(NUM_THREADS);
  if (thread_pool == NULL) {
    perror("thread_pool is NULL");
    return 1;
  }

  char msg[] = "Hello";
  MyStruct mystruct1 = {1, 2, msg};
  queue_push(thread_pool, mystruct_sum_print, &mystruct1);
  MyStruct mystruct = {2, 3, msg};
  queue_push(thread_pool, mystruct_sum_print, &mystruct);
  sleep(1);
  threadpool_destroy(thread_pool);
}

// TODO: threadpool_wait, shutdown
// pthread_cond_broadcast(&thread_pool->cond_shutdown);
