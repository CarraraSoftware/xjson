#ifndef XJSON_H_
#define XJSON_H_

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define CARR_SV_IMPLEMENTATION
#include "sv.h"
#include "vec.h"

#define X_parse_value_int(x)  (x).type != X_TOKEN_NULL ? carr_sv_parse_int((x).string) : 0
#define X_parse_value_str(x)  (x).type != X_TOKEN_NULL ? carr_sv_to_cstr((x).string) : ""
#define X_parse_value_bool(x) (x).type == X_TOKEN_TRUE ? true : false
#define X_parse_value_arr(x)  X_parse_value_int((x))

#define X_fmt_value_int(x)  x
#define X_fmt_value_str(x)  strlen((x)) > 0 ? (x) : "''"
#define X_fmt_value_bool(x) (x) ? "true" : "false"
#define X_fmt_value_arr(x)  X_fmt_value_int((x))

#define X_free_int(_)
#define X_free_bool(_)
#define X_free_str(x) free((void*)x)
#define X_free_arr(x) carr_vec_free(&(x))

#define X_init_value_int  0
#define X_init_value_str  ""
#define X_init_value_bool false
#define X_init_value_arr  {0}

#define XVEC X
#define XINT(xname, xstring)        X(int,         xname, xstring, "%d", X_parse_value_int,  X_free_int,  X_fmt_value_int,  X_init_value_int, ...)
#define XSTR(xname, xstring)        X(const char*, xname, xstring, "%s", X_parse_value_str,  X_free_str,  X_fmt_value_str,  X_init_value_str, ...)
#define XBOOL(xname, xstring)       X(bool,        xname, xstring, "%s", X_parse_value_bool, X_free_bool, X_fmt_value_bool, X_init_value_bool,...)
#define XARR(xtype, xname, xstring) XVEC(xtype,    xname, xstring, "%d", X_parse_value_arr,  X_free_arr,  X_fmt_value_arr,  X_init_value_arr, ...)

#define X_CONCAT(x, y) x ## y
#define X_PAND(x) x


#define X_JSON_PARSER_DEFAULT_STRUCT(x_struct_name) \
    x_struct_name X_CONCAT(X_json_default_struct_, x_struct_name) ()

#define X_JSON_PARSE(x_struct_name) \
    void X_CONCAT(X_json_parse_, x_struct_name) (x_struct_name* j, CarrStringBuilder *buf)

#define X_JSON_STRUCT_FREE(x_struct_name) \
    void X_CONCAT(X_json_free_, x_struct_name) (x_struct_name* j)

#define X_JSON_PRINT(x_struct_name) \
    void X_CONCAT(X_json_print_, x_struct_name) (x_struct_name* j)    

typedef enum {
    X_TOKEN_STRING,
    X_TOKEN_NUMBER,
    X_TOKEN_OBJECT_START,
    X_TOKEN_OBJECT_END,
    X_TOKEN_ARRAY_START,
    X_TOKEN_ARRAY_END,
    X_TOKEN_TRUE,
    X_TOKEN_FALSE,
    X_TOKEN_NULL,
} X_TokenType;

typedef struct {
    CarrStringView  string;
    X_TokenType     type;
} X_Token;

typedef struct {
    X_Token* items;
    size_t   len;
    size_t   cap;
} X_Tokens;

typedef struct {
    CarrStringBuilder buf;
    size_t            cursor;
    X_Tokens          tokens;
} X_Parser;

X_Token X_token_new(CarrStringView s, X_TokenType t);
void    X_token_print(X_Token t);

X_Parser X_parser_new(CarrStringBuilder *buf);
void     X_parser_free(X_Parser *p);

char X_parser_next(X_Parser* p);
char X_parser_peek(X_Parser* p);
bool X_parser_expect(X_Parser* p, char exp);
bool X_parser_consume_space(X_Parser* p);
bool X_parser_is_eof(X_Parser *p);
bool X_parser_string(X_Parser *p);
bool X_parser_number(X_Parser *p);
bool X_parser_value(X_Parser *p);
bool X_parser_object(X_Parser *p);
bool X_parser_element(X_Parser *p);
bool X_parser_array(X_Parser *p);


#ifdef XJSON_IMPLEMENTATION

X_Parser X_parser_new(CarrStringBuilder* buf)
{
    X_Tokens toks = {0};
    carr_vec_init(&toks);
    X_Parser p = {
        .buf    = *buf,
        .cursor = 0,
        .tokens = toks,
    };
    return p;
}

void X_parser_free(X_Parser* p)
{
    vec_free(&(p->tokens));
    sb_free(&(p->buf));
}


X_Token X_token_new(CarrStringView s, X_TokenType t)
{
    return (X_Token) {
        .string = s,
        .type   = t,
    };
}

void X_token_print(X_Token t)
{
    printf("[%d] ", t.type); sv_printn(t.string);
}

char X_parser_next(X_Parser* p)
{
    return p->buf.data[p->cursor++];
}

char X_parser_peek(X_Parser* p)
{
    return p->buf.data[p->cursor];
}

bool X_parser_expect(X_Parser* p, char exp)
{
    char current = X_parser_next(p);
    if (current != exp) {
        printf("ERROR: expected '%c', but got '%c'\n", exp, current);
        return false;
    }
    return true;
}

bool X_parser_consume_space(X_Parser* p)
{
    while (!X_parser_is_eof(p) && isspace(X_parser_peek(p))) p->cursor++;
    return true;
}

bool X_parser_is_eof(X_Parser *p)
{
    return p->cursor >= p->buf.len;
}

bool X_parser_string(X_Parser *p)
{
    // TODO: parse escape/control characters

    if (!X_parser_consume_space(p)) return false;
    if (!X_parser_expect(p, '"'))   return false;

    size_t strsz = 0;
    while (X_parser_peek(p) != '"') {
        if (X_parser_is_eof(p)) {
            printf("ERROR: unmatched '\"'\n");
            return false;
        }
        p->cursor++;
        strsz++;
    }

    CarrStringView str = {
        .data = p->buf.data + p->cursor - strsz,
        .len  = strsz,
    };

    X_Token tok = X_token_new(str, X_TOKEN_STRING);
    carr_vec_append(&(p->tokens), tok);
    if (!X_parser_expect(p, '"')) return false;

    return true;
}

bool X_parser_number(X_Parser *p)
{
    // TODO: parse number types other than integers

    if (!X_parser_consume_space(p)) return false;

    size_t numsz = 0;
    while (isdigit(X_parser_peek(p))) {
        if (X_parser_is_eof(p)) {
            printf("ERROR: unmatched '\"'\n");
            return false;
        }
        numsz++;
        p->cursor++;
    }
    CarrStringView num = {
        .data = p->buf.data + p->cursor - numsz,
        .len  = numsz,
    };
    X_Token tok = X_token_new(num, X_TOKEN_NUMBER);

    carr_vec_append(&(p->tokens), tok);

    return true;
}

bool X_parser_array(X_Parser *p)
{
    if (!X_parser_consume_space(p)) return false;
    if (!X_parser_expect(p, '['))   return false;
    CarrStringView left_brac = {
        .data = p->buf.data + p->cursor - 1,
        .len  = 1,
    };
    X_Token tok_start = X_token_new(left_brac, X_TOKEN_ARRAY_START);
    carr_vec_append(&(p->tokens), tok_start);
    

    if (!X_parser_consume_space(p)) return false;
    if (X_parser_peek(p) == ']') {
        X_parser_next(p);
        return true;
    }

    X_parser_value(p);
    while (X_parser_peek(p) == ',') {
        X_parser_next(p);
        X_parser_value(p);
    }

    if (!X_parser_expect(p, ']')) return false;
    CarrStringView right_brac = {
        .data = p->buf.data + p->cursor - 1,
        .len  = 1,
    };
    X_Token tok_end = X_token_new(right_brac, X_TOKEN_ARRAY_END);
    carr_vec_append(&(p->tokens), tok_end);
    return true;
}

bool X_parser_literal(X_Parser *p, const char* lit, X_TokenType type)
{
    if (!X_parser_consume_space(p)) return false;

    size_t n = strlen(lit);

    if (p->cursor + n >= p->buf.len) {
        printf("ERROR: JSON overflow\n");
        return false;
    }

    for (size_t i = 0; i < n; ++i) {
        if (p->buf.data[p->cursor+i] != lit[i]) {
            printf("ERROR: expected '%s', got '%.*s'\n", lit, (int)n, p->buf.data + p->cursor);
            return false;
        }
    }

    CarrStringView l = {
        .data = p->buf.data + p->cursor,
        .len  = n,
    };
    p->cursor += n;

    X_Token tok = X_token_new(l, type);
    carr_vec_append(&(p->tokens), tok);
    return true;
}

bool X_parser_null(X_Parser *p)
{
    if (!X_parser_consume_space(p))                 return false;
    if (!X_parser_literal(p, "null", X_TOKEN_NULL)) return false;
    return true;
}

bool X_parser_false(X_Parser *p)
{
    if (!X_parser_consume_space(p))                   return false;
    if (!X_parser_literal(p, "false", X_TOKEN_FALSE)) return false;
    return true;
}

bool X_parser_true(X_Parser *p)
{
    if (!X_parser_consume_space(p))                 return false;
    if (!X_parser_literal(p, "true", X_TOKEN_TRUE)) return false;
    return true;
}

bool X_parser_value(X_Parser *p)
{
    if (!X_parser_consume_space(p)) return false;

    if (X_parser_peek(p) == '"') {
        if (!X_parser_string(p)) return false;
    } else if (isdigit(X_parser_peek(p))) {
        if (!X_parser_number(p)) return false;
    } else if (X_parser_peek(p) == '{') {
        if (!X_parser_object(p)) return false;
    } else if (X_parser_peek(p) == '[') {
        if (!X_parser_array(p)) return false;
    } else if (X_parser_peek(p) == 't') {
        if (!X_parser_true(p)) return false;
    } else if (X_parser_peek(p) == 'f') {
        if (!X_parser_false(p)) return false;
    } else if (X_parser_peek(p) == 'n') {
        if (!X_parser_null(p)) return false;
    } else {
        printf("ERROR: unknown symbol: %c\n", X_parser_peek(p));
        return false;
    }

    if (!X_parser_consume_space(p)) return false;
    return true;
}

bool X_parser_object(X_Parser *p)
{
    X_parser_consume_space(p);
    X_parser_expect(p, '{');

    CarrStringView left_brac = {
        .data = p->buf.data + p->cursor - 1,
        .len  = 1,
    };
    X_Token tok_start = X_token_new(left_brac, X_TOKEN_OBJECT_START);
    carr_vec_append(&(p->tokens), tok_start);

    while (!X_parser_is_eof(p)) {
        if (!X_parser_consume_space(p)) return false;
        if (X_parser_peek(p) == '}') {
            break;
        }

        if (!X_parser_string(p)) return false;

        if (!X_parser_consume_space(p)) return false;
        if (!X_parser_expect(p, ':'))   return false;

        if (!X_parser_consume_space(p)) return false;
        if (!X_parser_value(p))         return false;

        if (!X_parser_consume_space(p)) return false;
        if (X_parser_peek(p) != ',') {
            break;
        }
        X_parser_next(p);
    }
    if (!X_parser_expect(p, '}')) return false;

    CarrStringView right_brac = {
        .data = p->buf.data + p->cursor - 1,
        .len  = 1,
    };
    X_Token tok_end = X_token_new(right_brac, X_TOKEN_OBJECT_END);
    carr_vec_append(&(p->tokens), tok_end);

    return true;
}

bool X_parser_element(X_Parser *p)
{
    if (!X_parser_consume_space(p)) return false;
    if (!X_parser_object(p))        return false;
    if (!X_parser_consume_space(p)) return false;
    return true;
}


#endif // XJSON_IMPLEMENTATION
#endif // XJSON_H_

