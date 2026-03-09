# Quick Start
Define your structure with the XNAME and XFIELDS macros.
Additionally, you can use the XINT and XSTR helpers to define the individual fields.
```C
#define XNAME MyStruct
#define XFIELDS \
    XINT(int_field, "int_field") \
    XSTR(str_field, "str_field") \
#include "xjson.h"
```

The first time you include 'xjson.h', all the helper function will be included,
but also the struct and all the functions necessary for using the just defined fields.
That is, you will access to the struct:
```C
typedef struct {
    int         int_field;
    const char* str_field;
} MyStruct
```
and the functions:
```C
MyStruct X_json_default_struct_MyStruct();
void     X_json_parse_MyStruct(const char* tokens[], size_t tokens_size, MyStruct* j);
void     X_json_print_MyStruct(MyStruct* j);
```


If you wish to define another struct that also needs to be parsed from json,
you can just repeat the previous steps:
```C
#define XNAME AnotherStruct
#define XFIELDS \
    XINT(another_int_field, "another_int_field") \
    XSTR(another_str_field, "another_str_field") \
#include "xjson.h"
```
Which in turn will give the struct:
```C
typedef struct {
    int         another_int_field;
    const char* another_str_field;
} AnotherStruct
```
and the functions:
```C
AnotherStruct X_json_default_struct_AnotherStruct();
void          X_json_parse_AnotherStruct(const char* tokens[], size_t tokens_size, AnotherStruct* j);
void          X_json_print_AnotherStruct(AnotherStruct* j);
```


Note that the '#include "xjson.h"' for the second struct,
does not include all the helper functions again.

See example.c for a more complete usage of the lib.
