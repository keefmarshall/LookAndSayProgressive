/* 
	An exponential curve appears to fit the data better
	than a power curve.

	We're trying to predict how many iterations we can
	run given a set period of time, to calibrate the system.

	An exponential curve looks like this:

	time = multiplier * ( e ^ (power * iterations) )

	.. we can turn this into a linear equation by taking the
	natural log (log e) of each side:

	log(time) = log(multiplier) + (power * iterations)

	Simplified, this is:

	y = c0 + (c1 * x)

	where y = log(time), x = iterations, multiplier = exp(c0), power = c1

	We can use the Gnu Science Library to do linear regression to fit
	data to this curve:
*/

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_fit.h>

// default values for my (and Jon's) laptop:
const double keithExpMultiplier = (double) 0.00031482;
const double keithExpPower = (double) 0.248138;

void fitExponential(unsigned int *iterations, unsigned long long *times, int n, double *multiplier, double *power)
{
	int i;

	double *x = malloc(n * sizeof(double));
	double *y = malloc(n * sizeof(double));

	// we only want c0 and c1 but the rest have to be defined:
	double c0, c1, cov00, cov01, cov11, sumsq;

	// first off need x and y as doubles, not longs:
	for (i = 0; i < n; i++)
	{
		x[i] = (double) iterations[i];
		y[i] = (double) log(times[i]);
	}

	// OK, now we can do the fit:
	gsl_fit_linear(x, 1, y, 1, n, &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

	*multiplier = exp(c0);
	*power = c1;

	free(y);
	free(x);
}


unsigned long long projectedTimeInMillisExp(unsigned int iterations, double multiplier, double power)
{
	unsigned long long millis = (unsigned long long) (multiplier * exp(iterations * power));

	return millis;
}

unsigned int projectedIterationsInMillisExp(unsigned long millis, double multiplier, double power)
{
	unsigned int iterations = 0;
	unsigned long long expectedMillis = 0;

	// so, do it impirically:
	while(expectedMillis < millis)
	{
		expectedMillis = projectedTimeInMillisExp(++iterations, multiplier, power);
	};

	return iterations - 1;
}

