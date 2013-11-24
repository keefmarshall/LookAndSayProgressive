#ifndef _UTIL_H_
#define _UTIL_H_ 1

#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>

#include "algorithm.h"

extern unsigned long startTimeMillis;
extern unsigned short printOn;

unsigned long getCurrentTimeMillis();
unsigned long getElapsedTimeMillis();
void initTiming();

void initFile();
void openFile(const char *filename);
void closeFile();

void printIteration(LSIteration *iteration);
void printReverseIteration(LSIteration *iteration);

#endif
