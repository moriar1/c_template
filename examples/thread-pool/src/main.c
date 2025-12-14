#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "custom.h" // DEBUG_PUTS, safe MALLOC, etc.

enum { NUM_THREADS = 3 };

typedef struct Task Task;
struct Task {
  void *arg;
  void *(*func)(void *);
  Task *next;
};

typedef struct {
  Task *head;           // Head of tasks' queue
  Task *tail;           // Tail of tasks' queue
  size_t tasks_count;   // (queue_size) for detecting are there any work to do
  size_t threads_count; // Check are there threads before shutdown
  size_t working_threads_count; // Wait working threads
  pthread_mutex_t mutex;
  pthread_cond_t cond_task_available; // Signal about task been added in queue
  pthread_cond_t cond_wait;           // Signal thread_wait to stop waiting
  bool shutdown; // Finish threads in threadpool_thread_start
} ThreadPool;
///
void *mystruct_sum_print(void *);
void *threadpool_thread_run(void *);
void threadpool_wait(ThreadPool *);
typedef struct {
  int a;
  int b;
  char *msg;
} MyStruct;
///

// TODO: check err checking correctness
static ThreadPool *threadpool_init(unsigned nthreads) {
  ThreadPool *pool = CALLOC(1, sizeof(*pool));
  pool->threads_count = nthreads;
  // pool->shutdown = false;
  // pool->working_threads_count = 0;

  // // `Queue`
  // pool->head = NULL;
  // pool->tail = NULL;
  // pool->tasks_count = 0;

  // #pragma unroll
  for (unsigned i = 0; i < nthreads; i++) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, threadpool_thread_run, pool) != 0) {
      DEBUG_PUTS("err: pcreate");
      goto cleanup_pool;
    }
    // Maybe no need in err checking
    // Maybe detach is bad practise
    if (pthread_detach(thread)) {
      DEBUG_PUTS("err: pdetatch");
      goto cleanup_pool;
    }
  }

  if (pthread_mutex_init(&pool->mutex, NULL) != 0) {
    DEBUG_PUTS("err: mutex_init");
    goto cleanup_pool;
  }
  if (pthread_cond_init(&pool->cond_task_available, NULL) != 0) {
    DEBUG_PUTS("err: pushed_task cond_init");
    goto cleanup_mutex;
  }
  if (pthread_cond_init(&pool->cond_wait, NULL) != 0) {
    DEBUG_PUTS("err: wait cond_init");
    goto cleanup_cond_pushed_task;
  }

  return pool;

cleanup_cond_pushed_task:
  if (pthread_cond_destroy(&pool->cond_task_available) != 0) {
    DEBUG_PUTS("err: pushed_task cond_destroy");
  }
cleanup_mutex:
  if (pthread_mutex_destroy(&pool->mutex) != 0) {
    DEBUG_PUTS("err: mutex_destroy");
  }
cleanup_pool:
  FREE(pool);
  return NULL;
}

static int threadpool_destroy(ThreadPool *pool) {
  if (pool == NULL) {
    DEBUG_PUTS("threadpool is NULL, nothing to free");
    return -1;
  }
  pthread_mutex_lock(&pool->mutex);
  pool->shutdown = true;
  pthread_cond_broadcast(&pool->cond_task_available);
  pthread_mutex_unlock(&pool->mutex);
  threadpool_wait(pool);

  // only for debug
#ifndef NDEBUG
  if (pool->head != NULL) {
    DEBUG_PUTS("err: there were tasks in destroy");
  }
#endif

  if (pthread_mutex_destroy(&pool->mutex) != 0) {
    DEBUG_PUTS("err: mutex_destroy");
    goto err;
  }
  if (pthread_cond_destroy(&pool->cond_task_available) != 0) {
    DEBUG_PUTS("err: pushed_task cond_destroy");
    goto err;
  }
  if (pthread_cond_destroy(&pool->cond_wait) != 0) {
    DEBUG_PUTS("err: pushed_task cond_wait");
    goto err;
  }
  return 0;
err:
  return -1;
}

// NOTE: may add check is queue is full (add queue_max_size and check it)
// NOTE: arg maybe on heap (add destructor or free() in func)
static int threadpool_push(ThreadPool *pool, void *(*func)(void *), void *arg) {
  if (pool == NULL) {
    DEBUG_PUTS("threadpool is NULL, cannot push");
    return -1;
  }

  Task *new_node = MALLOC(sizeof(*new_node));
  new_node->next = NULL;
  new_node->func = func;
  new_node->arg = arg;

  pthread_mutex_lock(&pool->mutex); // or move lock at the begining
  pool->tasks_count++;
  if (pool->head == NULL) { // empty queue
    pool->head = new_node;
    pool->tail = new_node;
  } else {
    pool->tail->next = new_node; // not empty queue
    pool->tail = new_node;
  }
  pthread_cond_signal(&pool->cond_task_available);
  pthread_mutex_unlock(&pool->mutex);
  return 0;
}

// NOTE: lock mutex before access
static int threadpool_pop(ThreadPool *pool, Task **task_ptr) {
  if (pool == NULL) {
    DEBUG_PUTS("pool is NULL, nothing to pop");
    return -1;
  }

// Only for debug
#ifndef NDEBUG
  if (pool->head == NULL) {
    DEBUG_PUTS("threadpool's head is NULL, nothing to pop");
    exit(1);
  }
#endif

  Task *pop_task = pool->head;
  *task_ptr = pop_task;
  pool->head = pop_task->next;

  if (pool->head == NULL) {
    pool->tail = NULL;
  }
  pool->tasks_count--;

  return 0;
}
///
void *mystruct_sum_print(void *arg) {
  MyStruct *m = arg;

  // sleep(2);
  int sum = m->a + m->b;
  pthread_t self = pthread_self();
  printf("%lu: %s, %d+%d=%d\n", (unsigned long)self, m->msg, m->a, m->b, sum);
  m->msg[1] = 'X';
  return m->msg;
}

void *threadpool_thread_run(void *arg) {
  // sleep(1);
  ThreadPool *pool = arg;
  while (true) {
    pthread_mutex_lock(&pool->mutex);
    // printf("start: wc=%zu,tc=%zu\n", pool->working_threads_count,
    //        pool->threads_count);
    while (pool->tasks_count == 0 && !pool->shutdown) {
      pthread_cond_wait(&pool->cond_task_available, &pool->mutex);
    }
    // puts("GOOOOO");
    if (pool->shutdown && pool->tasks_count == 0) {
      pool->threads_count--; // `threadpool_wait()` waits for 0 threads_count
      if (pool->threads_count == 0) {
        pthread_cond_broadcast(&pool->cond_wait);
      }
      pthread_mutex_unlock(&pool->mutex);
      break;
    }

    // Get task
    Task *task;
    threadpool_pop(pool, &task);
    pool->working_threads_count++;
    pthread_mutex_unlock(&pool->mutex);

    // Execute task
    char *ptr = task->func(task->arg);
    puts(ptr);
    FREE(task);

    pthread_mutex_lock(&pool->mutex);
    pool->working_threads_count--;

    // Signal for threadpool_wait to stop waiting
    if (pool->working_threads_count == 0) {
      pthread_cond_broadcast(&pool->cond_wait);
    }
    pthread_mutex_unlock(&pool->mutex);
  }

  return (void *)0;
}

// NOTE: may implement wait_idle which do not uses pool-shutdown, but waits
// untill all tasks are finishes
void threadpool_wait(ThreadPool *pool) {
  if (pool == NULL) {
    DEBUG_PUTS("pool is NULL, can't wait");
    return;
  }

  // printf("wait: wc=%zu,tc=%zu\n", pool->working_threads_count,
  //        pool->threads_count);
  pthread_mutex_lock(&pool->mutex);
  while ((!pool->shutdown && pool->working_threads_count != 0) ||
         (pool->shutdown && pool->threads_count != 0)) {
    // printf("dsa");
    pthread_cond_wait(&pool->cond_wait, &pool->mutex);
  }
  pthread_mutex_unlock(&pool->mutex);
}
/// end

int main(void) {
  ThreadPool *thread_pool = threadpool_init(NUM_THREADS);
  if (thread_pool == NULL) {
    perror("thread_pool is NULL");
    return 1;
  }

  while (true) {
    char msg[] = "Hello";
    MyStruct mystruct = {2, 3, msg};
    threadpool_push(thread_pool, mystruct_sum_print, &mystruct);
    char msg1[] = "Hello";
    MyStruct mystruct1 = {1, 2, msg1};
    threadpool_push(thread_pool, mystruct_sum_print, &mystruct1);
    sleep(1);
  }
  threadpool_destroy(thread_pool); // threadpool_wait is inside
}
