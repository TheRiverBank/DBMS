typedef struct field field_t;
struct field {
    char *name;
    field_t *next_field;
    int size;
    int type;
    int offset;
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

table_t create_table(char *tbl_name, char *field_names[], int field_types[], int field_sizes[], int n_fields);
int insert_record(int values[], table_t tbl);
void print_records_in_page(table_t tbl, page_t page);
void print_db(table_t tbl);
int offst_to_field(table_t tbl, char *fld_name);

int table_search(table_t tbl, char *fld_name, int value);
int search_table_linear(table_t tbl, char *fld_name, int value, table_t res_tbl);
int search_table_binary(table_t tbl, char *fld_name, int value, table_t res_tbl);