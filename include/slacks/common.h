#ifndef SLACKS_COMMON_H
#define SLACKS_COMMON_H

#include <stddef.h>
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
#define HEADER_BEGIN extern "C"{
#define HEADER_END };
#else
#define HEADER_BEGIN
#define HEADER_END
#endif

HEADER_BEGIN

uint32_t murmur(char* str, size_t len);

static inline uint32_t
murmur_str(char* str){
    return murmur(str, strlen(str));
}

typedef struct{
    char* name;
    uint32_t value;
} symbol;

symbol* symbol_new(char* data);
int symbol_equals(symbol* self, char* name);
int symbol_equal(symbol* self, symbol* other);

typedef struct {
    char* data;
    size_t size;
    size_t asize;
} sstream;

#define SSTREAM_INIT_SIZE 1024
#define SSTREAM_GROW_SIZE 10

sstream* sstream_new();
char* sstream_cstr(sstream* self);
char sstream_getc(sstream* self, size_t index);
void sstream_free(sstream* self);
void sstream_grow(sstream* self, size_t len);
void sstream_put(sstream* self, char* data, size_t size);
void sstream_putc(sstream* self, char data);
void sstream_printf(sstream* self, char* fmt, ...);

static inline sstream*
sstream_wrap(char* data){
    sstream* stream = sstream_new();
    if(stream){
        sstream_put(stream, data, strlen(data));
    }
    return stream;
}

static inline void
sstream_puts(sstream* self, sstream* other){
    sstream_put(self, other->data, other->size);
}

static inline void
sstream_putstr(sstream* self, char* str){
    sstream_put(self, str, strlen(str));
}

HEADER_END

#endif