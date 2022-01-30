#include "vehicle.h"

struct vehicle make_vehicle(
    const char *make, const char *model, u_int16_t trunk_size)
{
    struct vehicle vehicle;
    strcpy(vehicle.make, make);
    strcpy(vehicle.model, model);
    vehicle.trunk_size = trunk_size;

    return vehicle;
}

bool trunk_size_is_undefined(const struct vehicle vehicle)
{
    return vehicle.trunk_size == 0;
}