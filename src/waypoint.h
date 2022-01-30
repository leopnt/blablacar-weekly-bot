#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cfg_parser.h"

#define DATETIME_SIZE 32
#define ADDRESS_SIZE 512

#define CONFIG_FILE_LINE_MAX_LENGTH 128
#define WAYPOINT_HOME_SRC_LAT_CFG_VAR_NAME "WAYPOINT_HOME_SRC_LAT"
#define WAYPOINT_HOME_SRC_LONG_CFG_VAR_NAME "WAYPOINT_HOME_SRC_LONG"
#define WAYPOINT_HOME_DEST_LAT_CFG_VAR_NAME "WAYPOINT_HOME_DEST_LAT"
#define WAYPOINT_HOME_DEST_LONG_CFG_VAR_NAME "WAYPOINT_HOME_DEST_LONG"

enum home_waypoint_type
{
    SRC_WAYPOINT,
    DEST_WAYPOINT
};

enum home_coordinates_type
{
    LAT_COORDINATE,
    LONG_COORDINATE,
};

struct waypoint
{
    char datetime[DATETIME_SIZE];
    char address[ADDRESS_SIZE];
    double latitude;
    double longitude;
};

struct waypoint make_waypoint(
    const char *datetime, const char *address, float latitude, float longitude);

float get_waypoint_depart_hour(const struct waypoint waypoint);

struct waypoint read_home_src_waypoint(const char *cfg_filename);
struct waypoint read_home_dest_waypoint(const char *cfg_filename);
struct waypoint *set_home_waypoint_from_cfg_line(
    struct waypoint *home_waypoint,
    char *cfg_line,
    enum home_waypoint_type waypoint_type_to_match);
enum home_waypoint_type get_cfg_line_waypoint_type(const char *cfg_line);
struct waypoint *set_home_waypoint_coordinates_from_cfg_line(
    struct waypoint *home_waypoint, char *cfg_line);
enum home_coordinates_type get_cfg_line_coordinates_type(const char *cfg_line);

#endif