#define X(xtype, xname, ...) xtype xname;
typedef struct {
    XFIELDS
} XNAME;
#undef X

X_JSON_PARSER_DEFAULT_STRUCT(XNAME)
{
    #define X(_1, xname, _2, _3, _4, _5, _6, xinit, ...) .xname = xinit,
    XNAME j = {
        XFIELDS    
    };
    #undef X
    return j;
}

X_JSON_STRUCT_FREE(XNAME)
{
    #define X(xtype, xname, xstring, xfmt, xparser, xfree, ...) xfree(j->xname);
    XFIELDS
    #undef X
}

X_JSON_PARSE(XNAME)
{
    X_Parser p = X_parser_new(buf);
    X_parser_element(&p);

    if (p.tokens.len < 1) return;

    #undef XVEC
    #define XVEC(xtype, xname, xstring, xfmt, xparser, ...)                    \
    if (                                                                       \
        strlen(xstring) == array_name.string.len &&                            \
        0 == strncmp(array_name.string.data, xstring, array_name.string.len)   \
    ) {                                                                        \
        carr_vec_append(&(j->xname), xparser(curtok));                         \
    }

    #define X(xtype, xname, xstring, xfmt, xparser, ...)                       \
    if (                                                                       \
        strlen(xstring) == curtok.string.len &&                                \
        0 == strncmp(curtok.string.data, xstring, curtok.string.len)           \
    ) {                                                                        \
        j->xname = xparser(nxttok);                                            \
    }

    size_t idx = 0;

    bool array_definition = false;
    X_Token array_name;
    while (idx < p.tokens.len - 1) {
        X_Token curtok = p.tokens.items[idx];
        X_Token nxttok = p.tokens.items[idx+1];


        if (nxttok.type == X_TOKEN_ARRAY_START) {
            assert(("single array JSON not supported\n", idx > 0));
            array_definition = true;
            array_name = curtok;
            idx += 2;
            continue;
        }
        if (curtok.type == X_TOKEN_ARRAY_END) {
            array_definition = false;
            array_name.string.len = 0;
            idx++;
            continue;
        }
        if (curtok.type == X_TOKEN_OBJECT_START) {
            // printf("OBJECT START\n");
            idx++;
            continue;
        }
        if (curtok.type == X_TOKEN_OBJECT_END) {
            // printf("OBJECT END\n");
            idx++;
            continue;
        }

        XFIELDS
        
        if (array_definition) {
            idx += 1;
        } else {
            idx += 2;
        }
    }
    #undef X
    #undef XVEC
    #define XVEC X

    X_parser_free(&p);
}

X_JSON_PRINT(XNAME)
{
    #undef XVEC
    #define XVEC(xtype, xname, xstring, xfmtstr, _1, _2, xfmtval, ...) \
    printf("    %s: [\n", #xname);                                     \
    for (size_t i = 0; i < j->xname.len; ++i) {                        \
         printf("        " xfmtstr ",\n", xfmtval(j->xname.items[i])); \
    }                                                                  \
    printf("    ],\n");


    #define X(xtype, xname, xstring, xfmtstr, _1, _2, xfmtval, ...)        \
    printf("    %s: " xfmtstr "\n", #xname, xfmtval(j->xname));

    printf("{\n");
    XFIELDS
    printf("}\n");

    #undef X
    #undef XVEC
    #define XVEC X
}

#undef XNAME
#undef XFIELDS
