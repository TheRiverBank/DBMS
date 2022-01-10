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


table_t create_table(char *tbl_name, char *field_names[], int field_types[], int field_sizes[], int n_fields);
int insert_record(int values[], table_t tbl);
void print_records_in_page(table_t tbl, page_t page);