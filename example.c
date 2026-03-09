#define XNAME Person
#define XFIELDS       \
   XINT(age,  "age")  \
   XSTR(name, "name") \
   XINT(size, "size") \
   XSTR(addr, "addr") 
#include "xjson.h"

#define XNAME Position
#define XFIELDS \
    XSTR(name, "name") \
    XSTR(type, "type")
#include "xjson.h"

#define XNAME Company
#define XFIELDS \
    XSTR(name,    "name") \
    XSTR(address, "address") \
    XINT(CEP,     "zip_code")
#include "xjson.h"

static const char* input[] = {
    "name", "Carrara", "age",  "3001", "size", "20", "addr", "Rua Das PreProcessadoras", 
};

static const char* input2[] = {
    "name", "Engineer", "type", "Senior ", 
};

static const char* input3[] = {
    "name", "CarraraSoftware", 
    "type", "LTDA", 
    "address", "Rua Da Minha Casa, Nº meu número",
    "zip_code", "023302402",
};


int main(void)
{
    Person person = X_json_default_struct_Person();
    X_json_parse_Person(input, sizeof(input) / sizeof(input[0]), &person);
    X_json_print_Person(&person);

    Position pos = X_json_default_struct_Position();
    X_json_parse_Position(input2, sizeof(input2) / sizeof(input2[0]), &pos);
    X_json_print_Position(&pos);

    Company c = X_json_default_struct_Company();
    X_json_parse_Company(input3, sizeof(input3) / sizeof(input3[0]), &c);
    X_json_print_Company(&c);

    return 0;
}
