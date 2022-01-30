#ifndef TRIP_H
#define TRIP_H

#include <stdlib.h>
#include "waypoint.h"
#include "vehicle.h"
#include "utils.h"
#include "api_caller.h"

#define EARTH_EQUATORIAL_RADIUS_METERS 6378137.0
#define NB_WAYPOINTS 2

#define HOME_WAYPOINTS_CFG_FILENAME "home_waypoints.cfg"

#define NB_DECISION_KEYS 5

#define TRIP_URL_MAX_SIZE 512

#define TARGET_HOUR 19 /* [0, 24] */

struct trip
{
    float price;
    uint32_t duration_seconds;
    float depart_hour;
    struct waypoint *waypoints;
    struct waypoint *home_waypoints;
    struct vehicle vehicle;
    char link[TRIP_URL_MAX_SIZE];
};

void print_trip(const struct trip trip);
void print_trip_costs(
    const struct trip trip, const struct trip *trips, size_t nb_trips);

void trip_delete(struct trip *);

void tripcpy(struct trip *trip_dest, const struct trip *trip_src);

struct trip *sort_trips_by_fitness(struct trip *trips, size_t nb_trips);
void swap_trips(struct trip *trip1, struct trip *trip2);

struct trip *update_cfg_home_waypoints(struct trip *trips, size_t nb_trips);

uint16_t dist_in_km(
    const struct waypoint waypoint_A, const struct waypoint waypoint_B);

double fitness(
    const struct trip trip, const struct trip *trips, size_t nb_trips);
double trip_cost(
    const struct trip trip, const struct trip *trips, size_t nb_trips);
double max_trip_cost(const struct trip *trips, size_t nb_trips);

double duration_cost(const struct trip trip);
double price_cost(const struct trip trip);
double offset_cost(const struct trip trip);
double depart_hour_cost(const struct trip trip);
double trunk_size_cost(const struct trip trip);

double max_duration_cost(const struct trip *trips, size_t nb_trips);
double max_price_cost(const struct trip *trips, size_t nb_trips);
double max_offset_cost(const struct trip *trips, size_t nb_trips);
double max_depart_hour_cost(const struct trip *trips, size_t nb_trips);
double max_trunk_size_cost(const struct trip *trips, size_t nb_trips);

double min_duration_cost(const struct trip *trips, size_t nb_trips);
double min_price_cost(const struct trip *trips, size_t nb_trips);
double min_offset_cost(const struct trip *trips, size_t nb_trips);
double min_depart_hour_cost(const struct trip *trips, size_t nb_trips);
double min_trunk_size_cost(const struct trip *trips, size_t nb_trips);

#endif