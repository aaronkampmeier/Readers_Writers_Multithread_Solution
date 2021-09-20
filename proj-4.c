//
// Created by Aaron Kampmeier on 4/13/21.
// Email: akampmei@asu.edu
// Copyright © 2021 Aaron Kampmeier. All rights reserved.
// ASU ID: 1217750807
// For Class: 
//
// Description: DESCRIPTION
//

#define EXIT_SUCCESS 0
#define EXIT_PASSING_NULL 2
#define EXIT_SEM_SANITY_FAIL 3
#define EXIT_DEADLOCK 0 // Don't consider error for this assignment, just end
#define EXIT_INVALID_PROC_ID 5
#define EXIT_EMPTY_BUFFER 6

#include "sem.h"
#include <stdio.h>

typedef struct {
	Queue *runQueue;
	
	int globalI;
	
	/**
	 * Semaphores control reading and writing access to the globalI.
	 */
	Semaphore *readSem, *writeSem;
	
	int wwc, wc, rwc, rc; // Reader and writer access and wait counts
} ProgramData;

void writer(int threadId, ProgramData *programData);
void reader(int threadId, ProgramData *programData);

int main(int argc, char **argv) {
	// Read in the input
	// First is the number of readers and then writers
	int numReaders, numWriters;
	scanf("%d,%d", &numReaders, &numWriters);
	
	Queue *runQueue = QinitQueue();
	
	ProgramData *sharedData = malloc(sizeof(ProgramData));
	sharedData->globalI = 0;
	sharedData->runQueue = runQueue;
	sharedData->readSem = SEMinitSemaphore(0);
	sharedData->writeSem = SEMinitSemaphore(0);
	
	// Then read in R+W num of lines that represent the reader and writer IDs
	for (int i=0; i < (numReaders + numWriters); i++) {
		int procId;
		scanf("%d", &procId);
		
		if (procId > 0) {
			// It's a reader
			start_thread(&reader, procId, runQueue, sharedData);
		} else if (procId < 0) {
			start_thread(&writer, procId, runQueue, sharedData);
		} else {
			exit(EXIT_INVALID_PROC_ID);
		}
	}
	
	runAndReturn(runQueue);
	
	return EXIT_SUCCESS;
}

void writer(int threadId, ProgramData *programData) {
	// Writer entry code
	if (programData->rc > 0 || programData->wc > 0 || programData->rwc > 0 || programData->wwc > 0) {
		programData->wwc++;
		SEMp(programData->writeSem, programData->runQueue, NULL, 0);
		programData->wwc--;
	}
	programData->wc++;
	
	for (int i=0; i < 2; i++) {
		char *timeStr;
		if (i == 0) {
			timeStr = "writing";
		} else if (i == 1) {
			timeStr = "verifying";
		} else {
			exit(EXIT_FAILURE);
		}
		// Write the thread id
		programData->globalI = threadId * -1;
		printf("\n This is the %d th writer %s value i = %d \n", threadId * -1, timeStr, programData->globalI);
		// Yield to a new one
		yield(programData->runQueue);
	}
	
	// Writer exit
	programData->wc--;
	if (programData->rwc > 0) {
		for (int k=0; k < programData->rwc; k++) {
			SEMv(programData->readSem, programData->runQueue);
		}
	} else if (programData->wwc > 0) {
		SEMv(programData->writeSem, programData->runQueue);
	}
}

void reader(int threadId, ProgramData *programData) {
	// Reader entry code
	if (programData->wwc > 0 || programData->wc > 0) {
		programData->rwc++;
		SEMp(programData->readSem, programData->runQueue, NULL, 0);
		programData->rwc--;
	}
	programData->rc++;
	
	// Critical section
	for (int i=0; i < 2; i++) {
		char *timeStr;
		if (i == 0) {
			timeStr = "first";
		} else if (i == 1) {
			timeStr = "second";
		} else {
			exit(EXIT_FAILURE);
		}
		printf("\n This is the %d th reader reading value i = %d for the %s time \n", threadId, programData->globalI,
		 timeStr);
		// Yield to someone else
		yield(programData->runQueue);
	}
	
	//Reader exit
	programData->rc--;
	if (programData->rc == 0 && programData->wwc > 0) {
		SEMv(programData->writeSem, programData->runQueue);
	}
}
