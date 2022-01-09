struct page {
    char *data;
    int page_nr;
};
typedef struct page *page_t;

int open_file(const char *filename);
int close_file(int fd);
page_t get_page(const char *filename, int blk_num);
int read_page(const char *filename, page_t page);
int write_page(const char *filename, page_t page);


int page_set_current_pos(int pos, page_t page);
int page_put_int(int val, page_t page);
int page_get_int(page_t page);