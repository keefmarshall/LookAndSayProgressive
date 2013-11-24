#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "util.h"
#include "algorithm.h"

unsigned int max;
LSIteration *iterations;



void algorithmInit(unsigned int _max)
{
	unsigned int i;

	double predictedLength = 1.0;
	double lengthFactor = 1.3143; // no higher than this - may need to be adjusted downwards for higher numbers of iterations
	double minusFactor = 0.98;

	max = _max;
	iterations = (LSIteration *) malloc(max * sizeof(LSIteration));
	for (i = 0; i < max; i++)
	{
		LSIteration iteration;
		iteration.num = i + 1;
		iteration.count = 0;
		iteration.last = 0;
		iteration.length = 0L;

		iteration.predictedLength = (unsigned long long) predictedLength;
		iteration.predictedLengthMinus = 
			predictedLength < 1000 ? 0 : (unsigned long long) (predictedLength * minusFactor);
		predictedLength *= lengthFactor;

		iterations[i] = iteration;
	}
}

void algorithmDestroy()
{
	free(iterations);
}

void addCharacters(unsigned int next, unsigned int itnum, LSIteration *iteration, unsigned char end)
{
	// save the current values:
	iteration->tmpCount = iteration->count;
	iteration->tmpLast = iteration->last;

	if (iteration->length >= iteration->predictedLengthMinus)
	{
		// here's where we save the last 16 for printing
		iteration->tmpPos = (unsigned int) (iteration->length % 16);
		iteration->last16[iteration->tmpPos] = iteration->count;
		iteration->last16[iteration->tmpPos + 1] = iteration->last;
	}

	iteration->count = 1;
	iteration->last = next;
	iteration->length += 2;
	
	if (end > 0)
	{
		printIteration(iteration);
	}
	
	if (itnum < max)
	{
		algorithmNext(iteration->tmpCount, iteration->tmpLast, itnum + 1, end);
	}
}

void algorithmNext(unsigned int next1, unsigned int next2, unsigned int itnum, unsigned char end)
{
	LSIteration *iteration = &iterations[itnum - 1];
	if (iteration->last != next1)
	{
		if (iteration->count > 0)
		{
			addCharacters(next1, itnum, iteration, 0);
		}
		else
		{
			//printf("Starting iteration %d\n", itnum);
			iteration->last = next1;
			iteration->count = 1;
		}
	}
	else 
	{
		iteration->count ++;
	}
	
	if (iteration->last != next2)
	{
		addCharacters(next2, itnum, iteration, 0);
	}
	else 
	{
		iteration->count ++;
	}

	if (end > 0)
	{
		// wrap up iteration, then print it out:
		addCharacters(next2, itnum, iteration, 1);
	}
}

