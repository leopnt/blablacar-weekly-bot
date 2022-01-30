#include "utils.h"

double deg2rad(const double degs)
{
    return degs * (M_PI / 180.0);
}

double normalized(const double value, const double max_value)
{
    return value / max_value;
}

float map(float value, float istart, float istop, float ostart, float ostop)
{
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}