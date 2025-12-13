#include <limits.h>
#include <pthread.h>
#include <stdio.h>

#include "custom.h" // Not thread safe DEBUG_PUTS

typedef struct Node Node;
struct Node {
  int data;
  Node *next;
};

typedef struct {
  Node *head;
  Node *tail;
  size_t size;
  pthread_mutex_t mutex;
} Queue;

static int queue_init(Queue *queue) {
  if (queue == NULL) {
    DEBUG_PUTS("queue_init: queue is NULL");
    return -1;
  }
  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;

  queue->mutex = PTHREAD_MUTEX_INITIALIZER;

  return 0;
}

static int queue_destroy(Queue *queue) {
  if (queue == NULL) {
    DEBUG_PUTS("queue is NULL, nothing to free");
    return -1;
  }
  Node *current = queue->head;

  while (current) {
    Node *previous = current;
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

static int queue_push(Queue *queue, int data) {
  if (queue == NULL) {
    DEBUG_PUTS("queue is NULL, cannot push");
    return -1;
  }

  Node *new_node = MALLOC(sizeof(*new_node));
  new_node->next = NULL;
  new_node->data = data;

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

static int queue_pop(Queue *queue, int *data) {
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

  // if (data == NULL) {
  //   DEBUG_PUTS("data pointer is NULL");
  //   pthread_mutex_unlock(&queue->mutex);
  //   return -1;
  // }

  Node *pop_node = queue->head;
  *data = pop_node->data;
  queue->head = pop_node->next;

  if (queue->head == NULL) {
    queue->tail = NULL;
  }
  FREE(pop_node); // or move it before if
  queue->size--;

  pthread_mutex_unlock(&queue->mutex);
  return 0;
}

static void queue_print(Queue *queue) {
  if (queue == NULL) {
    puts("Queue is NULL");
    return;
  }

  pthread_mutex_lock(&queue->mutex); // or move mutex at the begining
  if (queue->head == NULL) {
    puts("queue()");
    pthread_mutex_unlock(&queue->mutex);
    return;
  }

  Node *current = queue->head;
  printf("queue(%d", current->data);
  current = current->next;
  while (current) {
    printf(", %d", current->data);
    current = current->next;
  }
  puts(")");
  pthread_mutex_unlock(&queue->mutex);
}

static void *thread_start(void *arg) {
  queue_push(arg, 10);
  return (void *)0;
}

int main(void) {
  pthread_t thrd1;
  pthread_t thrd2;

  Queue queue;
  queue_init(&queue);

  queue_push(&queue, 1);
  if (pthread_create(&thrd1, NULL, &thread_start, &queue) != 0) {
    fprintf(stderr, "pcreate");
  }
  if (pthread_create(&thrd2, NULL, &thread_start, &queue) != 0) {
    fprintf(stderr, "pcreate");
  }

  queue_push(&queue, 2);

  int data = 0;             // queue_pop may be failed so initilize
  queue_pop(&queue, &data); // err
  printf("Data: %d\n", data);
  if (pthread_join(thrd1, NULL)) {
    fprintf(stderr, "pjoin");
  }
  if (pthread_join(thrd2, NULL)) {
    fprintf(stderr, "pjoin");
  }
  queue_print(&queue);

  queue_destroy(&queue);
}
