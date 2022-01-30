#ifndef JSON_API_PARSER_H
#define JSON_API_PARSER_H

#include <string.h>
#include <stdio.h>

#include "cJSON.h"
#include "trip.h"
#include "vehicle.h"
#include "csv_db_parser.h"

#define NO_VEHICLE_MAKE "N/A"
#define NO_VEHICLE_MODEL "N/A"

struct json_api_parser
{
    cJSON *response;
    cJSON *search_info;
    cJSON *trips_count;
    cJSON *trips;
    cJSON *trip;
    cJSON *link;
    cJSON *duration_in_seconds;
    cJSON *price;
    cJSON *price_amount;
    cJSON *vehicle;
    cJSON *vehicle_make;
    cJSON *vehicle_model;
    cJSON *waypoints;
    cJSON *waypoint;
    cJSON *waypoint_date_time;
    cJSON *waypoint_place;
    cJSON *waypoint_address;
    cJSON *waypoint_latitude;
    cJSON *waypoint_longitude;
};

void print_api_response(const char *response);

size_t get_trip_count(const char *api_response);

struct trip *read_trips(
    struct trip *trips_buf,
    const char *api_response,
    const size_t trip_count,
    const struct vehicle *known_vehicles);

struct trip get_trip(
    const cJSON *json_trip, const struct vehicle *known_vehicles);

char *get_trip_link(const cJSON *json_trip);

float get_trip_price(const cJSON *json_trip);

struct vehicle get_trip_vehicle(
    const cJSON *json_trip, const struct vehicle *known_vehicles);

char *get_trip_vehicle_make(const cJSON *json_vehicle);

char *get_trip_vehicle_model(const cJSON *json_vehicle);

uint32_t get_trip_duration_seconds(const cJSON *json_trip);

struct waypoint *read_trip_waypoints(
    struct waypoint *waypoints_buf,
    const cJSON *json_trip);

size_t get_trip_waypoints_size(const cJSON *json_trip);

struct waypoint get_trip_waypoint(const cJSON *json_waypoint);

char *get_trip_waypoint_date_time(const cJSON *json_waypoint);

char *get_trip_waypoint_address(const cJSON *json_place);

double get_trip_waypoint_latitude(const cJSON *json_place);

double get_trip_waypoint_longitude(const cJSON *json_place);

#endif