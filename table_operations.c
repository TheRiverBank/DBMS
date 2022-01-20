#include <stdio.h>
#include <stdlib.h>
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

void print_records_in_page(table_t tbl, page_t page) {
    int i, j, n_records;

    n_records = 64;
    page->current_pos = 0;
    for (i = 0; i < n_records; i++) {
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
    int i, fd, n_blocks;

    // Get number of bytes in file
    struct stat buf;
    stat(tbl->tbl_name, &buf);
    off_t size = buf.st_size;
    // Get number of written blocks
    n_blocks = size / BLOCK_SIZE;

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
