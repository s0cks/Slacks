#include <stdlib.h>
#include <slacks/common.h>
#include <stdarg.h>
#include <stdio.h>

sstream*
sstream_new(){
    sstream* ret = malloc(sizeof(sstream));
    if(ret){
        ret->data = malloc(sizeof(char) * SSTREAM_INIT_SIZE);
        ret->size = 0;
        ret->asize = SSTREAM_INIT_SIZE;
    }
    return ret;
}

char*
sstream_cstr(sstream* self){
    if(self->size < self->asize && self->data[self->size] == 0){
        return self->data;
    }
    sstream_grow(self, self->size + 1);
    self->data[self->size] = 0;
    return self->data;
}

char
sstream_getc(sstream* self, size_t index){
    if(index > self->size || index < 0){
        return 0;
    }
    return self->data[index];
}

void
sstream_free(sstream* self){
    if(!self){
        return;
    }

    free(self->data);
    free(self);
}

void
sstream_grow(sstream* self, size_t nsize){
    if(self->asize > nsize){
        return;
    }
    size_t nasize = self->asize + SSTREAM_GROW_SIZE;
    while(nasize < nsize){
        nasize += SSTREAM_GROW_SIZE;
    }
    self->data = realloc(self->data, nasize);
    self->asize = nasize;
}

void
sstream_put(sstream* self, char* data, size_t len){
    if(self->size + len > self->asize){
        sstream_grow(self, self->size + len);
    }
    memcpy(self->data + self->size, data, len);
    self->size += len;
}

void
sstream_putc(sstream* self, char data){
    if(self->size + 1 >= self->asize){
        sstream_grow(self, self->size + 1);
    }
    self->data[self->size++] = data;
}

void
sstream_printf(sstream* self, char* fmt, ...){
    if(self->size + 1 >= self->asize){
        sstream_grow(self, self->size + 1);
    }
    va_list list;
    va_start(list, fmt);
    int len = vsnprintf(self->data + self->size, self->asize - self->size, fmt, list);
    va_end(list);
    if(len < 0){
        return;
    }
    if(len >= self->asize - self->size){
        sstream_grow(self, self->size + len + 1);
        va_start(list, fmt);
        len = vsnprintf(self->data + self->size, self->asize - self->size, fmt, list);
        va_end(list);
    }
    if(len < 0){
        return;
    }
    self->size += len;
}