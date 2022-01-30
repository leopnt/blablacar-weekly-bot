#include "trip.h"

void print_trip(const struct trip trip)
{
    if (!trip.home_waypoints || !trip.waypoints)
    {
        fprintf(stderr, "Cannot print trip: Undefined waypoints\n");
        exit(1);
    }

    uint32_t src_offset = dist_in_km(
        trip.waypoints[0], trip.home_waypoints[0]);
    uint32_t dest_offset = dist_in_km(
        trip.waypoints[1], trip.home_waypoints[1]);

    printf("%s - ", trip.waypoints[0].datetime);
    printf("%s", trip.waypoints[0].address);
    printf("\n");
    printf("\\-> %s - ", trip.waypoints[1].datetime);
    printf("%s\n", trip.waypoints[1].address);
    printf("[%us\t", trip.duration_seconds);
    printf("%.2f€\t", trip.price);
    printf("%s %s %uL\t",
           trip.vehicle.make, trip.vehicle.model, trip.vehicle.trunk_size);
    printf("Total offset: %um]\n", src_offset + dest_offset);
}

void print_trip_costs(
    const struct trip trip, const struct trip *trips, size_t nb_trips)
{
    printf(
        "Cost(∆t)\t%.2f\n",
        map(
            duration_cost(trip),
            min_duration_cost(trips, nb_trips),
            max_duration_cost(trips, nb_trips), 0.0, 1.0));
    printf(
        "Cost( €)\t%.2f\n",
        map(
            price_cost(trip),
            min_price_cost(trips, nb_trips),
            max_price_cost(trips, nb_trips), 0.0, 1.0));
    printf(
        "Cost( ε)\t%.2f\n",
        map(
            offset_cost(trip),
            min_offset_cost(trips, nb_trips),
            max_offset_cost(trips, nb_trips), 0.0, 1.0));
    printf(
        "Cost(DH)\t%.2f\n",
        map(
            depart_hour_cost(trip),
            min_depart_hour_cost(trips, nb_trips),
            max_depart_hour_cost(trips, nb_trips), 0.0, 1.0));
    printf(
        "Cost(TS)\t%.2f\n",
        map(trunk_size_cost(trip),
            min_trunk_size_cost(trips, nb_trips),
            max_trunk_size_cost(trips, nb_trips), 0.0, 1.0));
}

void trip_delete(struct trip *trip)
{
    if (trip->waypoints != NULL)
    {
        free(trip->waypoints);
        trip->waypoints = NULL;
    }

    if (trip->home_waypoints != NULL)
    {
        free(trip->home_waypoints);
        trip->home_waypoints = NULL;
    }
}

struct trip *sort_trips_by_fitness(struct trip *trips, size_t nb_trips)
{
    double curr_trip_fit;
    double next_trip_fit;
    for (size_t i = 0; i < nb_trips; i++)
    {
        for (size_t j = 0; j < nb_trips - 1; j++)
        {
            curr_trip_fit = fitness(trips[j], trips, nb_trips);
            next_trip_fit = fitness(trips[j + 1], trips, nb_trips);
            if (curr_trip_fit < next_trip_fit)
                swap_trips(&trips[j], &trips[j + 1]);
        }
    }

    return trips;
}

void swap_trips(struct trip *trip1, struct trip *trip2)
{
    struct trip trip_tmp = *trip1;
    *trip1 = *trip2;
    *trip2 = trip_tmp;
}

struct trip *update_cfg_home_waypoints(struct trip *trips, size_t nb_trips)
{
    struct waypoint wpnt_home_src = read_home_src_waypoint(
        HOME_WAYPOINTS_CFG_FILENAME);
    struct waypoint wpnt_home_dest = read_home_dest_waypoint(
        HOME_WAYPOINTS_CFG_FILENAME);

    for (size_t i = 0; i < nb_trips; i++)
    {
        trips[i].home_waypoints = malloc(
            sizeof(struct waypoint) * NB_WAYPOINTS);

        trips[i].home_waypoints[0] = wpnt_home_src;
        trips[i].home_waypoints[1] = wpnt_home_dest;
    }

    return trips;
}

uint16_t dist_in_km(
    const struct waypoint waypoint_A, const struct waypoint waypoint_B)
{
    double lat_A = deg2rad(waypoint_A.latitude);
    double long_A = deg2rad(waypoint_A.longitude);
    double lat_B = deg2rad(waypoint_B.latitude);
    double long_B = deg2rad(waypoint_B.longitude);

    double delta_long = long_B - long_A;
    double delta_lat = lat_B - lat_A;

    double a = pow(sin(delta_lat / 2), 2);
    a += cos(lat_A) * cos(lat_B);
    a *= pow(sin(delta_long / 2), 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return (uint32_t)(EARTH_EQUATORIAL_RADIUS_METERS * c);
}

double fitness(
    const struct trip trip, const struct trip *trips, size_t nb_trips)
{
    return 1.0 - trip_cost(trip, trips, nb_trips);
}

double trip_cost(
    const struct trip trip, const struct trip *trips, size_t nb_trips)
{
    double sum = 0.0;
    uint8_t counter = NB_DECISION_KEYS - 1;

    /* add normalized costs */
    sum += duration_cost(trip) / max_duration_cost(trips, nb_trips);
    sum += price_cost(trip) / max_price_cost(trips, nb_trips);
    sum += offset_cost(trip) / max_offset_cost(trips, nb_trips);
    sum += depart_hour_cost(trip) / max_depart_hour_cost(trips, nb_trips);
    if (trip.vehicle.trunk_size != 0)
    {
        sum += trunk_size_cost(trip) / max_trunk_size_cost(trips, nb_trips);
        counter++;
    }

    return sum / (float)counter;
}

double max_trip_cost(const struct trip *trips, size_t nb_trips)
{
    double max_trip_cost = 0.0;
    double cost = 0.0;
    for (size_t i = 0; i < nb_trips; i++)
    {
        cost = trip_cost(trips[i], trips, nb_trips);
        if (cost > max_trip_cost)
            max_trip_cost = cost;
    }

    return max_trip_cost;
}

double duration_cost(const struct trip trip)
{
    return 0.0001 * pow(trip.duration_seconds, 2);
}

double price_cost(const struct trip trip)
{
    return exp(0.25 * trip.price) - 1;
}

double offset_cost(const struct trip trip)
{
    uint32_t src_offset = dist_in_km(
        trip.waypoints[0], trip.home_waypoints[0]);
    uint32_t dest_offset = dist_in_km(
        trip.waypoints[1], trip.home_waypoints[1]);

    return src_offset + dest_offset;
}

double depart_hour_cost(const struct trip trip)
{
    /* normal distribution */
    return -24.0 * exp(-0.25 * pow(trip.depart_hour - TARGET_HOUR, 2)) + 24.0;
}

double trunk_size_cost(const struct trip trip)
{
    return 1000 / (0.001 * trip.vehicle.trunk_size);
}

double max_duration_cost(const struct trip *trips, size_t nb_trips)
{
    double max_duration_cost = 0.0;
    double cost = 0.0;
    for (size_t i = 0; i < nb_trips; i++)
    {
        cost = duration_cost(trips[i]);
        if (cost > max_duration_cost)
            max_duration_cost = cost;
    }

    return max_duration_cost;
}

double max_price_cost(const struct trip *trips, size_t nb_trips)
{
    double max_price_cost = 0.0;
    double cost = 0.0;
    for (size_t i = 0; i < nb_trips; i++)
    {
        cost = price_cost(trips[i]);
        if (cost > max_price_cost)
            max_price_cost = cost;
    }

    return max_price_cost;
}

double max_offset_cost(const struct trip *trips, size_t nb_trips)
{
    double max_offset_cost = 0.0;
    double cost = 0.0;
    for (size_t i = 0; i < nb_trips; i++)
    {
        cost = offset_cost(trips[i]);
        if (cost > max_offset_cost)
            max_offset_cost = cost;
    }

    return max_offset_cost;
}

double max_depart_hour_cost(const struct trip *trips, size_t nb_trips)
{
    double max_depart_hour_cost = 0.0;
    double cost = 0.0;
    for (size_t i = 0; i < nb_trips; i++)
    {
        cost = depart_hour_cost(trips[i]);
        if (cost > max_depart_hour_cost)
            max_depart_hour_cost = cost;
    }

    return max_depart_hour_cost;
}

double max_trunk_size_cost(const struct trip *trips, size_t nb_trips)
{
    double max_trunk_size_cost = 0.0;
    double cost = 0.0;
    for (size_t i = 0; i < nb_trips; i++)
    {
        if (trunk_size_is_undefined(trips[i].vehicle))
            continue;

        cost = trunk_size_cost(trips[i]);
        if (cost > max_trunk_size_cost)
            max_trunk_size_cost = cost;
    }

    return max_trunk_size_cost;
}

double min_duration_cost(const struct trip *trips, size_t nb_trips)
{
    double min_duration_cost = INFINITY;
    double cost = 0.0;
    for (size_t i = 0; i < nb_trips; i++)
    {
        cost = duration_cost(trips[i]);
        if (cost < min_duration_cost)
            min_duration_cost = cost;
    }

    return min_duration_cost;
}

double min_price_cost(const struct trip *trips, size_t nb_trips)
{
    double min_price_cost = INFINITY;
    double cost = 0.0;
    for (size_t i = 0; i < nb_trips; i++)
    {
        cost = price_cost(trips[i]);
        if (cost < min_price_cost)
            min_price_cost = cost;
    }

    return min_price_cost;
}

double min_offset_cost(const struct trip *trips, size_t nb_trips)
{
    double min_offset_cost = INFINITY;
    double cost = 0.0;
    for (size_t i = 0; i < nb_trips; i++)
    {
        cost = offset_cost(trips[i]);
        if (cost < min_offset_cost)
            min_offset_cost = cost;
    }

    return min_offset_cost;
}

double min_depart_hour_cost(const struct trip *trips, size_t nb_trips)
{
    double min_depart_hour_cost = INFINITY;
    double cost = 0.0;
    for (size_t i = 0; i < nb_trips; i++)
    {
        cost = depart_hour_cost(trips[i]);
        if (cost < min_depart_hour_cost)
            min_depart_hour_cost = cost;
    }

    return min_depart_hour_cost;
}

double min_trunk_size_cost(const struct trip *trips, size_t nb_trips)
{
    double min_trunk_size_cost = INFINITY;
    double cost = 0.0;
    for (size_t i = 0; i < nb_trips; i++)
    {
        if (trunk_size_is_undefined(trips[i].vehicle))
            continue;

        cost = trunk_size_cost(trips[i]);
        if (cost < min_trunk_size_cost)
            min_trunk_size_cost = cost;
    }

    return min_trunk_size_cost;
}