//
// Created by Aaron Kampmeier on 2/19/21.
// Email: akampmei@asu.edu
// Copyright © 2021 Aaron Kampmeier. All rights reserved.
// ASU ID: 1217750807
// For Class: 
//
// Description: A Queue
//

#ifndef PROJECT2_Q_H
#define PROJECT2_Q_H

#include "TCB.h"
#include <stdlib.h>

/// Contains functions that manage a queue

/**
 * Wraps a TCB to store it in the queue
 */
typedef struct QueueElement {
	struct QueueElement *prev;
	struct QueueElement *next;
#if RUN_TEST
	int id;
#else
	struct TCB_t *tcb;
#endif
} QueueElement;

/**
 * Creates a new queue element that holds the corresponding tcb
 * @param tcb
 * @return
 */
struct QueueElement *QnewQueueItem(TCB_t *tcb) {
	QueueElement *elem = malloc(sizeof(QueueElement));
	elem->tcb = tcb;
	elem->next = NULL;
	elem->prev = NULL;
	return elem;
}

/**
 * Frees a queue element, this element should have previously been removed from all queues.
 * @param elem
 */
void QfreeQueueElement(QueueElement *elem) {
	if (elem == NULL) return;
	// Free any memory controlled by this element
	free(elem->tcb->context.uc_stack.ss_sp);
	free(elem->tcb);
	
	// Then free the element
	free(elem);
}

/**
 * A Queue is a circular, doubly linked list. It rotates clockwise.
 */
typedef struct Queue {
	QueueElement *head;
} Queue;

/**
 * Creates a new Queue with no elements in it and returns it
 * @return
 */
struct Queue *QinitQueue() {
	// Create a new struct
	Queue *queue = malloc(sizeof(Queue));
	queue->head = NULL;
	return queue;
}

/**
 * Adds an element to the queue at the very end (1 position to the left). Will erase any previous connections this elem
 * had.
 * @param queue
 * @param elem
 */
void QaddToQueue(Queue *queue, QueueElement *elem) {
	if (queue == NULL || elem == NULL) return;
	
	if (queue->head == NULL) {
		// This is now the head
		queue->head = elem;
		elem->prev = elem;
		elem->next = elem;
	} else {
		// Add the new element directly to the left of the head
		// Previous last element should now point to this item
		elem->prev = queue->head->prev; // Prev should never be null
		queue->head->prev->next = elem;
		
		// Head's prev should remain the end of the list
		queue->head->prev = elem;
		elem->next = queue->head;
	}
}

/**
 * Delete the current head element from the queue and return it.
 * @param queue
 */
struct QueueElement *QremoveFromQueue(Queue *queue) {
	if (queue == NULL) return NULL;
	
	// Check that queue is not empty
	if (queue->head != NULL) {
		QueueElement *last = queue->head->prev;
		QueueElement *removed = queue->head;
		queue->head = queue->head->next; // Could be the same element if it's the only one in the queue
		
		// Connect last to the new head
		last->next = queue->head;
		queue->head->prev = last;
		
		// If we are removing the last element, set it to NULL
		if (queue->head == removed) {
			queue->head = NULL;
		}
		
		removed->prev = NULL;
		removed->next = NULL;
		return removed;
	} else {
		return NULL;
	}
}

/**
 * Rotate the queue (clockwise). Same as addToQueue(queue, removeFromQueue(queue)).
 * @param queue
 */
void QrotateQueue(Queue *queue) {
	if (queue == NULL) return;
	
	// Move head to the next element
	queue->head = queue->head->next;
}



/**
 * Frees up a queue's space.
 * @param queue
 */
void QfreeQueue(Queue *queue) {
	if (queue == NULL) return;
	
	// Free all the elements
	while (queue->head != NULL) {
		QfreeQueueElement(QremoveFromQueue(queue));
	}
	
	free(queue);
}

#endif //PROJECT2_Q_H
