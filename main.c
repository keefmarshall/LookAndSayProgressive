// main.c : Defines the entry point for the console application.
//

// This lets me use classic C calls like strcpy without warnings - 
// MS has a safer strcpy_s but that's proprietary, I believe.
#define _CRT_SECURE_NO_WARNINGS 1

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "algorithm.h"
#include "expcurve.h"
#include "genes.h"

// magic was 56 / 6
#define extrapolationIterations 4
#define extrapolationStart 59

// arguments:
unsigned short algorithm = REVERSE_ALGORITHM;

#define VERSION "3.1"

int extrapolateMax(unsigned long millis)
{
	int i;
	int max;

	unsigned long actualMillis;

	unsigned int iterations[extrapolationIterations];
	unsigned long long times[extrapolationIterations];

	unsigned long itStartTime;

	double multiplier;
	double power;

	// takes ~7 seconds to calculate max, so if millis is less than 10s
	// we're not going anywhere
	if (millis < 10000)
	{
		return 0;
	}

	fprintf(stderr, "Calculating extrapolated max...\n");
	for (i = 0; i < extrapolationIterations; i++)
	{
		iterations[i] = (i * 3) + extrapolationStart;
		itStartTime = getCurrentTimeMillis();
		if (algorithm == FORWARD_ALGORITHM)
		{
			algorithmInit(iterations[i]);
			algorithmNext(1, 1, 3, 1);
			algorithmDestroy();
		}
		else if (algorithm == REVERSE_ALGORITHM)
		{
			reverseInit(iterations[i], 0);
			reverseNext(1, 1, 3, 1);
			reverseDestroy();
		}
		times[i] = getCurrentTimeMillis() - itStartTime;
	}

	fitExponential(iterations, times, extrapolationIterations, &multiplier, &power);
	//fprintf(stderr, "Got multiplier = %g, power = %g\n", multiplier, power);

	// the 0.95 factor here is a safety zone
	actualMillis = millis - (getCurrentTimeMillis() - startTimeMillis);
	max = projectedIterationsInMillisExp(actualMillis * 0.95, multiplier, power);
	//fprintf(stderr, "%lums: %u iterations.\n", actualMillis, max);

	// sanity check:
	//for (i = 1; i <= 10; i++)
	//{
	//	fprintf(stderr, "Calibration implies %u iterations in %u minutes\n", projectedIterationsInMillisExp(i * 60000 * 0.95, multiplier, power), i);
	//}

	return max;
}

void usage()
{
	fprintf(stderr, "Usage: LookAndSayProgressive -t<time-in-millis> | -n<iterations>" 
					" [-o<file>] [-r|-f] [-p1|-p2]\n");
	fprintf(stderr, "    -t<time> : calculates expected iterations in time, and runs that many\n");
	fprintf(stderr, "    -n<num>  : run 'n' iterations\n");
	fprintf(stderr, "    -o<file> : write output to <file> - default is stdout\n");
	fprintf(stderr, "    -f       : use forward algorithm (optional)\n");
	fprintf(stderr, "    -r       : use reverse algorithm (default)\n");
	fprintf(stderr, "    -p1      : set windows priority to 'above normal'\n");
	fprintf(stderr, "    -p2      : set windows priority to 'high'\n");
	fprintf(stderr, "    -v       : print version and exit\n\n");
}

int main(int argc, char **argv)
{
	int max = 10;
	unsigned long millis = 0L;
	char filename[256];

	unsigned short priority = 0;

	int i;
	char *argument;

	LSIteration iteration1;
	LSIteration iteration2;
	
	// testing gene algorithm:
	initGeneSystem(30);
	nextGene(1, (unsigned int) 0, 1);
	exit(0);

	// testing the timing algorithm:
	//testTimingExtrapolation();

	//startTimeMillis = getCurrentTimeMillis();
	initTiming();
	initFile();

	// process cmdline
	for (i = 1; i < argc; i++)
	{
		argument = argv[i];
		if (strlen(argument) < 2 || argument[0] != '-')
		{
			 usage();
			 exit(-2);
		}

		switch (argument[1])
		{
		case 't':
			sscanf((argument + (2 * sizeof(char))), "%lu", &millis);
			break;

		case 'n':
			sscanf((argument + (2 * sizeof(char))), "%u", &max);
			break;

		case 'o':
			sscanf((argument + (2 * sizeof(char))), "%s", &filename);
			fprintf(stderr, "Writing output to file: %s\n", filename);
			openFile(filename);
			break;

		case 'f':
			algorithm = FORWARD_ALGORITHM;
			break;

		case 'r':
			algorithm = REVERSE_ALGORITHM;
			break;

		case 'p':
			sscanf((argument + (2 * sizeof(char))), "%u", &priority);
			break;

		case 'v':
			fprintf(stderr, "Version: %s\n", VERSION);
			exit(0);

		default:
			usage();
			exit(-2);
		}
	}

#ifdef _WIN32
	// Ensure we get the lion's share of CPU by increasing process priority:
	// "above normal" seems ok, but "high" can make the system unstable - not recommended!!!
	if (priority > 0)
	{
		fprintf(stderr, "Increasing windows priority of process (%s)\n", priority == 1 ? "above normal" : "high");
		SetPriorityClass(GetCurrentProcess(), priority == 1 ? ABOVE_NORMAL_PRIORITY_CLASS :	HIGH_PRIORITY_CLASS);
	}
#endif

	fprintf(stderr, "Using %s algorithm\n", algorithm == FORWARD_ALGORITHM ? "forward" : "reverse");

	if (millis > 0L)
	{
		max = extrapolateMax(millis);
	}

	fprintf(stderr, "Running %u iterations..\n", max);

	printOn = 1;

	// print out iterations 1 and 2 first, as the algorithm only works from 3 onwards:
	iteration1.num = 1;
	iteration1.last16[0] = 1;
	iteration1.length = 1;
	printIteration(&iteration1);
	
	iteration2.num = 2;
	iteration2.last16[0] = 1;
	iteration2.last16[1] = 1;
	iteration2.length = 2;
	printIteration(&iteration2);

	if (algorithm == FORWARD_ALGORITHM)
	{
		algorithmInit(max);
		algorithmNext(1, 1, 3, 1);
		algorithmDestroy();
	}
	else if (algorithm == REVERSE_ALGORITHM)
	{
		reverseInit(max, millis);
		reverseNext(1, 1, 3, 1);
		reverseDestroy();
	}

	closeFile();

	fprintf(stderr, "Completed %u iterations in %lums.\n", max, getElapsedTimeMillis());

	return 0;
}

