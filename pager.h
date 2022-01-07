struct page {
    char *data;
    int page_nr;
};
typedef struct page *page_t;

void open_file(const char *filename);
page_t get_page(const char *filename, int blk_num);
int read_page(const char *filename, page_t page);
int write_page(const char *filename, page_t page);


int page_set_current_pos(const char *filename, int pos, page_t page);
int page_put_int(const char *filename, int val, page_t page);
int page_get_int(const char *filename, page_t page);