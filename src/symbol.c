#include <slacks/common.h>
#include <stdlib.h>

symbol*
symbol_new(char* data){
    symbol* sym = malloc(sizeof(symbol));
    sym->name = strdup(data);
    sym->value = murmur_str(data);
    return sym;
}

int
symbol_equals(symbol* self, char* name){
    uint32_t hash = murmur_str(name);
    if(self->value == hash){
        return strcmp(self->name, name) == 0;
    }
    return 0;
}

int
symbol_equal(symbol* self, symbol* other){
    if(self->value == other->value){
        return strcmp(self->name, other->name) == 0;
    }
    return 0;
}