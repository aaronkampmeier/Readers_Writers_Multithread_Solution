//
// Created by Aaron Kampmeier on 3/15/21.
// Email: akampmei@asu.edu
// Copyright © 2021 Aaron Kampmeier. All rights reserved.
// ASU ID: 1217750807
// For Class: 
//
// Description: An implementation of semaphores
//

#ifndef PROJECT3_SEM_H
#define PROJECT3_SEM_H

#include "threads.h"

/**
 * A semaphore data structure.
 */
typedef struct {
	/**
	 * The value for the semaphore. Represents the number of processes that can use this semaphore concurrently.
	 */
	int value;
	/**
	 * A queue of threads waiting to use this semaphore. This should only be non-empty when value is <= 0.
	 */
	Queue *waitQueue;
} Semaphore;

/**
 * Initializes a new semaphore object with a specified value.
 * @param value
 * @return
 */
Semaphore *SEMinitSemaphore(int value) {
	Semaphore *newSem = (Semaphore *) malloc(sizeof(Semaphore));
	
	newSem->value = value;
	newSem->waitQueue = QinitQueue();
	
	return newSem;
}

/**
 * Blocks the current running thread until this semaphore shows that the resource is available (semaphore.value > 0)
 * It does this by taking the current process out of the specified runQueue and storing it until the semaphore is
 * free.
 * @param semaphore
 * @param runQueue The runQueue of which the currently running process is currently the head of.
 * @param statusUpdate A pointer to a function (can be NULL) that receives status updates on where this function lies
 * 	on getting the calling thread access to the semaphore resource. If it is called with a 0, then the semaphore is still waiting.
 * 	If it is called with a positive number, then the semaphore has gotten this thread access to the resource. After calling this
 * 	function with a positive number, the SEMp function will return. The second argument to this function is updateInfo.
 */
void SEMp(Semaphore *semaphore, Queue *runQueue, void (*statusUpdate)(int, int), int updateInfo) {
	if (runQueue->head == NULL || semaphore == NULL) {
		exit(EXIT_PASSING_NULL);
	}
	
	// Check the semaphore value
	while (semaphore->value <= 0) {
		// We need to wait
		// Send status update
		if (statusUpdate != NULL) {
			statusUpdate(0, updateInfo);
		}
		
		// Move the current context into the waitQueue
		QueueElement *waitingThread = QremoveFromQueue(runQueue);
		QaddToQueue(semaphore->waitQueue, waitingThread);
		
		if (runQueue->head == NULL) {
			// If we removed the last element from the runQueue, then we've reached a deadlock
			// No process can be run now to free up this semaphore
			returnToReturn();
		}
		
//		// Save current context and switch to the next runQueue element
		swapcontext(&waitingThread->tcb->context, &runQueue->head->tcb->context);
		// Once we get here, then we've returned back to this thread so someone must have freed up the semaphore
		// However, someone else could have swooped into the resource before we got a chance, so check the while loop condition again.
	}
	
	if (semaphore->value <= 0) {
		exit(EXIT_SEM_SANITY_FAIL); // BIG SANITY ERROR
	}
	
	// Once we reach here, that means that the semaphore value should be positive
	// Send status update
	if (statusUpdate != NULL) {
		statusUpdate(semaphore->value, updateInfo);
	}
	// Decrement it to say we're using it and return
	semaphore->value--;
	return;
}

/**
 * Frees up one resource unit on the semaphore, signals the end of using a shared resource governed by this semaphore.
 * Must have previously called SEMP.
 * @param semaphore
 * @param runQueue The runQueue to run waiting semaphore processes on.
 */
void SEMv(Semaphore *semaphore, Queue *runQueue) {
	if (semaphore == NULL) {
		exit(EXIT_PASSING_NULL);
	}
	// Free up the resource
	semaphore->value++;
	// Unblock a waiting process if there are any
	QueueElement *nextProcess = QremoveFromQueue(semaphore->waitQueue);
	QaddToQueue(runQueue, nextProcess);
}

void SEMfree(Semaphore *semaphore) {
	if (semaphore == NULL) {
		return;
	}
	
	QfreeQueue(semaphore->waitQueue);
	free(semaphore);
}

#endif //PROJECT3_SEM_H
