#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bot.h"

int main(int argc, char const *argv[])
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    struct bot bot;

    size_t day_nb = 0;
    while (true)
    {
        printf("DAY %zu\n", day_nb);

        bot_fetch_best_trip(&bot);
        if (is_best_trip_new_best(bot))
        {
            printf("NEW BEST!\n");
            printf("%f\n%s\n",
                   bot.best_day_trip_fitness, bot.best_day_trip_url);

            update_weekly_best_trip(&bot);
            bot_send_email(bot);
        }

        printf("BOT ENTERS SLEEP FOR %ds\n", BOT_SLEEP_INTERVAL_SEC);
        bot_sleep();

        if (should_start_new_week(bot))
        {
            bot_weekly_reset(&bot);
            printf("BOT WEEKLY EXIT\n");
            exit(0);
        }

        day_nb++;
    }

    int del = remove(DOWNLOADED_JSON_FILENAME);
    if (del != 0)
    {
        perror("Cannot uncache trips. Please delete manually");
        exit(1);
    }

    curl_global_cleanup();

    return 0;
}
