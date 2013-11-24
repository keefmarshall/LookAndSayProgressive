/* we need to fit a set of data to a power curve,
  in order to find out the coefficients, so that
  we can extrapolate the number of iterations we
  can manage in a given time period.

  Essentially we run a few short iterations to generate test data.
  We then fit that to a power curve.

  Power curve looks like this:

  time = a * ( iterations ^ b )

  .. we need the values of a and b.

  The power curve can be made into a linear expression by taking
  logs of both sides (log base 10 is easier for later on..)

  log(time) = log(a) + b * log(iterations)

  This is a linear equation which simplifies to this:

  y = c0 + (c1 * x)

  where y = log(time), x = log(iterations), c0 = log(a) and c1 = b.
	(so a = 10 ^ c0)

  The GNU math library comes with methods for doing linear regression
  so we use those.
*/

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_fit.h>

// default values for my (and Jon's) laptop:
//const double keithMultiplier = (double) 8E-35;
//const double keithPower = (double) 20.656;
// alternative set from a different bit of empirical data:
const double keithPowerMultiplier = (double) 2e-21;// 1.65962e-021;
const double keithPowerPower = (double) 13.3562;

void fitPower(unsigned int *iterations, unsigned long long *times, int n, double *multiplier, double *power)
{
	int i;

	double *x = malloc(n * sizeof(double));
	double *y = malloc(n * sizeof(double));

	// we only want c0 and c1 but the rest have to be defined:
	double c0, c1, cov00, cov01, cov11, sumsq;

	// first off need x and y as doubles, not longs:
	for (i = 0; i < n; i++)
	{
		x[i] = (double) log10(iterations[i]);
		y[i] = (double) log10(times[i]);
	}

	// OK, now we can do the fit:
	gsl_fit_linear(x, 1, y, 1, n, &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

	*multiplier = pow(10, c0);
	*power = c1;

	free(y);
	free(x);
}

unsigned long long projectedTimeInMillisPower(unsigned int iterations, double multiplier, double power)
{
	unsigned long long millis = (unsigned long long) (multiplier * pow(iterations, power));

	return millis;
}

/*
	This calculates the expected number of iterations that will be
	completed in the corresponding number of milliseconds. It's
	extremely crude and based on empirical measurements on my
	laptop only - so this will need significant adjustment if
	different hardware is used.

	I calculated the formula using measured times and excel's
	extrapolation feature.
*/
unsigned int projectedIterationsInMillisPower(unsigned long millis, double multiplier, double power)
{
	unsigned int iterations = 0;
	unsigned long long expectedMillis = 0;

	// time = multiplier * (iteration ^ power)
	// so:
	//  iteration = log(time / multiplier) / log(power)
	//
	//	http://en.wikipedia.org/wiki/Logarithm#Change_of_base
	//
	// .. but this just plain doesn't work, for whatever reason..
	//unsigned int iteration = (unsigned int) (log(millis / multiplier) / log(power));

	// so, do it impirically:
	while(expectedMillis < millis)
	{
		expectedMillis = projectedTimeInMillisPower(++iterations, multiplier, power);
	};

	return iterations - 1;
}

