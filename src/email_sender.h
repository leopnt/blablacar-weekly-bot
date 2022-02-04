#ifndef EMAIL_SENDER_H
#define EMAIL_SENDER_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#define SMTPS_SERV_URL "smtps://smtp.gmail.com:465"

struct upload_status
{
    size_t bytes_read;
    const char *payload_text;
};

int send_email(
    const char *from_mail,
    const char *to_mail,
    const char *username,
    const char *password,
    const char *payload_text);

#endif