#include <stdio.h>
#include "pager.h"
#include "test.h"
#include "table_operations.h"


    
int test_write() {
    /* Write something to page 1 and close the file */
    char *filename = "db";
    int fd = open_file(filename);
    printf("File open.\nCreating page\n");
    page_t p = get_page(filename, 0);
    page_set_pos_beg(p);
    printf("Page created, making table\n");
    char *field_names[2] = {"ID", "Age"};
    int field_types[2] = {0, 0};
    int field_sizes[2] = {4, 4};
    table_t tbl = create_table("test", field_names, field_types, field_sizes, 2);
    tbl->current_page = p;
    
    printf("Table created\nInserting record\n");
    int i;
    int n_records = 200;
    for (i = 0; i < n_records; i++) {
        int vals[2] = {i, 432};
        insert_record(vals, tbl);
    } 
    printf("Record inserted\nPrinting record\n");
    //print_records_in_page(tbl, tbl->current_page);

    //print_records_in_page(tbl, tbl->current_page);
    write_page(filename, tbl->current_page);
    print_db(tbl);
    //print_records_in_page(tbl, tbl->current_page);

    //print_db(tbl);
}
