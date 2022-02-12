#include <stdio.h>
#include "pager.h"
#include "test.h"
#include "table_operations.h"
#include "sequential_index_file.h"

int test_binary_search() {
    /* Write something to page 1 and close the file */
    char *filename = "db";
    int fd = open_file(filename);
 
    page_t p = get_page(filename, 0);
    page_set_pos_beg(p);
 
    char *field_names[3] = {"ID", "Age"};
    int field_types[2] = {0, 0};
    int field_sizes[2] = {4, 4};
    table_t tbl = create_table(filename, field_names, field_types, field_sizes, 2);
    tbl->current_page = p;
    
    
    int i;
    int n_records = 200;
    int search_val = 33;
    for (i = 0; i < n_records; i++) {
        int vals[2] = {i, 432};
        if (i == search_val) 
            insert_record(vals, tbl);
        insert_record(vals, tbl);
    } 
 
    write_page(filename, tbl->current_page);

    int found = table_search(tbl, "ID", search_val);
    if (found == 1) {
        printf("FOUND\n");
    }
}


int test_index_file() {
    /* Write something to page 1 and close the file */
    char *filename = "index_test_table";
    int fd = open_file(filename);
 
    page_t p = get_page(filename, 0);
    page_set_pos_beg(p);
 
    char *field_names[3] = {"ID", "number", "age"};
    int field_types[3] = {0, 0, 0};
    int field_sizes[3] = {4, 4, 4};
    table_t tbl = create_table(filename, field_names, field_types, field_sizes, 3);
    tbl->current_page = p;
    
    int i;
    int n_records = 20;

    for (i = 0; i < n_records; i++) {
        int vals[3] = {i, 432, i * 2};
        insert_record(vals, tbl);
    } 
    printf("Records inserted\n");
    write_page(filename, tbl->current_page);
    printf("Creating index table\n");
    table_t idx_table = create_index_file("index_table", "ID", 0);
}

