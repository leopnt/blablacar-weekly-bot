#ifndef API_CALLER_H
#define API_CALLER_H

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "waypoint.h"
#include "trip.h"

#define URL_MAX_SIZE 512
#define RESPONSE_MAX_SIZE 65536

#define API_PAGE_COUNT "80"

#define DOWNLOADED_JSON_FILENAME "trips.json"
#define API_KEY_FILENAME "api_key.txt"
#define API_KEY_BUF_SIZE 128

#define DEPARTURE_DATE "2022-02-04"

struct api_caller
{
    char url[URL_MAX_SIZE];
    char response[RESPONSE_MAX_SIZE];
};

char *write_response_from_json_file(char *api_response, const char *filename);

char *write_response_from_url(char *api_response, const char *api_url);
static size_t read_callback(
    char *buffer, size_t size, size_t nitems, void *userdata);

char *make_url(char url[URL_MAX_SIZE]);
char *read_api_key(char api_key_dest[API_KEY_BUF_SIZE], const char *filename);

#endif