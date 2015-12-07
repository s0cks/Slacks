#include <slacks/http.h>
#include <stdlib.h>

http_client*
http_client_new(){
    http_client* client = malloc(sizeof(http_client));
    if(client == NULL){
        fprintf(stderr, "Out of memory\n");
        abort();
    }
    client->curl = curl_easy_init();
    client->code = 0;
}

static size_t
writeData(void* buffer, size_t size, size_t nmemb, void* userp){
    sstream_put((sstream*) userp, buffer, size * nmemb);
    return size * nmemb;
}

sstream*
http_get(http_client* client, char* url){
    sstream* buffer = sstream_new();
    CURLcode res;
    curl_easy_setopt(client->curl, CURLOPT_URL, url);
    curl_easy_setopt(client->curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, &writeData);
    curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, buffer);
    res = curl_easy_perform(client->curl);
    if(res != CURLE_OK){
        fprintf("Unable to perform curl operation: %s", curl_easy_strerror(res));
        abort();
    }
    curl_easy_getinfo(client->curl, CURLINFO_RESPONSE_CODE, &client->code);
    return buffer;
}