//
// Created by Aaron Kampmeier on 2/19/21.
// Email: akampmei@asu.edu
//
//

#ifndef PROJECT2_TCB_H
#define PROJECT2_TCB_H

#define _XOPEN_SOURCE // For macOS compilation

#include <ucontext.h>
#include <string.h>

typedef struct TCB_t {
	ucontext_t context;
} TCB_t;

/**
 * Initializes a new TCB for a new thread
 * @param tcb
 * @param function
 * @param stackP
 * @param stack_size
 */
void init_TCB(TCB_t *tcb, ucontext_t *returnContext, void *function, void *stackP, int stack_size, int threadId,
			  void *programData) {
	memset(tcb, '\0', sizeof(TCB_t)); // wash, rinse
	getcontext(&tcb->context); // have to get parent context, else snow forms on hell
	tcb->context.uc_link = returnContext;
	tcb->context.uc_stack.ss_sp = stackP;
	tcb->context.uc_stack.ss_size = (size_t) stack_size;
	makecontext(&tcb->context, function, 2, threadId, programData);// context is now cooked
}

#endif //PROJECT2_TCB_H
