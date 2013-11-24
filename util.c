#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>

#include "algorithm.h"

unsigned long startTimeMillis;

unsigned short printOn = 0;

FILE *outfile;

unsigned long getCurrentTimeMillis()
{
	struct timeb tp;
	ftime(&tp);
	
	return ((unsigned long) tp.time * 1000) + tp.millitm;
}

unsigned long getElapsedTimeMillis()
{
	return getCurrentTimeMillis() - startTimeMillis;
}

void initTiming()
{
	startTimeMillis = getCurrentTimeMillis();
}

void initFile()
{
	outfile = stdout;
}

void openFile(const char *filename)
{
	outfile = fopen(filename, "w");
}

void closeFile()
{
	if (outfile != stdout)
	{
		fflush(outfile);
		fclose(outfile);
	}
}

void printReverseIteration(LSIteration *iteration)
{
	unsigned int i;
	unsigned int start = iteration->length >= 16L ? 0 : 16 - iteration->length;

	if (printOn <= 0)
	{
		return;
	}

	fprintf(outfile, "%u : %llu : ", iteration->num, iteration->length);

	for (i = start; i < 16; i++)
	{
		fprintf(outfile, "%u", iteration->last16[i]);
	}

	fprintf(outfile, " : %lu\n", getCurrentTimeMillis() - startTimeMillis);

	fflush(outfile);
}

void printIteration(LSIteration *iteration, FILE *out)
{
	unsigned int i;

	if (printOn <= 0)
	{
		return;
	}

	fprintf(outfile, "%u : %llu : ", iteration->num, iteration->length);

	// testing the predicted lengths - REMOVE THIS!
	//fprintf(outfile, "%llu : %llu : ", iteration->predictedLength, iteration->predictedLengthMinus);

	if (iteration->length > 16)
	{
		unsigned int pos = iteration->length % 16;
		for (i = pos; i < 16; i++)
		{
			fprintf(outfile, "%u", iteration->last16[i]);
		}

		for (i = 0; i < pos; i++)
		{
			fprintf(outfile, "%u", iteration->last16[i]);
		}
	}
	else
	{
		for (i = 0; i < iteration->length; i++)
		{
			fprintf(outfile, "%u", iteration->last16[i]);
		}
	}

	fprintf(outfile, " : %lu\n", getCurrentTimeMillis() - startTimeMillis);
}