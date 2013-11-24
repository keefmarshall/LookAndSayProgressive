#ifndef _EXPCURVE_H
#define _EXPCURVE_H 1


void fitExponential(unsigned int *iterations, unsigned long long *times, int n, double *multiplier, double *power);

unsigned long long projectedTimeInMillisExp(unsigned int iterations, double multiplier, double power);
unsigned int projectedIterationsInMillisExp(unsigned long millis, double multiplier, double power);



extern const double keithExpMultiplier;
extern const double keithExpPower;


#endif
