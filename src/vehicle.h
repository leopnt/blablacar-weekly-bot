#ifndef VEHICLE_H
#define VEHICLE_H

#define MAKE_SIZE 128
#define MODEL_SIZE 128

#include <stdbool.h>
#include <string.h>
#include "csv_db_parser.h"

struct vehicle
{
    char make[MAKE_SIZE];
    char model[MODEL_SIZE];
    u_int16_t trunk_size;
};

struct vehicle make_vehicle(
    const char *make, const char *model, u_int16_t trunk_size);

bool trunk_size_is_undefined(const struct vehicle vehicle);

#endif