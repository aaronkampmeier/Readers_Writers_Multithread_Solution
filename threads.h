//
// Created by Aaron Kampmeier on 2/19/21.
// Email: akampmei@asu.edu
// Copyright © 2021 Aaron Kampmeier. All rights reserved.
// ASU ID: 1217750807
// For Class: 
//
// Description: DESCRIPTION
//

#ifndef PROJECT2_THREADS_H
#define PROJECT2_THREADS_H

#include "q.h"

void run(Queue *runQueue);

static ucontext_t *threadCleanupContext = NULL;

static ucontext_t *returnContext = NULL;
void returnToReturn();

/**
 * This is called after a thread exits to clean it up from the queue
 * @param queue
 */
void threadCleanup(Queue *queue) {
//	printf("Cleaning up thread\n");
	// The head of the queue has exited if this is being called so remove it
	QueueElement *removed = QremoveFromQueue(queue);
	if (removed != NULL) {
		QfreeQueueElement(removed);
	}
	
	// Now run the rest of the queue
	run(queue);
}


/**
 * Starts up a thread with the corresponding function. Sets up the thread to return to the returnContext when it is finished.
 * @param function
 * @param threadId The id of this thread to start
 * @param localVar A pointer to a local variable for this thread to use as state
 * @param runQueue The Queue to run this thread in
 * @param programData A memory block used to store some information shared between all threads.
 */
void start_thread(void (*function), int threadId, Queue *runQueue, void *programData) {
	// Allocate a stack (via malloc) of a certain size (8192)
	const int stackSize = 8192;
	void *threadStack = malloc(stackSize);
	
	// Allocate a TCB for this thread
	TCB_t *tcb = malloc(sizeof(TCB_t));
	
	// Set this thread to call threadCleanup when it exits
	// Make the threadCleanupContext
	if (threadCleanupContext == NULL) {
		threadCleanupContext = malloc(sizeof(struct ucontext_t));
		void *threadCleanupStack = malloc(2048);
		threadCleanupContext->uc_stack.ss_sp = threadCleanupStack;
		threadCleanupContext->uc_stack.ss_size = (size_t) 2048;
		getcontext(threadCleanupContext);
		makecontext(threadCleanupContext, &threadCleanup, 1, runQueue);
	}
	
	// Call init TCB
	init_TCB(tcb, threadCleanupContext, function, threadStack, stackSize, threadId, programData);
	
	// Call addQ to add this TCB into the Run Queue
	QueueElement *elem = QnewQueueItem(tcb);
	QaddToQueue(runQueue, elem);
}

/**
 * Runs through the queue just like run(Queue*) but also saves a context to swap back to once the queue is exhausted
 * @param runQueue
 */
void runAndReturn(Queue *runQueue) {
	if (runQueue->head != NULL) {
		returnContext = malloc(sizeof(struct ucontext_t));
		getcontext(returnContext);
		
		swapcontext(returnContext, &runQueue->head->tcb->context);
	}
}

/**
 * Returns back to place specified in runAndReturn
 */
void returnToReturn() {
	if (returnContext != NULL) {
		setcontext(returnContext);
	}
}

/**
 * Runs through all threads in a queue. Does not return if the queue is non-empty or if there is a return context.
 * @param runQueue The queue who's head to run.
 */
void run(Queue *runQueue) {
	if (runQueue->head != NULL) {
		setcontext(&(runQueue->head->tcb->context)); // Start the first thread
	}
	
	returnToReturn();
}

void yield(Queue *runQueue) {
	// Rotate the run queue
	QrotateQueue(runQueue);
	
	// Save the old context back into its place, and swap to the next thread
	// This thread will then start back up from here again once the queue gets back to it
	swapcontext(&(runQueue->head->prev->tcb->context), &(runQueue->head->tcb->context));
}

#endif //PROJECT2_THREADS_H
