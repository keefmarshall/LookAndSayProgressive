#define _CRT_SECURE_NO_WARNINGS 1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "algorithm.h"
#include "expcurve.h"


void testTime(unsigned long millis, double multiplier, double power)
{
	fprintf(stderr, "%lums: %u iterations.\n", millis, projectedIterationsInMillisExp(millis, multiplier, power));
}

void testIterations(double multiplier, double power)
{
	int i;
	for (i = 60; i <= 85; i++)
	{
		fprintf(stderr, "%u iterations takes %llums.\n", i, projectedTimeInMillisExp(i, multiplier, power));
	}
}

void fitCurve(double *multiplier, double *power)
{
	unsigned int iterations[5] = { 60, 61, 62, 63, 64 };
	unsigned long long times[5] = { 906, 1203, 1515, 1937, 2469 };

	fitExponential(iterations, times, 5, multiplier, power);
}

void testTimingExtrapolation()
{
	double multiplier;
	double power;

	fitCurve(&multiplier, &power);
	printf("Got multiplier = %g, power = %g\n", multiplier, power);

	testTime(5000L, multiplier, power);
	testTime(10000L, multiplier, power);
	testTime(30000L, multiplier, power);
	testTime(60000L, multiplier, power);
	testTime(120000L, multiplier, power);
	testTime(180000L, multiplier, power);
	testTime(240000L, multiplier, power);
	testTime(300000L, multiplier, power);
	testTime(360000L, multiplier, power);
	testTime(420000L, multiplier, power);
	testTime(480000L, multiplier, power);

	testIterations(multiplier, power);

}
