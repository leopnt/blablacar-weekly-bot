#ifndef BOT_H
#define BOT_H

#include <time.h>
#include <unistd.h>

#include "api_caller.h"
#include "trip.h"
#include "json_api_parser.h"
#include "csv_db_parser.h"
#include "email_sender.h"

#define RESPONSE_JSON_FILENAME "output_file.json"
#define CREDENTIALS_FILENAME "credentials.txt"

#define BOT_SLEEP_INTERVAL_SEC 86400 /* 24 hours */

#define SUNDAY 0

struct bot
{
    double best_day_trip_fitness;
    double best_weekly_trip_fitness;
    char best_day_trip_url[URL_MAX_SIZE];
    char best_weekly_trip_url[URL_MAX_SIZE];
};

void bot_fetch_best_trip(struct bot *bot);
bool is_best_trip_new_best(const struct bot bot);
void update_weekly_best_trip(struct bot *bot);
void bot_sleep();
void bot_send_email(const struct bot bot);
bool should_start_new_week(const struct bot bot);
void bot_weekly_reset(struct bot *bot);

#endif