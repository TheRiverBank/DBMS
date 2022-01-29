#include <stdio.h>
#include "pager.h"
#include "test.h"
#include "table_operations.h"

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
    for (i = 0; i < n_records; i++) {
        int vals[2] = {i, 432};
        insert_record(vals, tbl);
    } 
 
    write_page(filename, tbl->current_page);
    //print_db(tbl);

    int found = table_search(tbl, "ID", 33);
    if (found == 1) {
        printf("FOUND\n");
    }
}


