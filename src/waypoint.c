#include "waypoint.h"

struct waypoint make_waypoint(
    const char *datetime, const char *address, float latitude, float longitude)
{
    struct waypoint waypoint;

    strcpy(waypoint.datetime, datetime);
    strcpy(waypoint.address, address);
    waypoint.latitude = latitude;
    waypoint.longitude = longitude;

    return waypoint;
}

float get_waypoint_depart_hour(const struct waypoint waypoint)
{
    float float_hour = 0.0;

    struct tm t;
    sscanf(waypoint.datetime, "%d-%d-%dT%d:%d:%d",
           &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);

    float_hour = t.tm_hour + ((float)t.tm_min) / 60.0;
    return float_hour;
}

struct waypoint read_home_src_waypoint(const char *filename)
{
    struct waypoint home_src_waypoint;

    FILE *cfg_file = fopen(filename, "r");

    if (cfg_file == NULL)
    {
        perror(filename);
        exit(1);
    }

    char line_buf[CONFIG_FILE_LINE_MAX_LENGTH] = "";
    while (!feof(cfg_file))
    {
        fgets(line_buf, CONFIG_FILE_LINE_MAX_LENGTH, cfg_file);

        set_home_waypoint_from_cfg_line(
            &home_src_waypoint, line_buf, SRC_WAYPOINT);
    }

    fclose(cfg_file);

    return home_src_waypoint;
}

struct waypoint read_home_dest_waypoint(const char *filename)
{
    struct waypoint home_dest_waypoint;

    FILE *cfg_file = fopen(filename, "r");

    if (cfg_file == NULL)
    {
        perror(filename);
        exit(1);
    }

    char line_buf[CONFIG_FILE_LINE_MAX_LENGTH] = "";
    while (!feof(cfg_file))
    {
        fgets(line_buf, CONFIG_FILE_LINE_MAX_LENGTH, cfg_file);

        set_home_waypoint_from_cfg_line(
            &home_dest_waypoint, line_buf, DEST_WAYPOINT);
    }

    fclose(cfg_file);

    return home_dest_waypoint;
}

struct waypoint *set_home_waypoint_from_cfg_line(
    struct waypoint *home_waypoint,
    char *cfg_line,
    enum home_waypoint_type waypoint_type_to_match)
{
    enum home_waypoint_type waypoint_type;
    waypoint_type = get_cfg_line_waypoint_type(cfg_line);

    if (waypoint_type == waypoint_type_to_match)
        set_home_waypoint_coordinates_from_cfg_line(home_waypoint, cfg_line);

    return home_waypoint;
}

enum home_waypoint_type get_cfg_line_waypoint_type(const char *cfg_line)
{
    if (strstr(cfg_line, WAYPOINT_HOME_SRC_LAT_CFG_VAR_NAME))
        return SRC_WAYPOINT;
    if (strstr(cfg_line, WAYPOINT_HOME_SRC_LONG_CFG_VAR_NAME))
        return SRC_WAYPOINT;
    if (strstr(cfg_line, WAYPOINT_HOME_DEST_LAT_CFG_VAR_NAME))
        return DEST_WAYPOINT;
    if (strstr(cfg_line, WAYPOINT_HOME_DEST_LONG_CFG_VAR_NAME))
        return DEST_WAYPOINT;

    return -1;
}

struct waypoint *set_home_waypoint_coordinates_from_cfg_line(
    struct waypoint *home_waypoint, char *cfg_line)
{
    enum home_coordinates_type coordinates_type;
    coordinates_type = get_cfg_line_coordinates_type(cfg_line);

    cfg_line_clear_var_name(cfg_line);
    if (coordinates_type == LAT_COORDINATE)
        home_waypoint->latitude = atof(cfg_line);
    if (coordinates_type == LONG_COORDINATE)
        home_waypoint->longitude = atof(cfg_line);

    return home_waypoint;
}

enum home_coordinates_type get_cfg_line_coordinates_type(const char *cfg_line)
{
    if (strstr(cfg_line, WAYPOINT_HOME_SRC_LAT_CFG_VAR_NAME))
        return LAT_COORDINATE;
    if (strstr(cfg_line, WAYPOINT_HOME_SRC_LONG_CFG_VAR_NAME))
        return LONG_COORDINATE;
    if (strstr(cfg_line, WAYPOINT_HOME_DEST_LAT_CFG_VAR_NAME))
        return LAT_COORDINATE;
    if (strstr(cfg_line, WAYPOINT_HOME_DEST_LONG_CFG_VAR_NAME))
        return LONG_COORDINATE;

    return -1;
}