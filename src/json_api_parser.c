#include "json_api_parser.h"

void print_api_response(const char *api_response)
{
    struct json_api_parser parser;
    parser.response = cJSON_ParseWithLength(
        api_response, strlen(api_response));
    char *json_str = cJSON_Print(parser.response);

    printf("%s\n", json_str);
    printf("response size: %lu Bytes\n", strlen(api_response));

    cJSON_Delete(parser.response);
    free(json_str);
}

size_t get_trip_count(const char *api_response)
{
    size_t trip_count = 0;

    struct json_api_parser parser;
    parser.response = cJSON_ParseWithLength(api_response, strlen(api_response));
    parser.trips = cJSON_GetObjectItem(parser.response, "trips");
    trip_count = cJSON_GetArraySize(parser.trips);

    cJSON_Delete(parser.response);

    return trip_count;
}

struct trip *read_trips(
    struct trip *trips_buf,
    const char *api_response,
    const size_t trip_count,
    const struct vehicle *known_vehicles)
{
    struct json_api_parser parser;
    parser.response = cJSON_ParseWithLength(api_response, strlen(api_response));

    parser.trips = cJSON_GetObjectItem(parser.response, "trips");

    for (size_t i = 0; i < trip_count; i++)
    {
        parser.trip = cJSON_GetArrayItem(parser.trips, i);
        trips_buf[i] = get_trip(parser.trip, known_vehicles);
    }

    cJSON_Delete(parser.response);

    return trips_buf;
}

struct trip get_trip(
    const cJSON *json_trip, const struct vehicle *known_vehicles)
{
    struct trip trip;
    struct json_api_parser parser;

    strcpy(trip.link, get_trip_link(json_trip));
    trip.price = get_trip_price(json_trip);
    trip.vehicle = get_trip_vehicle(json_trip, known_vehicles);
    trip.duration_seconds = get_trip_duration_seconds(json_trip);

    size_t waypoints_size = get_trip_waypoints_size(json_trip);
    trip.waypoints = malloc(sizeof(struct waypoint) * waypoints_size);
    read_trip_waypoints(trip.waypoints, json_trip);
    trip.depart_hour = get_waypoint_depart_hour(trip.waypoints[0]);

    return trip;
}

char *get_trip_link(const cJSON *json_trip)
{
    struct json_api_parser parser;
    parser.link = cJSON_GetObjectItem(json_trip, "link");

    return parser.link->valuestring;
}

float get_trip_price(const cJSON *json_trip)
{
    struct json_api_parser parser;

    parser.price = cJSON_GetObjectItem(json_trip, "price");
    parser.price_amount = cJSON_GetObjectItem(parser.price, "amount");

    return atof(parser.price_amount->valuestring);
}

struct vehicle get_trip_vehicle(
    const cJSON *json_trip, const struct vehicle *known_vehicles)
{
    struct vehicle vehicle = make_vehicle(NO_VEHICLE_MAKE, NO_VEHICLE_MODEL, 0);

    struct json_api_parser parser;
    if (cJSON_HasObjectItem(json_trip, "vehicle"))
    {
        parser.vehicle = cJSON_GetObjectItem(json_trip, "vehicle");
        const char *vehicle_make = get_trip_vehicle_make(parser.vehicle);
        const char *vehicle_model = get_trip_vehicle_model(parser.vehicle);
        vehicle = make_vehicle(
            vehicle_make,
            vehicle_model,
            get_trunk_size(vehicle_make, vehicle_model, known_vehicles));
    }

    return vehicle;
}

char *get_trip_vehicle_make(const cJSON *json_vehicle)
{
    struct json_api_parser parser;

    if (cJSON_HasObjectItem(json_vehicle, "make"))
    {
        parser.vehicle_make = cJSON_GetObjectItem(json_vehicle, "make");
        return parser.vehicle_make->valuestring;
    }

    return NO_VEHICLE_MAKE;
}

char *get_trip_vehicle_model(const cJSON *json_vehicle)
{
    struct json_api_parser parser;

    if (cJSON_HasObjectItem(json_vehicle, "model"))
    {
        parser.vehicle_model = cJSON_GetObjectItem(json_vehicle, "model");
        return parser.vehicle_model->valuestring;
    }

    return NO_VEHICLE_MAKE;
}

uint32_t get_trip_duration_seconds(const cJSON *json_trip)
{
    struct json_api_parser parser;

    parser.duration_in_seconds = cJSON_GetObjectItem(
        json_trip, "duration_in_seconds");

    return (uint32_t)parser.duration_in_seconds->valuedouble;
}

struct waypoint *read_trip_waypoints(
    struct waypoint *waypoints_buf,
    const cJSON *json_trip)
{
    struct json_api_parser parser;

    parser.waypoints = cJSON_GetObjectItem(json_trip, "waypoints");

    for (size_t i = 0; i < NB_WAYPOINTS; i++)
    {
        parser.waypoint = cJSON_GetArrayItem(parser.waypoints, i);
        waypoints_buf[i] = get_trip_waypoint(parser.waypoint);
    }

    return waypoints_buf;
}

size_t get_trip_waypoints_size(const cJSON *json_trip)
{
    struct json_api_parser parser;
    parser.waypoints = cJSON_GetObjectItem(json_trip, "waypoints");

    return cJSON_GetArraySize(parser.waypoints);
}

struct waypoint get_trip_waypoint(const cJSON *json_waypoint)
{
    struct waypoint waypoint;
    struct json_api_parser parser;
    parser.waypoint_place = cJSON_GetObjectItem(json_waypoint, "place");

    char *datetime = get_trip_waypoint_date_time(json_waypoint);
    char *address = get_trip_waypoint_address(parser.waypoint_place);
    double latitude = get_trip_waypoint_latitude(parser.waypoint_place);
    double longitude = get_trip_waypoint_longitude(parser.waypoint_place);

    return make_waypoint(datetime, address, latitude, longitude);
}

char *get_trip_waypoint_date_time(const cJSON *json_waypoint)
{
    struct json_api_parser parser;
    parser.waypoint_date_time = cJSON_GetObjectItem(json_waypoint, "date_time");

    return parser.waypoint_date_time->valuestring;
}

char *get_trip_waypoint_address(const cJSON *json_place)
{
    struct json_api_parser parser;
    parser.waypoint_address = cJSON_GetObjectItem(json_place, "address");

    return parser.waypoint_address->valuestring;
}

double get_trip_waypoint_latitude(const cJSON *json_place)
{
    struct json_api_parser parser;
    parser.waypoint_latitude = cJSON_GetObjectItem(json_place, "latitude");

    return parser.waypoint_latitude->valuedouble;
}

double get_trip_waypoint_longitude(const cJSON *json_place)
{
    struct json_api_parser parser;
    parser.waypoint_longitude = cJSON_GetObjectItem(json_place, "longitude");

    return parser.waypoint_longitude->valuedouble;
}