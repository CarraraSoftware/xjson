#define XJSON_IMPLEMENTATION
#include "xjson.h"

typedef struct {
    int* items;
    size_t len;
    size_t cap;
} IntVec;

#define XNAME Person
#define XFIELDS                        \
    XINT(age,     "age")               \
    XSTR(name,    "name")              \
    XARR(IntVec,  numbers, "numbers")  \
    XARR(IntVec,  ids,     "ids")  
#include "xgen.h"

int main(void)
{
    StringBuilder person_buf = sb_from_file("person.json");
    Person person = X_json_default_struct_Person();
    X_json_parse_Person(&person, &person_buf);
    X_json_print_Person(&person);
    X_json_free_Person(&person);

    return 0;
}
