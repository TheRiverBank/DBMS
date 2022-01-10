#include <stdio.h>
#include "pager.h"
#include "test.h"
#include "table_operations.h"


    
int test_write() {
    /* Write something to page 1 and close the file */
    const char *filename = "db";
    int fd = open_file(filename);
    printf("File open.\nCreating page\n");
    page_t p = get_page(filename, 0);
    printf("Page created, making table\n");
    char *tbl_names[2] = {"ID", "Age"};
    int field_types[2] = {0, 0};
    int field_sizes[2] = {4, 4};
    table_t tbl = create_table("test", tbl_names, field_types, field_sizes, 2);
    tbl->current_page = p;
    
    printf("Table created\nInserting record\n");
    int vals[2] = {0, 42};
    insert_record(vals, tbl);
    printf("Record inserted\nPrinting record\n");
    //print_records_in_page(tbl, tbl->current_page);


    printf("Table created\nInserting record\n");
    int vals2[2] = {55, 35};
    insert_record(vals2, tbl);
    printf("Record inserted\nPrinting record\n");
    print_records_in_page(tbl, tbl->current_page);

    write_page(filename, tbl->current_page);
    close_file(fd);

    fd = open_file(filename);
    p = get_page(filename, 0);
    tbl->current_page = p;
    print_records_in_page(tbl, tbl->current_page);
}
