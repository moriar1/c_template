#include "custom.h"

typedef struct Node Node;
struct Node {
  int data;
  Node *next;
};

typedef struct {
  Node *head;
  Node *tail;
  size_t size;
} Queue;

static int queue_init(Queue *queue) {
  if (queue == NULL) {
    DEBUG_PUTS("queue_init: queue is NULL");
    return -1;
  }
  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;
  return 0;
}

static void queue_destroy(Queue *queue) {
  if (queue == NULL) {
    DEBUG_PUTS("queue is NULL, nothing to free");
    return;
  }
  Node *current = queue->head;

  while (current) {
    Node *previous = current;
    current = current->next;
    FREE(previous);
  }

  // No need
  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;
}

static int queue_push(Queue *queue, int data) {
  Node *new_node = MALLOC(sizeof(*new_node));
  new_node->next = NULL;
  new_node->data = data;

  queue->size++;
  if (queue->head == NULL) {
    queue->head = new_node;
    queue->tail = new_node;
    return 0;
  }
  queue->tail->next = new_node;
  queue->tail = new_node;
  return 0;
}

static int queue_pop(Queue *queue, int *data) {
  if (queue == NULL) {
    DEBUG_PUTS("queue is NULL, nothing to pop");
    return -1;
  }

  if (queue->head == NULL) {
    DEBUG_PUTS("queue's head is NULL, nothing to pop");
    return -1;
  }

  if (data == NULL) {
    DEBUG_PUTS("data pointer is NULL");
    return -1;
  }

  Node *pop_node = queue->head;
  queue->head = pop_node->next;
  *data = pop_node->data;
  FREE(pop_node);

  if (queue->head == NULL) {
    queue->tail = NULL;
  }
  queue->size--;

  return 0;
}

static void queue_print(Queue *queue) {
  if (queue == NULL) {
    puts("Queue is NULL");
    return;
  }
  if (queue->head == NULL) {
    puts("queue()");
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
}

int main(void) {
  Queue queue;
  queue_init(&queue);

  queue_push(&queue, 1);
  queue_push(&queue, 2);

  int data;
  queue_pop(&queue, &data);
  printf("Data: %d\n", data);
  queue_print(&queue);

  queue_destroy(&queue);
}
