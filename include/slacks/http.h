#ifndef SLACKS_HTTP_H
#define SLACKS_HTTP_H

#include <curl/curl.h>
#include "common.h"

HEADER_BEGIN

typedef struct{
    CURL* curl;
    int code;
} http_client;

http_client* http_client_new();

sstream* http_get(http_client* client, char* url);
char* http_encode(http_client* client, char* url);

HEADER_END

#endif