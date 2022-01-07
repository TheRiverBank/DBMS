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

int insert_record() {
    const char *filename = "db";
    open_file(filename);

    page_t p = get_page(filename, 0);
    page_set_current_pos(filename, 12, p);
    page_put_int(filename, 12, p);
    int val = page_get_int(filename, p);


    page_t p_2 = get_page(filename, 2);
    page_set_current_pos(filename, 12, p_2);
    page_put_int(filename, 123, p_2);
    int val_2 = page_get_int(filename, p_2);


    page_set_current_pos(filename, 12, p);
    val = page_get_int(filename, p);

    printf("page 1 val: %d, page 2 val: %d\n", val, val_2);
}

int main() {
    char *query = "select employee from employees";
    parse(query);
    open_file("db");

    insert_record();
}