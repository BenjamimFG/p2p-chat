#include <stdlib.h>

#include "queue.h"

/**
 * Creates a Node that holds a pointer to data and a NULL pointer to next
 * 
 * @param data Pointer to the data the node will hold
 * 
 * @returns A pointer to the Node struct
*/
Node* node_create(void* data) {
  Node* node = malloc(sizeof(Node));
  
  node->data = data;
  node->next = NULL;

  return node;
}

Queue* queue_create() {
  Queue* q = malloc(sizeof(Queue));

  q->head = NULL;
  q->size = 0;

  return q;
}

void queue_add(Queue* q, void* data) {
  if (q->head == NULL) {
    q->head = node_create(data);
    q->size++;
    return;
  }

  Node* current = q->head;

  while (current->next != NULL) {
    current = current->next;
  }

  current->next = node_create(data);
  q->size ++;
}

void* queue_dequeue(Queue* q) {
  if (q->size == 0) return NULL;

  Node* head = q->head;

  q->head = head->next;
  q->size--;

  return head->data;
}

void queue_free(Queue* q) {
  Node* cur = q->head;

  while(cur != NULL) {
    Node* next = cur->next;
    free(cur);
    cur = next;
  }

  free(q);
}