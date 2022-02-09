/* Sequential index file.
 * Solution to keeping a sorted index file:
 * Be idle on insertions and when a search occurs,
 * check if there has been any insertions since last sort.
 * If there has been insertions, merge sort entire file. 
 * Insertions in the database file should be followed by an 
 * insertion in the index file. If there is no space for the
 * record, add it to an overflow block. 
 * 
 * Index file creation should probably be postponed until 
 * there is a sufficiently large number of blocks to 
 * justify the index file.
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

int populate_index_file(table_t idx_tbl, table_t data_tbl) {
    /* Inserts all key values from data table into index table.
     * The search key field name in the given index table must correspond
     * to the desired search key field name in the data table.
     * The data table must be sorted before this function is called
     */
    int i, offst, cur_blk_idx_data, cur_blk_idx_index;
    int n_blocks;
    page_t cur_data_pg, cur_idx_pg;
    
    cur_blk_idx_data = 0;
    cur_blk_idx_index = 0;

    cur_data_pg = get_page(data_tbl->tbl_name, cur_blk_idx_data);
    cur_idx_pg = get_page(idx_tbl->tbl_name, cur_blk_idx_index);

    page_set_pos_beg(cur_data_pg);
    page_set_pos_beg(cur_idx_pg);

    n_blocks = get_num_blocks(data_tbl->tbl_name);
    // Get offset to the key value in the data table.
    offst = offst_to_field(data_tbl, idx_tbl->first_field->name);

    for (i = 0; i < n_blocks; i++) {
        while(cur_data_pg->current_pos <= cur_data_pg->last_used_byte) {
            // Get the value at search key pos
            int val = page_get_int_at(cur_data_pg, offst + cur_data_pg->current_pos);
            // Add the search key value to the index table
            page_put_int(val, idx_tbl);
            // Add the block number where this value is found
            page_put_int(i, idx_tbl);

            if(cur_idx_pg->current_pos + INT_SIZE > BLOCK_SIZE) {
                write_page(idx_tbl->tbl_name, cur_blk_idx_index);
                cur_blk_idx_index += 1;
                cur_idx_pg = get_page(idx_tbl->tbl_name, cur_blk_idx_index);
            }
        }
        cur_blk_idx_data += 1;
        cur_data_pg = get_page(data_tbl->tbl_name, cur_blk_idx_data);
        // ASSUME THAT DATA TABLE WITH SWITCH BLOCK BEFORE INDEX TABLE  
    }

}


int set_entry(page_t page, int value, int addr) {
    page_put_int(value, page);
    page_put_int(addr, page);
}


