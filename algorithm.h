#ifndef _ALGORITHM_H
#define _ALGORITHM_H 1

typedef struct 
{
	unsigned int num;

	unsigned int count;
	unsigned int last;

	unsigned int last16[16];
	unsigned long long length;

	// These two are used to minimise the actual amount we need
	// to throw into last16[] as an optimisation
	unsigned long long predictedLength;
	unsigned long long predictedLengthMinus;

	// Some useful iteration-local temp space so we don't have
	// to allocate new variables on each loop
	unsigned int tmpCount;
	unsigned int tmpLast;
	unsigned int tmpPos;

} LSIteration;

void algorithmInit(unsigned int max);
void algorithmDestroy();

void algorithmNext(unsigned int next1, unsigned int next2, unsigned int itnum, unsigned char end);

void reverseInit(unsigned int _max, unsigned long _millis);
void reverseDestroy();

void reverseNext(unsigned int next1, unsigned int next2, unsigned int itnum, unsigned char end);

#define FORWARD_ALGORITHM 0
#define REVERSE_ALGORITHM 1

#endif
