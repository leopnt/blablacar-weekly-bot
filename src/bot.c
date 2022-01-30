#include "bot.h"

void bot_fetch_best_trip(struct bot *bot)
{
    struct api_caller api_caller;
    make_url(api_caller.url);
    write_response_from_url(api_caller.response, api_caller.url);
    write_response_from_json_file(
        api_caller.response, DOWNLOADED_JSON_FILENAME);

    struct csv_db_parser csv_db_parser;
    csv_db_parser.vehicles = malloc(sizeof(struct vehicle) * DB_NB_VEHICLES);
    read_vehicles(csv_db_parser.vehicles, "./db/car_db_filtered.csv");

    size_t trip_count = get_trip_count(api_caller.response);
    struct trip *trips = malloc(sizeof(struct trip) * trip_count);
    read_trips(trips, api_caller.response, trip_count, csv_db_parser.vehicles);
    update_cfg_home_waypoints(trips, trip_count);

    sort_trips_by_fitness(trips, trip_count);

    bot->best_day_trip_fitness = fitness(trips[0], trips, trip_count);
    strcpy(bot->best_day_trip_url, trips[0].link);

    /* verbose infos */
    struct trip best_trip = trips[0];
    print_trip(best_trip);
    print_trip_costs(best_trip, trips, trip_count);
    printf("FITNESS: %2.2f\n\n", fitness(best_trip, trips, trip_count));

    for (size_t i = 0; i < trip_count; i++)
    {
        /* delete allocated waypoints */
        trip_delete(&trips[i]);
    }

    free(trips);
    free(csv_db_parser.vehicles);
}

bool is_best_trip_new_best(const struct bot bot)
{
    int cmp_result = strcmp(bot.best_day_trip_url, bot.best_weekly_trip_url);
    bool same_url = (cmp_result == 0);

    if (bot.best_day_trip_fitness != bot.best_weekly_trip_fitness && !same_url)
        return true;

    return false;
}

void update_weekly_best_trip(struct bot *bot)
{
    strcpy(bot->best_weekly_trip_url, bot->best_day_trip_url);
    bot->best_weekly_trip_fitness = bot->best_day_trip_fitness;
}

void bot_sleep()
{
    sleep(BOT_SLEEP_INTERVAL_SEC);
}

void bot_send_email(const struct bot bot)
{
    FILE *credentials_file = fopen(CREDENTIALS_FILENAME, "r");
    char *line = NULL;
    size_t len = 0;
    char username[64] = "";
    char password[64] = "";

    if (credentials_file == NULL)
    {
        perror(CREDENTIALS_FILENAME);
        exit(1);
    }

    size_t i = 0;
    /* read username and password lines */
    while (getline(&line, &len, credentials_file) != EOF)
    {
        if (i == 0)
            strcpy(username, line);
        if (i == 1)
            strcpy(password, line);

        i++;
    }
    username[strcspn(username, "\n")] = '\0'; /* remove \n */

    fclose(credentials_file);
    if (line)
        free(line);

    char email_addr[64];
    sprintf(email_addr, "<%s>", username);
    char fitness_str_buf[64] = "";
    sprintf(fitness_str_buf, "%f", bot.best_day_trip_fitness);

    /* make mail payload text */
    char payload_text[1024] = "";
    strcat(payload_text, "To: ");
    strcat(payload_text, email_addr);
    strcat(payload_text, "\r\n");
    strcat(payload_text, "From: ");
    strcat(payload_text, email_addr);
    strcat(payload_text, "\r\n");
    strcat(payload_text, "Subject: BBCBOT new best trip\r\n");
    /* empty line to divide headers from body, see RFC5322 */
    strcat(payload_text, "\r\n");
    strcat(payload_text, fitness_str_buf);
    strcat(payload_text, "\r\n");
    strcat(payload_text, bot.best_day_trip_url);
    strcat(payload_text, "\r\n");

    send_email(email_addr, email_addr, username, password, payload_text);
}

bool should_start_new_week(const struct bot bot)
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    mktime(timeinfo); /* set week day */

    if (timeinfo->tm_wday == SUNDAY)
        return true;

    return false;
}

void bot_weekly_reset(struct bot *bot)
{
    bot->best_day_trip_fitness = 0;
    strcpy(bot->best_day_trip_url, "");
    bot->best_weekly_trip_fitness = 0;
    strcpy(bot->best_weekly_trip_url, "");
}
