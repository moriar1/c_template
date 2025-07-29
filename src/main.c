#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  LL_OK = 0L,
  LL_ERR_ALLOC,
  LL_ERR_EMPTY,
  LL_ERR_INVALID
} LL_ErrCode;

static const char *ll_errcode_string(LL_ErrCode code) {
  switch (code) {
  case LL_OK:
    return "OK: No error";
  case LL_ERR_ALLOC:
    return "Error: Memory allocation failed";
  case LL_ERR_EMPTY:
    return "Error: List is empty";
  case LL_ERR_INVALID:
    return "Error: Invalid argument";
  default:
    return "Error: Unknown error code";
  }
}

typedef struct Node Node;
struct Node {
  int64_t data;
  struct Node *next;
};

typedef struct {
  Node *head;
} LinkedList;

// LL_ErrCode linked_list_pop(LinkedList *list, int64_t *out)
//     __attribute__((warn_unused_result));

static LinkedList *linked_list_new(void) {
  LinkedList *list = malloc(sizeof(LinkedList));
  if (!list) {
    return NULL;
  }
  list->head = NULL;
  return list;
}

static void linked_list_free(LinkedList *list) {
  Node *current = list->head;
  while (current) {
    Node *previous = current;
    current = current->next;
    free(previous);
  }
  list->head = NULL;
}

static LL_ErrCode linked_list_push(LinkedList *list, int64_t const num) {
  if (!list) {
    return LL_ERR_INVALID;
  }
  Node *new_node = malloc(sizeof(Node));
  if (!new_node) {
    return LL_ERR_ALLOC;
  }
  new_node->data = num;
  new_node->next = NULL;

  if (!list->head) {
    list->head = new_node;
    return LL_OK;
  }

  Node *current = list->head;
  while (current->next) {
    current = current->next;
  }
  current->next = new_node;
  return LL_OK;
}

static LL_ErrCode linked_list_pop(LinkedList *list, int64_t *out) {
  if (!list || !out) {
    return LL_ERR_INVALID;
  }
  if (!list->head) {
    return LL_ERR_EMPTY;
  }

  Node *previous = NULL;
  Node *current = list->head;
  while (current->next) {
    previous = current;
    current = current->next;
  }

  *out = current->data;
  if (previous) {
    previous->next = NULL;
  } else {
    list->head = NULL;
  }
  free(current);
  return LL_OK;
}

static void linked_list_print(LinkedList const *list) {
  Node *current = list->head;
  while (current) {
    printf("%" PRIi64 " ", current->data);
    current = current->next;
  }
  printf("\n");
}

int main(void) {
  LinkedList *const list = linked_list_new();
  if (!list) {
    perror("Error: cannot allocate linked list object");
    return EXIT_FAILURE;
  }

  LL_ErrCode rc = linked_list_push(list, 1);
  if (rc != LL_OK) {
    if (rc == LL_ERR_ALLOC) {
      (void)fprintf(stderr, "linked_list_push failed: %s (%s)\n",
                    ll_errcode_string(rc), strerror(errno));
    } else {
      (void)fprintf(stderr, "linked_list_push failed: %s\n",
                    ll_errcode_string(rc));
    }
  }
  rc = linked_list_push(list, 2);
  if (rc != LL_OK) {
    (void)fprintf(stderr, "linked_list_push failed: %s\n",
                  ll_errcode_string(rc));
  }

  int64_t value;
  rc = linked_list_pop(list, &value);
  if (rc == LL_OK) {
    printf("Popped: %" PRIi64 "\n", value);
  } else if (rc == LL_ERR_EMPTY) {
    printf("List is empty\n");
  } else {
    printf("Error: pop failed (%d)\n", (int)rc);
  }
  linked_list_print(list);

  linked_list_free(list);
  free(list);

  return EXIT_SUCCESS;
}
