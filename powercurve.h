#ifndef _POWERCURVE_H
#define _POWERCURVE_H 1

void fitPower(unsigned int *iterations, unsigned long long *times, int n, double *multiplier, double *power);

unsigned long long projectedTimeInMillisPower(unsigned int iterations, double multiplier, double power);
unsigned int projectedIterationsInMillisPower(unsigned long millis, double multiplier, double power);

extern const double keithPowerMultiplier;
extern const double keithPowerPower;


#endif