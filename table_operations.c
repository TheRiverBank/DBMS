#include <stdio.h>
#include <stdlib.h>
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
typedef struct table table_t;

struct record {
    char *field_name;
    int value;
};
typedef struct record record_t;


table_t create_table(char *tbl_name, char *field_names[], int field_types[], int field_sizes[], int n_fields) {
    table_t *tbl = malloc(sizeof(table_t));
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
}

int insert_record(char *values[], table_t *tbl) {
    int i;
    field_t *fld = tbl->first_field;
    page_t pg = tbl->current_page;
    for (i = 0; i < tbl->n_fields; i++) {
        if (fld->type == INT_TYPE) {
            // IF page has space
            page_put_int((int) *((int *)&values[i]), pg);

            // ELSE get page with space
        }
    }
}