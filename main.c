#include <slacks/slack/slack_webclient.h>

int main(int argc, char** argv){
    char* client_id = "16093022609.16089129652";
    char* client_secret = "cd8c9365cfdc0e4f09a6586f59196b04";
    slack_webclient* client = slack_webclient_new(client_id, client_secret);
    slack_webclient_authorize(client);
    return 0;
}