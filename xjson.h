#ifndef X_JSON_H_
#define X_JSON_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define X_parse_value_int(x) atoi((x))
#define X_parse_value_str(x) (x)

#define XINT(name, string) X(int,         name, string, "%d", X_parse_value_int, ...)
#define XSTR(name, string) X(const char*, name, string, "%s", X_parse_value_str, ...)

#define X_CONCAT(x, y) x ## y
#define X_PAND(x) x

#define X_JSON_PARSER_DEFAULT_STRUCT(struct_name) \
    struct_name X_CONCAT(X_json_default_struct_, struct_name) ()

#define X_JSON_PARSE(struct_name) \
    void X_CONCAT(X_json_parse_, struct_name) (const char* tokens[], size_t tokens_size, struct_name* j)

#define X_JSON_PRINT(struct_name) \
    void X_CONCAT(X_json_print_, struct_name) (struct_name* j)

#endif // X_JSON_H_

#ifdef XNAME
    #define X(type, name, ...) type name;
    typedef struct {
    #ifdef XFIELDS
        XFIELDS
    #endif
    } XNAME;
    #undef X
#else
    #error "MISSING JSON STRUCT NAME\n"
#endif

X_JSON_PARSER_DEFAULT_STRUCT(XNAME)
{
#ifdef XFIELDS
    XNAME j = {0};
#else
    XNAME j;
#endif
    return j;
}

X_JSON_PARSE(XNAME)
{
    #define X(type, name, string, fmt, parser, ...) \
    if (0 == strcmp(tokens[i], string)) {           \
        j->name = parser(tokens[i+1]);              \
    }
    for (size_t i = 0; i < tokens_size; i+=2) {
        #ifdef XFIELDS
        XFIELDS
        #else
        continue;
        #endif
    }
    #undef X
}

X_JSON_PRINT(XNAME)
{
    #define X(type, name, string, fmt, ...)        \
    printf("    %s: " fmt "\n", string, j->name);
    printf("{\n");
    XFIELDS
    printf("}\n");
    #undef X
}

#undef XNAME
#undef XFIELDS
