struct index_file {
    page_t current_page;
};

struct file_entry {
    int search_key;
    int record_addr;
};
typedef struct file_entry file_entry_t;

#define INDEX_HEADER_SIZE = 16 


table_t create_index_file(char *filename, char *search_key_name, int key_data_type);
int populate_index_file(table_t idx_tbl, table_t data_tbl);