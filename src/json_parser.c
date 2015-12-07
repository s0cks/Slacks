#include <slacks/common.h>
#include <slacks/json.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct{
    sstream* content;
    long pos;
} json_parser;

int
iswhitespace(char c){
    switch(c){
        case '\n':
        case '\t':
        case '\r':
        case ' ':{
            return 1;
        }
        default:{
            return 0;
        }
    }
}

char
parser_peek(json_parser* parser){
    return sstream_getc(parser->content, parser->pos);
}

char
parser_next(json_parser* parser){
    if(parser->pos >= parser->content->size){
        return '\0';
    }
    return sstream_getc(parser->content, ++parser->pos);
}

char
parser_next_real(json_parser* parser){
    char next;
    while(iswhitespace(next = parser_next(parser)));
    return next;
}

void
parser_skip(json_parser* parser, size_t len){
    for(size_t i = 0; i < len; i++){
        parser_next(parser);
    }
}

sstream*
parse_name(json_parser* parser){
    sstream* name = sstream_new();
    char next;
    while((next = parser_next(parser)) != '"'){
        sstream_putc(name, next);
    }
    return name;
}

json_obj*
parse_string(json_parser* parser){
    sstream* buffer = sstream_new();
    char next;
    while((next = parser_next(parser)) != '"'){
        switch(next){
            case '\\':{
                switch(next = parser_next(parser)){
                    case '\\': sstream_putc(buffer, '\\'); break;
                    case 't': sstream_putc(buffer, '\t'); break;
                    case 'n': sstream_putc(buffer, '\n'); break;
                    default: sstream_putc(buffer, next); break;
                }
                break;
            }
            default:{
                sstream_putc(buffer, next);
                break;
            }
        }
    }
    return json_string_stream(buffer);
}

json_obj*
parse_number(json_parser* parser, char next){
    sstream* buffer = sstream_new();
    sstream_putc(buffer, next);
    int real = 0;
    while((isdigit(next = parser_peek(parser))) || (next == '.' && !real)){
        if(next == '.'){
            real = 1;
        }
        sstream_putc(buffer, next);
        parser_next(parser);
    }
    json_obj* obj = (real ? json_real_new(atof(sstream_cstr(buffer))) : json_integer_new(atoi(sstream_cstr(buffer))));
    sstream_free(buffer);
    return obj;
}

json_obj*
parse_null(json_parser* parser){
    parser_skip(parser, 3);
    return json_null();
}

json_obj*
parse_boolean(json_parser* parser, char next){
    switch(next){
        case 't': parser_skip(parser, 3); return json_true();
        case 'f': parser_skip(parser, 4); return json_false();
        default:{
            fprintf(stderr, "Unknown token: %c\n", next);
            abort();
        }
    }
}

json_obj* parse_obj(json_parser* parser);

json_obj*
parse_array(json_parser* parser){
    json_obj* array = json_array_new();
    char next;
    while(((next = parser_next_real(parser)) != '\0' || next != ']')){
        if(next == '"'){
            json_array_push(array, parse_string(parser));
        } else if(isdigit(next)){
            json_array_push(array, parse_number(parser, next));
        } else if(next == 't' || next == 'f'){
            json_array_push(array, parse_boolean(parser, next));
        } else if(next == 'n'){
            json_array_push(array, parse_null(parser));
        } else if(next == '['){
            json_array_push(array, parse_array(parser));
        } else if(next == '{'){
            json_array_push(array, parse_obj(parser));
        } else if(next == ','){
            // Fallthrough
        } else if(next == ']'){
            break;
        }
    }
    return array;
}

json_obj*
parse_obj(json_parser* parser){
    json_obj* obj = json_object_new();
    sstream* name = NULL;
    char next;
    while(((next = parser_next_real(parser)) != '}' || next != '\0')){
        switch(next){
            case '"':{
                name = parse_name(parser);
                next = parser_next_real(parser);
                break;
            }
            case ':': next = parser_next_real(parser); break;
            case ',': continue;
            case '{': continue;
            case '}': goto finished;
            default:{
                fprintf(stderr, "Unknown token: %c\n", next);
                abort();
            }
        }

        if(next == ':'){
            next = parser_next_real(parser);
        }

        if(next == '"'){
            json_object_set(obj, sstream_cstr(name), parse_string(parser));
        } else if(isdigit(next)){
            json_object_set(obj, sstream_cstr(name), parse_number(parser, next));
        } else if(next == 'n'){
            json_object_set(obj, sstream_cstr(name), parse_null(parser));
        } else if(next == 't' || next == 'f'){
            json_object_set(obj, sstream_cstr(name), parse_boolean(parser, next));
        } else if(next == '{'){
            json_object_set(obj, sstream_cstr(name), parse_obj(parser));
        } else if(next == '['){
            json_object_set(obj, sstream_cstr(name), parse_array(parser));
        }
    }

    finished:
    return obj;
}

json_obj*
json_parse(sstream** stream){
    json_parser* parser = malloc(sizeof(json_parser));
    if(parser == NULL){
        fprintf(stderr, "Out of memory\n");
        abort();
    }
    parser->content = *stream;
    parser->pos = 0;

    json_obj* obj;
    switch(parser_peek(parser)){
        case '{': obj = parse_obj(parser); break;
        case '[': obj = parse_array(parser); break;
        default: fprintf(stderr, "Unknown char: %c\n", parser_peek(parser)); obj = NULL; break;
    }
    free(parser);
    return obj;
}