#ifndef SLACKS_JDOC_H
#define SLACKS_JDOC_H

#include "common.h"

HEADER_BEGIN

typedef enum{
    JS_OBJECT,
    JS_ARRAY,
    JS_STRING,
    JS_INTEGER,
    JS_REAL,
    JS_TRUE,
    JS_FALSE,
    JS_NULL
} json_type;

typedef struct{
    json_type type;
} json_obj;

#define json_typeof(json) ((json)->type)
#define json_is_object(json) ((json) && json_typeof(json) == JS_OBJECT)
#define json_is_array(json) ((json) && json_typeof(json) == JS_ARRAY)
#define json_is_string(json) ((json) && json_typeof(json) == JS_STRING)
#define json_is_integer(json) ((json) && json_typeof(json) == JS_INTEGER)
#define json_is_real(json) ((json) && json_typeof(json) == JS_REAL)
#define json_is_true(json) ((json) && json_typeof(json) == JS_TRUE)
#define json_is_false(json) ((json) && json_typeof(json) == JS_FALSE)
#define json_is_null(json) ((json) && json_typeof(json) == JS_NULL)
#define json_is_number(json) (json_is_real(json) || json_is_integer(json))
#define json_is_boolean(json) ((json_is_false(json) || json_is_true(json)))

json_obj* json_object_new();
json_obj* json_array_new();
json_obj* json_string_new(char* value);
json_obj* json_string_stream(sstream* stream);
json_obj* json_integer_new(int value);
json_obj* json_real_new(double value);
json_obj* json_null();
json_obj* json_true();
json_obj* json_false();

void json_free(json_obj* obj);

void json_object_set(json_obj* obj, char* key, json_obj* value);
void json_array_set(json_obj* obj, size_t index, json_obj* value);
void json_array_push(json_obj* obj, json_obj* value);
int json_boolean_value(json_obj* obj);
int json_integer_value(json_obj* obj);
double json_real_value(json_obj* obj);
sstream* json_string_value(json_obj* obj);
json_obj* json_object_get(json_obj* obj, char* key);
json_obj* json_array_get(json_obj* obj, size_t index);

json_obj* json_parse(sstream** stream);

char* json_tostring(json_obj* obj);

HEADER_END

#endif