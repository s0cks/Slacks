#include <slacks/slack/slack_webclient.h>

slack_webclient*
slack_webclient_new(char* id, char* secret){
    slack_webclient* client = malloc(sizeof(slack_webclient));
    if(client == NULL){
        fprintf(stderr, "Out of memory\n");
        abort();
    }
    client->id = strdup(id);
    client->secret = strdup(secret);
    client->http = http_client_new();
/*
    sstream* tail = sstream_new();
    sstream_printf(tail, "client_id=%s&scope=client", id);
    char* url = concat(SLACK_AUTHORIZE, sstream_cstr(tail));
    sstream_free(tail);
    printf("[Slacks]: %s\n", url);
    printf("[Slack]: %s\n", sstream_cstr(http_get(client->http, url)));
    free(url);
    */
    return client;
}

void
slack_webclient_authorize(slack_webclient* self){
    sstream* tail = sstream_new();
    sstream_printf(tail, "client_id=%s&scope=client", self->id);
    char* url = concat(SLACK_AUTHORIZE, sstream_cstr(tail));
    http_get(self->http, url);
    sstream_free(tail);
    free(url);
}