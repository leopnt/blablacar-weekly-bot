#include "api_caller.h"

char *write_response_from_json_file(char *api_response, const char *filename)
{
    FILE *json_file = fopen(filename, "r");
    if (json_file == NULL)
    {
        perror(filename);
        exit(1);
    }

    fseek(json_file, 0, SEEK_END);
    long fsize = ftell(json_file);
    fseek(json_file, 0, SEEK_SET);

    fread(api_response, fsize, 1, json_file);
    fclose(json_file);

    return api_response;
}

char *write_response_from_url(char *api_response, const char *url)
{
    CURL *curl = curl_easy_init();
    CURLcode res;

    if (!curl)
    {
        perror("curl failed");
        exit(1);
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    FILE *output_file = fopen(DOWNLOADED_JSON_FILENAME, "w");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output_file);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);

    fclose(output_file);
    return api_response;
}

static size_t read_callback(
    char *ptr, size_t size, size_t nmemb, void *userdata)
{
    char *api_response = (char *)userdata;
    strcat(api_response, ptr);

    return size * nmemb;
}

char *make_url(char url[URL_MAX_SIZE])
{
    char api_key[128];
    read_api_key(api_key, "api_key.txt");

    struct waypoint from_waypnt = read_home_src_waypoint(
        HOME_WAYPOINTS_CFG_FILENAME);
    struct waypoint to_waypnt = read_home_dest_waypoint(
        HOME_WAYPOINTS_CFG_FILENAME);

    strcpy(url, "https://public-api.blablacar.com/api/v3/trips?");
    strcat(url, "from_coordinate=");

    char str_buf[64];
    sprintf(str_buf, "%.16f,%.16f",
            from_waypnt.latitude, from_waypnt.longitude);
    strcat(url, str_buf);
    strcat(url, "&to_coordinate=");
    sprintf(str_buf, "%.16f,%.16f",
            to_waypnt.latitude, to_waypnt.longitude);
    strcat(url, str_buf);
    strcat(url, "&locale=fr-FR");
    strcat(url, "&currency=EUR");
    strcat(url, "&start_date_local=" DEPARTURE_DATE "T00:00:00");
    strcat(url, "&count=" API_PAGE_COUNT);
    strcat(url, "&key=");
    strcat(url, api_key);

    return url;
}

char *read_api_key(char api_key_dest[API_KEY_BUF_SIZE], const char *filename)
{
    FILE *api_key_file = fopen(API_KEY_FILENAME, "r");
    if (api_key_file == NULL)
    {
        perror(API_KEY_FILENAME);
        exit(1);
    }

    fgets(api_key_dest, API_KEY_BUF_SIZE, api_key_file);
    fclose(api_key_file);

    return api_key_dest;
}
