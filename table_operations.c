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
    int size;
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
    fld_head->size = field_sizes[0];
    fld_head->type = field_types[0];

    tmp_fld = fld_head;
    for (i = 1; i < n_fields; i++) {
        field_t *f = malloc(sizeof(field_t));
        f->name = field_names[i];
        f->size = field_sizes[i];
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

int offst_to_field(table_t tbl, char *fld_name) {
    int i, offset = 0;
    field_t *fld = tbl->first_field;

    for (i = 0; i < tbl->n_fields; i++) {
        if (strcmp(fld->name, fld_name) == 0) {
            return offset;
        }

        offset += fld->size;
        fld = fld->next_field;
    }

    return -1;
}

void add_record(table_t tbl, table_t res_tbl) {
    int i;
    // Assume tbl->current_page is positioned at the begining of a record
    // which is to be copied to res_tbl;
    for (i = 0; i < tbl->n_fields; i++) {
        // Plave each value from the original table in the result table.
        page_put_int(page_get_int(tbl->current_page), res_tbl->current_page);
    }
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

void get_start_pos_and_page(page_t cur_page, table_t tbl, int cur_page_idx, int val, int offset, int* start_pos, int* start_page) {
  /* Finds the position and page of the first occurance of val */
  int s_pos, s_page, end = 0;
  /* Iterate backwards */
  while (cur_page_idx >= 0) {
    int pos = page_current_pos(cur_page);
    while (pos >= HEADER_SIZE) {
      /* If current pos is not the val, last occurance found */
      if (page_get_int_at(cur_page, pos + offset) != val) {
        /* If pos is at end of page go to begining of next page */
        if (pos == page_last_written_byte(cur_page) - tbl->rec_len) {
          s_pos = HEADER_SIZE;
          s_page = cur_page_idx + 1;
        }
        /* If not, go one record forward and stay on page */
        else {
          s_pos = pos + tbl->rec_len;
          s_page = cur_page_idx;
        }
        end = 1;
        break;
      }
      else 
        pos -= tbl->rec_len;
    }

    if (end) 
      break;

    /* Go backwards one page */
    cur_page_idx -= 1;
    tbl->current_page = get_page(tbl->tbl_name , cur_page_idx);
    cur_page = tbl->current_page;
    /* Set pos at end and continue iterating records backwards */
    page_set_current_pos(page_last_written_byte(cur_page) - tbl->rec_len, cur_page);
  }

  /* If value was found at first pos in first page, cur_page_idx is -1. Set to start */
  if (cur_page_idx < 0) {
    s_page = 0;
    s_pos = HEADER_SIZE;
  }

  /* Initialize found pos and page */
  tbl->current_page = get_page(tbl->tbl_name, s_page);
  page_set_current_pos(s_pos, cur_page);
  /* Return found pos and page */
  *start_pos = s_pos;
  *start_page = s_page;
}

int get_records(int start_page, int n_blocks, page_t cur_page, table_t tbl, int val, int offset, table_t res_tbl) {
  /* Gets start pos and page from get_start_pos_and_page(), 
   * then iterates forward until all records with val are found */
  while (start_page <= n_blocks) {
    int pos = page_current_pos(cur_page);
    while(pos <= page_last_written_byte(cur_page) - tbl->rec_len) {
      if (page_get_int_at(cur_page, pos + offset) == val) {
        /* Append record to schema */
        page_set_current_pos(pos, cur_page);
        add_record(tbl, res_tbl);
        /* Go to next record */
        pos += tbl->rec_len;
      } 
      /* Current pos does not have val so all records have been found, return. */
      else return 0;
    }
    /* Go to next page. Reset current page first */
    page_set_pos_beg(cur_page);
    /* Unpin page possibly writing it */
    write_page(tbl->tbl_name, tbl->current_page);
  
    /* Go to next page */
    start_page += 1;
    tbl->current_page = get_page(tbl->tbl_name , start_page);
    cur_page = tbl->current_page;
    
    page_set_pos_beg(tbl->current_page);
  }

  return 0;
}


int search_table_binary(table_t tbl, char *fld_name, int value, table_t res_tbl) {
     /* Binary search of table (only int)*/
    int i, j, n_blocks, n_records, cur_page_num;
    page_t cur_page;
    int left_page_idx, right_page_idx, cur_page_idx;
    int left_val, right_val, rec_val;
    int left_val_pos, right_val_pos, mid_val_pos;
    int offset = 0;

    n_blocks = get_num_blocks(tbl->tbl_name);
    
    left_page_idx = 0;
    right_page_idx = n_blocks;

    cur_page_idx = get_mid_page(left_page_idx, right_page_idx);
    cur_page = get_page(tbl->tbl_name, cur_page_idx);
    tbl->current_page = cur_page;

    while (right_page_idx >= left_page_idx) {
        left_val_pos = cur_page->current_pos;
        right_val_pos = cur_page->last_used_byte - tbl->rec_len;
        // Check min and max value in current page
        left_val = page_get_int_at(cur_page, left_val_pos + offset);
        right_val = page_get_int_at(cur_page, right_val_pos + offset);
        // Check if desired value is in this page
        printf("Checking: %d %d\n", left_val, right_val);
        if (value >= left_val && value <= right_val) {
            mid_val_pos = get_mid_record(left_val_pos, right_val_pos, tbl->rec_len);
            printf("Found page with mid val: %d %d\n", right_val_pos, left_val_pos);
            while(right_val_pos >= left_val_pos) {
                rec_val = page_get_int_at(cur_page, mid_val_pos + offset);
                printf("Testing: %d\n", rec_val);
                if (value == rec_val) {
                    int start_pos, start_page;
                    printf("FOUND - binary\n");
                    tbl->current_page->current_pos = mid_val_pos;
                    get_start_pos_and_page(cur_page, tbl, cur_page_idx, value, offset, &start_pos, &start_page);
                    
                    tbl->current_page = get_page(tbl->tbl_name, start_page);
                    cur_page = tbl->current_page;
                    page_set_current_pos(start_pos, cur_page);

                    get_records(start_page, n_blocks, cur_page, tbl, value, offset, res_tbl);

                    
                    return 0;
                }

               else {
                /* Pager increments pos when calling page_get_int_at, dont want that */
                page_set_current_pos(mid_val_pos, cur_page);
                }

                /* Get next middle in page */
                if (rec_val < value) 
                    left_val_pos = mid_val_pos + tbl->rec_len;
                else 
                    right_val_pos = mid_val_pos - tbl->rec_len;

                mid_val_pos = get_mid_record(left_val_pos, right_val_pos, tbl->rec_len);
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

int search_table_linear(table_t tbl, char *fld_name, int value, table_t res_tbl) {
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

table_t copy_table(table_t tbl) {
    int i;
    char *field_names[tbl->n_fields];
    int field_types[tbl->n_fields];
    int field_sizes[tbl->n_fields];

    field_t *fld = tbl->first_field;
   
    for (i = 0; i < tbl->n_fields; i++) {
        field_names[i] = fld->name;
        field_types[i] = fld->type;
        field_sizes[i] = fld->size;

        fld = fld->next_field;
    }


    table_t tbl_cpy = create_table("search_res", field_names, field_types, 
                                                                 field_sizes, tbl->n_fields);
                                                                 
    return tbl_cpy;
}

void print_db(table_t tbl);
int table_search(table_t tbl, char *fld_name, int value) {
    // Copy table structure
    table_t res_tbl = copy_table(tbl);
   
    // Create a page for the result
    page_t tmp_page = get_page(res_tbl->tbl_name, 0);
    res_tbl->current_page = tmp_page;
    // Add tuples in which tuple(value) = value
    
    int found = search_table_binary(tbl, fld_name, value, res_tbl);
    write_page(res_tbl->tbl_name, tmp_page);
    print_db(res_tbl);
    return found;
    // Later.. include only those tuples specified in the select clause
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

