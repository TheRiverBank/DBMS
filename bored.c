#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

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


int_item_t create_item_nm(char *name, int value) {
    int_item_t item;
    item.name = name;
    item.value = value;

    return item;
}

int_item_t *create_item_m(char *name, int value) {
    int_item_t *item = (int_item_t *)malloc(sizeof(int_item_t));
    item->name = name;
    item->value = value;

    return item;
}

int main() {
    char *query = "select employee from employees";
    parse(query);
}