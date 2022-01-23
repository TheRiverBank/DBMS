#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pager.h"
#include "parser.h"

#define INT_TYPE 0
#define STR_TYPE 1

typedef struct field field_t;
struct field {
    char *name;
    field_t *next_field;
    int offset;
    int type;
};

struct table {
    char *tbl_name;
    field_t *first_field;
    page_t current_page;
    int n_fields;
    int rec_len;
};
typedef struct table *table_t; 

struct record {
    char *field_name;
    int value;
};
typedef struct record record_t;


table_t create_table(char *tbl_name, char *field_names[], int field_types[], int field_sizes[], int n_fields) {
    /* Creates a table by creating a set of field descriptors for 
     * the given fields. 
     */
    table_t tbl = malloc(sizeof(table_t));
    field_t *fld_head, *tmp_fld;
    tbl->tbl_name = tbl_name;
    
    int i;
    fld_head = (field_t *)malloc(sizeof(field_t));
    fld_head->name = field_names[0];
    fld_head->offset = field_sizes[0];
    fld_head->type = field_types[0];

    tmp_fld = fld_head;
    for (i = 1; i < n_fields; i++) {
        field_t *f = malloc(sizeof(field_t));
        f->name = field_names[i];
        f->offset = field_sizes[i];
        f->type = field_types[i];
        
        tmp_fld->next_field = f;
        tmp_fld = tmp_fld->next_field;
    }

    for (i = 0; i < n_fields; i++) {
        tbl->rec_len += field_sizes[i];
    }
    tbl->first_field = fld_head;
    tbl->n_fields = n_fields;
}

int insert_record(int values[], table_t tbl) {
    /* Inserts the given values into the table */
    char *filename = "db";
    int i;
    field_t *fld = tbl->first_field;
    page_t pg = tbl->current_page;
    for (i = 0; i < tbl->n_fields; i++) {
        if (fld->type == INT_TYPE) {
            int stat = page_put_int(values[i], pg);
            // If page is full, get next page
            if (stat == 0){
                // Write the page befre swapping it out
                write_page(filename, pg);
                int pg_n = pg->page_nr;
                pg_n++;
                pg = get_page(filename, pg_n);
                tbl->current_page = pg;
            }
        }
    }
}

int get_num_blocks(char *f_name) {
     int n_blocks;
    // Get number of bytes in file
    struct stat buf;
    stat(f_name, &buf);
    off_t size = buf.st_size;
    // Get number of written blocks
    n_blocks = size / BLOCK_SIZE;

    return n_blocks;
}

int offst_to_field(table_t tbl, char *fld_name) {
    int i, offset = 0;
    field_t *fld = tbl->first_field;

    for (i = 0; i < tbl->n_fields; i++) {
        if (strcmp(fld->name, fld_name) == 0) {
            return offset;
        }

        offset += fld->offset;
        fld = fld->next_field;
    }

    return -1;
}

int get_mid_page(int left, int right) {
  /* Convert to indecies */
  return left + (right - left) / 2;
}

int get_mid_record(int left, int right, int record_len) {
    int left_new = (left - HEADER_SIZE) / record_len;
    int right_new = (right - HEADER_SIZE) / record_len;

    return ((left_new + ((right_new - left_new) / 2)) * record_len) + HEADER_SIZE;
}

int search_table_binary(table_t tbl, char *fld_name, int value) {
     /* Binary search of table (only int)*/
    int i, j, n_blocks, n_records, cur_page_num;
    page_t cur_page;
    int left_page_idx, right_page_idx, cur_page_idx;
    int left_val, right_val, mid_val;
    int left_val_pos, right_val_pos;
    int offset = 0;

    n_blocks = get_num_blocks(tbl->tbl_name);
    
    left_page_idx = 0;
    right_page_idx = n_blocks;

    cur_page_idx = get_mid_page(left_page_idx, right_page_idx);
    cur_page = get_page(tbl->tbl_name, cur_page_idx);
    tbl->current_page = cur_page;

    while (left_page_idx <= right_page_idx) {
        left_val_pos = cur_page->current_pos;
        right_val_pos = cur_page->last_used_byte - tbl->rec_len;
        // Check min and max value in current page
        left_val = page_get_int_at(cur_page, left_val_pos + offset);
        right_val = page_get_int_at(cur_page, right_val_pos + offset);
        // Check if desired value is in this page
        if (value >= left_val && value <= right_val) {
            mid_val = get_mid_record(left_val_pos, right_val_pos, tbl->rec_len);
            while(left_val_pos <= right_val_pos) {
                if (value == mid_val) {
                    // Found the value, have to find first occurance
                    int start_pos, start_page;

                    page_set_current_pos(mid_val, cur_page);


                }
            }
        }

        // If val was not in current page, find next direction
        if (right_val < value) {
            left_page_idx = cur_page->page_nr + 1;
        } else {
            right_page_idx = cur_page->page_nr - 1;
        }

        page_set_pos_beg(cur_page);
        write_page(tbl->tbl_name, cur_page);

        cur_page_idx = get_mid_page(left_page_idx, right_page_idx);
        cur_page = get_page(tbl->tbl_name, cur_page_idx);
        page_set_pos_beg(cur_page);
        tbl->current_page = cur_page;
    }   
    
    return 0;
}

int search_table_linear(table_t tbl, char *fld_name, int value) {
    /* Linear search of table (only int)*/
    int i, j, n_blocks, cur_page_num;
    page_t pg;

    n_blocks = get_num_blocks(tbl->tbl_name);

    // Need to add a offset to the fld_name searched for in the record
    cur_page_num = 0;
    pg = get_page(tbl->tbl_name, cur_page_num);
    page_set_pos_beg(pg);
    tbl->current_page = pg;

    int val, offset;
    offset = offst_to_field(tbl, fld_name);
    i = 0;
    while (i < n_blocks) {
        while(pg->current_pos <= pg->last_used_byte) {
            val = page_get_int_at(pg, offset + pg->current_pos);
            if (val == value) {
                return 1;
            }
            pg->current_pos += tbl->rec_len;
        }
        i++;
        pg = get_page(tbl->tbl_name, i);
    }

    return 0;
}

void print_records_in_page(table_t tbl, page_t page) {
    int i, j;
    
    while(page->current_pos <= page->last_used_byte) {
        field_t *field_desc = tbl->first_field;
        for (j = 0; j < tbl->n_fields; j++) {
            int val = page_get_int(page);
        
            printf("%s: %d\t", field_desc->name, val);
            field_desc = field_desc->next_field;
        }
        printf("\n");
    }
}

void print_db(table_t tbl) {
    int i, fd, n_blocks, n_records;

    // Get number of bytes in file
    struct stat buf;
    stat(tbl->tbl_name, &buf);
    off_t size = buf.st_size;
    // Get number of written blocks
    n_blocks = get_num_blocks(tbl->tbl_name);

    page_t page = get_page(tbl->tbl_name, 0);
    page_set_pos_beg(page);
    tbl->current_page = page;
 
    i = 0;
    while (i < n_blocks) {
        print_records_in_page(tbl, page);
        int pg_n = page->page_nr;
        pg_n++;
        page = get_page("db", pg_n);
        page_set_pos_beg(page);
        tbl->current_page = page;
        i++;
    }
}

