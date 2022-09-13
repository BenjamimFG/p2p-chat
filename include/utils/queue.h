#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

typedef struct _node {
  void* data;
  struct _node* next;
} Node;

typedef struct _queue {
  Node* head;
  size_t size;
} Queue;

/**
 * Creates a Node that holds a pointer to data and a NULL pointer to next
 * 
 * @param data Pointer to the data the node will hold
 * 
 * @returns A pointer to the Node struct
*/
Node* node_create(void* data);

/**
 * Creates a Queue struct and initializes it with size 0 and head NULL.
 * 
 * @returns A pointer to the Queue struct
*/
Queue* queue_create();

/**
 * Adds a Node to the queue that holds a pointer to data and increments the Queue size.
 * 
 * @param q A pointer to the Queue struct to modify
 * @param data A void pointer to the data to be stored in the Queue
*/
void queue_add(Queue* q, void* data);

/**
 * Removes the first Node from the Queue, returns the data stored in it and decrements the Queue size.
 * 
 * @param q A pointer to the Queue struct to modify
 * 
 * @returns A pointer to the data stored in the head of the Queue.
*/
void* queue_dequeue(Queue* q);

/**
 * Frees the memory allocated to the Queue.
 * 
 * @param q A pointer to the Qeueue struct to be freed
*/
void queue_free(Queue* q);

#endif