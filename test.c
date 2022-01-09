#include <stdio.h>
#include "pager.h"

int test_write() {
    /* Write something to page 1 and close the file */
    const char *filename = "db";
    
    printf("Opening file\n");
    int fd = open_file(filename);
    printf("File open.\nCreating page\n");
    page_t p = get_page(filename, 0);
    page_t p1 = get_page(filename, 1);
    printf("Page created\nSetting pos\n");
    page_set_current_pos(12, p);
    page_set_current_pos(12, p1);
    printf("Pos set\nPlacing int\n");
    page_put_int(69, p);
    page_put_int(432, p1);
    int val = page_get_int(p);
    int val2 = page_get_int(p1);
    printf("page 0 val: %d page 1 val: %d\n", val, val2);
    write_page(filename, p);
    close_file(fd);
}

int test_read() {
    const char *filename = "db";
    int fd = open_file(filename);

    page_t p1;
    read_page(filename, p1);

    page_set_current_pos(12, p1);


    int val1 = page_get_int(p1);
  

    printf("page 0 val: %d\n", val1);  

}