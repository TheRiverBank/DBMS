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
    /* Write something to page 1 and close the file */
    const char *filename = "db";
    
    printf("Opening file\n");
    int fd = open_file(filename);
    printf("File open.\nCreating page\n");
    page_t p = get_page(filename, 0);
    printf("Page created\nSetting pos\n");
    page_set_current_pos(filename, 12, p);
    printf("Pos set\nPlacing int\n");
    page_put_int(filename, 69, p);
    int val = page_get_int(filename, p);

    printf("page 0 val: %d\n", val);
    write_page(filename, p);
    close_file(fd);
}

int retreive_record() {
    const char *filename = "db";
    int fd = open_file(filename);

    page_t p1;
    read_page(filename, p1);

    page_set_current_pos(filename, 12, p1);


    int val1 = page_get_int(filename, p1);
  

    printf("page 0 val: %d\n", val1);  

}

int main() {
    char *query = "select employee from employees";
    parse(query);
    open_file("db");

    insert_record();
    retreive_record();
}