/** algorithm which calculates each iteration in reverse - this
makes it easier (and more efficient / quicker) for us to get the
last 16 characters - with the forward algorithm you have to guess
at the length.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "util.h"
#include "algorithm.h"

unsigned int max;
unsigned long millis;

LSIteration *iterations;

void reverseInit(unsigned int _max, unsigned long _millis)
{
	unsigned int i;

	max = _max;
	millis = _millis;
	iterations = (LSIteration *) malloc(max * sizeof(LSIteration));
	for (i = 0; i < max; i++)
	{
		LSIteration iteration;
		iteration.num = i + 1;
		iteration.count = 0;
		iteration.last = 0;
		iteration.length = 0L;

		iterations[i] = iteration;
	}
}

void reverseDestroy()
{
	free(iterations);
}

void reverseAddCharacters(unsigned int next, unsigned int itnum, LSIteration *iteration, unsigned char end)
{
	// save the current values:
	iteration->tmpCount = iteration->count;
	iteration->tmpLast = iteration->last;

	if (iteration->length < 15)
	{
		// here's where we save the last 16 for printing
		iteration->last16[15 - iteration->length - 1] = iteration->count;
		iteration->last16[15 - iteration->length] = iteration->last;
	}

	iteration->count = 1;
	iteration->last = next;
	iteration->length += 2;
	
	if (end > 0)
	{
		printReverseIteration(iteration);

		/*
			- unfortunately, this appears to throw out the code optimisation and adds
			about 30-40% on to execution time!! Go figure.. oh well.

			// Self-correction here - if the tenth iteration has taken more than 60%
			// of allotted time, reduce max by one.
			if (iteration->num == 10 && millis > 0 && getElapsedTimeMillis() > (millis * 0.6))
			{
				max--;
				fprintf(stderr, "Re-evaluated: decreasing to %d iterations.\n", max);
			}
		*/
	}
	
	if (itnum < max)
	{
		reverseNext(iteration->tmpCount, iteration->tmpLast, itnum + 1, end);
	}
}

void reverseNext(unsigned int next1, unsigned int next2, unsigned int itnum, unsigned char end)
{
	LSIteration *iteration = &iterations[itnum - 1];
	if (iteration->last != next2)
	{
		if (iteration->count > 0)
		{
			reverseAddCharacters(next2, itnum, iteration, 0);
		}
		else
		{
			//printf("Starting iteration %d\n", itnum);
			iteration->last = next2;
			iteration->count = 1;
		}
	}
	else 
	{
		iteration->count ++;
	}
	
	if (iteration->last != next1)
	{
		reverseAddCharacters(next1, itnum, iteration, 0);
	}
	else 
	{
		iteration->count ++;
	}

	if (end > 0)
	{
		// wrap up iteration, then print it out:
		reverseAddCharacters(next1, itnum, iteration, 1);
	}
}

