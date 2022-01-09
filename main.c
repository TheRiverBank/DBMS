#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "pager.h"

struct int_item {
    char *name;
    int value;
    size_t size;
};

struct str_item {
    char *name;
    char *value;
    size_t size;
};

typedef struct int_item int_item_t;
typedef struct str_item str_item_t;



int main() {
    char *query = "select employee from employees";
    parse(query);
    open_file("db");

}