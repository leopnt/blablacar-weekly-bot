#ifndef CSV_DB_PARSER_H
#define CSV_DB_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "vehicle.h"

#define CSV_SEPARATOR ','
#define DB_NB_VEHICLES 1617
#define NB_WORD_PER_LINE 3
#define MAX_LINE_SIZE 32

struct csv_db_parser
{
    struct vehicle *vehicles;
};

struct vehicle *read_vehicles(
    struct vehicle *vehicles_buf, const char *filename);

char **create_parsing_line();
void delete_parsing_line(char **parsing_line);

char **parse_vehicle_line(char **parsin_line_buf, const char *line);

uint16_t get_trunk_size(
    const char *make, const char *model, const struct vehicle *known_vehicles);

#endif