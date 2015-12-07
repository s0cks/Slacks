#include <slacks/http.h>
#include <stdlib.h>
#include <slacks/common.h>

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
    size_t len = size * nmemb;
    sstream* stream = userp;
    if(stream->size + len > stream->asize){
        sstream_grow(stream, stream->size + len);
    }
    memcpy(stream->data + stream->size, buffer, len);
    stream->data[stream->size + len] = '\0';
    stream->size += len;
    return len;
}

char*
http_encode(http_client* client, char* url){
    return curl_easy_escape(client->curl, url, 0);
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
        fprintf(stderr, "Unable to perform curl operation: %s", curl_easy_strerror(res));
        abort();
    }
    curl_easy_getinfo(client->curl, CURLINFO_RESPONSE_CODE, &client->code);
    return buffer;
}