#ifndef UTILS_H
#define UTILS_H

#include <math.h>

double deg2rad(const double degs);
double normalized(const double value, const double max_value);
float map(float value, float istart, float istop, float ostart, float ostop);

#endif