struct index_file {
    page_t current_page;
};

struct file_entry {
    int search_key;
    int record_addr;
};
typedef struct file_entry file_entry_t;