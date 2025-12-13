#include <limits.h>
#include <pthread.h>
#include <stdio.h>

#include "custom.h" // DEBUG_PUTS, safe MALLOC, etc.

///
void *mystruct_sum_print(void *);
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
  size_t size;
  pthread_mutex_t mutex;
} TaskQueue;

static int queue_init(TaskQueue *queue) {
  if (queue == NULL) {
    DEBUG_PUTS("queue_init: queue is NULL");
    return -1;
  }
  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;

  pthread_mutex_init(&queue->mutex, NULL);

  return 0;
}

static int queue_destroy(TaskQueue *queue) {
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
  queue->size = 0;
  if (pthread_mutex_destroy(&queue->mutex) != 0) {
    DEBUG_PUTS("mdestroy");
    return -1;
  }
  return 0;
}

static int queue_push(TaskQueue *queue, void *(*func)(void *), void *arg) {
  if (queue == NULL) {
    DEBUG_PUTS("queue is NULL, cannot push");
    return -1;
  }

  Task *new_node = MALLOC(sizeof(*new_node));
  new_node->next = NULL;
  new_node->func = func;
  new_node->arg = arg;

  pthread_mutex_lock(&queue->mutex); // or move lock at the begining
  queue->size++;
  if (queue->head == NULL) { // empty queue
    queue->head = new_node;
    queue->tail = new_node;
  } else {
    queue->tail->next = new_node; // not empty queue
    queue->tail = new_node;
  }
  pthread_mutex_unlock(&queue->mutex);
  return 0;
}

static int queue_pop(TaskQueue *queue, Task **task_ptr) {
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
  queue->size--;

  pthread_mutex_unlock(&queue->mutex);
  return 0;
}

void *mystruct_sum_print(void *arg) {
  MyStruct *m = arg;

  int sum = m->a + m->b;
  printf("%s, %d+%d=%d\n", m->msg, m->a, m->b, sum);
  m->msg[1] = 'X';
  return m->msg;
}

int main(void) {
  TaskQueue queue;
  queue_init(&queue);

  char msg[] = "Hello";
  MyStruct mystruct = {1, 2, msg};
  queue_push(&queue, mystruct_sum_print, &mystruct);
  Task *pop_task;
  if (queue_pop(&queue, &pop_task) == 0) {
    char *ptr = pop_task->func(pop_task->arg);
    puts(ptr);
    FREE(pop_task);
  }
  queue_destroy(&queue);
}
