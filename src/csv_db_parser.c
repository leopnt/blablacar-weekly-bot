#include "csv_db_parser.h"

struct vehicle *read_vehicles(
    struct vehicle *vehicles_buf, const char *filename)
{
    FILE *csv_file = fopen(filename, "r");
    char *line = NULL;
    size_t len = 0;
    size_t read;

    if (csv_file == NULL)
    {
        perror(filename);
        exit(1);
    }

    size_t i = 0;
    char **parsing_line = create_parsing_line();
    while ((read = getline(&line, &len, csv_file)) != EOF)
    {
        parse_vehicle_line(parsing_line, line);
        vehicles_buf[i] = make_vehicle(
            parsing_line[0], parsing_line[1], (uint16_t)atoi(parsing_line[2]));
        i++;
    }
    delete_parsing_line(parsing_line);

    fclose(csv_file);
    if (line)
        free(line);

    return vehicles_buf;
}

char **create_parsing_line()
{
    size_t mem_size = (sizeof(char) * MAX_LINE_SIZE) * NB_WORD_PER_LINE;
    char **parsing_line = malloc(mem_size);

    for (uint8_t i = 0; i < NB_WORD_PER_LINE; i++)
        parsing_line[i] = malloc(sizeof(char) * MAX_LINE_SIZE);

    return parsing_line;
}

char **parse_vehicle_line(char **parsing_line_buf, const char *line)
{
    char c;
    uint8_t line_idx = 0;
    char *word;
    uint8_t word_idx = 0;
    uint8_t word_char_idx = 0;

    while ((c = line[line_idx]) != '\n')
    {
        word = parsing_line_buf[word_idx];
        word[word_char_idx] = toupper(c);

        if (c == CSV_SEPARATOR)
        {
            word[word_char_idx] = '\0';
            word_char_idx = -1;
            word_idx++;
        }

        word_char_idx++;
        line_idx++;
    }

    return parsing_line_buf;
}

void delete_parsing_line(char **parsing_line)
{
    for (uint8_t i = 0; i < NB_WORD_PER_LINE; i++)
        free(parsing_line[i]);

    free(parsing_line);
}

uint16_t get_trunk_size(
    const char *make, const char *model, const struct vehicle *known_vehicles)
{
    for (size_t i = 0; i < DB_NB_VEHICLES; i++)
    {
        struct vehicle known_vehicle = known_vehicles[i];
        char *make_match = strstr(make, known_vehicle.make);
        char *model_match = strstr(model, known_vehicle.model);

        if (make_match && model_match)
            return known_vehicle.trunk_size;
    }

    return 0;
}