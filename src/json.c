#include <slacks/common.h>
#include <slacks/json.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct json_object_entry{
    struct json_object_entry* next;
    symbol* key;
    json_obj* value;
} json_object_entry;

typedef struct{
    json_obj type;
    json_object_entry* root;
} json_object;

typedef struct{
    json_obj type;
    json_obj** table;
    size_t size;
    size_t asize;
} json_array;

typedef struct{
    json_obj type;
    sstream* value;
} json_string;

typedef struct{
    json_obj type;
    double value;
} json_real;

typedef struct{
    json_obj type;
    int value;
} json_integer;

json_obj null_obj = { JS_NULL };
json_obj false_obj = { JS_FALSE };
json_obj true_obj = { JS_TRUE };

static inline void
ensure(json_obj* obj){
    if(obj == NULL){
        fprintf(stderr, "Out of memory\n");
        abort();
    }
}

json_obj*
json_object_new(){
    json_object* obj = malloc(sizeof(json_object));
    ensure(obj);
    obj->type.type = JS_OBJECT;
    obj->root = NULL;
    return &obj->type;
}

json_obj*
json_array_new(){
    json_array* obj = malloc(sizeof(json_array));
    ensure(obj);
    obj->type.type = JS_ARRAY;
    obj->table = malloc(sizeof(json_obj*) * 16);
    obj->size = 0;
    obj->asize = 16;
    for(size_t i = 0; i < 16; i++){
        obj->table[i] = NULL;
    }
    return &obj->type;
}

json_obj*
json_string_new(char* value){
    json_string* obj = malloc(sizeof(json_string));
    ensure(obj);
    obj->type.type = JS_STRING;
    obj->value = sstream_wrap(value);
    return &obj->type;
}

json_obj*
json_string_stream(sstream* value){
    json_string* obj = malloc(sizeof(json_string));
    ensure(obj);
    obj->type.type = JS_STRING;
    obj->value = value;
    return &obj->type;
}

json_obj*
json_integer_new(int value){
    json_integer* obj = malloc(sizeof(json_integer));
    ensure(obj);
    obj->type.type = JS_INTEGER;
    obj->value = value;
    return &obj->type;
}

json_obj*
json_real_new(double value){
    json_real* obj = malloc(sizeof(json_real));
    ensure(obj);
    obj->type.type = JS_REAL;
    obj->value = value;
    return &obj->type;
}

json_obj*
json_true(){
    return &true_obj;
}

json_obj*
json_false(){
    return &false_obj;
}

json_obj*
json_null(){
    return &null_obj;
}

#ifndef container_of
#define container_of(ptr_, type_, member_)({ \
    const typeof(((type_*) 0)->member_)* __mbptr = ((void*) ptr_); \
    (type_*)((char*) __mbptr - offsetof(type_, member_)); \
})
#endif

char*
json_tostring(json_obj* obj){
    if(obj == NULL){
        return "NULL";
    }

    switch(obj->type){
        case JS_NULL: return "null";
        case JS_FALSE: return "false";
        case JS_TRUE: return "true";
        case JS_INTEGER:{
            char* data = malloc(sizeof(char) * 10);
            snprintf(data, 10, "%d", container_of(obj, json_integer, type)->value);
            return data;
        };
        case JS_REAL:{
            char* data = malloc(sizeof(char) * 10);
            snprintf(data, 10, "%lf", container_of(obj, json_real, type)->value);
            return data;
        };
        case JS_ARRAY:{
            sstream* stream = sstream_new();
            sstream_putc(stream, '[');
            json_array* array = container_of(obj, json_array, type);
            for(size_t i = 0; i < array->asize; i++){
                if(array->table[i] != NULL){
                    sstream_printf(stream, "%s,", json_tostring(array->table[i]));
                }
            }
            char* ret = strdup(sstream_cstr(stream));
            sstream_free(stream);
            return ret;
        };
        case JS_OBJECT:{
            sstream* stream = sstream_new();
            sstream_putc(stream, '{');
            json_object* object = container_of(obj, json_object, type);
            json_object_entry* entry = object->root;
            while(entry){
                sstream_printf(stream, "%s: %s,", entry->key->name, json_tostring(entry->value));
                entry = entry->next;
            }
            char* ret = strdup(sstream_cstr(stream));
            sstream_free(stream);
            return ret;
        };
        case JS_STRING:{
            sstream* stream = sstream_new();
            json_string* string = container_of(obj, json_string, type);
            sstream_printf(stream, "\"%s\"", sstream_cstr(string->value));
            char* ret = strdup(sstream_cstr(stream));
            sstream_free(stream);
            return ret;
        };
        default: printf("Unknown Type: %d\n", obj->type); return "<unknown>";
    }
}

void
json_free(json_obj* obj){
    if(obj == NULL){
        return;
    }

    if(json_is_boolean(obj) || json_is_null(obj)){
        return;
    }

    if(json_is_string(obj)){
        json_string* str = container_of(obj, json_string, type);
        sstream_free(str->value);
        free(str);
    } else if(json_is_object(obj)){
        json_object* o = container_of(obj, json_object, type);
        json_object_entry* entry = o->root;
        json_object_entry* tmp = NULL;
        while(entry){
            tmp = entry->next;
            free(entry->key->name);
            free(entry->key);
            json_free(entry->value);
            free(entry);
            entry = tmp;
        }

        free(o);
    } else if(json_is_array(obj)){
        json_array* array = container_of(obj, json_array, type);
        for(size_t i = 0; i < array->asize; i++){
            if(array->table[i] != NULL){
                json_free(array->table[i]);
            }
        }
        free(array->table);
        free(array);
    } else if(json_is_integer(obj)){
        free(container_of(obj, json_integer, type));
    } else if(json_is_real(obj)){
        free(container_of(obj, json_real, type));
    }
}

json_object_entry*
json_new_entry(char* key, json_obj* value){
    json_object_entry* entry = malloc(sizeof(json_object_entry));
    if(entry == NULL){
        fprintf(stderr, "Out of memory\n");
        abort();
    }
    entry->key = symbol_new(key);
    entry->value = value;
    entry->next = NULL;
    return entry;
}

void
json_object_set(json_obj* obj, char* key, json_obj* value){
    json_object* o = container_of(obj, json_object, type);
    json_object_entry* entry = o->root;
    while(entry){
        if(symbol_equals(entry->key, key)){
            entry->value = value;
            return;
        }

        entry = entry->next;
    }

    entry = malloc(sizeof(json_object_entry));
    if(entry == NULL){
        fprintf(stderr, "Out of memory\n");
        abort();
    }
    entry->key = symbol_new(key);
    entry->value = value;
    entry->next = o->root;
    o->root = entry;
}

void
json_array_set(json_obj* obj, size_t index, json_obj* value){
    json_array* array = container_of(obj, json_array, type);
    if(index < 0 || index > array->asize){
        fprintf(stderr, "Index out of bounds: %lu\n", index);
    }
    if(array->table[index] != NULL){
        array->table[index] = value;
    } else{
        array->table[index] = value;
        array->size++;
    }
}

void
json_array_push(json_obj* obj, json_obj* value){
    json_array* array = container_of(obj, json_array, type);
    if(array->size + 1 > array->asize){
        array->table = realloc(array->table, array->asize + 1);
        array->asize++;
    }
    for(size_t i = 0; i < array->asize; i++){
        if(array->table[i] == NULL){
            array->table[i] = value;
            array->size++;
        }
    }
}

int
json_boolean_value(json_obj* obj){
    return json_is_true(obj) ? 1 : 0;
}

int
json_integer_value(json_obj* obj){
    return container_of(obj, json_integer, type)->value;
}

double
json_real_value(json_obj* obj){
    return container_of(obj, json_real, type)->value;
}

sstream*
json_string_value(json_obj* obj){
    return container_of(obj, json_string, type)->value;
}

json_obj*
json_object_get(json_obj* obj, char* key){
    json_object* o = container_of(obj, json_object, type);
    json_object_entry* entry = o->root;
    while(entry){
        if(symbol_equals(entry->key, key)){
            return entry->value;
        }

        entry = entry->next;
    }

    return NULL;
}

json_obj*
json_array_get(json_obj* obj, size_t index){
    json_array* array = container_of(obj, json_array, type);
    if(index < 0 || index > array->asize){
        return NULL;
    }
    return array->table[index];
}