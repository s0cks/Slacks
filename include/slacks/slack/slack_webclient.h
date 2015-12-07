#ifndef SLACKS_SLACK_WEBCLIENT_H
#define SLACKS_SLACK_WEBCLIENT_H

#include "../common.h"
#include "../http.h"

HEADER_BEGIN

typedef struct{
    char* id;
    char* secret;
    http_client* http;
} slack_webclient;

#define SLACK_AUTHORIZE "https://slack.com/oauth/authorize?"

slack_webclient* slack_webclient_new(char* id, char* secret);
void slack_webclient_authorize(slack_webclient* self);

HEADER_END

#endif