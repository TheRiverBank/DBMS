/* B+-tree index file.
 * Solution to keeping a sorted index file:
 * Be idle on insertions and when a search occurs,
 * check if there has been any insertions since last sort.
 * If so, merge sort entire file. 
 * Insertions in the database file should be followed by an 
 * insertion in the index file. If there is no space for the
 * record, add it to an overflow block. 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "pager.h"
#include "table_operations.h"
#include "sequential_index_file.h"

table_t create_index_file(char *filename, char *search_key_name, int key_data_type) {
    /* Open or create the index file by borrowing function from pager
     * Creates a table for the index file with two fields, search key and address in block.
     * A page is initially set up for the table and is set as the current page.
     * Returns the created table-
     */
    int fd = open_file(filename);
    table_t idx_table = create_table(filename, &search_key_name, (int *)1, (int*)INT_SIZE, 1);
    page_t page = get_page(filename, 0);

    field_t *key_fld = (field_t *)malloc(sizeof(field_t));
    field_t *addr_fld = (field_t *)malloc(sizeof(field_t));

    key_fld->name = search_key_name;
    key_fld->offset = 0;
    key_fld->type = INT_SIZE;

    addr_fld->name = "address";
    addr_fld->next_field = NULL;
    addr_fld->offset = INT_SIZE;
    
    key_fld->next_field = addr_fld;

    idx_table->current_page = page;
    idx_table->first_field = key_fld;
    idx_table->n_fields = 2;
    idx_table->tbl_name = filename;
    
    return idx_table;
}


int set_entry(page_t page, int value, int addr) {
    page_put_int(value, page);
    page_put_int(addr, page);
}
